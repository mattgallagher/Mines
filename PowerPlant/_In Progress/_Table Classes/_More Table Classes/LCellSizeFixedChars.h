// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellSizeFixedChars.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple sizer which truncates to a fixed width of characters

#ifndef _H_LCellSizeFixedChars
#define _H_LCellSizeFixedChars
#pragma once

#include <UTextTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LCellSizeFixedChars : public LCellSizer {
public:
						LCellSizeFixedChars(
							LTextTableView	*inView,
							Boolean inTrunc=true,
							UInt16	inMaxChars=1);

						LCellSizeFixedChars(
							UInt16	inMaxChars=1, Boolean inTrunc=true);

	virtual				~LCellSizeFixedChars();

	virtual UInt16	TruncatedLength(
								const char		*inValue,	// C string
								UInt16			inItemSize,
								TableIndexT		inCol);


	void SetSize(UInt16);
	UInt16 GetSize() const;


private:
	UInt16 mMaxChars;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
