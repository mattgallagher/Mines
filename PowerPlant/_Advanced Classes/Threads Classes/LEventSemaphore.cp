// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventSemaphore.cp			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LEventSemaphore.h>
#include <LThread.h>
#include <PP_Constants.h>
#include <UException.h>
#include <UThread.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LEventSemaphore()
// ---------------------------------------------------------------------------
//	Default constructor.  The semaphore is created in the unposted state.

LEventSemaphore::LEventSemaphore()
	: LSemaphore(), mPostCount(0)
{
	// there's nothing to do
}


// ---------------------------------------------------------------------------
//	¥ LEventSemaphore(Boolean posted)
// ---------------------------------------------------------------------------
//	Constructor.
//
//	If posted is true, the semaphore is marked as being in the posted
//	state, and any calls to Wait() will return immediately.

LEventSemaphore::LEventSemaphore(Boolean posted)
	: LSemaphore()
{
	if (posted)
	{
		mExcessSignals	= max_Int32;
		mPostCount		= 1;
	}
	else
	{
		mPostCount		= 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LEventSemaphore()
// ---------------------------------------------------------------------------
//	Destructor.

LEventSemaphore::~LEventSemaphore()
{
	// there's nothing to do
}


// ---------------------------------------------------------------------------
//	¥ Signal
// ---------------------------------------------------------------------------
//	Make the semaphore available to all threads.  This includes all
//	blocked threads, as well as those that will call Wait() before
//	the next call to Reset().

void
LEventSemaphore::Signal()
{
	{
		StCritical	critical;	// disable preemption within this block

		// unblock all of our waiting threads "simultaneously"
		UnblockAll(noErr);

		mExcessSignals = max_Int32;
		++mPostCount;
	}

	// reschedule
	LThread::Yield();
}


// ---------------------------------------------------------------------------
//	¥ Reset
// ---------------------------------------------------------------------------
//	Make the semaphore unavailable to any thread until the next call
//	to Signal().  Returns the number of times that Signal() was called
//	since the last call to Reset().  Throws an exception if the semaphore
//	is already reset.

UInt32
LEventSemaphore::Reset()
{
	StCritical	critical;	// disable preemption
	UInt32		count;

	if (mExcessSignals > 0)
	{
		count			= mPostCount;
		mPostCount		= 0;
		mExcessSignals	= 0;
	}
	else
	{
		Throw_(errSemaphoreAlreadyReset);
	}

	return (count);
}


PP_End_Namespace_PowerPlant
