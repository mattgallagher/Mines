// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropMsg.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple drop zone which appends to the table and highlights the entire table

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LListener.h>
#include <LTableDropMsg.h>
#include <LTableDragMsg.h>
#include <LTableView.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LTableDropMsg
// ---------------------------------------------------------------------------
//	Normal Constructor

LTableDropMsg::LTableDropMsg(LTableView* target, CommandT dropMsg, LListener* tellDropped) :
	LTableDropZone(target, LTableDragMsg::kDragMsgFlavor),
	mDropOKmsg(dropMsg),
	mTellDropped(tellDropped)
{}




// ---------------------------------------------------------------------------------
//		¥ ItemIsAcceptable
// ---------------------------------------------------------------------------------
// only acceptable if comes from same app & is correct message

Boolean
LTableDropMsg::ItemIsAcceptable(
	DragReference	inDragRef,
	ItemReference	inItemRef )
{
	Boolean isAcceptable = false;

	// Make sure the table is enabled and
	// there's text in the drag data.
	if( mTargetTable->IsEnabled()) {
		FlavorFlags	theFlags;
		OSErr result = ::GetFlavorFlags( inDragRef, inItemRef, mSingleFlavor, &theFlags );

		if ((result==noErr) && (theFlags & flavorSenderOnly))
		{	// OK, so it's a command from this app, is it the command this pane wants?
			CommandT	draggedMsg;
			long		expectedLen = sizeof(draggedMsg);
			GetFlavorData(inDragRef, inItemRef, mSingleFlavor, &draggedMsg, &expectedLen, 0L);
			isAcceptable = (draggedMsg == mDropOKmsg);
		}
	}
	return isAcceptable;
}


// ---------------------------------------------------------------------------------
//		¥ ReceiveDragItem
// ---------------------------------------------------------------------------------

void
LTableDropMsg::ReceiveDragItem(
	DragReference	/* inDragRef */,
	DragAttributes	/* inDragAttrs */,
	ItemReference	/* inItemRef */,
	Rect&			/* inItemBounds */)
{
// the only thing that is acceptable is a flavour carrying the command we
// expected, thus we don't need to get any data from the drag
	mTellDropped->ListenToMessage(mDropOKmsg, 0L);
}


PP_End_Namespace_PowerPlant
