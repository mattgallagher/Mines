// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRadioGroupView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages a group of Controls by ensuring that only one Control in a
//	group is "on" at any time. This is the standard behavior of a set of
//	Radio Buttons.
//
//	A RadioGroupView assumes that all its subpanes that are objects of
//	class LControl (or any subclass of LControl) are members of the group.
//	The "off" value is zero, and the "on" value is one.
//
//	To coordinate with a RadioGroupView, Controls that wish to behave as
//	radio buttons must broadcast the msg_ControlClicked when their value
//	changes to one ("on"). This signals the RadioGroupView, which is a
//	Listener, to turn off the other radio buttons in the group.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LRadioGroupView.h>
#include <LControl.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <TArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LRadioGroupView						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LRadioGroupView::LRadioGroupView()
{
	mCurrentRadio = nil;
}


// ---------------------------------------------------------------------------
//	¥ LRadioGroupView						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LRadioGroupView::LRadioGroupView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)

	: LView(inPaneInfo, inViewInfo)
{
	mCurrentRadio = nil;
}


// ---------------------------------------------------------------------------
//	¥ LRadioGroupView						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LRadioGroupView::LRadioGroupView(
	LStream*	inStream)

	: LView(inStream)
{
	mCurrentRadio = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LRadioGroupView						Destructor				  [public]
// ---------------------------------------------------------------------------

LRadioGroupView::~LRadioGroupView()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LRadioGroupView::FinishCreateSelf()
{
		// Make all SubPanes that are Controls participants in
		// the Radio Group

	bool	allOff = true;

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		LControl	*theControl = dynamic_cast<LControl*>(theSub);
		if (theControl != nil) {
			AddRadio(theControl);
			allOff &= (theControl->GetValue() == Button_Off);
		}
	}

		// If all RadioButtons are OFF, turn ON the first one

	if (allOff && mRadioButtons.GetCount() > 0) {
		mRadioButtons[1]->SetValue(Button_On);
	}
}


// ---------------------------------------------------------------------------
//	¥ AddRadio														  [public]
// ---------------------------------------------------------------------------
//	Add a Control to a RadioGroupView
//
//	If the Control is "on" it becomes the current radio, turning off the
//	former current radio.

void
LRadioGroupView::AddRadio(
	LControl	*inRadio)
{
	if (inRadio == nil) {
		return;
	}

	inRadio->AddListener(this);			// Attach Radio to this Group
	mRadioButtons.AddItem(inRadio);

	if (inRadio->GetValue() == Button_On) {
										// RadioButton is ON, so it becomes
										//   the current one
		ListenToMessage(msg_ControlClicked, inRadio);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentRadioID												  [public]
// ---------------------------------------------------------------------------
//	Return the PaneID of the current RadioButton in the Group

PaneIDT
LRadioGroupView::GetCurrentRadioID()
{
	PaneIDT	currentID = 0;
	if (mCurrentRadio != nil) {
		currentID = mCurrentRadio->GetPaneID();
	}
	return currentID;
}


// ---------------------------------------------------------------------------
//	¥ SetCurrentRadioID												  [public]
// ---------------------------------------------------------------------------
//	Set the current RadioButton in the group to the Control with the
//	specified PaneID

void
LRadioGroupView::SetCurrentRadioID(
	PaneIDT		inRadioID)
{
	LControl	*theRadio = dynamic_cast<LControl*>(FindPaneByID(inRadioID));

	if (theRadio != nil) {
		theRadio->SetValue(Button_On);
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------
//	React to Messages from its Broadcasters, which must be the Controls
//	in the group.

void
LRadioGroupView::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	switch (inMessage) {

		case msg_BroadcasterDied: {		// A button is being deleted

				// ioParam is an LBroadcaster*. We must static_cast to
				// an LControl* rather than dynamic_cast since this
				// call is coming from the destructor of LBroadcaster
				// and the LControl part has already been destroyed.
				// This is OK, since we only need the LControl* pointer
				// value to compare to our stored values and we never
				// call a LControl function using this pointer.

			LControl	*deadButton = static_cast<LControl*>
										(static_cast<LBroadcaster*>(ioParam));

			if (deadButton == mCurrentRadio) {
				mCurrentRadio = nil;	// Current RadioButton died
			}

			mRadioButtons.Remove(deadButton);
			break;
		}

		case msg_ControlClicked:		// Control clicked was turned on, so
										//   it becomes the current one
			LControl	*theRadio = static_cast<LControl*>(ioParam);
			SignalIf_(theRadio == nil);

			if (theRadio != mCurrentRadio) {
										// Turn off all other buttons
				TArrayIterator<LControl*>	iterator(mRadioButtons);
				LControl	*button;
				while (iterator.Next(button)) {
					if (button != theRadio) {
						button->SetValue(Button_Off);
					}
				}

				mCurrentRadio = theRadio;
			}
			break;
	}
}


PP_End_Namespace_PowerPlant
