// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScrollerView.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LScrollerView
#define _H_LScrollerView
#pragma once

#include <LView.h>
#include <LListener.h>
#include <Controls.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LScrollBar;

#pragma options align=mac68k

struct SScrollerViewInfo {
	SInt16	horizBarLeftIndent;
	SInt16	horizBarRightIndent;
	SInt16	vertBarTopIndent;
	SInt16	vertBarBottomIndent;
	PaneIDT	scrollingViewID;
};

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LScrollerView : public LView,
						public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('sclv') };

						LScrollerView();

						LScrollerView( const LScrollerView& inOriginal );

						LScrollerView(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								SInt16				inHorizBarLeftIndent,
								SInt16				inHorizBarRightIndent,
								SInt16				inVertBarTopIndent,
								SInt16				inVertBarBottomIndent,
								SInt16				inBarThickness,
								LView*				inScrollingView,
								bool				inLiveScrolling);

						LScrollerView( LStream* inStream );

	virtual				~LScrollerView();

	bool				HasVerticalScrollBar() const
							{ return (mVerticalBar != nil); }

	bool				HasHorizontalScrollBar() const
							{ return (mHorizontalBar != nil); }
							
	LView*				GetScrollingView() const
							{ return mScrollingView; }

	virtual void		InstallView( LView* inScrollingView );

	virtual void		ExpandSubPane(
								LPane*		inSub,
								Boolean		inExpandHoriz,
								Boolean		inExpandVert);

	virtual void		AdjustScrollBars();

	virtual void		CalcRevealedRect();
	
	bool				CalcPortStructureRect( Rect& outRect ) const;
	
	virtual Boolean		CalcPortExposedRect(
								Rect&		outRect,
								bool		inOKIfHidden = false) const;

	virtual void		ResizeFrameBy(
								SInt16		inWidthDelta,
								SInt16		inHeightDelta,
								Boolean		inRefresh);
								
	virtual void		RestorePlace( LStream* inPlace );

	virtual void		SubImageChanged( LView* inSubView );

	virtual void		ListenToMessage(
								MessageT	inMessage,
								void*		ioParam);

	virtual void		VertScroll( SInt16 inPart );

	virtual void		HorizScroll( SInt16 inPart );

	virtual void		ThumbScroll(
								LScrollBar*		inScrollBar,
								SInt32			inThumbValue);

protected:
	virtual void		FinishCreateSelf();
	
	virtual void		DrawSelf();
	
	virtual void		ActivateSelf();
	
	virtual void		DeactivateSelf();

	virtual void		InvalidateBorder();

private:
	void				MakeScrollBars(
								SInt16		inHorizBarLeftIndent,
								SInt16		inHorizBarRightIndent,
								SInt16		inVertBarTopIndent,
								SInt16		inVertBarBottomIndent,
								bool		inLiveScrolling);

protected:
	LView*			mScrollingView;
	LScrollBar*		mVerticalBar;
	LScrollBar*		mHorizontalBar;
	PaneIDT			mScrollingViewID;
	SInt16			mBarThickness;
	bool			mIsTrackingScroll;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
