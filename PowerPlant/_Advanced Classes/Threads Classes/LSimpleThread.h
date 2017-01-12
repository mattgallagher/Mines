// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleThread.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LSimpleThread
#define _H_LSimpleThread
#pragma once

#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// callbacks
typedef void	(*ThreadProc)(LThread& thread, void* arg);

// ---------------------------------------------------------------------------

class LSimpleThread : public LThread {
public:

	// constructors / destructors
	LSimpleThread(
		ThreadProc				proc,
		void*					arg,
		Boolean					isPreemptive = false,
		UInt32					stackSize = thread_DefaultStack,
		LThread::EThreadOption	flags = threadOption_Default);

protected:

	// thread execution
	virtual void*	Run();

private:

	// member variables
	ThreadProc		mProc;
	void*			mArg;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LSimpleThread
