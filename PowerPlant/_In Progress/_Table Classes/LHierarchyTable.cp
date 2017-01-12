// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHierarchyTable.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDropFlag.h>
#include <LHierarchyTable.h>
#include <LNodeArrayTree.h>
#include <UTableHelpers.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LHierarchyTable						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LHierarchyTable::LHierarchyTable()
{
	mCustomHilite = true;
	::MacSetRect(&mFlagRect, 2, 2, 18, 14);

	mCollapsableTree = new LNodeArrayTree;
}


// ---------------------------------------------------------------------------
//	¥ LHierarchyTable						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LHierarchyTable::LHierarchyTable(
	LStream*	inStream)
	
	: LTableView(inStream)
{
	mReconcileOverhang = false;		// Force off. Otherwise, collapsing
									// a row could cause scrolling
	mCustomHilite = true;

	::MacSetRect(&mFlagRect, 2, 2, 18, 14);

	mCollapsableTree = new LNodeArrayTree;
}


// ---------------------------------------------------------------------------
//	¥ ~LHierarchyTable						Destructor				  [public]
// ---------------------------------------------------------------------------

LHierarchyTable::~LHierarchyTable()
{
	delete mCollapsableTree;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetWideOpenTableSize											  [public]
// ---------------------------------------------------------------------------
//	Pass back the number of rows and columns of the wide open Table
//	(Table with all rows fully expanded).

void
LHierarchyTable::GetWideOpenTableSize(
	TableIndexT&	outRows,
	TableIndexT&	outCols) const
{
	outRows = mCollapsableTree->CountNodes();
	outCols = mCols;
}


// ---------------------------------------------------------------------------
//	¥ SetCollapsableTree											  [public]
// ---------------------------------------------------------------------------
//	Set a new CollapsableTree for a HierarchyTable
//
//	Use to restore the state of a HierarchyTable. Make sure to set the
//	TableStorage BEFORE calling this function, and be sure that the Tree
//	has the same number of Nodes as rows of data in the Storage.

void
LHierarchyTable::SetCollapsableTree(
	LCollapsableTree*	inTree)
{
	delete mCollapsableTree;

	mCollapsableTree = inTree;

	UnselectAllCells();						// Clear selection

	mTableGeometry->RemoveRows(mRows, 1);	// Adjust Geometry to match
	mRows = inTree->CountExposedNodes();	//   exposed nodes of Tree
	mTableGeometry->InsertRows(mRows, 0);

	Refresh();								// Redraw entire Table
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Add rows to a HierarchyTable.
//
//	With a hierarchy, new rows can be inserted as either the siblings
//	or the children of the "after" row. The InsertSiblingRows() and
//	InsertChildRows() funnctions handle these two cases. However, this
//	function, inherited from LTableView, must be overridden to choose
//	between these two alternatives.
//
//	For simplicity, we choose to implement this function as the
//	equivalent of InsertSiblingRows() with the new rows not being
//	collapsable.

void
LHierarchyTable::InsertRows(
	UInt32		inHowMany,
	TableIndexT	inAfterRow,				// WideOpen Index
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inRefresh)
{
	InsertSiblingRows(inHowMany, inAfterRow, inDataPtr, inDataSize,
			false, inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ InsertSiblingRows
// ---------------------------------------------------------------------------
//	Add rows to a HierarchyTable.
//
//	Rows are added within the same parent as inAfterRow (as siblings of
//	inAfterRow).
//
//	Returns the wide open index where the first row was inserted

TableIndexT
LHierarchyTable::InsertSiblingRows(
	UInt32		inHowMany,
	TableIndexT	inAfterRow,				// WideOpen Index
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inCollapsable,
	Boolean		inRefresh)
{
	TableIndexT		lastWideOpen = mCollapsableTree->CountNodes();
	if (inAfterRow > lastWideOpen) {	// If inAfterIndex is too big,
		inAfterRow = lastWideOpen;		//   insert after last row
	}

		// Inserts Rows into CollapsableTree and store the data
		// for the new Rows

	TableIndexT	insertAtRow = mCollapsableTree->InsertSiblingNodes(inHowMany,
								inAfterRow, inCollapsable);

	if (mTableStorage != nil) {
		mTableStorage->InsertRows(inHowMany, insertAtRow - 1,
							inDataPtr, inDataSize);
	}

		// If inserted rows are exposed, we need to adjust the
		// visual display, which involves the Table Geometry,
		// Selection, and Image size.

	TableIndexT	exInsert = mCollapsableTree->GetExposedIndex(insertAtRow);
	if (exInsert > 0) {
		mRows += inHowMany;
		mTableGeometry->InsertRows(inHowMany, exInsert - 1);

		if (mTableSelector != nil) {
			mTableSelector->InsertRows(inHowMany, exInsert - 1);
		}

		AdjustImageSize(false);

		if (inRefresh) {
			RefreshRowsDownFrom(insertAtRow);
		}
	}

	return insertAtRow;
}


// ---------------------------------------------------------------------------
//	¥ InsertChildRows
// ---------------------------------------------------------------------------
//	Add rows to a HierarchyTable.
//
//	Rows are added as the first children of inParentRow
//
//	Returns the wide open index where the first row was inserted (which will
//	always be (inParentRow + 1) if insertion succeeds).

TableIndexT
LHierarchyTable::InsertChildRows(
	UInt32		inHowMany,
	TableIndexT	inParentRow,			// WideOpen Index
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inCollapsable,
	Boolean		inRefresh)
{
		// If Row is not collapsable, adding children will make it
		// collapsable. If so, we'll need to refresh the Parent
		// row in addition to all the rows below it

	TableIndexT	refreshBelowRow = inParentRow + 1;
	if (!mCollapsableTree->IsCollapsable(inParentRow)) {
		refreshBelowRow -= 1;
	}

		// Inserts Rows into CollapsableTree and store the data
		// for the new Rows

	mCollapsableTree->InsertChildNodes(inHowMany, inParentRow, inCollapsable);

	if (mTableStorage != nil) {
		mTableStorage->InsertRows(inHowMany, inParentRow,
									inDataPtr, inDataSize);
	}

		// If inserted rows are exposed, we need to adjust the
		// visual display, which involves the Table Geometry,
		// Selection, and Image size.

	TableIndexT	exParent = mCollapsableTree->GetExposedIndex(inParentRow);
	if ((exParent > 0) && mCollapsableTree->IsExpanded(inParentRow)) {
		mRows += inHowMany;
		mTableGeometry->InsertRows(inHowMany, exParent);

		if (mTableSelector != nil) {
			mTableSelector->InsertRows(inHowMany, exParent);
		}

		AdjustImageSize(false);

		if (inRefresh) {
			RefreshRowsDownFrom(refreshBelowRow);
		}
	}

	return (inParentRow + 1);
}


// ---------------------------------------------------------------------------
//	¥ AddLastChildRow
// ---------------------------------------------------------------------------
//	Add one row as the last child of inParentRow
//
//	Returns the wide open index where the row was inserted

TableIndexT
LHierarchyTable::AddLastChildRow(
	TableIndexT	inParentRow,			// WideOpen Index
	const void*	inDataPtr,
	UInt32		inDataSize,
	Boolean		inCollapsable,
	Boolean		inRefresh)
{
		// If Row is not collapsable, adding children will make it
		// collapsable. If so, we'll need to refresh the Parent
		// row in addition to all the rows below it.

	TableIndexT	refreshAdjustment = 0;
	if (!mCollapsableTree->IsCollapsable(inParentRow)) {
		refreshAdjustment = 1;			// Parent was not collapsable. It will
	}									//   always be one row above the
										//   inserted row since it does not
										//   have any other children.

		// Insert Row into CollapsableTree and store its data

	TableIndexT	woInsertAt = mCollapsableTree->AddLastChildNode(inParentRow,
															inCollapsable);
	TableIndexT	woInsertAfter = woInsertAt - 1;

	if (mTableStorage != nil) {
		mTableStorage->InsertRows(1, woInsertAfter, inDataPtr, inDataSize);
	}

		// If inserted row is exposed, we need to adjust the
		// visual display, which involves the Table Geometry,
		// Selection, and Image size.

	TableIndexT	exInsertAt = mCollapsableTree->GetExposedIndex(woInsertAt);
	if (exInsertAt > 0) {
		mRows += 1;
		mTableGeometry->InsertRows(1, exInsertAt - 1);

		if (mTableSelector != nil) {
			mTableSelector->InsertRows(1, exInsertAt - 1);
		}

		AdjustImageSize(false);

		if (inRefresh) {
			RefreshRowsDownFrom(woInsertAt - refreshAdjustment);
		}
	}

	return woInsertAt;
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Delete row from a HierarchyTable
//
//	inHowMany must be 1

void
LHierarchyTable::RemoveRows(
	UInt32		inHowMany,
	TableIndexT	inFromRow,				// WideOpen Index
	Boolean		inRefresh)

{
	if (inHowMany != 1) {
		SignalStringLiteral_("Can only remove one row at a time from a HierarchyTable");
		return;
	}

	TableIndexT	exposedRow = mCollapsableTree->GetExposedIndex(inFromRow);

		// Remove Row from CollapsableTree and delete the data
		// for the removed rows

	UInt32	totalRemoved, exposedRemoved;
	mCollapsableTree->RemoveNode(inFromRow, totalRemoved, exposedRemoved);

	if (mTableStorage != nil) {
		mTableStorage->RemoveRows(totalRemoved, inFromRow);
	}

		// Adjust the visual display of the Table if we removed
		// any exposed rows

	if (exposedRemoved > 0) {
		if (inRefresh) {				// Redraw cells below removed rows
			STableCell	topLeftCell(exposedRow, 1);
			STableCell	botRightCell(mRows, mCols);

			RefreshCellRange(topLeftCell, botRightCell);
		}

		mRows -= exposedRemoved;
		mTableGeometry->RemoveRows(exposedRemoved, exposedRow);

		if (mTableSelector != nil) {
			mTableSelector->RemoveRows(exposedRemoved, exposedRow);
		}

		AdjustImageSize(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllRows
// ---------------------------------------------------------------------------
//	Remove all rows, leaving the table empty
//
//	This is faster than removing rows one at a time

void
LHierarchyTable::RemoveAllRows(
	Boolean		inRefresh)
{
	if (mRows > 0) {
	
			// Set internal row count to zero. The Geometry, Storage,
			// or Selector may query the table while removing entries
			// and they expect the row count to be already adjusted.

		TableIndexT		revealedRows = mRows;
		mRows = 0;
		
			// The Geometry and Selector deal with revealed rows,
			// so we remove all revealed rows from them. Storage
			// has data for all cells, even those that are collapsed,
			// so we remove data for the number of "wide open" rows.
		
		mTableGeometry->RemoveRows(revealedRows, 1);
		
		if (mTableStorage != nil) {
			mTableStorage->RemoveRows(mCollapsableTree->CountNodes(), 1);
		}
		
		if (mTableSelector != nil) {
			mTableSelector->RemoveRows(revealedRows, 1);
		}

		delete mCollapsableTree;	// Make an empty tree
		mCollapsableTree = new LNodeArrayTree;
		
		if (inRefresh) {			// All rows removed, so refresh
			Refresh();				//   the entire Pane
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CollapseRow
// ---------------------------------------------------------------------------
//	Collapse the specified Row, visually hiding its child rows

void
LHierarchyTable::CollapseRow(
	TableIndexT	inWideOpenRow)
{
	if (!mCollapsableTree->IsCollapsable(inWideOpenRow)) {
		SignalStringLiteral_("Can't collapse an uncollapsable row");
		return;
	}
								// Collapse within Hierarchy storage
	UInt32	rowsConcealed = mCollapsableTree->CollapseNode(inWideOpenRow);

	if (rowsConcealed > 0) {
		ConcealRowsBelow(rowsConcealed, inWideOpenRow);
	}
}


// ---------------------------------------------------------------------------
//	¥ DeepCollapseRow
// ---------------------------------------------------------------------------
//	Collapse the specified Row and all its child rows

void
LHierarchyTable::DeepCollapseRow(
	TableIndexT	inWideOpenRow)
{
	if (!mCollapsableTree->IsCollapsable(inWideOpenRow)) {
		SignalStringLiteral_("Can't collapse an uncollapsable row");
		return;
	}
								// Collapse within Hierarchy storage
	UInt32	rowsConcealed = mCollapsableTree->DeepCollapseNode(inWideOpenRow);

	if (rowsConcealed > 0) {
		ConcealRowsBelow(rowsConcealed, inWideOpenRow);
	}
}


// ---------------------------------------------------------------------------
//	¥ ExpandRow
// ---------------------------------------------------------------------------
//	Expand the specified Row, visually revealing its child rows

void
LHierarchyTable::ExpandRow(
	TableIndexT	inWideOpenRow)
{
	if (!mCollapsableTree->IsCollapsable(inWideOpenRow)) {
		SignalStringLiteral_("Can't expand an uncollapsable row");
		return;
	}

	UInt32	rowsRevealed = mCollapsableTree->ExpandNode(inWideOpenRow);

	if (rowsRevealed > 0) {
		RevealRowsBelow(rowsRevealed, inWideOpenRow);
	}
}


// ---------------------------------------------------------------------------
//	¥ DeepExpandRow
// ---------------------------------------------------------------------------
//	Expand the specified Row and all its child rows

void
LHierarchyTable::DeepExpandRow(
	TableIndexT	inWideOpenRow)
{
	if (!mCollapsableTree->IsCollapsable(inWideOpenRow)) {
		SignalStringLiteral_("Can't expand an uncollapsable row");
		return;
	}

	UInt32	rowsRevealed = mCollapsableTree->DeepExpandNode(inWideOpenRow);

	if (rowsRevealed > 0) {
		RevealRowsBelow(rowsRevealed, inWideOpenRow);
	}
}


// ---------------------------------------------------------------------------
//	¥ RevealRow
// ---------------------------------------------------------------------------
//	Make the specified row visible within the Table by expanding all
//	of its parent rows

void
LHierarchyTable::RevealRow(
	TableIndexT	inWideOpenRow)
{
	if (mCollapsableTree->GetExposedIndex(inWideOpenRow) == 0) {
											// Row is not currently exposed
											// Expand all its ancestors
		TableIndexT	woIndex = inWideOpenRow;
		TableIndexT	lastExpanded;
		while (true) {
			woIndex = mCollapsableTree->GetParentIndex(woIndex);

			if (woIndex > 0) {
				ExpandRow(woIndex);
				lastExpanded = woIndex;
			} else {
				break;
			}
		}

		RefreshRowsDownFrom(lastExpanded);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ RefreshRowsDownFrom
// ---------------------------------------------------------------------------
//	Force a redraw at the next update event of all the rows in the Table
//	from the specified row down to the last row

void
LHierarchyTable::RefreshRowsDownFrom(
	TableIndexT	inWideOpenRow)
{
	Boolean		exposed = true;
	TableIndexT	exRow = 0;

	if (inWideOpenRow > 0) {
		exRow = mCollapsableTree->GetExposedIndex(inWideOpenRow);
		exposed = (exRow > 0);
	}

	if (exposed) {
		STableCell	topLeftCell(exRow, 1);
		STableCell	botRightCell(mRows, mCols);
		RefreshCellRange(topLeftCell, botRightCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ RefreshRowsBelow
// ---------------------------------------------------------------------------
//	Force a redraw at the next update event of all the rows in the Table
//	below (NOT including) the specified row

void
LHierarchyTable::RefreshRowsBelow(
	TableIndexT	inWideOpenRow)
{
	Boolean		exposed = true;
	TableIndexT	exRow = 0;

	if (inWideOpenRow > 0) {
		exRow = mCollapsableTree->GetExposedIndex(inWideOpenRow);
		exposed = (exRow > 0);
	}

	if (exposed) {
		STableCell	topLeftCell(exRow + 1, 1);
		STableCell	botRightCell(mRows, mCols);
		RefreshCellRange(topLeftCell, botRightCell);
	}
}


// ---------------------------------------------------------------------------
//	¥ ConcealRowsBelow
// ---------------------------------------------------------------------------
//	Conceal rows below the specified row
//
//	Concealing visually removes the rows from the Table, but does not
//	remove the data for the Cells in the rows from mTableStorage

void
LHierarchyTable::ConcealRowsBelow(
	UInt32		inHowMany,
	TableIndexT	inWideOpenRow)
{
	RefreshRowsBelow(inWideOpenRow);

	mRows -= inHowMany;

	TableIndexT	exRow = mCollapsableTree->GetExposedIndex(inWideOpenRow);
	mTableGeometry->RemoveRows(inHowMany, exRow + 1);

	if (mTableSelector != nil) {
		mTableSelector->RemoveRows(inHowMany, exRow + 1);
	}

	AdjustImageSize(false);
}


// ---------------------------------------------------------------------------
//	¥ RevealRowsBelow
// ---------------------------------------------------------------------------
//	Reveal rows below the specified row
//
//	Revealing visually adds the rows to the Table, but the data for
//	the Cells in the rows is already in mTableStorage

void
LHierarchyTable::RevealRowsBelow(
	UInt32		inHowMany,
	TableIndexT	inWideOpenRow)
{
	mRows += inHowMany;
	TableIndexT	exRow = mCollapsableTree->GetExposedIndex(inWideOpenRow);

	mTableGeometry->InsertRows(inHowMany, exRow);

	if (mTableSelector != nil) {
		mTableSelector->InsertRows(inHowMany, exRow);
	}

	AdjustImageSize(false);

	RefreshRowsBelow(inWideOpenRow);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcCellFlagRect
// ---------------------------------------------------------------------------
//	Pass back the rectangle, in local coordinates, containing the DropFlag
//	for a Cell

void
LHierarchyTable::CalcCellFlagRect(
	const STableCell&	inCell,
	Rect&				outRect)
{
	Rect	cellRect;
	GetLocalCellRect(inCell, cellRect);

	outRect.left   = (SInt16) (cellRect.left + mFlagRect.left);
	outRect.top    = (SInt16) (cellRect.top + mFlagRect.top);
	outRect.right  = (SInt16) (outRect.left + (mFlagRect.right - mFlagRect.left));
	outRect.bottom = (SInt16) (outRect.top + (mFlagRect.bottom - mFlagRect.top));
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------
//	Handle a mouse click within a HierarchyTable

void
LHierarchyTable::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	STableCell	hitCell;
	SPoint32	imagePt;

	LocalToImagePoint(inMouseDown.whereLocal, imagePt);

	if (GetCellHitBy(imagePt, hitCell)) {
										// Click is inside hitCell
										// Check if click is inside DropFlag
		TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(hitCell.row);
		Rect	flagRect;
		CalcCellFlagRect(hitCell, flagRect);

		if (mCollapsableTree->IsCollapsable(woRow) &&
			::MacPtInRect(inMouseDown.whereLocal, &flagRect)) {
										// Click is inside DropFlag
			FocusDraw();
			ApplyForeAndBackColors();
			Boolean	expanded = mCollapsableTree->IsExpanded(woRow);
			if (LDropFlag::TrackClick(flagRect, inMouseDown.whereLocal,
									expanded)) {
										// Mouse released inside DropFlag
										//   so toggle the Row
				if (inMouseDown.macEvent.modifiers & optionKey) {
										// OptionKey down means to do
										//   a deep collapse/expand
					if (expanded) {
						DeepCollapseRow(woRow);
					} else {
						DeepExpandRow(woRow);
					}

				} else {				// Shallow collapse/expand
					if (expanded) {
						CollapseRow(woRow);
					} else {
						ExpandRow(woRow);
					}
				}
			}

		} else if (ClickSelect(hitCell, inMouseDown)) {
										// Click outside of the DropFlag
			ClickCell(hitCell, inMouseDown);
		}

	} else {							// Click is outside of any Cell
		UnselectAllCells();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawDropFlag
// ---------------------------------------------------------------------------
//	Draw the DropFlag in a HierarchyTable Cell

void
LHierarchyTable::DrawDropFlag(
	const STableCell&	inCell,
	TableIndexT			inWideOpenRow)
{
	Rect	flagRect;
	CalcCellFlagRect(inCell, flagRect);

	if (mCollapsableTree->IsCollapsable(inWideOpenRow)) {
		LDropFlag::Draw(flagRect, mCollapsableTree->IsExpanded(inWideOpenRow));
	}
}


PP_End_Namespace_PowerPlant
