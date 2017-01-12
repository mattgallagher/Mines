// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDropFlag.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Draws and tracks clicks for a drop flag.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDropFlag.h>
#include <UDrawingUtils.h>
#include <UGWorld.h>
#include <URegions.h>

#include <Icons.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Icon family resource ID's for the states of the drop flag
//	The size of the rectangle you pass to Draw and TrackClick determine
//	which icon size gets used.
//		Large Icon		32 x 32
//		Small Icon		16 x 16
//		Mini Icon		16 x 12

const ResIDT	icon_Up			= 2101;
const ResIDT	icon_UpDark		= icon_Up + 1;
const ResIDT	icon_Side		= 2103;
const ResIDT	icon_Down		= 2111;
const ResIDT	icon_DownDark	= icon_Down + 1;

const SInt32		delay_Animation	= 4;


// ---------------------------------------------------------------------------
//	¥ Draw
// ---------------------------------------------------------------------------
//	Draw aLDropFlag at a particular location with the specified
//	orientation

void
LDropFlag::Draw(
	const Rect&	inRect,
	bool		inIsDown)
{
	ResIDT	iconID = icon_Up;
	if (inIsDown) {
		iconID = icon_Down;
	}

	::PlotIconID(&inRect, kAlignNone, kTransformNone, iconID);
}


// ---------------------------------------------------------------------------
//	¥ TrackClick
// ---------------------------------------------------------------------------
//	Track the mouse after an initial click inside a LDropFlag. This
//	functions draws the hilited and intermediate states of the flag
//	as necessary, so the current port must be set up properly.
//
//	inRect specifies the location of the DropFlag
//	inMouse is the mouse location (usually from a MouseDown EventRecord)
//		in local coordinates
//	inIsUp specifies if the flag was up or down when the click started
//
//	Returns whether the mouse was release inside the DropFlag.

Boolean
LDropFlag::TrackClick(
	const Rect&		inRect,
	const Point&	inMouse,
	bool			inIsDown)
{
	if (not ::MacPtInRect(inMouse, &inRect)) {
		return false;
	}

	ResIDT		iconID = icon_Up;
	if (inIsDown) {
		iconID = icon_Down;
	}

									// For the initial mouse down, the
									// mouse is currently inside the HotSpot
									// when it was previously outside
	::PlotIconID(&inRect, kAlignNone, kTransformNone, (SInt16) (iconID + 1));

									// Track mouse while it is down
	Boolean	isInside = true;
	MouseTrackingResult	trackResult;
	StRegion	flagRgn(inRect);
	
	do {
		UMouseTracking::TrackMouseDownInRegion(nil, flagRgn, isInside, trackResult);
		
		ResIDT	trackIconID = iconID;
		
		if (isInside) {
			trackIconID = (SInt16) (iconID + 1);
		}
		
		::PlotIconID(&inRect, kAlignNone, kTransformNone, trackIconID);
		
	} while (trackResult != kMouseTrackingMouseReleased);
	
									// If mouse button was release while inside
									//   the flag, we animate from up to down,
									//   or vice versa.
	if (isInside) {
		RGBColor	backColor;
		::GetBackColor(&backColor);

		UInt32	ticks;				// Draw intermediate state
		::Delay(delay_Animation, &ticks);

		{
			StOffscreenGWorld	offWorld(inRect, 0, 0, nil, nil, &backColor);
			::PlotIconID(&inRect, kAlignNone, kTransformNone, icon_Side);
		}

		::Delay(delay_Animation, &ticks);

									// Draw dark end state
		ResIDT	endIconID = icon_Down + 1;
		if (inIsDown) {
			endIconID = icon_Up + 1;
		}

		{
			StOffscreenGWorld	offWorld(inRect, 0, 0, nil, nil, &backColor);
			::PlotIconID(&inRect, kAlignNone, kTransformNone, endIconID);
		}

		::Delay(delay_Animation, &ticks);
									// Draw normal end state
		::PlotIconID(&inRect, kAlignNone, kTransformNone, (SInt16) (endIconID - 1));

	} else {						// Draw original state
		::PlotIconID(&inRect, kAlignNone, kTransformNone, iconID);
	}

	return isInside;
}


PP_End_Namespace_PowerPlant
