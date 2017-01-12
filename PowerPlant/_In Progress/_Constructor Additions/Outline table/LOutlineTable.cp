// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineTable.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOutlineTable.h>
#include <LOutlineItem.h>

#include <LDragAndDrop.h>
#include <LFastArrayIterator.h>
#include <LStream.h>
#include <UMemoryMgr.h>
#include <UTableHelpers.h>
#include <UTBAccessors.h>

#include <Drag.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LOutlineTable							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//
//	Stream Data
//		SInt16:		Text traits ID
//		UInt16:		First level indent (in pixels)

LOutlineTable::LOutlineTable(
	LStream* inStream)

: LTableView(inStream),
  mOutlineItems(sizeof (LOutlineItem*)),
  mFirstLevelItems(sizeof (LOutlineItem*))

{
	SetUseDragSelect(true);

	*inStream >> mDefaultTxtrID;
	*inStream >> mFirstLevelIndent;
	mOkayToAdjustImageSize = true;
	mMaximumIndent = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LOutlineTable						Destructor				  [public]
// ---------------------------------------------------------------------------

LOutlineTable::~LOutlineTable()
{

	// Delete all of the outline item objects that are left in the table.

	LFastArrayIterator iter(mOutlineItems);
	LOutlineItem* item;
	while (iter.Next(&item)) {
		delete item;
	}
}


// ===========================================================================

#pragma mark -
#pragma mark ** item accessors

// ---------------------------------------------------------------------------
//		* FindItemForRow
// ---------------------------------------------------------------------------
//	Return the OutlineItem which is drawn in a given row.

LOutlineItem*
LOutlineTable::FindItemForRow(
	TableIndexT inRow) const
{
	LOutlineItem* item = nil;
	mOutlineItems.FetchItemAt((ArrayIndexT) inRow, &item);
	return item;
}


// ---------------------------------------------------------------------------
//		* FindRowForItem
// ---------------------------------------------------------------------------
//	Given an OutlineItem, find the row it's drawn in.

TableIndexT
LOutlineTable::FindRowForItem(
	const LOutlineItem* inOutlineItem) const
{
	return (TableIndexT) mOutlineItems.FetchIndexOf(&inOutlineItem);
}


// ===========================================================================

#pragma mark -
#pragma mark ** row management

// ---------------------------------------------------------------------------
//		* InsertItem
// ---------------------------------------------------------------------------
//	Insert a new item into the outline. If inSuperItem is nil, it will
//	be placed as a top-level item; otherwise it will be a child of
//	inSuperItem. If inAfterItem is nil, it will be placed as the first
//	child of inSuperItem (or first item in the outline); otherwise it will
//	follow the named item.

void
LOutlineTable::InsertItem(
	LOutlineItem*	inOutlineItem,
	LOutlineItem*	inSuperItem,
	LOutlineItem*	inAfterItem,
	Boolean			inRefresh,
	Boolean			inAdjustImageSize)
{

	// Disallow image-size calculations if there are lots of items
	// being inserted. This saves *lots* of time.

	StValueChanger<Boolean> adjust(mOkayToAdjustImageSize, inAdjustImageSize);

	// Find a home for this item.

	TableIndexT afterRow;
	if (inAfterItem != nil) {

		// Insert as sibling after named item.

		afterRow = FindRowForItem(inAfterItem);
		if (afterRow == 0) {
			SignalStringLiteral_("LOutlineTable::InsertItem - inserting after sibling that doesn't exist");
		}
		afterRow += inAfterItem->DeepCountSubItems();

	}
	else if (inSuperItem != nil) {

		// Insert as first child of super item.

		afterRow = FindRowForItem(inSuperItem);
		if (afterRow == 0) {
			SignalStringLiteral_("LOutlineTable::InsertItem - inserting after super item that doesn't exist");
		}
	}
	else {

		// Insert as first item in outline.

		afterRow = 0;

	}

	// Configure this item.

	inOutlineItem->mOutlineTable = this;
	inOutlineItem->mSuperItem = inSuperItem;
	if (inSuperItem != nil) {
		inOutlineItem->mLeftEdge = (SInt16) (inSuperItem->mLeftEdge + inSuperItem->mIndentSize);
		inSuperItem->AddSubItem(inOutlineItem, inAfterItem);
	}
	else {
		inOutlineItem->mLeftEdge = (SInt16) mFirstLevelIndent;
		inOutlineItem->mIndentDepth = 1;
	}

	// Place it in outline.

	mOutlineItems.InsertItemsAt(1, (ArrayIndexT) afterRow + 1, &inOutlineItem);
	LTableView::InsertRows(1, afterRow, nil, 0, inRefresh);
	if (inSuperItem == nil) {
		mFirstLevelItems.InsertItemsAt(1,
					mFirstLevelItems.FetchIndexOf(&inAfterItem) + 1, &inOutlineItem);
	}

	// See if maximum indentation level has changed.

	if (inOutlineItem->mIndentDepth > mMaximumIndent) {
		mMaximumIndent = inOutlineItem->mIndentDepth;
		MaximumIndentChanged();
	}

	// Allow cell to do post-install configuration.

	inOutlineItem->FinishCreate();
	inOutlineItem->UpdateRowSize();

}


// ---------------------------------------------------------------------------
//		* RemoveItem
// ---------------------------------------------------------------------------
//	Remove the named item from the outline. If it has children, remove
//	them as well.

void
LOutlineTable::RemoveItem(
	LOutlineItem*	inOutlineItem,
	Boolean			inRefresh,
	Boolean			inAdjustImageSize)
{

	// Disallow image-size calculations if there are lots of items
	// being inserted. This saves *lots* of time.

	StValueChanger<Boolean> adjust(mOkayToAdjustImageSize, inAdjustImageSize);

	// Get rid of subitems first.

	inOutlineItem->Collapse();

	// Find this item in the table.

	TableIndexT itemRow = FindRowForItem(inOutlineItem);
	if (itemRow == 0)
		return;

	// Remove it.

	LTableView::RemoveRows(1, itemRow, inRefresh);
	mOutlineItems.RemoveItemsAt(1, (ArrayIndexT) itemRow);
	mFirstLevelItems.Remove(&inOutlineItem);

	LOutlineItem* superItem = inOutlineItem->mSuperItem;
	if (superItem != nil)
		superItem->RemoveSubItem(inOutlineItem);

	// See if this might affect the maximum indentation of the table.
	// The only case where the table depth might be affected is when
	// the the deleted item has no siblings and is at the current maximum
	// indentation of the table. In this case, we have to traverse
	// the entire table and look for another item at the same indentation
	// level.

	if (inOutlineItem->GetIndentDepth() == mMaximumIndent) {

		UInt32 numSiblings = 0;
		if ((superItem != nil) && (superItem->mSubItems != nil))
			numSiblings = superItem->mSubItems->GetCount();
		else
			numSiblings = mFirstLevelItems.GetCount();

		if (numSiblings == 0)
			RecalcMaxIndent();

	}

	// Delete the item itself.

	delete inOutlineItem;

}


// ===========================================================================

#pragma mark -
#pragma mark ** column management

// ---------------------------------------------------------------------------
//		* InsertCols
// ---------------------------------------------------------------------------
//	Overriden to inform the outline items about the new column.

void
LOutlineTable::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol,
	const void*		inDataPtr,
	UInt32			inDataSize,
	Boolean			inRefresh)
{

	// Adjust the table geometry for the new columns.

	LTableView::InsertCols(inHowMany, inAfterCol, inDataPtr, inDataSize, inRefresh);

	// Tell the outline items that new cells have been added.

	LFastArrayIterator iter(mOutlineItems);
	LOutlineItem* item;
	while (iter.Next(&item)) {
		item->InsertCols(inHowMany, inAfterCol, inDataPtr, inDataSize, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//		* RemoveCols
// ---------------------------------------------------------------------------
//	Overriden to inform the outline items about the removed column.

void
LOutlineTable::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol,
	Boolean			inRefresh)
{

	// Adjust the table geometry for the removed columns.

	LTableView::RemoveCols(inHowMany, inFromCol, inRefresh);

	// Tell each outline item that columns have been removed.

	LFastArrayIterator iter(mOutlineItems);
	LOutlineItem* item;
	while (iter.Next(&item)) {
		item->RemoveCols(inHowMany, inFromCol, inRefresh);
	}
}


// ===========================================================================

#pragma mark -
#pragma mark ** selecting cells

// ---------------------------------------------------------------------------
//		* HiliteCell
// ---------------------------------------------------------------------------
//	Ask the outline item to redraw the selection for this cell.

void
LOutlineTable::HiliteCell(
	const STableCell&	inCell,
	Boolean				/* inHilite */)
{

	// Ask the cell to redraw the selection.

	LOutlineItem* item = FindItemForRow(inCell.row);
	if (item != nil)
		item->RefreshSelection(inCell);

}


// ---------------------------------------------------------------------------
//		* HiliteSelection
// ---------------------------------------------------------------------------
//	Overriden to highlight cells individually. The region-based approach
//	in LTableView won't work because the outline items control their own
//	highlighting.

void
LOutlineTable::HiliteSelection(
	Boolean		/* inActively */,
	Boolean		inHilite)
{

	// Ask each selected cell to redraw its selection.

	STableCell cell;
	while (GetNextSelectedCell(cell)) {
		HiliteCell(cell, inHilite);
	}
}


// ---------------------------------------------------------------------------
//		* CellHitByMarquee
// ---------------------------------------------------------------------------
//	Return true if the named cell is touched by the selection marquee.

Boolean
LOutlineTable::CellHitByMarquee(
	const STableCell&	inCell,
	const Rect&			inMarqueeLocalRect)
{
	LOutlineItem* item = FindItemForRow(inCell.row);
	if (item != nil)
		return item->CellHitByMarquee(inCell, inMarqueeLocalRect);
	else
		return false;
}


// ---------------------------------------------------------------------------
//	¥ GetLocalCellRect
// ---------------------------------------------------------------------------
//	Pass back the bounding rectangle of the specified Cell and return
//	whether it intersects the Frame of the TableView
//
//	The bounding rectangle is in Local coordinates so it will always be
//	within QuickDraw space when its within the Frame. If the bounding
//	rectangle is outside the Frame, return false but do NOT set the rectangle
//	to (0,0,0,0) (this is opposite of what LTableView::GetLocalCellRect does)

Boolean
LOutlineTable::GetLocalCellRect(
	const STableCell	&inCell,
	Rect				&outCellRect) const
{
	SInt32	cellLeft, cellTop, cellRight, cellBottom;
	mTableGeometry->GetImageCellBounds(inCell, cellLeft, cellTop,
							cellRight, cellBottom);

	Boolean	insideFrame =
		ImageRectIntersectsFrame(cellLeft, cellTop, cellRight, cellBottom);

	SPoint32	imagePt;
	imagePt.h = cellLeft;
	imagePt.v = cellTop;
	ImageToLocalPoint(imagePt, topLeft(outCellRect));
	outCellRect.right  = (SInt16) (outCellRect.left + (cellRight - cellLeft));
	outCellRect.bottom = (SInt16) (outCellRect.top + (cellBottom - cellTop));

	return insideFrame;
}

// ===========================================================================

#pragma mark -
#pragma mark ** refreshing

// ---------------------------------------------------------------------------
//		* RefreshCellRange
// ---------------------------------------------------------------------------
//	Overriden to include the space to the right of the last cell. This is
//	done because many outline items shade the entire row (which may extend
//	to the right of the last cell).

void
LOutlineTable::RefreshCellRange(
	const STableCell&	inTopLeft,
	const STableCell&	inBotRight)
{
		// We only need to refresh the portion of the cell range
		// that is revealed within the Frame of the Table

									// Nothing to refresh if invisible
	Rect	frameRect;				//   or beyond QuickDraw space
	if (!IsVisible()  ||  !CalcLocalFrameRect(frameRect)) return;

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

		// If refreshing last column, inval all the way to
		// the right side of the frame

	if (refreshBotRight.col == mCols) {
		refreshRect.right = frameRect.right;
	}

		// Convert refreshRect to Port coordinates

	LocalToPortPoint(topLeft(refreshRect));
	LocalToPortPoint(botRight(refreshRect));

		// Restrict refreshRect to revealed portion of Table

	if (::SectRect(&refreshRect, &mRevealedRect, &refreshRect)) {
										// Force update if any cells
		InvalPortRect(&refreshRect);	//   in the range are revealed
	}
}


// ---------------------------------------------------------------------------
//		* AdjustImageSize
// ---------------------------------------------------------------------------
//	Overriden to disable image adjusting. Image size adjusting is typically
//	disabled only when calling InsertItem or RemoveItem.

void
LOutlineTable::AdjustImageSize(
	Boolean		inRefresh)
{
	if (mOkayToAdjustImageSize)
		LTableView::AdjustImageSize(inRefresh);
}


// ===========================================================================

#pragma mark -
#pragma mark ** drawing

// ---------------------------------------------------------------------------
//		* DrawSelf												[protected]
// ---------------------------------------------------------------------------
//	Overriden to allow outline items to do row-wide drawing.

void
LOutlineTable::DrawSelf()
{
	DrawBackground();

	// Determine cells that need updating. Rather than checking
	// on a cell by cell basis, we just see which cells intersect
	// the bounding box of the update region. This is relatively
	// fast, but may result in unnecessary cell updates for
	// non-rectangular update regions.

	RgnHandle localUpdateRgnH = GetLocalUpdateRgn();
	Rect updateRect;
	::GetRegionBounds(localUpdateRgnH, &updateRect);

	::DisposeRgn(localUpdateRgnH);

	STableCell topLeftCell, botRightCell;
	FetchIntersectingCells(updateRect, topLeftCell, botRightCell);
	
	Rect	frameRect;
	CalcLocalFrameRect(frameRect);

	// Draw each cell within the update rect.

	STableCell cell;
	for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {

		// Offer row a chance to draw a row-wide adornment.

		LOutlineItem* item = FindItemForRow(cell.row);
		if (item != nil) {
		
				// Get the bounds of the topLeftCell. Then set the
				// left and right to that of the frame rect so that
				// the rect covers the entire row.
		
			Rect	rowRect;
			cell.col = topLeftCell.col;
			
			GetLocalCellRect(cell, rowRect);
			
			rowRect.left  = frameRect.left;
			rowRect.right = frameRect.right;

			item->DrawRowAdornments(rowRect);
		}

		// Draw individual cells.

		for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
			Rect cellRect;
			GetLocalCellRect(cell, cellRect);
			DrawCell(cell, cellRect);
		}
	}
}


// ---------------------------------------------------------------------------
//		* DrawCell												[protected]
// ---------------------------------------------------------------------------
//	Ask the outline item to draw the contents for a given cell.

void
LOutlineTable::DrawCell(
	const STableCell&	inCell,
	const Rect&			inLocalRect)
{
	LOutlineItem* item = FindItemForRow(inCell.row);
	if (item != nil)
		item->DrawCell(inCell, inLocalRect);
}


// ---------------------------------------------------------------------------
//		* GetHiliteRgn											[protected]
// ---------------------------------------------------------------------------
//	Overriden to return a nil region. Since the outline items control
//	their own selection drawing, the region-based system in LTableView
//	cannot be used.

void
LOutlineTable::GetHiliteRgn(
	RgnHandle /* ioHiliteRgn */)
{
	// hilite region is always empty: we draw selection differently
}


// ===========================================================================

#pragma mark -
#pragma mark ** clicking

// ---------------------------------------------------------------------------
//		* ClickSelf												[protected]
// ---------------------------------------------------------------------------
//	If no cell is hit by the click, try drag selection.

void
LOutlineTable::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	SOutlineDrawContents	emptyDrawInfo;
	STableCell				hitCell;
	SPoint32				imagePt;

	LocalToImagePoint(inMouseDown.whereLocal, imagePt);

	if (GetCellHitBy(imagePt, hitCell))
		ClickCell(hitCell, inMouseDown);
	else
		TrackEmptyClick(hitCell, inMouseDown, emptyDrawInfo);		// click is outside all cells, try drag selection
}


// ---------------------------------------------------------------------------
//		* ClickCell												[protected]
// ---------------------------------------------------------------------------
//	Ask the outline item to respond to a click in a given cell.

void
LOutlineTable::ClickCell(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
	LOutlineItem* item = FindItemForRow(inCell.row);
	if (item != nil)
		item->ClickCell(inCell, inMouseDown);
}


// ---------------------------------------------------------------------------
//		* TrackDrag												[protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineItem::TrackDrag when a drag is detected from
//	within the content area of an item. Override to track the drag.

void
LOutlineTable::TrackDrag(
	const STableCell&		/* inCell */,
	const SMouseDownEvent&	/* inMouseDown */)
{
	// override hook
}


// ---------------------------------------------------------------------------
//		* TrackEmptyClick										[protected]
// ---------------------------------------------------------------------------
//	A mouse-down has been detected that falls outside the content of all
//	cells. If the mouse moves, drag select cells. If not, deselect all cells.

void
LOutlineTable::TrackEmptyClick(
	const STableCell&			inCell,
	const SMouseDownEvent&		inMouseDown,
	const SOutlineDrawContents&	/* inDrawContents */)
{
	if (mTableSelector == nil) {
		return;
	}

	// If shift key is down, we extend the selection (in the same sense that
	// the Finder does). If not, we erase the selection.

	if (!(inMouseDown.macEvent.modifiers & shiftKey)) {
		UnselectAllCells();
		UpdatePort();
	}

	// If mouse moves, we trigger a drag selection.

	if (mUseDragSelect &&
		LDragAndDrop::DragAndDropIsPresent() &&
		::WaitMouseMoved(inMouseDown.macEvent.where)) {

		mTableSelector->DragSelect(inCell, inMouseDown);
	}
}


// ===========================================================================

#pragma mark -
#pragma mark ** table sizing

// ---------------------------------------------------------------------------
//		* RecalcMaxIndent										[protected]
// ---------------------------------------------------------------------------
//	Called by RemoveItem whenever the table's maximum nesting depth might
//	have changed. Traverses the table until it finds an item at the current
//	depth, or all rows have been visited.

void
LOutlineTable::RecalcMaxIndent()
{
	UInt32 maxIndent = 0;

	LArrayIterator iter(mOutlineItems, LArrayIterator::from_End);
	LOutlineItem* item;

	while (iter.Previous(&item)) {
		UInt32 indent = item->mIndentDepth;
		if (indent == mMaximumIndent)
			return;
		if (maxIndent < indent)
			maxIndent = indent;
	}

	// If we get this far, indentation level has changed.

	mMaximumIndent = (UInt16) maxIndent;
	MaximumIndentChanged();

}


// ---------------------------------------------------------------------------
//		* MaximumIndentChanged									[protected]
// ---------------------------------------------------------------------------
//	Override hook. The maximum indentation level of the table has changed.
//	Subclasses might want to use this for adjusting column widths, etc.

void
LOutlineTable::MaximumIndentChanged()
{
	// override hook
}


// ===========================================================================

#pragma mark -
#pragma mark ** disallowed functions

// ---------------------------------------------------------------------------
//		* InsertRows											[private]
// ---------------------------------------------------------------------------
//	Disallowed. Use InsertItem to create a new row.

void
LOutlineTable::InsertRows(
	UInt32			/* inHowMany */,
	TableIndexT		/* inAfterRow */,
	const void*		/* inDataPtr */,
	UInt32			/* inDataSize */,
	Boolean			/* inRefresh */)
{
	SignalStringLiteral_("LOutlineTable::InsertRows not allowed");
}


// ---------------------------------------------------------------------------
//		* RemoveRows											[private]
// ---------------------------------------------------------------------------
//	Disallowed. Use RemoveItem to delete a row.

void
LOutlineTable::RemoveRows(
	UInt32		/* inHowMany */,
	TableIndexT	/* inFromRow */,
	Boolean		/* inRefresh */)
{
	SignalStringLiteral_("LOutlineTable::RemoveRows not allowed");
}


// ===========================================================================

#pragma mark -
#pragma mark ** in-place editing helper

// ---------------------------------------------------------------------------
//		* SetCurrentInPlaceEditCell								[private]
// ---------------------------------------------------------------------------
//	Called by LEditableOutlineItem::StartInPlaceEdit to inform the table
//	that a cell is being edited.

void
LOutlineTable::SetCurrentInPlaceEditCell(
	const STableCell&	inCell)
{
	mCurrentInPlaceEditCell = inCell;
}

PP_End_Namespace_PowerPlant
