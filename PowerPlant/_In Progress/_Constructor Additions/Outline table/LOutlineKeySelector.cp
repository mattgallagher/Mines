// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineKeySelector.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOutlineKeySelector.h>
#include <LOutlineItem.h>
#include <LOutlineTable.h>

#include <LCommander.h>
#include <PP_KeyCodes.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LOutlineKeySelector(LOutlineTable*, MessageT)
// ---------------------------------------------------------------------------
//	Constructor, specifying the outline table to be selected and which
//	attachment messages to listen to. LOutlineKeySelector can safely accept
//	all message codes (which is the default).

LOutlineKeySelector::LOutlineKeySelector(
	LOutlineTable*	inOutlineTable,
	MessageT		inMessage)

: LAttachment(inMessage)
{
	mOutlineTable = inOutlineTable;
}


// ---------------------------------------------------------------------------
//	¥ LOutlineKeySelector(LStream*)
// ---------------------------------------------------------------------------
//	Stream constructor. No extra data members are defined.

LOutlineKeySelector::LOutlineKeySelector(
	LStream*	inStream)

: LAttachment(inStream)
{
	mOutlineTable = (dynamic_cast<LOutlineTable*>
						(LAttachable::GetDefaultAttachable()));
}


// ---------------------------------------------------------------------------
//	¥ ~LOutlineKeySelector
// ---------------------------------------------------------------------------
//	Destructor

LOutlineKeySelector::~LOutlineKeySelector()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ExecuteSelf											[protected]
// ---------------------------------------------------------------------------
//	Decode the message and dispatch it.

void
LOutlineKeySelector::ExecuteSelf(
	MessageT		inMessage,
	void*			ioParam)
{
	SetExecuteHost(true);

	if (mOutlineTable != nil) {

		switch (inMessage) {

			case msg_CommandStatus:
				FindCommandStatus((SCommandStatus*) ioParam);
				break;

			case cmd_SelectAll:
				SelectAll();
				break;

			case msg_KeyPress:
				HandleKeyEvent((EventRecord*) ioParam);
				break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus										[protected]
// ---------------------------------------------------------------------------
//	Always enable the Select All menu command.

void
LOutlineKeySelector::FindCommandStatus(
	SCommandStatus*	inCommandStatus)
{
	if (inCommandStatus->command == cmd_SelectAll) {
		*(inCommandStatus->enabled) = true;
		SetExecuteHost(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyEvent										[protected]
// ---------------------------------------------------------------------------
//	Recognize and dispatch the arrow keys.

void
LOutlineKeySelector::HandleKeyEvent(
	const EventRecord* inEvent)
{
	// Decode the key-down message.

	SInt16 theKey = (SInt16) (inEvent->message & charCodeMask);

	switch (theKey) {

		case char_LeftArrow:
			CollapseSelection();
			SetExecuteHost(false);
			break;

		case char_RightArrow:
			if (inEvent->modifiers & optionKey) {
				DeepExpandSelection();
			} else {
				ExpandSelection();
			}
			SetExecuteHost(false);
			break;

		case char_UpArrow:
			UpArrow((inEvent->modifiers & shiftKey) != 0);
			SetExecuteHost(false);
			break;

		case char_DownArrow:
			DownArrow((inEvent->modifiers & shiftKey) != 0);
			SetExecuteHost(false);
			break;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SelectAll												[protected]
// ---------------------------------------------------------------------------
//	The Select All menu item has been chosen. Select all cells in the
//	outline.

void
LOutlineKeySelector::SelectAll()
{
	mOutlineTable->SelectAllCells();
	SetExecuteHost(false);
}


// ---------------------------------------------------------------------------
//	¥ UpArrow												[protected]
// ---------------------------------------------------------------------------
//	Select the next cell above the current cell. If no cell is selected,
//	select the bottom left cell. If inExtendSelection is true, add this cell
//	to the existing selection.

void
LOutlineKeySelector::UpArrow(
	Boolean inExtendSelection)
{

	// Find first selected cell.

	STableCell theCell;
	if (mOutlineTable->GetNextSelectedCell(theCell)) {

		// Found a selected cell.
		// If not in the first row, move up one and select.

		if (theCell.row > 1) {
			theCell.row--;
			if (!inExtendSelection)
				mOutlineTable->UnselectAllCells();
			mOutlineTable->SelectCell(theCell);
			ScrollRowIntoFrame(theCell.row);
		}
	}
	else {

		// Nothing selected. Start from bottom.

		TableIndexT rows, cols;
		mOutlineTable->GetTableSize(rows, cols);

		if (rows > 0) {
			theCell.row = rows;
			theCell.col = 1;
			mOutlineTable->UnselectAllCells();
			mOutlineTable->SelectCell(theCell);
			ScrollRowIntoFrame(theCell.row);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DownArrow												[protected]
// ---------------------------------------------------------------------------
//	Select the next cell below the current cell. If no cell is selected,
//	select the top left cell. If inExtendSelection is true, add this cell
//	to the existing selection.

void
LOutlineKeySelector::DownArrow(
	Boolean		inExtendSelection)
{

	// Find first selected cell.

	TableIndexT rows, cols;
	STableCell theCell;
	if (!mOutlineTable->GetNextSelectedCell(theCell)) {

		// Nothing selected. Start from top.

		mOutlineTable->GetTableSize(rows, cols);
		if (rows > 0) {
			theCell.row = 1;
			theCell.col = 1;
			mOutlineTable->UnselectAllCells();
			mOutlineTable->SelectCell(theCell);
			ScrollRowIntoFrame(theCell.row);
		}

	}
	else {

		// Found a selected cell. Look for last selected cell.

		STableCell lastCell = theCell;
		while (mOutlineTable->GetNextSelectedCell(theCell)) {
			lastCell = theCell;
		}

		// Found last selected cell.
		// If not in the last row, move down one and select.

		mOutlineTable->GetTableSize(rows, cols);

		if (lastCell.row < rows) {
			lastCell.row++;
			if (!inExtendSelection)
				mOutlineTable->UnselectAllCells();
			mOutlineTable->SelectCell(lastCell);
			ScrollRowIntoFrame(lastCell.row);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CollapseSelection										[protected]
// ---------------------------------------------------------------------------
//	Collapse all selected cells.

void
LOutlineKeySelector::CollapseSelection()
{
	STableCell theCell;
	while (mOutlineTable->GetNextSelectedCell(theCell)) {
		LOutlineItem* item = mOutlineTable->FindItemForRow(theCell.row);
		if (item != nil)
			item->Collapse();
	}
}


// ---------------------------------------------------------------------------
//	¥ ExpandSelection										[protected]
// ---------------------------------------------------------------------------
//	Expand all selected cells.

void
LOutlineKeySelector::ExpandSelection()
{
	STableCell theCell;
	while (mOutlineTable->GetNextSelectedCell(theCell)) {
		LOutlineItem* item = mOutlineTable->FindItemForRow(theCell.row);
		if (item != nil)
			item->Expand();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeepExpandSelection									[protected]
// ---------------------------------------------------------------------------
//	Deep expand all selected cells.

void
LOutlineKeySelector::DeepExpandSelection()
{
	STableCell theCell;
	while (mOutlineTable->GetNextSelectedCell(theCell)) {
		LOutlineItem* item = mOutlineTable->FindItemForRow(theCell.row);
		if (item != nil)
			item->DeepExpand();
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollRowIntoFrame
// ---------------------------------------------------------------------------

void
LOutlineKeySelector::ScrollRowIntoFrame(
	TableIndexT		inRow)
{
	mOutlineTable->UpdatePort();		// In case any unhilited cells need
										//   to redraw

	// Find out how many columns are in the table.

	TableIndexT rows, cols;
	mOutlineTable->GetTableSize(rows, cols);

	// Find the first cell where a selection is drawn.

	LOutlineItem* item = mOutlineTable->FindItemForRow(inRow);
	ThrowIfNil_(item);

	STableCell cell;
	cell.row = inRow;
	cell.col = 1;

	while (cell.col <= cols) {

		if (item->ShowsSelection(cell)) {
			mOutlineTable->ScrollCellIntoFrame(cell);
			return;
		}

		cell.col++;
	}

	// No selection shown anywhere, just choose the first cell in the row.

	cell.col = 1;
	mOutlineTable->ScrollCellIntoFrame(cell);

}

PP_End_Namespace_PowerPlant

