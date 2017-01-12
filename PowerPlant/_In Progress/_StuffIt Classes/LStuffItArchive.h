// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStuffItArchive.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
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


#ifndef _H_LStuffItArchive
#define _H_LStuffItArchive
#pragma once

#include <UStuffItSupport.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStuffItArchive {
public:
					LStuffItArchive( FSSpec& inArchive );

	virtual			~LStuffItArchive();

	virtual void	InitDefaults();

											// File handling
	virtual void	Create();
	virtual void	Open();
	virtual void	Close();

											// Archive Element Handling
	virtual void	StuffToArchive(
							LStuffItFileList&	inFileList,
							archiveObject*		inObjectParent = nil,
							unsigned char*		inPassword = kNoPassword);
									
	virtual void	StuffToArchive(
							FSSpec&				inFile,
							archiveObject*		inObjectParent = nil,
							unsigned char*		inPassword= kNoPassword);
									
	virtual void	AddFolderToArchive(
							Str255				folderName,
							archiveObject&		outFolderObject,
							archiveObject*		inObjectParent= kNoPassword);

	virtual void	UnStuffFromArchive(
							archiveObject&		inObject,
							FSSpec				destSpec,
							unsigned char*		inPassword = kNoPassword);

	virtual void	UnStuffFromArchive(
							LStuffItArcObjectList&	inObjectList,
							FSSpec					destSpec,
							unsigned char*			inPassword = kNoPassword);

											// Iterative Archive Navigation
	virtual void	Reset();
	
	virtual Boolean	Current( archiveObject& outObject );
	
	virtual Boolean	Next( archiveObject& outObject );
	
	virtual Boolean	Down( archiveObject& outObject );
	
	virtual Boolean	Up( archiveObject& outObject );

											// Archive Comments
	virtual void	GetComments(Handle outComment, StScrpHandle outStyleInfo);
	
	virtual void	SetComments(Handle inComment, StScrpHandle inStyleInfo);

											// Utilities
	virtual SInt32	GetDecompressedSize();
	
	virtual void	SetObjectInfo( archiveObject& inObject );
	
	virtual void	UpdateObjectInfo( archiveObject& inOutObject );
	
	virtual archiveObject*	GetDefaultParent();

											// Default Parameters
	bool					mCompressOriginal;
	bool					mDeleteOriginal;
	bool					mEncryptOriginal;
	bool					mResolveAliases;
	bool					mNoRecompression;
	bool					mPromptForDestination;
	bool					mStopOnAbort;
	SInt16					mCreateFolder;
	SInt16					mConflictAction;
	SInt16					mConvertTextFiles;
	bool					mShowNoProgress;
	SITAlertCallbackUPP		mAlertCBUPP;
	SITStatusCallbackUPP	mStatusCBUPP;
	SITPeriodicCallbackUPP	mPeriodicCBUPP;

protected:
	FSSpec			mFSSpec;
	bool			mArchiveOpen;
	bool			mIteratorInitialized;
	archiveInfo		mArchiveInfo;
	archiveObject	mCurrentObject;
	bool			mIteratorAtHead;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
