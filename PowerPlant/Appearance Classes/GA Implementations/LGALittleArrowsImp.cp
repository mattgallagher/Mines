// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGALittleArrowsImp.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGALittleArrowsImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LControl.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UEventMgr.h>

PP_Begin_Namespace_PowerPlant


const SInt16	littleArrows_ArrowWidth		= 11;
const SInt16	littleArrows_ArrowHeight	= 10;
const SInt16	littleArrows_Width			= 13;
const SInt16	littleArrows_Height			= 23;


// ---------------------------------------------------------------------------
//	¥ LGALittleArrowsImp					Constructor				  [public]
// ---------------------------------------------------------------------------

LGALittleArrowsImp::LGALittleArrowsImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGALittleArrowsImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGALittleArrowsImp::LGALittleArrowsImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mFirstDelay	= 8;					// Ticks before value starts to
										//   continuously change while
}										//   pressing the mouse in an arrow


// ---------------------------------------------------------------------------
//	¥ ~LGALittleArrowsImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGALittleArrowsImp::~LGALittleArrowsImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	ResIDT	textTraitID;
	Str255	title;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, kControlLittleArrowsProc);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::Init(
	LControlPane*	inControlPane,
	SInt16			/*inControlKind*/,
	ConstStringPtr	/*inTitle*/,
	ResIDT			/*inTextTraitsID*/,
	SInt32			/*inRefCon*/)
{
	mControlPane  = inControlPane;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcLocalUpArrowRect											  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::CalcLocalUpArrowRect(
	Rect&	outRect) const
{
	CalcLocalLittleArrowRect(outRect);	// Get frame of entire control

										// Adjust frame to that of just the
										//   up arrow
	outRect.left  += 1;
	outRect.top   += 1;
	outRect.right  = (SInt16) (outRect.left + littleArrows_ArrowWidth);
	outRect.bottom = (SInt16) (outRect.top + littleArrows_ArrowHeight);
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalDownArrowRect										  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::CalcLocalDownArrowRect(
	Rect&	outRect) const
{
	CalcLocalLittleArrowRect(outRect);	// Get frame of entire control

										// Adjust frame to that of just the
										//   down arrow
	outRect.left  += 1;
	outRect.right  = (SInt16) (outRect.left + littleArrows_ArrowWidth);
	outRect.top   += littleArrows_ArrowHeight + 2;
	outRect.bottom = (SInt16) (outRect.top + littleArrows_ArrowHeight);
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalLittleArrowRect										  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::CalcLocalLittleArrowRect(
	Rect&	outRect) const
{
		// Size of little arrows is constant. Locate arrows aligned
		// with the left and vertically centered within the Frame.

	CalcLocalFrameRect(outRect);
	SInt16		frameHeight = UGraphicUtils::RectHeight(outRect);

	outRect.right  = (SInt16) (outRect.left + littleArrows_Width);
	outRect.top   += (frameHeight / 2) - (littleArrows_ArrowHeight + 1);
	outRect.bottom = (SInt16) (outRect.top + littleArrows_Height);

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::DrawSelf()
{
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalLittleArrowRect ( localFrame );

	if ( mControlPane->GetBitDepth() < 4 ) {	// BLACK & WHITE
		::RGBForeColor ( &Color_Black );
												// If we are disabled we draw in gray
		if ( !IsActive()  ||  !IsEnabled()) {
			StColorPenState::SetGrayPattern();
		}

	} else {									// COLOR
		RGBColor	tempColor = IsEnabled () && IsActive () ? Color_Black :
										  UGAColorRamp::GetColor(colorRamp_Gray7);
		::RGBForeColor ( &tempColor );
	}
												// Draw the frame around the entire control
	UGraphicUtils::NoCornerPixelFrame ( localFrame );

												// Now draw the dividing line
	::MoveTo ( localFrame.left,
			   (SInt16) (localFrame.top + littleArrows_ArrowHeight + 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1),
				  (SInt16) (localFrame.top + littleArrows_ArrowHeight + 1) );

												// UP ARROW
	DrawLittleArrowsNormal ( true, mControlPane->GetBitDepth() );

												// DOWN ARROW
	DrawLittleArrowsNormal ( false, mControlPane->GetBitDepth() );
}


// ---------------------------------------------------------------------------
//	¥ DrawLittleArrowsNormal									   [protected]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::DrawLittleArrowsNormal(
	Boolean		inUpArrow,
	SInt16		inDepth)
{
	StColorPenState theSavedPenState;
	theSavedPenState.Normalize ();

	RGBColor	tempColor;
												// Get the frame for the control
	Rect	localFrame;
	if ( 	inUpArrow ) {
		CalcLocalUpArrowRect ( localFrame );

	} else {
		CalcLocalDownArrowRect ( localFrame );
	}

	if ( inDepth < 4 ) {					// BLACK & WHITE
												// Before we draw anything we need to erase the
												// control in case we we had previously been
												// hilited
		::EraseRect ( &localFrame );

												// If we are disabled we draw in gray
		if ( !IsActive()  ||  !IsEnabled()) {
			StColorPenState::SetGrayPattern();
		}

	} else {									// COLOR
												// We need to normail in case we have just drawn
												// a portion in B&W in which case the pen
												// pattern will be off
		StColorPenState::Normalize ();

												// First make sure the face of the control is
												// drawn
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
		::RGBForeColor ( &tempColor );
		::PaintRect ( &localFrame );

												// We only draw the bevels if we are enabled
		if ( IsEnabled () && IsActive ()) {
												// LIGHT BEVELS
												// Setup Colors for top left edges
			::RGBForeColor ( &Color_White );
			UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

												// SHADOW BEVELS
												// Setup Colors for bottom right edges
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			::RGBForeColor ( &tempColor );
			UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );
		}

												// DRAW ACTUAL ARROW
												// Setup the color for either the enabled or
												// disabled states
		if ( IsEnabled () && IsActive ()) {
			::RGBForeColor ( &Color_Black );

		} else {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
			::RGBForeColor ( &tempColor );
		}
	}
												// Handle the drawing of the actual arrow
	DrawActualArrow ( localFrame, inUpArrow );
}


// ---------------------------------------------------------------------------
//	¥ DrawLittleArrowsHilited									   [protected]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::DrawLittleArrowsHilited(
	Boolean		inUpArrow,
	SInt16		inDepth)
{
	StColorPenState theSavedPenState;
	theSavedPenState.Normalize ();

	RGBColor	tempColor;

													// Get the frame for the control
	Rect	localFrame;
	if ( 	inUpArrow ) {
		CalcLocalUpArrowRect ( localFrame );
	} else {
		CalcLocalDownArrowRect ( localFrame );
	}

	if ( inDepth < 4 ) {					// BLACK & WHITE
												// First make sure the face of the control is
												// drawn
		::PaintRect ( &localFrame );

	} else {									// COLOR
												// First make sure the face of the control is
												// drawn
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		::RGBForeColor ( &tempColor );
		::PaintRect ( &localFrame );

												// LIGHT BEVELS
												// Setup Colors for top left edges
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

												// SHADOW BEVELS
												// Setup Colors for bottom right edges
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );
	}

	::RGBForeColor ( &Color_White );
	DrawActualArrow ( localFrame, inUpArrow );
}


// ---------------------------------------------------------------------------
//	¥ DrawActualArrow											   [protected]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::DrawActualArrow(
	const Rect&		inRect,
	Boolean			inUpArrow)
{
								// Now we can draw the actual arrows either an
								// up or a down depending on the flag passed in
								// to us
	if ( inUpArrow ) {
		::MoveTo (		(SInt16) (inRect.left + 5),  (SInt16) (inRect.top + 3) );
		::MacLineTo (	(SInt16) (inRect.right - 6), (SInt16) (inRect.top + 3) );
		::MoveTo (		(SInt16) (inRect.left + 4),  (SInt16) (inRect.top + 4) );
		::MacLineTo (	(SInt16) (inRect.right - 5), (SInt16) (inRect.top + 4) );
		::MoveTo (		(SInt16) (inRect.left + 3),  (SInt16) (inRect.top + 5) );
		::MacLineTo (	(SInt16) (inRect.right - 4), (SInt16) (inRect.top + 5) );
		::MoveTo (		(SInt16) (inRect.left + 2),  (SInt16) (inRect.top + 6) );
		::MacLineTo (	(SInt16) (inRect.right - 3), (SInt16) (inRect.top + 6) );

	} else {
		::MoveTo (		(SInt16) (inRect.left + 2),  (SInt16) (inRect.top + 3) );
		::MacLineTo (	(SInt16) (inRect.right - 3), (SInt16) (inRect.top + 3) );
		::MoveTo (		(SInt16) (inRect.left + 3),  (SInt16) (inRect.top + 4) );
		::MacLineTo (	(SInt16) (inRect.right - 4), (SInt16) (inRect.top + 4) );
		::MoveTo (		(SInt16) (inRect.left + 4),  (SInt16) (inRect.top + 5) );
		::MacLineTo (	(SInt16) (inRect.right - 5), (SInt16) (inRect.top + 5) );
		::MoveTo (		(SInt16) (inRect.left + 5),  (SInt16) (inRect.top + 6) );
		::MacLineTo (	(SInt16) (inRect.right - 6), (SInt16) (inRect.top + 6) );
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FindHotSpot												   [protected]
// ---------------------------------------------------------------------------

SInt16
LGALittleArrowsImp::FindHotSpot	(
	Point	inPoint) const
{
	SInt16	hotSpot = 0;

	Rect upArrowRect;							// Up Arrow
	CalcLocalUpArrowRect(upArrowRect);
	if (::MacPtInRect(inPoint, &upArrowRect)) {
		hotSpot = kControlUpButtonPart;
	}

	Rect downArrowRect;							// Down Arrow
	CalcLocalDownArrowRect(downArrowRect);
	if (::MacPtInRect(inPoint, &downArrowRect)) {
		hotSpot = kControlDownButtonPart;
	}

	return hotSpot;
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot											   [protected]
// ---------------------------------------------------------------------------

Boolean
LGALittleArrowsImp::PointInHotSpot(
	Point		inPoint,
	SInt16		inHotSpot) const
{
	Boolean	hotSpotHit = false;

	if (inHotSpot == kControlUpButtonPart) {
		Rect upArrowRect;
		CalcLocalUpArrowRect(upArrowRect);
		hotSpotHit = ::MacPtInRect(inPoint, &upArrowRect);

	} else if (inHotSpot == kControlDownButtonPart) {
		Rect downArrowRect;
		CalcLocalDownArrowRect(downArrowRect);
		hotSpotHit = ::MacPtInRect(inPoint, &downArrowRect);
	}

	return hotSpotHit;
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------

Boolean
LGALittleArrowsImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		/* inModifiers */)
{
										// Use delays based on ticks so that
										//   the value doesn't change too
										//   quickly while pressing the mouse
	UInt32	nextActionTicks = ::TickCount() + mFirstDelay;
	SInt32	delay = mFirstDelay;

										// For the initial mouse down, the
										//   mouse is currently inside the
										//   HotSpot when it was previously
										//   outside
	Boolean		currInside = true;
	Boolean		prevInside = false;
	mControlPane->HotSpotAction(inHotSpot, currInside, prevInside);

	StRegion		clipR;				// Get copy of clipping region
	::GetClip(clipR);
										// Track the mouse while it is down
	Point	currPt = inPoint;
	while (::StillDown()) {
										// Don't do anything if enough time
										//   hasn't passed
		UInt32	currTicks = ::TickCount();

		if (currTicks >= nextActionTicks) {
			nextActionTicks = currTicks + delay;

			if (delay > 0) {			// Delay gets shorter the longer the
				delay--;				//   the mouse is held down
			}

			::GetMouse ( &currPt );		// Must keep track if mouse moves from
			prevInside = currInside;	// In-to-Out or Out-To-In
			currInside =  (mControlPane->PointInHotSpot(currPt, inHotSpot)
								&& ::PtInRgn(currPt, clipR));

										// Action will adjust value
			 mControlPane->HotSpotAction(inHotSpot, currInside, prevInside);
		}
	}

	EventRecord	macEvent;
	if (UEventMgr::GetMouseUp(macEvent)) {
		currPt = macEvent.where;
		::GlobalToLocal(&currPt);
		currInside = (mControlPane->PointInHotSpot(currPt, inHotSpot)
							&& ::PtInRgn(currPt, clipR));
										// Do NOT call HotSpotAction() here.
										//   We were live tracking while
										//   the mouse was down.
	}
										// Return if we are still inside the
	return currInside;					//   control or not
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::HotSpotAction(
	SInt16		 inHotSpot,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
								// If the mouse moved in or out of the hot spot
								// handle the hiliting of the control based on
								// the hot spot the mouse went down in
	if ( (inCurrInside != inPrevInside) &&
		 mControlPane->FocusExposed() ) {

		 bool	upOrDown = (inHotSpot == kControlUpButtonPart);

		Rect	frame;
		CalcLocalFrameRect(frame);

		StColorDrawLoop		drawLoop(frame);
		SInt16				depth;
		while (drawLoop.NextDepth(depth)) {

			if (inCurrInside) {
				DrawLittleArrowsHilited(upOrDown, depth);

			} else {
				DrawLittleArrowsNormal(upOrDown, depth);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGALittleArrowsImp::DoneTracking(
	SInt16		inHotSpot,
	Boolean		/* inGoodTrack */)
{
	if (mControlPane->FocusExposed()) {			// Draw arrows in normal state
		Rect	frame;
		CalcLocalFrameRect(frame);

		StColorDrawLoop		drawLoop(frame);
		SInt16				depth;
		while (drawLoop.NextDepth(depth)) {
			DrawLittleArrowsNormal((inHotSpot == kControlUpButtonPart), depth );
		}
	}
}


PP_End_Namespace_PowerPlant
