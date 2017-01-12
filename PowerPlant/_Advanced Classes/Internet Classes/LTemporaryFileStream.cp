// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTemporaryFileStream.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A FileStream that will automatically create itself in the temporary files
//  folder and delete itself when the object is deleted.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTemporaryFileStream.h>
#include <LString.h>

#include <Aliases.h>
#include <Files.h>
#include <Folders.h>
#include <Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	• LTemporaryFileStream					Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	Automatically creates a temp file with a unique name

LTemporaryFileStream::LTemporaryFileStream()
{
	SInt16	theVRef;
	SInt32	theDirID;
	OSErr	theErr;

	ThrowIfOSErr_(::FindFolder(kOnSystemDisk, kTemporaryFolderType,
					kCreateFolder, &theVRef, &theDirID));

	// create a new file name from the tickcount
	// we loop until we hit a name not in use
	do {
		LStr255 tempFileName;
		tempFileName = (SInt32)TickCount();
		theErr = ::FSMakeFSSpec(theVRef, theDirID, tempFileName, &mMacFileSpec);
	} while (theErr != fnfErr);

	CreateNewDataFile(FOUR_CHAR_CODE('ttxt'),FOUR_CHAR_CODE('TEXT'));
	OpenDataFork(fsRdWrPerm);
}


// ---------------------------------------------------------------------------
//	• LTemporaryFileStream					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a TemporaryFile from a file name.

LTemporaryFileStream::LTemporaryFileStream(
	ConstStr255Param	inFileName)
{
	SInt16	theVRef;
	SInt32	theDirID;

	ThrowIfOSErr_(::FindFolder(kOnSystemDisk, kTemporaryFolderType,
					kCreateFolder, &theVRef, &theDirID));

	::FSMakeFSSpec(theVRef, theDirID, inFileName, &mMacFileSpec);

	CreateNewDataFile(FOUR_CHAR_CODE('ttxt'), FOUR_CHAR_CODE('TEXT'));
	OpenDataFork(fsRdWrPerm);
}


// ---------------------------------------------------------------------------
//	• LTemporaryFileStream					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a TemporaryFile from a Toolbox File System Specification

LTemporaryFileStream::LTemporaryFileStream(
	FSSpec& inFileSpec)
	
	: LFileStream(inFileSpec)
{
}


// ---------------------------------------------------------------------------
//	• LTemporaryFileStream					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a TemporaryFile from an Alias
//
//	outWasChanged indicates if the AliasHandle was changed during resolution
//	inFromFile is a File Specifier for the starting point for a relative
//		search. If nil, an absolute search is performed

LTemporaryFileStream::LTemporaryFileStream
	(AliasHandle	inAlias,
	Boolean&		outWasChanged,
	FSSpec*			inFromFile)

	: LFileStream(inAlias, outWasChanged, inFromFile)
{
}


// ---------------------------------------------------------------------------
//	• ~LTemporaryFileStream					Destructor				  [public]
// ---------------------------------------------------------------------------

LTemporaryFileStream::~LTemporaryFileStream()
{
	CloseDataFork();
	CloseResourceFork();
	::FSpDelete(&mMacFileSpec);
}


// ---------------------------------------------------------------------------
//	• SetMarker														  [public]
// ---------------------------------------------------------------------------
//	Place the Read/Write Marker at an offset from a specified position
//
//	inFromWhere can be streamFrom_Start, streamFrom_End, or streamFrom_Marker

void
LTemporaryFileStream::SetMarker(
	SInt32		inOffset,
	EStreamFrom	inFromWhere)
{
	//Make this work like other streams
	if (inFromWhere == streamFrom_End) {
		inOffset = inOffset * -1;
	}
	OSErr	err = ::SetFPos(GetDataForkRefNum(), inFromWhere, inOffset);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	• OpenOrCreateResourceFork										  [public]
// ---------------------------------------------------------------------------
//	Opens the resource fork of the temporary file, if it exists.
//	If the resource fork doesn’t exist, it creates one.
//	If the file doesn’t exist, it creates the file with the designated
//	type and creator codes.

SInt16 LTemporaryFileStream::OpenOrCreateResourceFork(
	SInt16		inPrivileges,
	OSType		inCreator,
	OSType		inFileType,
	ScriptCode	inScriptCode)
{

	// if Temporary Folder didn’t exist, create one now (and rebuild FSSpec)

	if (mMacFileSpec.parID == 0) {
		SInt16 theVRef;
		SInt32 theDirID;
		Str63 theFileName;

		LString::CopyPStr(mMacFileSpec.name, theFileName, sizeof(Str63));
		ThrowIfOSErr_(::FindFolder(kOnSystemDisk, kTemporaryFolderType,
								kCreateFolder, &theVRef, &theDirID));
		::FSMakeFSSpec(theVRef, theDirID, theFileName, &mMacFileSpec);
	}


	// try opening the resource fork

	mResourceForkRefNum = ::FSpOpenResFile(&mMacFileSpec, (SInt8) inPrivileges);
	if (mResourceForkRefNum == -1) {

		// resource file didn’t exist, let’s create it

		OSErr theErr = ResError();
		if (theErr == fnfErr) {
			::FSpCreateResFile(&mMacFileSpec, inCreator, inFileType, inScriptCode);
			ThrowIfResError_();
			mResourceForkRefNum = ::FSpOpenResFile(&mMacFileSpec, (SInt8) inPrivileges);
			ThrowIfResError_();

		} else {
			ThrowOSErr_(theErr);
		}
	}

	return mResourceForkRefNum;
}


PP_End_Namespace_PowerPlant
