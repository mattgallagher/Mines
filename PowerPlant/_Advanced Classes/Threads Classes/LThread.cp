// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LThread.cp					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LThread.h>
#include <LSemaphore.h>
#include <UDebugging.h>
#include <UEnvironment.h>
#include <UException.h>

#include <MacErrors.h>
#include <Gestalt.h>
#include <LowMem.h>
#include <Power.h>
#include <Processes.h>
#include <Threads.h>

#if TARGET_RT_MAC_CFM
	#include <CodeFragments.h>
#endif

#if THREAD_PROFILE
	#include <profiler.h>
#endif


// ===========================================================================
//	The code in this module may be invoked at interrupt time, or called
//	from the profiler, and should therefore not be profiled.
// ===========================================================================

#ifdef __MWERKS__
#pragma profile off
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ Conditional compilation
// ===========================================================================


// debugging aids
#define BREAK_ON_YIELD		0		// non-zero to break on Yield
#define BREAK_ON_SUSPEND	0		// non-zero to break on Suspend
#define BREAK_ON_SLEEP		0		// non-zero to break on Sleep
#define BREAK_ON_WAIT		0		// non-zero to break on Wait
#define BREAK_ON_BLOCK		0		// non-zero to break on Block


// ===========================================================================
//	¥ Constants
// ===========================================================================


// stack size for disposal thread
#if TARGET_CPU_PPC
	const UInt32		kDisposalThreadStack	= 5000;
#else
	const UInt32		kDisposalThreadStack	= 2500;
#endif

const ThreadOptions	kThreadOptionsMask		= kNewSuspend | kUsePremadeThread |
											  kCreateIfNeeded | kFPUNotNeeded |
											  kExactMatchThread;

const SInt32		kInactivityInterval		= 5 * 60;	// 5 seconds


// ===========================================================================
//	¥ LThread::Init											   LThread::Init ¥
// ===========================================================================

//	For Thread Mgr callbacks,
//		Carbon			use UPPs
//		CFM-68K			use UPPs
//		Classic 68K		use function ptrs
//		Classic PPC		use function ptrs

class LThread::Init
{
	// constructors / destructors
									Init();
									~Init();
	// static member variables
	static ThreadEntryTPP		sDoEntryUPP;
	static ThreadSwitchTPP		sDoSwapInUPP;
	static ThreadSwitchTPP		sDoSwapOutUPP;
#if THREAD_DEBUG
	static ThreadTerminationTPP	sDoExitUPP;
#endif
	// friends
	friend class					LThread;
};


// static member variables
ThreadEntryTPP			LThread::Init::sDoEntryUPP		= NewThreadEntryUPP(LThread::DoEntry);
ThreadSwitchTPP			LThread::Init::sDoSwapInUPP		= NewThreadSwitchUPP(LThread::DoSwapIn);
ThreadSwitchTPP			LThread::Init::sDoSwapOutUPP	= NewThreadSwitchUPP(LThread::DoSwapOut);
#if THREAD_DEBUG
ThreadTerminationTPP	LThread::Init::sDoExitUPP		= NewThreadTerminationUPP(LThread::DoExit);
#endif


// ---------------------------------------------------------------------------
//	¥ Init
// ---------------------------------------------------------------------------
//	Constructor.  Performs low-level initialisation for the Thread Class
//	Library (essentially, checking if the Thread Manager is available and
//	setting the proper flag in UEnvironment accordingly).

LThread::Init::Init()
{
	SInt32	gestaltResult, gestaltMask;

	gestaltMask	 = (1 << gestaltThreadMgrPresent);
	gestaltMask	|= (1 << gestaltSpecificMatchSupport);

	// On PowerMacs, we'll need the native (shared library)
	// version of the Thread Manager.  Also, the check we're
	// about to do will let the user do a "weak import" of
	// the Thread Manager's shared library.
#if TARGET_CPU_PPC
	gestaltMask	|= (1 << gestaltThreadsLibraryPresent);
#endif

	// is the Thread Manager running ?
	if ((::Gestalt(gestaltThreadMgrAttr, &gestaltResult) == noErr) &&
		((gestaltResult & gestaltMask) == gestaltMask) &&
		CFM_AddressIsResolved_(::NewThread))
	{
		// we're all set!
		UEnvironment::SetFeature(env_HasThreadsManager, true);

		// low-level initialisation
		::MacGetCurrentProcess(&LThread::sPSN);
		::GetThreadCurrentTaskRef(&LThread::sThreadTaskRef);

#if THREAD_INACTIVITY
		// check for Power Manager
		gestaltMask = (1 << gestaltPMgrCPUIdle);
		if ((Gestalt(gestaltPowerMgrAttr, &gestaltResult) == noErr) &&
			((gestaltResult & gestaltMask) == gestaltMask))
		{
			LThread::sHasIdleUpdate = true;
		}
#endif
	}
}


// ---------------------------------------------------------------------------
//	¥ ~Init
// ---------------------------------------------------------------------------
//	Destructor.  If the Threads Class Library was actually used, this cleans
//	up after it.

LThread::Init::~Init()
{
	try {
		if (LThread::sInited) {
			LThread::ExitThreads();
		}
	}

	catch (...) { }			// just prevent the exception from propagating
}


// ===========================================================================
//	¥ LThread::Cleanup										LThread::Cleanup ¥
// ===========================================================================


class LThread::Cleanup : public LThread
{
private:
					Cleanup(QHdr volatile& inQueue, LSemaphore& inSem);
	QHdr volatile&	mQueue;
	LSemaphore&		mSem;
	friend class	LThread;
protected:
	virtual void*	Run();
};


// ---------------------------------------------------------------------------
//	¥ Cleanup
// ---------------------------------------------------------------------------
//	Constructor

LThread::Cleanup::Cleanup(QHdr volatile& inQueue, LSemaphore& inSem)
	: LThread(false, kDisposalThreadStack), mQueue(inQueue), mSem(inSem)
{
	// there's nothing else to do ...
}


// ---------------------------------------------------------------------------
//	¥ Run
// ---------------------------------------------------------------------------
//	Thread function executed by the cleanup thread.
//
//	This function sits in a loop, waiting for an element (i.e., a thread)
//	to appear in the cleanup queue.  When one appears, it is deleted.

void*
LThread::Cleanup::Run()
{
	LThread	*thread;

	while (true)
	{
		// wait until a thread needs to be deallocated
		mSem.Wait();

		thread = RemoveFromGlobalQueue(mQueue.qHead, mQueue);

		THREAD_ASSERT(thread != NULL);

		// release thread object's memory
		delete [] (char *) thread;
	}

	return (NULL);
}


// ===========================================================================
//	¥ LThread														 LThread ¥
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ Static member variables
// ---------------------------------------------------------------------------


Boolean				LThread::sInited;
LThread* volatile	LThread::sThread;
LThread* volatile	LThread::sMainThread;
LThread*			LThread::sCleanupThread;
QHdr volatile		LThread::sThreadQueue;
QHdr volatile		LThread::sCleanupQueue;
LSemaphore*			LThread::sCleanupSem;
void*				LThread::sThreadTaskRef;
ProcessSerialNumber	LThread::sPSN;
IOCompletionUPP		LThread::sThreadComplUPP	= NewIOCompletionUPP((IOCompletionProcPtr) &LThread::ThreadComplProc);
TimerUPP			LThread::sThreadTimerUPP	= NewTimerUPP((TimerProcPtr) &LThread::ThreadTimerProc);
SInt16 volatile		LThread::sReadyThreadCount;
#if THREAD_INACTIVITY
Boolean				LThread::sHasIdleUpdate;
SInt32				LThread::sNextUpdateTicks;
#endif
#if !TARGET_RT_MAC_CFM
long				LThread::sGlobals	= SetCurrentA5();
#endif
LThread::Init		LThread::sThreadInit;


// ---------------------------------------------------------------------------
//	¥ Local variables
// ---------------------------------------------------------------------------


#if BREAK_ON_YIELD
static Boolean		sBreakBeforeYield	= true;
static Boolean		sBreakAfterYield	= true;
#endif


// ---------------------------------------------------------------------------
//	¥ EnterThreads	[static]
// ---------------------------------------------------------------------------

void
LThread::EnterThreads()
{
	try {

		// make queue for releasing storage of preemptive threads
		sCleanupSem = new LSemaphore;

		// we are now officially initialized
		sInited = true;

		// make thread that releases the storage of preemptive threads
		sCleanupThread = new LThread::Cleanup(sCleanupQueue, *sCleanupSem);

		// start the cleanup thread
		sCleanupThread->Resume();
	}

	catch (...) {						// Initialization failed

		if (sCleanupThread != NULL) {
			sCleanupThread->DeleteThread();
			sCleanupThread = NULL;
		}

		delete sCleanupSem;
		sCleanupSem = NULL;

		sInited = false;				// We didn't get init'ed after all

		throw;							// Rethrow exception
	}
}


// ---------------------------------------------------------------------------
//	¥ ExitThreads	[static]
// ---------------------------------------------------------------------------
//	Cleans up the threads class library.  If the LThread::EnterThreads()
//	returned successfully, this function gets called automatically when
//	your application quits.

void
LThread::ExitThreads()
{
	LThread	*thread;

//	::DebugStr("\pEntering ExitThreads");

	if (!sInited)
		return;

	// you should only call this from the main thread
	THREAD_ASSERT(sThread == sMainThread);

	// remove main thread from the list of all known threads
	RemoveFromGlobalQueue(reinterpret_cast<QElemPtr>(&sMainThread->mThreadLink), sThreadQueue);
#if THREAD_DEBUG
	// we don't need thread term proc anymore
	::SetThreadTerminator(sMainThread->mThread, NULL, NULL);
#endif

	// dispose of all known threads
	while (sThreadQueue.qHead != NULL)
	{
		thread = RemoveFromGlobalQueue(sThreadQueue.qHead, sThreadQueue);

		THREAD_ASSERT(thread != NULL);

		thread->DeleteThread();
	}

	// delete main thread
	LThread*	oldMain = sMainThread;

	sMainThread = NULL;
	sThread     = NULL;

	delete oldMain;

	delete sCleanupSem;
	sCleanupSem = NULL;

	sInited = false;
}


// ---------------------------------------------------------------------------
//	¥ AllocateThreads	[static]
// ---------------------------------------------------------------------------
//	Pre-allocate some (Thread Manager) threads.

void
LThread::AllocateThreads(
	Boolean	inPreemptive,
	UInt16	inNumThreads,
	UInt32	inStackSize)
{
	ThreadStyle	style;

	THREAD_ASSERT(!InPreemptiveThread());

	style = (inPreemptive ? kPreemptiveThread : kCooperativeThread);

	if (inNumThreads > 0)
	{
		ThrowIfOSErr_( ::CreateThreadPool(style, (SInt16) inNumThreads, (Size) inStackSize));
	}
}


// ---------------------------------------------------------------------------
//	¥ GetFreeThreads	[static]
// ---------------------------------------------------------------------------
//	Count the number of unused pre-allocated (Thread Manager) threads.

void
LThread::GetFreeThreads(
	Boolean	inPreemptive,
	UInt16&	outNumThreads,
	UInt32	inStackSize)
{
	ThreadStyle	style;
	SInt16		count;

	style = (inPreemptive ? kPreemptiveThread : kCooperativeThread);

	if (inStackSize == thread_DefaultStack)
	{
		ThrowIfOSErr_(::GetFreeThreadCount(style, &count));
	}
	else
	{
		ThrowIfOSErr_(::GetSpecificFreeThreadCount(style, (Size) inStackSize, &count));
	}

	outNumThreads = (UInt16) count;
}


// ---------------------------------------------------------------------------
//	¥ GetThreadManagerTaskRef	[static]
// ---------------------------------------------------------------------------
//	Return the Thread Manager's reference number for this application.

void*
LThread::GetThreadManagerTaskRef()
{
	return (sThreadTaskRef);
}


// ---------------------------------------------------------------------------
//	¥ Yield	[static]
// ---------------------------------------------------------------------------
//	Give up control of the CPU.

void
LThread::Yield(const LThread* yieldTo)
{
	// Return safely if we haven't been initialised.  This means that
	// applications can call this function even if the Thread Manager isn't
	// available.

	if (not sInited) return;

#if BREAK_ON_YIELD
	if (sBreakBeforeYield)	UDebugging::DebugString(StringLiteral_("About to yield"));
#endif

	// transfer control to some other needy thread
	ThreadID theID = (yieldTo == NULL) ? kNoThreadID : yieldTo->mThread;
	::YieldToThread(theID);

#if BREAK_ON_YIELD
	if (sBreakAfterYield)	UDebugging::DebugString(StringLiteral_("Just yielded"));
#endif

	if (sThread != NULL) {

		// in case something bad happened while we weren't current
		ExceptionCode err = sThread->mError;
		sThread->mError = noErr;
		ThrowIfError_(err);
	}

#if THREAD_INACTIVITY
	// prevent the Power Mgr from slowing down the CPU when there isn't any
	// user activity or I/O.
	if (sThread != sMainThread)
	{
		PreventInactivity();
	}
#endif
}


// ---------------------------------------------------------------------------
//	¥ PreventInactivity	[static]
// ---------------------------------------------------------------------------
//	Prevent the Power Manager from slowing down the CPU when there isn't any
//	user activity or I/O.

#if THREAD_INACTIVITY

void
LThread::PreventInactivity()
{
	if (sHasIdleUpdate)
	{
		SInt32	currTicks	= ::TickCount();

		if (sNextUpdateTicks < currTicks)
		{
			sNextUpdateTicks = currTicks + kInactivityInterval;
			::IdleUpdate();
		}
	}
}

#endif	// THREAD_INACTIVITY


// ---------------------------------------------------------------------------
//	¥ InPreemptiveThread	[static]
// ---------------------------------------------------------------------------
//	Returns whether or not the current thread is a preemptive thread.

Boolean
LThread::InPreemptiveThread()
{
	LThread	*thread	= GetCurrentThread();
	Boolean	preemptive;

	if (thread != NULL)
		preemptive = thread->IsPreemptive();
	else
		preemptive = false;

	return (preemptive);
}


// ---------------------------------------------------------------------------
//	¥ StateLinkToThread	[static]
// ---------------------------------------------------------------------------
//	Utility routine.  Given a pointer to a thread's mStateLink field, returns
//	a pointer to the thread itself.

LThread*
LThread::StateLinkToThread(QElemPtr inQElem)
{
	PP_Using_Namespace_Std	// Pro 3's MSL offsetof macro isn't
							// namespace std savvy.

	return (reinterpret_cast<LThread *>(
			reinterpret_cast<char *>(inQElem) -
			offsetof(LThread, mStateLink)));
}


// ---------------------------------------------------------------------------
//	¥ AddToGlobalQueue	[static]
// ---------------------------------------------------------------------------
//	Adds a thread to the global linked list of threads.

void
LThread::AddToGlobalQueue(LThread &inThread, QHdr volatile& ioQueue)
{
	::Enqueue(reinterpret_cast<QElemPtr>(&inThread.mThreadLink),
				(QHdrPtr) &ioQueue);
	++ioQueue.qFlags;
}


// ---------------------------------------------------------------------------
//	¥ RemoveFromGlobalQueue	[static]
// ---------------------------------------------------------------------------
//	Removes a thread from the global linked list of threads.

LThread*
LThread::RemoveFromGlobalQueue(QElemPtr inQElem, QHdr volatile& ioQueue)
{
	LThread	*thread;

	if (::Dequeue(inQElem, (QHdrPtr) &ioQueue) == noErr)
	{
		PP_Using_Namespace_Std	// Pro 3's MSL offsetof macro isn't
								// namespace std savvy.

		thread = reinterpret_cast<LThread *>(
					reinterpret_cast<char *>(inQElem) -
					offsetof(LThread, mThreadLink));
		--ioQueue.qFlags;
	}
	else
		thread = NULL;

	return (thread);
}


// ---------------------------------------------------------------------------
//	¥ AddToStateQueue	[static]
// ---------------------------------------------------------------------------
//	Adds a thread to a linked list, linking through the mStateLink field.

void
LThread::AddToStateQueue(LThread &inThread, QHdr& ioQueue)
{
	::Enqueue(reinterpret_cast<QElemPtr>(&inThread.mStateLink), &ioQueue);
}


// ---------------------------------------------------------------------------
//	¥ RemoveFromStateQueue	[static]
// ---------------------------------------------------------------------------
//	Removes a thread from a linked list, linking through the mStateLink field.

LThread*
LThread::RemoveFromStateQueue(QElemPtr inQElem, QHdr& ioQueue)
{
	LThread	*thread;

	if (::Dequeue(inQElem, &ioQueue) == noErr)
	{
		PP_Using_Namespace_Std	// Pro 3's MSL offsetof macro isn't
								// namespace std savvy.

		thread = reinterpret_cast<LThread *>(
					reinterpret_cast<char *>(inQElem) -
					offsetof(LThread, mStateLink));
		thread->mStateLink = NULL;
	}
	else
	{
		thread = NULL;
	}

	return (thread);
}


// ---------------------------------------------------------------------------
//	¥ DoForEach
// ---------------------------------------------------------------------------
//	Execute a user-supplied function for each thread.

void
LThread::DoForEach(LThreadIterator proc, void* arg)
{
	QHdr	tempQueue	= { 0, NULL, NULL };
	LThread	*thread;

	// take each thread off of the global queue & put it on the temp queue
	// before calling the iterator
	while (sThreadQueue.qHead != NULL)
	{
		thread = RemoveFromGlobalQueue(sThreadQueue.qHead, sThreadQueue);

		if (thread != NULL)
		{
			AddToGlobalQueue(*thread, tempQueue);

			(*proc)(*thread, arg);
		}

	}

	// move the threads back onto the global queue
	while (tempQueue.qHead != NULL)
	{
		thread = RemoveFromGlobalQueue(tempQueue.qHead, tempQueue);
		AddToGlobalQueue(*thread, sThreadQueue);
	}
}


// ---------------------------------------------------------------------------
//	¥ EnterCritical	[static]
// ---------------------------------------------------------------------------
//	Enter a critical section.

void
LThread::EnterCritical()
{
	::ThreadBeginCritical();
}


// ---------------------------------------------------------------------------
//	¥ ExitCritical	[static]
// ---------------------------------------------------------------------------
//	Leave a critical section.

void
LThread::ExitCritical()
{
	::ThreadEndCritical();
}


#if !TARGET_RT_MAC_MACHO
// ---------------------------------------------------------------------------
//	¥ operator new	[static]
// ---------------------------------------------------------------------------
//	Performs some sanity checks, then calls default operator new.

void*
LThread::operator new(PP_STD::size_t size)
{
	// we can't allocate memory while we're in a preemptive thread
	THREAD_ASSERT(!InPreemptiveThread());

	// Note this bizarre memory allocation.  This is used so we can hand off
	// the memory to the cleanup thread if need be (see operator delete).

	return (reinterpret_cast<void *>(new char[size]));
}


// ---------------------------------------------------------------------------
//	¥ operator new	[static]
// ---------------------------------------------------------------------------
//	Placement form of operator new.  You can use this to "pre-allocate" the
//	memory for a thread object.

void*
LThread::operator new(PP_STD::size_t /* size */, void* memptr)
{
	return (memptr);
}


// ---------------------------------------------------------------------------
//	¥ operator delete	[static]
// ---------------------------------------------------------------------------

void
LThread::operator delete(void* ptr)
{
	LThread	*thread	= (LThread *) ptr;

	// are we in a preemptive thread ?
	if (!InPreemptiveThread())
	{
		// no -- so we can delete the object's memory normally.

		delete [] reinterpret_cast<char *>(ptr);
	}
	else
	{
		// We're in a preemptive thread, so we can't release the object's
		// memory just yet.  What we do is we hand it off to the cleanup
		// thread, which is a cooperative thread whose whole purpose in
		// life is to service our requests.

		// What we're doing here is pretty skanky -- namely, passing a
		// reference to a *deconstructed* object.  If this breaks at one
		// point, we'll have to enqueue the thread onto the disposal
		// queue ourselves.

		AddToGlobalQueue(*thread, sCleanupQueue);

		sCleanupSem->Signal();
	}
}
#endif // !TARGET_RT_MAC_MACHO


// ===========================================================================
//	¥ Non-static member functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ LThread()
// ---------------------------------------------------------------------------
//	Constructor.  Completely initialises a thread object.
//
//	This function takes four arguments:
//
//		inPreemptive	- Indicates if the thread should be preemptive or not.
//		inStacksize		- The thread's stack size.  If it is thread_DefaultStack,
//						  the Thread Manager's default stack size is used.
//		inFlags			- Flags controlling the creation and behavior of the
//						  thread.
//		outResult		- location to stuff with thread result;  may be NULL.

LThread::LThread(
	Boolean					inPreemptive,
	UInt32					inStacksize,
	LThread::EThreadOption	inFlags,
	void					**outResult)
{
	ThreadStyle		style;
	ThreadOptions	options;

	// don't even think of creating a thread if the Thread Mgr isn't available
	Assert_(UEnvironment::HasFeature(env_HasThreadsManager));

	if (inFlags & threadOption_Main)
	{
		// The user is creating the main thread.

		// This thread is handled specially, since it already exists when the
		// application starts executing.

		// we can't have a preemptive main thread, nor two main threads!
		if (inPreemptive || (sMainThread != NULL) || sInited)
			Throw_(threadProtocolErr);

		// set up this thread to be the main thread
		CreateMainThread();

		// create the cleanup thread
		EnterThreads();

		return;
	}
	else if (!sInited)
	{
		// the user is trying to create a first thread that isn't the main thread
		Throw_(threadProtocolErr);
	}

	// init member variables
	mThreadLink		= NULL;
	mStateLink		= NULL;
	mThread			= kNoThreadID;
	mNextOfKin		= NULL;
	mResult			= NULL;
	mError			= noErr;
	mState			= threadState_Suspended;
	mPreemptive		= inPreemptive;
	mFPU			= !(inFlags & threadOption_NoFPU);
	mAsyncCompleted	= false;
	mSemaphore		= NULL;
#if THREAD_PROFILE
	mProfilerRef	= 0;
#endif

	if (outResult != NULL)
		*outResult = NULL;

	// setup timer info
	mTimer.ioThread				= this;
#if !TARGET_RT_MAC_CFM
	mTimer.ioGlobals			= sGlobals;
#endif
	mTimer.ioTask.qLink			= NULL;
	mTimer.ioTask.qType			= 0;
	mTimer.ioTask.tmAddr		= sThreadTimerUPP;
	mTimer.ioTask.tmCount		= 0;
	mTimer.ioTask.tmWakeUp		= 0;
	mTimer.ioTask.tmReserved	= 0;
	mTimer.ioSemQ.qFlags		= 0;
	mTimer.ioSemQ.qHead			= NULL;
	mTimer.ioSemQ.qTail			= NULL;
	mTimer.ioSemEl.qLink		= NULL;
	mTimer.ioSemEl.qType		= 0;

	// set up thread style
	style = inPreemptive ? kPreemptiveThread : kCooperativeThread;

	// set up thread options
	options = ((inFlags | kNewSuspend) & kThreadOptionsMask);

	// if we're in a preemptive thread, force the Thread Manager to
	// use a preallocated thread.
	if (InPreemptiveThread())
	{
		options |= kUsePremadeThread;
	}

	// recycle thread only if we MUST allocate from thread pool
	mRecycle = ((options & kUsePremadeThread) && !(options & kCreateIfNeeded));

	try
	{
		// allocate thread
		ThrowIfOSErr_(::NewThread(style, Init::sDoEntryUPP,
				reinterpret_cast<void *>(this), (Size) inStacksize, options,
				outResult, &mThread));

		// set up termination, switch-in, & switch-out glue callbacks
		ThrowIfOSErr_(::SetThreadSwitcher(mThread, Init::sDoSwapInUPP,
				reinterpret_cast<void *>(this), true));
		ThrowIfOSErr_(::SetThreadSwitcher(mThread, Init::sDoSwapOutUPP,
				reinterpret_cast<void *>(this), false));
#if THREAD_DEBUG
		ThrowIfOSErr_(::SetThreadTerminator(mThread, Init::sDoExitUPP,
				reinterpret_cast<void *>(this)));
#endif
#if THREAD_PROFILE
		// ProfilerCreateThread only works with cooperative threads
		UInt32	stackSize;
		Assert_(style == kCooperativeThread);
		ThrowIfOSErr_(::ThreadCurrentStackSpace(mThread, &stackSize));
		ThrowIfOSErr_(::ProfilerCreateThread(stackSize / 64, stackSize, &mProfilerRef));
#endif
		// add thread to the queue of known threads
		AddToGlobalQueue(*this, sThreadQueue);
	}

	catch(...)
	{
		Boolean	recycle;

		RemoveFromGlobalQueue(reinterpret_cast<QElemPtr>(&mThreadLink), sThreadQueue);

		if (mThread != kNoThreadID)
		{
			// recycle thread if we got it from the pool to begin with
			recycle = ((options & kUsePremadeThread) != 0);

			::SetThreadTerminator(mThread, NULL, NULL);
			::DisposeThread(mThread, NULL, recycle);
			mThread = kNoThreadID;
		}

		// transfer to enclosing error handler
		throw;

	}
}


// ---------------------------------------------------------------------------
//	¥ ~LThread
// ---------------------------------------------------------------------------
//	Destructor.  Doesn't do much, really, since all the work is done in
//	DeleteThread and operator delete.

LThread::~LThread()
{
	SignalIf_(this == sMainThread);		// Very bad to delete the main thread
}


// ---------------------------------------------------------------------------
//	¥ CreateMainThread
// ---------------------------------------------------------------------------
//	Performs special initialisation for the main thread.

void
LThread::CreateMainThread()
{
	// init member variables
	mThreadLink		= NULL;
	mStateLink		= NULL;
	mNextOfKin		= NULL;
	mResult			= NULL;
	mError			= noErr;
	mState			= threadState_Current;
	mPreemptive		= false;
	mFPU			= false;
	mRecycle		= false;
	mSemaphore		= NULL;
	mAsyncCompleted	= false;

	// setup timer info
	mTimer.ioThread				= this;
#if !TARGET_RT_MAC_CFM
	mTimer.ioGlobals			= sGlobals;
#endif
	mTimer.ioTask.qLink			= NULL;
	mTimer.ioTask.qType			= 0;
	mTimer.ioTask.tmAddr		= sThreadTimerUPP;
	mTimer.ioTask.tmCount		= 0;
	mTimer.ioTask.tmWakeUp		= 0;
	mTimer.ioTask.tmReserved	= 0;
	mTimer.ioSemQ.qFlags		= 0;
	mTimer.ioSemQ.qHead			= NULL;
	mTimer.ioSemQ.qTail			= NULL;
	mTimer.ioSemEl.qLink		= NULL;
	mTimer.ioSemEl.qType		= 0;

	// the thread ID to use is the current one
	::MacGetCurrentThread(&mThread);

	// set up termination, switch-in, & switch-out glue callbacks
	::SetThreadSwitcher(mThread, Init::sDoSwapInUPP,
			reinterpret_cast<void *>(this), true);
	::SetThreadSwitcher(mThread, Init::sDoSwapOutUPP,
			reinterpret_cast<void *>(this), false);
#if THREAD_DEBUG
	::SetThreadTerminator(mThread, Init::sDoExitUPP,
			reinterpret_cast<void *>(this));
#endif
#if THREAD_PROFILE
	mProfilerRef = ::ProfilerGetMainThreadRef();
#endif

	// add thread to the queue of known threads
	AddToGlobalQueue(*this, sThreadQueue);

	sThread = sMainThread = this;
}

// ---------------------------------------------------------------------------
//	¥ DeleteThread
// ---------------------------------------------------------------------------
//	Thread disposal function.
//
//	You must call this function to delete a Thread. You can't use
//	operator delete.
//
//	Note that under some circumstances, this call might not immediately
//	delete the Thread.

void
LThread::DeleteThread(void* inResult)
{
	SetResult(inResult);

	// remove ourselves from the thread queue
	RemoveFromGlobalQueue(reinterpret_cast<QElemPtr>(&mThreadLink), sThreadQueue);

	// enter critical section
	::ThreadBeginCritical();

	if (mState == threadState_Blocked)
	{
		// The thread is waiting for async I/O to complete, so we can't kill
		// it right away.  Instead, we stuff an error code into the object
		// that will cause the thread to delete itself once it's unblocked.
		mError = errKilledThread;
		::ThreadEndCritical();
		return;
	}

	// remove any leftover timer
	RemoveTimeTask(mTimer);

	if (mState == threadState_Ready)
	{
		// this is one less ready thread to deal with
		MakeUnready();
	}
	else if (mState == threadState_Waiting)
	{
		// the thread is waiting on a semaphore, so
		// remove it from the sem's queue
		THREAD_ASSERT(mSemaphore != NULL);
		mSemaphore->UnblockThread(reinterpret_cast<QElemPtr>(&this->mStateLink), errKilledThread);
	}

	ThreadID	savedID			= mThread;
	Boolean		savedRecycle	= mRecycle;
	void		*savedResult	= mResult;

#if THREAD_PROFILE
	::ProfilerDeleteThread(mProfilerRef);
#endif

	// exit critical section
	::ThreadEndCritical();

	// notify next of kin
	if (mNextOfKin != NULL)
		mNextOfKin->ThreadDied(*this);

	// release the thread object
	
	if (sThread == this) {
		sThread = NULL;
	}
	
	delete this;

#if THREAD_DEBUG
	// we don't need thread term proc anymore
	::SetThreadTerminator(savedID, NULL, NULL);
#endif

	// kill the thread
	::DisposeThread(savedID, savedResult, savedRecycle);

	Yield();
}


// ---------------------------------------------------------------------------
//	¥ ThreadDied
// ---------------------------------------------------------------------------

void
LThread::ThreadDied(const LThread& /* inThread */)
{
	// default behaviour:  do nothing
}


// ---------------------------------------------------------------------------
//	¥ Run
// ---------------------------------------------------------------------------
//	Execute a thread.
//
//	This is a pure virtual function in LThread, so you MUST override it in
//	your derived classes.  It is implemented here only as a debugging aid.

void*
LThread::Run()
{
	UDebugging::DebugString(StringLiteral_("Entering LThread::Run() -- this should never happpen"));

	return (NULL);
}


// ---------------------------------------------------------------------------
//	¥ Suspend
// ---------------------------------------------------------------------------
//	Make a thread ineligible for CPU time.
//
//	A suspended thread can only regain control of the CPU if another thread
//	resumes it first.

void
LThread::Suspend()
{
	Boolean			selfSuspend;
	ExceptionCode	err;

	try
	{
#if BREAK_ON_SUSPEND
		UDebugging::DebugString(StringLiteral_("About to suspend thread"));
#endif

		// enter critical section
		::ThreadBeginCritical();

		// can't suspend a thread that's sleeping or waiting or already suspended
		if ((mState != threadState_Current) && (mState != threadState_Ready))
		{
			Throw_(errBadThreadState);
		}

		// change thread state
		selfSuspend	= (mState == threadState_Current);
		mState		= threadState_Suspended;

		// If we're not suspending ourselves, it means the thread used to be in
		// the ready state.  So we have to bump down the ready thread count.
		if (!selfSuspend)
			MakeUnready();

		// end critical section by stopping thread at OS level
		err = ::SetThreadStateEndCritical(mThread, kStoppedThreadState, kNoThreadID);

		// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
		THREAD_ASSERT(err == noErr);

#if BREAK_ON_SUSPEND
		UDebugging::DebugString(StringLiteral_("Thread now resumed"));
#endif
	}

	catch (...)
	{
		// exit critical section before leaving function
		::ThreadEndCritical();

		// transfer to enclosing error handler
		throw;

	}

	// in case something bad happened while we were sleeping
	if (selfSuspend)
	{
		err		= mError;
		mError	= noErr;

		ThrowIfError_(err);

#if THREAD_INACTIVITY
		PreventInactivity();
#endif
	}
	else
		Yield();
}


// ---------------------------------------------------------------------------
//	¥ Resume
// ---------------------------------------------------------------------------
//	Make a suspended thread eligible for CPU time once more.

void
LThread::Resume()
{
	SInt16	err;

	// a thread can't resume itself !
	THREAD_ASSERT(sThread != this);

	try
	{
		// enter critical section
		::ThreadBeginCritical();

		// can't resume a thread that's not suspended
		if (mState != threadState_Suspended)
		{
			Throw_(errBadThreadState);
		}

		// adjust ready thread count accordingly
		MakeReady();

		// end critical section by resuming thread at OS level
		err = ::SetThreadStateEndCritical(mThread, kReadyThreadState, kNoThreadID);

		// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
		THREAD_ASSERT(err == noErr);

		Yield();

	}

	catch (...)
	{
		// exit critical section before leaving function
		::ThreadEndCritical();

		// transfer to enclosing error handler
		throw;

	}
}


// ---------------------------------------------------------------------------
//	¥ Block
// ---------------------------------------------------------------------------
//	Make a thread ineligible for CPU time.  Use this function after an
//	asynchronous I/O call.
//
//	The thread can only regain control of the CPU once the async call
//	completes.

void
LThread::Block()
{
	ExceptionCode	err;

	// a thread can only block itself
	THREAD_ASSERT(this == sThread);

	// If the async operation we're attempting has actually completed
	// already, just return.  This situation can occur, for example, when
	// a so-called async call executes synchronously.  We prevent a
	// needless (and potentially expensive) context switch this way.

	if (mAsyncCompleted)
	{
		mAsyncCompleted = false;

		RemoveTimeTask(mTimer);

		err		= mError;
		mError	= noErr;
		ThrowIfOSErr_((OSErr) err);

#if THREAD_INACTIVITY
		PreventInactivity();
#endif
		return;
	}

	// Normal case:  the async operation hasn't completed yet.

	// enter critical section
	::ThreadBeginCritical();

#if BREAK_ON_BLOCK
	UDebugging::DebugString(StringLiteral_("About to block thread"));
#endif

	// change thread state
	mState = threadState_Blocked;

	// End critical section by stopping thread at OS level.
	//
	// Because we're stopping the current thread, rescheduling
	// will have to occur.  Therefore, several error conditions
	// may arise:
	//
	// 1.	The current thread's critical depth isn't 1.  This
	//		means that we're either in a nested critical section
	//		(ThreadBeginCritical was called more that once), or
	//		we're not in a critical section at all.
	//
	// 2.	There are no other threads left to run.
	//
	// All of these situations are programming errors.

	err = ::SetThreadStateEndCritical(mThread, kStoppedThreadState, kNoThreadID);

	// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
	THREAD_ASSERT(err == noErr);

	mAsyncCompleted = false;

#if BREAK_ON_BLOCK
	UDebugging::DebugString(StringLiteral_("Thread now unblocked"));
#endif

	if (mError == errKilledThread)
	{
		// We were killed while waiting for the I/O
		// to complete.  Finish the job.
		DeleteThread(mResult);
	}
	else
	{
		err		= mError;
		mError	= noErr;

		ThrowIfError_(err);

#if THREAD_INACTIVITY
		PreventInactivity();
#endif
	}
}


// ---------------------------------------------------------------------------
//	¥ Sleep
// ---------------------------------------------------------------------------
//	Make a thread ineligible for CPU time for a certain time interval.
//
//	A sleeping thread can only regain control of the CPU when its sleep
//	time expires, or if another thread wakes it first.

void
LThread::Sleep(SInt32 milliSeconds)
{
	ExceptionCode	err;
	Boolean			selfSleep;

	// we don't like negative sleep intervals
	THREAD_ASSERT(milliSeconds >= 0);

	// interval is zero -- don't sleep, just blink !
	if (milliSeconds == 0)
	{
		Yield();
		return;
	}

	try
	{
#if BREAK_ON_SLEEP
		UDebugging::DebugString(StringLiteral_("About to put thread to sleep"));
#endif

		// enter critical section
		::ThreadBeginCritical();

		// can't put to sleep a thread that's suspended or waiting or already sleeping
		if ((mState != threadState_Current) && (mState != threadState_Ready))
		{
			Throw_(errBadThreadState);
		}

		// set up timer
		RemoveTimeTask(mTimer);
		InsertTimeTask(mTimer);

		// change thread state
		selfSleep	= (mState == threadState_Current);
		mState		= threadState_Sleeping;

		// If we're not putting ourselves to sleep, it means the thread used to be
		// in the ready state.  So we have to bump down the ready thread count.
		if (!selfSleep)
			MakeUnready();

		// start countdown
		PrimeTimeTask(mTimer, milliSeconds);

		// end critical section by suspending thread at OS level
		err = ::SetThreadStateEndCritical(mThread, kStoppedThreadState, kNoThreadID);

		// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
		THREAD_ASSERT(err == noErr);

#if BREAK_ON_SLEEP
		UDebugging::DebugString(StringLiteral_("Thread now awake"));
#endif
	}

	catch (...)
	{
		// remove timer from TM queue, in case we got to enqueue it
		RemoveTimeTask(mTimer);

		// exit critical section before leaving function
		::ThreadEndCritical();

		// transfer to enclosing error handler
		throw;

	}

	// in case something bad happened while we were sleeping
	if (selfSleep)
	{
		err		= mError;
		mError	= noErr;

		ThrowIfError_(err);

#if THREAD_INACTIVITY
		PreventInactivity();
#endif
	}
	else
		Yield();
}


// ---------------------------------------------------------------------------
//	¥ Wake
// ---------------------------------------------------------------------------
//	Make a sleeping thread eligible for CPU time once more.

void
LThread::Wake()
{
	SInt16	err;

	// a thread can't wake itself !
	THREAD_ASSERT(sThread != this);

	try
	{
		// enter critical section
		::ThreadBeginCritical();

		// can't wake a thread that's not sleeping
		if (mState != threadState_Sleeping)
		{
			Throw_(errBadThreadState);
		}

		// remove element from timer queue
		RemoveTimeTask(mTimer);

		// adjust ready thread count accordingly
		MakeReady();

		// end critical section by resuming thread at OS level
		err = ::SetThreadStateEndCritical(mThread, kReadyThreadState, kNoThreadID);

		// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
		THREAD_ASSERT(err == noErr);

		Yield();
	}

	catch (...)
	{
		// remove timer from TM queue, in case we didn't get to enqueue it
		RemoveTimeTask(mTimer);

		// exit critical section before leaving function
		::ThreadEndCritical();

		// transfer to enclosing error handler
		throw;

	}
}


// ---------------------------------------------------------------------------
//	¥ SemWait
// ---------------------------------------------------------------------------
//	Make a thread ineligible for CPU time until a semaphore is released.
//
//	A waiting thread can only regain control of the CPU when its semaphore
//	is released, or if its wait timer expires.
//
//	NOTE:	The current thread must be in a critical section upon
//			entering this function!

ExceptionCode
LThread::SemWait(
	LSemaphore	*inSemaphore,
	SInt32		inMilliSeconds,
	QHdr		&ioQHdr,
	Boolean		&outTimedOut)
{
	ExceptionCode	err;

	// perform some sanity checks ...
	THREAD_ASSERT(this == sThread);
	THREAD_ASSERT(inMilliSeconds == thread_WaitForever || inMilliSeconds > 0);
	THREAD_ASSERT(mSemaphore == NULL);
	THREAD_ASSERT(inSemaphore != NULL);

#if BREAK_ON_WAIT
	UDebugging::DebugString(StringLiteral_("About to wait thread"));
#endif

	// set up blocking vars
	mSemaphore = inSemaphore;

	// change thread state
	mState = threadState_Waiting;

	// add thread to semaphore's queue of waiting threads
	AddToStateQueue(*this, ioQHdr);

	// remove any leftover timer from TM queue
	RemoveTimeTask(mTimer);

	// if the caller wants a timeout, set up timer
	if (inMilliSeconds > 0)
	{
		InsertTimeTask(mTimer);
		PrimeTimeTask(mTimer, inMilliSeconds);
	}

	// End critical section by stopping thread at OS level.
	//
	// Because we're stopping the current thread, rescheduling
	// will have to occur.  Therefore, several error conditions
	// may arise:
	//
	// 1.	The current thread's critical depth isn't 1.  This
	//		means that we're either in a nested critical section
	//		(ThreadBeginCritical was called more that once), or
	//		we're not in a critical section at all.
	//
	// 2.	There are no other threads left to run.
	//
	// All of these situations are programming errors.

	err = ::SetThreadStateEndCritical(mThread, kStoppedThreadState, kNoThreadID);

	// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
	THREAD_ASSERT(err == noErr);

	// enter critical section
	::ThreadBeginCritical();

	// remove any leftover timer
	RemoveTimeTask(mTimer);

	if (mError == errSemaphoreDestroyed)
		outTimedOut = false;
	else
	{
		// remove the thread from our queue of waiting threads.
		// if the thread was in the queue, it means we timed out.
		outTimedOut = (RemoveFromStateQueue(reinterpret_cast<QElemPtr>(&mStateLink), ioQHdr) != NULL);
	}

	// set our states var correctly
	mSemaphore	= NULL;

	// in case something bad happened while we were waiting ...
	err			= mError;
	mError		= noErr;

#if THREAD_INACTIVITY
	PreventInactivity();
#endif

	return (err);
}


// ---------------------------------------------------------------------------
//	¥ SemUnwait	[static]
// ---------------------------------------------------------------------------
//	Make a waiting thread eligible for CPU time once again.
//
//	NOTE:	The current thread must be in a critical section upon
//			entering this function!

LThread*
LThread::SemUnwait(
	LSemaphore*		inSemaphore,
	ExceptionCode	inError,
	QElemPtr		inQElem,
	QHdr&			ioQHdr)
{
#pragma unused(inSemaphore)

	SInt16	err;
	LThread	*thread;

	thread = RemoveFromStateQueue(inQElem, ioQHdr);
	if (thread == NULL)
		return (NULL);

	// we can't unblock ourselves
	THREAD_ASSERT(thread != sThread);
	// do sanity check on semaphore
	THREAD_ASSERT(thread->mSemaphore != NULL);
	THREAD_ASSERT(thread->mSemaphore == inSemaphore);

	// can't unblock a thread that isn't waiting
	THREAD_ASSERT(thread->mState == threadState_Waiting);

#if BREAK_ON_WAIT
	UDebugging::DebugString(StringLiteral_("Thread now being unwaited (?)"));
#endif

	// set our state vars correctly
	thread->mSemaphore = NULL;
	if (thread->mError == noErr)
		thread->mError = inError;

	// if we're unblocking a thread because it's in the process
	// of being destroyed, don't bother waking it up.

	if (inError != errKilledThread)
	{
		// change thread state
		thread->MakeReady();

		err = ::SetThreadState(thread->mThread, kReadyThreadState, kNoThreadID);

		// THIS SHOULD NEVER FAIL IN SHIPPING CODE ...
		THREAD_ASSERT(err == noErr);
	}

	return (thread);
}


// ---------------------------------------------------------------------------
//	¥ IsPreemptive
// ---------------------------------------------------------------------------
//	Is the thread a preemptive thread ?

Boolean
LThread::IsPreemptive() const
{
	return (mPreemptive);
}


// ---------------------------------------------------------------------------
//	¥ SetNextOfKin
// ---------------------------------------------------------------------------
//	Set the thread to be notified of this thread's death

void
LThread::SetNextOfKin(LThread* inThread)
{
	mNextOfKin = inThread;
}


// ---------------------------------------------------------------------------
//	¥ NotifyError
// ---------------------------------------------------------------------------
//	Provoke an exception the next time the thread wakes up

void
LThread::NotifyError(ExceptionCode inError)
{
	mError = inError;
}


// ---------------------------------------------------------------------------
//	¥ GetRecycle
// ---------------------------------------------------------------------------
//	Is the thread's stack supposed to be recycled when the thread is
//	destroyed ?

Boolean
LThread::GetRecycle() const
{
	return (mRecycle);
}


// ---------------------------------------------------------------------------
//	¥ SetRecycle
// ---------------------------------------------------------------------------
//	Set whether or not the thread's stack is supposed to be recycled
//	when the thread is destroyed.

void
LThread::SetRecycle(Boolean recycle)
{
	mRecycle = recycle;
}


// ---------------------------------------------------------------------------
//	¥ GetResult
// ---------------------------------------------------------------------------
//	Get the thread's result

void*
LThread::GetResult() const
{
	return (mResult);
}


// ---------------------------------------------------------------------------
//	¥ SetResult
// ---------------------------------------------------------------------------
//	Set the thread's result

void
LThread::SetResult(void* inResult)
{
	mResult = inResult;
}


// ---------------------------------------------------------------------------
//	¥ GetFreeStack
// ---------------------------------------------------------------------------
//	Return the amount of free space in the thread's stack.  Note that
//	this information is of dubious accuracy outside of a critical section.

UInt32
LThread::GetFreeStack() const
{
	UInt32	stackSize;

	::ThreadCurrentStackSpace(mThread, &stackSize);

	return (stackSize);
}


// ---------------------------------------------------------------------------
//	¥ GetState
// ---------------------------------------------------------------------------
//	Get the thread's state

LThread::EThreadState
LThread::GetState() const
{
	return (mState);
}

#pragma mark -
// ===========================================================================
//	¥ UMainThread												 UMainThread ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ UMainThread							Default Constructor		  [public]
// ---------------------------------------------------------------------------

UMainThread::UMainThread()

	: LThread(false, thread_DefaultStack, LThread::threadOption_Main)
{
}


// ---------------------------------------------------------------------------
//	¥ ~UMainThread							Destructor				  [public]
// ---------------------------------------------------------------------------

UMainThread::~UMainThread()
{
}


// ---------------------------------------------------------------------------
//	¥ Run														   [protected]
// ---------------------------------------------------------------------------
//	Executes the thread's code.  Since the application's main thread starts
//	implicitly at main(), this is here for debugging purposes only.

void*
UMainThread::Run()
{
	SignalStringLiteral_("Entering UMainThread::Run() -- this should never happen!");

	return nil;
}

PP_End_Namespace_PowerPlant
