// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LLittleArrows.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LLittleArrows.h>
#include <LControlImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LLittleArrows							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LLittleArrows::LLittleArrows(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	InitLittleArrows();
}


// ---------------------------------------------------------------------------
//	¥ LLittleArrows							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LLittleArrows::LLittleArrows(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlLittleArrowsProc,
						Str_Empty, 0, inValueMessage, inValue,
						inMinValue, inMaxValue)
{
	InitLittleArrows();
}


// ---------------------------------------------------------------------------
//	¥ InitLittleArrows						Initializer				 [private]
// ---------------------------------------------------------------------------

void
LLittleArrows::InitLittleArrows()
{
	if (mValue < mMinValue) {		// Enforce min/max range
		mValue = mMinValue;
	} else if (mValue > mMaxValue) {
		mValue = mMaxValue;
	}

	mControlImp->SetValue(mValue);
}


// ---------------------------------------------------------------------------
//	¥ ~LLittleArrows						Destructor				  [public]
// ---------------------------------------------------------------------------

LLittleArrows::~LLittleArrows()
{
}


// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------

void
LLittleArrows::DoTrackAction(
	SInt16		inHotSpot,
	SInt32		/* inValue */)
{
	if (inHotSpot == kControlUpButtonPart) {
		SetValue(mValue + 1);

	} else if (inHotSpot == kControlDownButtonPart) {
		SetValue(mValue - 1);
	}
}


PP_End_Namespace_PowerPlant
