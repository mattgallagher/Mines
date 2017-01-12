// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableSingleRowSelector.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	Manages the selection for a TableView than can have only one selected row

#ifndef _H_LTableSingleRowSelector
#define _H_LTableSingleRowSelector
#pragma once

#include <UTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


class LTableSelState;

class	LTableSingleRowSelector : public LTableSelector {
public:
						LTableSingleRowSelector(
								LTableView				*inTableView);
	virtual				~LTableSingleRowSelector();

	virtual Boolean		CellIsSelected(
								const STableCell		&inCell) const;

	LTableSelState*	GetSavedSelection() const;
	void	SetSavedSelection(const LTableSelState*);

	virtual	TableIndexT	GetFirstSelectedRow() const;
	virtual	STableCell	GetFirstSelectedCell() const;

	virtual void		SelectCell(
								const STableCell		&inCell);
	virtual void		SelectAllCells();
	virtual void		UnselectCell(
								const STableCell		&inCell);
	virtual void		UnselectAllCells();

	virtual void		ClickSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown);
	virtual Boolean		DragSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown);

	virtual void		InsertRows(
								UInt32					inHowMany,
								TableIndexT				inAfterRow);
	virtual void		InsertCols(
								UInt32					inHowMany,
								TableIndexT				inAfterCol);
	virtual void		RemoveRows(
								UInt32					inHowMany,
								TableIndexT				inFromRow);
	virtual void		RemoveCols(
								UInt32					inHowMany,
								TableIndexT				inFromCol);


protected:
	virtual void		HiliteRow(
								TableIndexT			inRow,
								Boolean inSelected);

	STableCell		mSelectedCell;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
