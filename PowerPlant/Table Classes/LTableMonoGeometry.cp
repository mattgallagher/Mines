// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMonoGeometry.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages geometry of a TableView that has the same width and height for
//	every cell

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableMonoGeometry.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableMonoGeometry
// ---------------------------------------------------------------------------
//	Construct from input parameters

LTableMonoGeometry::LTableMonoGeometry(
	LTableView		*inTableView,
	UInt16			inColWidth,
	UInt16			inRowHeight)
		: LTableGeometry(inTableView)
{
	SignalIf_(mTableView == nil);

	mColWidth  = 0;
	mRowHeight = 0;

	SetColWidth(inColWidth, 0, 0);
	SetRowHeight(inRowHeight, 0, 0);
}


// ---------------------------------------------------------------------------
//	¥ GetImageCellBounds
// ---------------------------------------------------------------------------
//	Pass back the location in Image coords of the specified Cell

void
LTableMonoGeometry::GetImageCellBounds(
	const STableCell	&inCell,
	SInt32				&outLeft,
	SInt32				&outTop,
	SInt32				&outRight,
	SInt32				&outBottom) const
{
	if (mTableView->IsValidCell(inCell)) {
		outRight  = (SInt32) inCell.col * mColWidth;
		outLeft   = outRight - mColWidth;
		outBottom = (SInt32) inCell.row * mRowHeight;
		outTop    = outBottom - mRowHeight;

	} else {
		outLeft   = 0;
		outRight  = 0;
		outTop    = 0;
		outBottom = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetRowHitBy
// ---------------------------------------------------------------------------
//	Return the index number of the Row containing the specified point

TableIndexT
LTableMonoGeometry::GetRowHitBy(
	const SPoint32	&inImagePt) const
{
	return (TableIndexT) (inImagePt.v / mRowHeight + 1);
}


// ---------------------------------------------------------------------------
//	¥ GetColHitBy
// ---------------------------------------------------------------------------
//	Return the index number of the Column containing the specified point

TableIndexT
LTableMonoGeometry::GetColHitBy(
	const SPoint32	&inImagePt) const
{
	return (TableIndexT) (inImagePt.h / mColWidth + 1);
}


// ---------------------------------------------------------------------------
//	¥ GetTableDimensions
// ---------------------------------------------------------------------------
//	Pass back size of the Table based on the number and size of the rows
//	and columns

void
LTableMonoGeometry::GetTableDimensions(
	UInt32	&outWidth,
	UInt32	&outHeight) const
{
	UInt32	rows, columns;
	mTableView->GetTableSize(rows, columns);
	outWidth = columns * mColWidth;
	outHeight = rows * mRowHeight;
}


// ---------------------------------------------------------------------------
//	¥ SetRowHeight
// ---------------------------------------------------------------------------
//	Set the height of the specified rows
//
//	For LTableMonoGeometry, all rows have the same height, so inFromRow
//	and inToRow are ignored.

void
LTableMonoGeometry::SetRowHeight(
	UInt16		inHeight,
	TableIndexT	/* inFromRow */,
	TableIndexT	/* inToRow */)
{
	if (inHeight != mRowHeight) {
		mRowHeight = inHeight;

		SPoint32	theScrollUnit;
		mTableView->GetScrollUnit(theScrollUnit);
		theScrollUnit.v = inHeight;
		mTableView->SetScrollUnit(theScrollUnit);

		mTableView->Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetColWidth
// ---------------------------------------------------------------------------
//	Set the width of the specified columns
//
//	For LTableMonoGeometry, all columns have the same width, so inFromCol
//	and inToCol are ignored.

void
LTableMonoGeometry::SetColWidth(
	UInt16		inWidth,
	TableIndexT	/* inFromCol */,
	TableIndexT	/* inToCol */)
{
	if (inWidth != mColWidth) {
		mColWidth = inWidth;

		SPoint32	theScrollUnit;
		mTableView->GetScrollUnit(theScrollUnit);
		theScrollUnit.h = inWidth;
		mTableView->SetScrollUnit(theScrollUnit);

		mTableView->Refresh();
	}
}


PP_End_Namespace_PowerPlant
