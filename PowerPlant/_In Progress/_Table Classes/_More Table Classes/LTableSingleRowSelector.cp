// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableSingleRowSelector.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTableSingleRowSelector.h>
#include <LTableSelState.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant



// ---------------------------------------------------------------------------
//	¥ LTableSingleRowSelector
// ---------------------------------------------------------------------------
//	Constructor, associates Selector with a particular TableView

LTableSingleRowSelector::LTableSingleRowSelector(
	LTableView	*inTableView)
		: LTableSelector(inTableView)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTableSingleRowSelector
// ---------------------------------------------------------------------------
//	Destructor

LTableSingleRowSelector::~LTableSingleRowSelector()
{
}


// ---------------------------------------------------------------------------
//	¥ CellIsSelected
// ---------------------------------------------------------------------------
//	Return whether the specified cell is selected

Boolean
LTableSingleRowSelector::CellIsSelected(
	const STableCell	&inCell) const
{
	return (inCell.row == mSelectedCell.row);
}

#pragma mark --- Selecting ---

// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedCell
// ---------------------------------------------------------------------------
//	Return the first selected cell, defined as the min row & col (closest to
//  top-left corner)

STableCell
LTableSingleRowSelector::GetFirstSelectedCell() const
{
	return (mSelectedCell);
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedRow
// ---------------------------------------------------------------------------
//	Return the first selected cell's row, defined as the min row & col (closest to
//  top-left corner)

TableIndexT
LTableSingleRowSelector::GetFirstSelectedRow() const
{
	return mSelectedCell.row;
}


// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------
//	Select the specified Cell
//
//	Since only one cell can be selected, this deselects the currently
//	selected cell

void
LTableSingleRowSelector::SelectCell(
	const STableCell	&inCell)
{
	if (inCell.row != mSelectedCell.row) {
		HiliteRow(mSelectedCell.row, false);
		mSelectedCell = inCell;
		HiliteRow(inCell.row, true);
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectAllCells
// ---------------------------------------------------------------------------
//	Select all the cells in a Table
//
//	Since only one cell can be selected, this operation is not supported

void
LTableSingleRowSelector::SelectAllCells()
{
	SignalStringLiteral_("Can't select all with a SingleSelector");
}


// ---------------------------------------------------------------------------
//	¥ UnselectCell
// ---------------------------------------------------------------------------
//	Unselected the specified cell

void
LTableSingleRowSelector::UnselectCell(
	const STableCell	&inCell)
{
	if (inCell.row == mSelectedCell.row) {
		HiliteRow(mSelectedCell.row, false);
		mSelectedCell.row = 0;			// No cell is selected now
		mSelectedCell.col = 0;
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all the cells in a Table

void
LTableSingleRowSelector::UnselectAllCells()
{
	HiliteRow(mSelectedCell.row, false);
	mSelectedCell.row = 0;
	mSelectedCell.col = 0;
	mTableView->SelectionChanged();
}


// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection when clicking on a specified cell

void
LTableSingleRowSelector::ClickSelect(
	const STableCell		&inCell,
	const SMouseDownEvent&	inMouseDown)
{
	if (inCell.row == mSelectedCell.row) {		// Cell is already selected

		if ( (inMouseDown.macEvent.modifiers & cmdKey) ||
			 (inMouseDown.macEvent.modifiers & shiftKey) ) {

			 							// Command or Shift key is down
			 UnselectCell(inCell);		//   so unselect the cell
		}								// Otherwise, do nothing

	}
	else {							// All clicks on an unselected cell,
		SelectCell(inCell);				//   regardless of modifier keys,
	}									//   cause cell to be selected
}


// ---------------------------------------------------------------------------
//	¥ DragSelect
// ---------------------------------------------------------------------------
//	Adjust selection while the user is pressing the mouse in a Table
//
//	Return true if the mouse never leaves inCell

Boolean
LTableSingleRowSelector::DragSelect(
	const STableCell		&inCell,
	const SMouseDownEvent&	/* inMouseDown */)
{
	Boolean		inSameCell = true;

	SelectCell(inCell);

	STableCell	currCell = inCell;

	while (::StillDown()) {			// Track mouse and select the cell
		STableCell	hitCell;		//   currently containing the mouse
		SPoint32	imageLoc;
		Point		mouseLoc;
		mTableView->FocusDraw();
		::GetMouse(&mouseLoc);
		if (mTableView->AutoScrollImage(mouseLoc)) {
			mTableView->FocusDraw();
			Rect	frame;
			mTableView->CalcLocalFrameRect(frame);
			SInt32 pt = ::PinRect(&frame, mouseLoc);
			mouseLoc = *(Point*)&pt;
		}
		mTableView->LocalToImagePoint(mouseLoc, imageLoc);
		mTableView->GetCellHitBy(imageLoc, hitCell);

		if (currCell != hitCell) {
			inSameCell = false;

			if (mTableView->IsValidCell(hitCell)) {
				SelectCell(hitCell);
				currCell = hitCell;
			}
		}
	}

	return inSameCell;
}


// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Adjust selection when inserting rows into a Table

void
LTableSingleRowSelector::InsertRows(
	UInt32			inHowMany,
	TableIndexT		inAfterRow)
{
	if (inAfterRow < mSelectedCell.row) {

		// Inserting rows before the current selection. Adjust
		// row number of the selection so that the same logical
		// cell remains selected

		mSelectedCell.row += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Adjust selection when inserting columns into a Table

void
LTableSingleRowSelector::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol)
{
	if (inAfterCol < mSelectedCell.col) {

		// Inserting columns before the current selection. Adjust
		// column number of the selection so that the same logical
		// cell remains selected

		mSelectedCell.col += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Adjust selection when removing rows from a Table

void
LTableSingleRowSelector::RemoveRows(
	UInt32			inHowMany,
	TableIndexT		inFromRow)
{
	if (inFromRow <= mSelectedCell.row) {

		if ((inFromRow + inHowMany - 1) >= mSelectedCell.row) {

				// Row containing selected cell is being removed,
				// so make selection empty

			mSelectedCell.SetCell(0, 0);

		} else {

				// Rows before the selected cell are being removed.
				// Adjust row number of selection so that the same
				// logical cell remains selected.

			mSelectedCell.row -= inHowMany;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Adjust selection when removing columns from a Table

void
LTableSingleRowSelector::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{
	if (inFromCol <= mSelectedCell.col) {

		if ((inFromCol + inHowMany - 1) >= mSelectedCell.col) {

				// Column containing selected cell is being removed,
				// so make selection empty

			mSelectedCell.SetCell(0, 0);

		} else {

				// Columns before the selected cell are being removed.
				// Adjust column number of selection so that the same
				// logical cell remains selected.

			mSelectedCell.col -= inHowMany;
		}
	}
}

// ---------------------------------------------------------------------------
//	¥ HiliteRow												[protected]
// ---------------------------------------------------------------------------
//	Called by SelectCell and UnselectCell to cause the selection to be
//	redrawn for an entire row.

void
LTableSingleRowSelector::HiliteRow(
	TableIndexT	inRow, Boolean inSelected)
{
	TableIndexT rows, columns;
	mTableView->GetTableSize(rows, columns);

	STableCell cell(inRow, 1);
	while (cell.col <= columns) {
		mTableView->HiliteCell(cell, inSelected);
		cell.col++;
	}
}


#pragma mark --- Saved Selections ---

// ---------------------------------------------------------------------------
//	¥ Set & GetSavedSelection
// ---------------------------------------------------------------------------
LTableSelState*
LTableSingleRowSelector::GetSavedSelection() const
{
	LTableSelState* ret = new LTableSingleRowState(mSelectedCell.row);
	return ret;
}


void
LTableSingleRowSelector::SetSavedSelection(const LTableSelState* inSS)
{
	const LTableSingleRowState*	realState = dynamic_cast<const LTableSingleRowState*> (inSS);
	Assert_(realState);	// we should have manufactured it, so it should be correct type
	TableIndexT inRow = realState->GetRow();
	STableCell newSelCell(inRow);
	SelectCell(newSelCell);
}

PP_End_Namespace_PowerPlant
