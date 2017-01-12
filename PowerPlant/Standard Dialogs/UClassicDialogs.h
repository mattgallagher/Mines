// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UClassicDialogs.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation of UStandardDialogs, LFileChooser, and LFileDesignator
//	that uses Alerts and StandardFile

#ifndef _H_UClassicDialogs
#define _H_UClassicDialogs
#pragma once

#include <PP_Prefix.h>
#include <LFileTypeList.h>
#include <UAppleEventsMgr.h>
#include <Navigation.h>
#include <StandardFile.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UClassicDialogs {

	#include <UStandardDialogs.i>

	// -----------------------------------------------------------------------

	class	LFileChooser {
		public:
			#include <LFileChooser.i>

		protected:
			StandardFileReply			mSFReply;
			mutable StAEDescriptor		mFileDescList;
			NavObjectFilterProcPtr		mNavFilterProc;
			FSSpec						mDefaultLocation;
			bool						mSelectDefault;

			static NavObjectFilterProcPtr	sFilterProc;
			static bool						sChoosingFolder;
			static bool						sChoosingVolume;

			static pascal Boolean	FileFilter_NavServicesAPI(
										CInfoPBPtr		inPB,
										void*			ioDataPtr);
	};

	// -----------------------------------------------------------------------

	class	LFileDesignator {
		public:
			#include <LFileDesignator.i>

		protected:
			StandardFileReply			mSFReply;
			mutable StAEDescriptor		mFileDescList;
			FSSpec						mDefaultLocation;
			bool						mSelectDefault;
	};

	// -----------------------------------------------------------------------
	//	User data for StandaraFile callback functions

	struct	SHookUserData {
		StandardFileReply*		reply;
		FSSpec*					defaultSpec;
		bool					selectDefault;
	};

	// -----------------------------------------------------------------------
	//	Callback functions for selecting a Directory

	pascal Boolean	FileFilter_Directories(
							CInfoPBPtr		inPB,
							void*			ioDataPtr);

	pascal short	DialogHook_SelectDirectory(
							short			inItem,
							DialogPtr		inDialog,
							void*			ioDataPtr);

	void			SetSelectButtonTitle(
							ControlHandle	inButtonH,
							const Rect&		inButtonRect,
							ConstStringPtr	inDirName);

	bool			GetNewFolderName(
							Str255			outFolderName);

	pascal Boolean	EventFilter_NewFolder(
							DialogPtr			inDialog,
							EventRecord*		inMacEvent,
							DialogItemIndex*	outItemHit);

	// -----------------------------------------------------------------------
	//	Callback functions for selecting a Volume

	pascal Boolean	FileFilter_Volumes(
							CInfoPBPtr		inPB,
							void*			ioDataPtr);

	pascal short	DialogHook_SelectVolume(
							short			inItem,
							DialogPtr		inDialog,
							void*			ioDataPtr);

	// -----------------------------------------------------------------------
	//	Callback function for setting the initial location

	pascal short	DialogHook_DefaultLocation(
							short			inItem,
							DialogPtr		inDialog,
							void*			ioDataPtr);
}


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
