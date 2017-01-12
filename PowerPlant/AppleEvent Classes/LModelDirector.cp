// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LModelDirector.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for AppleEvent handlers and the AE Object Support Library
//
//	You should only create one object of this class. This object handles
//	all callbacks from the AppleEvent Manager. It either handles the callback
//	itself, or calls a member function for the LModelObject that is the
//	target of an AppleEvent.
//
//	The callback functions are static functions that call a virtual member
//	function of LModelDirector. You can override these functions in a
//	subclass of LModelDirector to change how AppleEvent callbacks are
//	handled.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LModelDirector.h>
#include <LModelObject.h>
#include <LModelProperty.h>
#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>

#include <AERegistry.h>
#include <AEObjects.h>
#include <AEPackObject.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LModelDirector*		LModelDirector::sModelDirector = nil;
AppleEvent*			LModelDirector::sCurrentAEReply = nil;


// ---------------------------------------------------------------------------
//	¥ LModelDirector						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LModelDirector::LModelDirector()
{
	sModelDirector	= this;
	mResolveFlags	= kAEIDoMinimum;

	try {
		UAppleEventsMgr::Initialize();
	}

	catch (...) { }

	::AEObjectInit();

	LModelObject::InitLazyList();
}


// ---------------------------------------------------------------------------
//	¥ LModelDirector						Constructor				  [public]
// ---------------------------------------------------------------------------

LModelDirector::LModelDirector(
	LModelObject*	inDefaultModel)
{
	sModelDirector	= this;
	mResolveFlags	= kAEIDoMinimum;

	LModelObject::SetDefaultModel(inDefaultModel);

	try {
		UAppleEventsMgr::Initialize();
		::AEObjectInit();
		InstallCallBacks();
	}

	catch (...) { }

	LModelObject::InitLazyList();
}


// ---------------------------------------------------------------------------
//	¥ ~LModelDirector						Destructor				  [public]
// ---------------------------------------------------------------------------

LModelDirector::~LModelDirector()
{
	LModelObject::DestroyLazyList();
}


// ---------------------------------------------------------------------------
//	¥ InstallCallBacks												  [public]
// ---------------------------------------------------------------------------
//	Install handler and callback functions used by the AppleEvent Manager
//		and the Object Support Library
//	Call this function only once (usually at the beginning of the program)

void
LModelDirector::InstallCallBacks()
{
	OSErr	err;

		// Generic handler for all AppleEvents

	UAppleEventsMgr::InstallAEHandlers(
			NewAEEventHandlerUPP(&LModelDirector::AppleEventHandler));

		// Specific handler for CountElements AppleEvent

	err = ::AEInstallEventHandler(kAECoreSuite, kAECountElements,
			NewAEEventHandlerUPP(&LModelDirector::CountElementsEventHandler),
			ae_CountElements, false);
	ThrowIfOSErr_(err);

		// Specific handler for CreateElement AppleEvent

	err = ::AEInstallEventHandler(kAECoreSuite, kAECreateElement,
			NewAEEventHandlerUPP(&LModelDirector::CreateElementEventHandler),
			ae_CreateElement, false);
	ThrowIfOSErr_(err);

		// Specific handler for DoObjectsExist AppleEvent

	err = ::AEInstallEventHandler(kAECoreSuite, kAEDoObjectsExist,
			NewAEEventHandlerUPP(&LModelDirector::DoObjectsExistEventHandler),
			ae_DoObjectsExist, false);
	ThrowIfOSErr_(err);

		// Generic accessor for Model Objects

	err = ::AEInstallObjectAccessor(typeWildCard, typeWildCard,
				NewOSLAccessorUPP(&LModelDirector::ModelObjectAccessor),
				0, false);
	ThrowIfOSErr_(err);

		// Accessor for List of Model Objects

	err = ::AEInstallObjectAccessor(typeWildCard, typeAEList,
				NewOSLAccessorUPP(&LModelDirector::ModelObjectListAccessor),
				0, false);
	ThrowIfOSErr_(err);

	err = ::AESetObjectCallbacks(
				NewOSLCompareUPP(&LModelDirector::OSLCompareObjects),
				NewOSLCountUPP(&LModelDirector::OSLCountObjects),
				NewOSLDisposeTokenUPP(&LModelDirector::OSLDisposeToken),
				nil,		// GetMarkToken
				nil,		// Mark
				nil,		// AdjustMarks
				nil);		// GetErrDesc
	ThrowIfOSErr_(err);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------
//	Respond to an AppleEvent
//
//	This generic handler directs the AppleEvent to the ModelObject
//	specified by the keyDirectObject parameter. If the keyDirectObject
//	parmater does not exist or is not an object specifier, the default
//	model handles the AppleEvent.
//
//	You must install a separate handler for AppleEvents where the
//	keyDirectObject parameter has a different meaning.

void
LModelDirector::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outReply,
	AEHandlerRefConT	inRefCon)
{
	OSErr				err = noErr;

	StAEDescriptor		theResult;
	StAEDescriptor		directObj;
	StAEDescriptor		directObjToken;

									// Get direct object parameter
	directObj.GetOptionalParamDesc(inAppleEvent, keyDirectObject,
										typeWildCard);

	if (directObj.IsNotNull()) {	// Direct object exists. Try to resolve
									//   it to a Token. It's OK for the
									//   resolve to fail, so we turn off
									//   throw debugging to prevent annoying
									//   breaks.
		StDisableDebugThrow_();
		err = Resolve(directObj, directObjToken);
	}

	if ( (directObj.IsNull()) ||
		 (err == errAENotAnObjSpec) ||
		 (err == errAENoSuchObject) ) {
									// Direct object parameter not present
									//   or is not an object specifier.
									//   Let the default model handle it.
		(LModelObject::GetDefaultModel())->
			HandleAppleEventAll(inAppleEvent, outReply, theResult, (SInt32) inRefCon);

	} else if (err == noErr) {
									// Process this event using the Token
									//   representing the direct direct
		ProcessTokenizedEvent(inAppleEvent, directObjToken,
								theResult, outReply, inRefCon);
	} else {
		Throw_(err);
	}
									// Put result code in Reply
	if ( (theResult.IsNotNull()) &&
		 (outReply.descriptorType != typeNull) ) {

		err = ::AEPutParamDesc(&outReply, keyAEResult, theResult);
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleCountElementsEvent										  [public]
// ---------------------------------------------------------------------------
//	Respond to a CountElements AppleEvent

void
LModelDirector::HandleCountElementsEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outReply,
	AEHandlerRefConT	/* inRefCon */)
{

		// Count event specifies:
		//		keyDirectObject  - container object whose elements to count
		//		keyAEObjectClass - class of thing to count
		//
		// AppleScript syntax is:
		//		count [each | every] className (in | of) container
		//		number of classNames (in | of) container

	OSErr				err = noErr;

									// Get container from direct object
	StAEDescriptor		directObj;
	directObj.GetParamDesc(inAppleEvent, keyDirectObject, typeWildCard);

	StAEDescriptor		directObjToken;
	if (directObj.IsNotNull()) {
		err = Resolve(directObj, directObjToken);
		ThrowIfOSErr_(err);
	}

									// Get class of thing to count
	StAEDescriptor	classDesc(inAppleEvent, keyAEObjectClass, typeType);
	DescType		classToCount;
	UExtractFromAEDesc::TheType(classDesc, classToCount);

	SInt32	itemCount = 0;			// Number of items of classToCount

	if (directObjToken.DescriptorType() == typeAEList) {
									// Container is a list. Count the number
									//   of items of class classToCount that
									//   are in this list.
		SInt32			containerSize = 0;
		::AECountItems(directObjToken, &containerSize);

		if (classToCount == cObject) {
									// Special case. AppleScript uses cObject to
									//   mean any item. For example,
									//		number of items in every window
									//   Everything in the list is an item, so the
									//   count is just the size of the list.
			itemCount = containerSize;

		} else {
									// We need to check each item in the list
			for (SInt32 i = 1; i <= containerSize; i++) {
				StAEDescriptor		itemDesc;
				DescType			theKeyword;

										// Get descriptor for this item
				err = ::AEGetNthDesc(directObjToken, i, typeWildCard, &theKeyword, itemDesc);
				ThrowIfOSErr_(err);

					// Item could belong to a built-in AE class (such as
					// an integer) or it could be an actual PP model object

				DescType	itemType = itemDesc.DescriptorType();

				if (itemType == type_ModelToken) {
										// Item is an actual PP Model object.
										//   Item type is the kind of model object.
					LModelObject*	item =  LModelObject::GetModelFromToken(itemDesc);
					itemType = item->GetModelKind();
				}

				if (itemType == classToCount) {
					itemCount += 1;		// Matching type. Count it.
				}
			}
		}

	} else {						// Container is a single object. Ask
									//   object to count the number of
									//   submodels of class classType.
		LModelObject*	container = LModelObject::GetModelFromToken(directObjToken);

		if (container == nil) {			// Can't count without a container
			Throw_(errAEEventNotHandled);
		}

		itemCount = container->CountSubModels(classToCount);
	}

									// Put result code in Reply
	if (outReply.descriptorType != typeNull) {

		err = ::AEPutParamPtr(&outReply, keyAEResult, typeLongInteger,
								&itemCount, sizeof(SInt32));
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleCreateElementEvent										  [public]
// ---------------------------------------------------------------------------
//	Respond to a CreateElement AppleEvent
//
//	CreateElement requires a special handler because it does not have
//	a keyDirectObject parameter that specifies a target ModelObject which
//	should respond to the event. The target ModelObject for this event
//	is the SuperModel for the element to create.

void
LModelDirector::HandleCreateElementEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outReply,
	AEHandlerRefConT	/* inRefCon */)
{
								// Get class of element to create
	StAEDescriptor	classD(inAppleEvent, keyAEObjectClass, typeType);
	DescType		elemClass;
	UExtractFromAEDesc::TheType(classD, elemClass);

								// Find out where to insert new element
	StAEDescriptor	insertD(inAppleEvent, keyAEInsertHere);
	DescType		insertPosition = typeNull;

	LModelObject	*target		= nil;
	LModelObject	*container	= nil;
	LModelObject	*element	= nil;
	OSErr			err			= noErr;

	switch (insertD.DescriptorType()) {

		case typeInsertionLoc: {	// Insert location is a position relative
									//   to some reference object

									// Get position (such as "at beggining",
									//	"at end', "before", "after", etc.)
			StAEDescriptor	positionD(insertD, keyAEPosition, typeEnumeration);
			UExtractFromAEDesc::TheEnum(positionD, insertPosition);

			StAEDescriptor	tokenD;	// Token for reference object. Initialized
									//   to typeNull, which is the default
									//   model.

									// Find out if insert location has a
									//   reference object
			StAEDescriptor	objectD(insertD, keyAEObject);

				// If reference object is typeNull, we leave the Token
				// as typeNull. This means that if the insert location
				// doesn't specify a reference we assume the reference
				// object is the default model (the application object).

			if (objectD.IsNotNull()) {
									// Reference object exists. Resolve it
									//   to a token.
				StAEDescriptor	refObjectD(insertD, keyAEObject,
											typeObjectSpecifier);
				err = Resolve(refObjectD, tokenD);
				ThrowIfOSErr_(err);
			}

									// Find PP reference object and its
									//   container
			target	  = LModelObject::GetModelFromToken(tokenD);
			target	  = target->GetInsertionTarget(insertPosition);
			container = target->GetInsertionContainer(insertPosition);
			break;
		}

		case typeNull: {

			// Insert location is unspecified. We assume that this means
			// the beginning of the default model (the application). This
			// handles typical scripts such as:
			//
			//		make new window
			//
			// where it's likely that the user wants the new window
			// to become the front window.

			insertPosition	= kAEBeginning;
			break;
		}

		case typeObjectSpecifier: {

			// The insertion location is an object. We interpret this
			// to mean that the new element is inserted at the end of
			// the object. For example, we consider the statement:
			//
			//			make new button of window 1
			//
			//	to be the same as:
			//
			//			make new button at end of window 1
			//
			// This differs from the typeNull case above, where we assume
			// inserting at the beginning. You normally add new subobjects
			// at the end.

			StAEDescriptor	tokenD;
			err = Resolve(insertD, tokenD);
			ThrowIfOSErr_(err);

			insertPosition	= kAEEnd;
			target			= LModelObject::GetModelFromToken(tokenD);
			target			= target->GetInsertionTarget(insertPosition);
			container		= target->GetInsertionContainer(insertPosition);
			break;
		}

		default: {
			SignalStringLiteral_("Unknown insertion location type");
			break;
		}
	}

	if (container == nil) {			// Nil container is the default model,
									//   which is usually the application
		container = LModelObject::GetDefaultModel();
	}

	ThrowIfNil_(container);

	element = container->HandleCreateElementEventAll(
				elemClass, insertPosition, target, inAppleEvent, outReply);

		// The AppleEvents Registry specifies that an object specifier
		// for the new element be returned as the keyAEResult parameter
		// in the Reply to the AppleEvent

	if ( (element != nil)  &&  (outReply.descriptorType != typeNull) ) {
		StAEDescriptor	elementDesc;
		element->MakeSpecifier(elementDesc);
		UAEDesc::AddKeyDesc(&outReply, keyAEResult, elementDesc);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleDoObjectsExistEvent										  [public]
// ---------------------------------------------------------------------------
//	Respond to an "exists" AppleEvent
//
//	Pass back whether or not the direct object of the AppleEvent is
//	a valid object within the program

void
LModelDirector::HandleDoObjectsExistEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outReply,
	AEHandlerRefConT	/* inRefCon */)
{
	OSErr				err    = noErr;
	Boolean				exists = false;

		// Simple Strategy: Direct object exists if we can successfully
		// resolve it to a token. We don't care what the token is, just
		// whether the resolution succeeds or fails.

	StAEDescriptor		directObj;		// Get direct object parameter
	directObj.GetParamDesc(inAppleEvent, keyDirectObject, typeWildCard);

	StAEDescriptor		directObjToken;

	{									// Failure to resolve is OK, so
										//   we temorarily disable throw
		StDisableDebugThrow_();			//   debugging

										// Try to resolve direct object
		err = Resolve(directObj, directObjToken);
	}

	if (err == noErr) {					// Resolve succeeded

		exists = true;					// Object exists unless the token
										//   is an empty list
		if (directObjToken.DescriptorType() == typeAEList) {
			SInt32	numItems = 0;
			::AECountItems(directObjToken, &numItems);
			exists = (numItems > 0);
		}

	} else if (err != errAENoSuchObject) {
		Throw_(err);					// If Resolve failed for a reason
										//   other than "no such object",
	}									//   then we need to throw

										// Put result code in Reply
	if (outReply.descriptorType != typeNull) {

		err = ::AEPutParamPtr(&outReply, keyAEResult, typeBoolean,
								&exists, sizeof(Boolean));
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ ProcessTokenizedEvent											  [public]
// ---------------------------------------------------------------------------

void
LModelDirector::ProcessTokenizedEvent(
	const AppleEvent&	inAppleEvent,
	AEDesc&				inDirectObjToken,
	AEDesc&				outResult,
	AppleEvent&			outReply,
	AEHandlerRefConT	inRefCon)
{
	LModelObject*	theResponder;
	OSErr			err = noErr;

	if (inDirectObjToken.descriptorType != typeAEList) {

		theResponder = LModelObject::GetModelFromToken(inDirectObjToken);
		theResponder->HandleAppleEventAll(inAppleEvent, outReply,
											outResult, (SInt32) inRefCon);

	} else {						// Direct object is a list of ModelObjects

		StAEDescriptor	insertD;
		insertD.GetOptionalParamDesc(inAppleEvent, keyAEInsertHere, typeWildCard);

		SInt32			count = 0;
		::AECountItems(&inDirectObjToken, &count);

									// Start off with result as an empty list
		err = ::AECreateList(nil, 0, false, &outResult);
		ThrowIfOSErr_(err);

		if (insertD.IsNull()) {		// No insert location

			for (SInt32 i = 1; i <= count; i++) {
				StAEDescriptor		token;
				StAEDescriptor		resultDesc;
				DescType			theKeyword;

				err = ::AEGetNthDesc(&inDirectObjToken, i, typeWildCard, &theKeyword, token);
				ThrowIfOSErr_(err);

				if (token.DescriptorType() == typeAEList) {
									// Token is a nested list, so we need to
									//   recursively process it
					ProcessTokenizedEvent(inAppleEvent, token, resultDesc, outReply, inRefCon);

				} else {			// Token for one object

					theResponder = LModelObject::GetModelFromToken(token);
					theResponder->HandleAppleEventAll(inAppleEvent, outReply, resultDesc, (SInt32) inRefCon);
				}

				UAEDesc::AddDesc(&outResult, 0, resultDesc);
			}

		} else {

				// There is an insert location. Processing the event
				// for a token may change the responder, so we need to
				// adjust the insertion location as we go.
				//
				// For example, consider adding 2 items, called A and B,
				// at the beginning of some object. We can add item A at
				// the beginning. After we do, we want to add item B
				// after item A, not at the beginning. If we didn't change
				// the insertion location, the items would be in the
				// order BA instead of AB.

			StAEDescriptor	event;
			err = ::AEDuplicateDesc(&inAppleEvent, event);
			ThrowIfOSErr_(err);

			for (SInt32 i = 1; i <= count; i++) {
				StAEDescriptor		token;
				StAEDescriptor		resultDesc;
				DescType			theKeyword;

				err = ::AEGetNthDesc(&inDirectObjToken, i, typeWildCard, &theKeyword, token);
				ThrowIfOSErr_(err);

				if (token.DescriptorType() == typeAEList) {
									// Token is a nested list, so we need to
									//   recursively process it
					ProcessTokenizedEvent(event, token, resultDesc, outReply, inRefCon);

				} else {

					theResponder = LModelObject::GetModelFromToken(token);

					if (i != 1) {
						StAEDescriptor	hereD;
						UAEDesc::MakeInsertionLoc(insertD, kAEAfter, hereD);
						err = ::AEPutKeyDesc(event, keyAEInsertHere, hereD);
						ThrowIfOSErr_(err);
					}

					theResponder->HandleAppleEventAll(event, outReply, resultDesc, (SInt32) inRefCon);
				}


				UAEDesc::AddDesc(&outResult, 0, resultDesc);

				err = ::AEDisposeDesc(insertD);
				err = ::AEDuplicateDesc(resultDesc, insertD);
				ThrowIfOSErr_(err);
			}
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AccessModelObject												  [public]
// ---------------------------------------------------------------------------
//	Get Token for ModelObject
//
//	Part of the OSL object resolution process. Given a Token for a
//	Container and a key description of some objects, make a Token
//	for those objects.

void
LModelDirector::AccessModelObject(
	DescType		inDesiredClass,
	const AEDesc&	inContainerToken,
	DescType		/* inContainerClass */,
	DescType		inKeyForm,
	const AEDesc&	inKeyData,
	AEDesc&			outToken,
	SInt32			/* inRefCon */)
{
	LModelObject	*theContainer = nil;

	//	The OSL normally does this.  But... it doesn't appear to in
	//	in the case of formWhose or formTest.  Since GetModelToken
	//	can "add" to existing AEDesc's, this is required.

	outToken.descriptorType = typeNull;
	outToken.dataHandle = nil;

	theContainer = LModelObject::GetModelFromToken(inContainerToken);

	try {
		theContainer->GetModelToken(inDesiredClass, inKeyForm, inKeyData,
										outToken);
	}

	catch (...) {
		DisposeToken(outToken);		//	In case the OSL doesn't do it
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ AccessModelObjectList											  [public]
// ---------------------------------------------------------------------------

void
LModelDirector::AccessModelObjectList(
	DescType		inDesiredClass,
	const AEDesc&	inContainerToken,
	DescType		/* inContainerClass */,
	DescType		inKeyForm,
	const AEDesc&	inKeyData,
	AEDesc&			outToken,
	SInt32			/* inRefCon */)
{
	OSErr		err = noErr;

	outToken.descriptorType = typeNull;
	outToken.dataHandle		= nil;

	SInt32		itemCount = 0;
	::AECountItems(&inContainerToken, &itemCount);

	bool	makeList = true;

	try {
		if (inKeyForm == formAbsolutePosition) {

				// For key form absolute position, we need to get
				// the token for a particular item in a list

			SInt32	itemIndex = UAppleEventsMgr::GetAbsolutePositionIndex(
									inKeyData, itemCount);

			if ( (itemIndex > 0) && (itemIndex <= itemCount) ) {
									// Index is within positive range

				DescType		theKeyword;
				StAEDescriptor	itemDesc;

					// Get Item from List and see if its kind
					// matches the desired class. cObject is a
					// generic class for any kind of item.

				err = ::AEGetNthDesc(	&inContainerToken,
										itemIndex,
										typeWildCard,
										&theKeyword,
										itemDesc);
				ThrowIfOSErr_(err);

				LModelObject*	itemObject =
								LModelObject::GetModelFromToken(itemDesc);

				if ( (inDesiredClass == cObject)  ||
					 (inDesiredClass == itemObject->GetModelKind()) ) {
					 					// We have a match. Make token
					 					//   for this item.
					LModelObject::PutInToken(itemObject, outToken);
				}
			}

				// Negative index means to get "all" items. In that case,
				// we do need to make a list of tokens.

			makeList = (itemIndex < 0);
		}

		if (makeList) {

				// Key form is not formAbsolutePosition or request was
				// for all items. In either case, the outToken is a list
				// containing a token for each item in the container

			err = ::AECreateList(nil, 0, false, &outToken);
			ThrowIfOSErr_(err);

			for (SInt32 i = 1; i <= itemCount; i++) {
				StAEDescriptor	containerDesc;
				StAEDescriptor	subModelDesc;
				LModelObject*	container;
				DescType		theKeyword;

				err = ::AEGetNthDesc(	&inContainerToken,
										i,
										typeWildCard,
										&theKeyword,
										containerDesc);
				ThrowIfOSErr_(err);

				container = LModelObject::GetModelFromToken(containerDesc);
				container->GetModelToken(inDesiredClass, inKeyForm,
											inKeyData, subModelDesc);

				UAEDesc::AddDesc(&outToken, 0, subModelDesc);
			}
		}
	}

	catch (...) {
		DisposeToken(outToken);		//	In case the OSL doesn't do it
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ DisposeToken													  [public]
// ---------------------------------------------------------------------------
//	A Token is no longer needed.
//
//	Will not propogate errors.

void
LModelDirector::DisposeToken(
	AEDesc&		ioToken)
{
	::AEDisposeDesc(&ioToken);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CompareObjects												  [public]
// ---------------------------------------------------------------------------
//	Pass back the result of comparing one object with another

void
LModelDirector::CompareObjects(
	DescType		inComparisonOperator,
	const AEDesc&	inBaseObject,
	const AEDesc&	inCompareObjectOrDesc,
	Boolean&		outResult)
{
		// Base Object compares itself to the other object

	LModelObject	*theModel = LModelObject::GetModelFromToken(inBaseObject);

	if (inCompareObjectOrDesc.descriptorType == type_ModelToken) {
								// Other "object" is another ModelObject
		outResult = theModel->CompareToModel(inComparisonOperator,
					LModelObject::GetModelFromToken(inCompareObjectOrDesc));

	} else {					// Other "object" is a Descriptor
		outResult = theModel->CompareToDescriptor(inComparisonOperator,
								inCompareObjectOrDesc);
	}
}


// ---------------------------------------------------------------------------
//	¥ CountObjects													  [public]
// ---------------------------------------------------------------------------
//	Pass back the number of objects of the desiredClass that are in the
//	specified container object

void
LModelDirector::CountObjects(
	DescType		inDesiredClass,
	DescType		/* inContainerClass */,
	const AEDesc&	inContainer,
	SInt32&			outCount)
{
	LModelObject	*theModel = LModelObject::GetModelFromToken(inContainer);
	outCount = theModel->CountSubModels(inDesiredClass);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Resolve												 [static] [public]
// ---------------------------------------------------------------------------
//	Wrapper for AEResolve.  Takes into consideration mResolveFlags and, if the
//	inSpec is typeType pSelection, will fabricate a selection property ospec
//	with a null wrapper.  (AppleScript has an annoying habit of not recreating
//	AE's exactly as they were sent.)

OSErr
LModelDirector::Resolve(
	const AEDesc&	inSpec,
	AEDesc&			outToken)
{
	OSErr		err = noErr;
	DescType	theType;
	Boolean		fabricate = false;

	if (inSpec.descriptorType == typeType) {
		UExtractFromAEDesc::TheType(inSpec, theType);

		if (theType == pSelection) {
			fabricate = true;
		}
	}

	if (fabricate) {
		StAEDescriptor	nullSpec;
		StAEDescriptor	propDesc(typeType, &theType, sizeof(theType));
		StAEDescriptor	fabSpec;

		err = ::CreateObjSpecifier(cProperty, nullSpec, formPropertyID,
											propDesc, false, fabSpec);
		if (err == noErr) {
			err = ::AEResolve(fabSpec, sModelDirector->mResolveFlags,
								&outToken);
		}

	} else {
		err = ::AEResolve(&inSpec, sModelDirector->mResolveFlags, &outToken);
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ AppleEventHandler									  [protected] [static]
// ---------------------------------------------------------------------------
//	Generic CallBack function for all AppleEvents

pascal OSErr
LModelDirector::AppleEventHandler(
	const AppleEvent*	inAppleEvent,
	AppleEvent*			outAEReply,
	AEHandlerRefConT	inRefCon)
{
	OSErr	err = noErr;

	sCurrentAEReply = outAEReply;

	try {
		sModelDirector->HandleAppleEvent(*inAppleEvent, *outAEReply, inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
		StoreReplyErrorString(inException.GetErrorString(), outAEReply);
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	LModelObject::FinalizeLazies();

	sCurrentAEReply = nil;

	return err;
}


// ---------------------------------------------------------------------------
//	¥ CountElementsEventHandler							  [protected] [static]
// ---------------------------------------------------------------------------
//	CallBack function for CountElements AppleEvent

pascal OSErr
LModelDirector::CountElementsEventHandler(
	const AppleEvent*	inAppleEvent,
	AppleEvent*			outAEReply,
	AEHandlerRefConT	inRefCon)
{
	OSErr	err = noErr;

	sCurrentAEReply = outAEReply;

	try {
		sModelDirector->HandleCountElementsEvent(*inAppleEvent, *outAEReply, inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
		StoreReplyErrorString(inException.GetErrorString(), outAEReply);
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	LModelObject::FinalizeLazies();

	sCurrentAEReply = nil;

	return err;
}


// ---------------------------------------------------------------------------
//	¥ CreateElementEventHandler							  [protected] [static]
// ---------------------------------------------------------------------------
//	CallBack function for CreateElement AppleEvent

pascal OSErr
LModelDirector::CreateElementEventHandler(
	const AppleEvent*	inAppleEvent,
	AppleEvent*			outAEReply,
	AEHandlerRefConT	inRefCon)
{
	OSErr	err = noErr;

	sCurrentAEReply = outAEReply;

	try {
		sModelDirector->HandleCreateElementEvent(*inAppleEvent, *outAEReply,
							inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
		StoreReplyErrorString(inException.GetErrorString(), outAEReply);
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	LModelObject::FinalizeLazies();

	sCurrentAEReply = nil;

	return err;
}


// ---------------------------------------------------------------------------
//	¥ DoObjectsExistEventHandler						  [protected] [static]
// ---------------------------------------------------------------------------
//	CallBack function for DoObjectsExist AppleEvent

pascal OSErr
LModelDirector::DoObjectsExistEventHandler(
	const AppleEvent*	inAppleEvent,
	AppleEvent*			outAEReply,
	AEHandlerRefConT	inRefCon)
{
	OSErr	err = noErr;

	sCurrentAEReply = outAEReply;

	try {
		sModelDirector->HandleDoObjectsExistEvent(*inAppleEvent, *outAEReply,
							inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
		StoreReplyErrorString(inException.GetErrorString(), outAEReply);
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	LModelObject::FinalizeLazies();

	sCurrentAEReply = nil;

	return err;
}


// ---------------------------------------------------------------------------
//	¥ ModelObjectAccessor								  [protected] [static]
// ---------------------------------------------------------------------------

pascal OSErr
LModelDirector::ModelObjectAccessor(
	DescType		inDesiredClass,
	const AEDesc*	inContainerToken,
	DescType		inContainerClass,
	DescType		inKeyForm,
	const AEDesc*	inKeyData,
	AEDesc*			outToken,
	SInt32			inRefCon)
{
	OSErr	err = noErr;

	try {
		sModelDirector->AccessModelObject(inDesiredClass,
									*inContainerToken, inContainerClass,
									inKeyForm, *inKeyData,
									*outToken, inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = errAENoSuchObject;
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ ModelObjectListAccessor							  [protected] [static]
// ---------------------------------------------------------------------------

pascal OSErr
LModelDirector::ModelObjectListAccessor(
	DescType		inDesiredClass,
	const AEDesc*	inContainerToken,
	DescType		inContainerClass,
	DescType		inKeyForm,
	const AEDesc*	inKeyData,
	AEDesc*			outToken,
	SInt32			inRefCon)
{
	OSErr	err = noErr;

	try {
		sModelDirector->AccessModelObjectList(inDesiredClass,
									*inContainerToken, inContainerClass,
									inKeyForm, *inKeyData,
									*outToken, inRefCon);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = errAENoSuchObject;
	}

	return err;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ OSLDisposeToken									  [protected] [static]
// ---------------------------------------------------------------------------

pascal OSErr
LModelDirector::OSLDisposeToken(
	AEDesc*		inToken)
{
	OSErr	err = noErr;

	try {
		sModelDirector->DisposeToken(*inToken);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ OSLCompareObjects									  [protected] [static]
// ---------------------------------------------------------------------------

pascal OSErr
LModelDirector::OSLCompareObjects(
	DescType		inComparisonOperator,
	const AEDesc*	inBaseObject,
	const AEDesc*	inCompareObjectOrDesc,
	Boolean*		outResult)
{
	OSErr	err = noErr;

		// According to Technote 1095, OLS has a bug where the
		// AEDesc* parameters to this callback functions are
		// pointers to data in an unlocked Handle. The recommended
		// solution is to make local copies of the AEDesc structs.
		//
		// The Technote further describes a potential memory leak
		// because the dispose token routine is not called for
		// these descriptors. However, PP does not allocate extra
		// memory for tokens so we don't need to worry about that.

	AEDesc	baseObject		= *inBaseObject;
	AEDesc	compareObject	= *inCompareObjectOrDesc;

	try {
		sModelDirector->CompareObjects(inComparisonOperator, baseObject,
										compareObject, *outResult);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ OSLCountObjects									  [protected] [static]
// ---------------------------------------------------------------------------

pascal OSErr
LModelDirector::OSLCountObjects(
	DescType		inDesiredClass,
	DescType		inContainerClass,
	const AEDesc*	inContainer,
	SInt32*			outCount)
{
	OSErr	err = noErr;

	try {
		sModelDirector->CountObjects(inDesiredClass, inContainerClass,
									*inContainer, *outCount);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = errAEEventNotHandled;
	}

	return err;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ StoreReplyErrorString								  [protected] [static]
// ---------------------------------------------------------------------------

void
LModelDirector::StoreReplyErrorString(
	ConstStringPtr	inErrorString,
	AppleEvent*		ioReply)
{
	if (ioReply->descriptorType != typeNull) {
		if (StrLength(inErrorString) > 0) {
			::AEPutParamPtr(ioReply, keyErrorString, typeChar,
							inErrorString + 1,
							StrLength(inErrorString));
		}
	}
}


PP_End_Namespace_PowerPlant
