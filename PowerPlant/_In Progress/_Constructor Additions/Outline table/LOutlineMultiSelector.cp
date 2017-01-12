// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineMultiSelector.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOutlineMultiSelector.h>
#include <LOutlineItem.h>
#include <LOutlineTable.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LOutlineMultiSelector					Constructor				  [public]
// ---------------------------------------------------------------------------

LOutlineMultiSelector::LOutlineMultiSelector(
	LOutlineTable*	inOutlineTable)

	: LTableMultiSelector(inOutlineTable)
{
	mOutlineTable			= inOutlineTable;
	mAddToSelection			= nil;
	mRemoveFromSelection	= nil;
	mInvertSelection		= nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LOutlineMultiSelector				Destructor				  [public]
// ---------------------------------------------------------------------------

LOutlineMultiSelector::~LOutlineMultiSelector()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DragSelect
// ---------------------------------------------------------------------------
//	Perform a Finder-like marquee selection across the items in this table.

Boolean
LOutlineMultiSelector::DragSelect(
	const STableCell&		/* inCell */,
	const SMouseDownEvent&	inMouseDown)
{
	StRegion	addRegion;				// Allocate regions
	StRegion	removeRegion;
	StRegion	invertRegion;

	mAddToSelection			= addRegion;
	mRemoveFromSelection	= removeRegion;
	mInvertSelection		= invertRegion;

										// Do the marquee selection.
	try {
		LMarqueeTask marquee(*this, inMouseDown);
		marquee.DoMarquee();
	}

	catch(...) { }

	mAddToSelection			= nil;		// StRegions will be disposed
	mRemoveFromSelection	= nil;
	mInvertSelection		= nil;

	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CheckSelectionChanged										   [protected]
// ---------------------------------------------------------------------------
//	Return true if the table needs to be redrawn (i.e. selection changed
//	or auto-scrolling is warranted). Do NOT perform any drawing at this
//	time as it may interfere with the marquee drawing.

Boolean
LOutlineMultiSelector::CheckSelectionChanged(
	const LMarqueeTask&	inMarqueeTask,
	const Rect&			inOldMarqueeRect,
	const Rect&			inNewMarqueeRect)
{

	// See if the table will want to scroll.

	Point localPoint;
	inMarqueeTask.GetCurrentPoint(localPoint);
	mOutlineTable->LocalToPortPoint(localPoint);
	Boolean wantsToAutoScroll = !mOutlineTable->PointIsInFrame(localPoint.h, localPoint.v);

	// See if the selection has changed at all.

	CheckCells(inOldMarqueeRect, inNewMarqueeRect);

	// Return true if either condition applies.

	return !::EmptyRgn(mAddToSelection)
		|| !::EmptyRgn(mRemoveFromSelection)
		|| wantsToAutoScroll;

}


// ---------------------------------------------------------------------------
//	¥ UpdateSelection										[protected]
// ---------------------------------------------------------------------------
//	Called by LMarqueeReceiver whenever CheckSelectionChanged returns
//	true. The marquee indication has been removed, so it is now safe
//	to change the selection and redraw it.

void
LOutlineMultiSelector::UpdateSelection(
	const LMarqueeTask&	inMarqueeTask,
	const Rect&			/* inOldMarqueeRect */,
	const Rect&			/* inNewMarqueeRect */)
{

	// Auto-scroll the display.

	Point localPoint;
	inMarqueeTask.GetCurrentPoint(localPoint);
	mOutlineTable->AutoScrollImage(localPoint);

	// Some objects moved in or out of the selection, adjust
	// selection accordingly.

	AdjustSelection(mAddToSelection, true);
	AdjustSelection(mRemoveFromSelection, false);

	// Empty the selection change lists.

	::SetEmptyRgn(mAddToSelection);
	::SetEmptyRgn(mRemoveFromSelection);

	// Make sure the display is updated.

	mOutlineTable->UpdatePort();

}


// ---------------------------------------------------------------------------
//	¥ FocusMarqueeArea										[protected]
// ---------------------------------------------------------------------------
//	Prepare for drawing in the table.

void
LOutlineMultiSelector::FocusMarqueeArea()
{
	mOutlineTable->FocusDraw();
}

// ---------------------------------------------------------------------------
//	¥ GetTargetView											[protected]
// ---------------------------------------------------------------------------
//	Return a pointer to the view we're working in.

const LView*	LOutlineMultiSelector::GetTargetView() const
{
	return mOutlineTable;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CheckCells											[protected]
// ---------------------------------------------------------------------------
//	Ask the outline table if any cells have moved into or out of the
//	selection area. Record these changes so that we can update the
//	selection quickly when UpdateSelection is called.
//
//	Do NOT perform any drawing at this time. It will interfere with the
//	marquee drawing.

void
LOutlineMultiSelector::CheckCells(
	const Rect&	inOldMarqueeRect,
	const Rect&	inNewMarqueeRect)
{
	// Hit-test all cells in the junction of the old and new marquee.

	Rect testRect = inOldMarqueeRect;
	::MacUnionRect(&testRect, &inNewMarqueeRect, &testRect);

	STableCell topLeftCell, botRightCell;
	mOutlineTable->FetchIntersectingCells(testRect, topLeftCell, botRightCell);

	// Iterate over all affected cells.

	STableCell cell;
	for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
		for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {

			// See if the marquee touches this cell's contents.

			Boolean cellIsHitBy = mOutlineTable->CellHitByMarquee(cell, inNewMarqueeRect);

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
}


// ---------------------------------------------------------------------------
//	¥ AdjustSelection										[protected]
// ---------------------------------------------------------------------------
//	Selection changes are stored in regions (just like LTableMultiSelector
//	stores the original selection). Add or remove cells from the selection
//	based on the region that is handed in.

void
LOutlineMultiSelector::AdjustSelection(
	const RgnHandle	inRgnHandle,
	Boolean			inAddToSelection)
{
	STableCell cell;
	Rect bbox;
	::GetRegionBounds(inRgnHandle, &bbox);

	for (cell.row = bbox.top; cell.row <= bbox.bottom; cell.row++) {
		for (cell.col = bbox.left; cell.col <= bbox.right; cell.col++) {
			Point cellAsPoint;
			cell.ToPoint(cellAsPoint);
			if (::PtInRgn(cellAsPoint, inRgnHandle)) {
				if (inAddToSelection)
					mOutlineTable->SelectCell(cell);
				else
					mOutlineTable->UnselectCell(cell);
			}
		}
	}
}

PP_End_Namespace_PowerPlant
