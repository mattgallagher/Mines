// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScrollerView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	A ScrollerView controls the position of another View and may have a
//	horizontal and/or vertical scroll bar

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LScrollerView.h>
#include <LScrollBar.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>

	
PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LScrollerView							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LScrollerView::LScrollerView()
{
	mScrollingView		= nil;
	mScrollingViewID	= PaneIDT_Undefined;
	mVerticalBar		= nil;
	mHorizontalBar		= nil;
	mBarThickness		= 16;
	mIsTrackingScroll	= false;
}


// ---------------------------------------------------------------------------
//	¥ LScrollerView							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LScrollerView::LScrollerView(			// [+++ Stub Implementation]
	const LScrollerView&	inOriginal)

	: LView(inOriginal),
	  LListener(inOriginal)
{
	mScrollingView		= nil;
	mVerticalBar		= nil;
	mHorizontalBar		= nil;
	mBarThickness		= inOriginal.mBarThickness;
	mIsTrackingScroll	= false;
}


// ---------------------------------------------------------------------------
//	¥ LScrollerView							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LScrollerView::LScrollerView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	SInt16				inHorizBarLeftIndent,
	SInt16				inHorizBarRightIndent,
	SInt16				inVertBarTopIndent,
	SInt16				inVertBarBottomIndent,
	SInt16				inBarThickness,
	LView*				inScrollingView,
	bool				inLiveScrolling)

	: LView(inPaneInfo, inViewInfo)
{
	mScrollingViewID	= PaneIDT_Undefined;
	mScrollingView		= nil;
	mBarThickness		= inBarThickness;
	mIsTrackingScroll	= false;
	mVerticalBar		= nil;
	mHorizontalBar		= nil;

	try {
		MakeScrollBars(inHorizBarLeftIndent, inHorizBarRightIndent,
					   inVertBarTopIndent, inVertBarBottomIndent,
					   inLiveScrolling);
	}

	catch (...) {
		delete mVerticalBar;
		delete mHorizontalBar;
		throw;
	}

	if (inScrollingView != nil) {
		mScrollingViewID = inScrollingView->GetPaneID();
		InstallView(inScrollingView);
	}
}


// ---------------------------------------------------------------------------
//	¥ LScrollerView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LScrollerView::LScrollerView(
	LStream*	inStream)

	: LView(inStream)
{
	SScrollerViewInfo	scrollerInfo;
	*inStream >> scrollerInfo.horizBarLeftIndent;
	*inStream >> scrollerInfo.horizBarRightIndent;
	*inStream >> scrollerInfo.vertBarTopIndent;
	*inStream >> scrollerInfo.vertBarBottomIndent;
	*inStream >> scrollerInfo.scrollingViewID;

	Boolean		liveScrolling;
	*inStream >> liveScrolling;

		// ScrollingView has not yet been created, since SuperViews are
		// created before their subviews when building Panes from a Stream.
		// Therefore, we store the ID of the ScrollingView so that the
		// FinishCreateSelf function can set up the proper connections.

	mScrollingViewID	= scrollerInfo.scrollingViewID;
	mScrollingView		= nil;
	mIsTrackingScroll	= false;

		// Overload meaning of image width to be the thickness of
		// the scroll bars. ScrollerView doesn't use image size.

	mBarThickness = (SInt16) mImageSize.width;
	if (mBarThickness <= 1) {
		mBarThickness = 16;
	}
	mImageSize.width = 1;

	mVerticalBar = nil;
	mHorizontalBar = nil;

	try {
		MakeScrollBars(scrollerInfo.horizBarLeftIndent,
					   scrollerInfo.horizBarRightIndent,
					   scrollerInfo.vertBarTopIndent,
					   scrollerInfo.vertBarBottomIndent,
					   liveScrolling);
	}

	catch (...) {
		delete mVerticalBar;
		delete mHorizontalBar;
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LScrollerView						Destructor				  [public]
// ---------------------------------------------------------------------------

LScrollerView::~LScrollerView()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ MakeScrollBars												 [private]
// ---------------------------------------------------------------------------
//	Create StdControl objects for the horizontal and/or vertical ScrollBars
//	of a Scroller

void
LScrollerView::MakeScrollBars(
	SInt16			inHorizBarLeftIndent,
	SInt16			inHorizBarRightIndent,
	SInt16			inVertBarTopIndent,
	SInt16			inVertBarBottomIndent,
	bool			inLiveScrolling)
{
	SPaneInfo	barInfo;				// Common information for ScrollBars
	barInfo.visible		= true;
	barInfo.enabled		= true;
	barInfo.userCon		= 0;
	barInfo.superView	= this;
	
	SInt32	overlap = 0;
	
	#if TARGET_API_MAC_CARBON			// Classic scroll bars always have a 1 pixel
										//   frame at the ends, which overlaps the
										//   frame of the scroller. On Carbon, scroll
										//   bars may draw differently, and we can get
										//   the theme metric to determine the size
										//   of the overlap.
		SInt32	metric;
		OSStatus status = ::GetThemeMetric(kThemeMetricScrollBarOverlap, &metric);
		
		if (status == noErr) {
			overlap = metric - 1;
		}
		
	#endif

	if (inHorizBarLeftIndent >= 0) {
										// Create Horizontal ScrollBar
		barInfo.paneID	= PaneIDT_HorizontalScrollBar;
		barInfo.width	= (SInt16) (mFrameSize.width - inHorizBarLeftIndent -
										   inHorizBarRightIndent + 2 * overlap);
		barInfo.height	= mBarThickness;
		barInfo.left	= inHorizBarLeftIndent - overlap;
		barInfo.top		= mFrameSize.height - mBarThickness;

		barInfo.bindings.left	= true;
		barInfo.bindings.right	= true;
		barInfo.bindings.top	= false;
		barInfo.bindings.bottom	= true;

		mHorizontalBar = new LScrollBar(barInfo, msg_Nothing,
											0, 0, 0, inLiveScrolling);
		mHorizontalBar->AddListener(this);
	}

	if (inVertBarTopIndent >= 0) {
										// Create Vertical ScrollBar
		barInfo.paneID	= PaneIDT_VerticalScrollBar;
		barInfo.width	= mBarThickness;
		barInfo.height	= (SInt16) (mFrameSize.height - inVertBarTopIndent -
											 inVertBarBottomIndent + 2 * overlap);
		barInfo.left	= mFrameSize.width - mBarThickness;
		barInfo.top		= inVertBarTopIndent - overlap;

		barInfo.bindings.left	= false;
		barInfo.bindings.right	= true;
		barInfo.bindings.top	= true;
		barInfo.bindings.bottom	= true;

		mVerticalBar = new LScrollBar(barInfo, msg_Nothing,
										0, 0, 0, inLiveScrolling);
		mVerticalBar->AddListener(this);
	}

	SetDefaultAttachable(this);		// Reset so Attachments don't get
									//   attached to the ScrollBars
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------
//	Finish creation of a Scroller by installing its ScrollingView

void
LScrollerView::FinishCreateSelf()
{
	CalcRevealedRect();

	LView*	scrollingView = dynamic_cast<LView*>
									(FindPaneByID(mScrollingViewID));
	if (scrollingView != nil) {
									// Prevent scroll bars from automatically
									//   drawing while building ScrollerView
		StPaneVisibleState		hideVert(mVerticalBar, triState_Off);
		StPaneVisibleState		hideHoriz(mHorizontalBar, triState_Off);

		InstallView(scrollingView);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LScrollerView::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
		// Adjust for the one pixel border that PowerPlant scrollers
		// have historically used
	
	::MacInsetRect(&frame, 1, 1);

									// Draw theme-compliant border
	ThemeDrawState	state = kThemeStateInactive;
	
	if (IsActive() and IsEnabled()) {
		state = kThemeStateActive;
	}

	::DrawThemeListBoxFrame(&frame, state);
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------

void
LScrollerView::ActivateSelf()
{
	InvalidateBorder();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LScrollerView::DeactivateSelf()
{
	InvalidateBorder();
}


// ---------------------------------------------------------------------------
//	¥ InvalidateBorder											   [protected]
// ---------------------------------------------------------------------------
//	Force redraw of border surrounding scroller

void
LScrollerView::InvalidateBorder()
{
	if (IsVisible()) {

		Rect	structFrame;
		Rect	frame;
		
		if ( CalcPortStructureRect(structFrame) &&
			 CalcPortFrameRect(frame)) {

			StRegion	borderRgn(structFrame);

			::MacInsetRect(&frame, 1, 1);

			borderRgn -= frame;

			InvalPortRgn(borderRgn);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InstallView													  [public]
// ---------------------------------------------------------------------------
//	Install a Scrolling View within this Scroller

void
LScrollerView::InstallView(
	LView*	inScrollingView)
{
	mScrollingView = inScrollingView;
	AdjustScrollBars();
}


// ---------------------------------------------------------------------------
//	¥ ExpandSubPane													  [public]
// ---------------------------------------------------------------------------
//	Expand a SubPane, which should be the Scroller's ScrollingView, to
//	fill the interior of a Scroller

void
LScrollerView::ExpandSubPane(
	LPane*		inSub,
	Boolean		inExpandHoriz,
	Boolean		inExpandVert)
{
	SDimension16	subSize;
	inSub->GetFrameSize(subSize);

	SPoint32		subLocation;
	inSub->GetFrameLocation(subLocation);

	if (inExpandHoriz) {
		subSize.width = (SInt16) (mFrameSize.width - 2);
		if (mVerticalBar != nil) {
			subSize.width -= (SInt16) (mBarThickness - 1);
		}
		subLocation.h = 1;
	} else {
		subLocation.h -= mFrameLocation.h;
	}

	if (inExpandVert) {
		subSize.height = (SInt16) (mFrameSize.height - 2);
		if (mHorizontalBar != nil) {
			subSize.height -= (SInt16) (mBarThickness - 1);
		}
		subLocation.v = 1;
	} else {
		subLocation.v -= mFrameLocation.v;
	}

	inSub->PlaceInSuperFrameAt(subLocation.h, subLocation.v, Refresh_No);
	inSub->ResizeFrameTo(subSize.width, subSize.height, Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ AdjustScrollBars												  [public]
// ---------------------------------------------------------------------------
//	Adjust the ScrollBars (value, min, and max) according to the current
//	state of the Scroller and ScrollingView

void
LScrollerView::AdjustScrollBars()
{
	if (mScrollingView == nil) {		// There's nothing to scroll
		if (mVerticalBar != nil) {
			mVerticalBar->SetMinValue(0);
			mVerticalBar->SetMaxValue(0);
		}

		if (mHorizontalBar != nil) {
			mHorizontalBar->SetMinValue(0);
			mHorizontalBar->SetMaxValue(0);
		}

		return;
	}

	if (mIsTrackingScroll) {			// Don't adjust while live tracking
		return;
	}

	SPoint32		scrollUnit;
	SDimension16	scrollFrameSize;
	SDimension32	scrollImageSize;
	SPoint32		scrollPosition;
	mScrollingView->GetScrollUnit(scrollUnit);
	mScrollingView->GetFrameSize(scrollFrameSize);
	mScrollingView->GetImageSize(scrollImageSize);
	mScrollingView->GetScrollPosition(scrollPosition);

	if (mVerticalBar != nil) {

		// vertDiff is the number of pixels of that the image can scroll.
		// Usually, it's the difference between the height of the Image
		// and the Frame. However, we also need to handle the case where
		// the top of the Image is below the top of the Frame.

		// There are 4 cases:
		//
		//	1) Image Top above Frame Top
		//	   Image Bottom below Frame Bottom
		//			vertDiff is the sum of of the two (positive) distances
		//			vertVal is distance between the tops
		//
		//	2) Image Top above Frame Top
		//	   Image Bottom above Frame Bottom
		//			vertDiff is the distance between the tops
		//			vertVal is vertDiff
		//
		//	3) Image Top below Frame Top
		//	   Image Bottom below Frame Bottom
		//			vertDiff is the maximum of the two distances
		//			vertVal is zero
		//
		//	4) Image Top below Frame Top
		//	   Image Bottom above Frame Bottom
		//			vertDiff is the distance between the tops
		//			vertVal is zero
		//

		SInt32	vertDiff = scrollPosition.v;	// Absolute value of
		if (vertDiff < 0) {						//   difference between tops
			vertDiff = -vertDiff;				//   of Image and Frame
		}

		SInt32	belowBottom = scrollImageSize.height - scrollPosition.v -
								scrollFrameSize.height;

		if (belowBottom > 0) {					// Image extends below Frame
			if (scrollPosition.v >= 0) {
				vertDiff += belowBottom;
			} else if (belowBottom > vertDiff) {
				vertDiff = belowBottom;			// Image top below Frame top
			}
		}

		SInt32	vertMax = 0;
		if (vertDiff > 0) {						// Scale by scroll unit
			vertMax = (vertDiff + scrollUnit.v - 1) / scrollUnit.v;
		}

		SInt32	vertVal = 0;
		if (scrollPosition.v > 0) {				// Scale by scroll unit
			vertVal = (scrollPosition.v + scrollUnit.v - 1) / scrollUnit.v;
		}

		mVerticalBar->SetMaxValue(vertMax);
		mVerticalBar->SetValue(vertVal);
		mVerticalBar->SetScrollViewSize(
							(scrollFrameSize.height + scrollUnit.v - 1) /
								scrollUnit.v);
	}

	if (mHorizontalBar != nil) {

			// Same as vertical case, except use left, right, and width

		SInt32	horizDiff = scrollPosition.h;
		if (horizDiff < 0) {
			horizDiff = -horizDiff;
		}

		SInt32	beyondRight = scrollImageSize.width - scrollPosition.h -
								scrollFrameSize.width;
		if (beyondRight > 0) {
			if (scrollPosition.h >= 0) {
				horizDiff += beyondRight;
			} else if (beyondRight > horizDiff) {
				horizDiff = beyondRight;
			}
		}

		SInt32	horizMax = 0;
		if (horizDiff > 0) {
			horizMax = (horizDiff + scrollUnit.h - 1) / scrollUnit.h;
		}

		SInt32	horizVal = 0;
		if (scrollPosition.h > 0) {
			horizVal = (scrollPosition.h + scrollUnit.h - 1) / scrollUnit.h;
		}

		mHorizontalBar->SetMaxValue(horizMax);
		mHorizontalBar->SetValue(horizVal);
		mHorizontalBar->SetScrollViewSize(
								(scrollFrameSize.width + scrollUnit.h - 1) /
									scrollUnit.h);
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcRevealedRect
// ---------------------------------------------------------------------------
//	Calculate the portion of the Frame which is revealed through the
//	Frames of all SuperViews. RevealedRect is in Port coordinates.

void
LScrollerView::CalcRevealedRect()
{
		// To accommodate controls which draw outside their Frame,
		// this function calculates the portion of its Structure rect
		// that is revealed
	
	if (CalcPortStructureRect(mRevealedRect)) {
									// Frame is in QD Space
		if (mSuperView != nil) {	// Intersect Frame with RevealedRect
									//   of SuperView
			Rect	superRevealed;
			mSuperView->GetRevealedRect(superRevealed);
			::SectRect(&superRevealed, &mRevealedRect, &mRevealedRect);
		}

	} else {						// Frame not in QD Space
		mRevealedRect.left =		//   so RevealedRect is empty
			mRevealedRect.top =
			mRevealedRect.right =
			mRevealedRect.bottom = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcPortStructureRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the ScrollerView's Structure Rectangle in Port coordinates
//
//	An Appearance Manager border may draw outside the Frame of a ScrollerView
//
//	Returns true if the Rect is within QuickDraw space (16-bit)
//	Returns false if the Rect is outside QuickDraw space
//		and outRect is unchanged

bool
LScrollerView::CalcPortStructureRect(
	Rect&	outRect) const
{
	bool	isInQDSpace = CalcPortFrameRect(outRect);

	if (isInQDSpace) {
	
			// Adjust Frame to account for border. On Platinum Theme, the
			// border is two pixels. For Carbon Targets, we can ask the
			// OS for the border size. We subtract one to compensate for
			// the one pixel border which PP scrollers historically used.
	
		SInt32		outset = 1;			// Two minus one
		
		#if TARGET_API_MAC_CARBON		// On Carbon, we can ask the system
										//   for the border outset.
			SInt32	metric;
			OSStatus	status = ::GetThemeMetric(kThemeMetricListBoxFrameOutset, &metric);

			if (status == noErr) {
				outset = metric - 1;
			}
			
		#endif
		
		::MacInsetRect(&outRect, (SInt16) -outset, (SInt16) -outset);
	}
	
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortExposedRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the exposed rectangle of a Pane and return whether that
//	rectangle is empty. The rectangle is in Port coordinates.

Boolean
LScrollerView::CalcPortExposedRect(
	Rect&	outRect,						// In Port coords
	bool	inOKIfHidden) const
{
	bool exposed = ((mVisible == triState_On) || inOKIfHidden)  &&
				   CalcPortStructureRect(outRect)  &&
				   (mSuperView != nil);

	if (exposed) {
		Rect	superRevealed;
		mSuperView->GetRevealedRect(superRevealed);

		exposed = ::SectRect(&outRect, &superRevealed, &outRect);
	}

	return exposed;
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts

void
LScrollerView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
		// Let LView do all the work. All Scroller has to do is
		// adjust the ScrollBars to account for the new size
		// of the Scroller and resize its Image so it matches
		// its Frame size.

	LView::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);

	{		// Prevent scroll bars from drawing while adjusting them
		StPaneVisibleState		hideVert(mVerticalBar, triState_Off);
		StPaneVisibleState		hideHoriz(mHorizontalBar, triState_Off);

		AdjustScrollBars();
	}

	ResizeImageBy(inWidthDelta, inHeightDelta, Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace
// ---------------------------------------------------------------------------
//	Read size and location information stored in a Stream by the
//	SavePlace() function

void
LScrollerView::RestorePlace(
	LStream		*inPlace)
{
	LView::RestorePlace(inPlace);		// Restore info for this View
	
	AdjustScrollBars();					// Keep scroll bars in synch
}


// ---------------------------------------------------------------------------
//	¥ SubImageChanged												  [public]
// ---------------------------------------------------------------------------
//	Adjust state when the Image of the ScrollingView changes
//
//	ScrollBar settings depend on the ScrollingView Image, so adjust them
//	to match the current state.

void
LScrollerView::SubImageChanged(
	LView*	inSubView)
{
	if (inSubView == mScrollingView) {
		AdjustScrollBars();
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------
//	Respond to messages from Broadcasters
//
//	ScrollerView listens to its ScrollBars, which broadcast messages when
//	manipulated

void
LScrollerView::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	if (inMessage == msg_ScrollAction) {
		LScrollBar::SScrollMessage	*scrollMsg =
						static_cast<LScrollBar::SScrollMessage*>(ioParam);

		if (scrollMsg->hotSpot == kControlIndicatorPart) {

			ThumbScroll(scrollMsg->scrollBar, scrollMsg->value);

		} else {
			if (scrollMsg->scrollBar == mHorizontalBar) {
				HorizScroll(scrollMsg->hotSpot);

			} else if (scrollMsg->scrollBar == mVerticalBar) {
				VertScroll(scrollMsg->hotSpot);
			}
		}

	} else if (inMessage == msg_ThumbDragged) {
		LScrollBar		*theSBar = static_cast<LScrollBar*>(ioParam);
		ThumbScroll(theSBar, theSBar->GetValue());
		AdjustScrollBars();

	} else if (inMessage == msg_ControlClicked) {
		AdjustScrollBars();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ VertScroll													  [public]
// ---------------------------------------------------------------------------
//	Function called to scroll vertically while clicking and holding inside
//	the vertical scroll bar

void
LScrollerView::VertScroll(
	SInt16		inPart)
{
	if ( (mVerticalBar == nil)  ||  (mScrollingView == nil) ) {
		return;
	}

	SPoint32		scrollUnit;
	SDimension16	scrollFrameSize;
	SInt32			vertUnits = 0;

	mScrollingView->GetScrollUnit(scrollUnit);
	mScrollingView->GetFrameSize(scrollFrameSize);

	switch (inPart) {				// Determine how much to scroll

		case kControlUpButtonPart:		// Scroll up one unit
			vertUnits = -1;
			break;

		case kControlDownButtonPart:	// Scroll down one unit
			vertUnits = 1;
			break;

		case kControlPageUpPart:		// Scroll up by Frame height
										//   less one unit of overlap
			vertUnits = 1 - (scrollFrameSize.height / scrollUnit.v);
			if (vertUnits >= 0) {
				vertUnits = -1;
			}
			break;

		case kControlPageDownPart:		// Scroll down by Frame height
										//   less one unit of overlap
			vertUnits = (scrollFrameSize.height / scrollUnit.v) - 1;
			if (vertUnits <= 0) {
				vertUnits = 1;
			}
			break;
	}

	if (vertUnits != 0) {			// Set ScrollBar value and scroll the view
		StValueChanger<bool>	tracking(mIsTrackingScroll, true);
		mVerticalBar->IncrementValue(vertUnits);
		mScrollingView->ScrollPinnedImageBy(0, vertUnits * scrollUnit.v,
												Refresh_Yes);
	}
}


// ---------------------------------------------------------------------------
//	¥ HorizScroll													  [public]
// ---------------------------------------------------------------------------
//	Function called to scroll horizontally while clicking and holding inside
//	the horizontal scroll bar

void
LScrollerView::HorizScroll(
	SInt16		inPart)
{
	if ( (mHorizontalBar == nil)  ||  (mScrollingView == nil) ) {
		return;
	}

	SPoint32		scrollUnit;
	SDimension16	scrollFrameSize;
	SInt32			horizUnits = 0;

	mScrollingView->GetScrollUnit(scrollUnit);
	mScrollingView->GetFrameSize(scrollFrameSize);

	switch (inPart) {				// Determine how much to scroll

		case kControlUpButtonPart:		// Scroll left one unit
			horizUnits = -1;
			break;

		case kControlDownButtonPart:	// Scroll right one unit
			horizUnits = 1;
			break;

		case kControlPageUpPart:		// Scroll left by Frame width
										//   less one unit of overlap
			horizUnits = 1 - (scrollFrameSize.width / scrollUnit.h);
			if (horizUnits >= 0) {
				horizUnits = -1;
			}
			break;

		case kControlPageDownPart:		// Scroll right by Frame width
										//   less one unit of overlap
			horizUnits = (scrollFrameSize.width / scrollUnit.h) - 1;
			if (horizUnits <= 0) {
				horizUnits = 1;
			}
			break;
	}

	if (horizUnits != 0) {			// Set ScrollBar value and scroll the view
		StValueChanger<bool>	tracking(mIsTrackingScroll, true);
		mHorizontalBar->IncrementValue(horizUnits);
		mScrollingView->ScrollPinnedImageBy(horizUnits * scrollUnit.h,
												0, Refresh_Yes);
	}
}


// ---------------------------------------------------------------------------
//	¥ ThumbScroll													  [public]
// ---------------------------------------------------------------------------
//	Function called to scroll while dragging the thumb

void
LScrollerView::ThumbScroll(
	LScrollBar*		inScrollBar,
	SInt32			inThumbValue)
{
		SPoint32		scrollPosition;			// Current position
		mScrollingView->GetScrollPosition(scrollPosition);

			// Determine new position based on the scroll direction,
			// thumb value, and scroll unit

		SPoint32		scrollUnit;
		mScrollingView->GetScrollUnit(scrollUnit);

		if (inScrollBar == mHorizontalBar) {
			scrollPosition.h = inThumbValue * scrollUnit.h;

		} else if (inScrollBar == mVerticalBar) {
			scrollPosition.v = inThumbValue * scrollUnit.v;
		}

			// Scroll View to the new position. The scrolling view will
			// call back to us to adjust the scroll bars as a result of
			// calling ScrollPinnedImageTo(). Since we are tracking a live
			// thumb, the scroll bars are already in the right state. So we
			// set the mIsTrackingScroll flag so that the resulting call to
			// AdjustScrollBars() will do nothing.

		StValueChanger<bool>	tracking(mIsTrackingScroll, true);
		mScrollingView->ScrollPinnedImageTo(scrollPosition.h,
											scrollPosition.v, Refresh_Yes);
}


PP_End_Namespace_PowerPlant
