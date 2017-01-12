// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextTableView.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	Simple subclass of LTableView that just draws strings
//
//	NOTE *Requires* use of an LCellSizer helper object, eg:
//	mMainList->SetCellSizer(new LCellSizeToFit(false));  // don't truncate to words

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTextTableView.h>
#include <LPane.h>
#include <LDragAndDrop.h>
#include <LFocusBox.h>
#include <UDrawingState.h>
#include <URegistrar.h>
#include <UTableHelpers.h>
#include <UTextTableHelpers.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <LStream.h>
#include <UKeyFilters.h>
#include <UTextTraits.h>
#include <UTBAccessors.h>
#include <LTableSelState.h>

// include these selectors only because of downcasts in LTextTableView::GetSavedSelection
#include <LTableSingleRowSelector.h>
#include <LTableMultiRowSelector.h>

#include <Drag.h>
#include <limits.h>

PP_Begin_Namespace_PowerPlant

// COPIED FROM LVIEW.CP
const SInt16		max_PortOrigin = 16384;
const SInt16		min_PortOrigin = -32768;
const SInt32		mask_Lo14Bits = 0x00003FFF;

#pragma mark --- Construction/Destruction ---

// ---------------------------------------------------------------------------
//	¥ RegisterClass [static]
// ---------------------------------------------------------------------------
//	Shortcut to simplify the RegisterClass line in your App

void
LTextTableView::RegisterClass ()
{
	URegistrar::RegisterClass (class_ID, (ClassCreatorFunc)CreateFromStream);
}


// ---------------------------------------------------------------------------
//	¥ CreateFromStream [static]
// ---------------------------------------------------------------------------
//	Return a new TextTableView object initialized using data from a Stream

LTextTableView*
LTextTableView::CreateFromStream(
	LStream	*inStream)
{
	return (new LTextTableView(inStream));
}


// ---------------------------------------------------------------------------
//	¥ LTextTableView
// ---------------------------------------------------------------------------
//	Default Constructor

LTextTableView::LTextTableView() :
	LNavigableTable(this),
	mCellSizer(0),
	mDragger(0),
	mNumFrozenRows(0),
	mFrozenRowUnderlineThickness(1),
	mFrozenRowsSelectable(false)
{
	InitTextTable(msg_Nothing, msg_Nothing, 255, 0);	// default to 255 chars in buffer
}


// ---------------------------------------------------------------------------
//	¥ LTextTableView
// ---------------------------------------------------------------------------
//	Construct from input parameters

LTextTableView::LTextTableView(
	const SPaneInfo	&inPaneInfo,
	const SViewInfo	&inViewInfo,
	MessageT		inSingleClickMsg,
	MessageT		inDoubleClickMsg,
	UInt32			inBufferSize,
	SInt16 			inTextTraitsID) :

	LTableView(inPaneInfo,inViewInfo),
	LNavigableTable(this),
	mCellSizer(0),
	mDragger(0),
	mNumFrozenRows(0),
	mFrozenRowUnderlineThickness(1),
	mFrozenRowsSelectable(false)
{
	InitTextTable(inSingleClickMsg, inDoubleClickMsg, inBufferSize, inTextTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ LTextTableView(LStream*)
// ---------------------------------------------------------------------------
//	Construct from the data in a Stream

LTextTableView::LTextTableView(LStream	*inStream) :
	LTableView(inStream),
	LNavigableTable(this),
	mCellSizer(0),
	mDragger(0),
	mNumFrozenRows(0),
	mFrozenRowUnderlineThickness(1),
	mFrozenRowsSelectable(false)
{
	STextTableViewInfo	tableInfo;
	*inStream >> tableInfo.doubleClickMessage;
	*inStream >> tableInfo.bufferSize;
	*inStream >> tableInfo.textTraitsID;
	*inStream >> tableInfo.singleClickMessage;

	InitTextTable(tableInfo.singleClickMessage, tableInfo.doubleClickMessage, tableInfo.bufferSize, tableInfo.textTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ InitTextTable
// ---------------------------------------------------------------------------
//	Private initializer

void
LTextTableView::InitTextTable(	MessageT				inSingleClickMsg,
								MessageT				inDoubleClickMsg,
								UInt32					inBufferSize,
								SInt16 					inTextTraitsID)
{
	mSingleClickMessage = inSingleClickMsg;
	mDoubleClickMessage = inDoubleClickMsg;
	mCellBufferSize = inBufferSize;
	
	if (mCellBufferSize == 0) {		// too many AppMaker tables generating default 0
		mCellBufferSize = 255;
	}

	mCellBuffer = new char[mCellBufferSize];
	mTextTraitsID = inTextTraitsID;

// later will be governed by setting like LListBox
	if (mSuperView != nil) {
		mFocusBox = new LFocusBox;
		mFocusBox->Hide();
		mFocusBox->AttachPane(mSuperView, false); // want focus around our containing scroller
	}

		// Table wants to be on duty when its SuperCommander
		// is put on duty. but may not be eg: if a tab group is on the same window
		// and created after us

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LTableView
// ---------------------------------------------------------------------------
//	Destructor

LTextTableView::~LTextTableView()
{
	delete mCellSizer;
	delete[] mCellBuffer;
	delete mDragger;
}


// ---------------------------------------------------------------------------
//	¥ GetNumFrozenRows & SetNumFrozenRows
//		¥ GetFrozenRowsSelectable & SetFrozenRowsSelectable
// ---------------------------------------------------------------------------
//	Accessors for variables that control frozen row behaviours

UInt16
LTextTableView::GetNumFrozenRows() const
{
	return mNumFrozenRows;
}


void
LTextTableView::SetNumFrozenRows(UInt16 inNumRows)
{
	mNumFrozenRows = inNumRows;
}



Boolean
LTextTableView::GetFrozenRowsSelectable() const
{
	return mFrozenRowsSelectable;
}


void
LTextTableView::SetFrozenRowsSelectable(Boolean inSelectable)
{
	mFrozenRowsSelectable = inSelectable;
}


// ---------------------------------------------------------------------------
//	¥ GetDoubleClickMessage & SetDoubleClickMessage
// ---------------------------------------------------------------------------
//	Accessors

MessageT
LTextTableView::GetDoubleClickMessage() const
{
	return mDoubleClickMessage;
}


//	Specify the message broadcasted when a cell is double-clicked
void
LTextTableView::SetDoubleClickMessage(
	MessageT	inMessage)
{
	mDoubleClickMessage = inMessage;
}


// ---------------------------------------------------------------------------
//	¥ GetSingleClickMessage & SetSingleClickMessage
// ---------------------------------------------------------------------------
//	Accessors

MessageT
LTextTableView::GetSingleClickMessage() const
{
	return mSingleClickMessage;
}


//	Specify the message broadcasted when a cell is clicked
void
LTextTableView::SetSingleClickMessage(
	MessageT	inMessage)
{
	mSingleClickMessage = inMessage;
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------
//	Return the value of a LTextTableView.
//
//	The "value" of a LTextTableView is the row number of the first selected cell,
//	with the first row being number 0 (the ListManager uses zero-based
//	numbering). If no cells are selected, the value is -1.
//
//	This "value" makes sense for a LTextTableView with one column, or a multi-column
//  table being used as a database browser

SInt32
LTextTableView::GetValue() const
{
	return (SInt32) mTableSelector->GetFirstSelectedRow()-1;  // Like LListBox, 0-based
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set the value of a LTextTableView.
//
//	The "value" of a LTextTableView is the row number of the first selected cell,
//	with the first row being number 0 (the ListManager uses zero-based
//	numbering). If no cells are selected, the value is -1.
//
//	This "value" makes sense for a LTextTableView with one column, or a multi-column
//  table being used as a database browser
//
//	This function selects the cell in row "inValue" and column 1, deselecting
//	any previously selected cells.

void
LTextTableView::SetValue(
	SInt32	inValue)  // 0-based
{
	mTableSelector->UnselectAllCells();
	mTableSelector->SelectCell(STableCell((TableIndexT) inValue+1, 1));  // first cell in 1-based row
	// NOTE if you want whole rows selected, use a Row selector, eg: LTableSingleRowSelector
	// or LTableEditableSelector
}


#pragma mark --- Cell Sizer ---

// ---------------------------------------------------------------------------
//	¥ SetCellSizer
// ---------------------------------------------------------------------------
//	Specify the Cell Sizer for a TableView.
//
//	The Cell Sizer specifies the width of the text drawn in cells of the Table

void
LTextTableView::SetCellSizer(
	LCellSizer	*inSizer)
{
	delete mCellSizer;
	mCellSizer = inSizer;
	inSizer->SetTableView(this);
}




#pragma mark --- Clicking ---

// ---------------------------------------------------------------------------
//	¥ ClickCell
// ---------------------------------------------------------------------------
//	Handle a mouse click within the specified Cell

void
LTextTableView::ClickCell(
	const STableCell&		/* inCell */,
	const SMouseDownEvent&	inMouseDown )
{
/*
On clicking we trigger one of the three actions below.

The caller LTableView::ClickSelf has already called ClickSelect
and so the selection changes even though the mouse is not released.

This is important for the reaction to the drag - we want the dragged
item to be the currently selected row, and ClickSelect will do the right
thing if we click without modifiers on already selected items it does
NOT change the selection, so we can drag a multiple selection.

*/
	const bool isDraggable = (mDragger && LDropArea::DragAndDropIsPresent() );
	if ( isDraggable &&	::WaitMouseMoved( inMouseDown.macEvent.where ) )
		mDragger->CreateDragTask(this, inMouseDown);
	else {
		if (isDraggable && inMouseDown.delaySelect)
			UpdateClickCount(inMouseDown);		//AET -
// properly update click count after a click on a cell in an inactive window
// that has delay select and Drag & Drop behavior.
		if (GetClickCount() == 1)
			SingleClicked();
		else
			DoubleClicked();
	} // not a drag
}


// ---------------------------------------------------------------------------
//	¥ Click
// ---------------------------------------------------------------------------
//	Handle a mouse click within the specified Cell

void
LTextTableView::Click(
	SMouseDownEvent	&inMouseDown )
{
	if ( mDragger && inMouseDown.delaySelect && LDropArea::DragAndDropIsPresent() ) {

		// In order to support dragging from an inactive window,
		// we must explicitly test for delaySelect and the
		// presence of Drag and Drop.

		// Convert to a local point.
		PortToLocalPoint( inMouseDown.whereLocal );

		// Execute click attachments.
		if ( ExecuteAttachments( msg_Click, &inMouseDown ) ) {

			// Handle the actual click event.
			if (SwitchTarget(this))
				ClickSelf( inMouseDown );

		}

	} else {

		// Call inherited for default behavior.
		if (SwitchTarget(this))
			LTableView::Click( inMouseDown );

	}
}



// ---------------------------------------------------------------------------
//	¥ ClickSelect
// ---------------------------------------------------------------------------
//	Adjust selection in response to a click in the specified cell
//
//	Return whether or not to process the click as a normal click
//	Overriden so we can ignore clicks in the frozen header rows
//	(Later catch them as a special case, eg: sort by this column

Boolean
LTextTableView::ClickSelect(
	const STableCell		&inCell,
	const SMouseDownEvent	&inMouseDown)
{
// see if click in header area
// we rely on GetCellHitBy to have fixed the row numbers, so a click in the header
// will have the correct number
	if ((inCell.row <= mNumFrozenRows) && !mFrozenRowsSelectable)
		return false;	// swallow a click in the heading
	// NOT YET IMPLEMENTED - handle clicks in heading differently

	if (mTableSelector != nil) {
		if (mUseDragSelect) {
			return mTableSelector->DragSelect(inCell, inMouseDown);

		} else {
			mTableSelector->ClickSelect(inCell, inMouseDown);
		}
	}

	return true;
}


// ---------------------------------------------------------------------------
//	¥ SingleClicked
// ---------------------------------------------------------------------------
//	Default action if cell clicked, override for custom actions

void
LTextTableView::SingleClicked()
{
	if (mSingleClickMessage != msg_Nothing)
		BroadcastMessage(mSingleClickMessage, this);
}


// ---------------------------------------------------------------------------
//	¥ DoubleClicked
// ---------------------------------------------------------------------------
//	Default action if cell double-clicked, override for custom actions

void
LTextTableView::DoubleClicked()
{
	if (mDoubleClickMessage != msg_Nothing)
		BroadcastMessage(mDoubleClickMessage, this);
}


#pragma mark --- Dragging ---

// ---------------------------------------------------------------------------
//	¥ SetDragger
// ---------------------------------------------------------------------------
//	Specify the Dragger for a TableView.
//
//	The Dragger is used to create DragTasks so cells or selections can be dragged
//  from the table using the Mac Drag Manager.

void
LTextTableView::SetTableDragger(
	LTableDragger	*inDragger)
{
	delete mDragger;
	mDragger = inDragger;
}



#pragma mark --- Drawing ---

// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------
// FocusDraw, so we get the correct font and colours from our TextTraits
Boolean
LTextTableView::FocusDraw(LPane*)
{
	Boolean	focused = LView::FocusDraw();
	if (focused) {
		StColorPenState::Normalize();
		UTextTraits::SetPortTextTraits(mTextTraitsID);
		if (mEnabled == triState_Off) {
			RGBColor	AGA_Gray6 = {39321, 39321, 39321};
			::RGBForeColor(&AGA_Gray6);
		}
	}

	return focused;
}



// ---------------------------------------------------------------------------
//	¥ GetLocalCellRect
// ---------------------------------------------------------------------------
//	Pass back the bounding rectangle of the specified Cell and return
//	whether it intersects the Frame of the TableView
//
//	The bounding rectangle is in Local coordinates so it will always be
//	within QuickDraw space when its within the Frame. If the bounding
//	rectangle is outside the Frame, return false and set the rectangle
//	to (0,0,0,0)

Boolean
LTextTableView::GetLocalCellRect(
	const STableCell	&inCell,
	Rect				&outCellRect) const
{
	SInt32	cellLeft, cellTop, cellRight, cellBottom;
	Boolean	insideFrame;
	mTableGeometry->GetImageCellBounds(inCell, cellLeft, cellTop,
							cellRight, cellBottom);

// the following looks a bit repetitive but is optimised to reduce the number of tests
	if (mNumFrozenRows) {
			SInt32 imageOffset = mFrameLocation.v - mImageLocation.v; // positive Frame and negative ImageLocn if scrolled

		if (inCell.row <= mNumFrozenRows)	{
		// frozen rows fake their location, as if overlaid on the top N rows of the visible area
		// we do the opposite of GetCellHitBy and map an Image location of the actual cell
		// back to the Image location of the visible cell
			cellTop += imageOffset;
			cellBottom += imageOffset;
			insideFrame =	ImageRectIntersectsFrame(cellLeft, cellTop, cellRight, cellBottom);
		} // is a frozen row
		else {
			if (ImageRectIntersectsFrame(cellLeft, cellTop, cellRight, cellBottom)) {  // is visible
				UInt16 frozenRowsHeight = FrozenRowsHeight();
				if ((cellBottom-imageOffset) >= frozenRowsHeight) { // not on top of frozen row
					insideFrame = true;
					if ((cellTop-imageOffset) <frozenRowsHeight)  // overlaps frozen row
						cellTop = imageOffset + frozenRowsHeight;
				}
				else
					insideFrame = false;
// WARNING test cellBottom so partially revealed rows are drawn correctly
			}
		} // not a frozen row
	} // have frozen rows
	else  // no frozen rows
		insideFrame =	ImageRectIntersectsFrame(cellLeft, cellTop, cellRight, cellBottom);

	if (insideFrame) {
		SPoint32	imagePt;
		imagePt.h = cellLeft;
		imagePt.v = cellTop;
		ImageToLocalPoint(imagePt, topLeft(outCellRect));
		outCellRect.right = (SInt16) (outCellRect.left + (cellRight - cellLeft));
		outCellRect.bottom = (SInt16) (outCellRect.top + (cellBottom - cellTop));
	}
	else {
		outCellRect.left = 0;
		outCellRect.top = 0;
		outCellRect.right = 0;
		outCellRect.bottom = 0;
	}

	return insideFrame;
}


// ---------------------------------------------------------------------------
//	¥ ScrollBits
// ---------------------------------------------------------------------------
//	Scroll the pixels of a View
//
//	Called internally by ScrollImageBy to shift the pixels
// 	Overridden so we can avoid scrolling any frozen rows at top of sheet

void
LTextTableView::ScrollBits(
	SInt32		inLeftDelta,			// Pixels to scroll horizontally
	SInt32		inTopDelta)				// Pixels to scroll vertically
{
	if (FocusExposed()) {
										// Get Frame in Port coords
		Rect	frame = mRevealedRect;

		if (mNumFrozenRows) 	// deduct height of frozen rows from scrolled bits
			frame.top += (SInt16) FrozenRowsHeight();

		PortToLocalPoint(topLeft(frame));
		PortToLocalPoint(botRight(frame));

										// Scroll Frame, clipping to the
										//   update region
		StRegion	updateRgn;
		::ScrollRect(&frame, (SInt16) -inLeftDelta, (SInt16) -inTopDelta, updateRgn);

		RefreshRgn(updateRgn);			// Force redraw of update region
	}
}

// ---------------------------------------------------------------------------
//	¥ DrawCell
// ---------------------------------------------------------------------------
//	Draw the contents of the specified Cell

void
LTextTableView::DrawCell(
	const STableCell	&inCell,
	const Rect			&inLocalRect)
{
	if (!mCellBuffer)
		return;

	UInt32 ioDataSize = mCellBufferSize;
	GetCellData(inCell, mCellBuffer, ioDataSize);

//	::MoveTo(inLocalRect.left, inLocalRect.bottom - 4);
	::MoveTo((SInt16) (inLocalRect.left+3), (SInt16) (inLocalRect.bottom - 2));
	if (mCellSizer)
		ioDataSize = mCellSizer->TruncatedLength(mCellBuffer, (UInt16) ioDataSize, inCell.col);
	::MacDrawText(mCellBuffer, 0, (SInt16) ioDataSize);

// bit of a hack to draw the underlining of the frozen rows
	if (inCell.row == mNumFrozenRows) {	// cell is on the bottom of the rows
		::MoveTo(inLocalRect.left,  (SInt16) (inLocalRect.bottom - mFrozenRowUnderlineThickness));
		::MacLineTo(inLocalRect.right, (SInt16) (inLocalRect.bottom - mFrozenRowUnderlineThickness));
// FUTURE - make line thickness optional, remember to update FrozenRowsHeight()
// depending on the method used
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
// override so frozen rows draw as a separate operation
// otherwise is straight copy of LTableView::DrawSelf

void
LTextTableView::DrawSelf()
{
	DrawBackground();

		// Determine cells that need updating. Rather than checking
		// on a cell by cell basis, we just see which cells intersect
		// the bounding box of the update region.

		// With frozen rows, the problem is handled in two passes
		// WRITTEN in blazing hurry for CW12 (my excuse for code quality)

	StClipRgnState	saveClip;					// Save current clip region since
												//   we change it for frozen rows

	RgnHandle	localUpdateRgnH = GetLocalUpdateRgn();
	Rect		updateRect;
	::GetRegionBounds(localUpdateRgnH, &updateRect);
	::DisposeRgn(localUpdateRgnH);

	STableCell	topLeftCell, botRightCell;

	if (mNumFrozenRows) {

			UInt16 frozenRowsHeight = FrozenRowsHeight();

			// subset of logic in LView::CalcPortOrigin, just adjusting the vertical coordinate
			SInt32	adjustedImageLocn = -mImageLocation.v;
			if (adjustedImageLocn > max_PortOrigin) {	// coord is too big
				adjustedImageLocn &= mask_Lo14Bits;		// use coord modulo 2^14
			} else if (adjustedImageLocn < min_PortOrigin) {
				adjustedImageLocn = min_PortOrigin;		// coord is too small, limit to minimum
			}

			SInt32 frozenTop = mFrameLocation.v + adjustedImageLocn; // positive Frame and negative ImageLocn (mod 16K and sign reversed above) if scrolled

			SInt32 frozenBottom = frozenTop + frozenRowsHeight;
			if (updateRect.top < frozenBottom) { // overlaps frozen rows, so draw two parts
				Rect		frozenRect = updateRect;
				frozenRect.top = (SInt16) frozenTop;
				frozenRect.bottom = (SInt16) frozenBottom;
				FetchIntersectingCells(frozenRect, topLeftCell, botRightCell);

											// Draw each cell within the update rect
				STableCell	cell;
				for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
					for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
						Rect	cellRect;
						GetLocalCellRect(cell, cellRect);
						DrawCell(cell, cellRect);
					}
				}
			}
// now adjust original drawing area
			updateRect.top = (SInt16) frozenBottom;
			::ClipRect(&updateRect);
	} // has frozen rows

	FetchIntersectingCells(updateRect, topLeftCell, botRightCell);

								// Draw each cell within the update rect
	STableCell	cell;
	for (cell.row = topLeftCell.row; cell.row <= botRightCell.row; cell.row++) {
		for (cell.col = topLeftCell.col; cell.col <= botRightCell.col; cell.col++) {
			Rect	cellRect;
			GetLocalCellRect(cell, cellRect);
			DrawCell(cell, cellRect);
		}
	}

	HiliteSelection(IsActive(), true);
}


// ---------------------------------------------------------------------------
//	¥ FrozenRowsHeight
// ---------------------------------------------------------------------------
// recalculates height each time
// in case geometry or text trait changes
// (could we make this smarter and cached?)

UInt16
LTextTableView::FrozenRowsHeight() const
{
	UInt16 accumHeight = mFrozenRowUnderlineThickness;	// for better drawing
	for (UInt16 frozenRow = 1; frozenRow <= mNumFrozenRows; frozenRow++) {
		STableCell	measureCell(frozenRow, 1);
		SInt32	cellLeft, cellTop, cellRight, cellBottom;
		mTableGeometry->GetImageCellBounds(measureCell, cellLeft, cellTop, cellRight, cellBottom);
		accumHeight += (UInt16) (cellBottom - cellTop);
	}
	return accumHeight;
}


// ---------------------------------------------------------------------------
//	¥ GetCellHitBy
// ---------------------------------------------------------------------------
//	Pass back the cell which contains the specified point.
//
//	If no cell contains the point, return false and
//
//		outCell.row = 0 if point is above the Table
//		outCell.row = mRows + 1 if point is below the Table
//
//		outCell.col = 0 if point is to the left of the Table
//		outCell.col = mCols + 1 if point is to the right of the Table
//
//	For example, if the horizontal coord. is within Column 2,
//	but the vertical coord. is above the Table,
//		outCell.row = 0
//		outCell.col = 2

Boolean
LTextTableView::GetCellHitBy(
	const SPoint32	&inImagePt,
	STableCell		&outCell) const
{
	Boolean	containsPoint = true;

// fix problem if a Refresh occurs before we have finished init of
// all LTextTableViews
	if (!mTableGeometry) {
		outCell.row = 0;
		return false;
	}
	if (inImagePt.v < 0) {
		outCell.row = 0;
		containsPoint = false;

	}
	else if (inImagePt.v >= mImageSize.height) {
		outCell.row = mRows + 1;
		containsPoint = false;

	}
	else {
		if (mNumFrozenRows) {
// adjust the image point if it is within frozen row area
// mImageLocation.v==0 unless we are scrolled, and will be negative after scrolling
			SInt32	localV = mImageLocation.v + inImagePt.v - mFrameLocation.v;
			// corrected back to measure from top of view, as if not scrolled, and made 0-based (above values are port coords)
			UInt16 frozenRowsHeight = FrozenRowsHeight();
			if (localV < frozenRowsHeight) { // inside frozen rows
				const SPoint32 headerPt = {inImagePt.h, localV};
				outCell.row = mTableGeometry->GetRowHitBy(headerPt);
			}
			else
				outCell.row = mTableGeometry->GetRowHitBy(inImagePt);
		} // frozen rows
		else
			outCell.row = mTableGeometry->GetRowHitBy(inImagePt);
	}

	if (inImagePt.h < 0) {
		outCell.col = 0;
		containsPoint = false;

	} else if (inImagePt.h >= mImageSize.width) {
		outCell.col = mCols + 1;
		containsPoint = false;

	} else {
		outCell.col = mTableGeometry->GetColHitBy(inImagePt);
	}
	return containsPoint;
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//	Just override to update our focus box, if any

void
LTextTableView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LView::ResizeFrameBy(inWidthDelta, inHeightDelta,inRefresh);
	if (mFocusBox) {			// Resize FocusBox, too
		mFocusBox->ResizeFrameBy(inWidthDelta, inHeightDelta,inRefresh);
	}
}



// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//	Just override to update our focus box, if any

void
LTextTableView::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LView::MoveBy(inHorizDelta, inVertDelta,inRefresh);
	if (mFocusBox) {			// Move FocusBox, too
		mFocusBox->MoveBy(inHorizDelta, inVertDelta, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------
//	Table is becoming visible

void
LTextTableView::ShowSelf()
{
	if (IsOnDuty() && (mFocusBox != nil)) {
		FocusDraw();					// needed in case mFocusBox draws
		mFocusBox->Show();			// Show FocusBox if we are the Target
	}
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	Table is becoming invisible

void
LTextTableView::HideSelf()
{
	if (IsOnDuty()) {				// Shouldn't be on duty when invisible
		SwitchTarget(GetSuperCommander());
	}
}


#pragma mark --- Saved Selections ---

// ---------------------------------------------------------------------------
//	¥ Set & GetSavedSelection
// ---------------------------------------------------------------------------
LTableSelState*
LTextTableView::GetSavedSelection() const
{
	LTableSelState* ret = 0;
	LTableSelector*		theSelector = mTableSelector;

// the following is only necessary because we have not integrated these methods into the main
// Powerplant LTableSelector abstract hierarchy, so we have to downcast the selector
	LTableMultiRowSelector*	realSelector = dynamic_cast<LTableMultiRowSelector*> (theSelector);
	if (realSelector)
		ret = realSelector->GetSavedSelection();
	else {
		LTableSingleRowSelector*	realSelector = dynamic_cast<LTableSingleRowSelector*> (theSelector);
		if (realSelector)
			ret = realSelector->GetSavedSelection();
	}
	return ret;
}


void
LTextTableView::SetSavedSelection(const LTableSelState* inSS)
{
	LTableSelector*		theSelector = GetTableSelector();

// the following is only necessary because we have not integrated these methods into the main
// Powerplant LTableSelector abstract hierarchy, so we have to downcast the selector
	LTableMultiRowSelector*	realSelector = dynamic_cast<LTableMultiRowSelector*> (theSelector);
	if (realSelector)
		realSelector->SetSavedSelection(inSS);
	else {
		LTableSingleRowSelector*	realSelector = dynamic_cast<LTableSingleRowSelector*> (theSelector);
		if (realSelector)
			realSelector->SetSavedSelection(inSS);
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollCellIntoFrame
// ---------------------------------------------------------------------------
//	Scroll the TableView as little as possible to move the specified Cell
//	so that it's entirely within the Frame of the TableView
//
//	If Cell is wider and/or taller than Frame, align Cell to left/top
//	of Frame.

// overriden so when scrolling up we add an offset for the frozen rows

void
LTextTableView::ScrollCellIntoFrame(
	const STableCell	&inCell)
{
	SInt32	cellLeft, cellTop, cellRight, cellBottom;
	mTableGeometry->GetImageCellBounds(inCell, cellLeft, cellTop,
							cellRight, cellBottom);

	if (ImagePointIsInFrame(cellLeft, cellTop) &&
		ImagePointIsInFrame(cellRight, cellBottom)) {

		return;						// Entire Cell is already within Frame
	}

	Rect	frame;					// Get Frame in Image coords
	CalcLocalFrameRect(frame);
	SPoint32	frameTopLeft, frameBotRight;
	LocalToImagePoint(topLeft(frame), frameTopLeft);
	LocalToImagePoint(botRight(frame), frameBotRight);

	SInt32	horizScroll = 0;		// Find horizontal scroll amount
	if (cellRight > frameBotRight.h) {
										// Cell is to the right of frame
		horizScroll = cellRight - frameBotRight.h;
	}

	if ((cellLeft - horizScroll) < frameTopLeft.h) {
										// Cell is to the left of frame
										//   or Cell is wider than frame
		horizScroll = cellLeft - frameTopLeft.h;
	}

	SInt32	vertScroll = 0;			// Find vertical scroll amount
	if (cellBottom > frameBotRight.v) {
										// Cell is below bottom of frame
		vertScroll = cellBottom - frameBotRight.v;
		if (mNumFrozenRows) 	// deduct height of frozen rows from scrolled bits
			vertScroll += FrozenRowsHeight();
	}

	if ((cellTop - vertScroll) < frameTopLeft.v) {
										// Cell is above top of frame
										//   or Cell is taller than frame
		vertScroll = cellTop - frameTopLeft.v;
	}

	ScrollPinnedImageBy(horizScroll, vertScroll, Refresh_Yes);
}

PP_End_Namespace_PowerPlant
