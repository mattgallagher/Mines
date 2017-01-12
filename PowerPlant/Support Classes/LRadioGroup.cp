// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRadioGroup.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	NOTE: Where possible, use LRadioGroupView instead. As a View, it's
//	easier to use with the Constructor view editor and you can get a
//	pointer to it by calling FindPaneByID.
//
//	Manages a group of Controls by ensuring that only one Control in a
//	group is "on" at any time. This is the standard behavior of a set of
//	Radio Buttons.
//
//	Although you will normally use this class with StdRadioButton objects,
//	the group members can be any kind of Control. Therefore, you can group
//	your own custom Control objects. This class assumes the "off" value is
//	zero, and the "on" value is one.
//
//	RadioGroup and its Controls have a Listener/Broadcaster relationship.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LRadioGroup.h>
#include <LControl.h>
#include <LView.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LRadioGroup							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LRadioGroup::LRadioGroup()
{
	mCurrentRadio = nil;
}


// ---------------------------------------------------------------------------
//	¥ LRadioGroup							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	Stream data is a list of PaneID numbers for the Controls in the group.
//	The Controls must already have been created and must be contained within
//	the current default view.

LRadioGroup::LRadioGroup(
	LStream*	inStream)
{
	mCurrentRadio = nil;

	SInt16	numberOfRadios;
	*inStream >> numberOfRadios;

	LView*	defaultView = LPane::GetDefaultView();

	StHidePen	dontDraw;
	for (SInt16 i = 1; i <= numberOfRadios; i++) {
		PaneIDT		radioID;
		*inStream >> radioID;
		AddRadio(dynamic_cast<LControl*>(defaultView->FindPaneByID(radioID)));
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LRadioGroup							Destructor				  [public]
// ---------------------------------------------------------------------------

LRadioGroup::~LRadioGroup()
{
}


// ---------------------------------------------------------------------------
//	¥ AddRadio														  [public]
// ---------------------------------------------------------------------------
//	Add a Control to a RadioGroup
//
//	If the Control is "on" it becomes the current radio, turning off the
//	former current radio.

void
LRadioGroup::AddRadio(
	LControl*	inRadio)
{
	if (inRadio == nil) {
		return;
	}

	inRadio->AddListener(this);			// Attach Radio to this Group

	if (mCurrentRadio == nil) {			// First Radio added is turned on
		inRadio->SetValue(Button_On);	//   and becomes the current Radio
		mCurrentRadio = inRadio;

	} else if (inRadio->GetValue() == Button_On) {
										// If any subsequent Radio is on,
										//   handle same as a click (which
										//   will change the current radio)
		ListenToMessage(msg_ControlClicked, inRadio);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentRadioID												  [public]
// ---------------------------------------------------------------------------
//	Return the PaneID of the current RadioButton in the Group

PaneIDT
LRadioGroup::GetCurrentRadioID()
{
	PaneIDT	currentID = 0;
	if (mCurrentRadio != nil) {
		currentID = mCurrentRadio->GetPaneID();
	}
	return currentID;
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------
//	React to Messages from its Broadcasters, which must be the Controls
//	in the group.

void
LRadioGroup::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	switch (inMessage) {

		case msg_BroadcasterDied:
			if ((static_cast<LBroadcaster*>(ioParam)) == mCurrentRadio) {
				mCurrentRadio = nil;	// Current RadioButton was deleted
			}

			if (mBroadcasters.GetCount() == 1) {
										// Last Broadcaster is dying.
				delete this;			// Nothing left in group, so
										//   delete this RadioGroup
			}
			break;

		case msg_ControlClicked:		// Control clicked was turned on, so
										//   we must switch current Radios
			LControl*	theRadio = static_cast<LControl*>(ioParam);
			SignalIf_(theRadio == nil);

			if (theRadio != mCurrentRadio) {
										// Turn off current On button
				if (mCurrentRadio != nil) {
					mCurrentRadio->SetValue(Button_Off);
				}
				mCurrentRadio = theRadio;
			}
			break;
	}
}


PP_End_Namespace_PowerPlant
