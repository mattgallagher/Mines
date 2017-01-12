// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNavServicesDialogs.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes which use Navigation Services dialogs to prompt the user to
//	open and save files

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UNavServicesDialogs.h>

#include <LFileTypeList.h>
#include <LString.h>
#include <PP_Resources.h>
#include <UDesktop.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>
#include <UModalDialogs.h>

#include <Script.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ StNavReplyRecord						Default Constructor		  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::StNavReplyRecord::StNavReplyRecord()
{
	mNavReply.validRecord = false;

	SetDefaultValues();
}


// ---------------------------------------------------------------------------
//	¥ ~StNavReplyRecord						Destructor				  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::StNavReplyRecord::~StNavReplyRecord()
{
	if (mNavReply.validRecord) {
		::NavDisposeReply(&mNavReply);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultValues												  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::StNavReplyRecord::SetDefaultValues()
{
									// Clean up record if it is valid
	if (mNavReply.validRecord) {
		::NavDisposeReply(&mNavReply);
	}

		// The Nav Services documentation states that the other fields
		// are invalid if validRecord is false. However, we put sensible
		// defaults in each field so that we can access them without
		// having to throw an error if validRecord is false.

	mNavReply.validRecord				= false;
	mNavReply.replacing					= false;
	mNavReply.isStationery				= false;
	mNavReply.translationNeeded			= false;
	mNavReply.selection.descriptorType = typeNull;
	mNavReply.selection.dataHandle		= nil;
	mNavReply.keyScript					= smSystemScript;
	mNavReply.fileTranslation			= nil;
}


// ---------------------------------------------------------------------------
//	¥ GetFileSpec													  [public]
// ---------------------------------------------------------------------------
//	Pass back the FSSpec for the underlying file. Accesses the first file
//	if there is more than one file.

void
UNavServicesDialogs::StNavReplyRecord::GetFileSpec(
	FSSpec&		outFileSpec) const
{
	UExtractFromAEDesc::TheFSSpec(Selection(), outFileSpec);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Load															  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::Load()
{
	::NavLoad();
}


// ---------------------------------------------------------------------------
//	¥ Unload														  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::Unload()
{
	::NavUnload();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskSaveChanges												  [public]
// ---------------------------------------------------------------------------

SInt16
UNavServicesDialogs::AskSaveChanges(
	ConstStringPtr	inDocumentName,
	ConstStringPtr	inAppName,
	bool			inQuitting)
{
	NavDialogOptions	options;
	::NavGetDefaultDialogOptions(&options);

	LString::CopyPStr(inDocumentName, options.savedFileName);
	LString::CopyPStr(inAppName, options.clientName);

	StNavEventUPP				eventUPP(NavEventProc);
	NavAskSaveChangesResult		reply = kNavAskSaveChangesCancel;

	NavAskSaveChangesAction		action = kNavSaveChangesClosingDocument;
	if (inQuitting) {
		action = kNavSaveChangesQuittingApplication;
	}

	UDesktop::Deactivate();

	OSErr	err = ::NavAskSaveChanges(&options, action, &reply, eventUPP, nil);

	UDesktop::Activate();

	ThrowIfOSErr_(err);

	return (SInt16) reply;
}


// ---------------------------------------------------------------------------
//	¥ AskConfirmRevert												  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::AskConfirmRevert(
	ConstStringPtr	inDocumentName)
{
	NavDialogOptions	options;
	::NavGetDefaultDialogOptions(&options);

	LString::CopyPStr(inDocumentName, options.savedFileName);

	StNavEventUPP				eventUPP(NavEventProc);
	NavAskDiscardChangesResult	reply = kNavAskDiscardChangesCancel;

	UDesktop::Deactivate();

	OSErr	err = ::NavAskDiscardChanges(&options, &reply, eventUPP, nil);

	UDesktop::Activate();

	ThrowIfOSErr_(err);

	return (reply == kNavAskDiscardChanges);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskOpenOneFile												  [public]
// ---------------------------------------------------------------------------
//	Simple wrapper for NavGetFile that lets a user select one file using
//	the default options

bool
UNavServicesDialogs::AskOpenOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags)
{
	LFileTypeList	fileTypes(inFileType);
	LFileChooser	chooser;

	inFlags &= ~kNavAllowMultipleFiles;		// Can't select multiple files
	NavDialogOptions*	options = chooser.GetDialogOptions();
	options->dialogOptionFlags = inFlags;

	bool	askOK = chooser.AskOpenFile(fileTypes);

	if (askOK) {
		chooser.GetFileSpec(1, outFileSpec);
	}

	return askOK;
}


// ---------------------------------------------------------------------------
//	¥ AskChooseOneFile												  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::AskChooseOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags)
{
	LFileTypeList	fileTypes(inFileType);
	LFileChooser	chooser;

	NavDialogOptions*	options = chooser.GetDialogOptions();
	options->dialogOptionFlags = inFlags;

	return chooser.AskChooseOneFile(fileTypes, outFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ AskChooseFolder												  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::AskChooseFolder(
	FSSpec&					outFileSpec,
	SInt32&					outFolderDirID)
{
	LFileChooser	chooser;

	return chooser.AskChooseFolder(outFileSpec, outFolderDirID);
}


// ---------------------------------------------------------------------------
//	¥ AskChooseVolume												  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::AskChooseVolume(
	FSSpec&					outFileSpec)
{
	LFileChooser	chooser;

	return chooser.AskChooseVolume(outFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ AskSaveFile													  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::AskSaveFile(
	ConstStringPtr			inDefaultName,
	OSType					inFileType,
	FSSpec&					outFileSpec,
	bool&					outReplacing,
	NavDialogOptionFlags	inFlags)
{
	LFileDesignator		designator;

	designator.SetFileType(inFileType);

	if (not (inFlags & kNavAllowStationery)) {
									// Turn off type popup. The popup
									//   contains file translation and
									//   stationery options. By design
									//   this function doesn't support
									//   file translation.
		inFlags |= kNavNoTypePopup;
	}

	NavDialogOptions*	options = designator.GetDialogOptions();
	options->dialogOptionFlags = inFlags;

	bool	askOK = designator.AskDesignateFile(inDefaultName);

	if (askOK) {
		designator.GetFileSpec(outFileSpec);
		outReplacing = designator.IsReplacing();
	}

	return askOK;
}

#pragma mark -

// ===========================================================================
//	LFileChooser Class
// ===========================================================================
//	Uses Navigation Services to ask user to open a file

// ---------------------------------------------------------------------------
//	¥ LFileChooser::LFileChooser			Constructor				  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::LFileChooser::LFileChooser()
{
	::NavGetDefaultDialogOptions(&mNavOptions);

	::GetIndString( mNavOptions.windowTitle,
					STRx_Standards, str_OpenDialogTitle);

	mNavFilterProc	= nil;
	mNavPreviewProc	= nil;
	mSelectDefault	= false;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::~LFileChooser			Destructor				  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::LFileChooser::~LFileChooser()
{
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------

NavDialogOptions*
UNavServicesDialogs::LFileChooser::GetDialogOptions()
{
	return &mNavOptions;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetDefaultLocation								  [public]
// ---------------------------------------------------------------------------
//	Specify the item that is selected when the "choose" dialog is displayed
//
//	If the inFileSpec refers to a directory, the inSelectIt parameter
//	specifies whether to select the directory itself (true), or to select
//	the first item within the directory (false).

void
UNavServicesDialogs::LFileChooser::SetDefaultLocation(
	const FSSpec&	inFileSpec,
	bool			inSelectIt)
{
	mDefaultLocation = inFileSpec;
	mSelectDefault	 = inSelectIt;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetObjectFilterProc								  [public]
// ---------------------------------------------------------------------------
//	Specify the object filter callback function
//
//	See the comments for UClassicDialogs::LFileChooser::SetObjectFilterProc()
//	if you wish to use the same callback function for both NavServices
//	and StandardFile.

void
UNavServicesDialogs::LFileChooser::SetObjectFilterProc(
	NavObjectFilterProcPtr	inFilterProc)
{
	mNavFilterProc = inFilterProc;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetPreviewProc									  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileChooser::SetPreviewProc(
	NavPreviewProcPtr	inPreviewProc)
{
	mNavPreviewProc = inPreviewProc;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskOpenFile										  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileChooser::AskOpenFile(
	const LFileTypeList&	inFileTypes)
{
									// Create UPPs for callback functions
	StNavEventUPP			eventUPP(NavEventProc);
	StNavObjectFilterUPP	objectFilterUPP(mNavFilterProc);
	StNavPreviewUPP			previewUPP(mNavPreviewProc);

	mNavReply.SetDefaultValues();	// Initialize Reply record

									// Set default location, the location
									//   that's displayed when the dialog
									//   first appears
	AEDesc*		defaultLocationDesc = nil;
	if (not mDefaultLocation.IsNull()) {
		defaultLocationDesc = mDefaultLocation;

		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}

	UDesktop::Deactivate();

	OSErr err = ::NavGetFile(
						defaultLocationDesc,
						mNavReply,
						&mNavOptions,
						eventUPP,
						previewUPP,
						objectFilterUPP,
						inFileTypes.TypeListHandle(),
						0L);							// User Data

	UDesktop::Activate();

	if ( (err != noErr) && (err != userCanceledErr) ) {
		Throw_(err);
	}

	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseOneFile								  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileChooser::AskChooseOneFile(
	const LFileTypeList&	inFileTypes,
	FSSpec&					outFileSpec)
{
									// Create UPPs for callback functions
	StNavEventUPP			eventUPP(NavEventProc);
	StNavObjectFilterUPP	objectFilterUPP(mNavFilterProc);
	StNavPreviewUPP			previewUPP(mNavPreviewProc);

	mNavReply.SetDefaultValues();

									// Can choose only one file
	mNavOptions.dialogOptionFlags &= ~kNavAllowMultipleFiles;

									// Set default location, the location
									//   that's displayed when the dialog
									//   first appears
	AEDesc*		defaultLocationDesc = nil;
	if (not mDefaultLocation.IsNull()) {
		defaultLocationDesc = mDefaultLocation;

		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}

	UDesktop::Deactivate();

	OSErr err = ::NavChooseFile(
						defaultLocationDesc,
						mNavReply,
						&mNavOptions,
						eventUPP,
						previewUPP,
						objectFilterUPP,
						inFileTypes.TypeListHandle(),
						0L);							// User Data

	UDesktop::Activate();

	if ( (err != noErr) && (err != userCanceledErr) ) {
		Throw_(err);
	}

	if (mNavReply.IsValid()) {
		mNavReply.GetFileSpec(outFileSpec);
	}

	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseFolder									  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileChooser::AskChooseFolder(
	FSSpec&			outFileSpec,
	SInt32&			outFolderDirID)
{
									// Create UPPs for callback functions
	StNavEventUPP			eventUPP(NavEventProc);
	StNavObjectFilterUPP	objectFilterUPP(mNavFilterProc);

	mNavReply.SetDefaultValues();

									// Set default location, the location
									//   that's displayed when the dialog
									//   first appears
	AEDesc*		defaultLocationDesc = nil;
	if (not mDefaultLocation.IsNull()) {
		defaultLocationDesc = mDefaultLocation;

		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}

	UDesktop::Deactivate();

	OSErr err = ::NavChooseFolder(
						defaultLocationDesc,
						mNavReply,
						&mNavOptions,
						eventUPP,
						objectFilterUPP,
						0L);							// User Data

	UDesktop::Activate();

	if ( (err != noErr) && (err != userCanceledErr) ) {
		Throw_(err);
	}

	if (mNavReply.IsValid()) {		// User chose a folder

		FSSpec	folderInfo;
		mNavReply.GetFileSpec(folderInfo);

			// The FSSpec from NavChooseFolder is NOT the file spec
			// for the folder. The parID field is actually the DirID
			// of the folder itself, not the folder's parent, and
			// the name field is empty. We call FSMakeFSSpec() using those
			// value to create an FSSpec for the folder itself.

		outFolderDirID = folderInfo.parID;

		::FSMakeFSSpec(folderInfo.vRefNum, folderInfo.parID, folderInfo.name,
						&outFileSpec);
	}

	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseVolume									  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileChooser::AskChooseVolume(
	FSSpec&		outFileSpec)
{
									// Create UPPs for callback functions
	StNavEventUPP			eventUPP(NavEventProc);
	StNavObjectFilterUPP	objectFilterUPP(mNavFilterProc);

	mNavReply.SetDefaultValues();

	AEDesc*		defaultLocationDesc = nil;
	if (not mDefaultLocation.IsNull()) {
		defaultLocationDesc = mDefaultLocation;

			// mSelectDefault should always be true when selecting
			// volumes since we can't navigate into anything

		mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
	}

	UDesktop::Deactivate();

	OSErr err = ::NavChooseVolume(
						defaultLocationDesc,
						mNavReply,
						&mNavOptions,
						eventUPP,
						objectFilterUPP,
						0L);							// User Data

	UDesktop::Activate();

	if (mNavReply.IsValid()) {		// User chose a volume

		FSSpec	volumeInfo;
		mNavReply.GetFileSpec(volumeInfo);

			// The FSSpec from NavChooseFolder is NOT the file spec
			// for the volume. The parID field is actually the DirID
			// of the volume itself, not the volumes's parent, and
			// the name field is empty. We must call PBGetCatInfo
			// to get the parent DirID and volume name

		Str255		name;
		CInfoPBRec	thePB;			// Directory Info Parameter Block
		thePB.dirInfo.ioCompletion	= nil;
		thePB.dirInfo.ioVRefNum		= volumeInfo.vRefNum;	// Volume is right
		thePB.dirInfo.ioDrDirID		= volumeInfo.parID;		// Volumes's DirID
		thePB.dirInfo.ioNamePtr		= name;
		thePB.dirInfo.ioFDirIndex	= -1;	// Lookup using Volume and DirID

		err = ::PBGetCatInfoSync(&thePB);
		ThrowIfOSErr_(err);

											// Create cannonical FSSpec
		::FSMakeFSSpec(thePB.dirInfo.ioVRefNum, thePB.dirInfo.ioDrParID,
					   name, &outFileSpec);
	}

	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::IsValid											  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileChooser::IsValid() const
{
	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetNumberOfFiles								  [public]
// ---------------------------------------------------------------------------

SInt32
UNavServicesDialogs::LFileChooser::GetNumberOfFiles() const
{
	SInt32	numFiles = 0;

	if (mNavReply.IsValid()) {
		AEDescList	selectedItems = mNavReply.Selection();
		OSErr	err = ::AECountItems(&selectedItems, &numFiles);

		ThrowIfOSErr_(err);
	}

	return numFiles;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileSpec										  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileChooser::GetFileSpec(
	SInt32		inIndex,
	FSSpec&		outFileSpec) const
{
	AEKeyword	theKey;
	DescType	theType;
	Size		theSize;

	AEDescList	selectedItems = mNavReply.Selection();
	OSErr err = ::AEGetNthPtr(&selectedItems, inIndex, typeFSS,
						&theKey, &theType, (Ptr) &outFileSpec,
						sizeof(FSSpec), &theSize);

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileDescList									  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileChooser::GetFileDescList(
	AEDescList&		outDescList) const
{
	outDescList = mNavReply.Selection();
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetScriptCode									  [public]
// ---------------------------------------------------------------------------

ScriptCode
UNavServicesDialogs::LFileChooser::GetScriptCode() const
{
	return mNavReply.KeyScript();
}

#pragma mark -

// ===========================================================================
//	LFileDesignator Class
// ===========================================================================
//	Uses Navigation Services to ask user to save a file

// ---------------------------------------------------------------------------
//	¥ LFileDesignator::LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::LFileDesignator::LFileDesignator()
{
	::NavGetDefaultDialogOptions(&mNavOptions);

	::GetIndString( mNavOptions.windowTitle,
					STRx_Standards, str_SaveDialogTitle);

	mFileType	 = FOUR_CHAR_CODE('\?\?\?\?');
	mFileCreator = LFileTypeList::GetProcessSignature();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::~LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UNavServicesDialogs::LFileDesignator::~LFileDesignator()
{
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileType									  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileDesignator::SetFileType(
	OSType		inFileType)
{
	mFileType = inFileType;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileCreator								  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileDesignator::SetFileCreator(
	OSType		inFileCreator)
{
	mFileCreator = inFileCreator;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------

NavDialogOptions*
UNavServicesDialogs::LFileDesignator::GetDialogOptions()
{
	return &mNavOptions;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetDefaultLocation							  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileDesignator::SetDefaultLocation(
	const FSSpec&	inFileSpec,
	bool			inSelectIt)
{
	mDefaultLocation = inFileSpec;
	mSelectDefault	 = inSelectIt;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::AskDesignateFile								  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileDesignator::AskDesignateFile(
	ConstStringPtr	inDefaultName)
{
	StNavEventUPP		eventUPP(NavEventProc);

	LString::CopyPStr(inDefaultName, mNavOptions.savedFileName);

	mNavReply.SetDefaultValues();

	AEDesc*		defaultLocationDesc = nil;
	if (not mDefaultLocation.IsNull()) {
		defaultLocationDesc = mDefaultLocation;

		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}

	UDesktop::Deactivate();

	OSErr err = ::NavPutFile(
						defaultLocationDesc,
						mNavReply,
						&mNavOptions,
						eventUPP,
						mFileType,
						mFileCreator,
						0L);					// User Data

	UDesktop::Activate();

	if ( (err != noErr) && (err != userCanceledErr) ) {
		Throw_(err);
	}

	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsValid										  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileDesignator::IsValid() const
{
	return mNavReply.IsValid();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetFileSpec									  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileDesignator::GetFileSpec(
	FSSpec&		outFileSpec) const
{
	mNavReply.GetFileSpec(outFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsReplacing									  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileDesignator::IsReplacing() const
{
	return mNavReply.IsReplacing();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsStationery									  [public]
// ---------------------------------------------------------------------------

bool
UNavServicesDialogs::LFileDesignator::IsStationery() const
{
	return mNavReply.IsStationery();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetScriptCode								  [public]
// ---------------------------------------------------------------------------

ScriptCode
UNavServicesDialogs::LFileDesignator::GetScriptCode() const
{
	return mNavReply.KeyScript();
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::CompleteSave									  [public]
// ---------------------------------------------------------------------------

void
UNavServicesDialogs::LFileDesignator::CompleteSave(
	NavTranslationOptions	inOption)
{
	if (mNavReply.IsValid()) {
		::NavCompleteSave(mNavReply, inOption);
	}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ NavEventProc													  [static]
// ---------------------------------------------------------------------------
//	Event filter callback routine for Navigation Services

pascal void
UNavServicesDialogs::NavEventProc(
	NavEventCallbackMessage		inSelector,
	NavCBRecPtr					ioParams,
	NavCallBackUserData			/* ioUserData */)
{
	if (inSelector == kNavCBEvent) {
		try {
			UModalAlerts::ProcessModalEvent(*(ioParams->eventData.eventDataParms.event));
		}

		catch (...) { }			// Can't throw back through the Toolbox
	}
}


// ===========================================================================
//	StNavEventUPP Class
// ===========================================================================

UNavServicesDialogs::StNavEventUPP::StNavEventUPP(
	NavEventProcPtr		inProcPtr)
{
	mNavEventUPP = nil;

	if (inProcPtr != nil) {
		mNavEventUPP = NewNavEventUPP(inProcPtr);
	}
}


UNavServicesDialogs::StNavEventUPP::~StNavEventUPP()
{
	if (mNavEventUPP != nil) {
		DisposeNavEventUPP(mNavEventUPP);
	}
}

#pragma mark -
// ===========================================================================
//	StNavObjectFilterUPP Class
// ===========================================================================

UNavServicesDialogs::StNavObjectFilterUPP::StNavObjectFilterUPP(
	NavObjectFilterProcPtr		inProcPtr)
{
	mNavObjectFilterUPP = nil;

	if (inProcPtr != nil) {
		mNavObjectFilterUPP = NewNavObjectFilterUPP(inProcPtr);
	}
}


UNavServicesDialogs::StNavObjectFilterUPP::~StNavObjectFilterUPP()
{
	if (mNavObjectFilterUPP != nil) {
		DisposeNavObjectFilterUPP(mNavObjectFilterUPP);
	}
}

#pragma mark -
// ===========================================================================
//	StNavPreviewUPP Class
// ===========================================================================

UNavServicesDialogs::StNavPreviewUPP::StNavPreviewUPP(
	NavPreviewProcPtr	inProcPtr)
{
	mNavPreviewUPP = nil;

	if (inProcPtr != nil) {
		mNavPreviewUPP = NewNavPreviewUPP(inProcPtr);
	}
}


UNavServicesDialogs::StNavPreviewUPP::~StNavPreviewUPP()
{
	if (mNavPreviewUPP != nil) {
		DisposeNavPreviewUPP(mNavPreviewUPP);
	}
}


PP_End_Namespace_PowerPlant
