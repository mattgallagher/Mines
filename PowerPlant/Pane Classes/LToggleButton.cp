// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LToggleButton.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Button that can be On or Off and that has an optional transition states
//	for animating between the On/Off states. The graphics for the Button can
//	be either an icon family ('ICN#"), icon ('ICON'), or picture ('PICT').
//
//	States:
//		On				Value is one
//		On Click		Clicking inside a ToggleButton that is currently on
//		Off				Value is zero
//		Off Click		Clicking inside a ToggleButton that is currently off
//		Transition		Moving between on/off (after a click)
//
//	The On Click, Off Click, and Transition states are used for animation
//	and visual feedback while tracking the mouse. Use resID_Undefined for
//	these states to suppress the animation.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LToggleButton.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <PP_Messages.h>
#include <PP_Resources.h>

#include <Icons.h>

PP_Begin_Namespace_PowerPlant

const	SInt32	delay_Animation	= 6;


// ---------------------------------------------------------------------------
//	¥ LToggleButton							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LToggleButton::LToggleButton()
{
	mGraphicsType	= ResType_IconList;
	mOnID			= resID_Undefined;
	mOnClickID		= resID_Undefined;
	mOffID			= resID_Undefined;
	mOffClickID		= resID_Undefined;
	mTransitionID	= resID_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LToggleButton							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LToggleButton::LToggleButton(
	const LToggleButton&	inOriginal)

	: LControl(inOriginal)
{
	mGraphicsType	= inOriginal.mGraphicsType;
	mOnID			= inOriginal.mOnID;
	mOnClickID		= inOriginal.mOnClickID;
	mOffID			= inOriginal.mOffID;
	mOffClickID		= inOriginal.mOffClickID;
	mTransitionID	= inOriginal.mTransitionID;
}


// ---------------------------------------------------------------------------
//	¥ LToggleButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LToggleButton::LToggleButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inClickedMessage,
	OSType				inGraphicsType,
	ResIDT				inOnID,
	ResIDT				inOnClickID,
	ResIDT				inOffID,
	ResIDT				inOffClickID,
	ResIDT				inTransitionID)

	: LControl(inPaneInfo, inClickedMessage, 0, 0, 1)
{
	mGraphicsType	= inGraphicsType;
	mOnID			= inOnID;
	mOnClickID		= inOnClickID;
	mOffID			= inOffID;
	mOffClickID		= inOffClickID;
	mTransitionID	= inTransitionID;
}


// ---------------------------------------------------------------------------
//	¥ LToggleButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//
//	Stream data must be:
//		OSType		Graphics Type ('ICN#', or 'ICON', or 'PICT')
//		ResIDT		Resource ID of on state
//		ResIDT		Resource ID of on click state
//		ResIDT		Resource ID of off state
//		ResIDT		Resource ID of off click state
//		ResIDT		Resource ID of transition state

LToggleButton::LToggleButton(
	LStream*	inStream)

	: LControl(inStream)
{
	*inStream >> mGraphicsType;
	*inStream >> mOnID;
	*inStream >> mOnClickID;
	*inStream >> mOffID;
	*inStream >> mOffClickID;
	*inStream >> mTransitionID;
}


// ---------------------------------------------------------------------------
//	¥ ~LToggleButton						Destructor				  [public]
// ---------------------------------------------------------------------------

LToggleButton::~LToggleButton()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetGraphicsType
// ---------------------------------------------------------------------------
//	Specify the kind of graphic for a ToggleButton
//
//	Supported kinds are:
//		'ICN#'		Icon Family
//		'ICON'		Black and White Icon
//		'PICT'		Picture

void
LToggleButton::SetGraphicsType(
	OSType	inGraphicsType)
{
	mGraphicsType = inGraphicsType;
}


// ---------------------------------------------------------------------------
//	¥ SetGraphics
// ---------------------------------------------------------------------------
//	Specify the resources IDs for all states of a ToggleButton

void
LToggleButton::SetGraphics(
	ResIDT			inOnID,
	ResIDT			inOnClickID,
	ResIDT			inOffID,
	ResIDT			inOffClickID,
	ResIDT			inTransitionID)
{
	mOnID			= inOnID;
	mOnClickID		= inOnClickID;
	mOffID			= inOffID;
	mOffClickID		= inOffClickID;
	mTransitionID	= inTransitionID;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw the ToggleButton

void
LToggleButton::DrawSelf()
{
	ResIDT	theState = mOffID;		// Value of 0 is OFF
	if (mValue != 0) {				// Non-zero value is ON
		theState = mOnID;
	}
	DrawGraphic(theState);
}


// ---------------------------------------------------------------------------
//	¥ DrawGraphic
// ---------------------------------------------------------------------------
//	Draw the graphic for a ToggleButton. Pane must already be focused.

void
LToggleButton::DrawGraphic(
	ResIDT	inGraphicID)
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	StColorPenState::Normalize();

	if (inGraphicID == resID_Undefined) {
		Pattern		ltGrayPat;
		::MacFillRect(&frame, UQDGlobals::GetLightGrayPat(&ltGrayPat));
		::MacFrameRect(&frame);

	} else if (mGraphicsType == ResType_IconList) {
		if (mEnabled == triState_On) {
			::PlotIconID(&frame, kAlignNone, kTransformNone, inGraphicID);
		} else {
			::PlotIconID(&frame, kAlignNone, kTransformDisabled, inGraphicID);
		}

	} else if (mGraphicsType == ResType_Picture) {
		PicHandle	macPictureH = ::GetPicture(inGraphicID);
		if (macPictureH != nil) {
			::DrawPicture(macPictureH, &frame);
		}

	} else if (mGraphicsType == ResType_Icon) {
		Handle	iconHandle = ::GetIcon(inGraphicID);
		if (iconHandle != nil) {
			::PlotIcon(&frame, iconHandle);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Turn a ToggleButton on or off

void
LToggleButton::SetValue(
	SInt32	inValue)
{
	if (inValue != mValue) {
		LControl::SetValue(inValue);
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction
// ---------------------------------------------------------------------------
//	Take action during mouse down tracking

void
LToggleButton::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
									// Draw if cursor moved from IN to OUT
									//   or from OUT to IN
	if (inCurrInside != inPrevInside) {

									// Pick which graphic to draw
		ResIDT	theState = mOffID;

		if (inCurrInside) {			// Use "click" state when inside
			theState = mOffClickID;
			if (mValue != 0) {
				theState = mOnClickID;
			}

		} else if (mValue != 0) {
			theState = mOnID;
		}

		if (theState != resID_Undefined) {
			FocusDraw();
			DrawGraphic(theState);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------
//	Respond to a click in a ToggleButton by toggling it between
//	on (value = 0) and off (value = 1)

void
LToggleButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
		// Animate Button from current state to a transition state
		// and then to the end state

	if (mTransitionID != resID_Undefined) {
		FocusDraw();

		DrawGraphic(mTransitionID);
		UInt32 ticks;
		::Delay(delay_Animation, &ticks);

		ResIDT	endClickState = mOnClickID;
		if (mValue != 0) {
			endClickState = mOffClickID;
		}
		DrawGraphic(endClickState);
	}

	SetValue(1 - GetValue());		// Toggle between on/off
}


// ---------------------------------------------------------------------------
//	¥ PointIsInFrame
// ---------------------------------------------------------------------------
//	Return whether a point is inside a ToggleButton

Boolean
LToggleButton::PointIsInFrame(
	SInt32	inHoriz,
	SInt32	inVert) const
{
	Boolean	isInFrame = LPane::PointIsInFrame(inHoriz, inVert);

	if (isInFrame && (mGraphicsType == ResType_IconList)) {
		Rect	frame;
		if (CalcPortFrameRect(frame)) {
			Point	thePoint;
			thePoint.h = (SInt16) inHoriz;
			thePoint.v = (SInt16) inVert;
			ResIDT	theState = mOffID;
			if (mValue != 0) {
				theState = mOnID;
			}
			isInFrame = ::PtInIconID(thePoint, &frame, kTransformNone, theState);
		}
	}

	return isInFrame;
}


PP_End_Namespace_PowerPlant
