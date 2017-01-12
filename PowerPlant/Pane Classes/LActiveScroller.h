// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LActiveScroller.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implements active scrolling of a view during the tracking of
//	scroll bars thumbs

#ifndef _H_LActiveScroller
#define _H_LActiveScroller
#pragma once

#include <LScroller.h>
#include <LStdControl.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

const SInt16	kWidthOfScrollArrow		=	24;
const SInt16	kThumbTrackWidthSlop 	= 	25;
const SInt16	kThumbTrackLengthSlop 	= 	113;

// ---------------------------------------------------------------------------

class LActiveScroller : public LScroller {
public:
	enum { class_ID = FOUR_CHAR_CODE('ascr') };

						LActiveScroller();

						LActiveScroller( LStream* inStream );

						LActiveScroller( const LScroller& inOriginal );

						LActiveScroller(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								SInt16				inHorizBarLeftIndent,
								SInt16				inHorizBarRightIndent,
								SInt16				inVertBarTopIndent,
								SInt16				inVertBarBottomIndent,
								LView*				inScrollingView);

	virtual				~LActiveScroller();

	virtual	Boolean		IsTrackingThumb();

	virtual void		AdjustScrollBars();

	virtual void		ListenToMessage(
								MessageT			inMessage,
								void*				ioParam);

protected:
	LStdControl*	mThumbControl;
	SInt32			mTrackBarUnits;
	SInt32			mTrackBarPin;
	SInt32			mTrackBarSize;
	SInt32			mOriginalValue;
	Rect			mTrackRect;
	SInt32			mValueSlop;

	DragGrayRgnUPP	mVertThumbAction;
	DragGrayRgnUPP	mHorizThumbAction;

	virtual	void		AssignThumbProcs();

	virtual	void		StartThumbTracking( LStdControl* inWhichControl );

	virtual	void		EndThumbTracking();

	virtual	void		AdaptToNewThumbValue( SInt32 inNewValue );

	virtual	SInt32 		CalcValueFromPoint( Point inPoint );

	virtual void		CalcTrackBarSize();

	virtual	void		HandleThumbScroll( LStdControl* inWhichControl );

	static pascal void	ActiveThumbScroll();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
