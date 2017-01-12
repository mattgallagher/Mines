// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LButton.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LButton.h>
#include <LStream.h>
#include <PP_Resources.h>
#include <UDrawingState.h>

#include <Icons.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LButton								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LButton::LButton()
{
	mGraphicsType	= ResType_IconList;
	mNormalID		= resID_Undefined;
	mPushedID		= resID_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LButton								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LButton::LButton(
	const LButton&	inOriginal)

	: LControl(inOriginal)
{
	mGraphicsType	= inOriginal.mGraphicsType;
	mNormalID		= inOriginal.mNormalID;
	mPushedID		= inOriginal.mPushedID;
}


// ---------------------------------------------------------------------------
//	¥ LButton								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LButton::LButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inClickedMessage,
	OSType				inGraphicsType,
	ResIDT				inNormalID,
	ResIDT				inPushedID)

	: LControl(inPaneInfo, inClickedMessage, 0, 0, 1)
{
	mGraphicsType	= inGraphicsType;
	mNormalID		= inNormalID;
	mPushedID		= inPushedID;
}


// ---------------------------------------------------------------------------
//	¥ LButton								Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//
//	Stream data must be:
//		OSType		Graphics Type ('ICN#', or 'ICON', or 'PICT')
//		ResIDT		Resource ID for normal graphic
//		ResIDT		Resource ID for pushed graphic

LButton::LButton(
	LStream*	inStream)

	: LControl(inStream)
{
	*inStream >> mGraphicsType;
	*inStream >> mNormalID;
	*inStream >> mPushedID;
}


// ---------------------------------------------------------------------------
//	¥ SetGraphicsType
// ---------------------------------------------------------------------------

void
LButton::SetGraphicsType(
	OSType	inGraphicsType)
{
	mGraphicsType = inGraphicsType;
}


// ---------------------------------------------------------------------------
//	¥ SetGraphics
// ---------------------------------------------------------------------------

void
LButton::SetGraphics(
	ResIDT	inNormalID,
	ResIDT	inPushedID)
{
	mNormalID = inNormalID;
	mPushedID = inPushedID;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw the Button

void
LButton::DrawSelf()
{
	DrawGraphic(mNormalID);
}


// ---------------------------------------------------------------------------
//	¥ DrawGraphic
// ---------------------------------------------------------------------------
//	Draw the graphic for a Button. Pane must already be focused.

void
LButton::DrawGraphic(
	ResIDT	inGraphicID)
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	StColorPenState::Normalize();

	if (inGraphicID == resID_Undefined) {
		Pattern		grayPat;
		::MacFillRect(&frame, UQDGlobals::GetDarkGrayPat(&grayPat));
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
//	¥ HotSpotAction
// ---------------------------------------------------------------------------
//	Take action during mouse down tracking
//
//	Buttons toggle between two graphics, depending on whether the mouse is
//	inside or outside the button.

void
LButton::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
									// Draw if cursor moved from IN to OUT
									//   or from OUT to IN
	if (inCurrInside != inPrevInside) {

		ResIDT	theID = mNormalID;	// Pick which graphic to draw
		if (inCurrInside) {
			theID = mPushedID;
		}

		FocusDraw();
		DrawGraphic(theID);
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	BroadcastValueMessage();		// Although value doesn't change,
									//   send message to inform Listeners
									//   that button was clicked
}


// ---------------------------------------------------------------------------
//	¥ PointIsInFrame
// ---------------------------------------------------------------------------

Boolean
LButton::PointIsInFrame(
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
			isInFrame = ::PtInIconID(thePoint, &frame, kTransformNone, mNormalID);
		}
	}

	return isInFrame;
}


PP_End_Namespace_PowerPlant
