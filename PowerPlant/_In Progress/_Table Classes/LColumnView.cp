// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LColumnView.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for a Table with one column
//
//	ColumnView broadcasts a message when the selection changes and
//	when a cell is double-clicked

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LColumnView.h>
#include <LDragTask.h>
#include <LStream.h>
#include <LTableMonoGeometry.h>
#include <LTableSingleSelector.h>
#include <LTableMultiSelector.h>
#include <LTableArrayStorage.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LColumnView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LColumnView::LColumnView(
	LStream*	inStream)

	: LTableView(inStream),
	  LDragAndDrop(UQDGlobals::GetCurrentWindowPort(), this)
{
	SColumnViewInfo	info;

	*inStream >> info.colWidth;
	*inStream >> info.rowHeight;
	*inStream >> info.useSingleSelector;
	*inStream >> info.useDragSelect;
	*inStream >> info.dataSize;
	*inStream >> info.doubleClickMsg;
	*inStream >> info.selectionMsg;
	InitColumnView(info);
}


// ---------------------------------------------------------------------------
//	¥ InitColumnView						Initializer				 [private]
// ---------------------------------------------------------------------------

void
LColumnView::InitColumnView(
	SColumnViewInfo&	inInfo)
{
	mUseFrameWidth = false;
	if (inInfo.colWidth == 0) {
		mUseFrameWidth = true;
		inInfo.colWidth = (UInt16) mFrameSize.width;
	}

	SetTableGeometry(new LTableMonoGeometry(this, inInfo.colWidth, inInfo.rowHeight));

	if (inInfo.useSingleSelector) {
		SetTableSelector(new LTableSingleSelector(this));
	} else {
		SetTableSelector(new LTableMultiSelector(this));
	}

	SetTableStorage(new LTableArrayStorage(this, inInfo.dataSize));

	mUseDragSelect = inInfo.useDragSelect;

	mDoubleClickMsg = inInfo.doubleClickMsg;
	mSelectionMsg = inInfo.selectionMsg;

	LTableView::InsertCols(1, 0, nil, 0, Refresh_No);


	mDragFlavor = FOUR_CHAR_CODE('TEXT');
	mFlavorFlags = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LColumnView							Destructor				  [public]
// ---------------------------------------------------------------------------

LColumnView::~LColumnView()
{
}


// ---------------------------------------------------------------------------
//	¥ SelectionChanged
// ---------------------------------------------------------------------------
//	Broadcast message when selected cells change

void
LColumnView::SelectionChanged()
{
	if (mSelectionMsg != msg_Nothing) {
		BroadcastMessage(mSelectionMsg, (void*) this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Make column same width as Frame (if that option is on)

void
LColumnView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LTableView::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);

	if (mUseFrameWidth) {
		SetColWidth((UInt16) mFrameSize.width, 1, 1);
	}
}


void
LColumnView::Click(
	SMouseDownEvent	&inMouseDown)
{
	Boolean	saveDelaySelect = inMouseDown.delaySelect;
	inMouseDown.delaySelect = false;
	LTableView::Click(inMouseDown);
	inMouseDown.delaySelect = saveDelaySelect;
}


// ---------------------------------------------------------------------------
//	¥ ClickCell
// ---------------------------------------------------------------------------
//	Broadcast message for a double-click on a cell

void
LColumnView::ClickCell(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
	if (GetClickCount() == 1) {

		if (LDragAndDrop::DragAndDropIsPresent() &&
			::WaitMouseMoved(inMouseDown.macEvent.where)) {

			UInt32	dataSize;
			GetCellData(inCell, nil, dataSize);
			StPointerBlock	buffer((SInt32) dataSize);
			GetCellData(inCell, buffer, dataSize);

			Rect	cellRect;
			GetLocalCellRect(inCell, cellRect);

			FocusDraw();
			LDragTask	theDragTask(inMouseDown.macEvent, cellRect, 1, mDragFlavor,
										buffer, (SInt32) dataSize, mFlavorFlags);

			OutOfFocus(nil);

		}


	} else if (GetClickCount() == 2) {
		if (mDoubleClickMsg != msg_Nothing) {
			BroadcastMessage(mDoubleClickMsg, (void*) this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HiliteDropArea
// ---------------------------------------------------------------------------
//	Hilite a DropArea to indicate that it can accept the current Drag

void
LColumnView::HiliteDropArea(
	DragReference	inDragRef)
{
	mPane->ApplyForeAndBackColors();
	Rect	dropRect;
	mPane->CalcLocalFrameRect(dropRect);
	StRegion	dropRgn(dropRect);
	::ShowDragHilite(inDragRef, dropRgn, true);
}


// ---------------------------------------------------------------------------
//	¥ ItemIsAcceptable
// ---------------------------------------------------------------------------
//	DragManager item is acceptable if it has data of mDragFlavor

Boolean
LColumnView::ItemIsAcceptable(
	DragReference	inDragRef,
	ItemReference	inItemRef)
{
	FlavorFlags		theFlags;
	return (GetFlavorFlags(inDragRef, inItemRef, mDragFlavor, &theFlags)
					== noErr);
}


// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Only one column allowed, so prevent insertion

void
LColumnView::InsertCols(
	UInt32			/* inHowMany */,
	TableIndexT		/* inAfterCol */,
	const void*		/* inDataPtr */,
	UInt32			/* inDataSize	*/,
	Boolean			/* inRefresh */)
{
	SignalStringLiteral_("Inserting columns not allowed");
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Only one column allowed, so prevent removal

void
LColumnView::RemoveCols(
	UInt32			/* inHowMany */,
	TableIndexT		/* inFromCol */,
	Boolean			/* inRefresh */)

{
	SignalStringLiteral_("Removing columns not allowed");
}


PP_End_Namespace_PowerPlant
