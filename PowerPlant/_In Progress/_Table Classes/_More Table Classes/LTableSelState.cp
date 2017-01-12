// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableSelState.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent


//	Family of lightweight classes to save the internal state of LTableSelector subclasses

#include <LTableSelState.h>
#include <LArray.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableSingleRowState
// ---------------------------------------------------------------------------
// stores a single row number

LTableSingleRowState::LTableSingleRowState(TableIndexT inRow) :
	mRow(inRow)
{
}


LTableSingleRowState::~LTableSingleRowState()
{
}


TableIndexT
LTableSingleRowState::GetRow() const
{
	return mRow;
}


void
LTableSingleRowState::SetRow(TableIndexT inRow)
{
	mRow = inRow;
}


// ---------------------------------------------------------------------------
//	¥ LTableSingleRowState
// ---------------------------------------------------------------------------
// stores a single row number

LTableMultiRowState::LTableMultiRowState(const STableCell& inCell, const RgnHandle inRgn) :
	mAnchorCell(inCell)
{
	mSelectionRgn = ::NewRgn();
	::MacCopyRgn(inRgn, mSelectionRgn);
}


LTableMultiRowState::~LTableMultiRowState()
{
	::DisposeRgn(mSelectionRgn);
}


const STableCell&
LTableMultiRowState::GetAnchorCell() const
{
	return mAnchorCell;
}


RgnHandle
LTableMultiRowState::GetRegion() const
{
	return mSelectionRgn;
}


// ---------------------------------------------------------------------------
//	¥ LOutlineSelState
// ---------------------------------------------------------------------------
// stores an array of expanded offsets, as well as wrapping an abstract
// highlighted state (one of its siblings).
// The expanded offsets are all measured from the top - just take a list and
// go down asking each visible item if is expanded

LOutlineSelState::LOutlineSelState(LTableSelState* inSel,LArray* inState) :
	mWrappedSel(inSel),
	mExpandState(inState)
{
}


LOutlineSelState::~LOutlineSelState()
{
	delete mWrappedSel;
	delete mExpandState;
}


LArray*
LOutlineSelState::GetExpandState() const
{
	return mExpandState;
}


void
LOutlineSelState::SetExpandState(LArray* inExpandState)
{
	Assert_(mExpandState==0);
	mExpandState = inExpandState;
}


LTableSelState*
LOutlineSelState::GetSavedSelection() const
{
	return mWrappedSel;
}


void
LOutlineSelState::SetSavedSelection(LTableSelState* inState)
{
	Assert_(mWrappedSel==0);
	mWrappedSel = inState;
}

PP_End_Namespace_PowerPlant
