// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPreferencesFile.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A File that will automatically locate itself in the Preferences
//  folder when created.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPreferencesFile.h>
#include <LString.h>

#include <Aliases.h>
#include <Files.h>
#include <Folders.h>
#include <Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	• LPreferencesFile						Default Constructor		  [pulbic]
// ---------------------------------------------------------------------------

LPreferencesFile::LPreferencesFile()
{
}


// ---------------------------------------------------------------------------
//	• LPreferencesFile						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a PreferencesFile from a file name. This is the preferred
//	constructor. Builds the file spec by using FindFolder to get the
//	Preferences folder, then attaches the specified name to it.

LPreferencesFile::LPreferencesFile(
	ConstStringPtr		inFileName,
	Boolean				inCreateFolder)
{
	SInt16	theVRef;
	SInt32	theDirID;
	OSErr	theErr;

	theErr = ::FindFolder(kOnSystemDisk, kPreferencesFolderType,
					inCreateFolder, &theVRef, &theDirID);

	if (theErr == noErr) {
		::FSMakeFSSpec(theVRef, theDirID, inFileName, &mMacFileSpec);

	} else {
		LString::CopyPStr(inFileName, mMacFileSpec.name, sizeof(Str63));
	}

}


// ---------------------------------------------------------------------------
//	• LPreferencesFile						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a PreferencesFile from a Toolbox File System Specification

LPreferencesFile::LPreferencesFile(
	FSSpec& 	inFileSpec)

	: LFile(inFileSpec)
{
}


// ---------------------------------------------------------------------------
//	• LPreferencesFile						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a PreferencesFile from an Alias
//
//	outWasChanged indicates if the AliasHandle was changed during resolution
//	inFromFile is a File Specifier for the starting point for a relative
//		search. If nil, an absolute search is performed

LPreferencesFile::LPreferencesFile(
	AliasHandle		inAlias,
	Boolean&		outWasChanged,
	FSSpec*			inFromFile)

	: LFile(inAlias, outWasChanged, inFromFile)
{
}


// ---------------------------------------------------------------------------
//	• ~LPreferencesFile						Destructor				  [public]
// ---------------------------------------------------------------------------

LPreferencesFile::~LPreferencesFile()
{
}


// ---------------------------------------------------------------------------
//	• OpenOrCreateResourceFork										  [public]
// ---------------------------------------------------------------------------
//	Opens the resource fork of the preferences file, if it exists.
//	If the resource fork doesn’t exist, it creates one.
//	If the file doesn’t exist, it creates the file with the designated
//	type and creator codes.

SInt16 LPreferencesFile::OpenOrCreateResourceFork(
	SInt16		inPrivileges,
	OSType		inCreator,
	OSType		inFileType,
	ScriptCode	inScriptCode)
{
	if (mMacFileSpec.parID == 0) {		// If Preferences Folder doesn't
		SInt16 theVRef;					//   exist, create one now (and
		SInt32 theDirID;				//   rebuild our FSSpec)
		Str63 theFileName;

		LString::CopyPStr(mMacFileSpec.name, theFileName, sizeof(Str63));

		ThrowIfOSErr_(
				::FindFolder(kOnSystemDisk, kPreferencesFolderType,
								kCreateFolder, &theVRef, &theDirID) );

		::FSMakeFSSpec(theVRef, theDirID, theFileName, &mMacFileSpec);
	}

										// Try opening the resource file
	mResourceForkRefNum =
					::FSpOpenResFile(&mMacFileSpec, (SInt8) inPrivileges);

	if (mResourceForkRefNum == -1) {	// Couldn't open resource file
		OSErr theErr = ::ResError();

		if (theErr == fnfErr) {			// Resource file doesn't exist,
										//   create it now
			::FSpCreateResFile(&mMacFileSpec, inCreator,
								inFileType, inScriptCode);
			ThrowIfResError_();
										// And open it
			mResourceForkRefNum =
					::FSpOpenResFile(&mMacFileSpec, (SInt8) inPrivileges);
			ThrowIfResError_();

		} else {						// Some other error. Abort.
			ThrowOSErr_(theErr);
		}
	}

	return mResourceForkRefNum;
}


PP_End_Namespace_PowerPlant

