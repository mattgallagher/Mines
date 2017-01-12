// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNewTextDrawing.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UNewTextDrawing
#define _H_UNewTextDrawing
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class UNewTextDrawing {

public:
	static void			MeasureWithJustification(
								Ptr			inText,
								SInt32		inLength,
								const Rect&	inRect,
								SInt16		inJustification,
								Rect&		outTextRect,
								Boolean		inMeasureTrailingSpace = false);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
