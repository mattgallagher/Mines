// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextHierTable.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTextHierTable
#define _H_LTextHierTable
#pragma once

#include <LHierarchyTable.h>
#include <UTextTraits.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTextHierTable : public LHierarchyTable {
public:
	enum { class_ID = FOUR_CHAR_CODE('txht') };

						LTextHierTable( LStream* inStream );
						
	virtual				~LTextHierTable();

protected:
	virtual void		DrawCell(
								const STableCell&	inCell,
								const Rect&			inLocalRect);

	virtual void		HiliteCellActively(
								const STableCell&	inCell,
								Boolean				inHilite);
								
	virtual void		HiliteCellInactively(
								const STableCell&	inCell,
								Boolean				inHilite);

protected:
	ResIDT				mLeafTextTraits;
	ResIDT				mParentTextTraits;
	UInt16				mFirstIndent;
	UInt16				mLevelIndent;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
