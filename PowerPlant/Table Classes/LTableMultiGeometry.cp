// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMultiGeometry.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages geometry of a TableView where rows and columns may have
//	different sizes

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableMultiGeometry.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableMultiGeometry					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTableMultiGeometry::LTableMultiGeometry(
	LTableView*		inTableView,
	UInt16			inColWidth,
	UInt16			inRowHeight)

	: LTableGeometry(inTableView),
	  mRowHeights(sizeof(UInt16)),
	  mColWidths(sizeof(UInt16))
{
	SignalIf_(mTableView == nil);

	mDefaultRowHeight = inRowHeight;
	mDefaultColWidth = inColWidth;

	TableIndexT		numRows, numCols;
	inTableView->GetTableSize(numRows, numCols);

	if (numRows > 0) {
		mRowHeights.InsertItemsAt(numRows, 1, &inRowHeight);
	}

	if (numCols > 0) {
		mColWidths.InsertItemsAt(numCols, 1, &inColWidth);
	}

	SPoint32	theScrollUnit;
	theScrollUnit.h = inColWidth;
	theScrollUnit.v = inRowHeight;
	inTableView->SetScrollUnit(theScrollUnit);
}


// ---------------------------------------------------------------------------
//	¥ ~LTableMultiGeometry					Destructor				  [public]
// ---------------------------------------------------------------------------

LTableMultiGeometry::~LTableMultiGeometry()
{
}


// ---------------------------------------------------------------------------
//	¥ GetImageCellBounds
// ---------------------------------------------------------------------------
//	Pass back the location in Image coords of the specified Cell

void
LTableMultiGeometry::GetImageCellBounds(
	const STableCell&	inCell,
	SInt32&				outLeft,
	SInt32&				outTop,
	SInt32&				outRight,
	SInt32&				outBottom) const
{
	outLeft		= 0;
	outRight	= 0;
	outTop		= 0;
	outBottom	= 0;

	if (mTableView->IsValidCell(inCell)) {

		for (TableIndexT row = 1; row < inCell.row; row++) {
			outTop += *(UInt16*) mRowHeights.GetItemPtr((SInt32) row);
		}
		outBottom = outTop + *(UInt16*) mRowHeights.GetItemPtr((SInt32) inCell.row);

		for (TableIndexT col = 1; col < inCell.col; col++) {
			outLeft += *(UInt16*) mColWidths.GetItemPtr((SInt32) col);
		}
		outRight = outLeft + *(UInt16*) mColWidths.GetItemPtr((SInt32) inCell.col);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetRowHitBy
// ---------------------------------------------------------------------------
//	Return the index number of the Row containing the specified point

TableIndexT
LTableMultiGeometry::GetRowHitBy(
	const SPoint32&		inImagePt) const
{
	TableIndexT		hitRow;
	TableIndexT		numRows = mRowHeights.GetCount();
	SInt32			vert = 0;

	for (hitRow = 1; hitRow <= numRows; hitRow++) {
		vert += *(UInt16*) mRowHeights.GetItemPtr((SInt32) hitRow);
		if (vert > inImagePt.v) break;
	}

	return hitRow;
}


// ---------------------------------------------------------------------------
//	¥ GetColHitBy
// ---------------------------------------------------------------------------
//	Return the index number of the Column containing the specified point

TableIndexT
LTableMultiGeometry::GetColHitBy(
	const SPoint32&		inImagePt) const
{
	TableIndexT		hitCol;
	TableIndexT		numCols = mColWidths.GetCount();
	SInt32			horiz = 0;

	for (hitCol = 1; hitCol <= numCols; hitCol++) {
		horiz += *(UInt16*) mColWidths.GetItemPtr((SInt32) hitCol);
		if (horiz > inImagePt.h) break;
	}

	return hitCol;
}


// ---------------------------------------------------------------------------
//	¥ GetTableDimensions
// ---------------------------------------------------------------------------
//	Pass back size of the Table based on the number and size of the rows
//	and columns

void
LTableMultiGeometry::GetTableDimensions(
	UInt32&		outWidth,
	UInt32&		outHeight) const
{
	UInt32	numRows, numCols;
	mTableView->GetTableSize(numRows, numCols);

	outHeight = 0;
	for (TableIndexT row = 1; row <= numRows; row++) {
		outHeight += *(UInt16*) mRowHeights.GetItemPtr((SInt32) row);
	}

	outWidth = 0;
	for (TableIndexT col = 1; col <= numCols; col++) {
		outWidth += *(UInt16*) mColWidths.GetItemPtr((SInt32) col);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetRowHeight
// ---------------------------------------------------------------------------
//	Return the height of the specified row

UInt16
LTableMultiGeometry::GetRowHeight(
	TableIndexT		inRow) const
{
	UInt16	height = 0;

	if (mTableView->IsValidRow(inRow)) {
		mRowHeights.FetchItemAt((SInt32) inRow, &height);
	}

	return height;
}


// ---------------------------------------------------------------------------
//	¥ SetRowHeight
// ---------------------------------------------------------------------------
//	Set the height of the specified rows

void
LTableMultiGeometry::SetRowHeight(
	UInt16			inHeight,
	TableIndexT		inFromRow,
	TableIndexT		inToRow)
{
	if ( (inToRow >= inFromRow) &&				// Check parameters
		 mTableView->IsValidRow(inFromRow) &&
		 mTableView->IsValidRow(inToRow) ) {

		 	// Cells starting at inFromRow and below must be refreshed.
		 	// We refresh both before and after changing the heights
		 	// to be sure that the proper area is refreshed.

		STableCell	fromCell(inFromRow, 1);
		STableCell	lastCell;
		mTableView->GetTableSize(lastCell.row, lastCell.col);

		mTableView->RefreshCellRange(fromCell, lastCell);

		mRowHeights.AssignItemsAt(inToRow - inFromRow + 1, (SInt32) inFromRow,
									&inHeight);

		mTableView->RefreshCellRange(fromCell, lastCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetColWidth
// ---------------------------------------------------------------------------
//	Return the width of the specified column

UInt16
LTableMultiGeometry::GetColWidth(
	TableIndexT		inCol) const
{
	UInt16	width = 0;

	if (mTableView->IsValidCol(inCol)) {
		mColWidths.FetchItemAt((SInt32) inCol, &width);
	}

	return width;
}


// ---------------------------------------------------------------------------
//	¥ SetColWidth
// ---------------------------------------------------------------------------
//	Set the width of the specified columns

void
LTableMultiGeometry::SetColWidth(
	UInt16			inWidth,
	TableIndexT		inFromCol,
	TableIndexT		inToCol)
{
	if ( (inToCol >= inFromCol) &&				// Check parameters
		 mTableView->IsValidCol(inFromCol) &&
		 mTableView->IsValidCol(inToCol) ) {

		 	// Cells starting at inFromCol and to the right must
		 	// be refreshed. We refresh both before and after changing
		 	// the widths to be sure that the proper area is refreshed.

		STableCell	fromCell(1, inFromCol);
		STableCell	lastCell;
		mTableView->GetTableSize(lastCell.row, lastCell.col);

		mTableView->RefreshCellRange(fromCell, lastCell);

		mColWidths.AssignItemsAt(inToCol - inFromCol + 1, (SInt32) inFromCol,
									&inWidth);

		mTableView->RefreshCellRange(fromCell, lastCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Insert specified number of rows into Geometry after a certain row

void
LTableMultiGeometry::InsertRows(
	UInt32			inHowMany,
	TableIndexT		inAfterRow)
{
	mRowHeights.InsertItemsAt(inHowMany, (SInt32) inAfterRow + 1, &mDefaultRowHeight);
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Insert specified number of columns into Geometry after a certain column

void
LTableMultiGeometry::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol)
{
	mColWidths.InsertItemsAt(inHowMany, (SInt32) inAfterCol + 1, &mDefaultColWidth);
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Remove specified number of rows from Geometry starting from a certain row

void
LTableMultiGeometry::RemoveRows(
	UInt32			inHowMany,
	TableIndexT		inFromRow)
{
	mRowHeights.RemoveItemsAt(inHowMany, (SInt32) inFromRow);
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Remove specified number of cooumns from Geometry starting from a
//	certain column

void
LTableMultiGeometry::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{
	mColWidths.RemoveItemsAt(inHowMany, (SInt32) inFromCol);
}


PP_End_Namespace_PowerPlant
