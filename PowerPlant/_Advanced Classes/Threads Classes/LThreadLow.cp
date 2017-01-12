// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LThreadLow.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LThread.h>
#include <UDebugging.h>
#include <UEnvironment.h>
#include <UException.h>

#if THREAD_DEBUG
	#include <cstdio>
#endif

#include <Gestalt.h>
#include <Power.h>
#include <Processes.h>

#if !TARGET_RT_MAC_MACHO
	#include <SegLoad.h>
#endif

#include <Threads.h>

#if TARGET_RT_MAC_CFM
	#include <CodeFragments.h>
#endif

#if THREAD_PROFILE
	#include <Profiler.h>
#endif


#ifdef __MWERKS__
// turn off profiling in this module
#pragma profile off
#endif


PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ Constants
// ===========================================================================


#define	DEBUG_COMPLETION_PROC	0		// set non-zero to break on entry


// define debugging macro, if needed
#if DEBUG_COMPLETION_PROC
#	define BREAK_INTO_DEBUGGER()	::Debugger()
#else
#	define BREAK_INTO_DEBUGGER()
#endif

// register-based arguments & return values
#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
#	define A0_PARAM
#	define A1_PARAM
#else
#	define A0_PARAM		: __A0
#	define A1_PARAM		: __A1
#endif
#define A0_RESULT		A0_PARAM
#define A1_RESULT		A1_RESULT


// milliseconds between attempts to wake up a thread
const SInt32		kThreadWakeupDelay	= 10;


// ===========================================================================
//	¥ Local function declarations
// ===========================================================================


#if THREAD_DEBUG
static void	DumpOneThread(LThread& thread, void *arg);
#endif


// ===========================================================================
//	Type coercions from OS structures to our structures.  Ugly.
// ===========================================================================

inline	SThreadParamBlk*
GetParmBlkPtr(ParmBlkPtr pbPtr A0_PARAM) A0_RESULT
{
	PP_Using_Namespace_Std	// Pro 3's MSL offsetof macro isn't
							// namespace std savvy.

	return (reinterpret_cast<SThreadParamBlk *>(
			reinterpret_cast<char *>(pbPtr) -
			offsetof(SThreadParamBlk, ioPB)));
}

inline SThreadTMTask*
GetTimeMgrPtr(TMTaskPtr tmTaskPtr A1_PARAM) A0_RESULT
{
	PP_Using_Namespace_Std	// Pro 3's MSL offsetof macro isn't
							// namespace std savvy.

	return (reinterpret_cast<SThreadTMTask *>(
			reinterpret_cast<char *>(tmTaskPtr) -
			offsetof(SThreadTMTask, ioTask)));
}


// ===========================================================================
//	¥ Callbacks and Completion Routines
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ DoEntry	[static]
// ---------------------------------------------------------------------------
//	Callback to the Thread Manager.
//
//	This function is called when a thread begins execution.  It simply
//	calls the thread's Run() member function.  If Run() returns (either
//	normally or through the exception mechanism), the thread object is
//	deallocated.

pascal void*
LThread::DoEntry(void* arg)
{
	LThread* volatile	thread	= reinterpret_cast<LThread *>(arg);
	void* volatile		result	= NULL;

//	::DebugStr("\pEntering DoEntry");

	try
	{
		// run the thread
		result = thread->Run();
	}
	catch (...)
	{
		// all we want is to stop error propagation
	}

	// destroy thread
	thread->DeleteThread(result);

	return (result);
}


// ---------------------------------------------------------------------------
//	¥ DoExit	[static]
// ---------------------------------------------------------------------------
//	"Fake" termination function.  Does nothing.

#if THREAD_DEBUG

pascal void
LThread::DoExit(ThreadID /* inThread */, void* /* arg */)
{
//	::DebugStr("\pWarning: Thread Mgr thread destroyed before thread object; g");
}

#endif


// ---------------------------------------------------------------------------
//	¥ DoSwapIn	[static]
// ---------------------------------------------------------------------------
//	Callback to the Thread Manager.
//
//	Sets up global variables when a thread is switched in.  It also calls
//	the thread's swap function, if it has one.

pascal void
LThread::DoSwapIn(ThreadID, void* arg)
{
	try
	{
		LThread*	thread	= reinterpret_cast<LThread *>(arg);

	//	::DebugStr("\pentering DoSwapIn");

#if !TARGET_RT_MAC_CFM
		// get access to globals
		SInt32 savedA5 = SetA5(thread->mTimer.ioGlobals);
#endif

#if THREAD_PROFILE
		::ProfilerSwitchToThread(thread->mProfilerRef);
#endif

		// call custom swap function
		thread->SwapContext(true);

#if !TARGET_RT_MAC_CFM
		// restore globals ptr
		SetA5(savedA5);
#endif
	}
	catch (...)
	{
		// all we want is to stop error propagation
	}
}


// ---------------------------------------------------------------------------
//	¥ DoSwapOut	[static]
// ---------------------------------------------------------------------------
//	Callback to the Thread Manager.
//
//	Sets up global variables when a thread is switched out.  It also calls
//	the thread's swap function, if it has one.

pascal void
LThread::DoSwapOut(ThreadID, void* arg)
{
	try
	{
		LThread*	thread	= reinterpret_cast<LThread *>(arg);

	//	::DebugStr("\pentering DoSwapOut");

#if !TARGET_RT_MAC_CFM
		// get access to globals
		SInt32 savedA5 = SetA5(thread->mTimer.ioGlobals);
#endif

		// call custom swap function
		thread->SwapContext(false);

#if !TARGET_RT_MAC_CFM
		// restore globals ptr
		SetA5(savedA5);
#endif
	}
	catch (...)
	{
		// all we want is to stop error propagation
	}
}


// ---------------------------------------------------------------------------
//	¥ ThreadComplProc	[static]
// ---------------------------------------------------------------------------
//	I/O completion routine.  Converts the given parameter block into a
//	SThreadParamBlk, then resumes the given thread.

pascal void
LThread::ThreadComplProc(ParmBlkPtr pbPtr A0_PARAM)
{
	SThreadParamBlk*	tpb = GetParmBlkPtr(pbPtr);

	// break if desired
	BREAK_INTO_DEBUGGER();

#if !TARGET_RT_MAC_CFM
	long savedA5 = SetA5(tpb->ioGlobals);
#endif

	ThreadAsynchronousResume(tpb->ioThread);

#if !TARGET_RT_MAC_CFM
	SetA5(savedA5);
#endif
}


// ---------------------------------------------------------------------------
//	¥ ThreadTimerProc	[static]
// ---------------------------------------------------------------------------
//	Time Manager completion routine.  Converts the given Time Manager task
//	into a SThreadTMTask, then resumes the given thread.

pascal void
LThread::ThreadTimerProc(TMTaskPtr tmTaskPtr A1_PARAM)
{
	SThreadTMTask*	tpb = GetTimeMgrPtr(tmTaskPtr);

	// break if desired
	BREAK_INTO_DEBUGGER();

#if !TARGET_RT_MAC_CFM
	long savedA5 = SetA5(tpb->ioGlobals);
#endif

	ThreadAsynchronousResume(tpb->ioThread);

#if !TARGET_RT_MAC_CFM
	SetA5(savedA5);
#endif
}



// ===========================================================================
//	¥ Non-static member functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ SwapContext
// ---------------------------------------------------------------------------
//	Override this function to provide custom thread-swapping behavior

void
LThread::SwapContext(Boolean swappingIn)
{
	if (swappingIn) {
		// new current thread
		sThread = this;

		// if this thread was in ready state, adjust ready thread count
		if (mState == threadState_Ready) {
			MakeUnready();
		}

		// this thread is becoming the current thread
		mState = threadState_Current;

	} else {
		// If this thread is in the current state, it means the thread is
		// yielding (ie, going into the ready state).  So we have to adjust
		// the thread's state as well as the ready thread count.

		if (mState == threadState_Current) {
			MakeReady();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DumpAllThreads	[static]
// ---------------------------------------------------------------------------
//	Prints information about all of the known threads to stdout.

#if THREAD_DEBUG

void
LThread::DumpAllThreads()
{
	PP_STD::printf("\n %-3s %-9s  %5s %-10s",
				   "ID ",
				   "State",
				   "Flags",
				   "Wait Sem");

	LThread::DoForEach(DumpOneThread, NULL);

	PP_STD::printf("\nNumber of ready threads: %ld\n", (long) CountReadyThreads());
}


// ---------------------------------------------------------------------------
//	¥ Dump
// ---------------------------------------------------------------------------
//	Prints information about the thread to stdout.

void
LThread::Dump() const
{
	static char	*states[] = {
		"Current",
		"Ready",
		"Suspended",
		"Sleeping",
		"Waiting",
		"Blocked"
	};

	PP_STD::printf("\n#%-3lu %-9s  %c%c%c%-2c 0x%08.8p",
				   mThread,
				   states[mState],
				   (this == sMainThread) ? 'M' : ' ',
				   mPreemptive ? 'P' : 'C',
				   mRecycle ? 'R' : ' ',
				   mFPU ? 'F' : ' ',
				   mSemaphore);
}

#endif // THREAD_DEBUG


// ===========================================================================
//	¥ Utility functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ SetupAsynchronousResume
// ---------------------------------------------------------------------------
//	Set up the fields of a standard parameter block (SThreadParamBlk) so
//	that the thread may be resumed at interrupt time.

void
LThread::SetupAsynchronousResume(
	SThreadPBPtr	threadPB,
	IOCompletionUPP	callbackUPP)
{
	if (callbackUPP == NULL)
		callbackUPP = sThreadComplUPP;

	threadPB->ioThread						= mTimer.ioThread;
#if !TARGET_RT_MAC_CFM
	threadPB->ioGlobals						= mTimer.ioGlobals;
#endif
	threadPB->ioPB.F.ioParam.ioCompletion	= callbackUPP;
}


// ---------------------------------------------------------------------------
//	¥ SuspendUntilAsyncResume
// ---------------------------------------------------------------------------
//	Check the ioResult field of the given standard parameter block
//	(SThreadPBPtr);  if it's noErr, suspend the thread.

SInt16
LThread::SuspendUntilAsyncResume(SThreadPBPtr threadPB, SInt16 error)
{
	if (error == noErr)
	{
		Block();
		error = threadPB->ioPB.F.ioParam.ioResult;
	}

	return (error);
}


// ---------------------------------------------------------------------------
//	¥ SuspendUntilAsyncResume
// ---------------------------------------------------------------------------
//	Alternate form of I/O blocking.  This function will only block the thread
//	if the thread hasn't already completed.  This is useful in calls that
//	may complete before returning to their caller;  in that case a context
//	switch is avoided.

SInt16
LThread::SuspendUntilAsyncResume(SThreadPBPtr threadPB)
{
	SInt16	error	= threadPB->ioPB.F.ioParam.ioResult;

	if (error == 1)
	{
		Block();
		error = threadPB->ioPB.F.ioParam.ioResult;
	}
	else
	{
		RemoveTimeTask(mTimer);
		mAsyncCompleted = false;
	}

	return (error);
}


// ---------------------------------------------------------------------------
//	¥ ThreadAsynchronousResume	[static]
// ---------------------------------------------------------------------------
//	Resume a thread at interrupt time.

void
LThread::ThreadAsynchronousResume(LThread* thread)
{
	ThreadID	id	= thread->mThread;
	ThreadState	state;
	SInt16		err;

	THREAD_ASSERT(sInited == true);
	THREAD_ASSERT(thread != NULL);

	// get the thread's state
	err = ::GetThreadStateGivenTaskRef(sThreadTaskRef, id, &state);

	// could we get its state?
	if (err == noErr)
	{
		SThreadTMTask	&timer = thread->mTimer;

		// remove any time manager element from its queue
		RemoveTimeTask(timer);

		// is the thread stopped ?
		if (state == kStoppedThreadState)
		{
			err = ::SetThreadReadyGivenTaskRef(sThreadTaskRef, id);

			// wake up this process (so thread can get a chance to run)
			thread->mAsyncCompleted = false;
			::WakeUpProcess(&sPSN);
		}
		else
		{
			// add task to timer queue, then prime it
			thread->mAsyncCompleted = true;
			InsertTimeTask(timer);
			PrimeTimeTask(timer, kThreadWakeupDelay);
		}
	}
	else
	{
		// Else, there's not much we can do.  Perhaps the thread was killed
		// while an asynchronous I/O operation was pending (ick).
	}
}


#if THREAD_DEBUG

// ---------------------------------------------------------------------------
//	¥ ThreadAssert
// ---------------------------------------------------------------------------
//	Called in response to an assertion failure.  Prints a message in
//	the debugger, then quits the application.

int		ThreadAssert(char* test, char* file, int line)
{
	extern char	*ThreadStrCat(const char* src, char* dest);

	static char	buff[256];
	char		*buffp	= buff + 1;
	long		base, digit;

	buffp = ThreadStrCat("Thread assert: ",	buffp);
	buffp = ThreadStrCat(test,				buffp);
	buffp = ThreadStrCat(", file ",			buffp);
	buffp = ThreadStrCat(file,				buffp);
	buffp = ThreadStrCat(", line ",			buffp);

	// convert line number
	if (line == 0)
		*buffp++ = '0';
	else
	{
		for (base = 100000; base >= 1; base /= 10)
		{
			digit = line / base;
			*buffp++ = (char) (digit + '0');
			line -= (int) (digit * base);
		}
	}

	buff[0]	= (char) (buffp - buff - 1);

	::DebugStr((StringPtr) buff);

	::ExitToShell();

	return (0);
}


static char*
ThreadStrCat(const char* src, char* dest)
{
	while (*src != '\0')
	{
		*dest++ = *src++;
	}

	return (dest);
}


// ---------------------------------------------------------------------------
//	¥ DumpOneThread
// ---------------------------------------------------------------------------

static void
DumpOneThread(LThread& thread, void */* arg */)
{
	thread.Dump();
}

#endif	// THREAD_DEBUG

PP_End_Namespace_PowerPlant
