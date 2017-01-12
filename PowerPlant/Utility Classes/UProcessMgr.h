// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UProcessMgr.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UProcessMgr
#define _H_UProcessMgr
#pragma once

#include <PP_Prefix.h>
#include <Processes.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace	UProcessMgr {

	extern const	ProcessSerialNumber		PSN_NoProcess;
	extern const	ProcessSerialNumber		PSN_CurrentProcess;

	OSStatus	FindPSN(
						ProcessSerialNumber	&outPSN,
						OSType				inSignature,
						OSType				inType = FOUR_CHAR_CODE('APPL'));

	OSStatus	ActivateFinder();
}


PP_End_Namespace_PowerPlant

#endif
