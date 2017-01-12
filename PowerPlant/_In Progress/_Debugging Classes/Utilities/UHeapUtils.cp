// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UHeapUtils.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A set of utility classes for heap manipulations.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <UHeapUtils.h>
#include <UMemoryMgr.h>
#include <UDebugUtils.h>
#include <UOnyx.h>
#include <PP_DebugMacros.h>

#include <Sound.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ CompactHeap
// ---------------------------------------------------------------------------
//	Compact the given heap. If nil, assumes the application heap. Returns
//	(in bytes) the largest contiguous free block.

Size
UHeapUtils::CompactHeap(
	const THz	inHeapZone)
{
#if TARGET_API_MAC_CARBON			// No Zones in Carbon

	#pragma unused(inHeapZone)
	return 0;

#else

	THz theHeapZone = (inHeapZone == nil) ? ::ApplicationZone() : inHeapZone;

	StZone_(theHeapZone);

	return ::CompactMem(maxSize);

#endif
}


// ---------------------------------------------------------------------------
//	¥ PurgeHeap
// ---------------------------------------------------------------------------
//	Purge the given heap. If nil, assumes the application heap.

void
UHeapUtils::PurgeHeap(
	const THz	inHeapZone)
{
#if TARGET_API_MAC_CARBON			// No Zones in Carbon

	#pragma unused(inHeapZone)

#else

	THz theHeapZone = (inHeapZone == nil) ? ::ApplicationZone() : inHeapZone;

	StZone_(theHeapZone);

		// Due to the way PurgeMem works (especially because it is passed
		// the maxSize constant), it is likely to put a memFullErr in
		// MemError. This is an ok and normal occurance, but it might
		// cause QC (meaning well) to report this as an error (which it
		// is not). Quiet QC down for this call.
	StQCTestState_(qcMemErrWarnings, false);

	::PurgeMem(maxSize);

#endif
}


// ---------------------------------------------------------------------------
//	¥ CompactAndPurgeHeap
// ---------------------------------------------------------------------------
//	Compacts and purges the given heap. If nil, assumes the application
//	heap. Returns (in bytes) the largest contiguous free block.

Size
UHeapUtils::CompactAndPurgeHeap(
	const THz	inHeapZone)
{
#if TARGET_API_MAC_CARBON			// No Zones in Carbon

	#pragma unused(inHeapZone)
	return 0;

#else

	THz theHeapZone = (inHeapZone == nil) ? ::ApplicationZone() : inHeapZone;

	StZone_(theHeapZone);

	Size grow = 0;	// Will always be zero. It is only non-zero if checking
					// the application zone, but in that case it's zero if
					// MaxApplZone has already been called, which is certainly
					// the case in a PowerPlant app since InitializeHeap must
					// always be used and called within a PowerPlant app.

	return ::MaxMem(&grow);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ScrambleHeap
// ---------------------------------------------------------------------------
//	Scrambles the heap. Drops into MacsBug to do this, so you will see
//	a flicker of whitescreen.
//
//	Since we drop into MacsBug via ::DebugStr(), this might not always
//	work. Other debuggers (e.g. MW Debug) and debugger utilities (e.g.
//	Spotlight) can intercept ::DebugStr() and prevent this from working.
//	Be aware of this...
//
//	But to make life easier, if QC support is available, then we'll use
//	that instead.

void
UHeapUtils::ScrambleHeap()
{
#if PP_QC_Support
	if (UQC::IsInstalled()) {
		UQC::ScrambleHeap();
	} else // turns into "else if" after preprocessing
#endif

	if (UDebugUtils::IsMacsBugInstalled()) {
/*
		::DebugStr(StringLiteral_(" ;hs; g"));

		Handle	tempHandle = ::NewHandle(sizeof(char));
		Ptr		tempPtr = *tempHandle;
		*tempPtr = 42;

		Handle	willScramble = ::NewHandle(0);

		if (*tempPtr == 42) {	// Error. Using deref'ed ptr
			::SysBeep(1);
		}

		if (willScramble != nil) {
			::DisposeHandle(willScramble);
		}
		if (tempHandle != nil) {
			::DisposeHandle(tempHandle);
		}

		::DebugStr(StringLiteral_(" ;hs; g"));
*/
			// Drop into MacsBug and turn on it's
			// heap scrambling. This assumes scrambling is
			// off (and there's no way to query MacsBug to
			// find the scramble state, so says Jim Murphy).

		::DebugStr(StringLiteral_(" ;hs; g"));

			// Allocating this Handle should force a
			// scramble to occur.
		Handle	tempHandle = ::NewHandle(0);
		if (tempHandle != nil) {
			::DisposeHandle(tempHandle);
		}

			// Turn off scrambling
		::DebugStr(StringLiteral_(" ;hs; g"));

	} else {
			// No MacsBug, no QC force an alert
		StChangeDebugSignal_(debugAction_Alert);
		SignalStringLiteral_("MacsBug not installed, cannot scramble." );
	}

	return;
}

PP_End_Namespace_PowerPlant

// declared inline in the header
#pragma mark UHeapUtils::IsScramblingHeap

