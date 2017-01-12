// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSemaphore.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

// PowerPlant headers
#include <LSemaphore.h>
#include <LThread.h>
#include <UDebugging.h>
#include <UThread.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ Static member variables
// ===========================================================================


#if SEMAPHORE_IDS
QHdr	LSemaphore::sSemaphores			= { 0, NULL, NULL };
UInt32	LSemaphore::sNextSemaphoreID	= 0;
#endif


// ===========================================================================
//	¥ Static member functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ FindSemaphore	[static]
// ---------------------------------------------------------------------------
//	Locates a semaphore by ID number.
//
//	Given a semaphore's ID, this function traverses the list of known
//	semaphores, looking for a matching ID.  If it finds it, it returns
//	the semaphore object;  else, it returns NULL.

#if SEMAPHORE_IDS

LSemaphore*
LSemaphore::FindSemaphore(UInt32 itsID)
{
	StCritical	critical;		// disable preemption
	LSemaphore*	foundSem	= NULL;
	QElemPtr	qElPtr;

	// for each semaphore ...
	for (qElPtr = sSemaphores.qHead; qElPtr != NULL; qElPtr = qElPtr->qLink)
	{
		LSemaphore*	sem;
		sem = reinterpret_cast<LSemaphore *>(
				reinterpret_cast<char *>(qElPtr) -
				offsetof(LSemaphore, mQLink));

		// if this is the one, return
		if (sem->mID == itsID)
		{
			foundSem = sem;
			break;
		}
	}

	return (foundSem);
}

#endif


// ===========================================================================
//	¥ Non-static member functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ LSemaphore()
// ---------------------------------------------------------------------------
//	Default constructor.  Sets the excess signal count to zero.

LSemaphore::LSemaphore()
{
	InitSemaphore(0);
}


// ---------------------------------------------------------------------------
//	¥ LSemaphore(SInt32 initialCount)
// ---------------------------------------------------------------------------
//	Constructor.
//
//	InitialCount, which must be non-negative, indicates the number of times
//	Wait() can be called before causing a thread to block.

LSemaphore::LSemaphore(SInt32 initialCount)
{
	THREAD_ASSERT(initialCount >= 0);

	InitSemaphore(initialCount);
}


// ---------------------------------------------------------------------------
//	¥ ~LSemaphore
// ---------------------------------------------------------------------------
//	Destructor.
//
//	This function unblocks all the threads that are waiting on the
//	semaphore.  Each thread will then throw the errSemaphoreDestroyed
//	exception.

LSemaphore::~LSemaphore()
{
#if SEMAPHORE_IDS
	// remove ourselves from the list of known semaphores
	::Dequeue(reinterpret_cast<QElemPtr>(&mQLink), &sSemaphores);
#endif

	// unblock all threads waiting on this semaphore,
	// telling them that it was destroyed
	UnblockAll(errSemaphoreDestroyed);
}


// ---------------------------------------------------------------------------
//	¥ InitSemaphore(const LSemaphore&)
// ---------------------------------------------------------------------------
//	Initialises the fields of a semaphore.

void
LSemaphore::InitSemaphore(SInt32 initialCount)
{
	mThreads.qFlags	= 0;
	mThreads.qHead	= NULL;
	mThreads.qTail	= NULL;
	mExcessSignals	= initialCount;

#if SEMAPHORE_IDS
	mQLink			= NULL;
	mID				= ++sNextSemaphoreID;

	// add ourselves to the list of known semaphores
	::Enqueue(reinterpret_cast<QElemPtr>(&mQLink), &sSemaphores);
#endif
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
//		semaphore_NoWait		Do not wait.  If the semaphore is
//								unavailable, return errSemaphoreTimedOut.
//		other (> 0)				Wait time in milliseconds
//
//	If the time interval expires before the semaphore becomes available,
//	this function returns errSemaphoreTimedOut.

ExceptionCode
LSemaphore::Wait(SInt32 milliSeconds)
{
	StCritical		critical;		// disable preemption
	ExceptionCode	err;

	// do we have a surplus of calls to Signal() ?
	if (mExcessSignals > 0)
	{
		--mExcessSignals;
		err = noErr;
	}
	// is the caller in a hurry ?
	else if (milliSeconds == semaphore_NoWait)
	{
		err = errSemaphoreTimedOut;
	}
	// the calling thread will have to wait in line
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
//	If any threads are waiting on the semaphore, one of them is unblocked.

void
LSemaphore::Signal()
{
	{
		StCritical	critical;		// disable preemption within this block

		if (mExcessSignals < 0)
		{
			THREAD_ASSERT(mThreads.qHead != NULL);

			UnblockThread(mThreads.qHead, noErr);
		}
		else
			++mExcessSignals;
	}

	// give control to next thread
	LThread::Yield();
}


// ---------------------------------------------------------------------------
//	¥ BlockThread
// ---------------------------------------------------------------------------
//	Block the current thread for the given number of milliseconds (see
//	Wait()).
//
//	NOTE:	The current thread must be in a 1-level-deep critical section
//			upon entering this function!

ExceptionCode
LSemaphore::BlockThread(SInt32 milliSeconds)
{
	LThread*		thread	= LThread::GetCurrentThread();
	Boolean			timedOut;
	ExceptionCode	err;

	// adjust signal count
	--mExcessSignals;

	// block the thread
	err = thread->SemWait(this, milliSeconds, mThreads, timedOut);

	if (timedOut)
	{
		// the current thread is no longer waiting for the sem,
		// so we have to adjust the signal count
		++mExcessSignals;

		// propagate error to caller
		if (err == noErr)
			err = errSemaphoreTimedOut;
	}

	return (err);
}


// ---------------------------------------------------------------------------
//	¥ UnblockThread
// ---------------------------------------------------------------------------
//	Unblock the thread pointed to by qEl, passing it the given error code.
//
//	NOTE:	The current thread must be in a critical section
//			upon entering this function!

LThread*
LSemaphore::UnblockThread(QElemPtr qEl, ExceptionCode error)
{
	LThread*	thread	= NULL;

	if (qEl != NULL)
	{
		thread = LThread::SemUnwait(this, error, qEl, mThreads);

		// was the thread in the queue ?
		if (thread != NULL)
		{
			// thread found:  all is well
			++mExcessSignals;
		}
	}

	return (thread);
}


// ---------------------------------------------------------------------------
//	¥ UnblockAll
// ---------------------------------------------------------------------------
//	Unblock all of the threads waiting on this semaphore, passing them
//	the given error code.

void
LSemaphore::UnblockAll(ExceptionCode error)
{
	StCritical	critical;		// disable preemption

	while (mThreads.qHead != NULL)
	{
		UnblockThread(mThreads.qHead, error);
	}
}

PP_End_Namespace_PowerPlant
