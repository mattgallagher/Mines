// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableEditableSelector.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTableEditableSelector.h>
#include <LTextTableView.h>
#include <LCellEditor.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableEditableSelector
// ---------------------------------------------------------------------------
//	Normal Constructor
// ---------------------------------------------------------------------------
LTableEditableSelector::LTableEditableSelector(LTableSelector* inRealSelector) :
	LTableSelector(nil),
	mRealSelector(inRealSelector),
	mEditors(sizeof(LCellEditor*))
{}


// ---------------------------------------------------------------------------
//	Destructor
// ---------------------------------------------------------------------------

LTableEditableSelector::~LTableEditableSelector()
{
	delete mRealSelector;
}


// ---------------------------------------------------------------------------
//	ClickSelect
// ---------------------------------------------------------------------------

void
LTableEditableSelector::ClickSelect(
	const STableCell		&inCell,
	const SMouseDownEvent	&inMouseDown)
{
// NOTE LCellEditor is not finished, so we just forward

/*	LCellEditor* theEditor;
	if ( (inMouseDown.macEvent.modifiers & shiftKey) || (inMouseDown.macEvent.modifiers & cmdKey) ) {
		theEditor = 0;  // assume extending selection
	}
	else {
// TO DO - allow user to specify that option or control clicks trigger edits
		theEditor = GetEditor(inCell);	// later may attach editor to a single cell, or row, or col
	}
	if (theEditor)  {
		UnselectAllCells();	// ????? This seems reasonable - the visible edit will do its own hilite, and we expect it to talk to us when finished
		theEditor->StartEditing();	// Note that this leaves an editor free to implement a wait until visible edit
	}
	else
*/
		mRealSelector->ClickSelect(inCell, inMouseDown);
}


// ---------------------------------------------------------------------------
//	Set and GetEditor
// ---------------------------------------------------------------------------
void
LTableEditableSelector::AttachEditorToCol(
	LCellEditor* adoptedEditor,
	TableIndexT	inCol)
{
	mEditors.InsertItemsAt(1, (ArrayIndexT) inCol-1,
							adoptedEditor, sizeof(LCellEditor*));
}


LCellEditor*
LTableEditableSelector::GetEditor(
	const STableCell		&inCell) const
{
// LATER - have editors attached to rows and specific cells
// so this will become much more complicated, with 2D RunArrays
	LCellEditor*	ret;
	mEditors.FetchItemAt((ArrayIndexT) inCell.col, &ret);
	return ret;
}


// ---------------------------------------------------------------------------
//	AFFECT OUR EDITORS THEN FORWARD TO REAL SELECTOR
// ---------------------------------------------------------------------------

void
LTableEditableSelector::InsertRows(
		UInt32					inHowMany,
		TableIndexT				inAfterRow)
{
// we don't yet attach editors just to a cell or row so no action
	mRealSelector->InsertRows(inHowMany, inAfterRow);
}


void
LTableEditableSelector::InsertCols(
	UInt32					inHowMany,
	TableIndexT				inAfterCol)
{
	LCellEditor* nilEditor=0;
	mEditors.InsertItemsAt(inHowMany, (ArrayIndexT) inAfterCol,
							nilEditor, sizeof(LCellEditor*));
	mRealSelector->InsertCols(inHowMany, inAfterCol);
}


void
LTableEditableSelector::RemoveRows(
	UInt32					inHowMany,
	TableIndexT				inFromRow)
{
// we don't yet attach editors just to a cell or row so no action
	mRealSelector->RemoveRows(inHowMany, inFromRow);
}


void
LTableEditableSelector::RemoveCols(
	UInt32					inHowMany,
	TableIndexT				inFromCol)
{
// NOT YET IMPLEMENTED - need to delete any editors
	mRealSelector->RemoveCols(inHowMany, inFromCol);
}



// ---------------------------------------------------------------------------
//	FORWARD TO REAL SELECTOR
// ---------------------------------------------------------------------------

Boolean
LTableEditableSelector::CellIsSelected(
	const STableCell		&inCell) const
{
	return mRealSelector->CellIsSelected(inCell);
}


STableCell
LTableEditableSelector::GetFirstSelectedCell() const
{
	return mRealSelector->GetFirstSelectedCell();
}


TableIndexT
LTableEditableSelector::GetFirstSelectedRow() const
{
	return mRealSelector->GetFirstSelectedRow();
}



void
LTableEditableSelector::SelectCell(
	const STableCell		&inCell)
{
	mRealSelector->SelectCell(inCell);
}


void
LTableEditableSelector::SelectAllCells()
{
	mRealSelector->SelectAllCells();
}


void
LTableEditableSelector::UnselectCell(
	const STableCell		&inCell)
{
	mRealSelector->UnselectCell(inCell);
}


void
LTableEditableSelector::UnselectAllCells()
{
	mRealSelector->UnselectAllCells();
}


Boolean
LTableEditableSelector::DragSelect(
	const STableCell		&inCell,
	const SMouseDownEvent	&inMouseDown)

{
	return mRealSelector->DragSelect(inCell,inMouseDown);
}

PP_End_Namespace_PowerPlant
