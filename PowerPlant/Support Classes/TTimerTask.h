// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TTimerTask.h				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//
//	Template class for an Event Loop Timer where the Toolbox callback calls
//	a member function of a C++ class. The function has the form:
//
//		void CMyObject::MyTimer( LTimerTask* inTask );
//
//	The template is parameterized by the class containing the function to
//	call. Typically, you will store a TTimerTask as a member object of the
//	class. For example,
//
//		class CMyClass {
//		public:
//			
//			CMyClass()
//				: mTimerTask(this, &CMyClass::TimerCallback)
//				{ }
//
//			void TimerCallback(LTimerTask *inTask);
//
//		private:
//			TTimerTask<CMyClass>	mTimerTask;
//
//	Whenever the timer fires (you will need to call mTimerTask.Install()
//	at some point), the TimerCallback() member function will be called
//	for the particular CMyClass object.

#ifndef _H_TTimerTask
#define _H_TTimerTask
#pragma once

#include <LTimerTask.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

template <class T> class TTimerTask : public LTimerTask {
public:

typedef	void (T:: * TimerFunc)( LTimerTask* );

// ---------------------------------------------------------------------------
//	¥ TTimerTask							Default Constructor
// ---------------------------------------------------------------------------

TTimerTask()
{
	mTimerObject = nil;
	mTimerFunc	 = nil;
}


// ---------------------------------------------------------------------------
//	¥ TTimerTask							Parameterized Constructor
// ---------------------------------------------------------------------------
//	Specify timer object and callback function
//
//	You must call Install() to register the task with the System

TTimerTask(
	T*			inTimerObject,
	TimerFunc	inTimerFunc)
{
	mTimerObject = inTimerObject;
	mTimerFunc	 = inTimerFunc;
}


// ---------------------------------------------------------------------------
//	¥ TTimerTask							Parameterized Constructor
// ---------------------------------------------------------------------------
//	Construct and install Timer Task

TTimerTask(
	EventLoopRef		inEventLoop,
	EventTimerInterval	inFireDelay,
	EventTimerInterval	inInterval,
	T*					inTimerObject,
	TimerFunc			inTimerFunc)
{
	mTimerObject = inTimerObject;
	mTimerFunc	 = inTimerFunc;

	Install(inEventLoop, inFireDelay, inInterval);
}


// ---------------------------------------------------------------------------
//	¥ ~TTimerTask							Destructor
// ---------------------------------------------------------------------------

virtual
~TTimerTask()
{
}


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------

using LTimerTask::Install;


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------

OSStatus
Install(
	EventLoopRef		inEventLoop,
	EventTimerInterval	inFireDelay,
	EventTimerInterval	inInterval,
	T*					inTimerObject,
	TimerFunc			inTimerFunc)
{
	mTimerObject = inTimerObject;
	mTimerFunc	 = inTimerFunc;

	return Install(inEventLoop, inFireDelay, inInterval);
}


// ---------------------------------------------------------------------------
//	¥ GetTimerObject
// ---------------------------------------------------------------------------

T*
GetTimerObject() const
{
	return mTimerObject;
}


// ---------------------------------------------------------------------------
//	¥ SetTimerObject
// ---------------------------------------------------------------------------

void
SetTimerObject(
	T*	inTimerObject)
{
	mTimerObject = inTimerObject;
}


// ---------------------------------------------------------------------------
//	¥ GetTimerFunc
// ---------------------------------------------------------------------------

TimerFunc
GetTimerFunc() const
{
	return mTimerFunc;
}


// ---------------------------------------------------------------------------
//	¥ SetTimerFunc
// ---------------------------------------------------------------------------

void
SetTimerFunc(
	TimerFunc	inTimerFunc)
{
	mTimerFunc = inTimerFunc;
}


// ---------------------------------------------------------------------------
//	¥ DoTask
// ---------------------------------------------------------------------------

virtual
void
DoTask()
{
	if ( (mTimerObject != nil) && (mTimerFunc != nil) ) {
	
		(mTimerObject->*mTimerFunc)(this);
	}
}


// ===========================================================================
//	Instance Variables
// ===========================================================================

private:
	T*			mTimerObject;
	TimerFunc	mTimerFunc;
};


PP_End_Namespace_PowerPlant

#endif
