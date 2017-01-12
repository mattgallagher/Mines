// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDropFlag.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDropFlag
#define _H_LDropFlag
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDropFlag {
public:
	static void		Draw(	const Rect&		inRect,
							bool	 		inIsDown);

	static Boolean	TrackClick(
							const Rect&		inRect,
							const Point&	inMouse,
							bool	 		inIsDown);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
