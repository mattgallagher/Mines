// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScroller.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Scroller controls the position of another View and may have a
//	horizontal and/or vertical scroll bar

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LScroller.h>
#include <LStdControl.h>
#include <LStream.h>
#include <UReanimator.h>
#include <UDrawingState.h>
#include <UControlMgr.h>

#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#if TARGET_API_MAC_CARBON

	static StControlActionUPP	sHorizSBarActionUPP(LScroller::HorizSBarAction);

#else

	#if TARGET_RT_MAC_CFM		// For CFM, a UPP is a pointer to a
								//   RoutineDescriptor struct. We use
								//   statics for storage.

		static RoutineDescriptor	sHorizSBarActionDesc =
			BUILD_ROUTINE_DESCRIPTOR( uppControlActionProcInfo,
									  LScroller::HorizSBarAction );

		static ControlActionUPP		sHorizSBarActionUPP =
										&sHorizSBarActionDesc;

	#else						// For non-CFM, a UPP is just a function
								//   pointer
		static ControlActionUPP		sHorizSBarActionUPP = LScroller::HorizSBarAction;
	#endif

#endif

// ---------------------------------------------------------------------------

#if TARGET_API_MAC_CARBON

	static StControlActionUPP	sVertSBarActionUPP(LScroller::VertSBarAction);

#else

	#if TARGET_RT_MAC_CFM		// For CFM, a UPP is a pointer to a
								//   RoutineDescriptor struct. We use
								//   statics for storage.

		static RoutineDescriptor	sVertSBarActionDesc =
			BUILD_ROUTINE_DESCRIPTOR( uppControlActionProcInfo,
									  LScroller::VertSBarAction );

		static ControlActionUPP		sVertSBarActionUPP =
										&sVertSBarActionDesc;

	#else						// For non-CFM, a UPP is just a function
								//   pointer
		static ControlActionUPP		sVertSBarActionUPP = LScroller::VertSBarAction;
	#endif

#endif


// ---------------------------------------------------------------------------
//	¥ LScroller								Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	Default Constructor

LScroller::LScroller()
{
	mScrollingView   = nil;
	mScrollingViewID = PaneIDT_Undefined;
	mVerticalBar	 = nil;
	mHorizontalBar   = nil;
}


// ---------------------------------------------------------------------------
//	¥ LScroller								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LScroller::LScroller(
	const LScroller&	inOriginal)

	: LView(inOriginal),
	  LListener(inOriginal)
{
	mScrollingView		= nil;
	mScrollingViewID	= PaneIDT_Undefined;
	mVerticalBar		= nil;
	mHorizontalBar		= nil;

	SInt16	leftIndent		= -1;			// Flag for no horizontal bar
	SInt16	rightIndent		= 0;
	SInt16	topIndent		= -1;			// Flag for no vertical bar
	SInt16	bottomIndent	= 0;

	Rect	barFrame;
	Rect	frame;
	CalcLocalFrameRect(frame);

	if (inOriginal.mHorizontalBar != nil) {	// Get indents for horizontal bar
		inOriginal.mHorizontalBar->CalcLocalFrameRect(barFrame);
		leftIndent  = (SInt16) (barFrame.left - frame.left);
		rightIndent = (SInt16) (frame.right - barFrame.right);
	}

	if (inOriginal.mVerticalBar != nil) {	// Get indents for vertical bar
		inOriginal.mVerticalBar->CalcLocalFrameRect(barFrame);
		topIndent    = (SInt16) (barFrame.top - frame.top);
		bottomIndent = (SInt16) (frame.bottom - barFrame.bottom);
	}

	try {
		MakeScrollBars( leftIndent, rightIndent,
					    topIndent, 	bottomIndent);
	}

	catch (...) {					// Failed to fully build scroll bars
		delete mVerticalBar;
		delete mHorizontalBar;
		throw;
	}

}


// ---------------------------------------------------------------------------
//	¥ LScroller								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LScroller::LScroller(
	const SPaneInfo	&inPaneInfo,
	const SViewInfo	&inViewInfo,
	SInt16			inHorizBarLeftIndent,
	SInt16			inHorizBarRightIndent,
	SInt16			inVertBarTopIndent,
	SInt16			inVertBarBottomIndent,
	LView			*inScrollingView)
		: LView(inPaneInfo, inViewInfo)
{
	mVerticalBar   = nil;
	mHorizontalBar = nil;

	try {
		MakeScrollBars(inHorizBarLeftIndent, inHorizBarRightIndent,
					   inVertBarTopIndent, inVertBarBottomIndent);
	}

	catch (...) {					// Failed to fully build scroll bars
		delete mVerticalBar;
		delete mHorizontalBar;
		throw;
	}

	mScrollingViewID = PaneIDT_Undefined;
	mScrollingView   = nil;

	if (inScrollingView != nil) {
		mScrollingViewID = inScrollingView->GetPaneID();
		InstallView(inScrollingView);
	}
}


// ---------------------------------------------------------------------------
//	¥ LScroller								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LScroller::LScroller(
	LStream*	inStream)

	: LView(inStream)
{
	SScrollerInfo	scrollerInfo;
	*inStream >> scrollerInfo.horizBarLeftIndent;
	*inStream >> scrollerInfo.horizBarRightIndent;
	*inStream >> scrollerInfo.vertBarTopIndent;
	*inStream >> scrollerInfo.vertBarBottomIndent;
	*inStream >> scrollerInfo.scrollingViewID;

		// ScrollingView has not yet been created, since SuperViews are
		// created before their subviews when building Panes from a Stream.
		// Therefore, we store the ID of the ScrollingView so that the
		// FinishCreateSelf function can set up the proper connections.

	mScrollingViewID = scrollerInfo.scrollingViewID;
	mScrollingView = nil;

	mVerticalBar = nil;
	mHorizontalBar = nil;

	try {
		MakeScrollBars(scrollerInfo.horizBarLeftIndent,
					   scrollerInfo.horizBarRightIndent,
					   scrollerInfo.vertBarTopIndent,
					   scrollerInfo.vertBarBottomIndent);
	}

	catch (...) {					// Failed to fully build scroll bars
		delete mVerticalBar;
		delete mHorizontalBar;
		throw;
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LScroller							Destructor				  [public]
// ---------------------------------------------------------------------------

LScroller::~LScroller()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ MakeScrollBars												 [private]
// ---------------------------------------------------------------------------
//	Create StdControl objects for the horizontal and/or vertical ScrollBars
//	of a Scroller

void
LScroller::MakeScrollBars(
	SInt16			inHorizBarLeftIndent,
	SInt16			inHorizBarRightIndent,
	SInt16			inVertBarTopIndent,
	SInt16			inVertBarBottomIndent)
{
	SPaneInfo	barInfo;				// Common information for ScrollBars
	barInfo.visible = false;			// ScrollBars aren't visible until
	barInfo.enabled = true;				//    Scroller is activated
	barInfo.userCon = 0;
	barInfo.superView = this;

	if (inHorizBarLeftIndent >= 0) {
										// Create Horizontal ScrollBar
		barInfo.paneID	= PaneIDT_HorizontalScrollBar;
		barInfo.width	= (SInt16) (mFrameSize.width - inHorizBarLeftIndent -
										   inHorizBarRightIndent);
		barInfo.height	= 16;
		barInfo.left	= inHorizBarLeftIndent;
		barInfo.top		= (SInt16) (mFrameSize.height - 16);

		barInfo.bindings.left	= true;
		barInfo.bindings.right	= true;
		barInfo.bindings.top	= false;
		barInfo.bindings.bottom = true;


		mHorizontalBar = new LStdControl(barInfo, msg_Nothing, 0, 0, 0,
									scrollBarProc, 0, Str_Empty, (SInt32) this);

		mHorizontalBar->SetActionProc(sHorizSBarActionUPP);
		mHorizontalBar->AddListener(this);
	}

	if (inVertBarTopIndent >= 0) {
										// Create Vertical ScrollBar
		barInfo.paneID	= PaneIDT_VerticalScrollBar;
		barInfo.width	= 16;
		barInfo.height	= (SInt16) (mFrameSize.height - inVertBarTopIndent -
											 inVertBarBottomIndent);
		barInfo.left	= (SInt16) (mFrameSize.width - 16);
		barInfo.top		= inVertBarTopIndent;

		barInfo.bindings.left	= false;
		barInfo.bindings.right	= true;
		barInfo.bindings.top	= true;
		barInfo.bindings.bottom = true;

		mVerticalBar = new LStdControl(barInfo, msg_Nothing, 0, 0, 0,
									scrollBarProc, 0, Str_Empty, (SInt32) this);

		mVerticalBar->SetActionProc(sVertSBarActionUPP);
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
LScroller::FinishCreateSelf()
{
	LView	*scrollingView = dynamic_cast<LView*>(FindPaneByID(mScrollingViewID));

	if (scrollingView != nil) {
									// Prevent scroll bars from automatically
									//   drawing while building Scroller
		StPaneVisibleState		hideVert(mVerticalBar, triState_Off);
		StPaneVisibleState		hideHoriz(mHorizontalBar, triState_Off);

		InstallView(scrollingView);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw a Scroller

void
LScroller::DrawSelf()
{
	Rect	frame;					// Scroller has a one pixel border
	CalcLocalFrameRect(frame);
	::PenNormal();
	ApplyForeAndBackColors();
	::MacFrameRect(&frame);

	if (mVerticalBar != nil) {
		::MoveTo((SInt16) (frame.right - 16), frame.top);
		::MacLineTo((SInt16) (frame.right - 16), (SInt16) (frame.bottom - 1));
	}

	if (mHorizontalBar != nil) {
		::MoveTo(frame.left, (SInt16) (frame.bottom - 16));
		::MacLineTo((SInt16) (frame.right - 1), (SInt16) (frame.bottom - 16));
	}

		// When inactive, ScrollBars are hidden. Just outline
		// the ScrollBar locations with one pixel borders.

	if (!IsActive()) {
		if (mVerticalBar != nil) {
			mVerticalBar->CalcPortFrameRect(frame);
			PortToLocalPoint(topLeft(frame));
			PortToLocalPoint(botRight(frame));
			::MacFrameRect(&frame);
			::MacInsetRect(&frame, 1, 1);
			::EraseRect(&frame);
		}

		if (mHorizontalBar != nil) {
			mHorizontalBar->CalcPortFrameRect(frame);
			PortToLocalPoint(topLeft(frame));
			PortToLocalPoint(botRight(frame));
			::MacFrameRect(&frame);
			::MacInsetRect(&frame, 1, 1);
			::EraseRect(&frame);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------
//	Activate Scroller
//
//	Show ScrollBars that were hidden when Deactivated

void
LScroller::ActivateSelf()
{
	if (mVerticalBar != nil) {
		mVerticalBar->Show();
	}

	if (mHorizontalBar != nil) {
		mHorizontalBar->Show();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------
//	Deactivate Scroller
//
//	According to Mac Human Interface Guidelines, ScrollBars in inactive
//	windows are hidden.

void
LScroller::DeactivateSelf()
{
	if ((mVerticalBar != nil) && mVerticalBar->IsVisible()) {
		mVerticalBar->Hide();			// Hide vertical ScrollBar
		mVerticalBar->DontRefresh(true);
	}

	if ((mHorizontalBar != nil) && mHorizontalBar->IsVisible()) {
		mHorizontalBar->Hide();			// Hide horizontal ScrollBar
		mHorizontalBar->DontRefresh(true);
	}

	if (FocusExposed()) {				// Redraw immediately
		Rect	frame;
		CalcLocalFrameRect(frame);
		if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
			DrawSelf();
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InstallView													  [public]
// ---------------------------------------------------------------------------
//	Install a Scrolling View within this Scroller

void
LScroller::InstallView(
	LView	*inScrollingView)
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
LScroller::ExpandSubPane(
	LPane	*inSub,
	Boolean	inExpandHoriz,
	Boolean	inExpandVert)
{
	SDimension16	subSize;
	inSub->GetFrameSize(subSize);

	SPoint32		subLocation;
	inSub->GetFrameLocation(subLocation);

	if (inExpandHoriz) {
		subSize.width = (SInt16) (mFrameSize.width - 2);
		if (mVerticalBar != nil) {
			subSize.width -= 15;
		}
		subLocation.h = 1;
	} else {
		subLocation.h -= mFrameLocation.h;
	}

	if (inExpandVert) {
		subSize.height = (SInt16) (mFrameSize.height - 2);
		if (mHorizontalBar != nil) {
			subSize.height -= 15;
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
LScroller::AdjustScrollBars()
{
	if (mScrollingView == nil) {
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
	}
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts

void
LScroller::ResizeFrameBy(
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
LScroller::RestorePlace(
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
LScroller::SubImageChanged(
	LView	*inSubView)
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
//	The ScrollBars of a Scroller broadcast a message after the user drags
//	the thumb

void
LScroller::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	if (inMessage == msg_ThumbDragged) {
		LStdControl		*theSBar = static_cast<LStdControl*>(ioParam);
		SInt32			scrollValue = theSBar->GetValue();
		SPoint32		scrollUnit;
		mScrollingView->GetScrollUnit(scrollUnit);
		SPoint32		scrollPosition;
		mScrollingView->GetScrollPosition(scrollPosition);
		if (theSBar == mVerticalBar) {
			scrollPosition.v = scrollValue * scrollUnit.v;
		} else if (theSBar == mHorizontalBar) {
			scrollPosition.h = scrollValue * scrollUnit.h;
		}
		mScrollingView->ScrollPinnedImageTo(scrollPosition.h, scrollPosition.v,
										Refresh_Yes);
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
LScroller::VertScroll(
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

		case kControlUpButtonPart:	// Scroll up one unit
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

	if (vertUnits != 0) {
		mVerticalBar->IncrementValue(vertUnits);
		mScrollingView->ScrollPinnedImageBy(0, vertUnits * scrollUnit.v,
												Refresh_Yes);
									// Scrolling changes the focus. The
									// Mac Control Manager will be very
									// unhappy if we don't restore focus
									// to the ScrollBar
		mVerticalBar->FocusDraw();
	}
}


// ---------------------------------------------------------------------------
//	¥ HorizScroll													  [public]
// ---------------------------------------------------------------------------
//	Function called to scroll horizontally while clicking and holding inside
//	the horizontal scroll bar

void
LScroller::HorizScroll(
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

		case kControlUpButtonPart:	// Scroll left one unit
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

	if (horizUnits != 0) {
		mHorizontalBar->IncrementValue(horizUnits);
		mScrollingView->ScrollPinnedImageBy(horizUnits * scrollUnit.h,
												0, Refresh_Yes);
									// Scrolling changes the focus, the
									// Mac Control Manager will be very
									// unhappy if we don't restore focus
									// to the ScrollBar
		mHorizontalBar->FocusDraw();
	}
}


// ---------------------------------------------------------------------------
//	¥ VertSBarAction											   [protected]
// ---------------------------------------------------------------------------
//	Toolbox callback function for the action to take will tracking a mouse
//	click in a vertical scroll bar

pascal void
LScroller::VertSBarAction(
	ControlHandle	inMacControl,
	SInt16			inPart)
{
	try {
		LScroller	*theScroller =
							(LScroller *) ::GetControlReference(inMacControl);
		theScroller->VertScroll(inPart);
	}

	catch (...) { }			// Don't propagate exception into Toolbox!
}


// ---------------------------------------------------------------------------
//	¥ HorizSBarAction											   [protected]
// ---------------------------------------------------------------------------
//	Toolbox callback function for the action to take will tracking a mouse
//	click in a horizontal scroll bar

pascal void
LScroller::HorizSBarAction(
	ControlHandle	inMacControl,
	SInt16			inPart)
{
	try {
		LScroller	*theScroller =
							(LScroller *) ::GetControlReference(inMacControl);
		theScroller->HorizScroll(inPart);
	}

	catch (...) { }			// Don't propagate exception into Toolbox!
}


PP_End_Namespace_PowerPlant
