// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	InterruptDisableLib.cp		PowerPlant 2.2.2		©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	Based on Apple Sample Code written by Quinn "The Eskimo!"
//	Portions ©1998 by Apple Computer, Inc.

#include "InterruptDisableLib.h"

#include <MixedMode.h>

// ---------------------------------------------------------------------------

#if defined(CALL_NOT_IN_CARBON) && !CALL_NOT_IN_CARBON
									// $$$ CARBON $$$
									// This package uses 68K code. It doesn't
									// seem possible to call it in Carbon.

	#error InterruptDisableLib is not Carbon campatible

#else


// ---------------------------------------------------------------------------
// Low-Level, Architecture-Specific, Subroutines

#if TARGET_CPU_PPC

	// PowerPC Specific Code

	// On PPC, we use MixedMode to handle moving the PPC parameters
	// into the right 68K registers and back again.  This make our
	// 68K very easy to write.

	enum {
		kGetSRProcInfo = kRegisterBased
				| RESULT_SIZE(SIZE_CODE(sizeof(UInt16)))
				| REGISTER_RESULT_LOCATION(kRegisterD0),
		kSetSRProcInfo = kRegisterBased
				| RESULT_SIZE(0)
				| REGISTER_ROUTINE_PARAMETER(1, kRegisterD0, SIZE_CODE(sizeof(UInt16)))
	};

	// We define the 68K as a statically initialised data structure.
	// The use of MixedMode to call these routines makes the routines
	// themselves very simple.

	static UInt16 gGetSR[] = {
		0x40c0,		// move sr,d0
		0x4e75		// rts
	};

	static UInt16 gSetSR[] = {
		0x46c0,		// move d0,sr
		0x4e75		// rts
	};

	static UInt16 GetSR(void)
		// Returns the current value of the SR, interrupt mask
		// and all!  This routine uses MixedMode to call the gGetSR data
		// structure as if it was 68K code (which it is!).

	{
		return (UInt16) CallUniversalProc( (UniversalProcPtr) &gGetSR, kGetSRProcInfo);
	}

	static void SetSR(UInt16 newSR)
		// Returns the value of the SR, including the interrupt mask and all
		// the flag bits.  This routine uses MixedMode to call the gGetSR data
		// structure as if it was 68K code (which it is!).
	{
		CallUniversalProc( (UniversalProcPtr) &gSetSR, kSetSRProcInfo, newSR);
	}

#elif TARGET_CPU_68K

	// Classic 68K and CFM-68K Specific Code

	// On classic 68K (and CFM-68K) we can simply access the
	// 68K SR register using some inline procedures.

	static UInt16 GetSR(void) = {
		0x40c0		// move sr,d0
	};

	#pragma parameter SetSR(__D0)
	static void SetSR(UInt16 newSR) = {
		0x46c0		// move d0,sr
	};

#elif

	#error "Unknown Target CPU"

#endif


// ---------------------------------------------------------------------------
//	¥ GetInterruptMask
// ---------------------------------------------------------------------------

extern pascal UInt16 GetInterruptMask()
{
	return (UInt16) ((GetSR() >> 8) & 7);
}


// ---------------------------------------------------------------------------
//	¥ SetInterruptMask
// ---------------------------------------------------------------------------

extern pascal UInt16 SetInterruptMask(UInt16 newMask)
{
	UInt16 currentSR;

	currentSR = GetSR();
	SetSR( (UInt16) ((currentSR & 0xF8FF) | (newMask << 8)) );

	return (UInt16) ((currentSR >> 8) & 7);
}

#endif
