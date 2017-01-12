// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDragValue.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple dragger which creates a task dragging the text of the
//	current selection

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LDragTask.h>
#include <LTableDragValue.h>
#include <LTextTableView.h>
//#include <UDragAndDropUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableDragValue						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTableDragValue::LTableDragValue()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTableDragValue						Destructor				  [public]
// ---------------------------------------------------------------------------

LTableDragValue::~LTableDragValue()
{
}


// ---------------------------------------------------------------------------
//	¥ CreateDragTask												  [public]
// ---------------------------------------------------------------------------

void
LTableDragValue::CreateDragTask(
	LTableView*				fromTable,
	const SMouseDownEvent&	inMouseDown)
{
	// Get the selected cell.
	STableCell	theCell = fromTable->GetFirstSelectedCell();

	if ( fromTable->IsValidCell( theCell ) ) {

		// Get the cell frame.
		Rect	theCellFrame;
		CalcDragRect(fromTable, theCell, theCellFrame);

		// Create the drag task.
		TableIndexT value = theCell.row - 1;	// keep 0-based like LListBox
		LDragTask	theDragTask( inMouseDown.macEvent, theCellFrame,
						1, FOUR_CHAR_CODE('DATA'), &value, sizeof(value), 0 );
/*
		// Check for a drop in the trash.
		if ( UDragAndDropUtils::DroppedInTrash(
			theDragTask.GetDragReference() ) ) {

			// Delete the cell and refresh the list.
			fromTable->RemoveRows( 1, theCell.row );
			fromTable->Refresh();

		}
*/	}
}


PP_End_Namespace_PowerPlant
