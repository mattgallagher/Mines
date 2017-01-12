// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LActiveScroller.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implements active scrolling of a view during the tracking of
//	scroll bars thumbs

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LActiveScroller.h>
#include <PP_Messages.h>
#include <UControlMgr.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UTBAccessors.h>


PP_Begin_Namespace_PowerPlant

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LActiveScroller						Default Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LActiveScroller::LActiveScroller()
{
	mThumbControl	  = nil;
	mVertThumbAction  = nil;
	mHorizThumbAction = nil;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LActiveScroller						Stream Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LActiveScroller::LActiveScroller(
	LStream*	inStream)

	: LScroller(inStream)
{
	mThumbControl 	  = nil;
	mVertThumbAction  = nil;
	mHorizThumbAction = nil;

	AssignThumbProcs();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LActiveScroller						Copy Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LActiveScroller::LActiveScroller(
	const LScroller&	inOriginal)

	: LScroller(inOriginal)
{
	mThumbControl	  = nil;
	mVertThumbAction  = nil;
	mHorizThumbAction = nil;

	AssignThumbProcs();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LActiveScroller						Parameterized Constructor [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LActiveScroller::LActiveScroller(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	SInt16 				inHBLeftIndent,
	SInt16 				inHBRightIndent,
	SInt16 				inVBTopIndent,
	SInt16 				inVBBottomIndent,
	LView* 				inScrollingView)

	: LScroller(inPaneInfo, inViewInfo, inHBLeftIndent, inHBRightIndent,
					inVBTopIndent, inVBBottomIndent, inScrollingView)
{
	mThumbControl	  = nil;
	mVertThumbAction  = nil;
	mHorizThumbAction = nil;

	AssignThumbProcs();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~LActiveScroller						Destructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LActiveScroller::~LActiveScroller()
{
	if (mVertThumbAction != nil) {
		DisposeDragGrayRgnUPP(mVertThumbAction);
	}

	if (mHorizThumbAction != nil) {
		DisposeDragGrayRgnUPP(mHorizThumbAction);
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ AssignThumbProcs
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Create new UPP's and assign them to the thumbs of each of the scroll
//	bars (if they exist).

void LActiveScroller::AssignThumbProcs()
{
	if (mVerticalBar != nil) {
		mVertThumbAction = NewDragGrayRgnUPP(LActiveScroller::ActiveThumbScroll);
		mVerticalBar->SetThumbFunc(mVertThumbAction);
	}

	if (mHorizontalBar != nil) {
		mHorizThumbAction = NewDragGrayRgnUPP(LActiveScroller::ActiveThumbScroll);
		mHorizontalBar->SetThumbFunc(mHorizThumbAction);
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ IsTrackingThumb
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Correct way to check to see if we're currently in the tracking loop.

Boolean	LActiveScroller::IsTrackingThumb()
{
	return (mThumbControl != nil);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ AdjustScrollBars
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	This method gets called as a result of the view scrolling.  We want this
//	to function the same as the LScroller implementation, except while we're
//	tracking.  We maintain the scroll bar during the track, so there's no
//	need to calculate the value and draw the control twice per scroll.

void LActiveScroller::AdjustScrollBars()
{
	if (!IsTrackingThumb()) {
		LScroller::AdjustScrollBars();
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ListenToMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Here we want to do exactly like we do in the LScroller implementaion
// 	except when we're done tracking.  Usually the msg_ThumbDragged causes
//	the scrolling view to adjust, but since we did live tracking, we're
//	already scrolled to the correct location.  So we just "eat" that
//	particular message.

void LActiveScroller::ListenToMessage(MessageT inMessage, void *ioParam)
{
	if (inMessage == msg_ThumbDragged) {
		(static_cast<LStdControl*>(ioParam))->SynchValue();

	} else {
		LScroller::ListenToMessage(inMessage, ioParam);
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ StartThumbTracking
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Set up the tracking instance variables.  We also calclulate the value slop
//	constant so that clicks in the thumb of a scroll bar with a large scroll
//	unit will not cause unnecessary jumps in the view.

void LActiveScroller::StartThumbTracking(LStdControl *inWhichControl)
{
	Assert_(mThumbControl == nil);

	// Note which control we're tracking.  Making this instance variable
	// non-null is an indicator that we are currently in a tracking loop.
	mThumbControl = inWhichControl;
	mOriginalValue = inWhichControl->GetValue();
	mTrackBarUnits = inWhichControl->GetMaxValue() - inWhichControl->GetMinValue();
	mThumbControl->CalcPortFrameRect(mTrackRect);

	// How we set up the values for the tracking bar size and pin location
	// is determined by whether we are tracking a horizontal or vertical
	// scroll bar. In either case, we calculate the scroll bar rect
	// without the scroll arrows.  We then enlarge that rect for the drag
	// slop area.

	CalcTrackBarSize();

	if (mThumbControl == mVerticalBar) {
		mTrackBarPin = mTrackRect.top + kWidthOfScrollArrow;
		::MacInsetRect(&mTrackRect, -kThumbTrackWidthSlop, -kThumbTrackLengthSlop);

	} else {
		mTrackBarPin = mTrackRect.left + kWidthOfScrollArrow;
		::MacInsetRect(&mTrackRect, -kThumbTrackLengthSlop, -kThumbTrackWidthSlop);
	}


	// Focus on the scroll bar that we're tracking.  If we don't do this
	// the call to ::GetMouse() could return coordinates relative to
	// a different view.
	mThumbControl->FocusDraw();

	// While we're tracking the thumb, we're calculating its value based
	// on the current mouse location.  That works great, assuming you
	// clicked _exactly_ in the center of the thumb.  We calculate how
	// much we need to compensate the value caluculations based on the
	// click distance from the center of the thumb.
	mValueSlop = 0;
	Point currentPoint;
	::GetMouse(&currentPoint);
	LocalToPortPoint(currentPoint);

	mValueSlop = mThumbControl->GetValue() - CalcValueFromPoint(currentPoint);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ EndThumbTracking
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Note that we're done tracking.  The most important thing is to NULL out
//	the instance variable that keeps track of the current tracking control.
//	When that variable is nil we're assumed to not be tracking.

void LActiveScroller::EndThumbTracking()
{
	Assert_(mThumbControl != nil);

	// Skanky Hack Alert!!!
	// Ok, I know this seems a bit gross, buts it's here for a good reason.
	// The control manager, in it's infinite wisdom, will allow you to install
	// a thumb tracking proc.  Yet, when it initially starts tracking, it
	// will calculate the region for the thumb and outline it.  Then it will
	// call the custom tracking proc.  Since we're messing with the control
	// value during the track, the control manager wants to redraw the
	// outline of the last known position of the thumb before the track.
	// We're tracking within a call to TrackControl() and have no way to
	// clean up after our track loop terminates.  So we just hide the bogus
	// draw from the user.

	if (mOriginalValue != mThumbControl->GetValue()) {
		Rect nullRect = {0,0,0,0};
		::ClipRect(&nullRect);
		LView::OutOfFocus(this);
	}

	// Note: I experimented with calling the controls def proc
	// directly using posCntl and drawCntl messages with success to correct
	// this drawing problem.  The problem still remains that the draw
	// occurs before TrackControl() exits.

	mThumbControl = nil;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ AdaptToNewThumbValue
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Here's where we do the scroll of the scrolling view based on the
//	new value for the tracking control.

void LActiveScroller::AdaptToNewThumbValue(SInt32 inNewValue)
{
	Assert_(mThumbControl != nil);

	SPoint32		scrollUnit;
	mScrollingView->GetScrollUnit(scrollUnit);
	SPoint32		scrollPosition;
	mScrollingView->GetScrollPosition(scrollPosition);
	SPoint32		origScrollPos = scrollPosition;

	if (mThumbControl == mVerticalBar) {
		scrollPosition.v = inNewValue * scrollUnit.v;
	} else {
		scrollPosition.h = inNewValue * scrollUnit.h;
	}

	mScrollingView->ScrollPinnedImageBy(scrollPosition.h - origScrollPos.h,
										scrollPosition.v - origScrollPos.v,
										true);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CalcValueFromPoint
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Given a point, calculate the value for the control.  The instance
//	variables used in this calculation are initialized in the
//	StartThumbTracking() method.  The returned value is adjusted for slop.

SInt32 LActiveScroller::CalcValueFromPoint(Point inPoint)
{
	SInt32 theValue;
	if (mThumbControl == mVerticalBar) {
		theValue = ((inPoint.v - mTrackBarPin) * mTrackBarUnits) / mTrackBarSize;
	} else {
		theValue = ((inPoint.h - mTrackBarPin) * mTrackBarUnits) / mTrackBarSize;
	}

	theValue += mValueSlop;

	return theValue;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CalcTrackBarSize
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Determine the number of pixels of movement of the thumb

void
LActiveScroller::CalcTrackBarSize()
{
		// Strategy: A CDEF has a call to calculate the thumb region.
		// We'll calculate the thumb region when the control is at
		// its min and max values. The difference in position is the
		// range of movement of the thumb.

	ControlHandle	controlH = mThumbControl->GetMacControl();

	SInt16	value = ::GetControlValue(controlH);	// Save current value
	SInt16	min = ::GetControlMinimum(controlH);
	SInt16	max = ::GetControlMaximum(controlH);

	StClipRgnState	emptyClip(nil);		// Prevent CDEF from drawing while
										//   we fiddle with the value

	StRegion	thumbRgn;

		// Get thumb position at min value

	::SetControlValue(controlH, min);

	UControlMgr::SendControlMessage(controlH, calcThumbRgn, RgnHandle(thumbRgn));

	Rect	thumbRect;
	thumbRgn.GetBounds(thumbRect);

	SInt16	minEdge = thumbRect.top;
	if (mThumbControl == mHorizontalBar) {
		minEdge = thumbRect.left;
	}

		// Get thumb position at max value

	::SetControlValue(controlH, max);

	UControlMgr::SendControlMessage(controlH, calcThumbRgn, RgnHandle(thumbRgn));

	thumbRgn.GetBounds(thumbRect);
	SInt16	maxEdge = thumbRect.top;
	if (mThumbControl == mHorizontalBar) {
		maxEdge = thumbRect.left;
	}

	::SetControlValue(controlH, value);		// Reset to original value

	mTrackBarSize = maxEdge - minEdge;		// Difference is range of motion
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ HandleThumbScroll
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Here is where the actual thumb action is handled.  We could eliminate the
//	click loop if we could guarantee to get called if the drag didn't change
//	the value of the control.  Currently, if the value doesn't change, no
//	notification is issued, therefore there's no way for us to reset the
//	tracking state.

void LActiveScroller::HandleThumbScroll(LStdControl *inWhichControl)
{
	// Setup the information that we'll need during the tracking loop.
	StartThumbTracking(inWhichControl);

	// Focus on the scroll bar that we're tracking.  If we don't do this
	// the call to ::GetMouse() could return coordinates relative to
	// a different view.
	mThumbControl->FocusDraw();

	// Start with equal points, that way we wont update the
	// scrolling view unnecessarily.
	Point currentPoint = Point_00;
	Point lastPoint    = Point_00;

	while (::StillDown()) {
		// Only attempt to update the value if the mouse moved.
		::GetMouse(&currentPoint);
		LocalToPortPoint(currentPoint);
		if (::EqualPt(currentPoint, lastPoint)) {
			continue;
		}

		// Remeber where the last mouse location was
		lastPoint = currentPoint;
		SInt32 theValue;

		// Check to see if the user tracked outside of the slop rect.
		// If they did, restore the original value of the control.
		// This mimics the same behaviour as the standard scroll bar.
		if (::MacPtInRect(currentPoint, &mTrackRect)) {
			theValue = CalcValueFromPoint(currentPoint);
		} else {
			theValue = mOriginalValue;
		}

		inWhichControl->SetValue(theValue);

		// we can't use the local value variable because it may have
		// been clipped in the previous call to SetValue().
		AdaptToNewThumbValue(inWhichControl->GetValue());

		// Since we updated the scrolling view, we'll need to refocus on
		// the scroll bar.  Otherwise when we call ::GetMouse() in the
		// next iteration, the coordinates returned will be relative
		// to the origin of the scrolling view.
		mThumbControl->FocusDraw();
	}

	EndThumbTracking();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ActiveThumbScroll												  [static]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	This is the static callback for the scroll bars when the thumb is tracked.
//	The LScroller implementation embeds a pointer to the scroller object
//	in the control reference filed of the control record.

pascal void	LActiveScroller::ActiveThumbScroll()
{
	try {
		LStdControl *theScrollBar = LStdControl::GetTrackingControl();
		if (theScrollBar != nil) {
			LActiveScroller	*theScroller = (LActiveScroller *)
							GetControlReference(theScrollBar->GetMacControl());
			theScroller->HandleThumbScroll(theScrollBar);
		}
	}

	catch (...) { }			// Don't propagate exception into Toolbox!
}


PP_End_Namespace_PowerPlant
