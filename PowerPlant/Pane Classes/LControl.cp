// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControl.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Base class for visual elements that have a numerical value, track mouse
//	clicks, and broadcast a message when the value changes

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LControl.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingUtils.h>
#include <UEventMgr.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LControl								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LControl::LControl()
{
	mValueMessage = cmd_Nothing;

	mValue = mMaxValue = mMinValue = 0;
}


// ---------------------------------------------------------------------------
//	¥ LControl								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LControl::LControl(
	const LControl&		inOriginal)

	: LPane(inOriginal),
	  LBroadcaster(inOriginal)
{
	mValueMessage	= inOriginal.mValueMessage;
	mValue			= inOriginal.mValue;
	mMinValue		= inOriginal.mMinValue;
	mMaxValue		= inOriginal.mMaxValue;
}


// ---------------------------------------------------------------------------
//	¥ LControl								Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct from input parameters

LControl::LControl(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,
	SInt32			inMinValue,
	SInt32			inMaxValue)

	: LPane(inPaneInfo)
{
	mValueMessage	= inValueMessage;
	mValue			= inValue;
	mMinValue		= inMinValue;
	mMaxValue		= inMaxValue;
}


// ---------------------------------------------------------------------------
//	¥ LControl								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LControl::LControl(
	LStream	*inStream)

	: LPane(inStream)
{
	SControlInfo	controlInfo;
	*inStream >> controlInfo.valueMessage;
	*inStream >> controlInfo.value;
	*inStream >> controlInfo.minValue;
	*inStream >> controlInfo.maxValue;

	mValueMessage	= controlInfo.valueMessage;
	mValue			= controlInfo.value;
	mMinValue		= controlInfo.minValue;
	mMaxValue		= controlInfo.maxValue;
}


// ---------------------------------------------------------------------------
//	¥ ~LControl								Destructor				  [public]
// ---------------------------------------------------------------------------

LControl::~LControl()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------
//	Return the value of a Control

SInt32
LControl::GetValue() const
{
	return mValue;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------
//	Specify the value for a Control
//
//	You will usually override this function to redraw the control to
//	reflect the new value setting. Normally, you'll call this function
//	at the beginning of the overriding function. For example:
//
//	void MyControl::SetValue(SInt32 inValue)
//	{
//		LControl::SetValue(inValue);
//			// ... Add code here to redraw control
//	}

void
LControl::SetValue(
	SInt32	inValue)
{
	if (inValue < mMinValue) {		// Enforce min/max range
		inValue = mMinValue;
	} else if (inValue > mMaxValue) {
		inValue = mMaxValue;
	}

	if (mValue != inValue) {		// If value is not the current value
		mValue = inValue;			//   Store new value
		BroadcastValueMessage();	//   Inform Listeners of value change
	}
}


// ---------------------------------------------------------------------------
//	¥ IncrementValue												  [public]
// ---------------------------------------------------------------------------

void
LControl::IncrementValue(
	SInt32	inIncrement)
{
	SetValue(mValue + inIncrement);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LControl::SetMinValue(
	SInt32	inMinValue)
{
	mMinValue = inMinValue;
	if (mValue < inMinValue) {			// Adjust value if less than new
		SetValue(inMinValue);			//   minimum value
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LControl::SetMaxValue(
	SInt32	inMaxValue)
{
	mMaxValue = inMaxValue;
	if (mValue > inMaxValue) {			// Adjust value if greater than new
		SetValue(inMaxValue);			//   maximum value
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControl::SetDataTag(
	SInt16			/* inPartCode */,
	FourCharCode	/* inTag */,
	Size			/* inDataSize */,
	void*			/* inDataPtr */)
{
	SignalStringLiteral_("Unsupported data tag");
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControl::GetDataTag(
	SInt16			/* inPartCode */,
	FourCharCode	/* inTag */,
	Size			/* inBufferSize */,
	void*			/* inBuffer */,
	Size*			/* outDataSize */) const
{
	ThrowOSErr_(errDataNotSupported);
}


// ---------------------------------------------------------------------------
//	¥ BroadcastValueMessage										   [protected]
// ---------------------------------------------------------------------------
//	Send our value message with our value as the parameter

void
LControl::BroadcastValueMessage()
{
	if (mValueMessage != msg_Nothing) {
		SInt32	value = mValue;
		BroadcastMessage(mValueMessage, &value);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FindHotSpot												   [protected]
// ---------------------------------------------------------------------------
//	Determine which hot spot, if any, contains the specified point
//
//		inPoint is in the local coordinates and is within the Frame of
//		the Control
//
//		Valid hot spot numbers are > 0. Return 0 if no hot spot is hit.
//		Controls can use any positive integers to number hot spots. Typical
//		would be to number hot spots sequentially beginning with 1, and to
//		store the hot spot locations in an array of rectangles.


SInt16
LControl::FindHotSpot(
	Point	/* inPoint */) const
{
	return	1;			// The entire control is a single hot spot, number 1
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot											   [protected]
// ---------------------------------------------------------------------------
//	Determine if a point is within a specified hot spot
//
//		inPoint is in the local coordinates for the Control
//		inHotSpot is the hot spot number
//
//		Return true if the point is within the hot spot
//
//		This function is called to check if the mouse is still within a
//		hot spot during mouse down tracking

Boolean
LControl::PointInHotSpot(
	Point 	inPoint,
	SInt16	/* inHotSpot */) const
{
	Point	portPt = inPoint;
	LocalToPortPoint(portPt);
	return PointIsInFrame(portPt.h, portPt.v);
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------
//	Track the mouse while it is down after clicking in a Control HotSpot
//
//	Returns whether the mouse is released within the HotSpot

Boolean
LControl::TrackHotSpot(
	SInt16	inHotSpot,
	Point 	/* inPoint */,
	SInt16	/* inModifiers */)
{
									// For the initial mouse down, the
									// mouse is currently inside the HotSpot
									// when it was previously outside
	Boolean		currInside = true;
	Boolean		prevInside = false;
	HotSpotAction(inHotSpot, currInside, prevInside);

	StRegion	clipR;				// Get copy of clipping region
	::GetClip(clipR);
	
									// Track mouse while it is down
	Point		currPt;
	MouseTrackingResult		trackResult;
	
	STrackingState	state;
	state.control = this;
	state.hotSpot = inHotSpot;
	
	do {
		state.isInside = currInside;
	
		UMouseTracking::TrackMouseDownWithAction(nil, currPt, trackResult,
											&TrackingCallback, 1, &state);
		
		prevInside = currInside;	// In-to-Out or Out-To-In
		currInside = PointInHotSpot(currPt, inHotSpot)
						&& ::PtInRgn(currPt, clipR);
						
		HotSpotAction(inHotSpot, currInside, prevInside);
		
	} while (trackResult != kMouseTrackingMouseReleased);
	
	return currInside;
}


// ---------------------------------------------------------------------------
//	¥ TrackingCallback									  [static] [protected]
// ---------------------------------------------------------------------------
//	Function called continuously during mouse down tracking

void
LControl::TrackingCallback(
	void*			inUserData)
{
	STrackingState*	state = static_cast<STrackingState*>(inUserData);

	(state->control)->HotSpotAction(
						state->hotSpot, state->isInside, state->isInside);
}
	

// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------
//	Take action during mouse down tracking
//
//	inCurrInside tells whether the mouse is currently inside the HotSpot
//	inPrevInside tells whether the mouse was inside the HotSpot on the
//		previous call to this function
//
//	Override to change the Control's visual appearance depending on
//	whether the mouse is inside or outside the HotSpot.
//
//	Override to continuously perform some action while the mouse is down
//	within a Control

void
LControl::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		/* inCurrInside */,
	Boolean		/* inPrevInside */)
{
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------
//	Clean up after mouse down tracking
//
//	inGoodClick indicates whether the mouse was released within the same
//	part as the original mouse down
//
//	Typically, controls have a highlighted appearance while the mouse is
//	down inside it while tracking. After tracking, you need to restore
//	the control to its normal state.

void
LControl::DoneTracking(
	SInt16		inHotSpot,
	Boolean		/* inGoodTrack */)
{
		// For most controls, calling HotSpotAction() for the case
		// where the mouse is not currently inside but was previously
		// inside will draw the control in its normal (untracked) state.

	HotSpotAction(inHotSpot, false, true);
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult													  [public]
// ---------------------------------------------------------------------------
//	Perform result of clicking and releasing mouse inside a HotSpot
//
//	Subclasses should usually override this function to implement
//	behavior associated with clicking in a Control HotSpot.

void
LControl::HotSpotResult(
	SInt16	/* inHotSpot */)
{
}


// ---------------------------------------------------------------------------
//	¥ SimulateHotSpotClick											  [public]
// ---------------------------------------------------------------------------
//	Simulate a mouse click within a HotSpot
//
//	Provide the same visual feedback and result as if the user clicked
//	inside a particular HotSpot of a Control. This function may be used
//	to implement keystroke equivalents for Control clicks.

void
LControl::SimulateHotSpotClick(
	SInt16	inHotSpot)
{
	if (IsEnabled() and IsActive()) {
		unsigned long	ticks;

		HotSpotAction(inHotSpot, true, false);	// Do action for click inside
		FlushPortBuffer();						// Force drawing
		
		::Delay(delay_Feedback, &ticks);		// Wait so user can see effect
		
		HotSpotAction(inHotSpot, false, true);	// Undo visual effect
		FlushPortBuffer();						// Force drawing
		
		HotSpotResult(inHotSpot);				// Perform result of click
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf													   [protected]
// ---------------------------------------------------------------------------
//	Handle a mouse click inside a Control
//
//	Controls have special areas, called HotSpots, that respond to mouse
//	clicks. When a mouse is clicked with a HotSpot, the mouse is tracked
//	and the Control can provide visual feedback. If the mouse is released
//	while inside the same HotSpot in which the original click occurred, a
//	result function is called.

void
LControl::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
									// Determine which HotSpot was clicked
	SInt16	theHotSpot = FindHotSpot(inMouseDown.whereLocal);

	if (theHotSpot > 0) {
		FocusDraw();
									// Track mouse while it is down
		Boolean	goodClick = TrackHotSpot(theHotSpot, inMouseDown.whereLocal,
										(SInt16) inMouseDown.macEvent.modifiers);
		DoneTracking(theHotSpot, goodClick);

		if (goodClick) {			// Mouse released inside HotSpot
			HotSpotResult(theHotSpot);
		}
	}
}


PP_End_Namespace_PowerPlant
