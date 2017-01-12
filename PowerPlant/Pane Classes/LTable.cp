// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTable.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A two-dimensional array of rectangular cells. All rows have the same
//	height. All columns have the same width. Only one cell may be selected
//	at any time.
//
//	Use LTableView if you need options such as variable-sized cells and
//	multiple selections.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTable.h>
#include <LArray.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>

#include <LowMem.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTable						Default Constructor				  [public]
// ---------------------------------------------------------------------------

LTable::LTable()
{
	InitTable(0, 0, 16, 100, 0);
}


// ---------------------------------------------------------------------------
//	¥ LTable						Parameterized Constructor		  [public]
// ---------------------------------------------------------------------------

LTable::LTable(
	const SPaneInfo	&inPaneInfo,
	const SViewInfo	&inViewInfo,
	SInt32			inNumberOfRows,
	SInt32			inNumberOfCols,
	SInt32			inRowHeight,
	SInt32			inColWidth,
	SInt32			inCellDataSize)
	
	: LView(inPaneInfo, inViewInfo)
{
	InitTable(inNumberOfRows, inNumberOfCols, inRowHeight, inColWidth,
				inCellDataSize);
}


// ---------------------------------------------------------------------------
//	¥ LTable						Stream Constructor				  [public]
// ---------------------------------------------------------------------------

LTable::LTable(
	LStream*	inStream)
	
	: LView(inStream)
{
	STableInfo	tableInfo;
	*inStream >> tableInfo.numberOfRows;
	*inStream >> tableInfo.numberOfCols;
	*inStream >> tableInfo.rowHeight;
	*inStream >> tableInfo.colWidth;
	*inStream >> tableInfo.cellDataSize;

	InitTable(tableInfo.numberOfRows, tableInfo.numberOfCols,
			tableInfo.rowHeight, tableInfo.colWidth, tableInfo.cellDataSize);
}


// ---------------------------------------------------------------------------
//	¥ ~LTable						Destructor						  [public]
// ---------------------------------------------------------------------------

LTable::~LTable()
{
	delete mCellData;
}


// ---------------------------------------------------------------------------
//	¥ InitTable						Private Initializer				 [private]
// ---------------------------------------------------------------------------

void
LTable::InitTable(
	SInt32			inNumberOfRows,
	SInt32			inNumberOfCols,
	SInt32			inRowHeight,
	SInt32			inColWidth,
	SInt32			inCellDataSize)
{
	mRowHeight = inRowHeight;
	mColWidth = inColWidth;
	mCellData = nil;
	mSelectedCell.row = mSelectedCell.col = 0;

	mRows = mCols = 0;
	InsertRows(inNumberOfRows, 0, nil);
	InsertCols(inNumberOfCols, 0, nil);

	SetCellDataSize(inCellDataSize);
}


// ---------------------------------------------------------------------------
//	¥ GetTableSize
// ---------------------------------------------------------------------------

void
LTable::GetTableSize(
	TableIndexT	&outRows,
	TableIndexT	&outCols) const
{
	outRows = mRows;
	outCols = mCols;
}


// ---------------------------------------------------------------------------
//	¥ IsValidCell
// ---------------------------------------------------------------------------

Boolean
LTable::IsValidCell(
	const TableCellT	&inCell) const
{
	return ( (inCell.row > 0) && (inCell.row <= mRows) &&
			 (inCell.col > 0) && (inCell.col <= mCols) );
}


// ---------------------------------------------------------------------------
//	¥ EqualCell
// ---------------------------------------------------------------------------

Boolean
LTable::EqualCell(
	const TableCellT	&inCellA,
	const TableCellT	&inCellB) const
{
	return ( (inCellA.row == inCellB.row) &&
			 (inCellA.col == inCellB.col) );
}


// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------

void
LTable::InsertRows(
	SInt32		inHowMany,
	TableIndexT	inAfterRow,
	void		*inCellData)
{
	if (mCellData != nil) {
		mCellData->InsertItemsAt(inHowMany * mCols, (ArrayIndexT) (inAfterRow * mCols + 1),
									inCellData);
	}

	mRows += (TableIndexT) inHowMany;

	ResizeImageBy(0, inHowMany * mRowHeight, true);
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------

void
LTable::InsertCols(
	SInt32		inHowMany,
	TableIndexT	inAfterCol,
	void		*inCellData)
{
	if (mCellData != nil) {
		SInt32	insertAtIndex = (SInt32) inAfterCol + 1;
		for (TableIndexT row = 1; row <= mRows; row++) {
			mCellData->InsertItemsAt((UInt32) inHowMany, insertAtIndex, inCellData);
			insertAtIndex += (SInt32) mCols + inHowMany;
		}
	}

	mCols += (UInt32) inHowMany;

	ResizeImageBy(inHowMany * mColWidth, 0, true);
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------

void
LTable::RemoveRows(
	SInt32		inHowMany,
	TableIndexT	inFromRow)
{
	if (mCellData != nil) {
		mCellData->RemoveItemsAt(inHowMany * mCols,
									(ArrayIndexT) ((inFromRow - 1) * mCols + 1));
	}
	mRows -= (UInt32) inHowMany;

	ResizeImageBy(0, -inHowMany * mRowHeight, true);
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------

void
LTable::RemoveCols(
	SInt32		inHowMany,
	TableIndexT	inFromCol)
{
	if (mCellData != nil) {
		SInt32	removeAtIndex = (SInt32) inFromCol;
		for (TableIndexT row = 1; row <= mRows; row++) {
			mCellData->RemoveItemsAt((UInt32) inHowMany, removeAtIndex);
			removeAtIndex += (SInt32) (mCols - inHowMany);
		}
	}

	mCols -= (UInt32) inHowMany;

	ResizeImageBy(-inHowMany * mColWidth, 0, true);
}


// ---------------------------------------------------------------------------
//	¥ SetRowHeight
// ---------------------------------------------------------------------------

void
LTable::SetRowHeight(
	SInt16		inHeight,
	TableIndexT	/* inFrom */,
	TableIndexT	/* inTo */)
{
	mRowHeight = inHeight;
}


// ---------------------------------------------------------------------------
//	¥ SetColWidth
// ---------------------------------------------------------------------------

void
LTable::SetColWidth(
	SInt16		inWidth,
	TableIndexT	/* inFrom */,
	TableIndexT	/* inTo */)
{
	mColWidth = inWidth;
}


// ---------------------------------------------------------------------------
//	¥ SetCellDataSize
// ---------------------------------------------------------------------------

void
LTable::SetCellDataSize(
	SInt32	inCellDataSize)
{
			// Create DynamicArray for holding data
			// mCellData must not already be allocated
	if ((mCellData == nil) && (inCellDataSize > 0)) {
		mCellData = new LArray((UInt32) inCellDataSize);
		mCellData->InsertItemsAt(mRows * mCols, 1, nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCellData
// ---------------------------------------------------------------------------

void
LTable::SetCellData(
	const TableCellT	&inCell,
	void				*inData)
{
	if (mCellData != nil) {
		mCellData->AssignItemsAt(1, FetchCellDataIndex(inCell), inData);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetCellData
// ---------------------------------------------------------------------------

void
LTable::GetCellData(
	const TableCellT	&inCell,
	void				*outData)
{
	if (mCellData != nil) {
		mCellData->FetchItemAt(FetchCellDataIndex(inCell), outData);
	}
}


// ---------------------------------------------------------------------------
//	¥ FetchLocalCellFrame
// ---------------------------------------------------------------------------

Boolean
LTable::FetchLocalCellFrame(
	const TableCellT	&inCell,
	Rect				&outCellFrame)
{
								// Get Top-Left in Image coordinates
	SPoint32	cellImage;
	cellImage.h = (SInt32) (inCell.col - 1) * mColWidth;
	cellImage.v = (SInt32) (inCell.row - 1) * mRowHeight;

								// Check if Cell intersects the Frame
	Boolean	intersectsFrame = ImageRectIntersectsFrame(
									cellImage.h, cellImage.v,
									cellImage.h + mColWidth,
									cellImage.v + mRowHeight);

	if (intersectsFrame) {		// Convert to Local coordinates
		ImageToLocalPoint(cellImage, topLeft(outCellFrame));
		outCellFrame.right = (SInt16) (outCellFrame.left + mColWidth);
		outCellFrame.bottom = (SInt16) (outCellFrame.top + mRowHeight);
	}

	return intersectsFrame;
}


// ---------------------------------------------------------------------------
//	¥ FetchCellHitBy
// ---------------------------------------------------------------------------

void
LTable::FetchCellHitBy(
	const SPoint32	&inImagePt,
	TableCellT		&outCell)
{
	outCell.row = (TableIndexT) ((inImagePt.v - 1) / mRowHeight + 1);
	outCell.col = (TableIndexT) ((inImagePt.h - 1) / mColWidth + 1);
}


// ---------------------------------------------------------------------------
//	¥ FetchCellDataIndex
// ---------------------------------------------------------------------------

SInt32
LTable::FetchCellDataIndex(
	const TableCellT	&inCell)
{
	return (SInt32) ((inCell.row - 1) * mCols + inCell.col);
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------

void
LTable::ClickSelf(
	const SMouseDownEvent &inMouseDown)
{
	TableCellT	hitCell;
	SPoint32	imagePt;

	LocalToImagePoint(inMouseDown.whereLocal, imagePt);
	FetchCellHitBy(imagePt, hitCell);

	if (IsValidCell(hitCell)) {
		ClickCell(hitCell, inMouseDown);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------

void
LTable::DrawSelf()
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

								// Find cell at top left of update rect
	SPoint32	topLeftUpdate;
	TableCellT	topLeftCell;
	LocalToImagePoint(topLeft(updateRect), topLeftUpdate);
	FetchCellHitBy(topLeftUpdate, topLeftCell);
	if (topLeftCell.row < 1) {	// Lower bound is cell (1,1)
		topLeftCell.row = 1;
	}
	if (topLeftCell.col < 1) {
		topLeftCell.col = 1;
	}

								// Find cell at bottom right of update rect
	SPoint32	botRightUpdate;
	TableCellT	botRightCell;
	LocalToImagePoint(botRight(updateRect), botRightUpdate);
	FetchCellHitBy(botRightUpdate, botRightCell);
								// Upper bound is cell (mRows,mCols)
	if (botRightCell.row > mRows) {
		botRightCell.row = mRows;
	}
	if (botRightCell.col > mCols) {
		botRightCell.col = mCols;
	}

								// Draw each cell within the update rect
	TableCellT	cell;
	for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
		for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
			DrawCell(cell);
		}
	}

	HiliteCell(mSelectedCell);
}


// ---------------------------------------------------------------------------
//	¥ DrawBackground
// ---------------------------------------------------------------------------
//	Draw the background of the Table

void
LTable::DrawBackground()
{
		// Default is to erase the frame to the background color

	Rect	frame;
	CalcLocalFrameRect(frame);
	
	ApplyForeAndBackColors();
	
	::EraseRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ HiliteCell
// ---------------------------------------------------------------------------

void
LTable::HiliteCell(
	const TableCellT&	/* inCell */)
{
	Rect	cellFrame;
	if (IsValidCell(mSelectedCell) &&
		FetchLocalCellFrame(mSelectedCell, cellFrame)) {
		UDrawingUtils::SetHiliteModeOn();
		ApplyForeAndBackColors();
		if (IsActive()) {
			::MacInvertRect(&cellFrame);
		} else {
			StColorPenState::Normalize();
			::PenMode(srcXor);
			::MacFrameRect(&cellFrame);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ UnhiliteCell
// ---------------------------------------------------------------------------

void
LTable::UnhiliteCell(
	const TableCellT&	/* inCell */)
{
	Rect	cellFrame;
	if (IsValidCell(mSelectedCell) &&
		FetchLocalCellFrame(mSelectedCell, cellFrame)) {
		UDrawingUtils::SetHiliteModeOn();
		ApplyForeAndBackColors();
		::MacInvertRect(&cellFrame);
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickCell
// ---------------------------------------------------------------------------

void
LTable::ClickCell(
	const TableCellT		&inCell,
	const SMouseDownEvent&	/* inMouseDown */)
{
	SelectCell(inCell);
}


// ---------------------------------------------------------------------------
//	¥ DrawCell
// ---------------------------------------------------------------------------

void
LTable::DrawCell(
	const TableCellT&	/* inCell */)
{
}


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------

void
LTable::ApplyForeAndBackColors() const
{
	Pattern	whitePat;
	UQDGlobals::GetWhitePat(&whitePat);
	::BackPat(&whitePat);

	::RGBForeColor(&Color_Black);
	::RGBBackColor(&Color_White);
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf
// ---------------------------------------------------------------------------

void
LTable::ActivateSelf()
{
	if (FocusExposed()) {
		Rect	cellFrame;
		if (IsValidCell(mSelectedCell) &&
			FetchLocalCellFrame(mSelectedCell, cellFrame)) {
			UDrawingUtils::SetHiliteModeOn();
			StColorPenState::Normalize();	// Undo inactive hiliting
			::PenMode(srcXor);
			::MacFrameRect(&cellFrame);

			HiliteCell(mSelectedCell);		// Do active hiliting
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf
// ---------------------------------------------------------------------------

void
LTable::DeactivateSelf()
{
	if (FocusExposed()) {
		UnhiliteCell(mSelectedCell);
		HiliteCell(mSelectedCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectCell
// ---------------------------------------------------------------------------

void
LTable::SelectCell(
	const TableCellT	&inCell)
{
	if (!EqualCell(inCell, mSelectedCell)) {
		FocusDraw();
		UnhiliteCell(mSelectedCell);

		mSelectedCell = inCell;
		HiliteCell(inCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSelectedCell
// ---------------------------------------------------------------------------

void
LTable::GetSelectedCell(
	TableCellT	&outCell) const
{
	outCell = mSelectedCell;
}

PP_End_Namespace_PowerPlant
