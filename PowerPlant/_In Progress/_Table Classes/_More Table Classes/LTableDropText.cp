// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropText.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	A simple drop zone which appends to the table and highlights the
//	entire table

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableDropText.h>
#include <LTableView.h>
#include <PP_Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableDropText						Constructor				  [public]
// ---------------------------------------------------------------------------

LTableDropText::LTableDropText(
	LTableView*		inTable)

	: LTableDropZone(inTable, ResType_Text)
{
}


// ---------------------------------------------------------------------------
//	¥ LTableDropText						Destructor				  [public]
// ---------------------------------------------------------------------------

LTableDropText::~LTableDropText()
{
}


// ---------------------------------------------------------------------------
//	¥ ReceiveDragItem												  [public]
// ---------------------------------------------------------------------------

void
LTableDropText::ReceiveDragItem(
	DragReference	inDragRef,
	DragAttributes	/* inDragAttrs */,
	ItemReference	inItemRef,
	Rect&			/* inItemBounds */)
{
	FlavorFlags	theFlags;
	ThrowIfOSErr_( ::GetFlavorFlags( inDragRef,
		inItemRef, mSingleFlavor, &theFlags ) );

	// Get the data.
	Size	theDataSize = 255;
	Str255	theString;
	ThrowIfOSErr_( ::GetFlavorData( inDragRef, inItemRef,
		mSingleFlavor, &theString[1], &theDataSize, 0 ) );

	// Get the data size and set the string length.
	ThrowIfOSErr_( ::GetFlavorDataSize( inDragRef,
		inItemRef, mSingleFlavor, &theDataSize ) );
	theString[0] = (UInt8) theDataSize;
/*
	// if it's a move operation (in sender and no option key)
	if ( UDragAndDropUtils::CheckIfViewIsAlsoSender( inDragRef ) &&
		!UDragAndDropUtils::CheckForOptionKey( inDragRef ) ) {

		// Get the selected cell.
		TableCellT	theSelectedCell;
		GetSelectedCell( theSelectedCell );

		if ( mDropRow != theSelectedCell.row ) {

			// Delete the old data.
			if ( IsValidCell( theSelectedCell ) ) {

				// Delete the original cell.
				RemoveRows( 1, theSelectedCell.row );

			}

			// Add the new data.
			TableIndexT	theRow;
			if ( mDropRow == -1 ) {
				theRow = arrayIndex_Last;
			} else {
				theRow = mDropRow;
				if ( theRow > theSelectedCell.row ) {
					// Adjust for deleted row.
					theRow -= 1;
				}
			}
			InsertRows( 1, theRow, theString );

			// Select the new cell, but without calling
			// SelectCell to avoid immediate drawing.
			mSelectedCell.row = theRow + 1;

		}

	} else
*/
	{ // it's a copy operation

		// Add the new data.
	// Get the selected cell.
	STableCell	theCell = mTargetTable->GetFirstSelectedCell();

		TableIndexT	theRow = theCell.row+1;
		mTargetTable->InsertRows( 1, theRow, &theString[1], (UInt32) theDataSize, true );
/*
		// Select the new cell, but without calling
		// SelectCell to avoid immediate drawing.
		mSelectedCell.row = theRow + 1;
*/
	}
}


PP_End_Namespace_PowerPlant
