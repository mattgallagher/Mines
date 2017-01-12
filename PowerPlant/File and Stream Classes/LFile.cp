// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFile.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for a Macintosh file with a data and a resource fork
//
//	A FSSpec (File System Specification) record identifies a Mac file.
//	When open, the data fork has a file refNum. Likewise, when open,
//	the resource fork has a file refNum. The LFile class stores an
//	FSSpec and the refNums for the data and resource forks.
//
//	This class does not provide many functions for manipulating files. You
//	should get the FSSpec or refNum of the fork you want to manipulate
//	and make direct calls to the Mac File Manager. However, use the
//	member functions for opening and closing data and resource forks.
//
//	The only file accessing functions provided are ones for reading and
//	writing the entire data fork.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFile.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFile									Default Constructor		  [public]
// ---------------------------------------------------------------------------

LFile::LFile()
{
	mMacFileSpec.vRefNum	= 0;
	mMacFileSpec.parID		= 0;
	mMacFileSpec.name[0]	= 0;
	mDataForkRefNum			= refNum_Undefined;
	mResourceForkRefNum		= refNum_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LFile									Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a File from a Toolbox File System Specification

LFile::LFile(
	const FSSpec&	inFileSpec)
{
	mMacFileSpec		= inFileSpec;
	mDataForkRefNum		= refNum_Undefined;
	mResourceForkRefNum	= refNum_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LFile									Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a File from an Alias
//
//	outWasChanged indicates if the AliasHandle was changed during resolution
//	inFromFile is a File Specifier for the starting point for a relative
//		search. If nil, an absolute search is performed

LFile::LFile(
	AliasHandle		inAlias,
	Boolean&		outWasChanged,
	FSSpec*			inFromFile)
{
	OSErr	err = ::ResolveAlias(inFromFile, inAlias, &mMacFileSpec,
								&outWasChanged);

	mDataForkRefNum		= refNum_Undefined;
	mResourceForkRefNum = refNum_Undefined;

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ ~LFile								Destructor				  [public]
// ---------------------------------------------------------------------------

LFile::~LFile()
{
	try {								// Don't throw out of a destructor
		CloseDataFork();
	}

	catch (...) { }

	try {
		CloseResourceFork();
	}

	catch (...) { }
}


// ---------------------------------------------------------------------------
//	¥ GetSpecifier
// ---------------------------------------------------------------------------
//	Return the Toolbox File System Specification for a File

void
LFile::GetSpecifier(
	FSSpec&		outFileSpec) const
{
	outFileSpec = mMacFileSpec;		// Copy FSSpec struct
}


// ---------------------------------------------------------------------------
//	¥ SetSpecifier
// ---------------------------------------------------------------------------
//	Set a new Toolbox File System Specification for a File
//
//	This has the side effect of closing any open forks of the file identified
//	by the old Specifier

void
LFile::SetSpecifier(
	const FSSpec&	inFileSpec)
{
	CloseDataFork();
	CloseResourceFork();

	mMacFileSpec = inFileSpec;
}


// ---------------------------------------------------------------------------
//	¥ UsesSpecifier
// ---------------------------------------------------------------------------
//	Returns whether the File's FSSpec is the same as the input FSSpec

bool
LFile::UsesSpecifier(
	const FSSpec&	inFileSpec) const
{
	return EqualFileSpec(mMacFileSpec, inFileSpec);
}


// ---------------------------------------------------------------------------
//	¥ SpecifierExists
// ---------------------------------------------------------------------------
//	Returns whether the File's FSSpec corresponds to an existing disk file

bool
LFile::SpecifierExists() const
{
	FInfo	finderInfo;			// File exists if we can get its Finder info
	
	return (::FSpGetFInfo(&mMacFileSpec, &finderInfo) == noErr);
}


// ---------------------------------------------------------------------------
//	¥ MakeAlias
// ---------------------------------------------------------------------------
//	Return a newly created Alias for a File
//
//	inFromFile is a File Specifier for the starting point for a relative
//		search. Pass nil if you don't need relative path information.

AliasHandle
LFile::MakeAlias(
	FSSpec*		inFromFile)
{
	AliasHandle	theAlias;
	::NewAlias(inFromFile, &mMacFileSpec, &theAlias);

	return theAlias;
}


// ---------------------------------------------------------------------------
//	¥ CreateNewFile
// ---------------------------------------------------------------------------
//	Create a new disk File, with an empty data fork and a resoure map.
//	You must call OpenDataFork or OpenResourceFork (with write permission)
//	before you can store information in the File.
//
//	If the file already exists, but doesn't have a resource map, this
//	function will create a resource map.

void
LFile::CreateNewFile(
	OSType		inCreator,
	OSType		inFileType,
	ScriptCode	inScriptCode)
{
	::FSpCreateResFile(&mMacFileSpec, inCreator, inFileType, inScriptCode);
	ThrowIfResError_();
}


// ---------------------------------------------------------------------------
//	¥ CreateNewDataFile
// ---------------------------------------------------------------------------
//	Create a new disk File, with an empty data fork and no resource map.
//	You must call OpenDataFork (with write permission) before you can store
//	data in the File.
//
//	The resource fork is uninitialized (no resource map), so you can't call
//	OpenResourceFork for the File. You can initialize the resource fork
//	by calling CreateNewFile.

void
LFile::CreateNewDataFile(
	OSType		inCreator,
	OSType		inFileType,
	ScriptCode	inScriptCode)
{
	OSErr	err = ::FSpCreate(&mMacFileSpec, inCreator, inFileType,
								inScriptCode);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ OpenDataFork
// ---------------------------------------------------------------------------
//	Open the data fork of a File with the specified permissions and
//	return the reference number for the opened fork
//
//	A data fork must be Open before you can read or write data

SInt16
LFile::OpenDataFork(
	SInt16	inPrivileges)
{
	OSErr	err = ::FSpOpenDF(&mMacFileSpec, (SInt8) inPrivileges, &mDataForkRefNum);
	if (err != noErr) {
		mDataForkRefNum = refNum_Undefined;
		Throw_(err);
	}
	return mDataForkRefNum;
}


// ---------------------------------------------------------------------------
//	¥ CloseDataFork
// ---------------------------------------------------------------------------
//	Close the data fork of a File

void
LFile::CloseDataFork()
{
	if (mDataForkRefNum != refNum_Undefined) {
		OSErr	err = ::FSClose(mDataForkRefNum);
		mDataForkRefNum = refNum_Undefined;
		ThrowIfOSErr_(err);

		::FlushVol(nil, mMacFileSpec.vRefNum);
	}
}


// ---------------------------------------------------------------------------
//	¥ ReadDataFork
// ---------------------------------------------------------------------------
//	Read the entire contents of a File's data fork into a newly created
//	Handle. The caller is responsible for disposing of the Handle.

Handle
LFile::ReadDataFork()
{
	SInt32	fileLength;
	OSErr	err = ::GetEOF(mDataForkRefNum, &fileLength);
	ThrowIfOSErr_(err);

	StHandleBlock	dataHandle(fileLength);

	err = ::SetFPos(mDataForkRefNum, fsFromStart, 0);
	ThrowIfOSErr_(err);

	err = ::FSRead(mDataForkRefNum, &fileLength, *dataHandle);
	ThrowIfOSErr_(err);

	return dataHandle.Release();
}


// ---------------------------------------------------------------------------
//	¥ WriteDataFork
// ---------------------------------------------------------------------------
//	Write to the data fork of a File from a buffer
//
//	The buffer contents completely replace any existing data

SInt32
LFile::WriteDataFork(
	const void*		inBuffer,
	SInt32			inByteCount)
{
	SInt32	bytesWritten = inByteCount;

	OSErr	err = ::SetFPos(mDataForkRefNum, fsFromStart, 0);
	ThrowIfOSErr_(err);

	err = ::FSWrite(mDataForkRefNum, &bytesWritten, inBuffer);
	::SetEOF(mDataForkRefNum, bytesWritten);

	ThrowIfOSErr_(err);

	return bytesWritten;
}


// ---------------------------------------------------------------------------
//	¥ OpenResourceFork
// ---------------------------------------------------------------------------
//	Open the resource fork of a File with the specified permissions and
//	return the reference number for the opened fork
//
//	A resource fork must be Open before you can read or write resources

SInt16
LFile::OpenResourceFork(
	SInt16	inPrivileges)
{
	mResourceForkRefNum = ::FSpOpenResFile(&mMacFileSpec, (SInt8) inPrivileges);

	if (mResourceForkRefNum == -1) {	// Open failed
		ThrowIfResError_();
	}

	return mResourceForkRefNum;
}


// ---------------------------------------------------------------------------
//	¥ CloseResourceFork
// ---------------------------------------------------------------------------
//	Close the resource fork of a File

void
LFile::CloseResourceFork()
{
	if (mResourceForkRefNum != refNum_Undefined) {
		::CloseResFile(mResourceForkRefNum);
		mResourceForkRefNum = refNum_Undefined;
		::FlushVol(nil, mMacFileSpec.vRefNum);
	}
}


// ---------------------------------------------------------------------------
//	¥ EqualFileSpec													  [static]
// ---------------------------------------------------------------------------
//	Compare two FSSpec structs for equality

bool
LFile::EqualFileSpec(
	const FSSpec&	inFileSpecA,
	const FSSpec&	inFileSpecB)
{
		// Compare each field in FSSpec struct
		//   EqualString() [case insensitive, diacritical sensitive] is
		//   the same comparison used by the File System

	return (inFileSpecA.vRefNum == inFileSpecB.vRefNum) &&
		   (inFileSpecA.parID == inFileSpecB.parID) &&
		   ::EqualString(inFileSpecA.name, inFileSpecB.name, false, true);
}


PP_End_Namespace_PowerPlant
