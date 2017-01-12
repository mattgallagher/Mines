// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHeapAction.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A PowerPlant periodical to perform certain actions (EHeapAction)
//	on the given heap.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LHeapAction.h>
#include <UHeapUtils.h>
#include <UOnyx.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LHeapAction								[public]
// ---------------------------------------------------------------------------
//	Parameterized constructor.
//
//	Creation of the object doesn't start the periodical. You must
//	explicitly call Start().

LHeapAction::LHeapAction(
	EHeapAction	inAction,
	const THz	inHeapZone,
	UInt32		inTicks,
	bool		inIsRepeater)
{
	mAction		= inAction;
	mHeapZone	= inHeapZone;
	mWaitTicks	= inTicks;
	mLastTime	= ::TickCount();
	mRepeater	= inIsRepeater;
}


// ---------------------------------------------------------------------------
//	LHeapAction									[public]
// ---------------------------------------------------------------------------
//	Copy constructor
//
//	Creation of the object doesn't start the periodical. You must
//	explicitly call Start().

LHeapAction::LHeapAction(
	const LHeapAction&	inOriginal)
{
	mAction		= inOriginal.mAction;
	mHeapZone	= inOriginal.mHeapZone;
	mWaitTicks	= inOriginal.mWaitTicks;
	mLastTime	= inOriginal.mLastTime;
	mRepeater	= inOriginal.mRepeater;
}


// ---------------------------------------------------------------------------
//	operator=									[public]
// ---------------------------------------------------------------------------
//	assignment operator

LHeapAction&
LHeapAction::operator=(
	const LHeapAction&	inRhs)
{
	if (this != &inRhs) {
		mAction		= inRhs.mAction;
		mHeapZone	= inRhs.mHeapZone;
		mWaitTicks	= inRhs.mWaitTicks;
		mLastTime	= inRhs.mLastTime;
		mRepeater	= inRhs.mRepeater;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LHeapAction								[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LHeapAction::~LHeapAction()
{
}


// ---------------------------------------------------------------------------
//	¥ SpendTime									[public, virtual]
// ---------------------------------------------------------------------------
//	Performs the heap action, but only if the specified delay
//	time has passed (allows such things as compactation every X seconds).

void
LHeapAction::SpendTime(
	const EventRecord&	/*inMacEvent*/)
{
	if (mLastTime >= ::TickCount()) {
		return;
	}

	switch (mAction) {
		case heapAction_None:
			break;

		case heapAction_Compact:
			UHeapUtils::CompactHeap(mHeapZone);
			break;

		case heapAction_Purge:
			UHeapUtils::PurgeHeap(mHeapZone);
			break;

		case heapAction_CompactAndPurge:
			UHeapUtils::CompactAndPurgeHeap(mHeapZone);
			break;

	#if PP_QC_Support
		case heapAction_QCTests:
			UQC::CheckHeap();
			UQC::BoundsCheck();
			UQC::ScrambleHeap();
			break;
	#endif

		default:
			SignalStringLiteral_("Unknown EHeapAction");
			break;
	}

	mLastTime = ::TickCount() + mWaitTicks;
}


// ---------------------------------------------------------------------------
//	¥ Start											[public, virtual]
// ---------------------------------------------------------------------------
//	Used to start the perodical. Optionally you can change/specify the
//	number of ticks for the buffer delay (negative values mean no change).
//	You should use this method and NOT the LPeriodical methods (e.g.
//	StartRepeating(), StartIdling(), etc.).

void
LHeapAction::Start(
	SInt32		inTicks)
{
		// inTicks allows you to change the tick check rate.
		// If a negative value, no change from the previous setting
	if (inTicks >= 0) {
		mWaitTicks = static_cast<UInt32>(inTicks);
	}

	if (IsRunning()) {
		return;
	}

	if (mRepeater) {
		StartRepeating();
	} else {
		StartIdling();
	}
}


// ---------------------------------------------------------------------------
//	¥ Stop										[public, virtual]
// ---------------------------------------------------------------------------
//	Stops the Periodical. This method should be used and NOT the
//	LPeriodical methods (StopRepeating(), StopIdling()).

void
LHeapAction::Stop()
{
	StopRepeating();
	StopIdling();
}


// ---------------------------------------------------------------------------
//	¥ MakeRepeater								[public]
// ---------------------------------------------------------------------------
//	Turns the Periodical into a Repeater.

void
LHeapAction::MakeRepeater()
{
		// If already a Repeater, just return
	if (IsRepeater())
		return;

		// If it's already running, just make sure it starts running
		// again (so one can change from Idler to Repeater on the fly).
	bool	wasRunning = IsRunning();
	Stop();
	mRepeater = true;

	if (wasRunning) {
		Start();
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeIdler									[public]
// ---------------------------------------------------------------------------
//	Turns the Periodical into an Idler.

void
LHeapAction::MakeIdler()
{
		// If already an Idler, just return
	if (IsIdler())
		return;

		// If it's already running, just make sure it starts running
		// again (so one can change from Idler to Repeater on the fly).
	bool	wasRunning = IsRunning();
	Stop();
	mRepeater = false;

	if (wasRunning) {
		Start();
	}
}

PP_End_Namespace_PowerPlant

// Defined inline in the header
#pragma mark LHeapAction::ChangeTime
#pragma mark LHeapAction::GetTicks
#pragma mark LHeapAction::GetSeconds
#pragma mark LHeapAction::IsRepeater
#pragma mark LHeapAction::IsIdler
