// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScroller.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LScroller
#define _H_LScroller
#pragma once

#include <LView.h>
#include <LListener.h>

#include <Controls.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStdControl;

#pragma options align=mac68k

typedef struct SScrollerInfo {
	SInt16	horizBarLeftIndent;
	SInt16	horizBarRightIndent;
	SInt16	vertBarTopIndent;
	SInt16	vertBarBottomIndent;
	PaneIDT	scrollingViewID;
} SScrollerInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LScroller : public LView,
					public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('scrl') };

						LScroller();

						LScroller( const LScroller& inOriginal );

						LScroller(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								SInt16				inHorizBarLeftIndent,
								SInt16				inHorizBarRightIndent,
								SInt16				inVertBarTopIndent,
								SInt16				inVertBarBottomIndent,
								LView*				inScrollingView);

						LScroller( LStream* inStream );

	virtual				~LScroller();

	Boolean				HasVerticalScrollBar() const
							{ return mVerticalBar != nil; }

	Boolean				HasHorizontalScrollBar() const
							{ return mHorizontalBar != nil; }
							
	LView*				GetScrollingView() const
							{ return mScrollingView; }

	virtual void		InstallView( LView* inScrollingView );

	virtual void		ExpandSubPane(
								LPane*			inSub,
								Boolean			inExpandHoriz,
								Boolean			inExpandVert);

	virtual void		AdjustScrollBars();

	virtual void		ResizeFrameBy(
								SInt16			inWidthDelta,
								SInt16			inHeightDelta,
								Boolean			inRefresh);
								
	virtual void		RestorePlace( LStream* inPlace );

	virtual void		SubImageChanged( LView* inSubView );

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

	virtual void		VertScroll( SInt16 inPart );

	virtual void		HorizScroll( SInt16 inPart );

	static pascal void	VertSBarAction(
								ControlHandle	inMacControl,
								SInt16			inPart);

	static pascal void	HorizSBarAction(
								ControlHandle	inMacControl,
								SInt16			inPart);

protected:
	virtual void		FinishCreateSelf();
	
	virtual void		DrawSelf();

	virtual void		ActivateSelf();

	virtual void		DeactivateSelf();

private:
	void				MakeScrollBars(
								SInt16		inHorizBarLeftIndent,
								SInt16		inHorizBarRightIndent,
								SInt16		inVertBarTopIndent,
								SInt16		inVertBarBottomIndent);
protected:
	LView*			mScrollingView;
	LStdControl*	mVerticalBar;
	LStdControl*	mHorizontalBar;
	PaneIDT			mScrollingViewID;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
