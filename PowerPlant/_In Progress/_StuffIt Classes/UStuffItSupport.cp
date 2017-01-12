// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UStuffItSupport.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility class for using the StuffIt Engine.
//
//	NOTE:
//	In order to greatly simplify many of the interfaces I have made the
//	default values for the various parameters static member variables. These
//	are public so you can directly adjust them as necessary.
//
//	IMPORTANT:	These classes require the use of the Aladdin StuffIt Engine
//				libraries. You may need a license from Aladdin in order to
//				distribute products using these libraries. Please be sure
//				to carefully read the included Aladdin documentation. For
//				more information see <http://www.aladdinsys.com>

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UStuffItSupport.h>
#include <LString.h>

PP_Begin_Namespace_PowerPlant

const ResIDT	kStuffItResourceID	= 2110;
const SInt16		kStuffItSITSuffix	= 4;
const SInt16		kStuffItSEASuffix	= 6;

// ===========================================================================

#pragma mark --- Static Declarations ---

SInt32						UStuffItSupport::sCookie 		= 0;
SInt32						UStuffItSupport::sEngineOpen 	= 0;

//StuffIt Engine Defaults for file handling, confilict resolution, etc..
//	NOTE: These are public so you can directly adjust them as necessary
SInt16						UStuffItSupport::sEngineLocation			= kUseExternalEngine;
Boolean						UStuffItSupport::sPromptForDestination 		= kPromptForDestination;
Boolean						UStuffItSupport::sDeleteOriginal			= kDontDeleteOriginal;
SInt16						UStuffItSupport::sCreateFolder 				= kCreateFolderSmart;
Boolean						UStuffItSupport::sSaveComments 				= kDontSaveComments;
Boolean						UStuffItSupport::sStopOnAbort 				= kStopOnAbort;
SInt16						UStuffItSupport::sConflictAction 			= kUnStuffConflictAutoRename;
Boolean						UStuffItSupport::sCompressOriginal 			= kCompressOriginal;
Boolean						UStuffItSupport::sEncryptOriginal 			= kDontEncryptOriginal;
Boolean						UStuffItSupport::sResolveAliases 			= kResolveAliases;
Boolean						UStuffItSupport::sNoRecompression 			= kDontRecompressCompresed;
SInt16						UStuffItSupport::sConvertTextFiles 			= kTextConvertSmart;
Boolean						UStuffItSupport::sAddLineFeeds 				= kAddLineFeeds;
OSType 						UStuffItSupport::sHqxCreator 				= kDefaultHQXCreator;
SInt16 						UStuffItSupport::sSegmentSize 				= kSegmentHighDensity;

// ===========================================================================

#pragma mark -
#pragma mark --- File Decompression/Decoding ---

// ---------------------------------------------------------------------------
//	¥ ExpandFile
// ---------------------------------------------------------------------------
//	Automatically determines file type and expands file.
//	You can normally call this instead of UnStuffFile/DecodeFile

void
UStuffItSupport::ExpandFile(FSSpec& inFile, FSSpecPtr inDest, FSSpec * outFile)
{
	StOpenStuffIt engineOpen;

	SInt16 fileType = FileTypeIs(inFile);
	switch (fileType) {
		case ftype_unknown:
			Throw_(notValidFileTypeErr);
			break;

		case ftype_StuffIt:
			UnStuffFile(inFile, inDest, outFile);
			break;

		default:
			DecodeFile(inFile, fileType, inDest, outFile);
			break;
	}
}

// ---------------------------------------------------------------------------
//	¥ UnStuffFile
// ---------------------------------------------------------------------------
//	Unstuffs a file

void
UStuffItSupport::UnStuffFile(FSSpec& inArchive, FSSpecPtr inDest, FSSpecPtr outFile)
{
	FSSpec	destSpec;

	// deal with valid destinations - and either set it to the specified one or the same as orig
	if (inDest)
		destSpec = *inDest;
	else
		destSpec = inArchive;
	destSpec.name[0] = 0;	// LDR NOTE: destination specification is via a FOLDER!

	StOpenStuffIt engineOpen;
	ThrowIfError_(UnStuffFSSpec(sCookie,
								&inArchive,
								&destSpec,
								outFile,
								sPromptForDestination,
								sDeleteOriginal,
								sCreateFolder,
								sSaveComments,
								sStopOnAbort,
								sConflictAction,
								sConvertTextFiles));
}


// ---------------------------------------------------------------------------
//	¥ DecodeFile
// ---------------------------------------------------------------------------
//	Decodes a file

void
UStuffItSupport::DecodeFile(FSSpec& inArchive, SInt16 fileType, FSSpecPtr inDest, FSSpec * outFile)
{
	FSSpec	destSpec;

	// deal with valid destinations - and either set it to the specified one or the same as orig
	if (inDest)
		destSpec = *inDest;
	else
		destSpec = inArchive;
	destSpec.name[0] = 0;	// LDR NOTE: destination specification is via a FOLDER!

	StOpenStuffIt engineOpen;
	ThrowIfError_(ExpandFSSpec(sCookie,
								fileType,
								&inArchive,
								&destSpec,
								outFile,
								sCreateFolder,
								sDeleteOriginal,
								sConvertTextFiles));
}

// ===========================================================================

#pragma mark -
#pragma mark --- File Compression/Encoding ---

// ---------------------------------------------------------------------------
//	¥ StuffFile
// ---------------------------------------------------------------------------
//	Stuff a file

void
UStuffItSupport::StuffFile(FSSpec& inFile, FSSpec * outArchive)
{
	LStuffItFileList theList;
	theList = inFile;

	StuffFileList(theList, outArchive);
}

// ---------------------------------------------------------------------------
//	¥ StuffFileList
// ---------------------------------------------------------------------------
//	Stuffs a list of files into a single archive

void
UStuffItSupport::StuffFileList(LStuffItFileList& inList, FSSpec * outArchive)
{
	StOpenStuffIt engineOpen;
	ThrowIfError_(::StuffFileList(sCookie,
								  inList,
								  outArchive,
								  outArchive,
								  sCompressOriginal,
								  sDeleteOriginal,
								  sEncryptOriginal,
								  sResolveAliases,
								  sNoRecompression,
								  sConflictAction,
								  kStuffItCompressionMax));
}


// ---------------------------------------------------------------------------
//	¥ BinhexFile
// ---------------------------------------------------------------------------
//	Applies Binhex encoding to a file

void
UStuffItSupport::BinhexFile(FSSpec& inFile, FSSpec * outFile, FSSpec * resultFile)
{
	StOpenStuffIt engineOpen;
	ThrowIfError_(HQXEncodeFSSpec(sCookie,
								&inFile,
								outFile,
								resultFile,
								sDeleteOriginal,
								sAddLineFeeds,
								sHqxCreator));
}

// ===========================================================================

#pragma mark -
#pragma mark --- SIT <-> SEA ---


// ---------------------------------------------------------------------------
//	¥ MakeArchiveSelfExtracting
// ---------------------------------------------------------------------------
//	Makes a stuffit archive self extracting

void
UStuffItSupport::MakeArchiveSelfExtracting(FSSpec& inArchive)
{
	StOpenStuffIt engineOpen;
	if (DetermineSelfExtracting(&inArchive))
		return;
	ThrowIfError_(ConvertSITtoSEA(sCookie, &inArchive));	//use global defaults here

	//change name to reflect new status if old file suffix
	//	appropriate.
	LStr255 fileName(inArchive.name);
	LStr255 sitSuffix(kStuffItResourceID, kStuffItSITSuffix);

	if (fileName.EndsWith(sitSuffix)) {
		LStr255 seaSuffix(kStuffItResourceID, kStuffItSEASuffix);
		UInt8 suffStarts = fileName.ReverseFindWithin(sitSuffix);
		fileName.Replace(suffStarts, seaSuffix.Length(), seaSuffix);
		ThrowIfError_(::FSpRename(&inArchive, fileName));
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeArchiveStuffIt
// ---------------------------------------------------------------------------
//	Makes a self extracting archive a stuffit archive

void
UStuffItSupport::MakeArchiveStuffIt(FSSpec& inArchive)
{
	StOpenStuffIt engineOpen;
	if (!DetermineSelfExtracting(&inArchive))
		return;
	ThrowIfError_(ConvertSITtoSEA(sCookie, &inArchive));	//use global defaults here

	//change name to reflect new status if old file suffix
	//	appropriate.
	LStr255 fileName(inArchive.name);
	LStr255 seaSuffix(kStuffItResourceID, kStuffItSEASuffix);

	if (fileName.EndsWith(seaSuffix)) {
		LStr255 sitSuffix(kStuffItResourceID, kStuffItSITSuffix);
		UInt8 suffStarts = fileName.ReverseFindWithin(seaSuffix);
		fileName.Replace(suffStarts, sitSuffix.Length(), sitSuffix);
		ThrowIfError_(::FSpRename(&inArchive, fileName));
	}
}


// ---------------------------------------------------------------------------
//	¥ IsSelfExtracting
// ---------------------------------------------------------------------------
//	Determines if an archive is self extracting

Boolean
UStuffItSupport::IsSelfExtracting(FSSpec& inFile)
{
	StOpenStuffIt engineOpen;
	Boolean isSelfExtracting = DetermineSelfExtracting(&inFile);
	return isSelfExtracting;
}

// ===========================================================================

#pragma mark -
#pragma mark --- Archive Segmenting ---


// ---------------------------------------------------------------------------
//	¥ SegmentArchive
// ---------------------------------------------------------------------------
//	Breaks single archive into several segments

void
UStuffItSupport::SegmentArchive(FSSpec& inArchive,
								FSSpecPtr inDest,
								Boolean AsSEA)
{
	FSSpec	destSpec;

	// deal with valid destinations - and either set it to the specified one or the same as orig
	if (inDest)
		destSpec = *inDest;
	else
		destSpec = inArchive;
	destSpec.name[0] = 0;	// LDR NOTE: destination specification is via a FOLDER!

	StOpenStuffIt engineOpen;
	ThrowIfError_(SegmentFSSpec(sCookie,
								&inArchive,
								&destSpec,
								NULL,
								sSegmentSize,
								AsSEA,
								sDeleteOriginal));
}


// ---------------------------------------------------------------------------
//	¥ JoinToArchive
// ---------------------------------------------------------------------------
//	Joins a archive segment into a archive

void
UStuffItSupport::JoinToArchive(FSSpec& inArchive,
								FSSpecPtr inDest,
								FSSpecPtr outArchive)
{
	FSSpec	destSpec;

	// deal with valid destinations - and either set it to the specified one or the same as orig
	if (inDest)
		destSpec = *inDest;
	else
		destSpec = inArchive;
	destSpec.name[0] = 0;	// LDR NOTE: destination specification is via a FOLDER!

	StOpenStuffIt engineOpen;
	ThrowIfError_(JoinFSSpec(sCookie,
								&inArchive,
								&destSpec,
								outArchive,
								sDeleteOriginal));
}

// ---------------------------------------------------------------------------
//	¥ IsFirstArchiveSegment
// ---------------------------------------------------------------------------
//	Determines if archive in question is the first segment of a archive

Boolean
UStuffItSupport::IsFirstArchiveSegment(FSSpec& inArchive)
{
	StOpenStuffIt engineOpen;
	Boolean isFirst = IsFirstSegment(&inArchive);
	return isFirst;
}

// ---------------------------------------------------------------------------
//	¥ CountArchiveSegments
// ---------------------------------------------------------------------------
//	Determines number of archives to be created given a segment size

void
UStuffItSupport::CountArchiveSegments(FSSpec& inArchive,
										SInt32	inSegmentSize,
										SInt16&	outSegmentCount)
{
	StOpenStuffIt engineOpen;
	ThrowIfError_(CountSegments(sCookie,
								&inArchive,
								inSegmentSize,
								&outSegmentCount));
}

// ===========================================================================

#pragma mark -
#pragma mark --- StuffIt Utilities ---

// ---------------------------------------------------------------------------
//	¥ OpenEngine
// ---------------------------------------------------------------------------
//	Opens the StuffIt engine (if necessary)
//
//	You should not normally need to call Open/Close yourself. The wrapper functions
//	call them as necessary.
//
//	NOTE: Important to call Open/Close in pairs

void
UStuffItSupport::OpenEngine()
{
	if (sEngineOpen == 0) {
		ThrowIfError_(OpenSITEngine(sEngineLocation, &sCookie));
		if (sEngineLocation == kUseExternalEngine) {
			SInt16 version = GetSITEngineVersion(sCookie);
			ThrowIfNot_(version >= kFirstSupportedEngine);
		}
	}

	sEngineOpen++;
}

// ---------------------------------------------------------------------------
//	¥ CloseEngine
// ---------------------------------------------------------------------------
//	Closes the StuffIt engine (if necessary)

void
UStuffItSupport::CloseEngine()
{
	sEngineOpen--;

	if (sEngineOpen == 0) {
		CloseSITEngine(sCookie);
	}
}

// ---------------------------------------------------------------------------
//	¥ CheckStuffItAvailable
// ---------------------------------------------------------------------------
//	Check for StuffIt Engine availability

Boolean
UStuffItSupport::CheckStuffItAvailable()
{
	try {
		UStuffItSupport::OpenEngine();
		UStuffItSupport::CloseEngine();
	} catch (...) {
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
//	¥ CheckEngineRegistered
// ---------------------------------------------------------------------------
//	Check for StuffIt Engine registration

Boolean
UStuffItSupport::CheckEngineRegistered()
{
	try {
		StOpenStuffIt engineOpen;
		Boolean isReg = IsSITEngineRegistered(sCookie);
		return isReg;
	} catch (...) {
		return false;
	}
}

// ---------------------------------------------------------------------------
//	¥ FileTypeIs
// ---------------------------------------------------------------------------
//	Determines file type of a file

SInt16
UStuffItSupport::FileTypeIs(FSSpec& inFile)
{
	StOpenStuffIt engineOpen;

#if (k5orLaterEngine)
	return DetermineFileType(sCookie, &inFile);
#else
	return DetermineFileType(&inFile);
#endif
}

// ---------------------------------------------------------------------------
//	¥ DefaultArchiveFSSpec
// ---------------------------------------------------------------------------
//	Determines the default FSSpec for a file to be archived

void
UStuffItSupport::DefaultArchiveFSSpec(FSSpec& inFile,
										FSSpec& outFile,
										Boolean AsSEA)
{
	LStuffItFileList theList;
	theList = inFile;

	DefaultArchiveFSSpec(theList, outFile, AsSEA);
}

// ---------------------------------------------------------------------------
//	¥ DefaultArchiveFSSpec
// ---------------------------------------------------------------------------
//	Determines the default FSSpec for a file to be archived

void
UStuffItSupport::DefaultArchiveFSSpec(LStuffItFileList& inList,
										FSSpec& outFile,
										Boolean AsSEA)
{
	StOpenStuffIt engineOpen;
	GetNewArchiveFSSpec(inList, AsSEA, &outFile);
}

// ---------------------------------------------------------------------------
//	¥ DefaultArchiveFSSpec
// ---------------------------------------------------------------------------
//	Determines the default FSSpec for a file to be archived

void
UStuffItSupport::DefaultArchiveFSSpec(FSSpecArrayHandle inList,
										FSSpec& outFile,
										Boolean AsSEA)
{
	StOpenStuffIt engineOpen;
	GetNewArchiveFSSpec(inList, AsSEA, &outFile);
}

// ===========================================================================

#pragma mark -
#pragma mark --- LStuffItFileList ---

// ---------------------------------------------------------------------------
//	¥ LStuffItFileList
// ---------------------------------------------------------------------------
//	Utility class for working with StuffIt file lists
//	You can create an object of this type in place of the FSSpecArrayHandle
//	required by the StuffIt Engine.


LStuffItFileList::LStuffItFileList()
{
	InitList();
}

LStuffItFileList::LStuffItFileList(FSSpec& inFile)
{
	InitList();
	Append(inFile);
}

LStuffItFileList::~LStuffItFileList()
{
	ResetList();
}

void
LStuffItFileList::InitList()
{
	StOpenStuffIt engineOpen;
	mListHandle = NewFSSpecList();
	ThrowIfNil_(mListHandle);
}

void
LStuffItFileList::ResetList()
{
	StOpenStuffIt engineOpen;
	DisposeFSSpecList(mListHandle);
}

void
LStuffItFileList::Reset()
{
	ResetList();
	InitList();
}

void
LStuffItFileList::Append(FSSpec& inFile)
{
	StOpenStuffIt engineOpen;
	ThrowIfOSErr_(AddToFSSpecList(&inFile, mListHandle));
}

// ===========================================================================

#pragma mark -
#pragma mark --- LStuffItArcObjectList ---

// ---------------------------------------------------------------------------
//	¥ LStuffItArcObjectList
// ---------------------------------------------------------------------------
//	Utility class for working with StuffIt Archive Object lists
//	You can create an object of this type in place of the arcObjectArrayHandle
//	required by the StuffIt Engine.


LStuffItArcObjectList::LStuffItArcObjectList()
{
	InitList();
}

LStuffItArcObjectList::LStuffItArcObjectList(archiveObject& inArcObject)
{
	InitList();
	Append(inArcObject);
}

LStuffItArcObjectList::~LStuffItArcObjectList()
{
	ResetList();
}

void
LStuffItArcObjectList::InitList()
{
	StOpenStuffIt engineOpen;
	mListHandle = NewArcObjectList();
	ThrowIfNil_(mListHandle);
}

void
LStuffItArcObjectList::ResetList()
{
	StOpenStuffIt engineOpen;
	DisposeArcObjectList(mListHandle);
}

void
LStuffItArcObjectList::Reset()
{
	ResetList();
	InitList();
}

void
LStuffItArcObjectList::Append(archiveObject& inArcObject)
{
	StOpenStuffIt engineOpen;
	ThrowIfOSErr_(AddToArcObjectList(&inArcObject, mListHandle));
}

PP_End_Namespace_PowerPlant

