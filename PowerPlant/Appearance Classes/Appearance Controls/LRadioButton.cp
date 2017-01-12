// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRadioButton.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LRadioButton.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LRadioButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LRadioButton::LRadioButton(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LRadioButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LRadioButton::LRadioButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlRadioButtonProc,
					inTitle, inTextTraitsID, inValueMessage, inValue, 0, 2)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LRadioButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LRadioButton::~LRadioButton()
{
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set value of a RadioButton

void
LRadioButton::SetValue(
	SInt32	inValue)
{
	if (inValue != mValue) {

			// If turning RadioButton on, broadcast message so that the
			// RadioGroup (if present) will turn off the other RadioButtons
			// in the group.

		if (inValue == Button_On) {
			BroadcastMessage(msg_ControlClicked, (void*) this);
		}

		LControlPane::SetValue(inValue);
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LRadioButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
		// In the Mac interface, clicking on a RadioButton always
		// turns it on (or leaves it on). The standard way to turn
		// off a RadioButton is to turn on another one in the
		// same Radio Group. A Radio Group will normally be a
		// Listener of a RadioButton.

	SetValue(Button_On);
}


PP_End_Namespace_PowerPlant
