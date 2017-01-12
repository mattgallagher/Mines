// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LProgressBar.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LProgressBar.h>
#include <LControlImp.h>
#include <LStream.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const UInt32				ticks_Animation	= 5;
const EventTimerInterval	eventTime_Animation = ticks_Animation / 60.0;


// ---------------------------------------------------------------------------
//	¥ LProgressBar							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LProgressBar::LProgressBar(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
	mNextIdleTick = 0;

	Boolean	indeterminate;
	*inStream >> indeterminate;

	if (indeterminate) {
		SetIndeterminateFlag(indeterminate);
	}
}


// ---------------------------------------------------------------------------
//	¥ LProgressBar							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LProgressBar::LProgressBar(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	Boolean				inIsIndeterminate,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlProgressBarProc,
						Str_Empty, 0, inValueMessage, inValue, inMinValue,
						inMaxValue)
{
	mNextIdleTick = 0;

	if (inIsIndeterminate) {
		SetIndeterminateFlag(inIsIndeterminate);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LProgressBar							Destructor				  [public]
// ---------------------------------------------------------------------------

LProgressBar::~LProgressBar()
{
}


// ---------------------------------------------------------------------------
//	¥ SetIndeterminateFlag											  [public]
// ---------------------------------------------------------------------------
//	Change progress bar to be or not be indeterminate
//
//	If making the progress bar indeterminate, inStartNow specifies whether
//	to starting idling (animating) now. If you pass false, you must call
//	Start() yourself to begin animating the progress bar.

void
LProgressBar::SetIndeterminateFlag(
	Boolean		inIsIndeterminate,
	Boolean		inStartNow)
{
	mControlImp->SetDataTag(kControlNoPart,
			kControlProgressBarIndeterminateTag,
			sizeof(Boolean), &inIsIndeterminate);

	if (inIsIndeterminate) {		// Indeterminate ProgressBar uses
		if (inStartNow) {			//   idle time to animate
			Start();
		}
	} else {
		StopIdling();
	}
}


// ---------------------------------------------------------------------------
//	¥ IsIndeterminate												  [public]
// ---------------------------------------------------------------------------

bool
LProgressBar::IsIndeterminate() const
{
	Boolean	indeterminate;

	mControlImp->GetDataTag(kControlNoPart,
			kControlProgressBarIndeterminateTag,
			sizeof(Boolean), &indeterminate);

	return indeterminate;
}


// ---------------------------------------------------------------------------
//	¥ Start															  [public]
// ---------------------------------------------------------------------------

void
LProgressBar::Start()
{
	if (IsIndeterminate()) {
		if (not GetControlImp()->IdlesAutomatically()) {
			StartIdling(eventTime_Animation);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Stop															  [public]
// ---------------------------------------------------------------------------

void
LProgressBar::Stop()
{
	if (IsIndeterminate()) {
		StopIdling();
	}
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------
//	Animate indeterminate progress bar

void
LProgressBar::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
		// With a TimerTask, we always want to idle the control to
		// animate the bar. Without a TimerTask (Classic never has
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
