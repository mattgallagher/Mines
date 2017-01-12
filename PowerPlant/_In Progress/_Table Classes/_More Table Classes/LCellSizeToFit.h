// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellSizeToFit.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple sizer which truncates to a fixed width of characters

#ifndef _H_LCellSizeToFit
#define _H_LCellSizeToFit
#pragma once

#include <UTextTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

class LCellSizeToFit : public LCellSizer {
public:
						LCellSizeToFit(
							LTextTableView	*inView,
							Boolean inTrunc);

						LCellSizeToFit(Boolean inTrunc=true);

	virtual				~LCellSizeToFit();

	virtual UInt16		TruncatedLength(
								const char		*inValue,	// C string
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
