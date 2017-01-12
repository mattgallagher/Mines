// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextHierTable.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTextHierTable.h>
#include <LCollapsableTree.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTextHierTable						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTextHierTable::LTextHierTable(
	LStream*	inStream)
	
	: LHierarchyTable(inStream)
{
	mFirstIndent		= 24;
	mLevelIndent		= 28;
	mLeafTextTraits		= 130;
	mParentTextTraits	= 131;
}


// ---------------------------------------------------------------------------
//	¥ ~LTextHierTable						Destructor				  [public]
// ---------------------------------------------------------------------------

LTextHierTable::~LTextHierTable()
{
}


// ---------------------------------------------------------------------------
//	¥ DrawCell													   [protected]
// ---------------------------------------------------------------------------

void
LTextHierTable::DrawCell(
	const STableCell&	inCell,
	const Rect&			inLocalRect)
{
	TableIndexT	woRow = mCollapsableTree->GetWideOpenIndex(inCell.row);

	DrawDropFlag(inCell, woRow);

	STableCell	woCell(woRow, inCell.col);
	Str255		theStr;
	UInt32		dataSize = sizeof(theStr);
	GetCellData(woCell, theStr, dataSize);

	ResIDT	textTraitsID = mLeafTextTraits;
	if (mCollapsableTree->IsCollapsable(woRow)) {
		textTraitsID = mParentTextTraits;
	}
	UTextTraits::SetPortTextTraits(textTraitsID);

	UInt32	nestingLevel = mCollapsableTree->GetNestingLevel(woRow);

	::MoveTo((SInt16) (inLocalRect.left + mFirstIndent + nestingLevel * mLevelIndent),
			 (SInt16) (inLocalRect.bottom - 4));
	::DrawString(theStr);
}


// ---------------------------------------------------------------------------
//	¥ HiliteCellActively										   [protected]
// ---------------------------------------------------------------------------

void
LTextHierTable::HiliteCellActively(
	const STableCell&	inCell,
	Boolean				/* inHilite */)
{
	Rect	cellFrame;
	if (GetLocalCellRect(inCell, cellFrame) && FocusExposed()) {
		UDrawingUtils::SetHiliteModeOn();
		cellFrame.left += (SInt16) mFirstIndent;
		::MacInvertRect(&cellFrame);
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteCellInactively										   [protected]
// ---------------------------------------------------------------------------

void
LTextHierTable::HiliteCellInactively(
	const STableCell&	inCell,
	Boolean				/* inHilite */)
{
	Rect	cellFrame;
	if (GetLocalCellRect(inCell, cellFrame) && FocusExposed()) {
		cellFrame.left += (SInt16) mFirstIndent;
		UDrawingUtils::SetHiliteModeOn();
		::PenNormal();
		::PenMode(srcXor);
		::MacFrameRect(&cellFrame);
	}
}


PP_End_Namespace_PowerPlant

