// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMultiSelector.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages the selection for a TableView that can have multiple
//	selected cells
//
//	Strategy: Uses a QuickDraw region to store the selected cells. Points
//	within the region represent selected cells, with the cell row and
//	column being interpreted as the "v" and "h" coords of a Point.
//
//	Since QuickDraw Points use 16-bit numbers, this selector class can
//	only be used for Tables with less than 32K rows and columns.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableMultiSelector.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableMultiSelector					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Associates Selector with a particular TableView

LTableMultiSelector::LTableMultiSelector(
	LTableView*	inTableView)
	
	: LTableSelector(inTableView)
{
	SignalIf_(mTableView == nil);

	mAnchorCell.SetCell(0, 0);
}


// ---------------------------------------------------------------------------
//	¥ ~LTableMultiSelector					Destructor				  [public]
// ---------------------------------------------------------------------------

LTableMultiSelector::~LTableMultiSelector()
{
}

#pragma mark --- Inquiries ---

// ---------------------------------------------------------------------------
//	¥ CellIsSelected
// ---------------------------------------------------------------------------
//	Return whether the specified cell is selected

Boolean
LTableMultiSelector::CellIsSelected(
	const STableCell&	inCell) const
{
	Point	cell;
	inCell.ToPoint(cell);
	return mSelectionRgn.Contains(cell);
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedCell
// ---------------------------------------------------------------------------
//	Return the first selected cell, defined as the min row & col (closest to
//  top-left corner)
//
//	Returns (0,0) if there is no selection

STableCell
LTableMultiSelector::GetFirstSelectedCell() const
{
		// Find first row containing a selected cell

	TableIndexT		selectedRow = GetFirstSelectedRow();
	STableCell		theCell(selectedRow, 0);

	if (selectedRow > 0) {					// Selected Row of zero means
											//   there's no selection

			// Find first selected cell by scanning across the row

		TableIndexT		numRows, numCols;
		mTableView->GetTableSize(numRows, numCols);

		while (++theCell.col < numCols) {	// Note that loop stops BEFORE
											// last column. Row must have
											// a selection. If no other one
											// is selected, the last one
											// must be.
			if (CellIsSelected(theCell)) {
				break;
			}
		}
	}

	return theCell;
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedRow
// ---------------------------------------------------------------------------
//	Return the first selected cell's row, defined as the min row & col (closest
//  to top-left corner)
//
//	Returns 0 if there is no selection

TableIndexT
LTableMultiSelector::GetFirstSelectedRow() const
{
	return mSelectionRgn.Bounds().top;
}

#pragma mark --- Selecting ---

// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------
//	Select the specified Cell
//
//	Does nothing if the specified Cell is invalid

void
LTableMultiSelector::SelectCell(
	const STableCell&	inCell)
{
	if ( mTableView->IsValidCell(inCell) &&
		 not CellIsSelected(inCell) ) {
											// Make region for the Cell
		StRegion	cellRgn((SInt16) inCell.col, 	   (SInt16) inCell.row,
							(SInt16) (inCell.col + 1), (SInt16) (inCell.row + 1));

		mSelectionRgn += cellRgn;			// Add cell to selection

		mAnchorCell = inCell;
		mTableView->HiliteCell(inCell, true);
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectAllCells
// ---------------------------------------------------------------------------
//	Select all the cells in a Table

void
LTableMultiSelector::SelectAllCells()
{
	TableIndexT	rows, cols;
	mTableView->GetTableSize(rows, cols);

	if ((rows > 0) && (cols > 0)) {			// Table has some cells
											// Unhilite existing selection
		mTableView->HiliteSelection(mTableView->IsActive(), false);
											// Put all cells in selection
		::MacSetRectRgn(mSelectionRgn, 1, 1, (SInt16) (cols + 1), (SInt16) (rows + 1));
		mAnchorCell.SetCell(1, 1);
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
LTableMultiSelector::SelectCellBlock(
	const STableCell&	inCellA,
	const STableCell&	inCellB)
{
	TableIndexT	left, top, right, bottom;

	if (inCellA.col < inCellB.col) {
		left = inCellA.col;
		right = inCellB.col + 1;
	} else {
		left = inCellB.col;
		right = inCellA.col + 1;
	}

	if (inCellA.row < inCellB.row) {
		top = inCellA.row;
		bottom = inCellB.row + 1;
	} else {
		top = inCellB.row;
		bottom = inCellA.row + 1;
	}

	::MacSetRectRgn(mSelectionRgn, (SInt16) left, (SInt16) top, (SInt16) right, (SInt16) bottom);

	mTableView->HiliteSelection(mTableView->IsActive(), true);
	mTableView->SelectionChanged();
}

#pragma mark --- Unselecting ---

// ---------------------------------------------------------------------------
//	¥ UnselectCell
// ---------------------------------------------------------------------------
//	Unselected the specified cell

void
LTableMultiSelector::UnselectCell(
	const STableCell&	inCell)
{
	if (CellIsSelected(inCell)) {
											// Make region for the Cell
		StRegion	cellRgn((SInt16) inCell.col,	   (SInt16) inCell.row,
							(SInt16) (inCell.col + 1), (SInt16) (inCell.row + 1));

		mSelectionRgn -= cellRgn;			// Remove Cell from selection

		mTableView->HiliteCell(inCell, false);
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all the cells in a Table

void
LTableMultiSelector::UnselectAllCells()
{
	UnselectAllCells(true);
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all the cells in a Table
//
//	Parameter specifies whether or not to tell the TableView that the
//	selection changed

void
LTableMultiSelector::UnselectAllCells(
	bool	inReportChange)
{
	if (!mSelectionRgn.IsEmpty()) {
		mTableView->HiliteSelection(mTableView->IsActive(), false);
		mSelectionRgn.Clear();
		mAnchorCell.SetCell(0, 0);
		if (inReportChange) {
			mTableView->SelectionChanged();
		}
	}
}

#pragma mark --- Mouse Tracking ---

// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection when clicking on a specified cell

void
LTableMultiSelector::ClickSelect(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
	if (inMouseDown.macEvent.modifiers & cmdKey) {
									// Command Key is down
									//   Allow discontiguous selection
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
				UnselectAllCells(false);
				SelectCell(inCell);
			}
			mAnchorCell = inCell;

		} else {					//   Extend selection from anchor cell
			STableCell	saveAnchor = mAnchorCell;
			UnselectAllCells(false);
			SelectCellBlock(saveAnchor, inCell);
			mAnchorCell = saveAnchor;
		}

	} else {						// No signficant modifier keys
		if (!CellIsSelected(inCell)) {
			UnselectAllCells(false);
			SelectCell(inCell);
		}
		mAnchorCell = inCell;
	}
}


// ---------------------------------------------------------------------------
//	¥ DragSelect
// ---------------------------------------------------------------------------
//	Adjust selection while the user is pressing the mouse in a Table
//
//	Return true if the mouse never leaves inCell

Boolean
LTableMultiSelector::DragSelect(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
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

				STableCell	saveAnchor = mAnchorCell;
				UnselectAllCells(false);
				SelectCellBlock(saveAnchor, hitCell);

				mAnchorCell = saveAnchor;
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
LTableMultiSelector::InsertRows(
	UInt32			inHowMany,
	TableIndexT		inAfterRow)
{
	if (mSelectionRgn.IsEmpty()) return;	// Nothing is selected

		// Strategy: We want to leave the same DATA selected after
		// the insertion. Selected cells above the insertion row
		// remain the same, but those below must shift down by the
		// number of inserted rows.

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);

	StRegion	aboveRgn;					// Part above insertion row
	if (inAfterRow > 0) {
		::MacSetRectRgn(aboveRgn, 1, 1, (SInt16) (cols + 1), (SInt16) (inAfterRow + 1));
		aboveRgn &= mSelectionRgn;
	}

	StRegion	belowRgn;					// Part below insertion row
											// Below = WholeSelection - Above
	::DiffRgn(mSelectionRgn, aboveRgn, belowRgn);
	::MacOffsetRgn(belowRgn, 0, (SInt16) inHowMany);	// Shift down "below" part

											// Join "above" and "below"
	::MacUnionRgn(aboveRgn, belowRgn, mSelectionRgn);

	if (mAnchorCell.row > inAfterRow) {		// Adjust anchor cell
		mAnchorCell.row += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Adjust selection when inserting columns into a Table

void
LTableMultiSelector::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol)
{
	if (mSelectionRgn.IsEmpty()) return;	// Nothing is selected

		// Strategy: We want to leave the same DATA selected after
		// the insertion. Selected cells left of the insertion col
		// remain the same, but those to the right must shift right
		// by the number of inserted cols.

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);


	StRegion	leftRgn;					// Part left of insertion
	if (inAfterCol > 0) {
		::MacSetRectRgn(leftRgn, 1, 1, (SInt16) (inAfterCol + 1), (SInt16) (rows + 1));
		leftRgn &= mSelectionRgn;
	}

	StRegion	rightRgn;					// Part right of insertion
											// Right = WholeSelection - Left
	::DiffRgn(mSelectionRgn, leftRgn, rightRgn);
	::MacOffsetRgn(rightRgn, (SInt16) inHowMany, 0);	// Shift right "right" part

											// Join "left" and "right"
	::MacUnionRgn(leftRgn, rightRgn, mSelectionRgn);

	if (mAnchorCell.col > inAfterCol) {		// Adjust anchor cell
		mAnchorCell.col += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Adjust selection when removing rows from a Table

void
LTableMultiSelector::RemoveRows(
	UInt32			inHowMany,
	TableIndexT		inFromRow)
{
	if (mSelectionRgn.IsEmpty()) return;	// Nothing is selected

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);	//   AFTER remaval

											// Check if removed rows
											//   contain any selected cells
	StRegion	removedRgn((SInt16) 1, 		    (SInt16) inFromRow,
						   (SInt16) (cols + 1), (SInt16) (inFromRow + inHowMany));
	removedRgn &= mSelectionRgn;
	bool	selectedRemoved = !removedRgn.IsEmpty();

	StRegion	aboveRgn;					// Part above rows being removed
	if (inFromRow > 1) {
		::MacSetRectRgn(aboveRgn, 1, 1, (SInt16) (cols + 1), (SInt16) inFromRow);
		aboveRgn &= mSelectionRgn;
	}
											// Part below rows being removed
	StRegion	belowRgn((SInt16) 1,		  (SInt16) (inFromRow + inHowMany),
						 (SInt16) (cols + 1), (SInt16) (rows + inHowMany + 1));
	belowRgn &= mSelectionRgn;

	::MacOffsetRgn(belowRgn, 0, (SInt16) -inHowMany);	// Shift up "below" part

											// Join "above" and "below"
	::MacUnionRgn(aboveRgn, belowRgn, mSelectionRgn);

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

	if (selectedRemoved) {					// Removal changed the selection
		mTableView->SelectionChanged();
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Adjust selection when removing columns from a Table

void
LTableMultiSelector::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{
	if (mSelectionRgn.IsEmpty()) return;	// Nothing is selected

	TableIndexT rows, cols;					// Get dimensions of Table
	mTableView->GetTableSize(rows, cols);	//   AFTER removal

											// Check if removed columns
											//   contain any selected cells
	StRegion	removedRgn((SInt16) inFromCol,			     (SInt16) 1,
						   (SInt16) (inFromCol + inHowMany), (SInt16) (rows + 1));
	removedRgn &= mSelectionRgn;
	bool	selectedRemoved = !removedRgn.IsEmpty();

	StRegion	leftRgn;					// Part left of deletion
	if (inFromCol > 1) {
		::MacSetRectRgn(leftRgn, 1, 1, (SInt16) inFromCol, (SInt16) (rows + 1));
		::SectRgn(leftRgn, mSelectionRgn, leftRgn);
	}
											// Part right of deletion
	StRegion	rightRgn((SInt16) (inFromCol + inHowMany), (SInt16) 1,
						 (SInt16) (cols + inHowMany + 1),  (SInt16) (rows + 1));
	rightRgn &= mSelectionRgn;

	::MacOffsetRgn(rightRgn, (SInt16) -inHowMany, 0);	// Shift "right" right part

	::MacUnionRgn(leftRgn, rightRgn, mSelectionRgn);

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

	if (selectedRemoved) {					// Removal changed the selection
		mTableView->SelectionChanged();
	}
}


PP_End_Namespace_PowerPlant
