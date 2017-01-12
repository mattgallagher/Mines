// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableSingleSelector.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages the selection for a TableView than can have only one selected cell

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableSingleSelector.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableSingleSelector					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Associates Selector with a particular TableView

LTableSingleSelector::LTableSingleSelector(
	LTableView*	inTableView)
	
	: LTableSelector(inTableView)
{
	SignalIf_(mTableView == nil);
}


// ---------------------------------------------------------------------------
//	¥ ~LTableSingleSelector					Destructor				  [public]
// ---------------------------------------------------------------------------

LTableSingleSelector::~LTableSingleSelector()
{
}

#pragma mark --- Inquiries ---

// ---------------------------------------------------------------------------
//	¥ CellIsSelected
// ---------------------------------------------------------------------------
//	Return whether the specified cell is selected

Boolean
LTableSingleSelector::CellIsSelected(
	const STableCell&	inCell) const
{
	return (inCell == mSelectedCell);
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedCell
// ---------------------------------------------------------------------------
//	Return the first selected cell, defined as the min row & col (closest to
//  top-left corner)

STableCell
LTableSingleSelector::GetFirstSelectedCell() const
{
	return mSelectedCell;
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedRow
// ---------------------------------------------------------------------------
//	Return the first selected cell's row, defined as the min row & col (closest to
//  top-left corner)

TableIndexT
LTableSingleSelector::GetFirstSelectedRow() const
{
	return mSelectedCell.row;
}

#pragma mark --- Selecting ---

// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------
//	Select the specified Cell
//
//	Since only one cell can be selected, this deselects the currently
//	selected cell
//
//	Does nothing if the specified Cell is invalid

void
LTableSingleSelector::SelectCell(
	const STableCell&	inCell)
{
	if ( mTableView->IsValidCell(inCell) &&
		 not CellIsSelected(inCell) ) {
		 
		mTableView->HiliteCell(mSelectedCell, false);
		mSelectedCell = inCell;
		mTableView->HiliteCell(inCell, true);
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
LTableSingleSelector::SelectAllCells()
{
	SignalStringLiteral_("Can't select all with a SingleSelector");
}

#pragma mark --- Unselecting ---

// ---------------------------------------------------------------------------
//	¥ UnselectCell
// ---------------------------------------------------------------------------
//	Unselect the specified cell

void
LTableSingleSelector::UnselectCell(
	const STableCell&	inCell)
{
	if (inCell == mSelectedCell) {
		mTableView->HiliteCell(mSelectedCell, false);
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
LTableSingleSelector::UnselectAllCells()
{
	mTableView->HiliteCell(mSelectedCell, false);
	mSelectedCell.row = 0;
	mSelectedCell.col = 0;
	mTableView->SelectionChanged();
}

#pragma mark --- Mouse Tracking ---

// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection when clicking on a specified cell

void
LTableSingleSelector::ClickSelect(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
	if (inCell == mSelectedCell) {		// Cell is already selected

		if ( (inMouseDown.macEvent.modifiers & cmdKey) ||
			 (inMouseDown.macEvent.modifiers & shiftKey) ) {

			 							// Command or Shift key is down
			 UnselectCell(inCell);		//   so unselect the cell
		}								// Otherwise, do nothing

	} else {							// All clicks on an unselected cell,
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
LTableSingleSelector::DragSelect(
	const STableCell&		inCell,
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

#pragma mark --- Adjusting Selection ---

// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Adjust selection when inserting rows into a Table

void
LTableSingleSelector::InsertRows(
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
LTableSingleSelector::InsertCols(
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
LTableSingleSelector::RemoveRows(
	UInt32			inHowMany,
	TableIndexT		inFromRow)
{
	if (inFromRow <= mSelectedCell.row) {

		if ((inFromRow + inHowMany - 1) >= mSelectedCell.row) {

				// Row containing selected cell is being removed,
				// so make selection empty

			mSelectedCell.SetCell(0, 0);
			mTableView->SelectionChanged();

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
LTableSingleSelector::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{
	if (inFromCol <= mSelectedCell.col) {

		if ((inFromCol + inHowMany - 1) >= mSelectedCell.col) {

				// Column containing selected cell is being removed,
				// so make selection empty

			mSelectedCell.SetCell(0, 0);
			mTableView->SelectionChanged();

		} else {

				// Columns before the selected cell are being removed.
				// Adjust column number of selection so that the same
				// logical cell remains selected.

			mSelectedCell.col -= inHowMany;
		}
	}
}


PP_End_Namespace_PowerPlant
