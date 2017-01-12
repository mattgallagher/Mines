// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScrollBar.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LScrollBar.h>
#include <LControlImp.h>
#include <PP_Messages.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LScrollBar							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LScrollBar::LScrollBar(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	InitScrollBar();
}


// ---------------------------------------------------------------------------
//	¥ LScrollBar							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LScrollBar::LScrollBar(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	bool				inLiveScrolling,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID,
					inLiveScrolling ? kControlScrollBarLiveProc
									: kControlScrollBarProc,
					Str_Empty, 0, inValueMessage, inValue, inMinValue,
					inMaxValue)
{
	InitScrollBar();
}


// ---------------------------------------------------------------------------
//	¥ InitScrollBar							Initializer				 [private]
// ---------------------------------------------------------------------------

void
LScrollBar::InitScrollBar()
{
	if (mValue < mMinValue) {		// Enforce min/max range
		mValue = mMinValue;
	} else if (mValue > mMaxValue) {
		mValue = mMaxValue;
	}

	mControlImp->SetValue(mValue);
}


// ---------------------------------------------------------------------------
//	¥ ~LScrollBar							Destructor				  [public]
// ---------------------------------------------------------------------------

LScrollBar::~LScrollBar()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------

void
LScrollBar::DoTrackAction(
	SInt16	inHotSpot,
	SInt32	inValue)
{
	if (inHotSpot >= kControlIndicatorPart) {	// Thumb is being dragged
		LControl::SetValue(inValue);			// Synch PP's value with
	}											//   Control Mgr value

	SScrollMessage	message;
	message.scrollBar	= this;
	message.hotSpot		= inHotSpot;
	message.value		= inValue;

	BroadcastMessage(msg_ScrollAction, &message);
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LScrollBar::DoneTracking(
	SInt16		inHotSpot,
	Boolean		inGoodClick)
{
	LControlPane::DoneTracking(inHotSpot, inGoodClick);

		// Notify Listeners (usually a ScrollerView) that tracking
		// has finished. We need to notify even if the click isn't
		// "good" because live scrolling will occur for all clicks.

	MessageT	msg = msg_ControlClicked;

	if (inHotSpot >= kControlIndicatorPart) {
		msg = msg_ThumbDragged;
	}

	BroadcastMessage(msg, this);
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------

void
LScrollBar::ActivateSelf()
{
	if (mEnabled == triState_On) {
		FocusDraw();
		StClipRgnState	emptyClip(nil);

		mControlImp->Hilite(kControlNoPart);

		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LScrollBar::DeactivateSelf()
{
	if (mEnabled == triState_On) {
		FocusDraw();
		StClipRgnState	emptyClip(nil);

		mControlImp->Hilite(kControlInactivePart);

		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetScrollViewSize												  [public]
// ---------------------------------------------------------------------------

void
LScrollBar::SetScrollViewSize(
	SInt32		inViewSize)
{
	mControlImp->SetScrollViewSize(inViewSize);
}


PP_End_Namespace_PowerPlant
