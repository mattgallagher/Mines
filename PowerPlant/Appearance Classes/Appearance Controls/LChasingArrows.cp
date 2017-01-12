// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LChasingArrows.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LChasingArrows.h>
#include <LControlImp.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const UInt32				ticks_Animation	= 5;
const EventTimerInterval	eventTime_Animation = ticks_Animation / 60.0;


// ---------------------------------------------------------------------------
//	¥ LChasingArrows						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LChasingArrows::LChasingArrows(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	mNextIdleTick = 0;
	
	if (not GetControlImp()->IdlesAutomatically()) {
		StartIdling(eventTime_Animation);
	}
}


// ---------------------------------------------------------------------------
//	¥ LChasingArrows						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LChasingArrows::LChasingArrows(
	const SPaneInfo&	inPaneInfo,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlChasingArrowsProc)
{
	mNextIdleTick = 0;
	
	if (not GetControlImp()->IdlesAutomatically()) {
		StartIdling(eventTime_Animation);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LChasingArrows						Destructor				  [public]
// ---------------------------------------------------------------------------

LChasingArrows::~LChasingArrows()
{
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------

void
LChasingArrows::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
		// With a TimerTask, we always want to idle the control to
		// spin the arrows. Without a TimerTask (Classic never has
		// a TimerTask), we idle only if enough time has passed since
		// the last idle. This limits the speed of the animation so
		// it isn't too fast.

	#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
	
		bool	doIdle = true;
		
		if (mTimerTask == nil) {
			doIdle = ::TickCount() >= mNextIdleTick;
		}
			
	#else
	
		bool	doIdle = ::TickCount() >= mNextIdleTick;
		
	#endif
	
	if (doIdle) {
		mControlImp->Idle();
		mNextIdleTick = ::TickCount() + ticks_Animation;
	}
}


PP_End_Namespace_PowerPlant
