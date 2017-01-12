// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTimerTaskFunctor.cp		PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Carbon Event Loop Timer where the Toolbox callback calls an ordinary
//	C function of the form:
//
//		void MyTimer( LTimerTask* inTask );
//
//	You pass a pointer to this function when you construct the
//	LTimerTaskFunctor object. You can change the function by calling
//	SetTimerFunc().

#include <LTimerTaskFunctor.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTimerTaskFunctor						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTimerTaskFunctor::LTimerTaskFunctor()
{
	mTimerFunc = nil;
}


// ---------------------------------------------------------------------------
//	¥ LTimerTaskFunctor						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTimerTaskFunctor::LTimerTaskFunctor(
	TimerFunc			inTimerFunc)
{
	mTimerFunc = inTimerFunc;
}


// ---------------------------------------------------------------------------
//	¥ LTimerTaskFunctor						Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct and install timer

LTimerTaskFunctor::LTimerTaskFunctor(
	EventLoopRef		inEventLoop,
	EventTimerInterval	inFireDelay,
	EventTimerInterval	inInterval,
	TimerFunc			inTimerFunc)
{
	mTimerFunc = inTimerFunc;

	Install(inEventLoop, inFireDelay, inInterval);
}


// ---------------------------------------------------------------------------
//	¥ ~LTimerTaskFunctor					Destructor				  [public]
// ---------------------------------------------------------------------------

LTimerTaskFunctor::~LTimerTaskFunctor()
{
}


// ---------------------------------------------------------------------------
//	¥ Install														  [public]
// ---------------------------------------------------------------------------

OSStatus
LTimerTaskFunctor::Install(
	EventLoopRef		inEventLoop,
	EventTimerInterval	inFireDelay,
	EventTimerInterval	inInterval,
	TimerFunc			inTimerFunc)
{
	mTimerFunc = inTimerFunc;

	return Install(inEventLoop, inFireDelay, inInterval);
}


// ---------------------------------------------------------------------------
//	¥ DoTask
// ---------------------------------------------------------------------------

void
LTimerTaskFunctor::DoTask()
{
	if (mTimerFunc != nil) {			// Call via function pointer
		(*mTimerFunc)(this);
	}
}


PP_End_Namespace_PowerPlant
