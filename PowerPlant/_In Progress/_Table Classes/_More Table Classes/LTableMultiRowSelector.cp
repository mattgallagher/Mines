// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMultiRowSelector.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableMultiRowSelector.h>
#include <LTableSelState.h>
#include <UTBAccessors.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableMultiRowSelector
// ---------------------------------------------------------------------------
//	Constructor, associates Selector with a particular TableView

LTableMultiRowSelector::LTableMultiRowSelector(
	LTableView*		inTableView)

	: LTableSelector(inTableView)
{
	mSelectionRgn = ::NewRgn();
}


// ---------------------------------------------------------------------------
//	¥ ~LTableMultiRowSelector
// ---------------------------------------------------------------------------
//	Destructor

LTableMultiRowSelector::~LTableMultiRowSelector()
{
	::DisposeRgn(mSelectionRgn);
}


// ---------------------------------------------------------------------------
//	¥ CellIsSelected
// ---------------------------------------------------------------------------
//	Return whether the specified cell is selected

Boolean
LTableMultiRowSelector::CellIsSelected(
	const STableCell	&inCell) const
{
	Point	cell;
	inCell.ToPoint(cell);
	cell.h = 1;
	return (::PtInRgn(cell, mSelectionRgn));
}

#pragma mark --- Selecting ---

// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedCell
// ---------------------------------------------------------------------------
//	Return the first selected cell, defined as the min row & col (closest to
//  top-left corner)

STableCell
LTableMultiRowSelector::GetFirstSelectedCell() const
{
	Point ret = {0,0};
	if (!::EmptyRgn(mSelectionRgn)) {
		Rect	rb;
		::GetRegionBounds(mSelectionRgn, &rb);

		ret.v = rb.top;
		ret.h = 1;
	}
	return STableCell(ret);
}



// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedRow
// ---------------------------------------------------------------------------
//	Return the first selected cell's row, defined as the min row & col (closest to
//  top-left corner)

TableIndexT
LTableMultiRowSelector::GetFirstSelectedRow() const
{
	TableIndexT ret = 0;	// no selection
	if (!::EmptyRgn(mSelectionRgn)) {
		Rect	rb;
		::GetRegionBounds(mSelectionRgn, &rb);
		ret = rb.top;
	}
	return ret;
}


// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------
//	Select the specified Cell

void
LTableMultiRowSelector::SelectCell(
	const STableCell	&inCell)
{
	if (!CellIsSelected(inCell)) {
		RgnHandle	cellRgn = ::NewRgn();
		::MacSetRectRgn(cellRgn, 1, (SInt16) inCell.row,
							  2, (SInt16) (inCell.row + 1));
		::MacUnionRgn(cellRgn, mSelectionRgn, mSelectionRgn);
		::DisposeRgn(cellRgn);

		HiliteRow(inCell.row, true);
		mTableView->SelectionChanged();
		mAnchorCell = inCell;
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectAllCells
// ---------------------------------------------------------------------------
//	Select all the cells in a Table

void
LTableMultiRowSelector::SelectAllCells()
{
	TableIndexT	rows, cols;
	mTableView->GetTableSize(rows, cols);

	if ((rows > 0) && (cols > 0)) {
		::MacSetRectRgn(mSelectionRgn, 1, 1, (SInt16) (cols + 1), (SInt16) (rows + 1));
		mTableView->HiliteSelection(mTableView->IsActive(), true);
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectCellBlock
// ---------------------------------------------------------------------------
//	Select cells within the specified range, where CellA and CellB are
//	the corners of a rectangular block of cells.

void
LTableMultiRowSelector::SelectCellBlock(
	const STableCell	&inCellA,
	const STableCell	&inCellB)
{
	TableIndexT	left, top, right, bottom;
	left = 1;
	right = 2;
	if (inCellA.row < inCellB.row) {
		top = inCellA.row;
		bottom = inCellB.row + 1;
	} else {
		top = inCellB.row;
		bottom = inCellA.row + 1;
	}

	::MacSetRectRgn(mSelectionRgn, (SInt16) left, (SInt16) top,(SInt16)  right, (SInt16) bottom);

	mTableView->HiliteSelection(mTableView->IsActive(), true);
	mTableView->SelectionChanged();
}

#pragma mark --- Unselecting ---

// ---------------------------------------------------------------------------
//	¥ UnselectCell
// ---------------------------------------------------------------------------
//	Unselected the specified cell

void
LTableMultiRowSelector::UnselectCell(
	const STableCell	&inCell)
{
	if (CellIsSelected(inCell)) {
		RgnHandle	cellRgn = ::NewRgn();
		::MacSetRectRgn(cellRgn, 1, (SInt16) inCell.row,
							  2, (SInt16) (inCell.row + 1));

		::DiffRgn(mSelectionRgn, cellRgn, mSelectionRgn);
		::DisposeRgn(cellRgn);

		HiliteRow(inCell.row, false);
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all the cells in a Table

void
LTableMultiRowSelector::UnselectAllCells()
{
	if (!::EmptyRgn(mSelectionRgn)) {
		mTableView->HiliteSelection(mTableView->IsActive(), false);
		::SetEmptyRgn(mSelectionRgn);
		mTableView->SelectionChanged();
	}
}

#pragma mark --- Mouse Tracking ---

// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection when clicking on a specified cell

void
LTableMultiRowSelector::ClickSelect(
	const STableCell		&inCell,
	const SMouseDownEvent	&inMouseDown)
{
	if (inMouseDown.macEvent.modifiers & cmdKey) {
									// Command Key is down
									//   Allow discontinguous selection
		if (CellIsSelected(inCell)) {
			UnselectCell(inCell);	//   Remove Cell from selection
			mAnchorCell.SetCell(0, 0);
		} else {
			SelectCell(inCell);		//   Add Cell to selection
		}

	} else if (inMouseDown.macEvent.modifiers & shiftKey) {
									// Shift Key is down
		if (mAnchorCell.IsNullCell()) {
			if (!CellIsSelected(inCell)) {
				UnselectAllCells();
				SelectCell(inCell);
			}

		} else {					//   Extend selection from anchor cell
			UnselectAllCells();
			SelectCellBlock(mAnchorCell, inCell);
		}

	} else {
		mAnchorCell = inCell;
		if (!CellIsSelected(inCell)) {
			UnselectAllCells();
			SelectCell(inCell);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DragSelect
// ---------------------------------------------------------------------------
//	Adjust selection while the user is pressing the mouse in a Table
//
//	Return true if the mouse never leaves inCell

Boolean
LTableMultiRowSelector::DragSelect(
	const STableCell		&inCell,
	const SMouseDownEvent	&inMouseDown)
{
	Boolean		inSameCell = true;

	ClickSelect(inCell, inMouseDown);	// First handle as a normal click

	if (inMouseDown.macEvent.modifiers & cmdKey) {

			// I really have no good idea what to do when the
			// command key is down. Should it toggle the selection
			// of the cells dragged over? Should it set a mode
			// based on the state of the cell originally clicked,
			// and then continue selecting or deselecting the cells
			// dragged over?

			// For the moment, do the simple thing and not support
			// drag select when the command key is down.

		return true;
	}

	STableCell	currCell = inCell;

	while (::StillDown()) {
		STableCell	hitCell;
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

				// Right now, all cells between the Anchor cell and
				// Curr cell are selected. New selection will be
				// all cells between the Anchor and the Hit cell.

				// ### This method of unhiliting everything and
				// then hiliting the proper selection will cause
				// a lot of flashing. But it works, and we'll
				// implement a better algorithm in the future.

				UnselectAllCells();
				SelectCellBlock(mAnchorCell, hitCell);

				currCell = hitCell;
			}
		}
	}

	return inSameCell;
}

#pragma mark --- Adjusting Selection ---

// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Adjust selection when inserting rows into a Table

void
LTableMultiRowSelector::InsertRows(
	UInt32			inHowMany,
	TableIndexT		inAfterRow)
{
	if (::EmptyRgn(mSelectionRgn)) return;	// Nothing is selected

		// Strategy: We want to leave the same DATA selected after
		// the insertion. Selected cells above the insertion row
		// remain the same, but those below must shift down by the
		// number of inserted rows.

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);

	RgnHandle	aboveRgn = ::NewRgn();		// Part above insertion row
	if (inAfterRow > 0) {
		::MacSetRectRgn(aboveRgn, 1, 1, (SInt16) (cols + 1), (SInt16) (inAfterRow + 1));
		::SectRgn(aboveRgn, mSelectionRgn, aboveRgn);
	}

	RgnHandle	belowRgn = ::NewRgn();		// Part below insertion row
											// Below = WholeSelection - Above
	::DiffRgn(mSelectionRgn, aboveRgn, belowRgn);
	::MacOffsetRgn(belowRgn, 0, (SInt16) inHowMany);	// Shift down "below" part

											// Join "above" and "below"
	::MacUnionRgn(aboveRgn, belowRgn, mSelectionRgn);

	::DisposeRgn(aboveRgn);
	::DisposeRgn(belowRgn);

	if (mAnchorCell.row > inAfterRow) {		// Adjust anchor cell
		mAnchorCell.row += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Adjust selection when inserting columns into a Table

void
LTableMultiRowSelector::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol)
{
	if (::EmptyRgn(mSelectionRgn)) return;	// Nothing is selected

		// Strategy: We want to leave the same DATA selected after
		// the insertion. Selected cells left of the insertion col
		// remain the same, but those to the right must shift right
		// by the number of inserted cols.

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);

	RgnHandle	leftRgn = ::NewRgn();		// Part left of insertion
	if (inAfterCol > 0) {
		::MacSetRectRgn(leftRgn, 1, 1, (SInt16) (inAfterCol + 1), (SInt16) (rows + 1));
		::SectRgn(leftRgn, mSelectionRgn, leftRgn);
	}

	RgnHandle	rightRgn = ::NewRgn();		// Part right of insertion
											// Right = WholeSelection - Left
	::DiffRgn(mSelectionRgn, leftRgn, rightRgn);
	::MacOffsetRgn(rightRgn, (SInt16) inHowMany, 0);	// Shift right "right" part

											// Join "left" and "right"
	::MacUnionRgn(leftRgn, rightRgn, mSelectionRgn);

	::DisposeRgn(leftRgn);
	::DisposeRgn(rightRgn);

	if (mAnchorCell.col > inAfterCol) {		// Adjust anchor cell
		mAnchorCell.col += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Adjust selection when removing rows from a Table

void
LTableMultiRowSelector::RemoveRows(
	UInt32			inHowMany,
	TableIndexT		inFromRow)
{
	if (::EmptyRgn(mSelectionRgn)) return;	// Nothing is selected

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);

	RgnHandle	aboveRgn = ::NewRgn();		// Part above deletion row
	if (inFromRow > 1) {
		::MacSetRectRgn(aboveRgn, 1, 1, (SInt16) (cols + 1), (SInt16) inFromRow);
		::SectRgn(aboveRgn, mSelectionRgn, aboveRgn);
	}

	RgnHandle	belowRgn = ::NewRgn();		// Part below deletion rows
	::MacSetRectRgn(belowRgn,
					1, (SInt16) (inFromRow + inHowMany),
					(SInt16) (cols + 1), (SInt16) (rows + inHowMany + 1));
	::SectRgn(belowRgn, mSelectionRgn, belowRgn);

	::MacOffsetRgn(belowRgn, 0, (SInt16) -inHowMany);	// Shift up "below" part

											// Join "above" and "below"
	::MacUnionRgn(aboveRgn, belowRgn, mSelectionRgn);

	::DisposeRgn(aboveRgn);
	::DisposeRgn(belowRgn);

	if (inFromRow <= mAnchorCell.row) {		// Adjust anchor cell

		if ((inFromRow + inHowMany - 1) >= mAnchorCell.row) {

				// Row containing anchor cell is being removed,
				// so make anchor cell null.

			mAnchorCell.SetCell(0, 0);

		} else {

				// Rows before the anchor cell are being removed.
				// Adjust row number of anchor cell so that the same
				// logical cell remains the anchor.

			mAnchorCell.row -= inHowMany;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Adjust selection when removing columns from a Table

void
LTableMultiRowSelector::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{
	if (::EmptyRgn(mSelectionRgn)) return;	// Nothing is selected

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);

	RgnHandle	leftRgn = ::NewRgn();		// Part left of deletion
	if (inFromCol > 1) {
		::MacSetRectRgn(leftRgn, 1, 1, (SInt16) inFromCol, (SInt16) (rows + 1));
		::SectRgn(leftRgn, mSelectionRgn, leftRgn);
	}

	RgnHandle	rightRgn = ::NewRgn();		// Part right of deletion
	::MacSetRectRgn(rightRgn,
					(SInt16) (inFromCol + inHowMany), 1,
					(SInt16) (cols + inHowMany + 1), (SInt16) (rows + 1));
	::SectRgn(rightRgn, mSelectionRgn, rightRgn);

	::MacOffsetRgn(rightRgn, (SInt16) -inHowMany, 0);	// Shift "right" right part

	::MacUnionRgn(leftRgn, rightRgn, mSelectionRgn);

	::DisposeRgn(leftRgn);
	::DisposeRgn(rightRgn);

	if (inFromCol <= mAnchorCell.col) {		// Adjust anchor cell

		if ((inFromCol + inHowMany - 1) >= mAnchorCell.col) {

				// Column containing anchor cell is being removed,
				// so make the anchor cell null

			mAnchorCell.SetCell(0, 0);

		} else {

				// Columns before the anchor cell are being removed.
				// Adjust column number of the anchor cell so that the
				// same logical cell remains the anchor.

			mAnchorCell.col -= inHowMany;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteRow												[protected]
// ---------------------------------------------------------------------------
//	Called by SelectCell and UnselectCell to cause the selection to be
//	redrawn for an entire row.

void
LTableMultiRowSelector::HiliteRow(
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
LTableMultiRowSelector::GetSavedSelection() const
{
	LTableMultiRowState*	ret = new LTableMultiRowState(mAnchorCell, mSelectionRgn);
	return ret;
}


void
LTableMultiRowSelector::SetSavedSelection(const LTableSelState* inSS)
{
	const LTableMultiRowState*	realState = dynamic_cast<const LTableMultiRowState*> (inSS);
	Assert_(realState);	// we should have manufactured it, so it should be correct type
	mAnchorCell = realState->GetAnchorCell();
	RgnHandle savedRgn = realState->GetRegion();
	::MacCopyRgn(savedRgn, mSelectionRgn);
	mTableView->SelectionChanged();
}

PP_End_Namespace_PowerPlant
