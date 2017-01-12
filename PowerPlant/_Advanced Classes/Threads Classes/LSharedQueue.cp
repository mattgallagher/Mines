// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharedQueue.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSharedQueue.h>
#include <UDebugging.h>
#include <UException.h>
#include <UThread.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LSharedQueue							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LSharedQueue::LSharedQueue()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LSharedQueue							Destructor				  [public]
// ---------------------------------------------------------------------------

LSharedQueue::~LSharedQueue()
{
}


// ---------------------------------------------------------------------------
//	¥ Next															  [public]
// ---------------------------------------------------------------------------
//	Removes and returns the first element in the queue.
//
//	An optional argument specifies how long the caller is willing to
//	wait for an element.  Possible values are:
//
//		sharedQueue_WaitForever	Wait indefinitely (default)
//		sharedQueue_NoWait		Do not wait. If queue is empty, return NULL
//		other (> 0)				Wait time in milliseconds

LLink*
LSharedQueue::Next(
	SInt32	milliSeconds)
{
	LLink*	outLinkP;
	OSErr	err;

	// block until there's something in the queue
	err = (OSErr) mAvailable.Wait(milliSeconds);

	if (err == errSemaphoreTimedOut)
	{
		outLinkP = NULL;
	}
	else if (err == noErr)
	{
		// get exclusive access to the queue within this block
		StMutex	mutex(*this);

		// remove first element from queue
		outLinkP = LQueue::NextGet();
	}
	else
		Throw_(err);

	return (outLinkP);
}


// ---------------------------------------------------------------------------
//	¥ Remove														  [public]
// ---------------------------------------------------------------------------
//	Remove an arbitrary element from the queue.
//
//	This function traverses the entire queue, looking for the given
//	queue element.  If the element is found, it is removed from the
//	queue.  Returns a Boolean indicating if the element was found.

Boolean
LSharedQueue::Remove(
	LLink*	inLinkP)
{
	Boolean	found;

	{
		StMutex	mutex(*this);				// Get exclusive access

		found = LQueue::Remove(inLinkP);	// Try to remove link
	}

	if (found) {							// If link was in the queue,
		mAvailable.Wait(semaphore_NoWait);	//   decrement availability count
	}

	return found;
}


// ---------------------------------------------------------------------------
//	¥ NextPut														  [public]
// ---------------------------------------------------------------------------
//	Adds the given element to the end of the queue

void
LSharedQueue::NextPut(
	LLink*	inLinkP)
{
	{
		// get exclusive access to the queue within this block
		StMutex	mutex(*this);

		// add element to the end of the queue
		LQueue::NextPut(inLinkP);
	}

	// signal the availability of an element
	mAvailable.Signal();
}


// ---------------------------------------------------------------------------
//	¥ DoForEach														  [public]
// ---------------------------------------------------------------------------
//	Execute a user-supplied function for each element in the queue

void
LSharedQueue::DoForEach(
	LQueueIterator	proc,
	void*			arg)
{
	// get exclusive access to the queue
	StMutex	mutex(*this);

	// iterate
	LQueue::DoForEach(proc, arg);
}

PP_End_Namespace_PowerPlant
