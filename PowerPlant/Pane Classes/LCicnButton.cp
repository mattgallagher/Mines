// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCicnButton.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A button that draws itself using 'cicn' resources.
//
//	GetCIcon Strategy:
//		Calling GetCIcon creates a new CIconHandle initialized from data
//		in a 'cicn' resource. We call GetCIcon the first time we need to
//		draw a 'cicn' and store the CIconHandle for later use. By not
//		calling GetCIcon in a Constructor, we save the overhead of creating
//		a CIconHandle if the 'cicn' is not drawn (which could be fairly
//		common for the "pushed" 'cicn' where the user may never click
//		a particular button).

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCicnButton.h>
#include <LStream.h>
#include <UDrawingState.h>

#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCicnButton							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCicnButton::LCicnButton()
{
	mNormalID		= resID_Undefined;
	mPushedID		= resID_Undefined;
	mNormalCicnH	= nil;
	mPushedCicnH	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LCicnButton							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCicnButton::LCicnButton(
	const LCicnButton&	inOriginal)

	: LControl(inOriginal)
{
	mNormalID		= inOriginal.mNormalID;
	mPushedID		= inOriginal.mPushedID;
	mNormalCicnH	= nil;
	mPushedCicnH	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LCicnButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LCicnButton::LCicnButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inClickedMessage,
	ResIDT				inNormalID,
	ResIDT				inPushedID)

	: LControl(inPaneInfo, inClickedMessage, 0, 0, 1)
{
	mNormalID		= inNormalID;
	mPushedID		= inPushedID;
	mNormalCicnH	= nil;
	mPushedCicnH	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LCicnButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//
//	Stream data must be:
//		ResIDT		Resource ID for normal graphic
//		ResIDT		Resource ID for pushed graphic

LCicnButton::LCicnButton(
	LStream*	inStream)

	: LControl(inStream)
{
	*inStream >> mNormalID;
	*inStream >> mPushedID;

	mNormalCicnH = nil;
	mPushedCicnH = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LCicnButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LCicnButton::~LCicnButton()
{
	if (mNormalCicnH != nil) {
		::DisposeCIcon(mNormalCicnH);
	}

	if (mPushedCicnH != nil) {
		::DisposeCIcon(mPushedCicnH);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCicns														  [public]
// ---------------------------------------------------------------------------
//	Specify new ID's for the normal and pushed 'cicn' resources

void
LCicnButton::SetCicns(
	ResIDT	inNormalID,
	ResIDT	inPushedID)
{
	mNormalID = inNormalID;			// Store new ID's
	mPushedID = inPushedID;

		// For both normal and pushed, dispose of existing CIconHandles

	if (mNormalCicnH != nil) {
		::DisposeCIcon(mNormalCicnH);
	}
	mNormalCicnH = nil;

	if (mPushedCicnH != nil) {
		::DisposeCIcon(mPushedCicnH);
	}
	mPushedCicnH = nil;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw the CicnButton

void
LCicnButton::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	StColorPenState::Normalize();

	if (mNormalCicnH == nil) {		// Load 'cicn' if necessary
		mNormalCicnH = ::GetCIcon(mNormalID);
	}

	if (mNormalCicnH != nil) {
		::PlotCIcon(&frame, mNormalCicnH);
	}
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot												   [protected]
// ---------------------------------------------------------------------------

SInt16
LCicnButton::FindHotSpot(
	Point	inPoint) const
{
	SInt16	hotSpot = 0;

	if (mNormalCicnH != nil) {

		Rect		frame;
		CalcLocalFrameRect(frame);

		CIconPtr	iconP = *mNormalCicnH;

			// If frame does not match bounds of icon, the icon is
			// scaled when drawn. We assume the whole frame is the hot spot.

			// If the frame matches the bounds, we check to see if inPoint
			// corresponds to a non-white pixel within the icon mask.

		if ( ((frame.right - frame.left) !=
			  (iconP->iconMask.bounds.right - iconP->iconMask.bounds.left)) ||

			 ((frame.bottom - frame.top) !=
			  (iconP->iconMask.bounds.bottom - iconP->iconMask.bounds.top))) {

			hotSpot = 1;

		} else if ( ::BitTst( (Ptr) &(iconP->iconMaskData[
										(inPoint.v - frame.top) *
										iconP->iconMask.rowBytes / 2]),
						   (inPoint.h - frame.left) ) ) {

			hotSpot = 1;
		}
	}

	return hotSpot;
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot											   [protected]
// ---------------------------------------------------------------------------
//	Determine if a point is within a specified hot spot

Boolean
LCicnButton::PointInHotSpot(
	Point 	inPoint,
	SInt16	inHotSpot) const
{
	return ( LControl::PointInHotSpot(inPoint, inHotSpot)  &&
			 (FindHotSpot(inPoint) == inHotSpot) );
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------
//	Take action during mouse down tracking
//
//	CicnButtons toggle between two graphics, depending on whether the mouse is
//	inside or outside the CicnButton.

void
LCicnButton::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
									// Draw if cursor moved from IN to OUT
									//   or from OUT to IN
	if (inCurrInside != inPrevInside) {
		Rect	frame;
		CalcLocalFrameRect(frame);
		StColorPenState::Normalize();

									// Pick which graphic to draw
		CIconHandle	theCicnH = mNormalCicnH;
		if (inCurrInside) {
			theCicnH = mPushedCicnH;
		}

		if (theCicnH == nil) {		// Load 'cicn' if necessary
			if (inCurrInside) {
				theCicnH = mPushedCicnH = ::GetCIcon(mPushedID);
			} else {
				theCicnH = mNormalCicnH = ::GetCIcon(mNormalID);
			}
		}

		if (theCicnH != nil) {
			FocusDraw();
			::PlotCIcon(&frame, theCicnH);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LCicnButton::HotSpotResult(
	SInt16	inHotSpot)
{
									// Undo CicnButton hilighting
	HotSpotAction(inHotSpot, false, true);
	BroadcastValueMessage();		// Although value doesn't change,
									//   send message to inform Listeners
									//   that CicnButton was clicked
}


PP_End_Namespace_PowerPlant
