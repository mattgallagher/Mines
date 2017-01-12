// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStuffItArchive.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper class for dealing with StuffIt Archives as an object. Uses low
//	level StuffIt Engine routines to walk the archive.
//
//	NOTE:
//	In order to greatly simplify the interfaces to StuffToArchive and
//	UnStuffFromArchive, I have made the default values for the various
//	parameters member variables. I think the defaults I have chosen will work
//	for most common instances, however if you find yourself in a situation
//	where you are changing them consistently, you might consider subclassing
//	this object and overriding the InitDefaults method.
//
//	IMPORTANT:	These classes require the use of the Aladdin StuffIt Engine
//				libraries. You may need a license from Aladdin in order to
//				distribute products using these libraries. Please be sure
//				to carefully read the included Aladdin documentation. For
//				more information see <http://www.aladdinsys.com>

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStuffItArchive.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LStuffItArchive						Constructor				  [public]
// ---------------------------------------------------------------------------

LStuffItArchive::LStuffItArchive(
	FSSpec&	inArchive)
{
	mFSSpec					= inArchive;
	mArchiveOpen			= false;
	mIteratorInitialized	= false;
	
	InitDefaults();
}


// ---------------------------------------------------------------------------
//	¥ ~LStuffItArchive						Destructor				  [public]
// ---------------------------------------------------------------------------

LStuffItArchive::~LStuffItArchive()
{
	try {
		Close();
	}

	catch (...) { }
}


// ---------------------------------------------------------------------------
//	¥ InitDefaults
// ---------------------------------------------------------------------------
//	Initializes defaults used within calls to StuffToArchive and
//	UnStuffFromArchive. You might want to consider subclassing the object and
//	overriding this method if you find yourself having to modify the default
//	behaviours consistently.

void
LStuffItArchive::InitDefaults()
{
	mCompressOriginal		= kCompressOriginal;
	mDeleteOriginal			= kDontDeleteOriginal;
	mEncryptOriginal		= kDontEncryptOriginal;
	mResolveAliases			= kResolveAliases;
	mNoRecompression		= kDontRecompressCompresed;
	mPromptForDestination	= kPromptForDestination;
	mStopOnAbort			= kStopOnAbort;
	mCreateFolder			= kCreateFolderSmart;
	mConflictAction			= kUnStuffConflictAutoRename;
	mConvertTextFiles		= kTextConvertSmart;
	mShowNoProgress			= kShowStandardProgress;
	mAlertCBUPP				= kNoCallback;
	mStatusCBUPP			= kNoCallback;
	mPeriodicCBUPP			= kNoCallback;
}

#pragma mark -
#pragma mark === File Handling ===

// ---------------------------------------------------------------------------
//	¥ Create
// ---------------------------------------------------------------------------
//	Creates a new archive. Will be called automatically if the file represented
//	by the FSSpec handed to the constructor does not exist.

void
LStuffItArchive::Create()
{
	OSErr err = NewArchive(&mFSSpec, kFileTypeStuffIt);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ Open
// ---------------------------------------------------------------------------
//	Opens the archive file represented by the FSSpec handed to the constructor

void
LStuffItArchive::Open()
{
	if (mArchiveOpen) {
		return;
	}

	OSErr err = OpenArchive(&mFSSpec, fsRdWrPerm, &mArchiveInfo);

	if (err == fnfErr) {
		Create();
		Open();
		err = noErr;
	}

	ThrowIfOSErr_(err);
	mArchiveOpen = true;
}


// ---------------------------------------------------------------------------
//	¥ Close
// ---------------------------------------------------------------------------
//	Closes the archive file represented by the FSSpec handed to the constructor

void
LStuffItArchive::Close()
{
	if (not mArchiveOpen) {
		return;
	}

	OSErr err = CloseArchive(&mArchiveInfo);
	ThrowIfOSErr_(err);
	mArchiveOpen = false;
}

#pragma mark -
#pragma mark === Archive Element Handling ===

// ---------------------------------------------------------------------------
//	¥ StuffToArchive
// ---------------------------------------------------------------------------
//	Compresses a file into the archive

void
LStuffItArchive::StuffToArchive(
	FSSpec&			inFile,
	archiveObject*	inObjectParent,
	unsigned char*	inPassword)
{
	LStuffItFileList tempList(inFile);
	StuffToArchive(tempList, inObjectParent, inPassword);
}


// ---------------------------------------------------------------------------
//	¥ StuffToArchive
// ---------------------------------------------------------------------------
//	Compresses a file into the archive

void
LStuffItArchive::StuffToArchive(
	LStuffItFileList&	inFileList,
	archiveObject*		inObjectParent,
	unsigned char*		inPassword)
{
	Open();

	if (!inObjectParent) {
		inObjectParent = GetDefaultParent();
	}

	StOpenStuffIt engineOpen;

	OSErr err = AddItemsToArchive (
							UStuffItSupport::sCookie,
							&mArchiveInfo,
							inObjectParent,
							inFileList,
							mCompressOriginal,
							mDeleteOriginal,
							mEncryptOriginal,
							mResolveAliases,
							mNoRecompression,
							mConflictAction,
							inPassword,
							kStuffItCompressionMax,
							NULL,
							mShowNoProgress,
							mAlertCBUPP,
							mStatusCBUPP,
							mPeriodicCBUPP);

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ AddFolderToArchive
// ---------------------------------------------------------------------------
//	Adds an empty folder the archive

void
LStuffItArchive::AddFolderToArchive(
	Str255			folderName,
	archiveObject&	outFolderObject,
	archiveObject*	inObjectParent)
{
	Open();

	if (!inObjectParent) {
		inObjectParent = GetDefaultParent();
	}

	OSErr err = CreateArchiveFolder(
							UStuffItSupport::sCookie,
							&mArchiveInfo,
							inObjectParent,
							folderName,
							&outFolderObject,
							true);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ UnStuffFromArchive
// ---------------------------------------------------------------------------
//	Gets a file from the archive. Does NOT delete the file within the archive

void
LStuffItArchive::UnStuffFromArchive(
	archiveObject&	inObject,
	FSSpec			destSpec,
	unsigned char*	inPassword)
{
	LStuffItArcObjectList tempList(inObject);
	UnStuffFromArchive(tempList, destSpec, inPassword);
}


// ---------------------------------------------------------------------------
//	¥ UnStuffFromArchive
// ---------------------------------------------------------------------------
//	Gets a file from the archive. Does NOT delete the file within the archive

void
LStuffItArchive::UnStuffFromArchive(
	LStuffItArcObjectList&	inObjectList,
	FSSpec					destSpec,
	unsigned char*			inPassword)
{
	Open();

	StOpenStuffIt engineOpen;

	OSErr err = ExpandFromArchive (
							UStuffItSupport::sCookie,
							&mArchiveInfo,
							inObjectList,
							&destSpec,
							&destSpec,
							mPromptForDestination,
							mCreateFolder,
							mStopOnAbort,
							mConflictAction,
							mConvertTextFiles,
							inPassword,
							mShowNoProgress,
							mAlertCBUPP,
							mStatusCBUPP,
							mPeriodicCBUPP);


	ThrowIfOSErr_(err);

}

#pragma mark -
#pragma mark === Iterative Archive Navigation ===

// ---------------------------------------------------------------------------
//	¥ Reset
// ---------------------------------------------------------------------------
//	Resets the iterator routines to the top of the archive

void
LStuffItArchive::Reset()
{
	Open();
	OSErr err = GetFirstChildArchiveObject(&mArchiveInfo, kArchiveRootLevel, &mCurrentObject);
	if (err != noObjectAtThatLocation) {
		ThrowIfOSErr_(err);
	}

	mIteratorInitialized = true;
	mIteratorAtHead = true;
}


// ---------------------------------------------------------------------------
//	¥ Current
// ---------------------------------------------------------------------------
//	Returns the current archive object pointed to by the iterator routines

Boolean
LStuffItArchive::Current(
	archiveObject&	outObject)
{
	Open();

	if (not mIteratorInitialized) {
		Reset();
	}

	outObject = mCurrentObject;
	return true;
}


// ---------------------------------------------------------------------------
//	¥ Next
// ---------------------------------------------------------------------------
//	Returns the next archive object pointed to by the iterator routines

Boolean
LStuffItArchive::Next(
	archiveObject&	outObject)
{
	Open();

	if (not mIteratorInitialized) {
		Reset();
	}

	if (mIteratorAtHead) {
		mIteratorAtHead = false;
		outObject = mCurrentObject;
		return true;
	}

	OSErr err = GetNextArchiveObject(&mArchiveInfo, &mCurrentObject, &outObject);
	if (err == noObjectAtThatLocation) {
		return false;
	}

	ThrowIfOSErr_(err);

	mCurrentObject = outObject;
	return true;
}


// ---------------------------------------------------------------------------
//	¥ Down
// ---------------------------------------------------------------------------
//	Walks "down" into a folder within an archive

Boolean
LStuffItArchive::Down(
	archiveObject&	outObject)
{
	Open();

	if (not mIteratorInitialized) {
		Reset();
	}

	OSErr err = GetFirstChildArchiveObject(&mArchiveInfo, &mCurrentObject, &outObject);
	if (err == noObjectAtThatLocation){
		return false;
	}

	ThrowIfOSErr_(err);

	mCurrentObject = outObject;
	return true;
}


// ---------------------------------------------------------------------------
//	¥ Up
// ---------------------------------------------------------------------------
//	Walks "up" out of a folder within an archive

Boolean
LStuffItArchive::Up(
	archiveObject&	outObject)
{
	Open();

	if (!mIteratorInitialized) {
		Reset();
		return false;
	}

	if (mIteratorAtHead) {
		return false;
	}

	OSErr err = GetParentArchiveObject(&mArchiveInfo, &mCurrentObject, &outObject);
	if (err == noObjectAtThatLocation){
		return false;
	}

	ThrowIfOSErr_(err);

	 mCurrentObject = outObject;
	return true;
}

#pragma mark -
#pragma mark === Archive Comments ===

// ---------------------------------------------------------------------------
//	¥ GetComments
// ---------------------------------------------------------------------------
//	Gets the comments from an archive

void
LStuffItArchive::GetComments(
	Handle			outComment,
	StScrpHandle	outStyleInfo)
{
	Open();
	OSErr err = GetArchiveComments(&mArchiveInfo, &outComment, &outStyleInfo);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ SetComments
// ---------------------------------------------------------------------------
//	Sets the comments for an archive

void
LStuffItArchive::SetComments(
	Handle			inComment,
	StScrpHandle	inStyleInfo)
{
	Open();
	OSErr err = SetArchiveComments(&mArchiveInfo, inComment, inStyleInfo);
	ThrowIfOSErr_(err);
}

#pragma mark -
#pragma mark === Utilities ===

// ---------------------------------------------------------------------------
//	¥ GetDecompressedSize
// ---------------------------------------------------------------------------
//	Example of using the iterator routines. Returns the decompressed size of
//	files within the archive.

SInt32
LStuffItArchive::GetDecompressedSize()
{
	SInt32 totalSize = 0;
	archiveObject currObject;
	Reset();
	while (Next(currObject)) {
		totalSize += (SInt32) currObject.objectExpandedSize;
	}

	return totalSize;
}


// ---------------------------------------------------------------------------
//	¥ SetArchiveObjectInfo
// ---------------------------------------------------------------------------
//	Writes changes to object info back to the archive. Allows objects to be
//	renamed, etc..

void
LStuffItArchive::SetObjectInfo(
	archiveObject&	inObject)
{
	Open();
	OSErr err = SetArchiveObjectInfo(&mArchiveInfo, &inObject);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ UpdateObjectInfo
// ---------------------------------------------------------------------------
//	Re-reads changes to object information from the archive.

void
LStuffItArchive::UpdateObjectInfo(
	archiveObject&	inOutObject)
{
	Open();
	OSErr err = UpdateArchiveObjectInfo(&mArchiveInfo, &inOutObject);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetDefaultParent
// ---------------------------------------------------------------------------
//	Utility routine to find the parent of the object pointed to by the iterator
//	routines. Returns the current object if no parent object.

archiveObject*
LStuffItArchive::GetDefaultParent()
{
	if (not mIteratorInitialized) {
		Reset();
	}

	archiveObject * outObject = nil;
	OSErr err = GetParentArchiveObject(&mArchiveInfo, &mCurrentObject, outObject);
	if (err == noObjectAtThatLocation){
		return &mCurrentObject;
	} else {
		return outObject;
	}
}


PP_End_Namespace_PowerPlant
