// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableView.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A two-dimensional Table of rectangular cells.
//
//	Rows and Columns use one-based indexes.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableView.h>
#include <UTableHelpers.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>

PP_Begin_Namespace_PowerPlant

#pragma mark --- Construction/Destruction ---

// ---------------------------------------------------------------------------
//	¥ LTableView							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTableView::LTableView()
{
	InitTable();
}


// ---------------------------------------------------------------------------
//	¥ LTableView							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTableView::LTableView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)

	: LView(inPaneInfo, inViewInfo)
{
	InitTable();
}


// ---------------------------------------------------------------------------
//	¥ LTableView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTableView::LTableView(
	LStream*	inStream)

	: LView(inStream)
{
	InitTable();
}


// ---------------------------------------------------------------------------
//	¥ InitTable								Initializer				 [private]
// ---------------------------------------------------------------------------

void
LTableView::InitTable()
{
	mRows					= 0;
	mCols					= 0;
	mTableGeometry			= nil;
	mTableSelector			= nil;
	mTableStorage			= nil;
	mUseDragSelect			= false;
	mCustomHilite			= false;
	mDeferAdjustment		= false;
	mRefreshAllWhenResized	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LTableView							Destructor				  [public]
// ---------------------------------------------------------------------------

LTableView::~LTableView()
{
	delete mTableGeometry;
	delete mTableSelector;
	delete mTableStorage;
}

#pragma mark --- Setters/Getters ---

// ---------------------------------------------------------------------------
//	¥ GetTableSize
// ---------------------------------------------------------------------------
//	Pass back the number of rows and columns in a TableView

void
LTableView::GetTableSize(
	TableIndexT&	outRows,
	TableIndexT&	outCols) const
{
	outRows = mRows;
	outCols = mCols;
}


// ---------------------------------------------------------------------------
//	¥ SetDeferAdjustment
// ---------------------------------------------------------------------------
//	Specify whether to defer adjustment of Table to account for a change
//	in the size of the Table (number or size of rows or columns).
//
//	Calling AdjustImageSize() can take a lot of time, especially for
//	large Tables and those with variable row or column sizes. If you are
//	going to make a series of changes to a Table, there's no need to adjust
//	the Image size until after all those changes are made.
//
//	Therefore, call SetDeferAdjustment(true) before making your changes,
//	then call SetDeferAdjustment(false) afterwards. Or use the
//	StDeferTableAdjustment stack-based class.

void
LTableView::SetDeferAdjustment(
	Boolean		inDefer)
{
	if (inDefer != mDeferAdjustment) {
		mDeferAdjustment = inDefer;
		if (not inDefer) {
			AdjustImageSize(false);
		}
	}
}


#pragma mark --- Cell Verification ---

// ---------------------------------------------------------------------------
//	¥ IsValidRow
// ---------------------------------------------------------------------------
//	Return whether a TableView includes a specified row

Boolean
LTableView::IsValidRow(
	TableIndexT	inRow) const
{
	return ( (inRow > 0) && (inRow <= mRows) );
}


// ---------------------------------------------------------------------------
//	¥ IsValidCol
// ---------------------------------------------------------------------------
//	Return whether a TableView includes a specified column

Boolean
LTableView::IsValidCol(
	TableIndexT	inCol) const
{
	return ( (inCol > 0) && (inCol <= mCols) );
}


// ---------------------------------------------------------------------------
//	¥ IsValidCell
// ---------------------------------------------------------------------------
//	Return whether a TableView includes a specified Cell

Boolean
LTableView::IsValidCell(
	const STableCell&	inCell) const
{
	return ( IsValidRow(inCell.row) && IsValidCol(inCell.col) );
}

#pragma mark --- Row & Col Management ---

// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Add rows to a TableView
//
//	Use inAfterRow of 0 to insert rows at the beginning
//	All cells in the newly inserted rows have the same data

void
LTableView::InsertRows(
	UInt32		inHowMany,
	TableIndexT	inAfterRow,
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inRefresh)
{
	SignalIf_(mTableGeometry == nil);

	if (inAfterRow > mRows) {				// Enforce upper bound
		inAfterRow = mRows;
	}

	mRows += inHowMany;

	if (mTableStorage != nil) {
		mTableStorage->InsertRows(inHowMany, inAfterRow,
								inDataPtr, inDataSize);
	}

	mTableGeometry->InsertRows(inHowMany, inAfterRow);

	if (mTableSelector != nil) {
		mTableSelector->InsertRows(inHowMany, inAfterRow);
	}

	AdjustImageSize(false);

	if (inRefresh) {						// Refresh inserted rows and
											//   all rows below
		STableCell	topLeftCell(inAfterRow + 1, 1);
		STableCell	botRightCell(mRows, mCols);

		RefreshCellRange(topLeftCell, botRightCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Add columns to a TableView
//
//	Use inAfterCol of 0 to insert columns at the beginning
//	All cells in the newly inserted rows have the same data

void
LTableView::InsertCols(
	UInt32		inHowMany,
	TableIndexT	inAfterCol,
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inRefresh)
{
	SignalIf_(mTableGeometry == nil);

	if (inAfterCol > mCols) {				// Enforce upper bound
		inAfterCol= mCols;
	}

	mCols += inHowMany;

	if (mTableStorage != nil) {
		mTableStorage->InsertCols(inHowMany, inAfterCol,
								inDataPtr, inDataSize);
	}

	mTableGeometry->InsertCols(inHowMany, inAfterCol);

	if (mTableSelector != nil) {
		mTableSelector->InsertCols(inHowMany, inAfterCol);
	}

	AdjustImageSize(false);

	if (inRefresh) {						// Refresh inserted columns
											//   and all cols to the right
		STableCell	topLeftCell(1, inAfterCol + 1);
		STableCell	botRightCell(mRows, mCols);

		RefreshCellRange(topLeftCell, botRightCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Delete rows from a TableView

void
LTableView::RemoveRows(
	UInt32		inHowMany,
	TableIndexT	inFromRow,
	Boolean		inRefresh)
{
										// Check input parameters
	if (inFromRow > mRows) {
		SignalStringLiteral_("inFromRow > number of rows");
		return;
	}

	if (inFromRow < 1) {				// Assume user means Row 1 if
		inFromRow = 1;					//   inFromRow is 0
	}

	if ((inFromRow + inHowMany - 1) > mRows) {
										// inHowMany is too big. Remove
										//   from inFromRow to last row
		inHowMany = mRows - inFromRow + 1;
	}

	if (inRefresh) {					// Refresh deleted rows and all
										//   rows below
		STableCell	topLeftCell(inFromRow, 1);
		STableCell	botRightCell(mRows, mCols);

		RefreshCellRange(topLeftCell, botRightCell);
	}

	mRows -= inHowMany;

	mTableGeometry->RemoveRows(inHowMany, inFromRow);

	if (mTableStorage != nil) {
		mTableStorage->RemoveRows(inHowMany, inFromRow);
	}

	if (mTableSelector != nil) {
		mTableSelector->RemoveRows(inHowMany, inFromRow);
	}

	AdjustImageSize(inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllRows
// ---------------------------------------------------------------------------

void
LTableView::RemoveAllRows(
	Boolean		inRefresh)
{
	if (mRows > 0) {
		RemoveRows(mRows, 1, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Delete columns from a TableView

void
LTableView::RemoveCols(
	UInt32		inHowMany,
	TableIndexT	inFromCol,
	Boolean		inRefresh)
{
										// Check input parameters
	if (inFromCol > mCols) {
		SignalStringLiteral_("inFromCol > number of columns");
		return;
	}

	if (inFromCol < 1) {				// Assume user means Column 1 if
		inFromCol = 1;					//   inFromCol is 0
	}

	if ((inFromCol + inHowMany - 1) > mCols) {
										// inHowMany is too big. Remove
										//   from inFromRow to last row
		inHowMany = mCols - inFromCol + 1;
	}

	if (inRefresh) {					// Refresh deleted columns and all
										//   columns to the right
		STableCell	topLeftCell(1, inFromCol);
		STableCell	botRightCell(mRows, mCols);

		RefreshCellRange(topLeftCell, botRightCell);
	}

	mCols -= inHowMany;

	mTableGeometry->RemoveCols(inHowMany, inFromCol);

	if (mTableStorage != nil) {
		mTableStorage->RemoveCols(inHowMany, inFromCol);
	}

	if (mTableSelector != nil) {
		mTableSelector->RemoveCols(inHowMany, inFromCol);
	}

	AdjustImageSize(inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllCols
// ---------------------------------------------------------------------------

void
LTableView::RemoveAllCols(
	Boolean		inRefresh)
{
	if (mCols > 0) {
		RemoveCols(mCols, 1, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustImageSize
// ---------------------------------------------------------------------------
//	Adjust the Image size of the Table to reflect the number and
//	size of the rows and columns

void
LTableView::AdjustImageSize(
	Boolean	inRefresh)
{
	if (!mDeferAdjustment) {
		UInt32	width, height;
		mTableGeometry->GetTableDimensions(width, height);
		ResizeImageTo((SInt32) width, (SInt32) height, inRefresh);
	}
}

#pragma mark --- Accessing Cells ---

// ---------------------------------------------------------------------------
//	¥ CellToIndex
// ---------------------------------------------------------------------------
//	Pass back the index number for a specified Cell
//
//	Cell does not have to be in Table, but Index is zero if Table
//	has no columns, or if inCell.row or inCell.col is zero.
//
//	Cells are ordered by column (across), and then by row (down)

void
LTableView::CellToIndex(
	const STableCell&	inCell,
	TableIndexT&		outIndex) const
{
	outIndex = 0;
	if ( (mCols > 0) && (inCell.row > 0) && (inCell.col > 0) ) {
		outIndex = (inCell.row - 1) * mCols + inCell.col;
	}
}


// ---------------------------------------------------------------------------
//	¥ IndexToCell
// ---------------------------------------------------------------------------
//	Pass back the cell for a specified index number
//
//	Index number does not have to refer to an actual Cell, but Cell is
//	(0,0) if Table has no columns or inIndex is zero

void
LTableView::IndexToCell(
	TableIndexT		inIndex,
	STableCell&		outCell) const
{
	outCell.SetCell(0, 0);

	if ((mCols > 0) && (inIndex > 0)) {
		outCell.row = (inIndex - 1) / mCols + 1;
		outCell.col = (inIndex - 1) % mCols + 1;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetNextCell
// ---------------------------------------------------------------------------
//	Pass back the Cell after the specified Cell
//
//	Cells are ordered by column (across), and then by row (down)
//
//	Return false if there is no cell after the specified one, and
//		pass back Cell (0,0)
//	Otherwise, return true and pass back the next Cell's indexes
//
//	Row zero is before the first row. The next cell after row zero
//		and any column is Cell (1,1)
//
//	Column zero is before column one. The next cell after row "r" and
//		column zero is Cell (r,1).

Boolean
LTableView::GetNextCell(
	STableCell&	ioCell) const
{
	Boolean	nextCellExists = true;

	TableIndexT	row = ioCell.row;		// Next Cell is in same row and
	TableIndexT col = ioCell.col + 1;	//   the next column

	if (col > mCols) {					// Wrap around to first column
		row += 1;						//   of the next row
		col = 1;
	}

	if (row == 0) {						// Cell (1,1) is after all cells
		row = 1;						//   in row zero
		col = 1;
	}

	if (row > mRows) {					// Test if beyond last row
		nextCellExists = false;
		row = 0;
		col = 0;
	}

	ioCell.SetCell(row, col);
	return nextCellExists;
}


// ---------------------------------------------------------------------------
//	¥ GetFirstSelectedCell
// ---------------------------------------------------------------------------
//	Return the first selected cell, using the LTableSelector helper object

STableCell
LTableView::GetFirstSelectedCell() const
{
	STableCell	selectedCell(0, 0);

	if (mTableSelector != nil) {
		selectedCell = mTableSelector->GetFirstSelectedCell();
	}

	return selectedCell;
}


// ---------------------------------------------------------------------------
//	¥ GetNextSelectedCell
// ---------------------------------------------------------------------------
//	Pass back the selected Cell after the specified Cell
//
//	Return false if there is no selected Cell after the specified one
//
//	This function uses the same ordering rules as GetNextCell().
//	Pass in Cell (0,0) to find the first selected Cell.

Boolean
LTableView::GetNextSelectedCell(
	STableCell&	ioCell) const
{
	Boolean	nextSelectionExists = false;

	while (GetNextCell(ioCell)) {
		if (CellIsSelected(ioCell)) {
			nextSelectionExists = true;
			break;
		}
	}

	return nextSelectionExists;
}


// ---------------------------------------------------------------------------
//	¥ GetPreviousCell
// ---------------------------------------------------------------------------
//	Pass back the Cell before the specified Cell
//
//	Cells are ordered by column (across), and then by row (down)
//
//	Return false if there is no cell before the specified one, and
//		pass back Cell (0,0)
//	Otherwise, return true and pass back the previous Cell's indexes
//
//	Cell (mRows,mCols) is the one before Cell (0,c), where c is any column.
//	Thus, you can pass in Cell (0,0) to get the last cell.
//
//	Cell (mRows,mCols) is also the cell before any Cell that is beyond
//	the limits of the table.
//
//	Column zero is before column one. The cell before row "r" and
//		column zero is Cell (r-1,mCols).

Boolean
LTableView::GetPreviousCell(
	STableCell&	ioCell) const
{
	Boolean	prevCellExists = true;

	TableIndexT	row = ioCell.row;
	TableIndexT col = ioCell.col;

	if ((row == 0) || (row > mRows)) {	// Last Cell is before all cells
		row = mRows;					//   in row zero and rows beyond
		col = mCols;					//   the last row

	} else if (col <= 1) {				// Wrap around to last column in
		row -= 1;						//   previous row
		col = mCols;

	} else {							// Previous Cell is on the same
		col -= 1;						//   row. Usually one column back,
		if (col > mCols) {				//   but the last column if input
			col = mCols;				//   column is beyond the last
		}								//   column
	}

	if (row == 0) {						// There is no previous Cell.
		prevCellExists = false;			//   This happens only if the input
		row = 0;						//   Cell is (1,1), (1,0), or the
		col = 0;						//   Table has no rows.
	}

	ioCell.SetCell(row, col);
	return prevCellExists;
}


// ---------------------------------------------------------------------------
//		# GetPreviousSelectedCell
// ---------------------------------------------------------------------------
//	Pass back the selected Cell before the specified Cell
//
//	Return false if there is no selected Cell before the specified one
//
//	This function uses the same ordering rules as GetPreviosCell().
//	Pass in Cell (0,0) to find the last selected Cell.

Boolean
LTableView::GetPreviousSelectedCell(
	STableCell&	ioCell) const
{
	Boolean	prevSelectionExists = false;

	while (GetPreviousCell(ioCell)) {
		if (CellIsSelected(ioCell)) {
			prevSelectionExists = true;
			break;
		}
	}

	return prevSelectionExists;
}


// ---------------------------------------------------------------------------
//	¥ ScrollCellIntoFrame
// ---------------------------------------------------------------------------
//	Scroll the TableView as little as possible to move the specified Cell
//	so that it's entirely within the Frame of the TableView
//
//	If Cell is wider and/or taller than Frame, align Cell to left/top
//	of Frame.

void
LTableView::ScrollCellIntoFrame(
	const STableCell&	inCell)
{
	SInt32	cellLeft, cellTop, cellRight, cellBottom;
	mTableGeometry->GetImageCellBounds(inCell, cellLeft, cellTop,
							cellRight, cellBottom);

	if (ImagePointIsInFrame(cellLeft, cellTop) &&
		ImagePointIsInFrame(cellRight, cellBottom)) {

		return;						// Entire Cell is already within Frame
	}

	Rect	frame;					// Get Frame in Image coords
	CalcLocalFrameRect(frame);
	SPoint32	frameTopLeft, frameBotRight;
	LocalToImagePoint(topLeft(frame), frameTopLeft);
	LocalToImagePoint(botRight(frame), frameBotRight);

	SInt32	horizScroll = 0;		// Find horizontal scroll amount
	if (cellRight > frameBotRight.h) {
										// Cell is to the right of frame
		horizScroll = cellRight - frameBotRight.h;
	}

	if ((cellLeft - horizScroll) < frameTopLeft.h) {
										// Cell is to the left of frame
										//   or Cell is wider than frame
		horizScroll = cellLeft - frameTopLeft.h;
	}

	SInt32	vertScroll = 0;			// Find vertical scroll amount
	if (cellBottom > frameBotRight.v) {
										// Cell is below bottom of frame
		vertScroll = cellBottom - frameBotRight.v;
	}

	if ((cellTop - vertScroll) < frameTopLeft.v) {
										// Cell is above top of frame
										//   or Cell is taller than frame
		vertScroll = cellTop - frameTopLeft.v;
	}

	ScrollPinnedImageBy(horizScroll, vertScroll, Refresh_Yes);
}

#pragma mark --- Geometry ---

// ---------------------------------------------------------------------------
//	¥ SetTableGeometry
// ---------------------------------------------------------------------------
//	Specify the Geometry for a TableView.
//
//	The Geometry determines the dimensions of the Cells in the Table

void
LTableView::SetTableGeometry(
	LTableGeometry*	inTableGeometry)
{
	mTableGeometry = inTableGeometry;
}


// ---------------------------------------------------------------------------
//	¥ GetImageCellBounds
// ---------------------------------------------------------------------------
//	Pass back the location in Image coords of the specified Cell

void
LTableView::GetImageCellBounds(
	const STableCell&	inCell,
	SInt32&				outLeft,
	SInt32&				outTop,
	SInt32&				outRight,
	SInt32&				outBottom) const
{
	mTableGeometry->GetImageCellBounds(inCell, outLeft, outTop,
							outRight, outBottom);
}


// ---------------------------------------------------------------------------
//	¥ GetLocalCellRect
// ---------------------------------------------------------------------------
//	Pass back the bounding rectangle of the specified Cell and return
//	whether it intersects the Frame of the TableView
//
//	The bounding rectangle is in Local coordinates so it will always be
//	within QuickDraw space when its within the Frame. If the bounding
//	rectangle is outside the Frame, return false and set the rectangle
//	to (0,0,0,0)

Boolean
LTableView::GetLocalCellRect(
	const STableCell&	inCell,
	Rect&				outCellRect) const
{
	SInt32	cellLeft, cellTop, cellRight, cellBottom;
	mTableGeometry->GetImageCellBounds(inCell, cellLeft, cellTop,
										cellRight, cellBottom);

	Boolean	insideFrame =
		ImageRectIntersectsFrame(cellLeft, cellTop, cellRight, cellBottom);

	if (insideFrame) {
		SPoint32	imagePt;
		imagePt.h = cellLeft;
		imagePt.v = cellTop;
		ImageToLocalPoint(imagePt, topLeft(outCellRect));
		outCellRect.right  = (SInt16) (outCellRect.left + (cellRight - cellLeft));
		outCellRect.bottom = (SInt16) (outCellRect.top + (cellBottom - cellTop));

	} else {
		::MacSetRect(&outCellRect, 0, 0, 0, 0);
	}

	return insideFrame;
}


// ---------------------------------------------------------------------------
//	¥ GetCellHitBy
// ---------------------------------------------------------------------------
//	Pass back the cell which contains the specified point.
//
//	If no cell contains the point, return false and
//
//		outCell.row = 0 if point is above the Table
//		outCell.row = mRows + 1 if point is below the Table
//
//		outCell.col = 0 if point is to the left of the Table
//		outCell.col = mCols + 1 if point is to the right of the Table
//
//	For example, if the horizontal coord. is within Column 2,
//	but the vertical coord. is above the Table,
//		outCell.row = 0
//		outCell.col = 2

Boolean
LTableView::GetCellHitBy(
	const SPoint32&	inImagePt,
	STableCell&		outCell) const
{
	Boolean	containsPoint = true;

	if (inImagePt.v < 0) {
		outCell.row = 0;
		containsPoint = false;

	} else if (inImagePt.v >= mImageSize.height) {
		outCell.row = mRows + 1;
		containsPoint = false;

	} else {
		outCell.row = mTableGeometry->GetRowHitBy(inImagePt);
	}

	if (inImagePt.h < 0) {
		outCell.col = 0;
		containsPoint = false;

	} else if (inImagePt.h >= mImageSize.width) {
		outCell.col = mCols + 1;
		containsPoint = false;

	} else {
		outCell.col = mTableGeometry->GetColHitBy(inImagePt);
	}

	return containsPoint;
}


// ---------------------------------------------------------------------------
//	¥ GetRowHeight
// ---------------------------------------------------------------------------
//	Return the height of the specified row

UInt16
LTableView::GetRowHeight(
	TableIndexT	inRow) const
{
	return mTableGeometry->GetRowHeight(inRow);
}


// ---------------------------------------------------------------------------
//	¥ SetRowHeight
// ---------------------------------------------------------------------------
//	Set the height of the specified rows

void
LTableView::SetRowHeight(
	UInt16		inHeight,
	TableIndexT	inFromRow,
	TableIndexT	inToRow)
{
	mTableGeometry->SetRowHeight(inHeight, inFromRow, inToRow);
	AdjustImageSize(Refresh_Yes);
}


// ---------------------------------------------------------------------------
//	¥ GetColWidth
// ---------------------------------------------------------------------------
//	Return the width of the specified column

UInt16
LTableView::GetColWidth(
	TableIndexT	inCol) const
{
	return mTableGeometry->GetColWidth(inCol);
}


// ---------------------------------------------------------------------------
//	¥ SetColWidth
// ---------------------------------------------------------------------------
//	Set the width of the specified columns

void
LTableView::SetColWidth(
	UInt16		inWidth,
	TableIndexT	inFromCol,
	TableIndexT	inToCol)
{
	mTableGeometry->SetColWidth(inWidth, inFromCol, inToCol);
	AdjustImageSize(Refresh_Yes);
}


// ---------------------------------------------------------------------------
//	¥ FetchIntersectingCells
// ---------------------------------------------------------------------------
//	Pass back the rectangular range of cells, specified by the top left
//	and bottom right cells, that intersects a given rectangle.

void
LTableView::FetchIntersectingCells(
	const Rect&		inLocalRect,			// In Local Coordinates
	STableCell&		outTopLeft,
	STableCell&		outBotRight) const
{
								// ¥ Top Left Cell

	SPoint32	topLeftPt;		// Convert top left corner to Image coords
								//   and find cell that contains it
	LocalToImagePoint(topLeft(inLocalRect), topLeftPt);
	GetCellHitBy(topLeftPt, outTopLeft);

	if (outTopLeft.row < 1) {	// Lower bound is cell (1,1)
		outTopLeft.row = 1;
	}
	if (outTopLeft.col < 1) {
		outTopLeft.col = 1;
	}

								// ¥ Bottom Right Cell

	SPoint32	botRightPt;		// Convert bottom right corner to Image coords
	LocalToImagePoint(botRight(inLocalRect), botRightPt);

	botRightPt.h -= 1;			// Pixel hangs below and to the right of
	botRightPt.v -= 1;			//   coordinate

	GetCellHitBy(botRightPt, outBotRight);

								// Upper bound is cell (mRows,mCols)
	if (outBotRight.row > mRows) {
		outBotRight.row = mRows;
	}
	if (outBotRight.col > mCols) {
		outBotRight.col = mCols;
	}
}

#pragma mark --- Selection ---

// ---------------------------------------------------------------------------
//	¥ SetTableSelector
// ---------------------------------------------------------------------------
//	Specify the Selector for a TableView.
//
//	The Selector stores and controls which Cells in a Table are selected

void
LTableView::SetTableSelector(
	LTableSelector*	inTableSelector)
{
	mTableSelector = inTableSelector;
}


// ---------------------------------------------------------------------------
//	¥ CellIsSelected
// ---------------------------------------------------------------------------
//	Return whether the specified Cell is part of the current selection

Boolean
LTableView::CellIsSelected(
	const STableCell&	inCell) const
{
	Boolean	selected = false;

	if (mTableSelector != nil) {
		selected = mTableSelector->CellIsSelected(inCell);
	}

	return selected;
}


// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------
//	Add the specified cell to the current selection

void
LTableView::SelectCell(
	const STableCell&	inCell)
{
	if (mTableSelector != nil) {
		mTableSelector->SelectCell(inCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectAllCells
// ---------------------------------------------------------------------------
//	Select all Cells in a Table

void
LTableView::SelectAllCells()
{
	if (mTableSelector != nil) {
		mTableSelector->SelectAllCells();
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectCell
// ---------------------------------------------------------------------------
//	Remove the specified cell from the current selection

void
LTableView::UnselectCell(
	const STableCell&	inCell)
{
	if (mTableSelector != nil) {
		mTableSelector->UnselectCell(inCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all currently selected cells so there is no selection

void
LTableView::UnselectAllCells()
{
	if (mTableSelector != nil) {
		mTableSelector->UnselectAllCells();
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection in response to a click in the specified cell
//
//	Return whether or not to process the click as a normal click

Boolean
LTableView::ClickSelect(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
	if (mTableSelector != nil) {
		if (mUseDragSelect) {
			return mTableSelector->DragSelect(inCell, inMouseDown);

		} else {
			mTableSelector->ClickSelect(inCell, inMouseDown);
		}
	}

	return true;
}


// ---------------------------------------------------------------------------
//	¥ SelectionChanged
// ---------------------------------------------------------------------------
//	Notification that the cells which are selected has changed

void
LTableView::SelectionChanged()
{
}

#pragma mark --- Storage ---

// ---------------------------------------------------------------------------
//	¥ SetTableStorage
// ---------------------------------------------------------------------------
//	Specify the Storage for a TableView
//
//	The Storage holds the data for each Cell in a Table

void
LTableView::SetTableStorage(
	LTableStorage*	inTableStorage)
{
	mTableStorage = inTableStorage;

	TableIndexT	storageRows, storageCols;
	inTableStorage->GetStorageSize(storageRows, storageCols);

	mRows = storageRows;
	if (storageRows > 0) {
		mTableGeometry->InsertRows(storageRows, 0);
	}

	mCols = storageCols;
	if (storageCols > 0) {
		mTableGeometry->InsertCols(storageCols, 0);
	}

	if ((storageRows > 0) || (storageCols > 0)) {
		AdjustImageSize(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCellData
// ---------------------------------------------------------------------------
//	Specify the data associated with a particular Cell

void
LTableView::SetCellData(
	const STableCell&	inCell,
	const void*			inDataPtr,
	UInt32				inDataSize)
{
	if (mTableStorage != nil) {
		mTableStorage->SetCellData(inCell, inDataPtr, inDataSize);

	} else {
		SignalStringLiteral_("No TableStorage for this Table");
	}
}


// ---------------------------------------------------------------------------
//	¥ GetCellData
// ---------------------------------------------------------------------------
//	Pass back the data for a particular Cell
//
//	outDataPtr points to storage which must be allocated by the caller.
//	It may be nil, in which case only the size of the data is passed back.
//
//	ioDataSize (if outDataPtr is nil)
//		input:	<ignored>
//		output:	size in bytes of cell's data
//
//	ioDataSize (if outDataPtr is not nil)
//		input:	maximum bytes of data to retrieve
//		output:	actual bytes of data passed back

void
LTableView::GetCellData(
	const STableCell&	inCell,
	void*				outDataPtr,
	UInt32&				ioDataSize) const
{
	if (mTableStorage != nil) {
		mTableStorage->GetCellData(inCell, outDataPtr, ioDataSize);
	} else {
		ioDataSize = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ FindCellData
// ---------------------------------------------------------------------------
//	Pass back the cell that contains the specified data
//
//	Return false if no match is found

Boolean
LTableView::FindCellData(
	STableCell&			outCell,
	const void*			inDataPtr,
	UInt32				inDataSize) const
{
	Boolean	found = false;

	if (mTableStorage != nil) {
		found = mTableStorage->FindCellData(outCell, inDataPtr, inDataSize);
	}

	return found;
}

#pragma mark --- Hiliting ---

// ---------------------------------------------------------------------------
//	¥ HiliteSelection
// ---------------------------------------------------------------------------
//	Draw or undraw hiliting for the current selection in either the
//	active or inactive state

void
LTableView::HiliteSelection(
	Boolean	inActively,
	Boolean	inHilite)
{
	if (mCustomHilite) {				// Hilite Cells one by one

		Rect	frame;					// Get range of Cells that are
		CalcLocalFrameRect(frame);		//   within the Frame
		STableCell	cell, topLeftCell, botRightCell;
		FetchIntersectingCells(frame, topLeftCell, botRightCell);

		for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
			for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
				if (CellIsSelected(cell)) {
					if (inActively) {
						HiliteCellActively(cell, inHilite);
					} else {
						HiliteCellInactively(cell, inHilite);
					}
				}
			}
		}

	} else if (FocusExposed()) {		// Hilite all selected cells
										//   as a group by finding the
										//   region occupied by all
										//   selected cells
		StRegion	hiliteRgn;
		GetHiliteRgn(hiliteRgn);
		UDrawingUtils::SetHiliteModeOn();
        StColorPenState saveColorPen;   // Preserve color & pen state
        ::PenNormal();
		ApplyForeAndBackColors();

		if (inActively) {
			::MacInvertRgn(hiliteRgn);

		} else {
			::PenMode(srcXor);
			::MacFrameRgn(hiliteRgn);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetHiliteRgn
// ---------------------------------------------------------------------------
//	Pass back a Region containing the frames of all selected cells which
//	are within the visible rectangle of the Table
//
//	Caller must allocate space for the region

void
LTableView::GetHiliteRgn(
	RgnHandle	ioHiliteRgn)
{
	::SetEmptyRgn(ioHiliteRgn);			// Assume no visible selection

	Rect	visRect;
	GetRevealedRect(visRect);			// Check if Table is revealed
	if (!::EmptyRect(&visRect)) {
		PortToLocalPoint(topLeft(visRect));
		PortToLocalPoint(botRight(visRect));

		STableCell	cell, topLeftCell, botRightCell;
		FetchIntersectingCells(visRect, topLeftCell, botRightCell);
		StRegion	cellRgn;

		for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
			for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
				if (CellIsSelected(cell)) {
					Rect	cellRect;
					GetLocalCellRect(cell, cellRect);
					::RectRgn(cellRgn, &cellRect);
					::MacUnionRgn(ioHiliteRgn, cellRgn, ioHiliteRgn);
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteCell
// ---------------------------------------------------------------------------
//	Draw or undraw hiliting for the specified Cell

void
LTableView::HiliteCell(
	const STableCell&	inCell,
	Boolean				inHilite)
{
	if (IsActive()) {
		HiliteCellActively(inCell, inHilite);

	} else {
		HiliteCellInactively(inCell, inHilite);
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteCellActively
// ---------------------------------------------------------------------------
//	Draw or undraw active hiliting for a Cell

void
LTableView::HiliteCellActively(
	const STableCell&	inCell,
	Boolean				/* inHilite */)
{
	Rect	cellFrame;
    if (GetLocalCellRect(inCell, cellFrame) && FocusExposed()) {
        StColorPenState saveColorPen;   // Preserve color & pen state
        ::PenNormal();
        UDrawingUtils::SetHiliteModeOn();
        ApplyForeAndBackColors();
		::MacInvertRect(&cellFrame);
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteCellInactively
// ---------------------------------------------------------------------------
//	Draw or undraw inactive hiliting for a Cell

void
LTableView::HiliteCellInactively(
	const STableCell&	inCell,
	Boolean				/* inHilite */)
{
	Rect	cellFrame;
	if (GetLocalCellRect(inCell, cellFrame) && FocusExposed()) {
        StColorPenState saveColorPen;   // Preserve color & pen state
        ::PenNormal();
		UDrawingUtils::SetHiliteModeOn();
        ApplyForeAndBackColors();
		::PenMode(srcXor);
		::MacFrameRect(&cellFrame);
	}
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf
// ---------------------------------------------------------------------------
//	Activate a Table

void
LTableView::ActivateSelf()
{
	HiliteSelection(false, false);	// Remove inactive hiliting
	HiliteSelection(true, true);	// Draw active hiliting
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf
// ---------------------------------------------------------------------------
//	Deactivate a Table

void
LTableView::DeactivateSelf()
{
	HiliteSelection(true, false);	// Remove active hiliting
	HiliteSelection(false, true);	// Draw inactive hiliting
}

#pragma mark --- Clicking ---

// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------
//	Handle a mouse click within a TableView

void
LTableView::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	STableCell	hitCell;
	SPoint32	imagePt;

	LocalToImagePoint(inMouseDown.whereLocal, imagePt);

	if (GetCellHitBy(imagePt, hitCell)) {
		if (ClickSelect(hitCell, inMouseDown)) {
			ClickCell(hitCell, inMouseDown);
		}

	} else {							// Click is outside of any Cell
		UnselectAllCells();
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickCell
// ---------------------------------------------------------------------------
//	Handle a mouse click within the specified Cell

void
LTableView::ClickCell(
	const STableCell&		/* inCell */,
	const SMouseDownEvent&	/* inMouseDown */)
{
}


// ---------------------------------------------------------------------------
//	¥ PointsAreClose
// ---------------------------------------------------------------------------
//	Return whether the two points are close enough to be part of a
//	multi-click. Points are in Local coordinates.
//
//	Points must meet the standard test (inherited function) as well as be
//	inside the same cell

Boolean
LTableView::PointsAreClose(
	Point	inFirstPt,
	Point	inSecondPt) const
{
	Boolean		areClose = LView::PointsAreClose(inFirstPt, inSecondPt);

	if (areClose) {
		SPoint32	imageLoc;

		STableCell	firstCell;
		LocalToImagePoint(inFirstPt, imageLoc);
		GetCellHitBy(imageLoc, firstCell);

		STableCell	secondCell;
		LocalToImagePoint(inSecondPt, imageLoc);
		GetCellHitBy(imageLoc, secondCell);

		areClose = (firstCell == secondCell);
	}

	return areClose;
}

#pragma mark --- Drawing ---

// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw a TableView

void
LTableView::DrawSelf()
{
	DrawBackground();

		// Determine cells that need updating. Rather than checking
		// on a cell by cell basis, we just see which cells intersect
		// the bounding box of the update region. This is relatively
		// fast, but may result in unnecessary cell updates for
		// non-rectangular update regions.

	Rect	updateRect;
	{
		StRegion	localUpdateRgn( GetLocalUpdateRgn(), false );
		localUpdateRgn.GetBounds(updateRect);
	}
	
	STableCell	topLeftCell, botRightCell;
	FetchIntersectingCells(updateRect, topLeftCell, botRightCell);
	
	DrawCellRange(topLeftCell, botRightCell);

	HiliteSelection(IsActive(), true);
}


// ---------------------------------------------------------------------------
//	¥ PrintPanelSelf
// ---------------------------------------------------------------------------

void
LTableView::PrintPanelSelf(
	const PanelSpec&	/* inPanel */)
{
	DrawBackground();
	
	Rect	frame;						// Draw all Cells within Frame
	CalcLocalFrameRect(frame);
	
	STableCell	topLeftCell, botRightCell;
	FetchIntersectingCells(frame, topLeftCell, botRightCell);
	
	DrawCellRange(topLeftCell, botRightCell);
	
		// Do NOT hilite selection when printing Table
}


// ---------------------------------------------------------------------------
//	¥ DrawCellRange
// ---------------------------------------------------------------------------

void
LTableView::DrawCellRange(
	const STableCell&	inTopLeftCell,
	const STableCell&	inBottomRightCell)
{
	STableCell	cell;
	
	for ( cell.row = inTopLeftCell.row;
		  cell.row <= inBottomRightCell.row;
		  cell.row++ ) {
		  
		for ( cell.col = inTopLeftCell.col;
			  cell.col <= inBottomRightCell.col;
			  cell.col++ ) {
			  
			Rect	cellRect;
			GetLocalCellRect(cell, cellRect);
			DrawCell(cell, cellRect);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawCell
// ---------------------------------------------------------------------------
//	Draw the contents of the specified Cell

void
LTableView::DrawCell(
	const STableCell&	/* inCell */,
	const Rect&			/* inLocalRect */)
{
}


// ---------------------------------------------------------------------------
//	¥ DrawBackground
// ---------------------------------------------------------------------------
//	Draw the background of the Table

void
LTableView::DrawBackground()
{
		// Default is to erase the frame to the background color

	Rect	frame;
	CalcLocalFrameRect(frame);
	
	ApplyForeAndBackColors();
	
	::EraseRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------

void
LTableView::ApplyForeAndBackColors() const
{
	Pattern	whitePat;
	UQDGlobals::GetWhitePat(&whitePat);
	::BackPat(&whitePat);

	::RGBForeColor(&Color_Black);
	::RGBBackColor(&Color_White);
}


// ---------------------------------------------------------------------------
//	¥ RefreshCell
// ---------------------------------------------------------------------------
//	Invalidate the area occupied by the specified cell so that its
//	contents will be redrawn during the next update event

void
LTableView::RefreshCell(
	const STableCell&	inCell)
{
	Rect	cellRect;
	if (GetLocalCellRect(inCell, cellRect)) {
		RefreshRect(cellRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ RefreshCellRange
// ---------------------------------------------------------------------------
//	Invalidate a rectangular block of cells so that their contents will
//	be redrawn during the next update event

void
LTableView::RefreshCellRange(
	const STableCell&	inTopLeft,
	const STableCell&	inBotRight)
{
		// We only need to refresh the portion of the cell range
		// that is revealed within the Frame of the Table

	if (!IsVisible()) return;			// Nothing is visible

		// Find the range of cells that are revealed

	Rect	localRevealedRect = mRevealedRect;		// Get Revealed Rect
	PortToLocalPoint(topLeft(localRevealedRect));	//   in Local coords
	PortToLocalPoint(botRight(localRevealedRect));

	STableCell	refreshTopLeft, refreshBotRight;
	FetchIntersectingCells(localRevealedRect, refreshTopLeft, refreshBotRight);

		// Find the intersection of the input cell range
		// with the revealed cell range

	if (refreshTopLeft.row < inTopLeft.row) {	// At top left, intersection
		refreshTopLeft.row = inTopLeft.row;		//   is the greater of the
	}											//   two row and col values

	if (refreshTopLeft.col < inTopLeft.col) {
		refreshTopLeft.col = inTopLeft.col;
	}

	if (refreshBotRight.row > inBotRight.row) {	// At bot right, intersection
		refreshBotRight.row = inBotRight.row;	//   is the lesser of the
	}											//   two row and col values

	if (refreshBotRight.col > inBotRight.col) {
		refreshBotRight.col = inBotRight.col;
	}

		// There's nothing to refresh if the intersection is empty

	if ( (refreshTopLeft.row > refreshBotRight.row) ||
		 (refreshTopLeft.col > refreshBotRight.col) ) {
		return;
	}

		// Get rectangle in Local coords that bounds the
		// refresh cell range

	Rect	refreshRect;
	Rect	cellRect;

	GetLocalCellRect(refreshTopLeft, refreshRect);
	GetLocalCellRect(refreshBotRight, cellRect);
	refreshRect.right = cellRect.right;
	refreshRect.bottom = cellRect.bottom;

	RefreshRect(refreshRect);
}

#pragma mark --- StDeferTableAdjustment ---

// ===========================================================================
// ¥ StDeferTableAdjustment
// ===========================================================================
//	Stack-based class for setting and restoring the "defer adjustment"
//	state of a TableView. Use for efficiency when making a series of
//	changes to a Table. For example,
//
//		{
//			StDeferTableAdjustment  defer(myTable);
//
//				// Make calls to Insert or Remove cells from myTable
//		}

StDeferTableAdjustment::StDeferTableAdjustment(
	LTableView*		inTable)
{
	mTable = inTable;
	mSaveDefer = inTable->GetDeferAdjustment();
	inTable->SetDeferAdjustment(true);
}


StDeferTableAdjustment::~StDeferTableAdjustment()
{
	mTable->SetDeferAdjustment(mSaveDefer);
}


PP_End_Namespace_PowerPlant
