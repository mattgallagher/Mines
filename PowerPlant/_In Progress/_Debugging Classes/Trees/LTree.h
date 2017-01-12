// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTree.h						PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	See LTree.cp for more information

#ifndef _H_LTree
#define _H_LTree
#pragma once

#include <PP_Debug.h>
#include <LView.h>
#include <LPeriodical.h>
#include <UTextTraits.h>
#include <LBroadcaster.h>
#include <LListener.h>
#include <LDebugStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

extern const RGBColor	Color_Red;
extern const RGBColor	Color_DarkGreen;


const PaneIDT	Tree_TextPane			=	FOUR_CHAR_CODE('TrPn');

#ifndef PP_DumpTree_Filename
	#define PP_DumpTree_Filename		"\pTree Dump"
#endif

class LTree :	public LView,
				public LBroadcaster,
				public LPeriodical {
public:
			enum { class_ID = FOUR_CHAR_CODE('tree') };

								LTree();
								LTree(
									const LTree&		inOriginal);
								LTree(
									const SPaneInfo&	inPaneInfo,
									const SViewInfo&	inViewInfo,
									UInt32				inThreshold,
									ResIDT				inTxtrID,
									bool				inDisplayExtraInfo = false);
								LTree(
									LStream*			inStream);

	virtual						~LTree();

	virtual	void				Update();

	virtual	void				DumpTree(
									EFlushLocation		inFlushLocation = flushLocation_File,
									ConstStringPtr		inFileName = PP_DumpTree_Filename);

	virtual	void				SpendTime(
									const EventRecord&	inMacEvent);
	virtual	void				StartRepeating();
	virtual	void				StopRepeating();

			void				SetThreshold(
									UInt32				inThreshold)
									{
										mThreshold = inThreshold;
									}
			UInt32				GetThreshold() const { return mThreshold; }

			bool				IsRunning() const { return mIsRepeating; }

			LView*				GetTextPane() const
									{
										return mTextPane;
									}

			bool				GetDisplayExtraInfo() const
									{
										return mDisplayExtraInfo;
									}
			void				SetDisplayExtraInfo(
									bool				inDisplayExtraInfo)
									{
										mDisplayExtraInfo = inDisplayExtraInfo;
									}

	virtual	void				SubImageChanged(
									LView*				inSubView);

	virtual	void				ScrollImageBy(
									SInt32				inLeftDelta,
									SInt32				inTopDelta,
									Boolean				inRefresh);

protected:
			UInt32				mLastTime;
			SInt16				mLastTop;
			UInt32				mThreshold;
			bool				mIsRepeating;
			TextTraitsRecord	mBaseTraits;
			bool				mDisplayExtraInfo;

			LView*				mTextPane;
			Handle				mTreeTextH;
			StScrpHandle		mTreeTextStyleH;

	virtual	void				FinishCreateSelf();
	virtual	void				HideSelf();
	virtual	void				ShowSelf();

	virtual	void				BuildTree(
									bool				inGenerateExtraInfo);
	virtual	void				InsertTree();

				// Nested classes
				//
				// Since LBroadcaster and LListener do not provide a means for
				// accessing their listener/broadcaster lists, these subclasses
				// have been created to access that information. These classes
				// are nested to avoid (ab)use.

		class LPeekBroadcaster : public LBroadcaster
		{
		public:
			const TArray<LListener*>& GetListeners() const { return LBroadcaster::mListeners; }
		};

		class LPeekListener : public LListener
		{
		public:
			const TArray<LBroadcaster*>& GetBroadcasters() const { return LListener::mBroadcasters; }
		};

private:
			void				InitTree(
									ResIDT				inTxtrID);

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LTree
