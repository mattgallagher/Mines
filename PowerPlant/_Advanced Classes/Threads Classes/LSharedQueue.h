// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharedQueue.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LSharedQueue
#define _H_LSharedQueue
#pragma once

#include <LMutexSemaphore.h>
#include <LQueue.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


const SInt32	sharedQueue_WaitForever	= -1;
const SInt32	sharedQueue_NoWait		= 0;

// ---------------------------------------------------------------------------

class LSharedQueue : public  LQueue,
					 private LMutexSemaphore
{
public:

	// constructors / destructors
					LSharedQueue();
	virtual			~LSharedQueue();

	// queue manipulation
	virtual LLink*	Next(SInt32 milliSeconds = sharedQueue_WaitForever);
	virtual void	NextPut(LLink* aLink);
	virtual Boolean	Remove(LLink* aLink);

	// iterators
	virtual void	DoForEach(LQueueIterator proc, void* arg);

protected:

	// member variables
	LSemaphore		mAvailable;		// will block threads when queue is empty
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LSharedQueue
