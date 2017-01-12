// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UConditionalDialogs.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper classes which use Navigation Services or StandardFile dialogs
//	to prompt the user to open and save files. Use NavServices if available
//	(based on a runtime check), and StandardFile otherwise.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#pragma once off			// Need to multiply #include <UStandardDialogs.i>
							// and <LFileChooser.i>
#include <UConditionalDialogs.h>

#if TARGET_API_MAC_CARBON

		// You must use UNavServicesDialogs under Carbon.
		// Remove this file from Carbon targets.

	#error "Carbon does not StandardFile (Classic Dialogs)"

#endif

#include <UClassicDialogs.h>
#include <UNavServicesDialogs.h>
#include <UEnvironment.h>

#include <Navigation.h>
#include <Script.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Static Variables

namespace UConditionalDialogs {

	static bool		sTryNavServices		= true;
	static bool		sNavServicesChecked = false;
	static bool		sNavServicesExists	= false;
}


// ---------------------------------------------------------------------------
//	¥ UseNavServices												  [public]
// ---------------------------------------------------------------------------
//	Returns whether to use Navigation Services

bool
UConditionalDialogs::UseNavServices()
{
		// First time called, check if NavServices exists on this
		// machine. There is a bug in NS 1.0, where calling
		// NavServicesAvailable() more than once with the Appearance
		// Manager not present will cause a crash. So we check for
		// AM first (NS requires AM).

	if (!sNavServicesChecked) {
		sNavServicesExists = UEnvironment::HasFeature(env_HasAppearance) &&
							 ::NavServicesAvailable();
		sNavServicesChecked = true;
	}

		// Use NS if "try" flag is on and NS exists

	return (sTryNavServices && sNavServicesExists);
}


// ---------------------------------------------------------------------------
//	¥ SetTryNavServices												  [public]
// ---------------------------------------------------------------------------
//	Set whether or not to try using Navigation Services
//
//	If the version of NS is less than the specified minimum version,
//	this module will always use StandardFile.
//
//	The version number is a coded hexadecimal number of the form
//	0xWWXYRRZZ, where
//
//		WW is the major version
//		X  is the minor (dot) version
//		Y  is the bug fix version
//		RR is the release kind
//				20 = development (d)
//				40 = alpha (a)
//				60 = beta (b)
//				80 = final
//		ZZ is the release number
//
//	Examples:
//		43.2.1b56  is  0x43218056
//		1.0        is  0x01008000
//
//	Pass 0 to always use NS if it is available (the first version of
//	NS was 1.0). Conversely, pass a very big number (0xFFFFFFFF) to
//	never use NS.
//
//	Many users have experienced problems with NS 1.0, but NS 1.1
//	that ships with Mac OS 8.5 is better. The default for this module is
//	to use any version of NS. If you experience problems, you might
//	want to require NS 1.1 (0x01108000).

void
UConditionalDialogs::SetTryNavServices(
	UInt32	inMinVersion)
{
	UseNavServices();

	if (sNavServicesExists) {
		UInt32	nsVersion = ::NavLibraryVersion();
		sTryNavServices = nsVersion >= inMinVersion;
	}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Load															  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::Load()
{
	if (UseNavServices()) {
		UNavServicesDialogs::Load();

	} else {
		UClassicDialogs::Load();
	}
}


// ---------------------------------------------------------------------------
//	¥ Unload														  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::Unload()
{
	if (UseNavServices()) {
		UNavServicesDialogs::Unload();

	} else {
		UClassicDialogs::Unload();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskSaveChanges												  [public]
// ---------------------------------------------------------------------------

SInt16
UConditionalDialogs::AskSaveChanges(
	ConstStringPtr	inDocumentName,
	ConstStringPtr	inAppName,
	bool			inQuitting)
{
	SInt16	answer;

	if (UseNavServices()) {
		answer =  UNavServicesDialogs::AskSaveChanges(inDocumentName,
											inAppName, inQuitting);

	} else {
		answer =  UClassicDialogs::AskSaveChanges(inDocumentName,
											inAppName, inQuitting);
	}

	return answer;
}


// ---------------------------------------------------------------------------
//	¥ AskConfirmRevert												  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskConfirmRevert(
	ConstStringPtr	inDocumentName)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskConfirmRevert(inDocumentName);

	} else {
		answer = UClassicDialogs::AskConfirmRevert(inDocumentName);
	}

	return answer;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AskOpenOneFile												  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskOpenOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskOpenOneFile(
										inFileType, outFileSpec, inFlags);

	} else {
		answer = UClassicDialogs::AskOpenOneFile(
										inFileType, outFileSpec, inFlags);
	}

	return answer;
}


// ---------------------------------------------------------------------------
//	¥ AskChooseOneFile												  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskChooseOneFile(
	OSType					inFileType,
	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskChooseOneFile(
										inFileType, outFileSpec, inFlags);

	} else {
		answer = UClassicDialogs::AskChooseOneFile(
										inFileType, outFileSpec, inFlags);
	}

	return answer;
}


// ---------------------------------------------------------------------------
//	¥ AskChooseFolder												  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskChooseFolder(
	FSSpec&					outFileSpec,
	SInt32&					outFolderDirID)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskChooseFolder(
											outFileSpec, outFolderDirID);

	} else {
		answer = UClassicDialogs::AskChooseFolder(
											outFileSpec, outFolderDirID);
	}

	return answer;
}


// ---------------------------------------------------------------------------
//	¥ AskChooseVolume												  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskChooseVolume(
	FSSpec&					outFileSpec)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskChooseVolume(outFileSpec);

	} else {
		answer = UClassicDialogs::AskChooseVolume(outFileSpec);
	}

	return answer;
}


// ---------------------------------------------------------------------------
//	¥ AskSaveFile													  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::AskSaveFile(
	ConstStringPtr			inDefaultName,
	OSType					inFileType,
	FSSpec&					outFileSpec,
	bool&					outReplacing,
	NavDialogOptionFlags	inFlags)
{
	bool	answer;

	if (UseNavServices()) {
		answer = UNavServicesDialogs::AskSaveFile(inDefaultName,
							inFileType, outFileSpec, outReplacing, inFlags);

	} else {
		answer = UClassicDialogs::AskSaveFile(inDefaultName,
							inFileType, outFileSpec, outReplacing, inFlags);
	}

	return answer;
}

#pragma mark -

// ===========================================================================
//	LFileChooser Class
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LFileChooser::LFileChooser			Constructor				  [public]
// ---------------------------------------------------------------------------

UConditionalDialogs::LFileChooser::LFileChooser()
{
	mClassicChooser		= nil;
	mNavServicesChooser = nil;

	if (UseNavServices()) {
		mNavServicesChooser = new UNavServicesDialogs::LFileChooser;

	} else {
		mClassicChooser = new UClassicDialogs::LFileChooser;
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::~LFileChooser			Destructor				  [public]
// ---------------------------------------------------------------------------

UConditionalDialogs::LFileChooser::~LFileChooser()
{
	delete mClassicChooser;
	delete mNavServicesChooser;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------
//	The returned NavDialogOptions* may be nil. Check for this before setting
//	fields of the struct.

NavDialogOptions*
UConditionalDialogs::LFileChooser::GetDialogOptions()
{
	NavDialogOptions*	options = nil;

	if (mNavServicesChooser != nil) {
		options = mNavServicesChooser->GetDialogOptions();

	} else if (mClassicChooser != nil) {
		options = mClassicChooser->GetDialogOptions();
	}

	return options;
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
UConditionalDialogs::LFileChooser::SetDefaultLocation(
	const FSSpec&	inFileSpec,
	bool			inSelectIt)
{
	if (mNavServicesChooser != nil) {
		mNavServicesChooser->SetDefaultLocation(inFileSpec, inSelectIt);

	} else if (mClassicChooser != nil) {
		mClassicChooser->SetDefaultLocation(inFileSpec, inSelectIt);
	}
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
UConditionalDialogs::LFileChooser::SetObjectFilterProc(
	NavObjectFilterProcPtr	inFilterProc)
{
	if (mNavServicesChooser != nil) {
		mNavServicesChooser->SetObjectFilterProc(inFilterProc);

	} else if (mClassicChooser != nil) {
		mClassicChooser->SetObjectFilterProc(inFilterProc);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::SetPreviewProc									  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileChooser::SetPreviewProc(
	NavPreviewProcPtr	inPreviewProc)
{
	if (mNavServicesChooser != nil) {
		mNavServicesChooser->SetPreviewProc(inPreviewProc);

	} else if (mClassicChooser != nil) {
		mClassicChooser->SetPreviewProc(inPreviewProc);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskOpenFile										  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileChooser::AskOpenFile(
	const LFileTypeList&	inFileTypes)
{
	bool	openOK = false;

	if (mNavServicesChooser != nil) {
		openOK = mNavServicesChooser->AskOpenFile(inFileTypes);

	} else if (mClassicChooser != nil) {
		openOK = mClassicChooser->AskOpenFile(inFileTypes);
	}

	return openOK;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseOneFile								  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileChooser::AskChooseOneFile(
	const LFileTypeList&	inFileTypes,
	FSSpec&					outFileSpec)
{
	bool	chooseOK = false;

	if (mNavServicesChooser != nil) {
		chooseOK = mNavServicesChooser->AskChooseOneFile(inFileTypes, outFileSpec);

	} else if (mClassicChooser != nil) {
		chooseOK = mClassicChooser->AskChooseOneFile(inFileTypes, outFileSpec);
	}

	return chooseOK;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseFolder									  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileChooser::AskChooseFolder(
	FSSpec&			outFileSpec,
	SInt32&			outFolderDirID)
{
	bool	chooseOK = false;

	if (mNavServicesChooser != nil) {
		chooseOK = mNavServicesChooser->AskChooseFolder(outFileSpec, outFolderDirID);

	} else if (mClassicChooser != nil) {
		chooseOK = mClassicChooser->AskChooseFolder(outFileSpec, outFolderDirID);
	}

	return chooseOK;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::AskChooseVolume									  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileChooser::AskChooseVolume(
	FSSpec&			outFileSpec)
{
	bool	chooseOK = false;

	if (mNavServicesChooser != nil) {
		chooseOK = mNavServicesChooser->AskChooseVolume(outFileSpec);

	} else if (mClassicChooser != nil) {
		chooseOK = mClassicChooser->AskChooseVolume(outFileSpec);
	}

	return chooseOK;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::IsValid											  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileChooser::IsValid() const
{
	bool	valid = false;

	if (mNavServicesChooser != nil) {
		valid = mNavServicesChooser->IsValid();

	} else if (mClassicChooser != nil) {
		valid = mClassicChooser->IsValid();
	}

	return valid;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetNumberOfFiles								  [public]
// ---------------------------------------------------------------------------

SInt32
UConditionalDialogs::LFileChooser::GetNumberOfFiles() const
{
	SInt32	numFiles = 0;

	if (mNavServicesChooser != nil) {
		numFiles = mNavServicesChooser->GetNumberOfFiles();

	} else if (mClassicChooser != nil) {
		numFiles = mClassicChooser->GetNumberOfFiles();
	}

	return numFiles;
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileSpec										  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileChooser::GetFileSpec(
	SInt32		inIndex,
	FSSpec&		outFileSpec) const
{
	if (mNavServicesChooser != nil) {
		mNavServicesChooser->GetFileSpec(inIndex, outFileSpec);

	} else if (mClassicChooser != nil) {
		mClassicChooser->GetFileSpec(inIndex, outFileSpec);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetFileDescList									  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileChooser::GetFileDescList(
	AEDescList&		outDescList) const
{
	if (mNavServicesChooser != nil) {
		mNavServicesChooser->GetFileDescList(outDescList);

	} else if (mClassicChooser != nil) {
		mClassicChooser->GetFileDescList(outDescList);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileChooser::GetScriptCode									  [public]
// ---------------------------------------------------------------------------

ScriptCode
UConditionalDialogs::LFileChooser::GetScriptCode() const
{
	ScriptCode	code = smSystemScript;

	if (mNavServicesChooser != nil) {
		code = mNavServicesChooser->GetScriptCode();

	} else if (mClassicChooser != nil) {
		code = mClassicChooser->GetScriptCode();
	}

	return code;
}

#pragma mark -

// ===========================================================================
//	LFileDesignator Class
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LFileDesignator::LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UConditionalDialogs::LFileDesignator::LFileDesignator()
{
	mClassicDesignator	   = nil;
	mNavServicesDesignator = nil;

	if (UseNavServices()) {
		mNavServicesDesignator = new UNavServicesDialogs::LFileDesignator;

	} else {
		mClassicDesignator = new UClassicDialogs::LFileDesignator;
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::~LFileDesignator								  [public]
// ---------------------------------------------------------------------------

UConditionalDialogs::LFileDesignator::~LFileDesignator()
{
	delete mClassicDesignator;
	delete mNavServicesDesignator;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileType									  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileDesignator::SetFileType(
	OSType		inFileType)
{
	if (mNavServicesDesignator != nil) {
		mNavServicesDesignator->SetFileType(inFileType);

	} else if (mClassicDesignator != nil) {
		mClassicDesignator->SetFileType(inFileType);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetFileCreator								  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileDesignator::SetFileCreator(
	OSType		inFileCreator)
{
	if (mNavServicesDesignator != nil) {
		mNavServicesDesignator->SetFileCreator(inFileCreator);

	} else if (mClassicDesignator != nil) {
		mClassicDesignator->SetFileCreator(inFileCreator);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetDialogOptions								  [public]
// ---------------------------------------------------------------------------

NavDialogOptions*
UConditionalDialogs::LFileDesignator::GetDialogOptions()
{
	NavDialogOptions*	options = nil;

	if (mNavServicesDesignator != nil) {
		options = mNavServicesDesignator->GetDialogOptions();

	} else if (mClassicDesignator != nil) {
		options = mClassicDesignator->GetDialogOptions();
	}

	return options;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::SetDefaultLocation							  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileDesignator::SetDefaultLocation(
	const FSSpec&	inFileSpec,
	bool			inSelectIt)
{
	if (mNavServicesDesignator != nil) {
		mNavServicesDesignator->SetDefaultLocation(inFileSpec, inSelectIt);

	} else if (mClassicDesignator != nil) {
		mClassicDesignator->SetDefaultLocation(inFileSpec, inSelectIt);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::AskDesignateFile								  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileDesignator::AskDesignateFile(
	ConstStringPtr	inDefaultName)
{
	bool	saveOK = false;

	if (mNavServicesDesignator != nil) {
		saveOK = mNavServicesDesignator->AskDesignateFile(inDefaultName);

	} else if (mClassicDesignator != nil) {
		saveOK = mClassicDesignator->AskDesignateFile(inDefaultName);
	}

	return saveOK;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsValid										  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileDesignator::IsValid() const
{
	bool	valid = false;

	if (mNavServicesDesignator != nil) {
		valid = mNavServicesDesignator->IsValid();

	} else if (mClassicDesignator != nil) {
		valid = mClassicDesignator->IsValid();
	}

	return valid;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetFileSpec									  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileDesignator::GetFileSpec(
	FSSpec&		outFileSpec) const
{
	if (mNavServicesDesignator != nil) {
		mNavServicesDesignator->GetFileSpec(outFileSpec);

	} else if (mClassicDesignator != nil) {
		mClassicDesignator->GetFileSpec(outFileSpec);
	}
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsReplacing									  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileDesignator::IsReplacing() const
{
	bool	replacing = false;

	if (mNavServicesDesignator != nil) {
		replacing = mNavServicesDesignator->IsReplacing();

	} else if (mClassicDesignator != nil) {
		replacing = mClassicDesignator->IsReplacing();
	}

	return replacing;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::IsStationery									  [public]
// ---------------------------------------------------------------------------

bool
UConditionalDialogs::LFileDesignator::IsStationery() const
{
	bool	stationery = false;

	if (mNavServicesDesignator != nil) {
		stationery = mNavServicesDesignator->IsStationery();

	} else if (mClassicDesignator != nil) {
		stationery = mClassicDesignator->IsStationery();
	}

	return stationery;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::GetScriptCode								  [public]
// ---------------------------------------------------------------------------

ScriptCode
UConditionalDialogs::LFileDesignator::GetScriptCode() const
{
	ScriptCode	code = smSystemScript;

	if (mNavServicesDesignator != nil) {
		code = mNavServicesDesignator->GetScriptCode();

	} else if (mClassicDesignator != nil) {
		code = mClassicDesignator->GetScriptCode();
	}

	return code;
}


// ---------------------------------------------------------------------------
//	¥ LFileDesignator::CompleteSave									  [public]
// ---------------------------------------------------------------------------

void
UConditionalDialogs::LFileDesignator::CompleteSave(
	NavTranslationOptions	inOption)
{
	if (mNavServicesDesignator != nil) {
		mNavServicesDesignator->CompleteSave(inOption);

	} else if (mClassicDesignator != nil) {
		mClassicDesignator->CompleteSave(inOption);
	}
}


PP_End_Namespace_PowerPlant
