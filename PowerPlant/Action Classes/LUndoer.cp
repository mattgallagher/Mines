// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LUndoer.cp					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Attachment for implementing Undo
//
//	An LUndoer object can be attached to a LCommander. The Undoer will
//	store the last Action object posted to the Commander, enable and
//	set the text for the "undo" menu item, and respond to the "undo"
//	command by telling the Action object to Undo/Redo itself.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LUndoer.h>
#include <LAction.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LUndoer								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LUndoer::LUndoer()
{
	mAction = nil;
}


// ---------------------------------------------------------------------------
//	¥ LUndoer								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LUndoer::LUndoer(
	LStream*		inStream)

	: LAttachment(inStream)
{
	mMessage = msg_AnyMessage;
	mAction  = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LUndoer								Destructor				  [public]
// ---------------------------------------------------------------------------

LUndoer::~LUndoer()
{
	try {
		PostAction(nil);				// Will delete current Action
	}

	catch (...) { }						// Don't allow exceptions to
										//   propagate from destructor
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf												   [protected]
// ---------------------------------------------------------------------------
//	Execute an Undoer Attachment

void
LUndoer::ExecuteSelf(
	MessageT	inMessage,
	void*		ioParam)
{
	switch (inMessage) {

		case msg_PostAction:		// New Action to perform
			PostAction(static_cast<LAction*> (ioParam));
			SetExecuteHost(false);
			break;

		case msg_CommandStatus: {	// Enable and set text for "undo"
									//   menu item
			SCommandStatus *status = static_cast<SCommandStatus*> (ioParam);

			if (status->command == cmd_Undo) {
				FindUndoStatus(status);
				SetExecuteHost(false);
			} else {
				SetExecuteHost(true);
			}
			break;
		}

		case cmd_Undo:				// Undo/Redo the Action
			ToggleAction();
			SetExecuteHost(false);
			break;

		default:
			SetExecuteHost(true);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ PostAction												   [protected]
// ---------------------------------------------------------------------------
//	A new Action has been posted to the host Commander

void
LUndoer::PostAction(
	LAction*	inAction)
{
	ExceptionCode	actionFailure = noErr;

	if ((inAction == nil)  ||
		((inAction != nil) && inAction->IsPostable())) {

										// Save old Action
		LAction	*oldAction = mAction;
		mAction = inAction;

		Boolean	deleteOldAction = false;

		if (oldAction != nil) {			// Finalize the old action

			try {
				oldAction->Finalize();
			}

			catch (...) { }

			deleteOldAction = true;
		}

		if (mAction != nil) {			// Do the new action
			try {
				mAction->Redo();
			}

			catch (const LException& inErr) {

				// Failed to "Do" the newly posted Action. Finalize
				// and delete the new Action, then check to see if the
				// old Action can still be undone/redone. If so, keep
				// the old Action as the last undoable Action.

				actionFailure = inErr.GetErrorCode();

				try {
					mAction->Finalize();
				}
				catch (...) { }

				delete mAction;
				mAction = nil;

				if ( (oldAction != nil) &&
					 (oldAction->CanUndo() || oldAction->CanRedo()) ) {

					mAction = oldAction;
					deleteOldAction = false;
				}
			}
		}

		if (deleteOldAction) {
			delete oldAction;
		}

	} else {							// A non-postable Action
		try {
			inAction->Redo();
		}

		catch (ExceptionCode inErr) {
			actionFailure = inErr;
		}

		catch (const LException& inException) {
			actionFailure = inException.GetErrorCode();
		}

		try {
			inAction->Finalize();
		}

		catch (...) { }

		delete inAction;
	}

	if (actionFailure != noErr) {
		Throw_(actionFailure);
	}
}


// ---------------------------------------------------------------------------
//	¥ ToggleAction												   [protected]
// ---------------------------------------------------------------------------
//	Undo/Redo the Action associated with this Undoer

void
LUndoer::ToggleAction()
{
	if (mAction != nil) {			// Shouldn't be nil, but let's be safe
		if (mAction->CanUndo()) {
			mAction->Undo();
		} else if (mAction->CanRedo()) {
			mAction->Redo();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ FindUndoStatus											   [protected]
// ---------------------------------------------------------------------------
//	Enable/disable and set the text for the "undo" menu item

void
LUndoer::FindUndoStatus(
	SCommandStatus*		ioStatus)
{
	*ioStatus->enabled = false;

	if (mAction != nil) {
		Str255	dummyString;

		if (mAction->CanRedo()) {			// Set "Redo" text
			*ioStatus->enabled = true;
			mAction->GetDescription(ioStatus->name, dummyString);

		} else if (mAction->CanUndo()) {	// Set "Undo" text
			*ioStatus->enabled = true;
			mAction->GetDescription(dummyString, ioStatus->name);
		}
	}

	if (!(*ioStatus->enabled)) {			// Set text to "Can't Undo"
		::GetIndString(ioStatus->name, STRx_UndoEdit, str_CantRedoUndo);
	}
}


PP_End_Namespace_PowerPlant
