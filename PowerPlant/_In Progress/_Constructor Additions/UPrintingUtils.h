// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UPrintingUtils.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UPrintingUtils
#define _H_UPrintingUtils
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//		¥ StFractionalPenWidth
// ===========================================================================
//	A stack-based utility class for drawing fractional line widths.
//	Only useful for printing.
//
//	For documentation on the SetLineWidth picture comment, see
//	Inside Macintosh: Imaging with QuickDraw, B-35 to B-37.

class StFractionalPenWidth {

public:
	StFractionalPenWidth(SInt16 inNumerator, SInt16 inDenominator);
	~StFractionalPenWidth();

protected:
	SInt16			mNumerator;
	SInt16			mDenominator;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
