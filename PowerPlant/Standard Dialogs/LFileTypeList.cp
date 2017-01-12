// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileTypeList.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Maintains a list of file types (four-character codes). You can access
//	this list as a count and array pointer (as used by Standard File) or
//	as a NavTypeListHandle (as used by Navigation Services).

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFileTypeList.h>
#include <PP_Resources.h>

#include <Processes.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct FileTypeList using the specifed 'open' resource ID. By
//	convention, the default ID is 128.

LFileTypeList::LFileTypeList(
	ResIDT		inOpenResID)
{
	Handle	openRes = ::GetResource(ResType_Open, inOpenResID);
	ThrowIfResFail_(openRes);

	::DetachResource(openRes);

	mTypeListH.Adopt(openRes);
}


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct FileTypeList consisting of a single file type

LFileTypeList::LFileTypeList(
	OSType		inFileType)

	: mTypeListH(sizeof(NavTypeList))
{
	OSType	signature = GetProcessSignature();

	NavTypeListPtr	typesP = (NavTypeListPtr) *((Handle) mTypeListH);

	typesP->componentSignature	= signature;
	typesP->reserved			= 0;
	typesP->osTypeCount			= 1;
	typesP->osType[0]			= inFileType;
}


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct FileTypeList from an array of file types
//
//	Do not pass 0 for inNumTypes or nil for inTypeList. If you want to
//	create a type list for all file types, pass fileTypes_All to the
//	constructor which takes a NavTypeListHandle parameter.

LFileTypeList::LFileTypeList(
	SInt16		inNumTypes,
	OSType*		inTypeList)

	: mTypeListH((Size) (sizeof(NavTypeList) +
							sizeof(OSType) * (inNumTypes - 1)))
{
	OSType	signature = GetProcessSignature();

	NavTypeListPtr	typesP = (NavTypeListPtr) *((Handle) mTypeListH);

	typesP->componentSignature	= signature;
	typesP->reserved			= 0;
	typesP->osTypeCount			= inNumTypes;

	::BlockMoveData(inTypeList, typesP->osType,
							(Size) (sizeof(OSType) * inNumTypes));
}


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct FileTypeList from an existing NavTypeListHandle. Object takes
//	ownership of the Handle which gets disposed when the object is deleted.
//
//	Pass fileTypes_All (nil) for inTypesH to specify all file types.

LFileTypeList::LFileTypeList(
	NavTypeListHandle	inTypesH)

	: mTypeListH((Handle) inTypesH)
{
}


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LFileTypeList::LFileTypeList(
	const LFileTypeList&	inFileTypeList)

	: mTypeListH(inFileTypeList.mTypeListH)
{
}


// ---------------------------------------------------------------------------
//	¥ LFileTypeList							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LFileTypeList&
LFileTypeList::operator = (
	const LFileTypeList&	inFileTypeList)
{
	if (&inFileTypeList != this) {
		mTypeListH = inFileTypeList.mTypeListH;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LFileTypeList						Destructor				  [public]
// ---------------------------------------------------------------------------

LFileTypeList::~LFileTypeList()
{
}


// ---------------------------------------------------------------------------
//	¥ NumberOfTypes													  [public]
// ---------------------------------------------------------------------------

SInt16
LFileTypeList::NumberOfTypes() const
{
	SInt16	numTypes = -1;			// Means all file types for StandardFile

	if (mTypeListH.IsValid()) {
		numTypes = ((NavTypeListPtr) *(mTypeListH.Get()))->osTypeCount;
	}

	return numTypes;
}


// ---------------------------------------------------------------------------
//	¥ TypeListPtr													  [public]
// ---------------------------------------------------------------------------

const OSType*
LFileTypeList::TypeListPtr() const
{
	OSType*		typeList = nil;

	if (mTypeListH.IsValid()) {
		::HLockHi(mTypeListH.Get());
		typeList = ((NavTypeListPtr) *(mTypeListH.Get()))->osType;
	}

	return typeList;
}


// ---------------------------------------------------------------------------
//	¥ TypeListHandle												  [public]
// ---------------------------------------------------------------------------

NavTypeListHandle
LFileTypeList::TypeListHandle() const
{
	return (NavTypeListHandle) mTypeListH.Get();
}


// ---------------------------------------------------------------------------
//	¥ SetSignature													  [public]
// ---------------------------------------------------------------------------
//	Set the signature (file creator code). Constructors for this class all
//	set the signature to that of the current process. Call this function
//	if you wish to use a different signature.

void
LFileTypeList::SetSignature(
	OSType		inSignature)
{
	NavTypeListPtr	typesP = (NavTypeListPtr) *((Handle) mTypeListH);

	typesP->componentSignature	= inSignature;
}


// ---------------------------------------------------------------------------
//	¥ GetProcessSignature											  [public]
// ---------------------------------------------------------------------------
//	Return the signature of the current process

OSType
LFileTypeList::GetProcessSignature()
{
	ProcessSerialNumber		thePSN;

	thePSN.highLongOfPSN = 0;
	thePSN.lowLongOfPSN  = kCurrentProcess;

	ProcessInfoRec	info;

	info.processInfoLength	= sizeof(ProcessInfoRec);
	info.processName		= nil;
	info.processAppSpec		= nil;

	OSErr	err = ::GetProcessInformation(&thePSN, &info);
	ThrowIfOSErr_(err);

	return info.processSignature;
}


PP_End_Namespace_PowerPlant
