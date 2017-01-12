// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LThread.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LThread
#define _H_LThread
#pragma once

#include <PP_Prefix.h>

#include <cstddef>

#if !TARGET_RT_MAC_MACHO
	#include <PPCToolbox.h>
#endif

#include <Files.h>
#include <Timer.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	• Conditional compilation
// ===========================================================================


// set to a non-zero value to get debugging functions
#ifndef THREAD_DEBUG
#	define THREAD_DEBUG			0
#endif

// set to a non-zero value to get profiling functions
#ifndef THREAD_PROFILE
#	define THREAD_PROFILE		0
#endif

// set to a non-zero value to prevent Power Mgr from slowing down the CPU
#ifndef THREAD_INACTIVITY
#	define THREAD_INACTIVITY	0
#endif

// set to a non-zero value to enable interrupt protection of timetask removals	••FV
// Please refer the MW tech note entitled "Thread Class and Interrupts"
#ifndef THREAD_PROTECT_TIMETASK
#	define THREAD_PROTECT_TIMETASK		0
#endif

// ===========================================================================
//	• Constants
// ===========================================================================


// error codes
enum {
	errKilledThread = 28000,
	errBadThreadState
};


// thread stack size
const UInt32	thread_DefaultStack	= 0;
const SInt32		thread_WaitForever	= -1;


// ===========================================================================
//	• Data structures
// ===========================================================================


// forward class & structure declarations
class	LSemaphore;
class	LThread;

PP_End_Namespace_PowerPlant	// we don't want the following 2 forward
							// declarations to be within the PowerPlant namespace,
							// so we have to exclude them here

struct	SchedulerInfoRec;
struct	ProcessSerialNumber;

PP_Begin_Namespace_PowerPlant	// and start the namespace over again

// alias types
typedef UInt32	ThreadIDT;
typedef UInt32	ProfilerRefT;
typedef void	(*LThreadIterator)(LThread& inThread, void *arg);


// support for thread resumption from within an I/O completion routine.
typedef struct {
	LThread*			ioThread;
	SInt32				ioGlobals;
	union {
	
#if !TARGET_RT_MAC_MACHO
		ATPParamBlock		A;
#endif

		CInfoPBRec			C;
		ParamBlockRec		F;
		HParamBlockRec		H;
		
#if !TARGET_RT_MAC_MACHO
		MPPParamBlock		M;
		PPCParamBlockRec	P;
		XPPParamBlock		X;
#endif
	} ioPB;
} SThreadParamBlk, *SThreadPBPtr;

// support for sleep, timed wakeups, & async resumes
typedef struct {
	LThread*	ioThread;
	SInt32		ioGlobals;
	TMTask		ioTask;
	QHdr		ioSemQ;
	QElem		ioSemEl;
} SThreadTMTask;


// ===========================================================================
//	• External function declarations
// ===========================================================================


#if THREAD_DEBUG
extern "C" {
	// this function is called by THREAD_ASSERT macro
	int		ThreadAssert(char *test, char *file, int line);
}
#endif


// ===========================================================================
//	• class LThread
// ===========================================================================

class LThread
{
public:

	// constants

	// thread creation flags
	enum EThreadOption	{ threadOption_UsePool = 0x0002,
						  threadOption_Alloc   = 0x0004,
						  threadOption_NoFPU   = 0x0008,
						  threadOption_Exact   = 0x0010,
						  threadOption_Main    = 0x1000,
						  threadOption_Default = threadOption_Alloc  };

	// thread states
	enum EThreadState	{ threadState_Current,
						  threadState_Ready,
						  threadState_Suspended,
						  threadState_Sleeping,
						  threadState_Waiting,
						  threadState_Blocked };

	// pre-allocated threads
	static void			AllocateThreads(
							Boolean	preemptive,
							UInt16	numThreads,
							UInt32	stackSize);
	static void			GetFreeThreads(
							Boolean	preemptive,
							UInt16&	numThreads,
							UInt32	stackSize);

	// thread switching
	static void			Yield(const LThread *yieldTo = NULL);

	// critical sections
	static void			EnterCritical();
	static void			ExitCritical();

	// inquiries
	static LThread*		GetCurrentThread();
	static LThread*		GetMainThread();
	static SInt16		CountReadyThreads();
	static Boolean		InPreemptiveThread();
	static Boolean		InMainThread();
	static void*		GetThreadManagerTaskRef();

	// iterators
	static void			DoForEach(LThreadIterator proc, void* arg);

	// asynchronous resumes
	static void			ThreadAsynchronousResume(LThread* aThread);

	// semaphore support
	static LThread*		StateLinkToThread(QElemPtr inQElem);

#if !TARGET_RT_MAC_MACHO
	// free store
	void*				operator new(PP_STD::size_t size);
	void*				operator new(PP_STD::size_t size, void* ptr);
	void				operator delete(void* ptr);
#endif

	// debugging / profiling
#if THREAD_DEBUG
	static void			DumpAllThreads();
#endif

	// constructors / destructors
						LThread(
							Boolean					inPreemptive,
							UInt32					inStacksize = thread_DefaultStack,
							LThread::EThreadOption	inFlags = threadOption_Default,
							void**					outResult = NULL);

	// thread disposal
	virtual void		DeleteThread(void* inResult = NULL);

	// altered states
	virtual void		Suspend();
	virtual void		Resume();
	virtual void		Sleep(SInt32 milliSeconds);
	virtual void		Wake();
	virtual void		Block();

	// inquiries
	Boolean				IsPreemptive() const;
	Boolean				IsCurrent() const;
	Boolean				GetRecycle() const;
	UInt32				GetFreeStack() const;
	void*				GetResult() const;
	EThreadState		GetState() const;

	// modifiers
	void				SetRecycle(Boolean recycle);
	void				SetResult(void* mResult);
	void				SetNextOfKin(LThread* inThread);
	void				NotifyError(ExceptionCode inError);

	// asynchronous I/O
	virtual void		SetupAsynchronousResume(
							SThreadPBPtr	threadPB,
							IOCompletionUPP	callbackUPP = NULL);
	virtual SInt16		SuspendUntilAsyncResume(
							SThreadPBPtr	threadPB,
							SInt16			error);
	virtual SInt16		SuspendUntilAsyncResume(
							SThreadPBPtr	threadPB);

	// debugging
#if THREAD_DEBUG
	virtual void		Dump() const;
#endif


protected:

	// thread queues
	static void			AddToStateQueue(LThread& inThread, QHdr& ioQueue);
	static LThread*		RemoveFromStateQueue(QElemPtr inQElem, QHdr& ioQueue);
	static LThread*		SemUnwait(LSemaphore* inSemaphore, ExceptionCode inError,
							QElemPtr inQElem, QHdr& ioQHdr);

	// Time Manager interface
	static void			InsertTimeTask(SThreadTMTask& task);
	static void			PrimeTimeTask(SThreadTMTask& task, SInt32 interval);
	static void			RemoveTimeTask(SThreadTMTask& task);

#if THREAD_INACTIVITY
	// Power Manager interface
	static void			PreventInactivity();
#endif

	// completion routines
#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
	static pascal void	ThreadComplProc(ParmBlkPtr pbPtr);
	static pascal void	ThreadTimerProc(TMTaskPtr tmTaskPtr);
#else
	static pascal void	ThreadComplProc(ParmBlkPtr pbPtr : __A0);
	static pascal void	ThreadTimerProc(TMTaskPtr tmTaskPtr : __A1);
#endif

	// static member variables
	static LThread* volatile	sThread;			// current thread
	static LThread* volatile	sMainThread;		// main thread
	static QHdr volatile		sThreadQueue;		// all known threads
	static ProcessSerialNumber	sPSN;				// ptr to application's PSN
	static void*				sThreadTaskRef;		// thread task ref
	static TimerUPP				sThreadTimerUPP;	// UPP for time mgr
	static IOCompletionUPP		sThreadComplUPP;	// UPP for async resumes

	// constructors / destructors
	virtual				~LThread();

	// thread switching
	virtual void		SwapContext(Boolean swappingIn);

	// thread execution
	virtual void*		Run() = 0;
	virtual void		ThreadDied(const LThread& inThread);

	// altered states
	void				MakeReady();
	void				MakeUnready();
	virtual ExceptionCode	SemWait(
								LSemaphore*	inSemaphore,
								SInt32		inMilliSeconds,
								QHdr&		ioQHdr,
								Boolean&	outTimedOut);

	// member variables
	QElemPtr			mThreadLink;	// link in queue of all known threads
	QElemPtr			mStateLink;		// link in state-dependent queue
	ThreadIDT			mThread;		// thread ID
	LThread*			mNextOfKin;		// thread to notify in case of death
	void*				mResult;		// thread result
	LSemaphore*			mSemaphore;		// the sem we're waiting on, or NULL
	SThreadTMTask		mTimer;			// timeout stuff
	ExceptionCode		mError;			// for detecting errors while a thread is asleep
	EThreadState		mState;			// current state (ready, sleeping, etc)
	Boolean				mPreemptive;	// is this a preemptive thread ?
	Boolean				mRecycle;		// re-use stack upon termination ?
	Boolean				mFPU;			// uses FPU ?
	Boolean				mAsyncCompleted;// true if thread’s async timer has fired
#if THREAD_PROFILE
	ProfilerRefT		mProfilerRef;	// Profiler context
#endif


private:

	// nested classes
	class				Init;
	class				Cleanup;

	// class initialisation
	static void			EnterThreads();
	static void			ExitThreads();

	// thread queues
	static LThread*		RemoveFromGlobalQueue(QElemPtr inQElem, QHdr volatile& ioQueue);
	static void			AddToGlobalQueue(LThread& inThread, QHdr volatile& ioQueue);

	// Thread Manager callbacks
	static pascal void*	DoEntry(void* arg);
	static pascal void	DoSwapIn(UInt32 switchedThread, void* arg);
	static pascal void	DoSwapOut(UInt32 switchedThread, void* arg);
#if THREAD_DEBUG
	static pascal void	DoExit(ThreadIDT inThread, void* arg);
#endif

	// static member variables
	static LThread::Init	sThreadInit;		// class initialisation object
	static Boolean			sInited;			// is the thread class initialised?
	static LThread*			sCleanupThread;		// thread disposal thread
	static QHdr volatile	sCleanupQueue;		// threads awaiting deletion
	static LSemaphore*		sCleanupSem;
	static SInt16 volatile	sReadyThreadCount;	// # of threads capable of runnning
#if THREAD_INACTIVITY
	static Boolean			sHasIdleUpdate;		// IdleUpdate call is available
	static SInt32			sNextUpdateTicks;	// TickCount at next call to IdleUpdate
#endif
#if !TARGET_RT_MAC_CFM
	static long				sGlobals;
#endif

	// constructors / destructors
						LThread(const LThread&);

	// assignment
	LThread&			operator = (const LThread&);

	// class initialisation
	void				CreateMainThread();

	// friends
	friend class		LSemaphore;
	friend class		LThread::Init;
	friend class		LThread::Cleanup;
};


// ===========================================================================
//	• UMainThread Class
// ===========================================================================


class UMainThread : public LThread
{
public:
					UMainThread();
	virtual			~UMainThread();
protected:
	virtual void*	Run();
};


// ===========================================================================
//	• Debugging Support
// ===========================================================================

#if THREAD_DEBUG
#	define THREAD_ASSERT(cond)	\
		((void) ((cond) ? 0 : ThreadAssert(#cond, __FILE__, __LINE__)))
#else
#	define THREAD_ASSERT(cond)	((void) 0)
#endif



// ===========================================================================
//	• Inline static member functions
// ===========================================================================

#ifdef __MWERKS__
#	pragma push
#	pragma profile off
#endif

// ---------------------------------------------------------------------------
//	• GetCurrentThread	[static]
// ---------------------------------------------------------------------------
//	Returns the current thread object.

inline LThread*
LThread::GetCurrentThread()
{
	return (sThread);
}


// ---------------------------------------------------------------------------
//	• GetMainThread	[static]
// ---------------------------------------------------------------------------
//	Returns the main thread object.

inline LThread*
LThread::GetMainThread()
{
	return (sMainThread);
}


// ---------------------------------------------------------------------------
//	• CountReadyThreads	[static]
// ---------------------------------------------------------------------------
//	Returns the number of ready threads (i.e., the number of threads eligible
//	for CPU time).  This number does not include the current thread.

inline SInt16
LThread::CountReadyThreads()
{
	return (sReadyThreadCount);
}


// ---------------------------------------------------------------------------
//	• InMainThread	[static]
// ---------------------------------------------------------------------------
//	Returns whether or not the current thread is also the main thread.

inline Boolean
LThread::InMainThread()
{
	return (GetCurrentThread() == GetMainThread());
}


// ---------------------------------------------------------------------------
//	• InsertTimeTask	[static]
// ---------------------------------------------------------------------------
//	Insert the given task into the Time Manager's queue.

inline void
LThread::InsertTimeTask(SThreadTMTask& task)
{
	// signal timer "semaphore"
	::Enqueue(&task.ioSemEl, &task.ioSemQ);
	// add element to timer queue
	::InsTime(reinterpret_cast<QElemPtr>(&task.ioTask));
}


// ---------------------------------------------------------------------------
//	• PrimeTimeTask	[static]
// ---------------------------------------------------------------------------
//	Activate the given task in the Time Manager's queue.

inline void
LThread::PrimeTimeTask(SThreadTMTask& task, long interval)
{
	::PrimeTime(reinterpret_cast<QElemPtr>(&task.ioTask), interval);
}


// ---------------------------------------------------------------------------
//	• RemoveTimeTask	[static]
// ---------------------------------------------------------------------------
//	Remove the given task from the Time Manager's queue.

#if THREAD_PROTECT_TIMETASK
#include "InterruptDisableLib.h"
#endif

inline void
LThread::RemoveTimeTask(SThreadTMTask& task)
{
#if THREAD_PROTECT_TIMETASK
    UInt16 oldMask = SetInterruptMask(7);
#endif

	// attempt to grab timer "semaphore"
	if (::Dequeue(&task.ioSemEl, &task.ioSemQ) == noErr)
	{
		::RmvTime(reinterpret_cast<QElemPtr>(&task.ioTask));
	}

#if THREAD_PROTECT_TIMETASK
	SetInterruptMask(oldMask);
#endif
}


// ===========================================================================
//	• Inline regular member functions
// ===========================================================================

// ---------------------------------------------------------------------------
//	• IsCurrent
// ---------------------------------------------------------------------------
//	Is the thread the current thread ?

inline Boolean
LThread::IsCurrent() const
{
	return (this == sThread);
}


// ---------------------------------------------------------------------------
//	• MakeReady
// ---------------------------------------------------------------------------
//	Put a thread into the ready state.

inline void
LThread::MakeReady()
{
	mState = threadState_Ready;
	++sReadyThreadCount;
}


// ---------------------------------------------------------------------------
//	• MakeUnready
// ---------------------------------------------------------------------------
//	Remove a thread from the ready state.

inline void
LThread::MakeUnready()
{
	--sReadyThreadCount;
}

#ifdef __MWERKS__
#	pragma pop
#endif

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LThread
