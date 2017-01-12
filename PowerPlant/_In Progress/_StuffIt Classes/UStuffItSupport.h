// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UStuffItSupport.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility class for using the StuffIt Engine.
//
//	IMPORTANT:	These classes require the use of the Aladdin StuffIt Engine
//				libraries. You may need a license from Aladdin in order to
//				distribute products using these libraries. Please be sure
//				to carefully read the included Aladdin documentation. For
//				more information see <http://www.aladdinsys.com>

#ifndef _H_UStuffItSupport
#define _H_UStuffItSupport
#pragma once

#include <PP_Prefix.h>

#include "StuffItEngineLib.h"
#include "StuffItEngineLibLowLevel.h"

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LStuffItFileList;
class LFile;

//Set this flag to zero value if you are using a pre 5.0 version
//	of the StuffIt engine.
#ifndef k5orLaterEngine
#define k5orLaterEngine		1
#endif

// ===========================================================================

#pragma mark UStuffItSupport

class UStuffItSupport {

public:
	static void				OpenEngine();
	static void				CloseEngine();

	static Boolean			CheckStuffItAvailable();
	static Boolean			CheckEngineRegistered();

	static void				ExpandFile(FSSpec& inFile, FSSpecPtr inDest = nil, FSSpec * outFile = nil);

	static void				BinhexFile(FSSpec& inFile, FSSpec * outFile = nil, FSSpec * resultFile = nil);

	static void				StuffFile(FSSpec& inFile, FSSpec * outArchive = nil);
	static void				StuffFileList(LStuffItFileList& inList, FSSpec * outFile = nil);
	static void				StuffFileList(FSSpecArrayHandle inList, FSSpec * outFile = nil);

	static void				MakeArchiveSelfExtracting(FSSpec& inArchive);

	static void				MakeArchiveStuffIt(FSSpec& inArchive);

	static Boolean			IsSelfExtracting(FSSpec& inFile);

	static SInt16			FileTypeIs(FSSpec& inFile);

	static void			UnStuffFile(FSSpec& inArchive, FSSpecPtr inDest = nil, FSSpec * outFile = nil);

	static void			DecodeFile(FSSpec& inFile, SInt16 fileType, FSSpecPtr inDest = nil, FSSpec * outFile = nil);

	static void				DefaultArchiveFSSpec(FSSpec& inFile,
										FSSpec& outFile,
										Boolean AsSEA = false);
	static void				DefaultArchiveFSSpec(LStuffItFileList& inList,
										FSSpec& outFile,
										Boolean AsSEA = false);
	static void				DefaultArchiveFSSpec(FSSpecArrayHandle inList,
										FSSpec& outFile,
										Boolean AsSEA = false);

	static void				SegmentArchive(FSSpec& inArchive,
										FSSpecPtr inDest = nil,
										Boolean AsSEA = false);

	static void				JoinToArchive(FSSpec& inArchive,
										FSSpecPtr inDest = nil,
										FSSpecPtr outArchive = nil);

	static Boolean			IsFirstArchiveSegment(FSSpec& inArchive);

	static void				CountArchiveSegments(FSSpec& inArchive,
												SInt32	inSegmentSize,
												SInt16&	outSegmentCount);

	// NOTE: These are public... Feel free to change them as you see fit.
	static SInt16			sEngineLocation;
	static Boolean			sPromptForDestination;
	static Boolean			sDeleteOriginal;
	static SInt16			sCreateFolder;
	static Boolean			sSaveComments;
	static Boolean			sStopOnAbort;
	static SInt16			sConflictAction;
	static Boolean			sCompressOriginal;
	static Boolean			sEncryptOriginal;
	static Boolean			sResolveAliases;
	static Boolean			sNoRecompression;
	static SInt16			sConvertTextFiles;
	static Boolean			sAddLineFeeds;
	static OSType 			sHqxCreator;
	static SInt16			sSegmentSize;

	// This is public, but don't change it!!!
	static SInt32			sCookie;

protected:
	static SInt32			sEngineOpen;

friend class LStuffItFileList;
friend class LStuffItArcObjectList;
};


// ===========================================================================

#pragma mark -
#pragma mark LStuffItFileList

class LStuffItFileList {

public:

							LStuffItFileList();
							LStuffItFileList(LFile& inFile);
							LStuffItFileList(FSSpec& inFile);
	virtual					~LStuffItFileList();

	void					Reset();

	void					Append(FSSpec& inFile);
	SInt32					Count()
							{
								return CountFSSpecList(mListHandle);
							}

	operator FSSpecArrayHandle()	{ return mListHandle; }


	LStuffItFileList&	operator=(
						FSSpec& inFile)
					{
						Reset();
						Append(inFile);
						return *this;
					}

	LStuffItFileList&	operator+=(
						FSSpec& inFile)
					{
						Append(inFile);
						return *this;
					}


protected:
	void					InitList();
	void					ResetList();

	FSSpecArrayHandle 		mListHandle;

friend class LStuffItFileList;
};

// ===========================================================================

#pragma mark -
#pragma mark LStuffItArcObjectList

class LStuffItArcObjectList {

public:

							LStuffItArcObjectList();
							LStuffItArcObjectList(archiveObject& inArcObject);
	virtual					~LStuffItArcObjectList();

	void					Reset();

	void					Append(archiveObject& inArcObject);

	SInt32					Count()
							{
								return CountArcObjectList(mListHandle);
							}

	operator arcObjectArrayHandle()	{ return mListHandle; }

	LStuffItArcObjectList&	operator=(
						archiveObject& inArcObject)
					{
						Reset();
						Append(inArcObject);
						return *this;
					}

	LStuffItArcObjectList&	operator+=(
						archiveObject& inArcObject)
					{
						Append(inArcObject);
						return *this;
					}


protected:
	void					InitList();
	void					ResetList();

	arcObjectArrayHandle	mListHandle;

friend class LStuffItArcObjectList;
};

// ===========================================================================

#pragma mark -
#pragma mark StOpenStuffIt

class StOpenStuffIt {
public:
		StOpenStuffIt()
		{
			UStuffItSupport::OpenEngine();
		}

		~StOpenStuffIt()
		{
			UStuffItSupport::CloseEngine();
		}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
