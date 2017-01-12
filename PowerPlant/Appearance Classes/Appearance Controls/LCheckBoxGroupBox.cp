// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCheckBoxGroupBox.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	A visual group box with a check box as its title.
//
//	The subpanes within the group box are all enabled/disabled depending
//	on whether the check box is on/off.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCheckBoxGroupBox.h>
#include <PP_Messages.h>
#include <TArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCheckBoxGroupBox						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LCheckBoxGroupBox::LCheckBoxGroupBox(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LCheckBoxGroupBox						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LCheckBoxGroupBox::LCheckBoxGroupBox(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	Boolean				inPrimary,
	ResIDT				inTextTraitsID,
	ConstStringPtr		inTitle,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
						inPrimary ? kControlGroupBoxCheckBoxProc
								  : kControlGroupBoxSecondaryCheckBoxProc,
						inTitle, inTextTraitsID, inValueMessage, inValue,
						kControlCheckBoxUncheckedValue,
						kControlCheckBoxMixedValue)
{
}


// ---------------------------------------------------------------------------
//	¥ LCheckBoxGroupBox						Destructor				  [public]
// ---------------------------------------------------------------------------

LCheckBoxGroupBox::~LCheckBoxGroupBox()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LCheckBoxGroupBox::FinishCreateSelf()
{
		// For some reason, CheckBoxGroup (in Appearance 1.0) does not
		// use the initial value passed to NewControl. It is always
		// unchecked. So, we set the value here, which also has the desired
		// side effect of adjusting the state of the subpanes.

		// The Group may be in an inconsistent state, so we force the
		// value to a bogus number (-1) to make sure that SetValue() will
		// change the value and get everything in synch.

	mControlSubPane->PokeValue(-1);

		// LControlView stashes the desired setting in the userCon of
		// the ControlSubPane

	SetValue(mControlSubPane->GetUserCon());
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------
//	The check box's value has changed, so we need to adjust the state of
//	the group view

void
LCheckBoxGroupBox::SetValue(
	SInt32	inValue)
{
	if (inValue != mControlSubPane->PeekValue()) {
		AdjustSubPanes(inValue >= Button_On);

		LControlView::SetValue(inValue);

		Draw(nil);						// Draw content in its new state
		DontRefresh();					// We've drawn so no need to update
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
LCheckBoxGroupBox::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	if (inMessage == msg_ControlClicked) {		// CheckBox was clicked
		LControl	*control = static_cast<LControl*>(ioParam);
		if (control != nil) {
			AdjustSubPanes(control->GetValue() == Button_On);
		}

	} else {
		BroadcastMessage(inMessage, ioParam);
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LCheckBoxGroupBox::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	SInt16	newValue = Button_On;
	if (GetValue() == Button_On) {
		newValue = Button_Off;
	}
	SetValue(newValue);
}


// ---------------------------------------------------------------------------
//	¥ AdjustSubPanes											   [protected]
// ---------------------------------------------------------------------------
//	Adjust the enabled/disabled state of the subpanes depending on
//	whether the check box is on or off.

void
LCheckBoxGroupBox::AdjustSubPanes(
	bool	inEnable)
{
	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane*	theSub;

	if (inEnable) {

		while (iterator.Next(theSub)) {		// Enable all subpanes
			theSub->Enable();
		}

	} else {

		while (iterator.Next(theSub)) {			// Disable all subpanes except
			if (theSub != mControlSubPane) {	//   the check box
				theSub->Disable();
			}
		}
	}
}


PP_End_Namespace_PowerPlant
