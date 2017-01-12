// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMarqueeTask.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Adapted from:
//		Finder Marquee by Jordan Zimmerman (c)1995 by Altura Software, Inc.
//		Used by permission.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMarqueeTask.h>
#include <LView.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UEventMgr.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	LMarqueeTask
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LMarqueeTask							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor specifying the receiver object and the mouse-down event
//	that triggered the marquee
//
//	As we autoscroll, the local coordinate system keeps changing.
//	Do everything in image coordinates.

LMarqueeTask::LMarqueeTask(
	LMarqueeReceiver&		inReceiver,
	const SMouseDownEvent&	inStartEvent)

	: mReceiver(inReceiver)
{
	InitMarqueeTask(inStartEvent.whereLocal, inStartEvent.macEvent.modifiers);

}
// ---------------------------------------------------------------------------
//	¥ LMarqueeTask							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor specifying the receiver object, the location of the
//	mouse-click that triggered the marquee, and the keyboard modifiers that
//	were pressed at the time of the click.

LMarqueeTask::LMarqueeTask(
	LMarqueeReceiver&	inReceiver,			// receiver object
	const Point&		inInitialPoint,		// initial (anchor) point (in local coordinates)
	UInt16				inModifiers)		// initial mouse modifiers

	: mReceiver(inReceiver)
{
	InitMarqueeTask(inInitialPoint, inModifiers);
}


// ---------------------------------------------------------------------------
//	¥ ~LMarqueeTask							Destructor				  [public]
// ---------------------------------------------------------------------------

LMarqueeTask::~LMarqueeTask()
{
	if (mMarqueeDrawn) {
		DrawMarqueeRect();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoMarquee														  [public]
// ---------------------------------------------------------------------------
//	The main "event loop" of the marquee task. Call this function
//	once you've created the marquee task object to track the mouse
//	until mouse-up.

void
LMarqueeTask::DoMarquee()
{
	// Draw the initial selection and marquee rect.

	StartTracking();

	// Track until mouse up
		
	Point				currPt;
	MouseTrackingResult	trackResult;
	
	do {
		UMouseTracking::TrackMouseDown(nil, currPt, trackResult);
		
		mCurrentModifiers = (UInt16) UEventMgr::GetKeyModifiers();
		
		mReceiver.FocusMarqueeArea();

		// Allow receiver to constrain mouse location,
		// then track new location.

		mReceiver.ConstrainMouse(*this, currPt, mCurrentModifiers);
		ContinueTracking(currPt);
		
	} while (trackResult != kMouseTrackingMouseReleased);

	// Mouse is now up, exit.

	EndTracking();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ StartTracking												   [protected]
// ---------------------------------------------------------------------------
//	Called by DoMarquee to set up the drag drawing.

void
LMarqueeTask::StartTracking()
{
	// Initial chance to draw selection before drawing marquee.

	Rect oldRect = Rect_0000;
	Rect localMarqueeRect;

	localMarqueeRect = mReceiver.ImageToLocalRect(mMarqueeRect);

	if (mReceiver.CheckSelectionChanged(*this, oldRect, localMarqueeRect)){
		mReceiver.UpdateSelection(*this, oldRect, localMarqueeRect);
	}
	// Draw the initial marquee rectangle.

	DrawMarqueeRect();
	mMarqueeDrawn = true;
	mWasFlicker   = false;
}


// ---------------------------------------------------------------------------
//	¥ GetInitialPoint												  [public]
// ---------------------------------------------------------------------------

void
LMarqueeTask::GetInitialPoint(
	Point	&outPoint) const
{
	outPoint = mReceiver.ImageToLocalPoint(mAnchorPoint);
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentPoint												  [public]
// ---------------------------------------------------------------------------

void
LMarqueeTask::GetCurrentPoint(
	Point	&outPoint) const
{
	outPoint = mReceiver.ImageToLocalPoint(mCurrentPoint);
}


// ---------------------------------------------------------------------------
//	¥ GetMarqueeRect												  [public]
// ---------------------------------------------------------------------------

void
LMarqueeTask::GetMarqueeRect(
	Rect	&outRect) const
{
	outRect =  mReceiver.ImageToLocalRect(mMarqueeRect);
}



// ---------------------------------------------------------------------------
//	¥ ContinueTracking											   [protected]
// ---------------------------------------------------------------------------
//	Redraws the marquee if the mouse has moved.
//	Called by DoMarquee each time the mouse location is polled.

void
LMarqueeTask::ContinueTracking(
	const Point&	inNewPoint)
{
	// Avoid flashing step 1: Do nothing if the mouse hasn't moved.
	SPoint32	newImagePoint;
	Point		tempLocal = inNewPoint;
	::LocalToGlobal(&tempLocal);
	newImagePoint = mReceiver.GlobalToImagePoint(tempLocal);
	if (newImagePoint.h == mCurrentPoint.h && newImagePoint.v == mCurrentPoint.v) {
		return;
	}

	StRegion	clipRegion;
	::GetClip(clipRegion);

		// Save and set up the pen.
	StColorPenState pen;
	pen.Normalize();
	::PenMode(patXor);

	Pattern		grayPat;
	::PenPat( UQDGlobals::GetGrayPat(&grayPat) );

	// Save the old marquee rectangle and set up the new one.
	SRect32 oldRect = mMarqueeRect;
	mCurrentPoint = newImagePoint;
	CalculateMarqueeRect();

	// Now draw the rectangle.
	Boolean	flashIt = false;

	try {
		// Get regions we'll need to do our thing.

		StRegion	oldRegion;
		StRegion	workRegion;
		StRegion	newRegion;

		Rect	localMarqueeRect, localOldRect;
		localMarqueeRect = mReceiver.ImageToLocalRect(mMarqueeRect);
		localOldRect = mReceiver.ImageToLocalRect(oldRect);

		// Generate 1 pixel thick outline regions of
		// the old and new marquee rects.
		MakeFrameRegion(oldRegion, localOldRect, workRegion);
		MakeFrameRegion(newRegion, localMarqueeRect, workRegion);

		// Get the area in common between the old and the new.
		::SectRgn(oldRegion, newRegion, workRegion);

		// Set the clip to the old clip minus the common area
		// of the old and new marquee rect.
		::DiffRgn(clipRegion, workRegion, workRegion);

		Point 					localPoint;
		GetCurrentPoint(localPoint);

		if (mReceiver.WantsToAutoscroll(localPoint)) {
			mWasFlicker = true;
			flashIt = true;

		} else if (mReceiver.CheckSelectionChanged(*this, localOldRect, localMarqueeRect)) {
			if (mWasFlicker) {
				mWasFlicker = false;
				flashIt     = true;

			} else {
				mReceiver.FocusMarqueeArea();

				// If there is a selection change, the old marquee must be erased,
				// the selections must be drawn, and then the new marquee can be drawn.
				::MacFrameRect(&localOldRect);
				pen.Normalize();

				mReceiver.UpdateSelection(*this, localOldRect, localMarqueeRect);

				// oster - reset the clip after UpdateSelection
				::SetClip(clipRegion);
				DrawMarqueeRect();
				mWasFlicker = false;
			}

		} else {
			if (mWasFlicker) {
				mWasFlicker = false;
				flashIt     = true;

			} else {
				StClipRgnState	clip(workRegion);

				// If there's no selection change, the marquee can be
				// drawn in one step that will erase the old and draw the new.
				::MacUnionRgn(newRegion, oldRegion, workRegion);
				::MacPaintRgn(workRegion);	// this will both erase the old and draw the new
				mWasFlicker = false;
			}
		}
	}

	catch (...) { }

	if (flashIt) {
		Rect	localMarqueeRect, localOldRect;
		localMarqueeRect = mReceiver.ImageToLocalRect(mMarqueeRect);
		localOldRect = mReceiver.ImageToLocalRect(oldRect);

		::MacFrameRect(&localOldRect);
		if (mReceiver.CheckSelectionChanged(*this, localOldRect, localMarqueeRect)) {
			mReceiver.UpdateSelection(*this, localOldRect, localMarqueeRect);
		}
		DrawMarqueeRect();
	}
}


// ---------------------------------------------------------------------------
//	¥ EndTracking												   [protected]
// ---------------------------------------------------------------------------
//	Called by DoMarquee after the mouse button is released.
//	Removes the marquee drawing.

void
LMarqueeTask::EndTracking()
{
	if (mMarqueeDrawn) {
		DrawMarqueeRect();
		mMarqueeDrawn = false;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalculateMarqueeRect										   [protected]
// ---------------------------------------------------------------------------
//	Update the mMarqueeRect field from the mAnchorPoint and
//	mCurrentPoint fields.
//
//	JLZ writes: Calculating the marquee rect isn't as simple as
//	Pt2Rect. Using Pt2Rect causes the pin point to shift around.
//	This function will calculate a correct marquee rect that
//	keeps the pin point in place.

void
LMarqueeTask::CalculateMarqueeRect()
{
	if ( (mCurrentPoint.h >= mAnchorPoint.h)  &&
		 (mCurrentPoint.v >= mAnchorPoint.v) ) {
											// Quadrant IV
		mMarqueeRect.left   = mAnchorPoint.h;
		mMarqueeRect.top    = mAnchorPoint.v;
		mMarqueeRect.right  = mCurrentPoint.h + 1;
		mMarqueeRect.bottom = mCurrentPoint.v + 1;

	} else if ( (mCurrentPoint.h <= mAnchorPoint.h)  &&
				(mCurrentPoint.v <= mAnchorPoint.v) ) {
											// Quadrant I
		mMarqueeRect.left   = mCurrentPoint.h;
		mMarqueeRect.top    = mCurrentPoint.v;
		mMarqueeRect.right  = mAnchorPoint.h + 1;
		mMarqueeRect.bottom = mAnchorPoint.v + 1;

	} else if ( (mCurrentPoint.h >= mAnchorPoint.h)  &&
				(mCurrentPoint.v <= mAnchorPoint.v) ) {
											// Quadrant II
		mMarqueeRect.left   = mAnchorPoint.h;
		mMarqueeRect.top    = mCurrentPoint.v;
		mMarqueeRect.right  = mCurrentPoint.h + 1;
		mMarqueeRect.bottom = mAnchorPoint.v + 1;

	} else {
											// Quadrant III
		mMarqueeRect.left   = mCurrentPoint.h;
		mMarqueeRect.top    = mAnchorPoint.v;
		mMarqueeRect.right  = mAnchorPoint.h + 1;
		mMarqueeRect.bottom = mCurrentPoint.v + 1;
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawMarqueeRect											   [protected]
// ---------------------------------------------------------------------------
//	Outline the entire marquee rectangle using XOR gray.

void
LMarqueeTask::DrawMarqueeRect() const
{
	mReceiver.FocusMarqueeArea();
	StColorPenState pen;
	pen.Normalize();
	::PenMode(patXor);

	Pattern		grayPat;
	::PenPat( UQDGlobals::GetGrayPat(&grayPat) );
	Rect	localMarqueeRect;

	localMarqueeRect = mReceiver.ImageToLocalRect(mMarqueeRect);
	::MacFrameRect(&localMarqueeRect);
}


// ---------------------------------------------------------------------------
//	¥ MakeFrameRegion											   [protected]
// ---------------------------------------------------------------------------
//	Utility to make a 1-pixel thick region of the frame outline of a rect.

void
LMarqueeTask::MakeFrameRegion(
	RgnHandle		inTargetRegion,
	const Rect&		inFrameRect,
	RgnHandle		inWorkRegion)
{
	::RectRgn(inTargetRegion, &inFrameRect);
	::MacCopyRgn(inTargetRegion, inWorkRegion);
	::InsetRgn(inWorkRegion, 1, 1);
	::DiffRgn(inTargetRegion, inWorkRegion, inTargetRegion);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InitMarqueeTask												 [private]
// ---------------------------------------------------------------------------
//	Shared initialization code used by LMarqueeTask's two constructors.

void
LMarqueeTask::InitMarqueeTask(
	const Point&	inInitialPoint,
	UInt16			inModifiers)
{
	Point 		whereGlobal;
	whereGlobal = inInitialPoint;
	::LocalToGlobal(&whereGlobal);

	mAnchorPoint = mCurrentPoint = mReceiver.GlobalToImagePoint(whereGlobal);

	mInitialModifiers = inModifiers;
	mCurrentModifiers = inModifiers;

	mMarqueeDrawn = false;
	mWasFlicker   = false;

	mUserData = 0;

	CalculateMarqueeRect();
}

#pragma mark -

// ===========================================================================
//	LMarqueeReceiver
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LMarqueeReceiver						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMarqueeReceiver::LMarqueeReceiver()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LMarqueeReceiver						Destructor				  [public]
// ---------------------------------------------------------------------------

LMarqueeReceiver::~LMarqueeReceiver()
{
}


// ---------------------------------------------------------------------------
//	¥ ConstrainMouse												  [public]
// ---------------------------------------------------------------------------
//	Override to provide shift or grid constraints (by modifying ioMouseLocal).
//	Default does nothing.

void
LMarqueeReceiver::ConstrainMouse(
	const LMarqueeTask&	/* inMarqueeTask */,
	Point&				/* ioMouseLocal */,
	UInt16				/* inModifiers */)
{
}


// ---------------------------------------------------------------------------
//	¥ CheckSelectionChanged											  [public]
// ---------------------------------------------------------------------------
//	Return true if the selection will change because of the realignment
//	of the marquee rect.
//
//	DO NOT do any on-screen drawing at this time!

Boolean
LMarqueeReceiver::CheckSelectionChanged(
	const LMarqueeTask&	/* inMarqueeTask */,
	const Rect&			/* inOldMarqueeRect */,
	const Rect&			/* inNewMarqueeRect */)
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ UpdateSelection												  [public]
// ---------------------------------------------------------------------------
//	Do any on-screen redrawing made necessary by selection changes
//	reported by CheckSelectionChanged().

void
LMarqueeReceiver::UpdateSelection(
	const LMarqueeTask&	/* inMarqueeTask */,
	const Rect&			/* inOldMarqueeRect */,
	const Rect&			/* inNewMarqueeRect */)
{
}


// ---------------------------------------------------------------------------
//	¥ FocusMarqueeArea												  [public]
// ---------------------------------------------------------------------------
//	Set up the drawing system to draw to the correct port and origin
//	for the marquee area.

void
LMarqueeReceiver::FocusMarqueeArea()
{
		// Focus target view. We have to cast away the const because
		// GetTargetView() returns a const LView*. In hindsight, the
		// returned LView* shouldn't be const. But changing the API
		// would break all subclasses of LMarqueeReceiver, so we just
		// live with the const_cast here.

	const_cast<LView*>(GetTargetView())->FocusDraw();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GlobalToImagePoint											  [public]
// ---------------------------------------------------------------------------

SPoint32
LMarqueeReceiver::GlobalToImagePoint(
	Point	inPoint) const
{
	::GlobalToLocal(&inPoint);

	SPoint32	val;
	GetTargetView()->LocalToImagePoint(inPoint, val);
	return val;
}


// ---------------------------------------------------------------------------
//	¥ ImageToLocalRect												  [public]
// ---------------------------------------------------------------------------

#ifndef topLeft32
	#define topLeft32(r)	(((SPoint32 *) &(r))[0])
#endif

#ifndef botRight32
	#define botRight32(r)	(((SPoint32 *) &(r))[1])
#endif

Rect
LMarqueeReceiver::ImageToLocalRect(
	const SRect32&	inRect) const
{
	Rect	r;

	GetTargetView()->ImageToLocalPoint(topLeft32(inRect), topLeft(r));
	GetTargetView()->ImageToLocalPoint(botRight32(inRect), botRight(r));
	return r;
}


// ---------------------------------------------------------------------------
//	¥ ImageToLocalPoint												  [public]
// ---------------------------------------------------------------------------

Point
LMarqueeReceiver::ImageToLocalPoint(
	const SPoint32&	inPoint) const
{
	Point	p;

	GetTargetView()->ImageToLocalPoint(inPoint, p);
	return p;
}


// ---------------------------------------------------------------------------
//	¥ WantsToAutoscroll												  [public]
// ---------------------------------------------------------------------------

Boolean
LMarqueeReceiver::WantsToAutoscroll(
	Point	inLocalPt)
{
	GetTargetView()->LocalToPortPoint(inLocalPt);
	return ! GetTargetView()->PointIsInFrame(inLocalPt.h, inLocalPt.v);
}


PP_End_Namespace_PowerPlant
