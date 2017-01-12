// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAControlImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAControlImp.h>

#include <LStream.h>
#include <PP_Messages.h>
#include <UEventMgr.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
// 	¥ LGAControlImp							Constructor				  [public]
// ---------------------------------------------------------------------------

LGAControlImp::LGAControlImp(
	LControlPane*	inControlPane)

	: LControlImp(inControlPane)
{
	mPushed = false;
}


// ---------------------------------------------------------------------------
// 	¥ LGASeparatorImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAControlImp::LGAControlImp(
	LStream*	inStream)

	: LControlImp (inStream)
{
	mPushed = false;
}


// ---------------------------------------------------------------------------
// 	¥ ~LGAControlImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAControlImp::~LGAControlImp ()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
// 	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::SetPushedState(
	Boolean		inPushedState)
{
	if (mPushed != inPushedState) {
		mPushed = inPushedState;

		mControlPane->Draw(nil);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
// 	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::EnableSelf()
{
	if (mControlPane->GetActiveState() == triState_On) {
		Refresh();
	}
}


// ---------------------------------------------------------------------------
// 	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::DisableSelf()
{
	if (mControlPane->GetActiveState() == triState_On) {
		Refresh();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
// 	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::ActivateSelf()
{
	if (mControlPane->GetEnabledState() == triState_On) {
		Refresh();
	}
}


// ---------------------------------------------------------------------------
// 	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::DeactivateSelf()
{
	if (mControlPane->GetEnabledState() == triState_On) {
		Refresh();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
// 	¥ PointInHotSpot												  [public]
// ---------------------------------------------------------------------------

Boolean
LGAControlImp::PointInHotSpot(
	Point 	inPoint,
	SInt16	/* inHotSpot */) const
{
	Rect	frameRect;
	CalcLocalFrameRect(frameRect);

	return ::MacPtInRect(inPoint, &frameRect);

}


// ---------------------------------------------------------------------------
// 	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LGAControlImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		/* inModifiers */)
{
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
		::GetMouse ( &currPt );			// Must keep track if mouse moves from
		prevInside = currInside;		// In-to-Out or Out-To-In
		currInside =  (mControlPane->PointInHotSpot(currPt, inHotSpot)
							&& ::PtInRgn(currPt, clipR));
		 mControlPane->HotSpotAction(inHotSpot, currInside, prevInside);
	}

										// Get location from MouseUp event
	EventRecord	macEvent;
	if (UEventMgr::GetMouseUp(macEvent)) {
		currPt = macEvent.where;
		::GlobalToLocal(&currPt);
		prevInside = currInside;
		currInside = (mControlPane->PointInHotSpot(currPt, inHotSpot)
							&& ::PtInRgn(currPt, clipR));
		 mControlPane->HotSpotAction(inHotSpot, currInside, prevInside);
	}
										// Return if we are still inside the
	return currInside;					//   control or not
}


// ---------------------------------------------------------------------------
// 	¥ HotSpotAction													  [public]
// ---------------------------------------------------------------------------

void
LGAControlImp::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		/* inPrevInside */)
{
								// Turn hiliting on and off as appropriate
	SetPushedState(inCurrInside);
}


PP_End_Namespace_PowerPlant
