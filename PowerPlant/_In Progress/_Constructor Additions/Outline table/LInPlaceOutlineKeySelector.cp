// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceOutlineKeySelector.cp PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LInPlaceOutlineKeySelector.h>

#include <LEditableOutlineItem.h>
#include <LInPlaceEditField.h>
#include <LOutlineTable.h>
#include <LStream.h>
#include <PP_KeyCodes.h>

PP_Begin_Namespace_PowerPlant

const UInt16	mask_NonShiftModifiers	= cmdKey + optionKey + controlKey;


// ---------------------------------------------------------------------------
//	¥ LInPlaceOutlineKeySelector			Stream Constructor
// ---------------------------------------------------------------------------

LInPlaceOutlineKeySelector::LInPlaceOutlineKeySelector(
	LStream*	inStream)

	: LOutlineKeySelector(inStream)
{
	*inStream >> mAcceptReturnKey;
}


// ---------------------------------------------------------------------------
//	¥ LInPlaceOutlineKeySelector			Parameterized Constructor
// ---------------------------------------------------------------------------
//	Constructor, specifying the table whose selection will be affected
//	by this attachment.

LInPlaceOutlineKeySelector::LInPlaceOutlineKeySelector(
	LOutlineTable*		inOutlineTable,
	MessageT			inMessage,
	Boolean				inAcceptReturnKey)

	: LOutlineKeySelector(inOutlineTable, inMessage)
{
	mAcceptReturnKey = inAcceptReturnKey;
}


// ---------------------------------------------------------------------------
//	¥ ~LInPlaceOutlineKeySelector			Destructor
// ---------------------------------------------------------------------------

LInPlaceOutlineKeySelector::~LInPlaceOutlineKeySelector()
{
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyEvent											   [protected]
// ---------------------------------------------------------------------------
//	Catch the Tab, Enter, and Return keys. These are used to move the
//	in-place edit field from one cell to another.

void
LInPlaceOutlineKeySelector::HandleKeyEvent(
	const EventRecord*	inEvent)
{
		// Sanity check: Make sure we point to a valid outline table.

	if (mOutlineTable == nil) {
		return;
	}

		// Decode the key-down message.

	SInt16 theKey = (SInt16) (inEvent->message & charCodeMask);
	if ((inEvent->modifiers & mask_NonShiftModifiers) != 0) {
		theKey = 0;
	}

	switch (theKey) {

		case char_Tab:
			RotateTarget((inEvent->modifiers & shiftKey) != 0);
			SetExecuteHost(false);
			break;

		case char_Enter:
		case char_Return:
			if (mAcceptReturnKey) {
				RotateTarget(false);
				SetExecuteHost(false);
				break;
			}

			// Fall thru if we don't accept return key

		default:
			LOutlineKeySelector::HandleKeyEvent(inEvent);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ RotateTarget												   [protected]
// ---------------------------------------------------------------------------
//	Called by HandleKeyEvent when the in-place edit field should be moved
//	to another cell.

void
LInPlaceOutlineKeySelector::RotateTarget(
	Boolean		inBackward)
{

	// Theory of operation. We look for a cell that serves as a starting
	// point for the "tab" operation. This can be the cell currently being
	// edited, the first cell in the selection list, or the top of the table.
	// From that point, we then advance forward or backward through the
	// cells until we find one that can do in-place editing.

	STableCell	seedCell;
	bool	 	includeSeed = true;
	bool		foundSeed = false;

	// First, see if we're already doing in-place editing.

	mOutlineTable->GetCurrentInPlaceEditCell(seedCell);
	if (seedCell.row > 0) {

		// A cell is being edited. Cancel its editing and use it as the seed.

		LEditableOutlineItem* item = (dynamic_cast<LEditableOutlineItem*>
										(mOutlineTable->FindItemForRow(seedCell.row)));
		if (item != nil) {
			LInPlaceEditField* editField = item->GetEditField();
			if (editField != nil)
				editField->StopEditing();
		}

		foundSeed = true;
		includeSeed = false;
	}

	// If nothing is already being edited, look for first selected cell.

	if (!foundSeed) {
		seedCell.SetCell(0, 0);

		if (mOutlineTable->GetNextSelectedCell(seedCell)) {
			foundSeed = true;
			includeSeed = !inBackward;
		}
	}

	// If nothing is selected, start from top of table.

	if (!foundSeed) {
		seedCell.SetCell(0, 0);

		if (mOutlineTable->GetNextCell(seedCell)) {
			foundSeed = true;
			includeSeed = !inBackward;
		}
	}

	// If nothing in table, bail out now.

	if (!foundSeed)
		return;


	// Okay, we've established the "seed" cell.
	// Now walk through the cells until we find one that can
	// do in-place editing.

	TableIndexT rows, cols;
	mOutlineTable->GetTableSize(rows, cols);

	STableCell cell = seedCell;

	do {

		// Try a row. Iterate over the cells in the row.

		LOutlineItem* rowItem = mOutlineTable->FindItemForRow(cell.row);
		if (rowItem != nil) {

			do {

				// Try a cell. See if it can do in-place editing.

				if (rowItem->CanDoInPlaceEdit(cell)) {
					if (includeSeed || (cell != seedCell)) {

						// It can, and it's not the seed cell. Do it.

						LEditableOutlineItem* editItem =
								(dynamic_cast<LEditableOutlineItem*>(rowItem));

						if (editItem != nil) {
							mOutlineTable->UnselectAllCells();
							mOutlineTable->SelectCell(cell);
							editItem->StartInPlaceEdit(cell);
							return;
						}
					}
				}

				// Can't do IPE in this cell. Try the next one.

				if (inBackward)
					cell.col--;
				else
					cell.col++;

			} while ((cell.col > 0) && (cell.col <= cols));
		}

		// Can't do IPE in this row. Try the next one.

		if (inBackward) {
			cell.col = cols;
			cell.row--;
			if (cell.row == 0)
				cell.row = rows;
		}
		else {
			cell.col = 1;
			cell.row++;
			if (cell.row > rows)
				cell.row = 1;
		}

	} while (cell.row != seedCell.row);
}


PP_End_Namespace_PowerPlant
