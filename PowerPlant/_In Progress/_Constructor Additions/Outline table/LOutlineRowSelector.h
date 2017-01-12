// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineRowSelector.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOutlineRowSelector
#define _H_LOutlineRowSelector
#pragma once

#include <LOutlineMultiSelector.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//		¥ LOutlineRowSelector
// ===========================================================================
//	This selection object selects or deselects all cells in a row as a
//	unit rather than selecting them individually. This fits with the
//	metaphor of an outline which usually presents a single object per row.
//
//	The implementation is pretty simple. It uses the region-based selection
//	mechanism in LTableMultiSelector, but "clips" this region to the first
//	column. CellIsSelected is overriden to return true for any columns if
//	column 1 is selected.

class LOutlineRowSelector : public LOutlineMultiSelector {

public:
						LOutlineRowSelector(
								LOutlineTable*		inOutlineTable);
	virtual				~LOutlineRowSelector();

	// row selection

	virtual void		SelectCell(
								const STableCell&	inCell);
	virtual void		UnselectCell(
								const STableCell&	inCell);

	virtual void		SelectAllCells();
	virtual void		SelectCellBlock(
								const STableCell&	inCellA,
								const STableCell&	inCellB);

	virtual Boolean		CellIsSelected(
								const STableCell&	inCell) const;

	// row & col maintenance

	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol);

	// selection testing

protected:
	virtual void		CheckCells(
								const Rect&			inOldMarqueeRect,
								const Rect&			inNewMarqueeRect);

	// selection drawing

	virtual void		HiliteRow(
								TableIndexT			inRow);

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
