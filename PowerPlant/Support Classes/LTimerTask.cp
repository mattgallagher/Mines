// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTimerTask.cp				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//	Wrapper class for an Event Loop Timer

#include <LTimerTask.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variable

EventLoopTimerUPP	LTimerTask::sTimerUPP;


// ---------------------------------------------------------------------------
//	¥ LTimerTask							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTimerTask::LTimerTask()
{
	mTimerRef = nil;
	mUserData = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LTimerTask							Destructor				  [public]
// ---------------------------------------------------------------------------

LTimerTask::~LTimerTask()
{
	Remove();				// Remove timer from System event loop
}


// ---------------------------------------------------------------------------
//	¥ Install														  [public]
// ---------------------------------------------------------------------------
//	Install timer onto a System event loop
//
//		inEventLoop - System event loop. Usually you will pass in the return
//						value from ::GetMainEventLoop() or
//						::GetCurrentEventLoop().
//
//		inFireDelay - Seconds before first firing of the timer
//
//		inInterval - Seconds between successive firings of the timer.
//						If zero, the timer fires once, but is still
//						installed. To fire it again, you can call
//						SetNextFireTime().
//
//		inUserData - Pointer to user-defined data
//
//	This function removes the timer if it is already installed, then
//	re-installs it. Thus, you may call Install() more than once. You
//	will need to do this in order to change the Event Loop or the
//	Interval.

OSStatus
LTimerTask::Install(
	EventLoopRef		inEventLoop,
	EventTimerInterval	inFireDelay,
	EventTimerInterval	inInterval)
{
									// Allocate static UPP on first call
	if (sTimerUPP == nil) {
		sTimerUPP = ::NewEventLoopTimerUPP(&TimerCallback);
	}
	
	if (mTimerRef != nil) {			// Already installed. Remove it.
		Remove();
	}

	return ::InstallEventLoopTimer( inEventLoop,
									inFireDelay,
									inInterval,
									sTimerUPP,
									this,
									&mTimerRef );
}


// ---------------------------------------------------------------------------
//	¥ Remove														  [public]
// ---------------------------------------------------------------------------
//	Remove timer from its event loop
//
//	You can re-install the timer later by calling Install().

void
LTimerTask::Remove()
{
	if (mTimerRef != nil) {
		::RemoveEventLoopTimer(mTimerRef);
		mTimerRef = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ IsInstalled													  [public]
// ---------------------------------------------------------------------------
//	Return whether the TimerTask is installed onto an event loop

bool
LTimerTask::IsInstalled() const
{
	return (mTimerRef != nil);		// Timer is installed if Ref exists
}


// ---------------------------------------------------------------------------
//	¥ SetNextFireTime												  [public]
// ---------------------------------------------------------------------------
//	Specify the next firing time, in seconds
//
//	Firing time temporarily overrides the interval until the next firing.
//	For example, if you installed the timer with a 1 second interval, and
//	then call SetNextFireTime(5), the timer will cease firing for 5 seconds,
//	fire, and then fire again at 1 second intervals.

OSStatus
LTimerTask::SetNextFireTime(
	EventTimerInterval	inNextFire)
{
	return ::SetEventLoopTimerNextFireTime(mTimerRef, inNextFire);
}


// ---------------------------------------------------------------------------
//	¥ SetUserData													  [public]
// ---------------------------------------------------------------------------

void
LTimerTask::SetUserData(
	void*	inUserData)
{
	mUserData = inUserData;
}


// ---------------------------------------------------------------------------
//	¥ GetUserData													  [public]
// ---------------------------------------------------------------------------

void*
LTimerTask::GetUserData() const
{
	return mUserData;
}


// ---------------------------------------------------------------------------
//	¥ DoTask										   [pure virtual] [public]
// ---------------------------------------------------------------------------
//	Subclasses must override to implement the action performed by the timer

#pragma mark LTimerTask::DoTask

// void
// LTimerTask::DoTask()


// ---------------------------------------------------------------------------
//	¥ TimerCallback											[static] [private]
// ---------------------------------------------------------------------------
//	Toolbox callback function for an event loop timer
//
//	This is a generic function used for all our timers. We store a pointer
//	to a LTimerTask object in the user data. This function extracts that
//	pointer, then calls the virtual DoTask() function.

pascal
void
LTimerTask::TimerCallback(
	EventLoopTimerRef	/* inTimerRef */,
	void*				inUserData)
{
	LTimerTask*	task = static_cast<LTimerTask*>(inUserData);
	
	try {
		task->DoTask();
	}
	
	catch (...) { }				// Can't throw out of a Toolbox callback
}


PP_End_Namespace_PowerPlant

