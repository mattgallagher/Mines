// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellSizeToFitMultiRows.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple sizer which wraps to multiple rows

#ifndef _H_LCellSizeToFitMultiRows
#define _H_LCellSizeToFitMultiRows
#pragma once

#include <UTextTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LCellSizeToFitMultiRows : public LCellSizer {
public:
						LCellSizeToFitMultiRows(
								LTextTableView*	inView,
								Boolean 		inTrunc);

						LCellSizeToFitMultiRows(
								Boolean 		inTrunc = true);

	virtual				~LCellSizeToFitMultiRows();

	virtual UInt16		TruncatedLength(
								const char*		inValue,	// C string
								UInt16			inItemSize,
								TableIndexT		inCol);

private:
	short*	mMeasureArray;
	UInt16	mMeasureLen;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
