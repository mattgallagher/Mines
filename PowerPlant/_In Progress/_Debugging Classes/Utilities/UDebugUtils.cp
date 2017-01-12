// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugUtils.cp 				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Utilities for working with debuggers

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDebugUtils.h>
#include <UEnvironment.h>
#include <UMemoryMgr.h>

#include <UOnyx.h>
#include <PP_DebugMacros.h>

#if !TARGET_API_MAC_OSX
	#include <MetroNubUtils.h>
#else
	// $$$ MACHO $$$ Can we do something with gdb instead?
	#define IsMWDebuggerRunning()		false
	#define AmIBeingMWDebugged()		false
#endif

#include <Gestalt.h>
#include <LowMem.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ CheckEnvironment
// ---------------------------------------------------------------------------
//	Perform a series of checks to examine the debugging environment at
//	runtime and alert the user to any potential problem situations. Useful
//	to call at application startup (at least after Toolbox initialization as
//	it may display alerts).

void
UDebugUtils::CheckEnvironment()
{
#if PP_Target_Classic
		// Warn if Norton CrashGuard is installed. CG doesn't play nice
		// (see the comments above GetLowLevelDebuggerInfo for more information).
		// NB: If this triggers, the other checks here (especially those for
		// debuggers) could fail or report erroneous information.
	SInt32 response;
	if (::Gestalt(FOUR_CHAR_CODE('CrGd'), &response) == noErr) {
			// If this happens, we cannot be 100% certain we actually have
			// a debugger installed, so we'll force an alert.
		StChangeDebugSignal_(debugAction_Alert);
		SignalStringLiteral_("Norton CrashGuard installed. You might want to uninstall it.\r\rNB: This may cause other debugging environment checks to return bogus values.");
	}

		// See if a debugger, any debugger, is present
	bool noDebugger = false;
	if (IsADebuggerPresent() == false) {
		noDebugger = true;
		StChangeDebugSignal_(debugAction_Alert);
		SignalStringLiteral_("No debugger found.\r\rThis is not a fatal condition, but it's highly recommended to have a debugger installed.");
	}

		// See if MacsBug is around
	if (IsMacsBugInstalled() == false) {
		StChangeDebugSignal_(debugAction_Alert);
		SignalStringLiteral_("MacsBug isn't installed. You might want to install it.");
	}

#ifdef Debug_Signal
		// Ensure gDebugSignal has a proper place to resolve
	if ((UDebugging::GetDebugSignal() == debugAction_Debugger) && noDebugger) {
			// No debugger and we're set to go into the debugger -- not good.
			// Force alerts.
		SetDebugSignal_(debugAction_Alert);
		SignalStringLiteral_("gDebugSignal set to debugAction_Debugger but no debugger installed. Forcing alerts.");
	}
#endif

#ifdef Debug_Throw
		// Ensure gDebugThrow has a proper place to resolve
	if ((UDebugging::GetDebugThrow() == debugAction_Debugger) && noDebugger) {
			// No debugger and we're set to go into the debugger -- not good.
			// Force alerts
		SetDebugThrow_(debugAction_Alert);
		SignalStringLiteral_("gDebugThrow set to debugAction_Debugger but no debugger installed. Forcing alerts.");
	}
#endif

		// If QC supports are enabled, ensure QC is installed
#if PP_QC_Support
	if (UQC::IsInstalled() == false) {
		SignalStringLiteral_("QC supports enabled but QC not installed.\r\rThis is not a fatal condition.");
	}
#endif

#endif	// PP_Target_Classid
}


// ---------------------------------------------------------------------------
//	¥ GetLowLevelDebuggerInfo
// ---------------------------------------------------------------------------
//	Discovers information about the installed low-level debugger. This
//	code is adapted from an Apple code snippet which can be found at:
//	<http://developer.apple.com/dev/techsupport/source/code/Snippets/Testing_and_Debugging/DebuggerPresence/debugger.c.html>
//	It also contains some "updates" made by Jason Yeo of Apple DTS.
//
//	* NOTE * As of this writing (3 February 1998), there is apparently a
//	conflict/problem if Norton CrashGuard is installed. CrashGuard
//	does some things to the debugWorld that interfere with what we try to
//	do here. CrashGuard version 2 and up (you can check for this with
//	Gestalt, 'CrGd' is the selector, returned value has version number in
//	lower 2 bytes and upper 2 bytes have a boolean if CG is on or off)
//	will have a debugWorld that will install a signature and also store off
//	the old MacJmpFlg and old MacJmp so you can track things down. Anything
//	less than v2 of CrashGuard (which as of this writing wasn't released yet)
//	has nothing in the debugWorld, but does do something ugly to it. So
//	it wreaks some havoc.
//
//	Since I do not have a v2 of CrashGuard, I have chosen not to implement
//	the possible workaround. And the pre-v2 workaround I was given from
//	Symantec I have also chosen to avoid. Why? As a developer, I don't see
//	much need for CrashGuard... I need a more useful debugger like MacsBug.
//
//	Eventually I hope to have a decent mechanism in place to deal with
//	CrashGuard. Meantime, I prefer to disable CrashGuard (i.e. remove it
//	totally from my Extensions folder. Just turning it off via it's
//	control panel is not enough).
//
//	Comments from the original snippet:
//----------------------------------------------------------------------------
/*
**  This is a small snippet of code that can be used to to detect if
**  macsbug is installed or not. NOTE:  This code is intended to only
**  work with version 6.2 of macsbug.  You should refer to your Low Level
**  Debugger's manual for more information on how they install
**  themselves.
**
**  This code is based on information obtained from the MacsBug
**  Reference. The basic assumptions are that macsbug will install itself
**  in the following manner:
**
**  If you are running with a Memory Manager that only works in 24 bit
**  mode, then the high -order byte of MacJmp is a flags byte that
**  contains the following information:
**
**  Bit Meaning
**  --- --------------------------------------
**  7   -   Set if debugger is running
**  6   -   Set if debugger can handle system errors
**  5   -   Set if debugger is installed
**  4   -   Set if debugger can support discipline utility
**
**  The lower 3 bytes are used to store the address of the debugger's
**  entry point.
**
**  If you are running with a Memory Manager that works in 32-bit mode,
**  the flags byte is moved to address 0xBFF and the long word at MacJmp
**  becomes a full 32-bit address that points to the debugger's entry
**  point..
**
**  Symantec has a comment in the Think Reference 2.0.1 which states:
**
**  "ADDENDUM:  The above information seems to be incorrect in the
**  reference manual. I have found through testing etc. that in both
**  modes, the Flag Byte appears at location 0xBFF.  The code reflects
**  these findings."
**
**  This is because they confused running in 24 bit mode, running in 32
**  bit mode, and the _ability_ to run in 32 bit mode.  It is the latter
**  ability which you must test to determine the location of the debugger
**  flags.
**
** as documented in the "Macsbug Reference & Debugging Guide", page 412
** if we have a 32 bit capable Memory Manager, debugger flags are at $BFF
** if we have a 24 bit capable Memory Manager, debugger flags are at $120
** Ptr at $120
**
** Note that the documentation is slightly obscure--you check if the machine
** is capable of 32-bit mode, _not_ if you are running in 32 bit mode.
*/
//----------------------------------------------------------------------------
//	Currently this routine only checks for MacsBug and TMON (or another,
//	generic debugger (other)).


/* stuff that should be defined in C, but isn't (it's in Private.a) */
#define MacJmp          (Ptr *)  0x120      /* MACSBUG jumptable [pointer] */
#define MacJmpByte      (char *) 0x120      /* MACSBUG flags in 24 bit mode [byte] */
#define MacJmpFlag      (char *) 0xBFF      /* MacsBug flag [byte] */
const	SInt16	kDebuggerInstalled		= 5;

bool
UDebugUtils::GetLowLevelDebuggerInfo(
	EDebuggerType&	outType,
	SInt16&			outSignature)
{
		// Initialize to defaults, assuming no debugger is present
	outType = debuggerType_NoDebugger;
	outSignature = (' ' << 8) | ' ';

		// No debugger to talk to on OS X
	if (UEnvironment::GetOSVersion() >= 0x1000)
		return false;

		// See if we're in 24-bit or 32-bit addressing mode
	SInt16 debugFlags = *MacJmpFlag;
	if (!UEnvironment::HasGestaltAttribute(
			gestaltAddressingModeAttr, gestalt32BitCapable)) {
		debugFlags = *MacJmpByte;	// In 24-bit mode
	}

	bool gotInfo = false;

	if (debugFlags & (1 << kDebuggerInstalled)) {

			// Got a debugger; what is it?
		#if TARGET_API_MAC_CARBON
			Ptr debugEntry = *MacJmp;
			Ptr	ROMBaseWorld = 0;		// $$$ CARBON $$$ What can we do here?
		#else
			Ptr debugEntry = StripAddress(*MacJmp);
			Ptr ROMBaseWorld = LMGetROMBase();
		#endif

		if (debugEntry < ROMBaseWorld) { // Not ROM based debugger

			#if TARGET_API_MAC_CARBON
				SInt16	**debugWorld = reinterpret_cast<SInt16**>(debugEntry - sizeof(Ptr));
			#else
				SInt16	**debugWorld = reinterpret_cast<SInt16**>(StripAddress(debugEntry - sizeof(Ptr)));
			#endif


			SLDisable_();	// Spotlight (understandably) doesn't like this
			outSignature = **debugWorld;
			SLEnable_();

			switch (outSignature) {
				case (('M' << 8) | 'T'):
					outType = debuggerType_MacsBug;
					break;

				case (('W' << 8) | 'H'):
					outType = debuggerType_TMON;
					break;

				default:
					outType = debuggerType_Other;
					break;
			}
		}

		gotInfo = true;
	}

	return gotInfo;
}


// ---------------------------------------------------------------------------
//	¥ IsMacsBugInstalled
// ---------------------------------------------------------------------------
//	Determines if MacsBug is installed or not.

bool
UDebugUtils::IsMacsBugInstalled()
{
	EDebuggerType	debuggerType;
	SInt16			debuggerSig;

	GetLowLevelDebuggerInfo(debuggerType, debuggerSig);

	return (debuggerType == debuggerType_MacsBug);
}


// ---------------------------------------------------------------------------
//	¥ IsTheDebuggerPresent
// ---------------------------------------------------------------------------
//	Checks to see if The Debugger (from Steve Jasik) is installed.

bool
UDebugUtils::IsTheDebuggerInstalled()
{
		// No debugger to talk to on OS X
	if (UEnvironment::GetOSVersion() >= 0x1000)
		return false;

	return *(SInt16*)(*(SInt32*)0x38) == 0x487A;
}


// ---------------------------------------------------------------------------
//	¥ IsADebuggerPresent()
// ---------------------------------------------------------------------------
//	Checks just to see if there is a debugger around, not necessarily
//	running, just present. Currently looks for what GetLowLevelDebuggerInfo
//	looks for, plus MW Debug and Jasik's Debugger.

bool
UDebugUtils::IsADebuggerPresent()
{
	EDebuggerType	debuggerType;
	SInt16			signature;

		// Use IsMWDebuggerRunning() here in hopes that the user will have
		// the "Automatically target applications" MW Debug pref turned on.
	return (GetLowLevelDebuggerInfo(debuggerType, signature)
			|| IsMWDebuggerRunning()
			|| IsTheDebuggerInstalled());
}


// ---------------------------------------------------------------------------
//	¥ IsADebuggerRunning()
// ---------------------------------------------------------------------------
//	Checks to see if there is a debugger around and we're being debugged
//	by it. Currently looks for what GetLowLevelDebuggerInfo looks for, plus
//	MW Debug and Jasik's Debugger.

bool
UDebugUtils::IsADebuggerRunning()
{
	EDebuggerType	debuggerType;
	SInt16			signature;
	return (GetLowLevelDebuggerInfo(debuggerType, signature)
			|| AmIBeingMWDebugged()
			|| IsTheDebuggerInstalled());
}


// ---------------------------------------------------------------------------
//	¥ Debugger
// ---------------------------------------------------------------------------
//	Breaks to a debugger with a sanity check to ensure there actually is
//	something running to catch the call (instead of an unimplemented
//	instruction crash :)

void
UDebugUtils::Debugger()
{
	if (UDebugUtils::IsADebuggerRunning() == false) {

			// If there's no debugger, have to force an alert
		StChangeDebugSignal_(debugAction_Alert);

		SignalStringLiteral_("No debugger running!");
		return;
	}

	::Debugger();
}


// ---------------------------------------------------------------------------
//	¥ DebugStr
// ---------------------------------------------------------------------------
//	Breaks into the debugger with the given string. Sanity check to
//	ensure a debugger is present first.

void
UDebugUtils::DebugStr(
	ConstStr255Param	inString)
{
	if (UDebugUtils::IsADebuggerRunning() == false) {

			// If there's no debugger, have to force an alert
		StChangeDebugSignal_(debugAction_Alert);

		SignalStringLiteral_("No debugger running!");
		return;
	}

	::DebugStr(inString);
}

PP_End_Namespace_PowerPlant
