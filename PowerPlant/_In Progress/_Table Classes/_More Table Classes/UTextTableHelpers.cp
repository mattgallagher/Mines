// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTextTableHelpers.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//  Family of helpers for LTextTableView

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UTextTableHelpers.h>
#include <LTextTableView.h>
#include <cctype>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LCellSizer::TruncateTrailingWord
// ---------------------------------------------------------------------------
// Trimming off a partial word if we do truncation

UInt16
LCellSizer::TruncateTrailingWord(
	const char*		inValue,	// C string
	UInt16			inEndWord) const
{
	UInt32 ix = inEndWord;
	while (ix > 0 && !PP_CSTD::isspace(inValue[ix]))
		--ix;  // skip to start of partial word

	while (ix > 0 && PP_CSTD::isspace(inValue[ix]))
		--ix;  // skip preceding whitespace

	if (ix > 0)  // was at least one word before partial word
		inEndWord = (UInt16) (ix+1);
	else
		if (PP_CSTD::isspace(inValue[1]))  // degenerate case "*<space>BLAH"
			inEndWord = 1;

	return inEndWord;
}



// ---------------------------------------------------------------------------
//	¥ LTableDragger::CalcDragRect
// ---------------------------------------------------------------------------

void
LTableDragger::CalcDragRect(
	LTableView*	fromTable,
	STableCell	startingCell,
	Rect&		outRect)
{
/*
we want an encompassing rect for the visibly selected cells.

Therefore we iterate cells for a while, until the selected cell is outside the visible rect,
and use the new cell's rectangle to push the right and bottom boundaries of our rect.

Note: there's a pathological case when all the selected cells are out of the visible area, in
which case GetNextSelectedCell will be called for all selected cells.

This is rather inefficient at CW11 as GetNextSelectedCell implemented in LTableView and
tests all cells, rather than being passed through to LTableSelector, as has been changed
for GetFirstSelectedCell.

In the assumption that the above change will ocurr, the following will then be efficient in that case.
In the normal case of the selected cell being visible, or close to the visible cells, the
GetNextSelectedCell efficiency doesn't matter.

*/
	fromTable->FocusDraw();
	fromTable->GetLocalCellRect( startingCell, outRect );
	STableCell nextCell = startingCell;
	Boolean gotOneInView = false;
	for (;;) {
		fromTable->GetNextSelectedCell(nextCell);
		if (nextCell.IsNullCell())
			break;	//******** end of selected cells

		Rect nextRect;
		fromTable->GetLocalCellRect( nextCell, nextRect );
		if (nextRect.bottom == 0) {
			if (gotOneInView)
				break;	// ********  selected cells are now beyond visible area
		}
		else {
			gotOneInView = true;  // keep iterating in case first selected cells ABOVE visible area
			if (outRect.top==0)  {	// first cell was above visible cells
				outRect.top = nextRect.top;
				outRect.left = nextRect.left;
			}
			if (nextRect.right > outRect.right)
				outRect.right = nextRect.right;
			if (nextRect.bottom > outRect.bottom)
				outRect.bottom = nextRect.bottom;
		}	// got a cell in view
	}	// loop semi-contiguous block of nearby selected cells
}


PP_End_Namespace_PowerPlant
