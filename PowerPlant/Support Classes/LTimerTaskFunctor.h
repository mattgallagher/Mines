// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTimerTaskFunctor.h			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Carbon Event Loop Timer where the Toolbox callback calls an ordinary
//	C function of the form:
//
//		void MyTimer( LTimerTask* inTask );

#ifndef _H_LTimerTaskFunctor
#define _H_LTimerTaskFunctor
#pragma once

#include <LTimerTask.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTimerTaskFunctor : public LTimerTask {
public:

	typedef	 void (* TimerFunc)( LTimerTask* );
	
						LTimerTaskFunctor();
						
						LTimerTaskFunctor( TimerFunc inTimerFunc );
						
						LTimerTaskFunctor(
								EventLoopRef		inEventLoop,
								EventTimerInterval	inFireDelay,
								EventTimerInterval	inInterval,
								TimerFunc			inTimerFunc);
						
	virtual				~LTimerTaskFunctor();
	
	using				LTimerTask::Install;
	
	OSStatus			Install(
								EventLoopRef		inEventLoop,
								EventTimerInterval	inFireDelay,
								EventTimerInterval	inInterval,
								TimerFunc			inTimerFunc);
								
	TimerFunc			GetTimerFunc() const;
	
	void				SetTimerFunc( TimerFunc inFunc );
	
	virtual void		DoTask();

private:
	TimerFunc		mTimerFunc;
};


// ===========================================================================
//	Inline Functions
// ===========================================================================

inline
LTimerTaskFunctor::TimerFunc
LTimerTaskFunctor::GetTimerFunc() const
{
	return mTimerFunc;
}


inline
void
LTimerTaskFunctor::SetTimerFunc(
	TimerFunc	inFunc)
{
	mTimerFunc = inFunc;
}

PP_End_Namespace_PowerPlant

#endif

