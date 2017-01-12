// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UThread.cp					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde
//
//	Thread utility classes
//
//	Stack-based Classes:
//		Several utility classes are designed for creating stack-based
//		objects, where the Constructor performs some action and the
//		Destructor undoes the action. The advantage of stack-based
//		objects is that the Destructor automatically gets called,
//		even when there is an exception thrown.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

// PowerPlant headers
#include <UThread.h>
#include <LMutexSemaphore.h>
#include <LThread.h>
#include <UException.h>

// system headers
#include <Threads.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ StCritical													  StCritical ¥
// ===========================================================================
//	Constructor enters a critical section (i.e., it disables preemption).
//	Destructor exits the critical section.

StCritical::StCritical()
{
	::ThreadBeginCritical();
}


StCritical::~StCritical()
{
	::ThreadEndCritical();
}


// ===========================================================================
// ¥ StMutex														 StMutex ¥
// ===========================================================================
//	Constructor waits on the given mutual exclusion semaphore.
//	Destructor releases the semaphore.

StMutex::StMutex(LMutexSemaphore& inMutex)
	: mMutex(inMutex)
{
	ThrowIfOSErr_((OSErr) mMutex.Wait());
}


StMutex::~StMutex()
{
	mMutex.Signal();
}


// ===========================================================================
// ¥ LYieldAttachment										LYieldAttachment ¥
// ===========================================================================
//
//	This class is an attachment that simply yields control to another thread
//	before returning.  It obviates the need to override
//	LApplication::ProcessNextEvent() in order to give time to other threads.
//	Also, an optional parameter to the constructor lets you specify how much
//	time should elapse before control returns to the caller.  This is useful
//	for preventing WaitNextEvent from being called too often.


// ---------------------------------------------------------------------------
//	¥ LYieldAttachment
// ---------------------------------------------------------------------------
//	Constructor.

LYieldAttachment::LYieldAttachment(SInt32 inQuantum)
	: LAttachment(msg_Event, true), mQuantum(inQuantum), mNextTicks(0)
{
	// there's nothing else to do
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf
// ---------------------------------------------------------------------------
//	Yield control to other threads.  Note that control is always yielded at
//	least once.

void	LYieldAttachment::ExecuteSelf(
	MessageT	/* inMessage */,
	void*		/* ioParam */)
{
	if (mQuantum < 0)
	{
		// no quantum:  just yield & leave

		LThread::Yield();
	}
	else
	{
		// We have a non-negative yield quantum.  Just sit in a loop for the
		// specified number of ticks, yielding control to other threads.

		UInt32	currTicks;

		do
		{
			LThread::Yield();

			currTicks = ::TickCount();

		} while (currTicks <= mNextTicks);

		mNextTicks = currTicks + mQuantum;
	}
}

PP_End_Namespace_PowerPlant
