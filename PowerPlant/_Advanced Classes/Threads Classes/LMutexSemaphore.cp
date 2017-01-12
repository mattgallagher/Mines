// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMutexSemaphore.cp			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

// PowerPlant headers
#include <LMutexSemaphore.h>
#include <LThread.h>
#include <UException.h>
#include <UThread.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LMutexSemaphore()
// ---------------------------------------------------------------------------
//	Default constructor.  The semaphore is created in the unowned state.

LMutexSemaphore::LMutexSemaphore()
	: LSemaphore(1), mOwner(NULL), mNestedWaits(0)
{
	// there's nothing to do
}


// ---------------------------------------------------------------------------
//	¥ LMutexSemaphore(Boolean owned)
// ---------------------------------------------------------------------------
//	Constructor.
//
//	If owned is true, the semaphore is marked as belonging to the current
//	thread.

LMutexSemaphore::LMutexSemaphore(Boolean owned)
	: LSemaphore(owned ? 0 : 1), mNestedWaits(0)
{
	if (owned)
	{
		THREAD_ASSERT(LThread::GetCurrentThread() != NULL);

		mOwner = LThread::GetCurrentThread();
	}
	else
	{
		mOwner = NULL;
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LMutexSemaphore()
// ---------------------------------------------------------------------------
//	Destructor.

LMutexSemaphore::~LMutexSemaphore()
{
	// there's nothing to do
}


// ---------------------------------------------------------------------------
//	¥ Wait
// ---------------------------------------------------------------------------
//	Wait for a semaphore to become available.
//
//	An optional argument specifies how long the caller is willing to
//	wait for the semaphore to become available.  Possible values are:
//
//		semaphore_WaitForever	Wait indefinitely (default).
//		semaphore_NoWait		Do not wait.  If the semaphore is unavailable,
//								return errSemaphoreTimedOut.
//		other (> 0)				Wait time in milliseconds
//
//	If the time interval expires before the semaphore becomes available,
//	this function returns errSemaphoreTimedOut.
//
//	If the current thread already owns the semaphore, this function merely
//	increments a usage count, and returns immediately.

ExceptionCode
LMutexSemaphore::Wait(SInt32 milliSeconds)
{
	StCritical		critical;		// disable preemption
	ExceptionCode	err;

	// does the current thread already own this semaphore ?
	if (mOwner == LThread::GetCurrentThread())
	{
		// yes:  bump up usage count
		err = noErr;
		mNestedWaits++;
	}
	// is this semaphore available ?
	else if (mOwner == NULL)
	{
		// claim semaphore for the current thread

		THREAD_ASSERT(mExcessSignals == 1);

		err				= noErr;
		mOwner			= LThread::GetCurrentThread();
		mNestedWaits	= 0;
		--mExcessSignals;
	}
	// semaphore is in use;  do we want to return immediately ?
	else if (milliSeconds == semaphore_NoWait)
	{
		err = errSemaphoreTimedOut;
	}
	// else wait for semaphore
	else
	{
		err = BlockThread(milliSeconds);
	}

	return (err);
}


// ---------------------------------------------------------------------------
//	¥ Signal
// ---------------------------------------------------------------------------
//	Release a semaphore.
//
//	If the owning semaphore called Wait() more than once, this function
//	merely decrements a usage count.  Else, if any threads are waiting
//	on the semaphore, one of them is unblocked.  If the current thread
//	doesn't own the semaphore, the errSemaphoreNotOwner exception is
//	thrown.

void
LMutexSemaphore::Signal()
{
	LThread		*thread	= LThread::GetCurrentThread();

	{
		StCritical	critical;	// disable preemption within this block

		// does the current thread actually own this semaphore ?
		if (thread != mOwner)
		{
			// no, return error
			Throw_(errSemaphoreNotOwner);
		}
		// has the current thread called Wait() more than once on this sem ?
		else if (mNestedWaits > 0)
		{
			// yes, decrement usage count
			--mNestedWaits;
		}
		// are other threads waiting for this semaphore ?
		else if (mExcessSignals < 0)
		{
			THREAD_ASSERT(mThreads.qHead != NULL);

			// yes, give the sem to the first thread
			mOwner = UnblockThread(mThreads.qHead, noErr);
		}
		// no other threads are waiting
		else
		{
			THREAD_ASSERT(mExcessSignals == 0);

			mOwner = NULL;
			++mExcessSignals;
		}
	}

	// reschedule
	LThread::Yield();
}

PP_End_Namespace_PowerPlant
