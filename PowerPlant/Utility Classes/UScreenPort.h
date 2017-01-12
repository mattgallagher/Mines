// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UScreenPort.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UScreenPort
#define _H_UScreenPort
#pragma once

#include <PP_Prefix.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	UScreenPort {
public:
	static void		Initialize();
	static void		Dispose();

	static GrafPtr	GetScreenPort();

protected:
	static GrafPtr	sScreenGrafPtr;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
