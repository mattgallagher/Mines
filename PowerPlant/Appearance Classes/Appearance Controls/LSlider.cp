// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSlider.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSlider.h>
#include <LControlImp.h>
#include <LStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSlider								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LSlider::LSlider(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream)
{
	SInt16	kind;
	*inStream >> kind;

	SInt16	numTicks;
	*inStream >> numTicks;

		// If slider has ticks (and is NOT NonDirectional), then it uses
		// the value to specify the number of tick marks. In that case,
		// we save the value, set it to the number of ticks, make the
		// ControlImp, and restore the value.

	SInt16	saveValue = (SInt16) mValue;

	if (!(kind & kControlSliderNonDirectional)) {
		if (kind & kControlSliderHasTickMarks) {

			if (numTicks < 2) {		// Can't have less than 2 tick marks
				numTicks = 2;		//   (one on each end)
			}
			mValue = numTicks;
		}
	}

	MakeControlImp(inImpID, kind);

	mValue = saveValue;
	InitSlider();
}


// ---------------------------------------------------------------------------
//	¥ LSlider								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LSlider::LSlider(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	bool				inLiveFeedback,
	bool				inNonDirectional,
	bool				inReverseDirection,
	bool				inHasTickMarks,
	SInt16				inNumTickMarks,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inValueMessage, inValue,
						inMinValue, inMaxValue)
{
	SInt16	kind = kControlSliderProc;

	if (inLiveFeedback) {
		kind += kControlSliderLiveFeedback;
	}

		// NonDirectional option overrides ReverseDirection and
		// HasTickMarks options. So if its NonDirectional, we
		// ignore those two options.

	if (inNonDirectional) {
		kind += kControlSliderNonDirectional;

	} else {

		if (inReverseDirection) {
			kind += kControlSliderReverseDirection;
		}

		if (inHasTickMarks) {
									// The value field specifies
									//   the number of tick marks
			kind += kControlSliderHasTickMarks;

			if (inNumTickMarks < 2) {	// Can't be less than 2 tick marks
				inNumTickMarks = 2;		//   (one at each end)
			}
			mValue = inNumTickMarks;
		}
	}

	MakeControlImp(inImpID, kind);

	mValue = inValue;
	InitSlider();
}


// ---------------------------------------------------------------------------
//	¥ InitSlider							Initializer				 [private]
// ---------------------------------------------------------------------------

void
LSlider::InitSlider()
{
	if (mValue < mMinValue) {		// Enforce min/max range
		mValue = mMinValue;
	} else if (mValue > mMaxValue) {
		mValue = mMaxValue;
	}

	mControlImp->SetValue(mValue);
}


// ---------------------------------------------------------------------------
//	¥ ~LSlider								Destructor				  [public]
// ---------------------------------------------------------------------------

LSlider::~LSlider()
{
}


// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------

void
LSlider::DoTrackAction(
	SInt16	/* inHotSpot */,
	SInt32	inValue)
{
	LControl::SetValue(inValue);
}


PP_End_Namespace_PowerPlant
