// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPeriodical.cp				PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	An abstract building block class for objects that want to receive a
//	function call at regular intervals
//
//	ее Queues
//	This class maintains two static queues: Idler and Repeater. You
//	add/remove Periodicals from these queues using StartIdling/StopIdling
//	and StartRepeating/StopRepeating.
//
//	Calling DevoteTimeToIdlers/DevoteTimeToRepeaters calls the SpendTime
//	function for each Periodical in the respective Queue.
//
//	It is up to the caller (Client) to determine the meaning of each
//	Queue and when to devote time to the Periodicals in them. The
//	PowerPlant LApplication class devotes time to Idlers at Null Event
//	time, and devotes time to Repeaters after every event.
//
//	ее Timers
//	On Carbon, you may wish to use event loop timers instead of the
//	Idler and Repeater queues. Call StartIdling/StartRepeating with
//	an EventTimeInterval parameter (a number of type 'double') to use
//	an event loop timer (if available).
//
//	ее Derived Classes
//	Classes derived from LPeriodical must define a SpendTime function, as
//	this is a pure virtual function in LPeriodical.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPeriodical.h>
#include <TArrayIterator.h>
#include <UDrawingState.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

TArray<LPeriodical*>*	LPeriodical::sIdlerQ = nil;
TArray<LPeriodical*>*	LPeriodical::sRepeaterQ = nil;


// ---------------------------------------------------------------------------
//	е LPeriodical							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPeriodical::LPeriodical()
{
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		mTimerTask = nil;
	#endif
}


// ---------------------------------------------------------------------------
//	е ~LPeriodical							Destructor				  [public]
// ---------------------------------------------------------------------------

LPeriodical::~LPeriodical()
{
	StopIdling();
	StopRepeating();
	
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		delete mTimerTask;
	#endif
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е StartIdling													  [public]
// ---------------------------------------------------------------------------
//	Put Periodical in the Idler queue

void
LPeriodical::StartIdling()
{
	if (sIdlerQ == nil) {			// Create queue if it doesn't exist
		sIdlerQ = new TArray<LPeriodical*>;
	}
									// Add to end of Idler queue if it isn't
									//   already in there
	if (sIdlerQ->FetchIndexOf(this) == LArray::index_Bad) {
		sIdlerQ->AddItem(this);
	}
}


// ---------------------------------------------------------------------------
//	е StopIdling													  [public]
// ---------------------------------------------------------------------------
//	Remove Periodical from the Idler queue

void
LPeriodical::StopIdling()
{
	if (sIdlerQ != nil) {
		sIdlerQ->Remove(this);
	}
	
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		RemoveTimer();
	#endif
}


// ---------------------------------------------------------------------------
//	е IsIdling														  [public]
// ---------------------------------------------------------------------------
//	Return whether a Periodical is in the Idler queue or, on Carbon, if
//	a timer is installed

bool
LPeriodical::IsIdling() const
{
	bool	idling = (sIdlerQ != nil)  &&
					 sIdlerQ->ContainsItem(const_cast<LPeriodical*>(this));
					 
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		idling |= (mTimerTask != nil) && mTimerTask->IsInstalled();
	#endif

	return idling;
}


// ---------------------------------------------------------------------------
//	е DevoteTimeToIdlers									 [static] [public]
// ---------------------------------------------------------------------------
//	Call SpendTime function for each Periodical in the Idler queue

void
LPeriodical::DevoteTimeToIdlers(
	const EventRecord&	inMacEvent)
{
	if (sIdlerQ != nil) {
		TArrayIterator<LPeriodical*> iterator(*sIdlerQ);
		LPeriodical	*theIdler;
		while (iterator.Next(theIdler)) {
			theIdler->SpendTime(inMacEvent);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е StartRepeating												  [public]
// ---------------------------------------------------------------------------
//	Put Periodical in the Repeater queue

void
LPeriodical::StartRepeating()
{
	if (sRepeaterQ == nil) {		// Create queue if it doesn't exist
		sRepeaterQ = new TArray<LPeriodical*>;
	}
									// Add to end of Repeater queue if it
									//   isn't already in there
	if (sRepeaterQ->FetchIndexOf(this) == LArray::index_Bad) {
		sRepeaterQ->AddItem(this);
	}
}


// ---------------------------------------------------------------------------
//	е StopRepeating													  [public]
// ---------------------------------------------------------------------------
//	Remove Periodical from the Repeater queue

void
LPeriodical::StopRepeating()
{
	if (sRepeaterQ != nil) {
		sRepeaterQ->Remove(this);
	}
	
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		RemoveTimer();
	#endif
}


// ---------------------------------------------------------------------------
//	е IsRepeating													  [public]
// ---------------------------------------------------------------------------
//	Return whether a Periodical is in the Repeater queue or, on Carbon, if
//	a timer is installed

bool
LPeriodical::IsRepeating() const
{
	bool	repeating = (sRepeaterQ != nil)  &&
						sRepeaterQ->ContainsItem(const_cast<LPeriodical*>(this));
					 
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
		repeating |= (mTimerTask != nil) && mTimerTask->IsInstalled();
	#endif

	return repeating;
}


// ---------------------------------------------------------------------------
//	е DevoteTimeToRepeaters									 [static] [public]
// ---------------------------------------------------------------------------
//	Call SpendTime function for each Periodical in the Repeater queue

void
LPeriodical::DevoteTimeToRepeaters(
	const EventRecord&	inMacEvent)
{
	if (sRepeaterQ != nil) {
		TArrayIterator<LPeriodical*> iterator(*sRepeaterQ);
		LPeriodical	*theRepeater;
		while (iterator.Next(theRepeater)) {
			theRepeater->SpendTime(inMacEvent);
		}
	}
}

#pragma mark -

// ===========================================================================
//	Carbon Event Loop Timer Support
// ===========================================================================
//
//	On Carbon, the OS will call installed Timers at specified intervals.
//
//	This is different from the behavior of a traditional Periodical, which
//	gets called from the event loop and whose frequency depends on how
//	fast events are being received. If your Periocical just wants to get
//	called at regular intervals and is not performing post-event processing,
//	consider using the Timer variations.
//
//	To update existing code to use Timers, just change the calls to
//	StartIdling() or StartRepeating() to have an EventTimeInterval
//	parameter (which is a number of type 'double'). The new overloads
//	of StartIdling() and StartRepeating() will work for all targets.
//	They will call through to the original functions if event timers
//	are not available.
//
//	If your program requires Carbon 1.1 or later, consider using LTimerTask
//	directly instead of LPeriodical. Instead of inheriting from LPeridical,
//	use a LTimerTask (or a subclass) as a member variable.
//
//	IMPORTANT NOTE: If you use a Timer, you need to be aware that the
//	SpendTime() function will be called from a system callback routine
//	rather than from the PP event loop. In particular, you must save and
//	restore the drawing state.
//
//	For example, Timers are called during mouse down tracking in menus
//	and controls. The system will not appreciate your changing the Port
//	or clipping region. You can use the StColorPortState class to
//	save/restore common state information.

// ---------------------------------------------------------------------------
//	е StartIdling													  [public]
// ---------------------------------------------------------------------------
//	Begin idling of a Periodical
//
//	If event timers are available (Carbon 1.1 or later), install a TimerTask
//	so that the Periodical gets time (i.e., the SpendTime() function is
//	called) at the specified interval (in seconds).
//
//	Otherwise, use the traditional mechanism of adding the Periodical to
//	the Idle Queue.

void
LPeriodical::StartIdling(
	EventTimerInterval	inInterval)
{
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
									// Timers require Carbon 1.1 so we
									//   must check for them
		if (CFM_AddressIsResolved_(InstallEventLoopTimer)) {
			InstallTimer(inInterval);
		
		} else {
			StartIdling();
		}
		
	#else
		#pragma unused(inInterval)
		
		StartIdling();
		
	#endif
}


// ---------------------------------------------------------------------------
//	е StartRepeating												  [public]
// ---------------------------------------------------------------------------
//	Begin repeating of a Periodical
//
//	If event timers are available (Carbon 1.1 or later), install a TimerTask
//	so that the Periodical gets time (i.e., the SpendTime() function is
//	called) at the specified interval (in seconds).
//
//	Otherwise, use the traditional mechanism of adding the Periodical to
//	the Repeater Queue.

void
LPeriodical::StartRepeating(
	EventTimerInterval	inInterval)
{
	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
									// Timers require Carbon 1.1 so we
									//   must check for them
		if (CFM_AddressIsResolved_(InstallEventLoopTimer)) {
			InstallTimer(inInterval);
		
		} else {
			StartRepeating();
		}
		
	#else
		#pragma unused(inInterval)
		
		StartRepeating();
		
	#endif
}

#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON

// ---------------------------------------------------------------------------
//	е InstallTimer												   [protected]
// ---------------------------------------------------------------------------
//	Install a TimerTask onto the current event loop at the specified interval
//
//	The initial firing delay is the same as the interval
//
//	Call this function only if event loop timers exist (Carbon 1.1 or later)

void
LPeriodical::InstallTimer(
	EventTimerInterval	inInterval)
{
	if (mTimerTask == nil) {
		mTimerTask = new TTimerTask<LPeriodical>(this, &LPeriodical::DoTimerTask);
	}

	mTimerTask->Install( ::GetCurrentEventLoop(), inInterval, inInterval );
}


// ---------------------------------------------------------------------------
//	е RemoveTimer												   [protected]
// ---------------------------------------------------------------------------

void
LPeriodical::RemoveTimer()
{
	if (mTimerTask != nil) {
		mTimerTask->Remove();
	}
}


// ---------------------------------------------------------------------------
//	е DoTimerTask												   [protected]
// ---------------------------------------------------------------------------
//	Callback function for a TimerTask

void
LPeriodical::DoTimerTask(
	LTimerTask*	/* inTask */)
{
									// Fill in an EventRecord with the
									//   current state to pass to
									//   SpendTime()
	EventRecord		event;
	
	event.what		= nullEvent;
	event.message	= 0;
	event.when		= ::TickCount();
	event.modifiers = (UInt16) ::GetCurrentKeyModifiers();
	
	::GetGlobalMouse(&event.where);
	
	SpendTime(event);
}

#endif // PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON

#pragma mark -

// ---------------------------------------------------------------------------
//	е DeleteIdlerAndRepeaterQueues							 [static] [public]
// ---------------------------------------------------------------------------
//	Delete the internal queues (Arrays) used to store pointers to Idler
//	and Repeater objects. This does NOT delete the Idler and Repeater
//	objects themselves.
//
//	Normally, you will only use this routine to clean up memory when
//	terminating a code resource or fragment.
//
//	You don't need to call this when quitting an application, since the queues
//	will disappear when the System deallocates the app's heap. However, you
//	may want to call this if you are fastidious about always deleting every
//	object that is created via new.

void
LPeriodical::DeleteIdlerAndRepeaterQueues()
{
	delete sIdlerQ;
	sIdlerQ = nil;

	delete sRepeaterQ;
	sRepeaterQ = nil;
}


PP_End_Namespace_PowerPlant
