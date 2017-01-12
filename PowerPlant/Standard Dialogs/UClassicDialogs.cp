// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UClassicDialogs.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes which use StandardFile dialogs to prompt the user to open
//	and save files

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UClassicDialogs.h>

#if TARGET_API_MAC_CARBON

		// You must use UNavServicesDialogs under Carbon.
		// Remove this file from Carbon targets.

	#error "Carbon does not support StandardFile (Classic Dialogs)"

#endif

#include <LString.h>
#include <PP_KeyCodes.h>
#include <PP_Resources.h>
#include <UDesktop.h>
#include <UEnvironment.h>
#include <UModalDialogs.h>
#include <UStandardFile.h>

#include <Aliases.h>
#include <LowMem.h>
#include <Script.h>

PP_Begin_Namespace_PowerPlant


namespace UClassicDialogs {

// ---------------------------------------------------------------------------
//	Constants

	const SInt16	item_SelectDirButton	= 10;
	const SInt16	item_NewFolderButton	= 12;
	const SInt16	CNTL_AMNewFolderButton	= 600;
	const SInt16	CNTL_StdNewFolderButton	= 601;
	const UInt8		max_FolderNameLength	= 31;
	const char		replaceBy_dirName		= char_Caret;
	const Point		where_Default			= { -1, -1 };

	const SInt16	DLOG_NewFolder			= 601;
	const SInt16	item_FolderName			= 3;


// ---------------------------------------------------------------------------
//	Static Variables

	static FSSpec	sLastSelection;		// Selected Directory
	static Str63	sCurrentName;		// Name in "Select" button
	static Str255	sSavedString;		// Original "Select" button name
}


// ---------------------------------------------------------------------------
//	¥ Load															  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::Load()
{
}		// StandardFile doesn't need loading


// ---------------------------------------------------------------------------
//	¥ Unload														  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::Unload()
{
}		// StandardFile doesn't need unloading

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskSaveChanges												  [public]
// ---------------------------------------------------------------------------

SInt16
UClassicDialogs::AskSaveChanges(
	ConstStringPtr	inDocumentName,
	ConstStringPtr	inAppName,
	bool			inQuitting)
{
	::ParamText(inAppName, inDocumentName, Str_Empty, Str_Empty);

	ResIDT	alertID = ALRT_SaveChangesClosing;
	if (inQuitting) {
		alertID = ALRT_SaveChangesQuitting;
	}

	return UModalAlerts::CautionAlert(alertID);
}


// ---------------------------------------------------------------------------
//	¥ AskConfirmRevert												  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::AskConfirmRevert(
	ConstStringPtr	inDocumentName)
{
	::ParamText(inDocumentName, Str_Empty, Str_Empty, Str_Empty);

	return ( UModalAlerts::CautionAlert(ALRT_ConfirmRevert)
			 == kStdOkItemIndex);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskOpenOneFile												  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::AskOpenOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	/* inFlags */)		// Flags not used
{
	LFileChooser	chooser;

	bool	openOK = chooser.AskOpenFile( LFileTypeList(inFileType) );

	if (openOK) {
		chooser.GetFileSpec(1, outFileSpec);
	}

	return openOK;
}


// ---------------------------------------------------------------------------
//	¥ AskChooseOneFile												  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::AskChooseOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags)
{
		// With StandardFile, there is no difference between choosing
		// a file and selecting a file to open

	return UClassicDialogs::AskOpenOneFile(inFileType,
											outFileSpec, inFlags);
}


// ---------------------------------------------------------------------------
//	¥ AskChooseFolder												  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::AskChooseFolder(
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
UClassicDialogs::AskChooseVolume(
	FSSpec&					outFileSpec)
{
	LFileChooser	chooser;

	return chooser.AskChooseVolume(outFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ AskSaveFile													  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::AskSaveFile(
	ConstStringPtr			inDefaultName,
	OSType					/* inFileType */,	// File type doesn't matter
	FSSpec&					outFileSpec,
	bool&					outReplacing,
	NavDialogOptionFlags	/* inFlags */)		// Flags not used
{
	LFileDesignator	designator;

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
//	Uses StandardFile to ask user to open a file

// ---------------------------------------------------------------------------
//	Class Variables

NavObjectFilterProcPtr	UClassicDialogs::LFileChooser::sFilterProc = nil;
bool					UClassicDialogs::LFileChooser::sChoosingFolder = false;
bool					UClassicDialogs::LFileChooser::sChoosingVolume = false;


// ---------------------------------------------------------------------------
//	¥ LFileChooser::LFileChooser			Constructor				  [public]
// ---------------------------------------------------------------------------

UClassicDialogs::LFileChooser::LFileChooser()
{
	mSFReply.sfGood			 = false;
	mSFReply.sfScript		 = smSystemScript;
	mSFReply.sfFlags		 = 0;

	mNavFilterProc			 = nil;

	mDefaultLocation.name[0] = 0;
	mSelectDefault			 = false;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::~LFileChooser			Destructor				  [public]
// ---------------------------------------------------------------------------

UClassicDialogs::LFileChooser::~LFileChooser()
{
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------

NavDialogOptions*
UClassicDialogs::LFileChooser::GetDialogOptions()
{
	return nil;			// StandardFile doesn't have dialog options
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
UClassicDialogs::LFileChooser::SetDefaultLocation(
	const FSSpec&	inFileSpec,
	bool			inSelectIt)
{
	mDefaultLocation = inFileSpec;
	mSelectDefault	 = inSelectIt;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetObjectFilterProc								  [public]
// ---------------------------------------------------------------------------
//	Specify the file filter callback function
//
//	Note that the callback function is a NavServices object filter, whose
//	declaration is:
//
//		pascal Boolean MyNavObjectFilter(
//			AEDesc*			inItem,
//			void*			inInfo,
//			void*			inUserData,
//			NavFilterModes	inFilterMode)
//
//	Since UClassicDialogs uses StandardFile, it installs a StandardFile
//	filter function which calls the NavServices object filter, passing a
//	CInfoPBPtr as the inUserData parameter, and nil for inItem and inInfo.
//
//	Also, a NavServices object filter returns true to display the item, and
//	false to hide it. This is the opposite of the return value for a
//	StandardFile filter.
//
//	In order for your object filter to work with both StandardFile and
//	NavServices, you will need to check for nil parameters as follows:
//
//		{
//			if (inItem != nil) {				// NavServices Callback
//
//				// Get info about item from inItem and inInfo
//
//			} else if (inUserData != nil) {		// StandardFile Callback
//
//				CInfoPBPtr thePB = (CInfoPBPtr) inUserData;
//			}
//		}

void
UClassicDialogs::LFileChooser::SetObjectFilterProc(
	NavObjectFilterProcPtr	inFilterProc)
{
	mNavFilterProc = inFilterProc;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetPreviewProc									  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileChooser::SetPreviewProc(
	NavPreviewProcPtr	/* inPreviewProc */)
{
}	// No StandardFile support for previews


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskOpenFile										  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileChooser::AskOpenFile(
	const LFileTypeList&	inFileTypes)
{
											// File Filter function
	StRoutineDescriptor<FileFilterYDUPP>	fileFilterUPP;

	if (mNavFilterProc != nil) {
		fileFilterUPP.SetUPP(NewFileFilterYDProc(FileFilter_NavServicesAPI));
	}

	StRoutineDescriptor<DlgHookYDUPP>		// Dialog Hook function
			dialogHook(NewDlgHookYDProc(
					DialogHook_DefaultLocation));

	StRoutineDescriptor<ModalFilterYDUPP>	// Event Filter function
			eventUPP(NewModalFilterYDProc(
					UModalAlerts::GetStdFileEventFilter()));

	SHookUserData	userData;			// User data for hook function
	userData.reply			= &mSFReply;
	userData.defaultSpec	= &mDefaultLocation;
	userData.selectDefault	= mSelectDefault;

	UDesktop::Deactivate();

	sFilterProc		= mNavFilterProc;
	sChoosingFolder = false;
	sChoosingVolume = false;

	::CustomGetFile(
			fileFilterUPP,
			inFileTypes.NumberOfTypes(),
			inFileTypes.TypeListPtr(),
			&mSFReply,
			0,							// Dialog ID (0 = use default)
			where_Default,
			dialogHook,
			eventUPP,
			nil,						// Activation List
			nil,						// Activate Proc
			&userData);

	UDesktop::Activate();

	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseOneFile								  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileChooser::AskChooseOneFile(
	const LFileTypeList&	inFileTypes,
	FSSpec&					outFileSpec)
{
	bool	goodChoice = AskOpenFile(inFileTypes);

	if (goodChoice) {
		outFileSpec = mSFReply.sfFile;
	}

	return goodChoice;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseFolder									  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileChooser::AskChooseFolder(
	FSSpec&			outFileSpec,
	SInt32&			outFolderDirID)
{
	OSType		fileTypes;

	StRoutineDescriptor<FileFilterYDUPP>	// File Filter function
				fileFilterUPP(NewFileFilterYDProc(FileFilter_NavServicesAPI));

	StRoutineDescriptor<DlgHookYDUPP>		// Dialog Hook function
			dlgHookUPP(NewDlgHookYDProc(DialogHook_SelectDirectory));

	StRoutineDescriptor<ModalFilterYDUPP>	// Event Filter function
			eventUPP(NewModalFilterYDProc(
					UModalAlerts::GetStdFileEventFilter()));

	SHookUserData	userData;			// User data for hook function
	userData.reply			= &mSFReply;
	userData.defaultSpec	= &mDefaultLocation;
	userData.selectDefault	= mSelectDefault;

	UDesktop::Deactivate();

	sFilterProc		= mNavFilterProc;
	sChoosingFolder = true;
	sChoosingVolume = false;

	::CustomGetFile(
			fileFilterUPP,
			-1,							// Number of types (-1 = all types)
			&fileTypes,
			&mSFReply,
			DLOG_GetDirectory,
			where_Default,
			dlgHookUPP,
			eventUPP,
			nil,						// Activation List
			nil,						// Activation Proc
			&userData);

	UDesktop::Activate();

	if (mSFReply.sfGood) {
		outFileSpec = mSFReply.sfFile;

									// Get the DirID of the folder

		CInfoPBRec	thePB;			// Directory Info Parameter Block
		thePB.dirInfo.ioCompletion	= nil;
		thePB.dirInfo.ioVRefNum		= mSFReply.sfFile.vRefNum;
		thePB.dirInfo.ioDrDirID		= mSFReply.sfFile.parID;
		thePB.dirInfo.ioNamePtr		= mSFReply.sfFile.name;
		thePB.dirInfo.ioFDirIndex	= 0;

		OSErr err = ::PBGetCatInfoSync(&thePB);
		ThrowIfOSErr_(err);

		outFolderDirID = thePB.dirInfo.ioDrDirID;
	}

	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseVolume									  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileChooser::AskChooseVolume(
	FSSpec&			outFileSpec)
{
	OSType		fileTypes;

	StRoutineDescriptor<FileFilterYDUPP>	// File Filter function
				fileFilterUPP(NewFileFilterYDProc(FileFilter_NavServicesAPI));

	StRoutineDescriptor<DlgHookYDUPP>		// Dialog Hook function
			dlgHookUPP(NewDlgHookYDProc(DialogHook_SelectVolume));

	StRoutineDescriptor<ModalFilterYDUPP>	// Event Filter function
			eventUPP(NewModalFilterYDProc(
					UModalAlerts::GetStdFileEventFilter()));

	SHookUserData	userData;			// User data for hook function
	userData.reply			= &mSFReply;
	userData.defaultSpec	= &mDefaultLocation;
	userData.selectDefault	= mSelectDefault;

	UDesktop::Deactivate();

	sFilterProc		= mNavFilterProc;
	sChoosingFolder = false;
	sChoosingVolume = true;

	::CustomGetFile(
			fileFilterUPP,
			-1,							// Number of types (-1 = all types)
			&fileTypes,
			&mSFReply,
			0,							// Dialog ID (0 = use default)
			where_Default,
			dlgHookUPP,
			eventUPP,
			nil,						// Activation List
			nil,						// Activation Proc
			&userData);

	UDesktop::Activate();

	if (mSFReply.sfGood) {
		outFileSpec = mSFReply.sfFile;
	}

	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::IsValid											  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileChooser::IsValid() const
{
	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetNumberOfFiles								  [public]
// ---------------------------------------------------------------------------

SInt32
UClassicDialogs::LFileChooser::GetNumberOfFiles() const
{
	SInt32	numFiles = 0;

	if (mSFReply.sfGood) {
		numFiles = 1;
	}

	return numFiles;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileSpec										  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileChooser::GetFileSpec(
	SInt32		inIndex,
	FSSpec&		outFileSpec) const
{
	ThrowIf_(inIndex != 1);

	ThrowIfNot_(mSFReply.sfGood);

	outFileSpec = mSFReply.sfFile;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileDescList									  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileChooser::GetFileDescList(
	AEDescList&		outDescList) const
{
	ThrowIfNot_(mSFReply.sfGood);

	OSErr	err = ::AECreateList(nil, 0, false, mFileDescList);
	ThrowIfOSErr_(err);

	err = ::AEPutPtr(mFileDescList, 1, typeFSS, &mSFReply.sfFile,
						sizeof(FSSpec));
	ThrowIfOSErr_(err);

	outDescList = mFileDescList;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetScriptCode									  [public]
// ---------------------------------------------------------------------------

ScriptCode
UClassicDialogs::LFileChooser::GetScriptCode() const
{
	return mSFReply.sfScript;
}



// ---------------------------------------------------------------------------
//	¥ FileFilter_NavServicesAPI
// ---------------------------------------------------------------------------
//	StandardFile file filter function that is a wrapper for a NavServices
//	API object filter function

pascal Boolean
UClassicDialogs::LFileChooser::FileFilter_NavServicesAPI(
	CInfoPBPtr	inPB,
	void*		/* inDataPtr */)
{
	Boolean		displayItem = true;

		// If we are choosing folders or volumes, perform first level
		// filter to see if item is a folder or volume

	if (sChoosingFolder) {

		displayItem = ((inPB->dirInfo.ioFlAttrib & ioDirMask) != 0);

	} else if (sChoosingVolume) {

		displayItem = ((inPB->dirInfo.ioFlAttrib & ioDirMask) != 0)  &&
			 		  (inPB->dirInfo.ioDrParID == fsRtParID);
	}

		// Call NavServices API filter function, passing the CInfoPBPtr
		// as the user data parameter.

	if (displayItem  &&  (sFilterProc != nil)) {
		displayItem =
				(*sFilterProc)(nil, nil, inPB, kNavFilteringBrowserList);
	}

		// StandardFile filters return false to display the item, and
		// true to hide the item.
		//
		// NavServices filters work oppositely, returning true to display
		// the item, and false to hide the item.

	return (not displayItem);
}


#pragma mark -

// ===========================================================================
//	LFileDesignator Class
// ===========================================================================
//	Uses StandardFile to ask user to save a file

// ---------------------------------------------------------------------------
//	¥ LFileDesignator::LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UClassicDialogs::LFileDesignator::LFileDesignator()
{
	mSFReply.sfGood			 = false;
	mSFReply.sfScript		 = smSystemScript;
	mSFReply.sfFlags		 = 0;

	mDefaultLocation.name[0] = 0;
	mSelectDefault			 = false;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::~LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UClassicDialogs::LFileDesignator::~LFileDesignator()
{
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileType									  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileDesignator::SetFileType(
	OSType		/* inFileType */)
{
}		// StandardFile doesn't care about the file type


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileCreator								  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileDesignator::SetFileCreator(
	OSType		/* inFileCreator */)
{
}		// StandardFile doesn't care about the file creator


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------

NavDialogOptions*
UClassicDialogs::LFileDesignator::GetDialogOptions()
{
	return nil;		// StandardFile doesn't have dialog options
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetDefaultLocation							  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileDesignator::SetDefaultLocation(
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
UClassicDialogs::LFileDesignator::AskDesignateFile(
	ConstStringPtr	inDefaultName)
{
	StRoutineDescriptor<DlgHookYDUPP>		// Our Hook function
			dialogHook(NewDlgHookYDProc(DialogHook_DefaultLocation));

	StRoutineDescriptor<ModalFilterYDUPP>	// Event Filter function
			eventUPP(NewModalFilterYDProc(
					UModalAlerts::GetStdFileEventFilter()));

	SHookUserData	userData;			// User data for hook function
	userData.reply			= &mSFReply;
	userData.defaultSpec	= &mDefaultLocation;
	userData.selectDefault	= mSelectDefault;

	UDesktop::Deactivate();

	::CustomPutFile(
			LStr255(STRx_Standards, str_SaveAs),
			inDefaultName,
			&mSFReply,
			0,					// Dialog ID (0 = use default)
			where_Default,
			dialogHook,
			eventUPP,
			nil,				// Activation List
			nil,				// Activate Proc
			&userData);

	UDesktop::Activate();

	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsValid										  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileDesignator::IsValid() const
{
	return mSFReply.sfGood;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetFileSpec									  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileDesignator::GetFileSpec(
	FSSpec&		outFileSpec) const
{
	ThrowIfNot_(mSFReply.sfGood);

	outFileSpec = mSFReply.sfFile;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsReplacing									  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileDesignator::IsReplacing() const
{
	return mSFReply.sfReplacing;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsStationery									  [public]
// ---------------------------------------------------------------------------

bool
UClassicDialogs::LFileDesignator::IsStationery() const
{
	return false;		// No built-in stationery option for Standard File
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetScriptCode								  [public]
// ---------------------------------------------------------------------------

ScriptCode
UClassicDialogs::LFileDesignator::GetScriptCode() const
{
	return mSFReply.sfScript;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::CompleteSave									  [public]
// ---------------------------------------------------------------------------

void
UClassicDialogs::LFileDesignator::CompleteSave(
	NavTranslationOptions	/* inOption */)
{
}		// StandardFile doesn't perform translation

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FileFilter_Directories
// ---------------------------------------------------------------------------
//	Screens out everything except directories (folders and volumes)

pascal Boolean
UClassicDialogs::FileFilter_Directories(
	CInfoPBPtr	inPB,
	void*		/* inDataPtr */)
{
	return ((inPB->dirInfo.ioFlAttrib & ioDirMask) == 0);
}


// ---------------------------------------------------------------------------
//	¥ DialogHook_SelectDirectory
// ---------------------------------------------------------------------------
//	Handles a "Select Directory" button in the custom standard get file
//	dialog box.
//
//	The item number of the "Select Directory" button is item_SelectDirButton.

pascal short
UClassicDialogs::DialogHook_SelectDirectory(
	short		inItem,
	DialogPtr	inDialog,
	void*		ioDataPtr)			// Ptr to StandardFileReply
{
									// This Hook is only for the main
									//   dialog box (not a sub dialog)
	if (::GetWRefCon(inDialog) != sfMainDialogRefCon) {
		return inItem;
	}

	SInt16		itemHit = inItem;

	CInfoPBRec	thePB;				// Directory Info Parameter Block
	thePB.dirInfo.ioCompletion = nil;

	SInt16	buttonType;				// Get Handle to "Set Directory" button
	Handle	buttonH;
	Rect	buttonRect;
	::GetDialogItem(inDialog, item_SelectDirButton, &buttonType, &buttonH,
				&buttonRect);

									// Get pointer the reply record
	StandardFileReply*	replyP = ((SHookUserData*) ioDataPtr)->reply;

	if (inItem == sfHookFirstCall) {
									// Set default location on first call
		itemHit = DialogHook_DefaultLocation(inItem, inDialog, ioDataPtr);

									// Make appropriate "New Folder" button

									// Get location of placeholder user item
		SInt16	folderButtonType;
		ControlHandle	folderButtonH;
		Rect	folderButtonRect;
		::GetDialogItem(inDialog, item_NewFolderButton, &folderButtonType,
				(Handle*) &folderButtonH, &folderButtonRect);

									// Use Std or AM button depending on
									//   whether Appearance Manager exists
		SInt16	folderButtonID = CNTL_StdNewFolderButton;

		if (UEnvironment::HasFeature(env_HasAppearance)) {
			folderButtonID = CNTL_AMNewFolderButton;
		}

		folderButtonH = ::GetNewControl(folderButtonID, inDialog);
		(**folderButtonH).contrlRect = folderButtonRect;

									// Replace user item with our button
		::SetDialogItem(inDialog, item_NewFolderButton,
						kResourceControlDialogItem,
						(Handle) folderButtonH, &folderButtonRect);

									// Save original button name
		::GetControlTitle((ControlHandle) buttonH, sSavedString);
		sCurrentName[0] = 0;

		if (itemHit != sfHookChangeSelection) {
									// Get current volume and directory
			replyP->sfFile.vRefNum = (SInt16) -::LMGetSFSaveDisk();
			replyP->sfFile.parID   = ::LMGetCurDirStore();
			replyP->sfFile.name[0] = 0;
		}
	}

	switch (inItem) {

		case sfHookLastCall:		// CustomGetFile is about to return
			if (replyP->sfGood == false) {
				break;				// User cancelled. Nothing to do.
			}
									// If item is an alias, resolve it to
									//   the "real" item
			Boolean	isFolder, wasAliased;
			::ResolveAliasFile(&sLastSelection, true, &isFolder, &wasAliased);

									// Get info about selected item
			thePB.dirInfo.ioVRefNum   = sLastSelection.vRefNum;
			thePB.dirInfo.ioDrDirID   = sLastSelection.parID;
			thePB.dirInfo.ioNamePtr   = sLastSelection.name;
			thePB.dirInfo.ioFDirIndex = 0;

			::PBGetCatInfoSync(&thePB);

									// Volumes have a special DirID
			replyP->sfIsVolume = (thePB.dirInfo.ioDrDirID == fsRtDirID);
									// Item is a folder if it isn't
									//   a volume
			replyP->sfIsFolder = !replyP->sfIsVolume;

				// Normally, directories use the system script. But if the
				// high bit of the frScript byte is 1, then the script code
				// is in the lower 7 bits.

			replyP->sfScript = smSystemScript;
			if ((thePB.dirInfo.ioDrFndrInfo.frScript & 0x80) != 0) {
				replyP->sfScript = (SInt16) (thePB.dirInfo.ioDrFndrInfo.frScript & 0x7F);
			}

									// Fill in FSSpec for the Directory
			::FSMakeFSSpec(sLastSelection.vRefNum, sLastSelection.parID,
							sLastSelection.name, &replyP->sfFile);
			break;

		case item_SelectDirButton:
									// User clicked our "Select Directory"
									//   button. Convert this to an "open"
									//   item so that the SF Package will
									//   know that the user has made a
									//   successful selection.
			itemHit = sfItemOpenButton;
			break;

		case item_NewFolderButton: {
									// User clicked our "New Folder" button.
									// Display dialog for entering folder
									// name, and make folder if we get a
									// good name.
			Str255	folderName;
			if (GetNewFolderName(folderName)) {
				FSSpec	folderSpec;
				OSErr	err = ::FSMakeFSSpec( replyP->sfFile.vRefNum,
											  replyP->sfFile.parID,
											  folderName,
											  &folderSpec);

				if (err == fnfErr) {	// Item must not already exist
					long	folderID;
					err = ::FSpDirCreate(&folderSpec, smSystemScript, &folderID);

					if (err == noErr) {
							// Set location to the new, empty folder

						replyP->sfFile.parID	= folderID;
						replyP->sfFile.name[0]	= 0;
						replyP->sfIsVolume		= false;
						replyP->sfIsFolder		= false;
						replyP->sfScript		= smSystemScript;

						itemHit = sfHookChangeSelection;
					}
				}
			}
			break;
		}

		default:
									// Copy FileSpec for current selection
			::BlockMoveData(&replyP->sfFile, &sLastSelection, sizeof(FSSpec));

			if (StrLength(sLastSelection.name) == 0) {

					// No item is selected, but the volume number and
					// dir ID in the sfFile field are those of the Directory
					// being viewed. Find the name and parent of this
					// Directory using these numbers.

				thePB.dirInfo.ioVRefNum = sLastSelection.vRefNum;
				thePB.dirInfo.ioDrDirID = sLastSelection.parID;
				thePB.dirInfo.ioFDirIndex = -1;
				thePB.dirInfo.ioNamePtr = sLastSelection.name;
				::PBGetCatInfoSync(&thePB);
				sLastSelection.parID = thePB.dirInfo.ioDrParID;
			}

									// If the name of the current item
									// has changed, update the title of
									// the Select button
			if (::IUEqualString(sLastSelection.name, sCurrentName) == 1) {

									// Save name for later comparison
				::BlockMoveData(sLastSelection.name, sCurrentName,
									StrLength(sLastSelection.name) + 1);

				SetSelectButtonTitle((ControlHandle) buttonH, buttonRect,
										sCurrentName);
			}
			break;
	}

	return itemHit;
}


// ---------------------------------------------------------------------------
//	¥ SetSelectButtonTitle									 [static] [public]
// ---------------------------------------------------------------------------
//	Sets the Title of the Button for selecting a Directory
//
//	Before using this function, initialize sSavedString to the text for
//	the Button title, using the carat (^) character as a place holder
//	for the directory name. Normally, you'll make this the Button title
//	in your custom dialog box.

void
UClassicDialogs::SetSelectButtonTitle(
	ControlHandle	inButtonH,
	const Rect&		inButtonRect,
	ConstStringPtr	inDirName)
{
									// Save directory name
	Str63	dirName;
	::BlockMoveData(inDirName, dirName, StrLength(inDirName) + 1);

									// Truncate name if necessary to fit
									//   inside the Button
	short	maxNameWidth = (short) ((inButtonRect.right - inButtonRect.left) -
							StringWidth(sSavedString) - CharWidth(' '));
	::TruncString(maxNameWidth, dirName, smTruncMiddle);

									// Insert (truncated) directory name
									//   into original button title
	Str255	buttonName;
	short	len = StrLength(sSavedString);
									// Copy original name up to (but not
									//   including) the special marker
	short	i = 1;
	while ((i <= len) && (sSavedString[i] != replaceBy_dirName)) {
		buttonName[i] = sSavedString[i];
		i++;
	}
									// Append directory name
	::BlockMoveData(dirName+1, buttonName+i, StrLength(dirName));
									// Append remaining part of original
									//   name (excluding the marker)
	short	j = (short) (i + StrLength(dirName) - 1);
	while (++i <= len) {
		buttonName[++j] = sSavedString[i];
	}
	buttonName[0] = (UInt8) j;		// Store length byte of Button name

									// Finally, we've got the Button name
	::SetControlTitle(inButtonH, buttonName);
	::ValidRect(&inButtonRect);
}


// ---------------------------------------------------------------------------
//	¥ GetNewFolderName
// ---------------------------------------------------------------------------

bool
UClassicDialogs::GetNewFolderName(
	Str255	outFolderName)
{
	bool	goodName = false;

	DialogPtr	theDialog = ::GetNewDialog(DLOG_NewFolder, nil, (WindowPtr) -1);

	if (theDialog != nil) {

		StRoutineDescriptor<ModalFilterUPP>
			filterUPP( NewModalFilterProc(EventFilter_NewFolder) );

		::SelectDialogItemText(theDialog, item_FolderName, 0, max_Int16);
		::SetDialogDefaultItem(theDialog, kStdOkItemIndex);

		::MacShowWindow(theDialog);

		DialogItemIndex	itemHit;

		do {
			::ModalDialog(filterUPP, &itemHit);

		} while ( (itemHit != kStdOkItemIndex)  &&
				  (itemHit != kStdCancelItemIndex) );

		if (itemHit == kStdOkItemIndex) {
			DialogItemType	itemType;
			Handle			itemHandle;
			Rect			itemRect;

			::GetDialogItem(theDialog, item_FolderName,
								&itemType, &itemHandle, &itemRect);

			::GetDialogItemText(itemHandle, outFolderName);

			goodName = (outFolderName[0] > 0);

			if (outFolderName[0] > max_FolderNameLength) {
				outFolderName[0] = max_FolderNameLength;
			}
		}

		::DisposeDialog(theDialog);
	}

	return goodName;
}


// ---------------------------------------------------------------------------
//	¥ EventFilter_NewFolder
// ---------------------------------------------------------------------------

pascal Boolean
UClassicDialogs::EventFilter_NewFolder(
	DialogPtr			inDialog,
	EventRecord*		inMacEvent,
	DialogItemIndex*	outItemHit)
{
		// Let default filter handle event first

	ModalFilterProcPtr	defaultFilter = UModalAlerts::GetModalEventFilter();

	Boolean	handled = (*defaultFilter)(inDialog, inMacEvent, outItemHit);

		// Disallow colons in folder name

		// ??? Limit length to 31 characters

	if ( !handled  &&  (inMacEvent->what == keyDown) ) {

		UInt16	theChar = (UInt16) (inMacEvent->message & charCodeMask);

		handled = (theChar == char_Colon);
	}

	return handled;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FileFilter_Volumes
// ---------------------------------------------------------------------------
//	Screens out everything except volumes

pascal Boolean
UClassicDialogs::FileFilter_Volumes(
	CInfoPBPtr	inPB,
	void*		/* inDataPtr */)
{
		// Volume is a directory whose parent is the root directory.
		// File Filter returns false if the item is a volume.

	return !( ((inPB->dirInfo.ioFlAttrib & ioDirMask) != 0)  &&
			  (inPB->dirInfo.ioDrParID == fsRtParID) );
}


// ---------------------------------------------------------------------------
//	¥ DialogHook_SelectVolume
// ---------------------------------------------------------------------------

pascal short
UClassicDialogs::DialogHook_SelectVolume(
	short		inItem,
	DialogPtr	inDialog,
	void*		ioDataPtr)			// SHookUserData*
{
									// This Hook is only for the main
									//   dialog box (not a sub dialog)
	if (::GetWRefCon(inDialog) != sfMainDialogRefCon) {
		return inItem;
	}

	short	itemHit = inItem;

	switch (inItem) {

		case sfHookFirstCall:		// Set default location on first call
			DialogHook_DefaultLocation(inItem, inDialog, ioDataPtr);
			itemHit = sfHookGoToDesktop;
			break;

		case sfHookGoToDesktop:		// Can't navigate out of top level
		case sfHookGoToNextDrive:
		case sfHookGoToPrevDrive:
			itemHit = sfHookNullEvent;
			break;

		case sfHookChangeSelection:	// All selections are on the desktop
			itemHit = sfHookGoToDesktop;
			break;

		case sfHookOpenFolder:		// "Opening" volume means select it
			itemHit = sfItemOpenButton;
			break;
	}

	return itemHit;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DialogHook_DefaultLocation
// ---------------------------------------------------------------------------

pascal short
UClassicDialogs::DialogHook_DefaultLocation(
	short		inItem,
	DialogPtr	inDialog,
	void*		ioDataPtr)			// SHookUserData*
{
									// This Hook is only for the main
									//   dialog box (not a sub dialog)
	if (::GetWRefCon(inDialog) != sfMainDialogRefCon) {
		return inItem;
	}

	short	itemHit = inItem;

	if (inItem == sfHookFirstCall) {	// Set default location on first call

									// Get pointers to our user data
		SHookUserData*		dataP		 = (SHookUserData*) ioDataPtr;
		StandardFileReply*	replyP		 = dataP->reply;
		FSSpec*				defaultSpecP = dataP->defaultSpec;

									// Don't set default location if our
									//    default file spec has no name
		if (defaultSpecP->name[0] != 0) {

				// Get information about the default location. It
				// could be a file or a folder.

			CInfoPBRec	thePB;

			thePB.hFileInfo.ioCompletion	= nil;
			thePB.hFileInfo.ioVRefNum		= defaultSpecP->vRefNum;
			thePB.hFileInfo.ioDirID			= defaultSpecP->parID;
			thePB.hFileInfo.ioNamePtr		= defaultSpecP->name;
			thePB.hFileInfo.ioFDirIndex		= 0;

			OSErr	err = ::PBGetCatInfoSync(&thePB);

			if (err != noErr) {		// Couldn't get info. Return without
				return inItem;		//   setting the default location.
			}

			if (thePB.hFileInfo.ioFlAttrib & ioDirMask) {
									// Default Location is a Directory
				if (dataP->selectDefault) {
									// Select the directory itself
					replyP->sfFile = *defaultSpecP;

				} else {			// Select first item in the Directory
					replyP->sfFile.vRefNum	= defaultSpecP->vRefNum;
					replyP->sfFile.parID	= thePB.dirInfo.ioDrDirID;
					replyP->sfFile.name[0]	= 0;
				}

									// If the directory is the root, it
									//   is a volume
				replyP->sfIsVolume	= (thePB.dirInfo.ioDrDirID == fsRtDirID);

									// If a directory isn't a volume, it
									//   is a folder
				replyP->sfIsFolder	= !replyP->sfIsVolume;

			} else {				// Default Lcoation is a File
				replyP->sfFile		= *defaultSpecP;
				replyP->sfIsVolume	= false;
				replyP->sfIsFolder	= false;
			}

				// Folders and Files names are normally in the System
				// Script. But if the high bit of the extended Finder
				// info script flag is set, then the script code is
				// in the lower 7 bits.

			replyP->sfScript	= smSystemScript;
			if ((thePB.dirInfo.ioDrFndrInfo.frScript & 0x80) != 0) {
				replyP->sfScript = (SInt16)
								(thePB.dirInfo.ioDrFndrInfo.frScript & 0x7F);
			}
									// Change itemHit to notify Toolbox
									//   that we changed the selection
			itemHit = sfHookChangeSelection;
		}
	}

	return itemHit;
}


PP_End_Namespace_PowerPlant
