// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineRowSelector.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOutlineRowSelector.h>
#include <LOutlineTable.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		* LOutlineRowSelector(LOutlineTable*)
// ---------------------------------------------------------------------------
//	Constructor, specifying the table we're attached to

LOutlineRowSelector::LOutlineRowSelector(
	LOutlineTable*		inOutlineTable)

: LOutlineMultiSelector(inOutlineTable)
{
}


// ---------------------------------------------------------------------------
//		* ~LOutlineRowSelector
// ---------------------------------------------------------------------------
//	Destructor

LOutlineRowSelector::~LOutlineRowSelector()
{
}


// ===========================================================================

#pragma mark -
#pragma mark ** row selection

// ---------------------------------------------------------------------------
//		* SelectCell
// ---------------------------------------------------------------------------
//	Overriden to ignore the column number. Select column 1 in the
//	named row.

void
LOutlineRowSelector::SelectCell(
	const STableCell& inCell)
{
	STableCell leftCell(inCell.row, 1);
	LTableMultiSelector::SelectCell(leftCell);
	HiliteRow(inCell.row);
}


// ---------------------------------------------------------------------------
//		* UnselectCell
// ---------------------------------------------------------------------------
//	Overriden to ignore the column number. Deselect column 1 in the
//	named row.

void
LOutlineRowSelector::UnselectCell(
	const STableCell&	inCell)
{
	STableCell leftCell(inCell.row, 1);
	LTableMultiSelector::UnselectCell(leftCell);
	HiliteRow(inCell.row);
}


// ---------------------------------------------------------------------------
//		* SelectAllCells
// ---------------------------------------------------------------------------
//	Overriden to select all cells in column 1.

void
LOutlineRowSelector::SelectAllCells()
{
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);

	if ((rows > 0) && (cols > 0)) {
		::MacSetRectRgn(mSelectionRgn, 1, 1, 2, (SInt16) (rows + 1));
		mTableView->HiliteSelection(mTableView->IsActive(), true);
		mTableView->SelectionChanged();
	}
}

// ---------------------------------------------------------------------------
//		* SelectCellBlock
// ---------------------------------------------------------------------------
//	Overriden to select all rows that intersect with this cell block.

void
LOutlineRowSelector::SelectCellBlock(
	const STableCell&	inCellA,
	const STableCell&	inCellB)
{
	STableCell leftCellA(inCellA.row, 1);
	STableCell leftCellB(inCellB.row, 1);
	LTableMultiSelector::SelectCellBlock(leftCellA, leftCellB);
}


// ---------------------------------------------------------------------------
//		* CellIsSelected
// ---------------------------------------------------------------------------
//	Overriden to ignore the column number. Returns true if column 1
//	in the named row is selected.

Boolean
LOutlineRowSelector::CellIsSelected(
	const STableCell&	inCell) const
{
	STableCell leftCell(inCell.row, 1);
	return LTableMultiSelector::CellIsSelected(leftCell);
}


// ===========================================================================

#pragma mark -
#pragma mark ** row & col maintenance

// ---------------------------------------------------------------------------
//		* InsertCols
// ---------------------------------------------------------------------------
//	Overriden because inserting columns doesn't affect selection.
//	Does nothing.

void
LOutlineRowSelector::InsertCols(
	UInt32			/* inHowMany */,
	TableIndexT		/* inAfterCol */)
{
	// Ignore because it won't affect selection.
}


// ===========================================================================

#pragma mark -
#pragma mark ** selection testing

// ---------------------------------------------------------------------------
//		* CheckCells					[protected]
// ---------------------------------------------------------------------------
//	Overriden from LOutlineMultiSelector because we want to select a
//	row if any cell in the row is touched by the marque rect.

void
LOutlineRowSelector::CheckCells(
	const Rect&	inOldMarqueeRect,
	const Rect&	inNewMarqueeRect)
{

	// Hit-test all cells in the junction of the old and new marquee.

	Rect testRect;

	if ((inOldMarqueeRect.left | inOldMarqueeRect.right |
	     inOldMarqueeRect.top | inOldMarqueeRect.bottom) == 0)
		testRect = inNewMarqueeRect;
	else {
		testRect = inOldMarqueeRect;
		::MacUnionRect(&testRect, &inNewMarqueeRect, &testRect);
	}

	STableCell topLeftCell, botRightCell;
	mOutlineTable->FetchIntersectingCells(testRect, topLeftCell, botRightCell);

	TableIndexT rows, cols;
	mOutlineTable->GetTableSize(rows, cols);

	// Iterate over all affected cells.

	STableCell cell(1, 1);
	for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {

		// See if the marquee touches this cell's contents.

		Boolean cellIsHitBy = false;
		for (cell.col = 1; cell.col <= cols; cell.col++) {
			if (mOutlineTable->CellHitByMarquee(cell, inNewMarqueeRect))
				cellIsHitBy = true;
		}
		cell.col = 1;

		// Invert that result if shift-dragging.

		Point cellAsPoint;
		cell.ToPoint(cellAsPoint);
		if (::PtInRgn(cellAsPoint, mInvertSelection)) {
			cellIsHitBy = !cellIsHitBy;
		}

		// Update selection regions.

		if (cellIsHitBy && !CellIsSelected(cell)) {
			StRegion	cellRgn((SInt16) cell.col, (SInt16) cell.row, (SInt16) (cell.col + 1), (SInt16) (cell.row + 1));
			::MacUnionRgn(cellRgn, mAddToSelection, mAddToSelection);

		} else if (!cellIsHitBy && CellIsSelected(cell)) {
			StRegion	cellRgn((SInt16) cell.col, (SInt16) cell.row, (SInt16) (cell.col + 1), (SInt16) (cell.row + 1));
			::MacUnionRgn(cellRgn, mRemoveFromSelection, mRemoveFromSelection);
		}
	}
}

// ===========================================================================

#pragma mark -
#pragma mark ** selection drawing

// ---------------------------------------------------------------------------
//		* HiliteRow						[protected]
// ---------------------------------------------------------------------------
//	Called by SelectCell and UnselectCell to cause the selection to be
//	redrawn for an entire row.

void
LOutlineRowSelector::HiliteRow(
	TableIndexT	inRow)
{
	TableIndexT rows, columns;
	mOutlineTable->GetTableSize(rows, columns);

	STableCell cell(inRow, 1);
	Boolean isSelected = CellIsSelected(cell);

	while (cell.col <= columns) {
		mOutlineTable->HiliteCell(cell, isSelected);
		cell.col++;
	}
}

PP_End_Namespace_PowerPlant

