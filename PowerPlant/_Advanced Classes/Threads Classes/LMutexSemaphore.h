// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMutexSemaphore.h			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LMutexSemaphore
#define _H_LMutexSemaphore
#pragma once

#include <LSemaphore.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

class LMutexSemaphore : public LSemaphore {
public:

	// constructors / destructors
					LMutexSemaphore();
					LMutexSemaphore(Boolean owned);
	virtual			~LMutexSemaphore();

	// blocking
	virtual void			Signal();
	virtual ExceptionCode	Wait(SInt32 milliSeconds = semaphore_WaitForever);

protected:

	// member variables
	LThread			*mOwner;		// thread holding this semaphore
	UInt32			mNestedWaits;	// # of times Wait() was called by owner
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LMutexSemaphore
