// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDragMsg.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple dragger which creates a task dragging the text of the current selection

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LDragTask.h>
#include <LTableDragMsg.h>
#include <LTextTableView.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LTableDragMsg
// ---------------------------------------------------------------------------
//	Normal Constructor

LTableDragMsg::LTableDragMsg(CommandT inMsg) :
	mDropOKmsg(inMsg)
{}


// ---------------------------------------------------------------------------
//	¥ CreateDragTask
// ---------------------------------------------------------------------------

void	LTableDragMsg::CreateDragTask(LTableView	*fromTable, const SMouseDownEvent& inMouseDown)
{
	// Get the selected cell.
	STableCell	theCell = fromTable->GetFirstSelectedCell();

	if ( fromTable->IsValidCell( theCell ) ) {

		// Get the cell frame.
		Rect	theCellFrame;
		CalcDragRect(fromTable, theCell, theCellFrame);

		// Create the drag task.
		LDragTask	theDragTask( inMouseDown.macEvent, theCellFrame,
						1, kDragMsgFlavor, &mDropOKmsg, sizeof(mDropOKmsg), flavorSenderOnly );
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
