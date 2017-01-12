// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAppleEventsMgr.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility functions for dealing with AppleEvents

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>
#include <UModalDialogs.h>
#include <PP_Resources.h>

#include <AEInteraction.h>
#include <AEObjects.h>
#include <AEPackObject.h>
#include <AERegistry.h>
#include <Processes.h>
#include <TextUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

AEAddressDesc	UAppleEventsMgr::sSelfTargetAddr;
bool			UAppleEventsMgr::sDontExecuteWorks;
AEDesc			UAppleEventsMgr::sAnyType;

// ---------------------------------------------------------------------------
//	Constants and Types

										// Return codes for Toolbox string
										// comparison functions
const SInt16		strings_Equal		= 0;
const SInt16		strings_NotEqual	= 1;

const SInt32	Version_AE101 = 0x01018000;	// AppleEvents version number


struct SAETableRec {					// Format of an 'aedt' entry
	OSType	eventClass;
	OSType	eventID;
	long	refCon;
};


// ---------------------------------------------------------------------------
//	¥ Initialize
// ---------------------------------------------------------------------------
//	Initialize the AppleEvents Unit

void
UAppleEventsMgr::Initialize()
{
	sSelfTargetAddr.descriptorType	= typeNull;
	sSelfTargetAddr.dataHandle		= nil;

	sAnyType.descriptorType			= typeNull;
	sAnyType.dataHandle				= nil;

	MakeSelfTargetAddr(sSelfTargetAddr);

		// The kAEDontExecute flag to AESend is recognized in
		// version 1.01 or later of the AppleEvent Magager.
		// In version 1.0, this flag is ignored, meaning that
		// you can't send a record-only AppleEvent.
		//
		// In fact, the AEManagerInfo call is only implemented in
		// version 1.01 or later. Under version 1.0, this call
		// returns paramErr (-50). Therefore, we assume we don't
		// have version 1.01 if the call fails.

	SInt32	aeVersion;
	OSErr	err = ::AEManagerInfo(keyAEVersion, &aeVersion);

	sDontExecuteWorks = ((err == noErr) && (aeVersion >= Version_AE101));

	//	Build wild card (any) type list
	err = ::AECreateList(nil, 0, false, &sAnyType);
	ThrowIfOSErr_(err);
	DescType	wildType = typeWildCard;
	UAEDesc::AddPtr(&sAnyType, 0, typeType, &wildType, sizeof(wildType));
}


// ---------------------------------------------------------------------------
//	¥ InstallAEHandlers
// ---------------------------------------------------------------------------
//	Install an AEHandler function for all AppleEvents listed in
//	a table stored in Resources of type ResType_AETable

void
UAppleEventsMgr::InstallAEHandlers(
	AEEventHandlerUPP		inHandler)
{
	SInt16	numTables = ::CountResources(ResType_AETable);

	for (SInt16 i = 1; i <= numTables; i++) {
		Handle tableH = ::GetIndResource(ResType_AETable, i);

		SInt16	numItems = (SInt16) (::GetHandleSize(tableH) / sizeof(SAETableRec));
		::HLockHi(tableH);
		SAETableRec* tableP = reinterpret_cast<SAETableRec*>(*tableH);

		for (SInt16 j = 1; j <= numItems; j++, tableP++) {
			OSErr err = ::AEInstallEventHandler(tableP->eventClass,
									tableP->eventID, inHandler,
									tableP->refCon, false);
			ThrowIfOSErr_(err);
		}

		::ReleaseResource(tableH);
	}
}


// ---------------------------------------------------------------------------
//	¥ CheckForMissedParams
// ---------------------------------------------------------------------------
//	Throw an exception if we missed a parameter in an AppleEvent

void
UAppleEventsMgr::CheckForMissedParams(
	const AppleEvent&	inAppleEvent)
{
		// We just want to check if there are any leftover parameters and
		// don't care about their contents. Therefore, pass nil for the
		// buffer and specify a maximum size of zero.

	DescType	theType;
	Size		theSize;
	OSErr		err = ::AEGetAttributePtr(&inAppleEvent, keyMissedKeywordAttr,
								typeWildCard, &theType, nil, 0, &theSize);

		// A return code of "descriptor not found" is good. It means that
		// no missing parameters were found. Any other return code is bad.

	if (err == noErr) {				// No Error means that the MissedKeyword
									// attribute exists and we missed a param
		ThrowOSErr_(errAEParamMissed);

	} else if (err != errAEDescNotFound) {
		ThrowOSErr_(err);			// Some other AE Manager error
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeAppleEvent
// ---------------------------------------------------------------------------
//	Make an AppleEvent to send to ourself
//
//		Creates an AppleEvent with standard parameters

void
UAppleEventsMgr::MakeAppleEvent(
	AEEventClass	inEventClass,
	AEEventID		inEventID,
	AppleEvent&		outAppleEvent)
{
	OSErr	err = ::AECreateAppleEvent(inEventClass, inEventID,
										&sSelfTargetAddr,
										kAutoGenerateReturnID,
										kAnyTransactionID,
										&outAppleEvent);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ SendAppleEvent
// ---------------------------------------------------------------------------
//	Send an AppleEvent with standard send options and don't ask for a reply
//
//		If inExecute is false, the AppleEvent is sent (so script engines
//		can record the event) but not executed (the application does not
//		receive the event).
//
//		This function disposes of the input AppleEvent descriptor.

void
UAppleEventsMgr::SendAppleEvent(
	AppleEvent&		inAppleEvent,
	Boolean			inExecute)
{
	AESendMode	theSendMode = kAENoReply;
	if (!inExecute) {
		if (!sDontExecuteWorks) {	// If kAEDontExecute is not supported,
									// just return. The program does not
									// expect to receive this event.
			::AEDisposeDesc(&inAppleEvent);
			return;
		}

		theSendMode += kAEDontExecute;
	}

	StAEDescriptor	theAEReply;

	OSErr	err = ::AESend(&inAppleEvent, theAEReply, theSendMode,
						kAENormalPriority, kAEDefaultTimeout, nil, nil);
	::AEDisposeDesc(&inAppleEvent);

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ SendAppleEventWithReply
// ---------------------------------------------------------------------------
//	Send an AppleEvent with standard send options and ask for a reply
//
//		If inRecord is false, the AppleEvent is sent and executed but not
//		recorded.
//
//		This function DOES NOT dispose of the input AppleEvent descriptor.

void
UAppleEventsMgr::SendAppleEventWithReply(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	Boolean				inRecord)
{
	AESendMode		mode = kAEWaitReply;

	if (!inRecord) {
		mode |= kAEDontRecord;
	}

	OSErr	err = ::AESend(&inAppleEvent, &outAEReply, mode, kAENormalPriority,
						kAEDefaultTimeout, nil, nil);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ MakeSelfTargetAddr
// ---------------------------------------------------------------------------
//	Make an address descriptor for the current process (this application)
//	which can be used to send AppleEvents to ourself

void
UAppleEventsMgr::MakeSelfTargetAddr(
	AEAddressDesc&	outSelfTargetAddr)
{
	ProcessSerialNumber	currProcess;

	currProcess.highLongOfPSN = 0;
	currProcess.lowLongOfPSN = kCurrentProcess;

	OSErr	err = ::AECreateDesc(typeProcessSerialNumber, (Ptr) &currProcess,
							sizeof(currProcess), &outSelfTargetAddr);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ CompareDescriptors
// ---------------------------------------------------------------------------
//	Return the result of comparing the data from two Descriptors

bool
UAppleEventsMgr::CompareDescriptors(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
	bool	result;

	switch (inComparisonOperator) {

		case kAEGreaterThan:		// Magnitude comparison
		case kAEGreaterThanEquals:
		case kAELessThan:
		case kAELessThanEquals:
			result = CompareAsNumbers(inLeftDesc, inComparisonOperator,
										inRightDesc);
			break;

		case kAEBeginsWith:			// String matching
		case kAEEndsWith:
		case kAEContains:
			result = CompareAsStrings(inLeftDesc, inComparisonOperator,
										inRightDesc);
			break;

		case kAEEquals:				// Compare based on data type of left
									//   descriptor
			switch (inLeftDesc.descriptorType) {

				case typeSInt16:		// Left descriptor is a number
				case typeSInt32:
				case typeSInt64:
				case typeUInt32:
				case typeIEEE32BitFloatingPoint:
				case typeIEEE64BitFloatingPoint:
				case typeExtended:
				case typeLongDateTime:
					result = CompareAsNumbers(inLeftDesc,
										inComparisonOperator, inRightDesc);
					break;

				case typeBoolean:		// Left descriptor is a Boolean
				case typeTrue:
				case typeFalse: {
					Boolean	leftBoolean;
					Boolean	rightBoolean;
					UExtractFromAEDesc::TheBoolean(inLeftDesc, leftBoolean);
					UExtractFromAEDesc::TheBoolean(inRightDesc, rightBoolean);
					result = (leftBoolean == rightBoolean);
					break;
				}

				default:				// String compare for all other types
					result = CompareAsStrings(inLeftDesc,
										inComparisonOperator, inRightDesc);
					break;
			}
			break;

		default:					// Unknown comparison operator
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ CompareAsNumbers
// ---------------------------------------------------------------------------
//	Return the result of comparing two Descriptors of numerical values

bool
UAppleEventsMgr::CompareAsNumbers(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
		// As in C++, we compare using the largest type of operand,
		// promoting the smaller type if necessary. The type order is:
		//
		//		Floating Point  (64-bit, 32-bit, or extended)
		//		Date  (LongDateTime is a 64-bit integer)
		//		Integer  (64-bit, 32-bit, or 16-bit)

	bool	result = false;
	
	if ( (inLeftDesc.descriptorType == typeIEEE32BitFloatingPoint)  ||
		 (inLeftDesc.descriptorType == typeIEEE64BitFloatingPoint)  ||
		 (inLeftDesc.descriptorType == typeExtended)  ||
		 (inRightDesc.descriptorType == typeIEEE32BitFloatingPoint)  ||
		 (inRightDesc.descriptorType == typeIEEE64BitFloatingPoint)  ||
		 (inRightDesc.descriptorType == typeExtended) ) {
		 
		result = CompareAsFloats(inLeftDesc, inComparisonOperator, inRightDesc);
		
	} else if ( (inLeftDesc.descriptorType == typeLongDateTime)  ||
				(inRightDesc.descriptorType == typeLongDateTime) ) {
		 
		result = CompareAsDates(inLeftDesc, inComparisonOperator, inRightDesc);
		
	} else {
	
		result = CompareAsIntegers(inLeftDesc, inComparisonOperator, inRightDesc);
	}
	
	return result;
}


// ---------------------------------------------------------------------------
//	¥ CompareAsFloats
// ---------------------------------------------------------------------------
//	Return the result of comparing two Descriptors, extracting the data
//	as 64-bit floating point numbers

bool
UAppleEventsMgr::CompareAsFloats(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
	bool	result = false;

	double	leftNumber;
	double	rightNumber;
	UExtractFromAEDesc::TheData( inLeftDesc, typeIEEE64BitFloatingPoint, &leftNumber, sizeof(double) );
	UExtractFromAEDesc::TheData( inRightDesc, typeIEEE64BitFloatingPoint, &rightNumber, sizeof(double) );

	switch (inComparisonOperator) {

		case kAEGreaterThan:
			result = (leftNumber > rightNumber);
			break;

		case kAEGreaterThanEquals:
			result = (leftNumber >= rightNumber);
			break;

		case kAEEquals:
			result = (leftNumber == rightNumber);
			break;

		case kAELessThan:
			result = (leftNumber < rightNumber);
			break;

		case kAELessThanEquals:
			result = (leftNumber <= rightNumber);
			break;
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ CompareAsDates
// ---------------------------------------------------------------------------
//	Return the result of comparing two Descriptors, extracting the data
//	as LongDateTime values

bool
UAppleEventsMgr::CompareAsDates(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
	bool	result = false;

	LongDateTime	leftNumber;
	LongDateTime	rightNumber;
	UExtractFromAEDesc::TheData( inLeftDesc, typeLongDateTime, &leftNumber, sizeof(LongDateTime) );
	UExtractFromAEDesc::TheData( inRightDesc, typeLongDateTime, &rightNumber, sizeof(LongDateTime) );

	switch (inComparisonOperator) {

		case kAEGreaterThan:
			result = (leftNumber > rightNumber);
			break;

		case kAEGreaterThanEquals:
			result = (leftNumber >= rightNumber);
			break;

		case kAEEquals:
			result = (leftNumber == rightNumber);
			break;

		case kAELessThan:
			result = (leftNumber < rightNumber);
			break;

		case kAELessThanEquals:
			result = (leftNumber <= rightNumber);
			break;
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ CompareAsIntegers
// ---------------------------------------------------------------------------
//	Return the result of comparing two Descriptors, extracting the data
//	as 64-bit integers

bool
UAppleEventsMgr::CompareAsIntegers(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
	bool	result = false;

	SInt64	leftNumber;
	SInt64	rightNumber;
	UExtractFromAEDesc::TheData( inLeftDesc, typeSInt64, &leftNumber, sizeof(SInt64) );
	UExtractFromAEDesc::TheData( inRightDesc, typeSInt64, &rightNumber, sizeof(SInt64) );

	switch (inComparisonOperator) {

		case kAEGreaterThan:
			result = (leftNumber > rightNumber);
			break;

		case kAEGreaterThanEquals:
			result = (leftNumber >= rightNumber);
			break;

		case kAEEquals:
			result = (leftNumber == rightNumber);
			break;

		case kAELessThan:
			result = (leftNumber < rightNumber);
			break;

		case kAELessThanEquals:
			result = (leftNumber <= rightNumber);
			break;
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ CompareAsStrings
// ---------------------------------------------------------------------------
//	Return the result of comparing two Descriptors, extracting the data
//	as strings

bool
UAppleEventsMgr::CompareAsStrings(
	const AEDesc&	inLeftDesc,
	DescType		inComparisonOperator,
	const AEDesc&	inRightDesc)
{
	bool	result = false;

	Str255	baseString;
	Str255	subString;
	UExtractFromAEDesc::ThePString(inLeftDesc, baseString, sizeof(baseString));
	UExtractFromAEDesc::ThePString(inRightDesc, subString, sizeof(subString));

									// No need to compare if subString
									//   is longer than baseSTring
	if (StrLength(baseString) >= StrLength(subString)) {

		switch (inComparisonOperator) {

			case kAEBeginsWith:
				result = ::IdenticalText(
							baseString + 1, subString + 1,
							StrLength(subString), StrLength(subString),
							nil) == strings_Equal;
				break;

			case kAEEndsWith:
				result = ::IdenticalText(
							baseString + StrLength(baseString) + 1
							- StrLength(subString), subString + 1,
							StrLength(subString), StrLength(subString),
							nil) == strings_Equal;
				break;

			case kAEContains: {		// Compare all substrings of inBaseString
									//   to inSubString
				SInt16	subCount = (SInt16) (StrLength(baseString) -
											 StrLength(subString) + 1);
				for (SInt16 i = 1; i <= subCount; i++) {
					result = ::IdenticalText(
								baseString + i, subString + 1,
								StrLength(subString), StrLength(subString),
								nil) == strings_Equal;
					if (result) {
						break;
					}
				}
				break;
			}

			case kAEEquals:
				result = ::IdenticalString(baseString, subString, nil)
							== strings_Equal;
				break;
		}
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ GetAbsolutePositionIndex
// ---------------------------------------------------------------------------
//	Return the index number of an item within a list of inItemCount items
//	based on a key data descriptor.
//
//	Returns a negative index for the special case of "all" items

SInt32
UAppleEventsMgr::GetAbsolutePositionIndex(
	const AEDesc&	inKeyData,
	SInt32			inItemCount)
{
	SInt32		index	 = 0;
	DescType	position = typeNull;

	if (inKeyData.descriptorType == typeAbsoluteOrdinal) {

			// For typeAbsolutionOrdinal, the key data Handle is a
			// DescType indicating the position. Valid values are:
			// kAEFirst, kAELast, kAEMiddle, kAEAny, kAEAll

		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			ThrowIfOSErr_( ::AEGetDescData(&inKeyData, &position, sizeof(position)));
		#else
			position = **(DescType**) inKeyData.dataHandle;
		#endif
		
	} else {
			// For other key types, we coerce the data to an index
		UExtractFromAEDesc::TheSInt32(inKeyData, index);
	}

	switch (position) {

		case kAEFirst:
			index = 1;
			break;

		case kAEMiddle:
			index = (inItemCount + 1) / 2;
			break;

		case kAELast:
			index = inItemCount;
			break;

		case kAEAny:					// Pick a random item number
			index = 0;
			if (inItemCount > 0) {
				index = ((UInt16) ::Random() % (UInt16) inItemCount) + 1;
			}
			break;

		case kAEAll:
			index = -1;
			break;

		default:
			if (index < 0) {		// Negative position counts back from
									//   end, with -1 being the last item
				index += inItemCount + 1;
				
				if (index <= 0) {
					ThrowOSErr_(errAENoSuchObject);
				}
			}
			break;
	}

	return index;
}


// ---------------------------------------------------------------------------
//	¥ InteractWithUser
// ---------------------------------------------------------------------------
//	Call before posting a dialog box (or other request for user interaction)
//	when handling an AppleEvent.
//
//	Returns an error code, which will be
//		noErr, errAETimeout, or errAENoUserInteraction
//
//	You may get errAENoUserInteraction if you call ::AESetUserInteraction()
//	with a parameter that restricts interaction to certain circumstances.
//
//	However, if inThrowFail is true, function throws an execption if there
//	is no interaction. In this case, if the function returns, it always
//	returns noErr.

OSErr
UAppleEventsMgr::InteractWithUser(
	bool		inThrowFail,
	SInt32		inTimeoutTicks,
	NMRecPtr	inNotification)
{
	AEIdleUPP	idleUPP = NewAEIdleUPP(AEIdleProc);

	OSErr	err = ::AEInteractWithUser(inTimeoutTicks, inNotification, idleUPP);

	DisposeAEIdleUPP(idleUPP);

	if ( (err != noErr)  &&  inThrowFail ) {
		ThrowOSErr_(err);
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ AEIdleProc
// ---------------------------------------------------------------------------
//	Toolbox callback function for handling events while waiting for user
//	interaction to a received AppleEvent or for a reply from a sent
//	AppleEvent

pascal Boolean
UAppleEventsMgr::AEIdleProc(
	EventRecord*	inMacEvent,
	SInt32*			outSleepTime,
	RgnHandle*		outMouseRgn)
{
	*outSleepTime = 6;
	*outMouseRgn = nil;

	try {
		UModalAlerts::ProcessModalEvent(*inMacEvent);
	}

	catch (...) { }			// Can't throw back through the Toolbox

	return false;
}


#pragma mark -

// ===========================================================================
//	StAEDescriptor Class
// ===========================================================================
//
//	StAEDescriptor class encapsulates an Apple Event Descriptor (AEDesc).
//
//	The Constructor creates an empty AEDesc. GetParamDesc and
//	GetOptionalParamDesc load information from an Apple Event into
//	the AEDesc. The Destructor disposes of the AEDesc.
//
//	The public member mDesc is an AEDesc record which you can pass
//	to AppleEvent routines or other functions.
//
//	Use StAEDescriptors as stack-based objects to ensure that Descriptors
//	are properly disposed, even if an exception occurs. For example,
//
//		StAEDescriptors	theDesc;
//		theDesc.GetParamDesc(myAppleEvent, myKeyword, typeQDRectangle);
//		UExtractFromAEDesc::TheRect(theDesc, myRect);
//
//	This class has constructors for creating Descriptors for the following
//	common data types:
//		Boolean, Short Integer, Long Interger, OSType, Pascal String

// ---------------------------------------------------------------------------
//	¥ StAEDescriptor						Default Constructor
// ---------------------------------------------------------------------------

StAEDescriptor::StAEDescriptor()
{
	InitToNull();
}


StAEDescriptor::StAEDescriptor(				// Copy Constructor
	const StAEDescriptor&	inOriginal)
{
	InitToNull();
											// Copy input's AEDesc
	OSErr	err = ::AEDuplicateDesc(inOriginal, &mDesc);
	ThrowIfOSErr_(err);
}


//	Warning:
//
//	This constructor will NOT fail if the indicated parameter does
//	not exist.  Instead, the StAEDescriptor will be initialized to typeNull.
//	This way, the caller can explicitly check to see if the indicated
//	parameter was non-existent (typeNull), or just fail when trying to
//	get data out of the typeNull descriptor.

StAEDescriptor::StAEDescriptor(
	const AEDesc&	inDesc,
	AEKeyword		inKeyword,
	DescType		inDesiredType)
{
	mDesc.descriptorType = typeNull;
	mDesc.dataHandle     = nil;

	OSErr	err;

	switch (inDesc.descriptorType) {

		case typeAERecord:
		case typeAppleEvent: {
			err = ::AEGetParamDesc(&inDesc, inKeyword, inDesiredType, &mDesc);
			//	Don't throw.
			//	The typeNull value assigned above is sufficient.
			break;
		}

		case typeNull:
			break;

		default: {
			StAEDescriptor	temp;

			err = ::AECoerceDesc(&inDesc, typeAERecord, &temp.mDesc);
			ThrowIfOSErr_(err);

			err = ::AEGetParamDesc(&temp.mDesc, inKeyword, inDesiredType, &mDesc);

			//	Don't throw if the descriptor doesn't exist.
			//	The typeNull value assigned above is sufficient.
			if (err != errAEDescNotFound)
				ThrowIfOSErr_(err);
			break;
		}
	}
}


StAEDescriptor::StAEDescriptor(
	DescType	inType,
	const void	*inData,
	SInt32		inSize)
{
	InitToNull();

	OSErr	err = ::AECreateDesc(inType, inData, inSize, &mDesc);
	ThrowIfOSErr_(err);
}


StAEDescriptor::StAEDescriptor(
	Boolean		inBoolean)
{
	InitToNull();
	Assign(inBoolean);
}


StAEDescriptor::StAEDescriptor(
	SInt16		inInt16)
{
	InitToNull();
	Assign(inInt16);
}


StAEDescriptor::StAEDescriptor(
	SInt32		inInt32)
{
	InitToNull();
	Assign(inInt32);
}


StAEDescriptor::StAEDescriptor(
	OSType		inOSType)
{
	InitToNull();
	Assign(inOSType);
}


StAEDescriptor::StAEDescriptor(
	ConstStringPtr	inString)
{
	InitToNull();
	Assign(inString);
}


StAEDescriptor::StAEDescriptor(
	const FSSpec&	inFileSpec)
{
	InitToNull();
	Assign(inFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ ~StAEDescriptor						Destructor				  [public]
// ---------------------------------------------------------------------------

StAEDescriptor::~StAEDescriptor()
{
	if (mDesc.dataHandle != nil) {
		::AEDisposeDesc(&mDesc);
	}
}


StAEDescriptor&
StAEDescriptor::Assign(
	const StAEDescriptor& inOriginal)
{
	if (this != &inOriginal) {
		Dispose();
											// Copy input's AEDesc
		OSErr	err = ::AEDuplicateDesc(inOriginal, &mDesc);
		ThrowIfOSErr_(err);
	}

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	Boolean		inBoolean)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeBoolean, &inBoolean, sizeof(inBoolean), &mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	SInt16		inInt16)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeShortInteger, &inInt16, sizeof(inInt16),
									&mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	SInt32		inInt32)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeLongInteger, &inInt32, sizeof(inInt32),
									&mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	OSType		inOSType)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeType, &inOSType, sizeof(inOSType), &mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	ConstStringPtr	inString)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeChar, inString + 1, inString[0], &mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


StAEDescriptor&
StAEDescriptor::Assign(
	const FSSpec&	inFileSpec)
{
	Dispose();

	OSErr	err = ::AECreateDesc(typeFSS, &inFileSpec, sizeof(FSSpec), &mDesc);
	ThrowIfOSErr_(err);

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Dispose
// ---------------------------------------------------------------------------
//	Dispose AEDesc and reset it to a null descriptor

void
StAEDescriptor::Dispose()
{
	if (mDesc.dataHandle != nil) {
		::AEDisposeDesc(&mDesc);
		InitToNull();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetParamDesc
// ---------------------------------------------------------------------------
//	Load Descriptor data from an AppleEvent
//
//	Throws an OSErr exception if it can't get the data

void
StAEDescriptor::GetParamDesc(
	const AppleEvent&	inAppleEvent,
	AEKeyword			inKeyword,
	DescType			inDesiredType)
{
	Dispose();

	OSErr	err = ::AEGetParamDesc(&inAppleEvent, inKeyword, inDesiredType,
									&mDesc);
	ThrowIfOSErr_(err);
}

// ---------------------------------------------------------------------------
//	¥ GetOptionalParamDesc
// ---------------------------------------------------------------------------
//	Load optional Descriptor data from an AppleEvent
//
//	Differs from GetParamDesc in that it does not throw an exception
//	if the request fails because the specified keyword data does not
//	exist. Use this function to extract optional parameters from
//	an AppleEvent.

void
StAEDescriptor::GetOptionalParamDesc(
	const AppleEvent&	inAppleEvent,
	AEKeyword			inKeyword,
	DescType			inDesiredType)
{
	Dispose();

	OSErr	err = ::AEGetParamDesc(&inAppleEvent, inKeyword, inDesiredType,
									&mDesc);
	if ((err != errAEDescNotFound) && (err != noErr)) {
		ThrowOSErr_(err);
	}
}

#pragma mark -

// ===========================================================================
//	UAEDesc Class
// ===========================================================================
//
//	UAEDesc is a collection of static functions helpful in dealing with
//	AELists and AERecords.
//
//	UAEDesc functions will throw exceptions as errors are encountered.

// ---------------------------------------------------------------------------
//	¥ AddKeyDesc
// ---------------------------------------------------------------------------
//	Adds the descriptor to the record with the given keyword.
//
//	Will make the ioRecord an AERecord if it is typeNull.

void
UAEDesc::AddKeyDesc(
	AERecord		*ioRecord,
	AEKeyword		inKeyword,
	const AEDesc	&inDesc	)
{
	OSErr	err = noErr;

	if (ioRecord->descriptorType == typeNull) {
		err = ::AECreateList(NULL, 0, true, ioRecord);
		ThrowIfOSErr_(err);
	}

	err = ::AEPutKeyDesc(ioRecord, inKeyword, &inDesc);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ AddDesc
// ---------------------------------------------------------------------------
//	Adds the descriptor to the ioDesc at inIndex.
//
//	If ioDesc is:
//
//		typeNull, inDesc will be copied to ioDesc
//
//		not typeNull and not a list, ioDesc will be converted to a list with
//		the first item the previous value of ioDesc and the inDesc appended
//		to that list
//
//		a list, inDesc will be added to ioDesc at inIndex

void
UAEDesc::AddDesc(
	AEDesc			*ioDesc,		//	io descriptor or descriptor list
	long			inIndex,		//	new/replace sub descriptor position
									//		(0 for append/don't care)
	const AEDesc	&inDesc)		//	new/replace sub descripter
{
	OSErr	err = noErr;

	if (ioDesc->descriptorType == typeNull) {
		err = ::AEDuplicateDesc(&inDesc, ioDesc);
		ThrowIfOSErr_(err);
		return;
	}

									// Must end up being some sort of list
									//   make descriptor a list
	if (ioDesc->descriptorType != typeAEList) {
		StAEDescriptor	tempDesc;

		err = ::AEDuplicateDesc(ioDesc, &tempDesc.mDesc);
		ThrowIfOSErr_(err);

		err = ::AEDisposeDesc(ioDesc);
		ThrowIfOSErr_(err);

		err = ::AECreateList(NULL, 0, false, ioDesc);
		ThrowIfOSErr_(err);

		err = ::AEPutDesc(ioDesc, 0, &tempDesc.mDesc);
		ThrowIfOSErr_(err);
	}

									// Add data to the list
	err = ::AEPutDesc(ioDesc, inIndex, &inDesc);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ AddPtr
// ---------------------------------------------------------------------------
//	Adds an equivalent descriptor to the ioDesc at inIndex using AddDesc.

void
UAEDesc::AddPtr(
	AEDesc			*ioDesc,		//	io descriptor or descriptor list
	long			inIndex,		//	new/replace sub descriptor position
									//		(0 for append/don't care)
	DescType		inDescType,		//	new/replace sub descripter type
	const void		*inDataPtr,		//	pointer to date for new sub descriptor
	Size			inDataSize)		//	size of data in inDataPtr
{
	StAEDescriptor	desc(inDescType, inDataPtr, inDataSize);
	AddDesc(ioDesc, inIndex, desc);
}


// ---------------------------------------------------------------------------
//	¥ MakeRange
// ---------------------------------------------------------------------------
//	Makes a formRange object specifier with the container object specifier
//	being inSuperSpecifier and boundary sub objects of type inKind specified
//	by the inIndex1 and inIndex2 absolute position indices.
//
//	If inIndex1 == inIndex2, a simple formAbsolutePosition object specifier
//	is constructed.

void
UAEDesc::MakeRange(
	const AEDesc	&inSuperSpecifier,
	DescType		inKind,
	SInt32			inIndex1,
	SInt32			inIndex2,
	AEDesc			*outDesc)
{
	StAEDescriptor	defContainer,
					desc1,
					desc2,
					ospec1,
					ospec2,
					rangeDesc;
	AEDesc			*container;
	Boolean			doRange = false;
	OSErr			err;

	if (inIndex2 > inIndex1) {
		doRange = true;
	}

	if (doRange) {
		err = ::AECreateDesc(typeCurrentContainer, NULL, 0, &defContainer.mDesc);
		ThrowIfOSErr_(err);

		container = &defContainer.mDesc;
	} else {
		container = (AEDesc *)&inSuperSpecifier;
	}

	//	index1
	Assert_(inIndex1 > 0);	//	0 is a bad number in the AEOM.
	err = ::CreateOffsetDescriptor(inIndex1, &desc1.mDesc);
	ThrowIfOSErr_(err);
	if (doRange) {
		err = ::CreateObjSpecifier(inKind, container,
				formAbsolutePosition, &desc1.mDesc, false, &ospec1.mDesc);
	} else {
		err = ::CreateObjSpecifier(inKind, container,
				formAbsolutePosition, &desc1.mDesc, false, outDesc);
	}
	ThrowIfOSErr_(err);

	if (doRange) {
		//	index2
		err = CreateOffsetDescriptor(inIndex2, &desc2.mDesc);
		ThrowIfOSErr_(err);
		err = CreateObjSpecifier(inKind, container,
				formAbsolutePosition, &desc2.mDesc, false, &ospec2.mDesc);
		ThrowIfOSErr_(err);

		//	range
		err = ::CreateRangeDescriptor(&ospec1.mDesc, &ospec2.mDesc,
				false, &rangeDesc.mDesc);
		ThrowIfOSErr_(err);
		Assert_(outDesc);
		err = ::CreateObjSpecifier(inKind, (AEDesc *) &inSuperSpecifier,
				formRange, &rangeDesc.mDesc, false, outDesc);
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeInsertionLoc
// ---------------------------------------------------------------------------
//	Makes a typeInsertionLoc AEDesc with:
//
//		a kAEObject of inObjSpec, and
//		a kAEPosition of inPosition.

void
UAEDesc::MakeInsertionLoc(
	const AEDesc	&inObjSpec,
	DescType		inPosition,
	AEDesc			*outDesc)
{
	StAEDescriptor	desc;
	OSErr			err;

	err = ::AECreateList(NULL, 0, true, &desc.mDesc);
	ThrowIfOSErr_(err);

	err = ::AEPutParamDesc(&desc.mDesc, keyAEObject, &inObjSpec);
	ThrowIfOSErr_(err);

	err = ::AEPutParamPtr(&desc.mDesc, keyAEPosition, typeEnumerated,
				&inPosition, sizeof(inPosition));
	ThrowIfOSErr_(err);

	Assert_(outDesc);
	::AEDisposeDesc(outDesc);

	err = ::AECoerceDesc(&desc.mDesc, typeInsertionLoc, outDesc);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ MakeBooleanDesc
// ---------------------------------------------------------------------------
//	Makes a typeBoolean AEDesc:

void
UAEDesc::MakeBooleanDesc(
	Boolean	inValue,
	AEDesc	*outDesc)
{
	SInt8	value = (SInt8) inValue;	//	This might be required as Frontmost 1.0 apparently
										//	ONLY looks at the most significant byte of boolean
										//	descriptor.

	ThrowIfOSErr_( ::AECreateDesc(typeBoolean, (Ptr) &value, sizeof(value), outDesc) );
}

PP_End_Namespace_PowerPlant
