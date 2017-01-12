// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDragText.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	A simple dragger which creates a task dragging the text of the
//	current selection

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableDragText.h>
#include <LDragTask.h>
#include <LTextTableView.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableDragText						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTableDragText::LTableDragText()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTableDragText						Destructor				  [public]
// ---------------------------------------------------------------------------

LTableDragText::~LTableDragText()
{
}


// ---------------------------------------------------------------------------
//	¥ CreateDragTask												  [public]
// ---------------------------------------------------------------------------

void
LTableDragText::CreateDragTask(
	LTableView*				fromTable,
	const SMouseDownEvent&	inMouseDown)
{
	// Get the selected cell.
	STableCell	theCell = fromTable->GetFirstSelectedCell();

	if ( fromTable->IsValidCell( theCell ) ) {

		// Get the cell frame.
		Rect	theCellFrame;
		CalcDragRect(fromTable, theCell, theCellFrame);

		// Get the cell data.
		Str255	theString;
		UInt32	theStrLen = 255;
		fromTable->GetCellData(theCell, &theString[1], theStrLen);
		theString[0] = (UInt8) theStrLen;

		// Create the drag task.
		LDragTask	theDragTask( inMouseDown.macEvent, theCellFrame,
						1, FOUR_CHAR_CODE('TEXT'), &theString[1], theString[0], 0 );

/*
		// Check for a drop in the trash.
		if ( UDragAndDropUtils::DroppedInTrash(
			theDragTask.GetDragReference() ) ) {

			// Delete the cell and refresh the list.
			RemoveRows( 1, theCell.row );
			Refresh();

		}
*/
	}
}


PP_End_Namespace_PowerPlant
