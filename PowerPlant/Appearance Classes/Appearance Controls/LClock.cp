// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LClock.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LClock.h>
#include <LControlImp.h>
#include <LStream.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UEnvironment.h>
#include <UKeyFilters.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LClock								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LClock::LClock(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
									// Clock value contains flags
	InitClock((ControlClockFlags) mValue);
}


// ---------------------------------------------------------------------------
//	¥ LClock								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LClock::LClock(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inClockKind,		// ProcID for clock kind
	ResIDT				inTextTraitsID,
	ControlClockFlags	inClockFlags,
	ClassIDT			inImpID)
	
	: LControlPane( inPaneInfo, inImpID, inClockKind, Str_Empty,
					inTextTraitsID, inValueMessage, (SInt32) inClockFlags)
{
	InitClock(inClockFlags);
}


// ---------------------------------------------------------------------------
//	¥ InitClock								Private Initializer		 [private]
// ---------------------------------------------------------------------------

void
LClock::InitClock(
	ControlClockFlags	inClockFlags)
{
									// A display only clock is not editable
	mIsDisplayOnly = ((inClockFlags & kControlClockIsDisplayOnly) != 0);

									// Live clock must be display only
	mIsLive		   = mIsDisplayOnly &&
						((inClockFlags & kControlClockIsLive) != 0);

									// This will select the leftmost part
									//   if we programmatically make the
									//   clock the Target
	mFocusPart	   = kControlFocusNextPart;

	mValue		   = 0;				// Control Manager sets clock value
									//   to zero after creation
									
	Start();						// Live updating (if needed)
}


// ---------------------------------------------------------------------------
//	¥ ~LClock								Destructor				  [public]
// ---------------------------------------------------------------------------

LClock::~LClock()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetLongDate													  [public]
// ---------------------------------------------------------------------------

void
LClock::SetLongDate(
	const LongDateRec&	inLongDate)
{
	SetDataTag(0, kControlClockLongDateTag, sizeof(LongDateRec),
				const_cast<LongDateRec*>(&inLongDate));
}


// ---------------------------------------------------------------------------
//	¥ GetLongDate													  [public]
// ---------------------------------------------------------------------------

void
LClock::GetLongDate(
	LongDateRec		&outLongDate) const
{
	GetDataTag(0, kControlClockLongDateTag, sizeof(LongDateRec),
				&outLongDate);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ClickSelf														  [public]
// ---------------------------------------------------------------------------

void
LClock::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	if (!mIsDisplayOnly) {

		if (!IsTarget()) {				// Click on Clock makes it the Target
			mFocusPart = kControlFocusNoPart;
			SwitchTarget(this);
		}

		if (IsTarget()) {
			LongDateRec		beforeLongDate;	// Get long date currently in clock
			GetLongDate(beforeLongDate);

			LControlPane::ClickSelf(inMouseDown);

			// Get long date after click. We initialize afterLongDate
			// to beforeLongDate since the clock does not set all fields
			// of the LongDateRec. We want these unset fields to have the
			// same value so that we can make a valid determination of
			// whether the clock changed.

			LongDateRec		afterLongDate;
			::BlockMoveData(&beforeLongDate, &afterLongDate,
								sizeof(LongDateRec));
			GetLongDate(afterLongDate);

			if ( !BlocksAreEqual(&afterLongDate, &beforeLongDate,
									sizeof(LongDateRec)) ) {

				UserChangedClock();			// Click changed clock setting
			}

		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress												  [public]
// ---------------------------------------------------------------------------

Boolean
LClock::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	Boolean		keyHandled = true;
	UInt16		theChar    = (UInt16) (inKeyEvent.message & charCodeMask);
	
	if (inKeyEvent.modifiers & cmdKey) {
										// Pass up command keystrokes to
										//   SuperCommander
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);

	} else if (theChar == char_Tab) {	// Move focus within Clock

			// Tab goes to next part. Shift-Tab goes to previous part

		ControlFocusPart	focusPart = kControlFocusNextPart;
		if (inKeyEvent.modifiers & shiftKey) {
			focusPart = kControlFocusPrevPart;
		}

		if (not mControlImp->SetKeyboardFocusPart(focusPart)) {

				// Setting focus failed. That means we were at the
				// start (for Shift-Tab) or end (for Tab) of the
				// clock parts. Pass up keystroke so that an
				// enclosing TabGroup can move to the next/previous
				// item in the group.

			keyHandled = LCommander::HandleKeyPress(inKeyEvent);

			if (!keyHandled) {

					// Tab not handled, so there's no enclosing
					// TabGroup. Reset the focus in the Clock so
					// that it wraps around to the first/last part.

				keyHandled = mControlImp->SetKeyboardFocusPart(focusPart);
			}
		}

	} else {
		LongDateRec		beforeLongDate;	// Get long date currently in clock
		GetLongDate(beforeLongDate);

		mControlImp->DoKeyPress(inKeyEvent);	// Let imp handle keypress

			// Get long date after keypress. We initialize afterLongDate
			// to beforeLongDate since the clock does not set all fields
			// of the LongDateRec. We want these unset fields to have the
			// same value so that we can make a valid determination of
			// whether the clock changed.

		LongDateRec		afterLongDate;
		::BlockMoveData(&beforeLongDate, &afterLongDate, sizeof(LongDateRec));
		GetLongDate(afterLongDate);

		if ( not BlocksAreEqual(&afterLongDate, &beforeLongDate,
								sizeof(LongDateRec)) ) {

			UserChangedClock();			// Keypress changed clock setting

		} else if (not UKeyFilters::IsNumberChar(theChar)) {

				// Keystroke didn't change clock and it's not a number.
				// Pass keystroke up the command chain. We special
				// case number keys since the clock field may already
				// be the number value typed. This won't change the
				// field, but we consider it a keystroke handled by
				// the clock

			keyHandled = LCommander::HandleKeyPress(inKeyEvent);
		}
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------

Boolean
LClock::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case msg_TabSelect:
			if (!IsEnabled() || mIsDisplayOnly) {
				cmdHandled = false;		// Can't tab into disabled or
										//   display only clock
			} else {
										// Set part to select depending if
										//   this is a forward or backward
										//   tab select
				mFocusPart = kControlFocusNextPart;
				if (*(Boolean*) ioParam) {	// *ioParam is true if backward
					mFocusPart = kControlFocusPrevPart;
				}
			}
			break;

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ UserChangedClock												  [public]
// ---------------------------------------------------------------------------

void
LClock::UserChangedClock()
{
	if (mValueMessage != msg_Nothing) {
		BroadcastMessage(mValueMessage, this);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BeTarget													   [protected]
// ---------------------------------------------------------------------------

void
LClock::BeTarget()
{
	mControlImp->BeTarget(mFocusPart);
	mFocusPart = kControlFocusNextPart;		// Reset to default
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget												   [protected]
// ---------------------------------------------------------------------------

void
LClock::DontBeTarget()
{
	if (UEnvironment::IsRunningOSX()) {
	
		// On Mac OS X 10.1, things changed so that the focus
		// hiliting does not automatically erase. So, we force
		// a redraw on X (any version). This won't cause a flash
		// since windows on X are buffered.
		
		Refresh();
	}

	mControlImp->DontBeTarget();
}


// ---------------------------------------------------------------------------
//	¥ HideSelf													   [protected]
// ---------------------------------------------------------------------------

void
LClock::HideSelf()
{
	if (IsOnDuty()) {				// Hidden Clock can't be the Target
		SwitchTarget(GetSuperCommander());
	}
	
	LControlPane::HideSelf();
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LClock::DisableSelf()
{
	if (IsOnDuty()) {				// Disabled Clock can't be the Target
		SwitchTarget(GetSuperCommander());
	}
	
	LControlPane::DisableSelf();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Start															  [public]
// ---------------------------------------------------------------------------

void
LClock::Start()
{
		// We use an interval of 0.5 seconds even though the clock
		// does not display fractional seconds. With an interval of
		// 1.0 seconds, we may get into a situation where the system
		// clock changes to the next second right after we update
		// our clock display. That would mean our display would be off
		// by almost a second. With an interval of 0.5, our display
		// won't be off by more than 0.5 seconds.
		
	if (mIsLive) {
		StartIdling(0.5);
	}
}


// ---------------------------------------------------------------------------
//	¥ Stop															  [public]
// ---------------------------------------------------------------------------

void
LClock::Stop()
{
	if (mIsLive) {
		StopIdling();
	}
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------

void
LClock::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	mControlImp->Idle();				// Live clock needs idle time
}


PP_End_Namespace_PowerPlant
