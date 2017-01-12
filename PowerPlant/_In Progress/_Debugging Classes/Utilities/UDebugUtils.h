// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugUtils.h	   			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Sundry utility routines for debugging

#ifndef _H_UDebugUtils
#define _H_UDebugUtils
#pragma once

#include <PP_Debug.h>
#include <LPeriodical.h>
#include <LCommander.h>

#include <MacMemory.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UDebugUtils {

		enum EDebuggerType {
			debuggerType_NoDebugger,
			debuggerType_MacsBug,
			debuggerType_TMON,
			debuggerType_Other
		};


	void			CheckEnvironment();

	bool			GetLowLevelDebuggerInfo(
							EDebuggerType&		outType,
							SInt16&				outSignature);
	bool			IsMacsBugInstalled();
	bool			IsTheDebuggerInstalled();
	bool			IsADebuggerPresent();
	bool			IsADebuggerRunning();

	void			Debugger();
	void			DebugStr(
							ConstStr255Param	inString);

}

// ---------------------------------------------------------------------------
//	Wrapper macros for breaking into a debugger. The benefit of using these
//	macros over the direct toolbox call is they will removed in your
//	release builds, so hopefully you'll never leave a DebugStr in your
//	shipping code again ;-) The PP_ versions go through the UDebugUtils
//	bottlenecks; the non-PP_ versions go direct to the toolbox.

#if PP_Debug

	#define PP_Debugger_()					PP_PowerPlant::UDebugUtils::Debugger()
	#define	Debugger_()						::Debugger()

	#define PP_DebugStr_(str)				PP_PowerPlant::UDebugUtils::DebugStr(str)
	#define DebugStr_(str)					::DebugStr(str)

#else

	#define PP_Debugger_()
	#define Debugger_()

	#define PP_DebugStr_(str)
	#define DebugStr_(str)

#endif


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_U_DebugUtils
