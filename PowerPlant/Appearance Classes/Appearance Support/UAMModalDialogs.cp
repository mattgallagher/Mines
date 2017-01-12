// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAMModalDialogs.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UAMModalDialogs.h>
#include <UModalDialogs.h>
#include <LEditText.h>
#include <LWindow.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ AskForOneNumber
// ---------------------------------------------------------------------------
//	Present a Moveable Modal dialog for entering a single number
//
//	Returns TRUE if entry is OK'd.
//	On entry, ioNumber is the current value to display for the number
//	On exit, ioNumber is the new value if OK'd, unchanged if Canceled

Boolean
UAMModalDialogs::AskForOneNumber(
	LCommander*		inSuper,
	ResIDT			inDialogID,
	PaneIDT			inEditTextID,
	SInt32&			ioNumber)
{
	StDialogHandler	theHandler(inDialogID, inSuper);
	LWindow*		theDialog = theHandler.GetDialog();

	LEditText*		theField = dynamic_cast<LEditText*>
								(theDialog->FindPaneByID(inEditTextID));

	if (theField == nil) {
		SignalStringLiteral_("No EditField with specified ID");
		return false;
	}

	theField->SetValue(ioNumber);
	theField->SelectAll();
	theDialog->SetLatentSub(theField);
	theDialog->Show();

	Boolean		entryOK = false;

	while (true) {
		MessageT	hitMessage = theHandler.DoDialog();

		if (hitMessage == msg_Cancel) {
			break;

		} else if (hitMessage == msg_OK) {
			ioNumber = theField->GetValue();
			entryOK = true;
			break;
		}
	}

	return entryOK;
}


// ---------------------------------------------------------------------------
//	¥ AskForOneString
// ---------------------------------------------------------------------------
//	Present a Moveable Modal dialog for entering a single string
//
//	Returns TRUE if entry is OK'd.
//	On entry, ioString is the current text to display for the number
//	On exit, ioString is the new text if OK'd, unchanged if Canceled

Boolean
UAMModalDialogs::AskForOneString(
	LCommander*		inSuper,
	ResIDT			inDialogID,
	PaneIDT			inEditTextID,
	Str255			ioString)
{
	StDialogHandler	theHandler(inDialogID, inSuper);
	LWindow*		theDialog = theHandler.GetDialog();

	LEditText*		theField = dynamic_cast<LEditText*>
								(theDialog->FindPaneByID(inEditTextID));

	if (theField == nil) {
		SignalStringLiteral_("No EditField with specified ID");
		return false;
	}

	theField->SetDescriptor(ioString);
	theField->SelectAll();
	theDialog->SetLatentSub(theField);
	theDialog->Show();

	Boolean		entryOK = false;

	while (true) {
		MessageT	hitMessage = theHandler.DoDialog();

		if (hitMessage == msg_Cancel) {
			break;

		} else if (hitMessage == msg_OK) {
			theField->GetDescriptor(ioString);
			entryOK = true;
			break;
		}
	}

	return entryOK;
}


PP_End_Namespace_PowerPlant
