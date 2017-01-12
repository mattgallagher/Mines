// ===========================================================================
//	UModalDialogs.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utilities for handling (moveable) modal dialog boxes


#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UModalDialogs.h>

#include <LControl.h>
#include <LEditField.h>
#include <LPeriodical.h>
#include <LUndoer.h>
#include <LWindow.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UEventMgr.h>
#include <UReanimator.h>

#include <ControlDefinitions.h>


PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ StDialogHandler										 StDialogHandler ¥
// ===========================================================================
//
//	Manages events while a dialog box is active
//
//	Typical use is for creating a "one shot" moveable modal dialog box
//	where you want the dialog box to handle all user interaction until
//	the dialog box closes.

// ---------------------------------------------------------------------------
//	¥ StDialogHandler						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Create new Dialog window from a PPob

StDialogHandler::StDialogHandler(
	ResIDT			inDialogResID,		// 'PPob' and 'RidL' Resource ID
	LCommander*		inSuper)

	: LCommander(inSuper)
{
	mDialog = LWindow::CreateWindow(inDialogResID, this);

	UReanimator::LinkListenerToBroadcasters(this, mDialog, inDialogResID);

	InitDialogHandler();
}


// ---------------------------------------------------------------------------
//	¥ StDialogHandler						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Take ownership of a Window to use as a Dialog box
//
//	If you wish to set up a Broadcaster/Listener relationship between
//	the Controls in the Window (such as the OK and Cancel buttons) and the
//	DialogHandler, you must do so explicitly.

StDialogHandler::StDialogHandler(
	LWindow*		inWindow,			// Window to use for dialog
	LCommander*		inSuper)

	: LCommander(inSuper)
{
	mDialog = inWindow;
	inWindow->SetSuperCommander(this);

	InitDialogHandler();
}


// ---------------------------------------------------------------------------
//	¥ InitDialogHandler						Initializer			   [protected]
// ---------------------------------------------------------------------------

void
StDialogHandler::InitDialogHandler()
{
		// Create and add an Undoer. It's OK if this fails. We
		// just go on without undo support.

	LUndoer*	theUndoer = nil;

	try {
		theUndoer = new LUndoer;
		mDialog->AddAttachment(theUndoer);
	}

	catch (...) {
		delete theUndoer;
	}

#if PP_Uses_Carbon_Events
	mFirstTime = true;
#endif

	mMessage   = msg_Nothing;
	mSleepTime = 6;
}


// ---------------------------------------------------------------------------
//	¥ ~StDialogHandler						Destructor				 [private]
// ---------------------------------------------------------------------------

StDialogHandler::~StDialogHandler()
{
	delete mDialog;
}


// ---------------------------------------------------------------------------
//	¥ DoDialog														  [public]
// ---------------------------------------------------------------------------
//	Handle an Event for a dialog box
//
//	Call this function repeatedly to handle events. If the event triggers
//	a Broadcaster to broadcast a message, the last such message heard by
//	the DialogHandler is returned. Otherwise, this function returns
//	msg_Nothing.

MessageT
StDialogHandler::DoDialog()
{
	EventRecord macEvent;

	if (IsOnDuty()) {
		UEventMgr::GetMouseAndModifiers(macEvent);
		AdjustCursor(macEvent);
	}

#if PP_Uses_Carbon_Events
	if (mFirstTime) {
		UpdateMenus();
		mFirstTime = false;
	}
#endif

	SetUpdateCommandStatus(false);
	mMessage = msg_Nothing;

	Boolean gotEvent = ::WaitNextEvent(everyEvent, &macEvent,
										mSleepTime, mMouseRgn);

		// Let Attachments process the event. Continue with normal
		// event dispatching unless suppressed by an Attachment.

	if (LEventDispatcher::ExecuteAttachments(msg_Event, &macEvent)) {
		if (gotEvent) {
			DispatchEvent(macEvent);
		} else {
			UseIdleTime(macEvent);
		}
	}

									// Repeaters get time after every event
	LPeriodical::DevoteTimeToRepeaters(macEvent);

									// Update status of menu items
	if (IsOnDuty() && GetUpdateCommandStatus()) {
		UpdateMenus();
	}

	return mMessage;
}


// ---------------------------------------------------------------------------
//	¥ AllowSubRemoval												  [public]
// ---------------------------------------------------------------------------

Boolean
StDialogHandler::AllowSubRemoval(
	LCommander*		inSub)
{
	Boolean allow = true;

	if (inSub == (dynamic_cast<LCommander*>(mDialog))) {
									// Dialog is trying to close itself
		mDialog->Hide();			// Just hide it now
		allow = false;				// And don't let it delete itself
		mMessage = msg_Cancel;		// Our destructor will delete it
	}

	return(allow);
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
StDialogHandler::FindCommandStatus(
	CommandT		inCommand,
	Boolean&		outEnabled,
	Boolean&		/* outUsesMark */,
	UInt16&			/* outMark */,
	Str255			/* outName */)
{
		// Don't enable any commands except the synthetic one for the
		// Apple Menu as a whole. This function purposely does not
		// call the inherited FindCommandStatus, thereby suppressing
		// commands that are handled by SuperCommanders. Only those
		// commands enabled by SubCommanders will be active.
		//
		// This is usually what you want for a moveable modal dialog.
		// Commands such as "New", "Open" and "Quit" that are handled
		// by the Applcation are disabled, but items within the dialog
		// can enable commands. For example, an EditField would enable
		// items in the "Edit" menu.

	outEnabled = (inCommand == SyntheticCommandForMenu_(MENU_Apple));
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
StDialogHandler::ListenToMessage(
	MessageT	inMessage,
	void*		/* ioParam */)
{
	mMessage = inMessage;		// Store message. DoDialog() will return
								//   this value.
}


#pragma mark -

// ===========================================================================
// ¥ StTimedDialogHandler								StTimedDialogHandler ¥
// ===========================================================================
//	A dialog handler that will simulate a click upon a specified control
//	after a given number of seconds elapse. Note that time is specified in
//	seconds and NOT ticks.
//
//	If the timeout is set to zero, the dialog will never timeout.
//
//	The countdown is not started (nor stopped) automatically by object
//	construction. You must explicitly call StartCountdown() to start the
//	timer and StopCountdown() to stop the timer.


// ---------------------------------------------------------------------------
//	¥ StTimedDialogHandler			Constructor						  [public]
// ---------------------------------------------------------------------------
//	Create a new dialog from a PPob

StTimedDialogHandler::StTimedDialogHandler(
	ResIDT			inDialogResID,
	LCommander*		inSuper,
	PaneIDT			inControlToClick,
	UInt32			inSecondsToWait)

	: StDialogHandler(inDialogResID, inSuper)
{
	mSecondsToWait	= inSecondsToWait;
	mControlToClick	= inControlToClick;
	mTargetTime		= 0;
	mCountingDown	= false;
}


// ---------------------------------------------------------------------------
//	¥ StTimedDialogHandler			Constructor						  [public]
// ---------------------------------------------------------------------------
//	Take ownership of a Window to use as a Dialog box
//
//	If you wish to set up a Broadcaster/Listener relationship between
//	the Controls in the Window (such as the OK and Cancel buttons) and the
//	DialogHandler, you must do so explicitly.

StTimedDialogHandler::StTimedDialogHandler(
	LWindow*		inWindow,
	LCommander*		inSuper,
	PaneIDT			inControlToClick,
	UInt32			inSecondsToWait)

	: StDialogHandler(inWindow, inSuper)
{
	mSecondsToWait	= inSecondsToWait;
	mControlToClick	= inControlToClick;
	mTargetTime		= 0;
	mCountingDown	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~StTimedDialogHandler					Destructor				 [private]
// ---------------------------------------------------------------------------

StTimedDialogHandler::~StTimedDialogHandler()
{
}


// ---------------------------------------------------------------------------
//	¥ SetTimeout													[public]
// ---------------------------------------------------------------------------
//	Set the number of seconds that must elapse before the control is
//	clicked. Will optionally restart the countdown if there is one
//	already in progress.

void
StTimedDialogHandler::SetTimeout(
	UInt32		inSecondsToWait,
	bool		inRestartCountdown)
{
	mSecondsToWait = inSecondsToWait;

	if (inRestartCountdown && IsCountingDown()) {
		StartCountdown();
	}
}


// ---------------------------------------------------------------------------
//	¥ StartCountdown												[public]
// ---------------------------------------------------------------------------
//	Initiates the countdown. Must be explicitly called to start the timer.

void
StTimedDialogHandler::StartCountdown()
{
	mTargetTime   = ::TickCount() + (mSecondsToWait * 60);
	mCountingDown = true;
	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ StopCountdown													[public]
// ---------------------------------------------------------------------------
//	Stops the countdown. Must be explicitly called to stop the timer.

void
StTimedDialogHandler::StopCountdown()
{
	StopRepeating();
	mCountingDown = false;
	mTargetTime   = 0;
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------
//	Performs the countdown. Upon reaching zero, simulates a click in
//	a specified control.

void
StTimedDialogHandler::SpendTime(
	const EventRecord&	/*inMacEvent*/)
{
	if ( (mSecondsToWait != 0) && (::TickCount() >= mTargetTime) ) {

		// Obtain the control and simulate a click upon it

		Assert_(GetDialog());	// If this fails you have bigger problems

		LControl *theControl = dynamic_cast<LControl*>
								(GetDialog()->FindPaneByID(GetControlToClick()));
		if (theControl) {
			theControl->SimulateHotSpotClick(kControlButtonPart);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetRemainingSeconds											[public]
// ---------------------------------------------------------------------------
//	Returns the number of seconds remaining.
//
//	Return value of zero means there is no countdown, and the simulated
//	control click will never occur.

UInt32
StTimedDialogHandler::GetRemainingSeconds() const
{
	UInt32 secRemain = 0;
	if (mSecondsToWait != 0) {
		if (mTargetTime >= ::TickCount()) {
			secRemain = ((mTargetTime - ::TickCount()) / 60L) + 1;
		}
	}

	return secRemain;
}

#pragma mark -

// ===========================================================================
// ¥ UModalDialogs											   UModalDialogs ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ AskForOneNumber										 [static] [public]
// ---------------------------------------------------------------------------
//	Present a Moveable Modal dialog for entering a single number
//
//	Returns TRUE if entry is OK'd.
//	On entry, ioNumber is the current value to display for the number
//	On exit, ioNumber is the new value if OK'd, unchanged if Canceled

bool
UModalDialogs::AskForOneNumber(
	LCommander*		inSuper,
	ResIDT			inDialogID,
	PaneIDT			inEditFieldID,
	SInt32&			ioNumber)
{
	StDialogHandler	theHandler(inDialogID, inSuper);
	LWindow			*theDialog = theHandler.GetDialog();

	LEditField *theField = dynamic_cast<LEditField*>
								(theDialog->FindPaneByID(inEditFieldID));

	if (theField == nil) {
		SignalStringLiteral_("No EditField with specified ID");
		return false;
	}

	theField->SetValue(ioNumber);
	theField->SelectAll();
	theDialog->SetLatentSub(theField);
	theDialog->Show();

	bool		entryOK = false;

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
//	¥ AskForOneString										 [static] [public]
// ---------------------------------------------------------------------------
//	Present a Moveable Modal dialog for entering a single string
//
//	Returns TRUE if entry is OK'd.
//	On entry, ioString is the current text to display for the number
//	On exit, ioString is the new text if OK'd, unchanged if Canceled

bool
UModalDialogs::AskForOneString(
	LCommander*		inSuper,
	ResIDT			inDialogID,
	PaneIDT			inEditFieldID,
	Str255			ioString)
{
	StDialogHandler	theHandler(inDialogID, inSuper);
	LWindow*		theDialog = theHandler.GetDialog();

	LEditField*		theField = dynamic_cast<LEditField*>
								(theDialog->FindPaneByID(inEditFieldID));

	if (theField == nil) {
		SignalStringLiteral_("No EditField with specified ID");
		return false;
	}

	theField->SetDescriptor(ioString);
	theField->SelectAll();
	theDialog->SetLatentSub(theField);
	theDialog->Show();

	bool		entryOK = false;

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

#pragma mark -

// ===========================================================================
// ¥ UModalAlerts												UModalAlerts ¥
// ===========================================================================
//
//	PP wrappers for the four kinds of modal alerts (plain, stop, note,
//	and caution). These functions properly deactivate floating windows
//	and process events while the alert is up.
//
//	This class has a default event filter function. You can install your
//	own by calling UModalAlerts::SetModalFilterProc().

ModalFilterProcPtr		UModalAlerts::sModalEventFilter   = nil;

#if PP_Target_Classic
ModalFilterYDProcPtr	UModalAlerts::sStdFileEventFilter = nil;
#endif

// ---------------------------------------------------------------------------
//	¥ Alert													 [static] [public]
// ---------------------------------------------------------------------------

DialogItemIndex
UModalAlerts::Alert(
	ResIDT		inALRTid)
{
	UDesktop::Deactivate();

	ModalFilterUPP	filterUPP = NewModalFilterUPP(GetModalEventFilter());

	DialogItemIndex	itemHit = ::Alert(inALRTid, filterUPP);

	DisposeModalFilterUPP(filterUPP);

	UDesktop::Activate();

	return itemHit;
}


// ---------------------------------------------------------------------------
//	¥ StopAlert												 [static] [public]
// ---------------------------------------------------------------------------

DialogItemIndex
UModalAlerts::StopAlert(
	ResIDT		inALRTid)
{
	UDesktop::Deactivate();

	ModalFilterUPP	filterUPP = NewModalFilterUPP(GetModalEventFilter());

	DialogItemIndex	itemHit = ::StopAlert(inALRTid, filterUPP);

	DisposeModalFilterUPP(filterUPP);

	UDesktop::Activate();

	return itemHit;
}


// ---------------------------------------------------------------------------
//	¥ NoteAlert												 [static] [public]
// ---------------------------------------------------------------------------

DialogItemIndex
UModalAlerts::NoteAlert(
	ResIDT		inALRTid)
{
	UDesktop::Deactivate();

	ModalFilterUPP	filterUPP = NewModalFilterUPP(GetModalEventFilter());

	DialogItemIndex	itemHit = ::NoteAlert(inALRTid, filterUPP);

	DisposeModalFilterUPP(filterUPP);

	UDesktop::Activate();

	return itemHit;
}


// ---------------------------------------------------------------------------
//	¥ CautionAlert											 [static] [public]
// ---------------------------------------------------------------------------

DialogItemIndex
UModalAlerts::CautionAlert(
	ResIDT		inALRTid)
{
	UDesktop::Deactivate();

	ModalFilterUPP	filterUPP = NewModalFilterUPP(GetModalEventFilter());

	DialogItemIndex	itemHit = ::CautionAlert(inALRTid, filterUPP);

	DisposeModalFilterUPP(filterUPP);

	UDesktop::Activate();

	return itemHit;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetModalEventFilter									 [static] [public]
// ---------------------------------------------------------------------------
//	Return pointer to function used for filtering events while a modal
//	dialog is up

ModalFilterProcPtr
UModalAlerts::GetModalEventFilter()
{
	ModalFilterProcPtr	theProc = sModalEventFilter;

	if (theProc == nil) {				// Use our default filter
		theProc = (ModalFilterProcPtr)
							UModalAlerts::EventFilter_ModalDialog;
	}

	return theProc;
}


// ---------------------------------------------------------------------------
//	¥ EventFilter_ModalDialog								 [static] [public]
// ---------------------------------------------------------------------------
//	Toolbox callback function for filtering events while a modal dialog is up
//
//	This function does the following:
//
//		- Passes event to PP's normal event dispatcher
//		- Calls the standard filter which handles mapping Return/Enter
//			keystrokes to the default button
//		- Maps Escape and Command-Period key presses to the cancel button

pascal Boolean
UModalAlerts::EventFilter_ModalDialog(
	DialogPtr			inDialog,
	EventRecord*		inMacEvent,
	DialogItemIndex*	outItemHit)
{
	Boolean		handled = false;

	try {
		ProcessModalEvent(*inMacEvent);		// Do normal PP event processing
	}

	catch (...) { }

											// Call Toolbox's standard filter
	handled = ::StdFilterProc(inDialog, inMacEvent, outItemHit);

			// If standard filter didn't handle the event, we
			// interpret a press of the escape key or command-period
			// as an equivalent to clicking the cancel button

	if ( !handled  &&  (inMacEvent->what == keyDown) ) {

		if ( UKeyFilters::IsEscapeKey(*inMacEvent)  ||
			 UKeyFilters::IsCmdPeriod(*inMacEvent) ) {

			 if (SimulateDialogButtonClick(inDialog, kStdCancelItemIndex)) {

			 		// Tell Dialog Manager that this event triggered
			 		// the Cancel button

				*outItemHit = kAlertStdAlertCancelButton;
				handled	   = true;
			}
		}
	}

	return handled;
}

#pragma mark -

#if PP_Target_Classic					// Standard File is only on Classic

// ---------------------------------------------------------------------------
//	¥ GetStdFileEventFilter									 [static] [public]
// ---------------------------------------------------------------------------
//	Return pointer to function used for filtering events while a Standard
//	File dialog is up

ModalFilterYDProcPtr
UModalAlerts::GetStdFileEventFilter()
{
	ModalFilterYDProcPtr	theProc = sStdFileEventFilter;

	if (theProc == nil) {				// Use our default filter
		theProc = (ModalFilterYDProcPtr)
							UModalAlerts::EventFilter_StdFile;
	}

	return theProc;
}


// ---------------------------------------------------------------------------
//	¥ EventFilter_StdFile									 [static] [public]
// ---------------------------------------------------------------------------
//	Toolbox callback function for filtering events while a Standard File
//	dialog is up
//
//	This function passes the event to PP's normal event dispatcher

pascal Boolean
UModalAlerts::EventFilter_StdFile(
	DialogPtr			/* inDialog */,
	EventRecord*		inMacEvent,
	DialogItemIndex*	/* outItemHit */,
	void*				/* inUserData */)
{
	try {
		ProcessModalEvent(*inMacEvent);		// Do normal PP event processing
	}

	catch (...) { }

	return false;
}

#endif // PP_Target_Classic

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SimulateDialogButtonClick								 [static] [public]
// ---------------------------------------------------------------------------
//	Simulate a click in a button of a dialog box by temporarily hiliting it
//
//	Returns false if the specified item isn't a button.

bool
UModalAlerts::SimulateDialogButtonClick(
	DialogPtr			inDialog,
	DialogItemIndex		inItemIndex)
{
	bool			itemIsAButton = false;

									// Get info about specified item

	DialogItemType	itemType = kUserDialogItem;	// Anything but button
	Handle			itemHandle;
	Rect			itemBox;

	::GetDialogItem(inDialog, inItemIndex, &itemType, &itemHandle, &itemBox);

	if (itemType == kButtonDialogItem) {
									// Item is a Button. Invert it for enough
									// time for the user to notice.
		::HiliteControl((ControlHandle) itemHandle, kControlButtonPart);

		UInt32	ticks;
		::Delay(delay_Feedback, &ticks);

		::HiliteControl((ControlHandle) itemHandle, kControlNoPart);

		itemIsAButton = true;
	}

	return itemIsAButton;
}


// ---------------------------------------------------------------------------
//	¥ ProcessModalEvent										 [static] [public]
// ---------------------------------------------------------------------------
//	Dispatches events which occur while an alert is up.
//
//	You can call this function from a Toolbox event filter callback function.

void
UModalAlerts::ProcessModalEvent(
	EventRecord&		ioMacEvent)
{
	StGrafPortSaver		savePort;	// Save/restore current Port
	LView::OutOfFocus(nil);			// We can't be sure of the focus

	LEventDispatcher*	theDispatcher =
							LEventDispatcher::GetCurrentEventDispatcher();

	SignalIf_(theDispatcher == nil);

	if (theDispatcher->ExecuteAttachments(msg_Event, &ioMacEvent)) {

		switch (ioMacEvent.what) {

				case mouseDown:
				case mouseUp:
				case keyDown:
				case keyUp:
				case autoKey:

						// We ignore mouse and key events. Caller must
						// handle those events if they apply to the
						// modal window.

					break;

				default:

						// Dispatch all other events in the normal way
						// This includes:
						//		Null Events
						//		Updates
						//		Activate/Deactivate
						//		OS Events (Suspend/Resume/Mouse Moved)
						//		High Level Events/Apple Events

					theDispatcher->DispatchEvent(ioMacEvent);
					break;
		}
	}

									// Repeaters get time after every event
	LPeriodical::DevoteTimeToRepeaters(ioMacEvent);

	LView::OutOfFocus(nil);			// Current Port might change
}


PP_End_Namespace_PowerPlant
