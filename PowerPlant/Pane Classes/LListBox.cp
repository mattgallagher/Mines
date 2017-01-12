// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LListBox.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for the Toolbox List Manager
//
//	You may manipulate the ListHandle using the Toolbox ListManager traps,
//	as well as inspect and change certain fields. As documented in Inside
//	Mac, you must directly store into certain fields in a ListRecord in
//	order to change settings for a List. For example, the selFlags field
//	controls how the ListManger handles selections in a List.
//
//	There are some traps you should not call, since there is a LListBox
//	function that performs the same task (in a way that's compatible
//	with PowerPlant Panes).
//
//		ListManager Trap		LListBox function
//		----------------		-----------------
//		LDispose				~LListBox
//		LSize					ResizeFrameBy
//		LDraw					Draw

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LListBox.h>
#include <LStream.h>
#include <LFocusBox.h>
#include <LView.h>

#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UTextTraits.h>
#include <UKeyFilters.h>
#include <UDrawingState.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LListBox								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LListBox::LListBox()
{
	InitListBox(false, false, false, false, 0, 0, 0);
}


// ---------------------------------------------------------------------------
//	¥ LListBox								Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//
//	This function creates a new Toolbox ListHandle for the copy. It copies
//	most settings of the original ListHandle, including all cell data,
//	selection flags, and scrolling flags. However, it does not copy the
//	refCon, lClikLoop, and userHandle.
//
//	Only the first 255 bytes of each cell's data is copied.

LListBox::LListBox(
	const LListBox&		inOriginal)

	: LPane(inOriginal),
	  LCommander(inOriginal),
	  LBroadcaster(inOriginal)
{
	mDoubleClickMessage = inOriginal.mDoubleClickMessage;
	mTextTraitsID = inOriginal.mTextTraitsID;

	mFocusBox = nil;
	if (inOriginal.mFocusBox != nil) {
		mFocusBox = new LFocusBox(*(inOriginal.mFocusBox));
	}

	ListHandle	origListH = inOriginal.mMacListH;
	ListPtr		origListP = *origListH;

									// Get info about list size, cell size,
									// and scrolling from original ListHandle
	Rect	displayRect = origListP->rView;
	Rect	dataBounds = origListP->dataBounds;
	Point	cellSize = origListP->cellSize;
	Boolean	hasGrow = inOriginal.mHasGrow;
	Boolean	hasHorizScroll = (origListP->hScroll != nil);
	Boolean	hasVertScroll = (origListP->vScroll != nil);

									// Create new Toolbox ListHandle using
									//   same specifications as original
	FocusDraw();
	mMacListH = ::LNew(&displayRect, &dataBounds, cellSize, 0,
						UQDGlobals::GetCurrentWindowPort(), false, hasGrow,
						hasHorizScroll, hasVertScroll);

	::LActivate(false, mMacListH);

									// Copy data for each cell
	if ((dataBounds.right > 0) && (dataBounds.bottom > 0)) {
		Cell	theCell = Point_00;
		do {
			char		cellData[255];
			SInt16		dataLen = 255;
			::LGetCell(cellData, &dataLen, theCell, origListH);
			::LSetCell(cellData, dataLen, theCell, mMacListH);
		} while (LNextCell(true, true, &theCell, origListH));
	}

									// Copy selection and scrolling flags
	(**mMacListH).selFlags = (**origListH).selFlags;
	(**mMacListH).listFlags = (**origListH).listFlags;
}


// ---------------------------------------------------------------------------
//	¥ LListBox								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LListBox::LListBox(
	const SPaneInfo&	inPaneInfo,
	Boolean				inHasHorizScroll,
	Boolean				inHasVertScroll,
	Boolean				inHasGrow,
	Boolean				inHasFocusBox,
	MessageT			inDoubleClickMessage,
	SInt16				inTextTraitsID,
	SInt16				inLDEFid,
	LCommander*			inSuper)

	: LPane(inPaneInfo),
	  LCommander(inSuper)
{
	InitListBox(inHasHorizScroll, inHasVertScroll, inHasGrow,
				inHasFocusBox, inDoubleClickMessage, inTextTraitsID,
				inLDEFid);
}


// ---------------------------------------------------------------------------
//	¥ LListBox								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LListBox::LListBox(
	LStream*	inStream)

	: LPane(inStream)
{
	SListBoxInfo	listInfo;
	*inStream >> listInfo.hasHorizScroll;
	*inStream >> listInfo.hasVertScroll;
	*inStream >> listInfo.hasGrow;
	*inStream >> listInfo.hasFocusBox;
	*inStream >> listInfo.doubleClickMessage;
	*inStream >> listInfo.textTraitsID;
	*inStream >> listInfo.LDEFid;
	*inStream >> listInfo.numberOfItems;

	InitListBox(listInfo.hasHorizScroll, listInfo.hasVertScroll,
				listInfo.hasGrow, listInfo.hasFocusBox,
				listInfo.doubleClickMessage, listInfo.textTraitsID,
				listInfo.LDEFid);

	if (listInfo.numberOfItems > 0) {
		::LAddColumn(1, 0, mMacListH);
		::LAddRow(listInfo.numberOfItems, 0, mMacListH);

		Cell	theCell = Point_00;

		for (theCell.v = 0; theCell.v < listInfo.numberOfItems; theCell.v++) {
			Str255	listItem;
			inStream->ReadPString(listItem);
			::LSetCell(listItem+1, listItem[0], theCell, mMacListH);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LListBox								Destructor				  [public]
// ---------------------------------------------------------------------------

LListBox::~LListBox()
{
	if (mMacListH != nil) {			// Dispose Toolbox ListHandle
		FocusDraw();
		::LDispose(mMacListH);
	}
}


// ---------------------------------------------------------------------------
//	¥ InitListBox							Initializer				 [private]
// ---------------------------------------------------------------------------

void
LListBox::InitListBox(
	Boolean			inHasHorizScroll,
	Boolean			inHasVertScroll,
	Boolean			inHasGrow,
	Boolean			inHasFocusBox,
	MessageT		inDoubleClickMessage,
	SInt16			inTextTraitsID,
	SInt16			inLDEFid)
{
	mHasGrow = inHasGrow;
	mFocusBox = nil;				// Create FocusBox if necessary
	if (inHasFocusBox) {
		mFocusBox = new LFocusBox;
		mFocusBox->Hide();
		mFocusBox->AttachPane(this, false);
	}

	Rect	displayRect;			// Determine size of Toolbox List
									//   ListBox has a one-pixel border
	CalcLocalFrameRect(displayRect);
	::MacInsetRect(&displayRect, 1, 1);

	if (inHasHorizScroll) {			// Adjust size for scroll bars
		displayRect.bottom -= 15;
	}
	if (inHasVertScroll) {
		displayRect.right -= 15;
	}

	mDoubleClickMessage = inDoubleClickMessage;
	mTextTraitsID = inTextTraitsID;

	StFocusAndClipIfHidden	focus(this);

									// Create Toolbox ListHandle with:
									//   no cells
									//   default cell size
									//   drawing mode off
	Rect	dataBounds = Rect_0000;
	Point	cellSize = Point_00;
	mMacListH = ::LNew(&displayRect, &dataBounds, cellSize, inLDEFid,
						UQDGlobals::GetCurrentWindowPort(), false,
						inHasGrow, inHasHorizScroll, inHasVertScroll);

	::LActivate(false, mMacListH);

		// Toolbox ListManager seems to have a bug. When there is only
		// one scroll bar (vertical or horizontal), the scroll bar is
		// not indented if inHasGrow is true. As a workaround, we
		// manually set the scroll bar size in such cases.

	if (inHasGrow) {
		Rect	bounds;

		if (inHasVertScroll && !inHasHorizScroll) {			// Vert only
			::GetControlBounds(::GetListVerticalScrollBar(mMacListH), &bounds);
			bounds.bottom = (SInt16) (displayRect.bottom - 14);
			::SetControlBounds(::GetListVerticalScrollBar(mMacListH), &bounds);

		} else if (inHasHorizScroll && !inHasVertScroll) {	// Horiz only
			::GetControlBounds(::GetListHorizontalScrollBar(mMacListH), &bounds);
			bounds.right = (SInt16) (displayRect.right - 14);
			::SetControlBounds(::GetListHorizontalScrollBar(mMacListH), &bounds);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------
//	Return the value of a ListBox.
//
//	The "value" of a ListBox is the row number of the first selected cell,
//	with the first row being number 0 (the ListManager uses zero-based
//	numbering). If no cells are selected, the value is -1.
//
//	This "value" makes sense for a ListBox with one column, by far the
//	most common case.

SInt32
LListBox::GetValue() const
{
	SInt32	value = -1;

	Cell	firstSelection = Point_00;
	if (::LGetSelect(true, &firstSelection, mMacListH)) {
		value = firstSelection.v;	// Row number of selection
	}

	return value;
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set the value of a ListBox.
//
//	The "value" of a ListBox is the row number of the first selected cell,
//	with the first row being number 0 (the ListManager uses zero-based
//	numbering). If no cells are selected, the value is -1.
//
//	This "value" makes sense for a ListBox with one column, by far the
//	most common case.
//
//	This function selects the cell in row "inValue" and column 1, deselecting
//	any previously selected cells.

void
LListBox::SetValue(
	SInt32	inValue)
{
	FocusDraw();
									// Deselect all cells. However, don't
									//   deselect the target cell if it
									//   is already selected.
	Cell	theCell = Point_00;
	while (::LGetSelect(true, &theCell, mMacListH)) {
		if (theCell.v != inValue) {
			::LSetSelect(false, theCell, mMacListH);
		} else {
			::LNextCell(true, true, &theCell, mMacListH);
		}
	}

	theCell.h = 0;					// First column
	theCell.v = (SInt16) inValue;	// Row specified by inValue
	::LSetSelect(true, theCell, mMacListH);
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return the descriptor of a ListBox, which is the text of the first
//	selected cell. The descriptor is an empty string if there are no
//	selected cells.
//
//	This function assumes that the cell data is text.

StringPtr
LListBox::GetDescriptor(
	Str255	outDescriptor) const
{
	outDescriptor[0] = 0;
	Cell	firstSelection = Point_00;
	if (::LGetSelect(true, &firstSelection, mMacListH)) {
		SInt16	dataLen = 255;
		::LGetCell(outDescriptor + 1, &dataLen, firstSelection, mMacListH);
		outDescriptor[0] = (UInt8) dataLen;
	}

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Set the descriptor of a ListBox, which is the text of the first
//	selected cell. Nothing happens if there are no selected cells.
//
//	This function assumes that the cell data is text.

void
LListBox::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	Cell	firstSelection = Point_00;
	if (::LGetSelect(true, &firstSelection, mMacListH) && FocusDraw()) {
		::LSetCell(inDescriptor+1, inDescriptor[0], firstSelection, mMacListH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------
//	Focus drawing. Overrides to use the default Pen state and to
//	set the TextTraits used by the ListBox

Boolean
LListBox::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean	focused = LPane::FocusDraw();
	UTextTraits::SetPortTextTraits(mTextTraitsID);

	Pattern		whitePat;
	::BackPat(UQDGlobals::GetWhitePat(&whitePat));

	return focused;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw ListBox

void
LListBox::DrawSelf()
{
		// A Mac ListRec stores a pointer to its owner port  We have to
		// change it to the current port in case we are drawing into
		// a port that is not the owner port. This happens when we are
		// printing or drawing into an offscreen port.

	GrafPtr	savePort = (**mMacListH).port;
	(**mMacListH).port = UQDGlobals::GetCurrentPort();

	RgnHandle	updateRgn = GetLocalUpdateRgn();
	::LUpdate(updateRgn, mMacListH);
	::DisposeRgn(updateRgn);

	(**mMacListH).port = savePort;

	ApplyForeAndBackColors();
	::PenNormal();

	Rect	frame;
	CalcLocalFrameRect(frame);
	::MacFrameRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside an ListBox

void
LListBox::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	if (SwitchTarget(this)) {
		FocusDraw();

		if (::LClick(inMouseDown.whereLocal, inMouseDown.macEvent.modifiers,
					mMacListH)) {

			BroadcastMessage(mDoubleClickMessage, this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Respond to Command message

Boolean
LListBox::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case msg_TabSelect:
			if (!IsEnabled()) {
				cmdHandled = false;
			}
			break;

		case cmd_SelectAll:
			SelectAllCells();
			break;

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
LListBox::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_SelectAll: {		// Check if cells exist and if List
									//   can have more than one selection
			ListPtr		listP = *mMacListH;
			outEnabled = (listP->dataBounds.right > 0)  &&
						 (listP->dataBounds.bottom > 0)  &&
						 ((listP->selFlags & lOnlyOne) == 0);
			break;
		}

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress
// ---------------------------------------------------------------------------
//	ListBox supports keyboard navigation and type selection

Boolean
LListBox::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	Boolean	keyHandled = true;
	UInt16	theKey = (UInt16) (inKeyEvent.message & charCodeMask);

	FocusDraw();

	if (UKeyFilters::IsNavigationKey(theKey)) {
		DoNavigationKey(inKeyEvent);

	}  else if ( (UKeyFilters::IsPrintingChar(theKey)) &&
                 (!(inKeyEvent.modifiers & cmdKey)) ) {
		DoTypeSelection(inKeyEvent);

	} else {
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ DoNavigationKey
// ---------------------------------------------------------------------------
//	Implements keyboard navigation by supporting selection change using
//	the arrow keys, page up, page down, home, and end

void
LListBox::DoNavigationKey(
	const EventRecord	&inKeyEvent)
{
	char	theKey = (char) (inKeyEvent.message & charCodeMask);
	Boolean	cmdKeyDown = (inKeyEvent.modifiers & cmdKey) != 0;
	Boolean	extendSelection = ((inKeyEvent.modifiers & shiftKey) != 0) &&
							  (((**mMacListH).selFlags & lOnlyOne) == 0);
	Cell	theSelection = Point_00;

	switch (theKey) {

		case char_LeftArrow:
			if (::LGetSelect(true, &theSelection, mMacListH)) {
				if (cmdKeyDown && extendSelection) {
									// Select all cells left of the first
									//   selected cell
					while (--theSelection.h >= 0) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select leftmost cell in the same row as
									//   the first selected cell
					theSelection.h = 0;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell left of the first selected cell
					if (theSelection.h > 0) {
						theSelection.h -= 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell to the left of the first
									//   selected cell
					if(theSelection.h > 0) {
						theSelection.h -= 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_RightArrow:
			if (GetLastSelectedCell(theSelection)) {
				SInt16	numColumns = (SInt16) ((**mMacListH).dataBounds.right - 1);

				if (cmdKeyDown && extendSelection) {
									// Select all cells right of the last
									//   selected cell
					while (++theSelection.h <= numColumns) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select rightmost cell in the same row as
									//   the last selected cell
					theSelection.h = numColumns;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the cell
									//   to the right of the last selected cell
					if(theSelection.h < numColumns) {
						theSelection.h += 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell to the right of the last
									//   selected cell

					if(theSelection.h < numColumns) {
						theSelection.h += 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_UpArrow:
			if (::LGetSelect(true, &theSelection, mMacListH)) {
				if (cmdKeyDown && extendSelection) {
									// Select all cells above the first
									//   selected cell
					while (--theSelection.v >= 0) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select cell at top of column of the
									//   first selected cell
					theSelection.v = 0;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell above the first selected cell
					if (theSelection.v > 0) {
						theSelection.v -= 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell one above the first
									//   selected cell
					if(theSelection.v > 0) {
						theSelection.v -= 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_DownArrow:
			if (GetLastSelectedCell(theSelection)) {
				SInt16	numRows = (SInt16) ((**mMacListH).dataBounds.bottom - 1);

				if (cmdKeyDown && extendSelection) {
									// Select all cells below the last
									//   selected cell
					while (++theSelection.v <= numRows) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select cell at bottom of column of
									//   the last selected cell
					theSelection.v = numRows;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell below the last selected cell
					if(theSelection.v < numRows) {
						theSelection.v += 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell one below the last
									//   selected cell

					if(theSelection.v < numRows) {
						theSelection.v += 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_Home:
			::LScroll(-16000, -16000, mMacListH);
			break;

		case char_End:
			::LScroll(16000, 16000, mMacListH);
			break;

		case char_PageUp:
		case char_PageDown: {
			SInt16	dRows = (SInt16) ((**mMacListH).visible.bottom -
									  (**mMacListH).visible.top - 1);
			if (theKey == char_PageUp) {
				dRows = (SInt16) (-dRows);
			}
			::LScroll(0, dRows, mMacListH);
			break;
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoTypeSelection
// ---------------------------------------------------------------------------
//	Change selection to the item beginning with the input characters

void
LListBox::DoTypeSelection(
	const EventRecord&	/* inKeyEvent */)
{
}	// +++ Not yet implemented


// ---------------------------------------------------------------------------
//	¥ SelectOneCell
// ---------------------------------------------------------------------------
//	Select the specified Cell and deselect all others

void
LListBox::SelectOneCell(
	Cell	inCell)
{
	StFocusAndClipIfHidden	focus(this);

	Cell	currentCell = Point_00;

	while (::LGetSelect(true, &currentCell, mMacListH)) {
		if (*(SInt32*)&currentCell == *(SInt32*)&inCell) {
									// Cell to select is already selected
			::LNextCell(true, true, &currentCell, mMacListH);

		} else {					// Deselect this cell
			::LSetSelect(false, currentCell, mMacListH);
		}
	}

	::LSetSelect(true, inCell, mMacListH);
}


// ---------------------------------------------------------------------------
//	¥ SelectAllCells
// ---------------------------------------------------------------------------
//	Select all the cells in a ListBox

void
LListBox::SelectAllCells()
{
	if (((**mMacListH).selFlags & lOnlyOne) == 0) {	// Make sure List has
													//   mulitple selection

		StFocusAndClipIfHidden	focus(this);

			// Loop thru cells one by one. It shouldn't hurt to
			// call LSetSelect() if List is empty.

		Cell	theCell = Point_00;
		do {
			::LSetSelect(true, theCell, mMacListH);
		} while (::LNextCell(true, true, &theCell, mMacListH));
	}
}


// ---------------------------------------------------------------------------
//	¥ UnselectAllCells
// ---------------------------------------------------------------------------
//	Unselect all the cells in a ListBox

void
LListBox::UnselectAllCells()
{
	StFocusAndClipIfHidden	focus(this);

	Cell	theCell = Point_00;
	do {
		::LSetSelect(false, theCell, mMacListH);
	} while (::LNextCell(true, true, &theCell, mMacListH));
}


// ---------------------------------------------------------------------------
//	¥ GetLastSelectedCell
// ---------------------------------------------------------------------------
//	Pass back the last selected Cell in a ListBox. Returns false if no
//	cells are selected

Boolean
LListBox::GetLastSelectedCell(
	Cell	&outCell)
{
	Cell	currentCell = Point_00;
	Boolean	hasSelection = ::LGetSelect(true, &currentCell, mMacListH);

	if (hasSelection) {
		do {
			outCell = currentCell;
		} while (::LNextCell(true, true, &currentCell, mMacListH) &&
				 ::LGetSelect(true, &currentCell, mMacListH));
	}

	return hasSelection;
}


// ---------------------------------------------------------------------------
//	¥ MakeCellVisible
// ---------------------------------------------------------------------------
//	Scroll the ListBox as little as possible to move the specified Cell
//	into view

void
LListBox::MakeCellVisible(
	Cell	inCell)
{
	Rect	visibleCells = (**mMacListH).visible;

	if (!::MacPtInRect(inCell, &visibleCells)) {
		SInt16	dCols = 0;			// ¥ Horizontal
		if (inCell.h > visibleCells.right - 1) {
									// Scroll left
			dCols = (SInt16) (inCell.h - visibleCells.right + 1);

		} else if (inCell.h < visibleCells.left) {
									// Scroll right
			dCols = (SInt16) (inCell.h - visibleCells.left);
		}

		SInt16	dRows = 0;			// ¥ Vertical
		if (inCell.v > visibleCells.bottom - 1) {
									// Scroll up
			dRows = (SInt16) (inCell.v - visibleCells.bottom + 1);

		} else if (inCell.v < visibleCells.top) {
									// Scroll down
			dRows = (SInt16) (inCell.v - visibleCells.top);
		}

		FocusDraw();
		::LScroll(dCols, dRows, mMacListH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BeTarget
// ---------------------------------------------------------------------------
//	ListBox is becoming the Target

void
LListBox::BeTarget()
{
	if ((mFocusBox != nil) && IsVisible()) {
		mFocusBox->Show();
	}
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget
// ---------------------------------------------------------------------------
//	ListBox is no longer the Target

void
LListBox::DontBeTarget()
{
	if (mFocusBox != nil) {
		mFocusBox->Hide();
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------
//	ListBox is becoming visible

void
LListBox::ShowSelf()
{
	FocusDraw();					// Turn on ListManager drawing mode
	StClipRgnState	saveClip(nil);	//   but suppress immediate redraw
	::LSetDrawingMode(true, mMacListH);

	if (IsOnDuty() && (mFocusBox != nil)) {
		mFocusBox->Show();			// Show FocusBox if we are the Target
	}
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	ListBox is becoming invisible

void
LListBox::HideSelf()
{
	FocusDraw();					// Turn off ListManager drawing mode
	::LSetDrawingMode(false, mMacListH);

	if (IsOnDuty()) {				// Shouldn't be on duty when invisible
		SwitchTarget(GetSuperCommander());
	}
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf
// ---------------------------------------------------------------------------
//	Activate ListBox. The Toolbox shows the selection and scroll bars.

void
LListBox::ActivateSelf()
{
	FocusDraw();
	::LActivate(true, mMacListH);
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf
// ---------------------------------------------------------------------------
//	Deactivate ListBox. The Toolbox hides the selection and scroll bars.

void
LListBox::DeactivateSelf()
{
	FocusDraw();
	::LActivate(false, mMacListH);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//	Both PowerPlant and the List Manager store a size for the ListBox,
//	so we must make sure that both sizes are in synch.

void
LListBox::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	FocusDraw();
									// Prevent List and Control Managers
									// from automatically drawing by setting
	StClipRgnState	saveClip(nil);	// an empty clipping region

									// Get current size of ListBox
	Rect	displayRect = (**mMacListH).rView;
	Rect	bounds = (**mMacListH).dataBounds;

		// If the cell width is the default width, adjust the cell
		// width so that it will remain at the default width for the
		// new width of the ListBox. The default width is the width
		// of the ListBox divided by the number of columns.

	if (bounds.right > bounds.left) {
		SInt16	defaultWidth = (SInt16) ((displayRect.right - displayRect.left) /
									(bounds.right - bounds.left));

		if ((**mMacListH).cellSize.h == defaultWidth) {
			Point	cellSize = (**mMacListH).cellSize;
			cellSize.h = (SInt16) ((displayRect.right - displayRect.left + inWidthDelta) /
									(bounds.right - bounds.left));
			::LCellSize(cellSize, mMacListH);
		}
	}

		// Resize viewing rectangle of Toolbox ListHandle

	::LSize((SInt16) (displayRect.right - displayRect.left + inWidthDelta),
			(SInt16) (displayRect.bottom - displayRect.top + inHeightDelta),
			mMacListH);

		// Toolbox ListManager seems to have a bug. When there is only
		// one scroll bar (vertical or horizontal), the scroll bar is
		// not indented if inHasGrow is true. As a workaround, we
		// manually set the scroll bar size.

	if (mHasGrow) {
		ControlHandle	vScroll = ::GetListVerticalScrollBar(mMacListH);
		ControlHandle	hScroll = ::GetListHorizontalScrollBar(mMacListH);

		Rect			controlBounds;
		Rect			viewBounds;

		if ((vScroll != nil) && (hScroll == nil)) {
									// Vertical Scroll Bar only
			::GetControlBounds(vScroll, &controlBounds);
			::GetListViewBounds(mMacListH, &viewBounds);

			controlBounds.bottom = (SInt16)(viewBounds.bottom - 14);

			::SetControlBounds(vScroll, &controlBounds);
		}

		if ((hScroll != nil) && (vScroll == nil)) {
									// Horizontal Scroll bar only
			::GetControlBounds(hScroll, &controlBounds);
			::GetListViewBounds(mMacListH, &viewBounds);

			controlBounds.right = (SInt16)(viewBounds.right - 14);

			::SetControlBounds(hScroll, &controlBounds);
		}
	}
									// Inherited Resize
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);

	if (mFocusBox != nil) {			// Resize FocusBox, too
		mFocusBox->ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//	Both PowerPlant and the List Manager store a location for the ListBox,
//	so we must make sure that both locations are in synch.

void
LListBox::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
									// Inherited MoveBy
	LPane::MoveBy(inHorizDelta, inVertDelta, inRefresh);

		// Unfortunately, the Toolbox does not have a call to
		// move a List, so we have to directly set the proper
		// fields of the ListHandle

		// Determine how far to offset ListHandle's view rectangle,
		// which we want to be the same as the Frame of the ListBox

	Rect	displayRect = (**mMacListH).rView;
	Rect	frame;
	CalcLocalFrameRect(frame);

	SInt16	horizMove = (SInt16) (frame.left - displayRect.left + 1);
	SInt16	vertMove  = (SInt16) (frame.top - displayRect.top + 1);

	if ((horizMove == 0) && (vertMove == 0)) {
		return;						// No need to move view rectangle
	}

									// Move view rectangle
	::MacOffsetRect(&(**mMacListH).rView, horizMove, vertMove);

									// Move Scroll Bars
									// Ugh. We directly offset the rectangle
									//   inside the ControlHandle instead
									//   of using MoveControl since we don't
									//   know what coordinate system the
									//   List Manager uses.

	ControlHandle	vScroll	= ::GetListVerticalScrollBar(mMacListH);
	ControlHandle	hScroll = ::GetListHorizontalScrollBar(mMacListH);

	Rect	controlBounds;

	if (vScroll != nil) {
		::GetControlBounds(vScroll, &controlBounds);
		::MacOffsetRect(&controlBounds, horizMove, vertMove);
		::SetControlBounds(vScroll, &controlBounds);
	}

	if (hScroll != nil) {
		::GetControlBounds(hScroll, &controlBounds);
		::MacOffsetRect(&controlBounds, horizMove, vertMove);
		::SetControlBounds(hScroll, &controlBounds);
	}

	if (mFocusBox != nil) {			// Move FocusBox, too
		mFocusBox->MoveBy(inHorizDelta, inVertDelta, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ SavePlace
// ---------------------------------------------------------------------------
//	Write size and location information to a Stream for later retrieval
//	by the RestorePlace() function

void
LListBox::SavePlace(
	LStream		*outPlace)
{
	LPane::SavePlace(outPlace);

									// Save Display rectangle
	Rect	theRect = (**mMacListH).rView;
	*outPlace << theRect;

									// Save Vertical ScrollBar rectangle
	ControlHandle	vScroll = ::GetListVerticalScrollBar(mMacListH);

	if (vScroll != nil) {
		::GetControlBounds(vScroll, &theRect);
		*outPlace << theRect;
	}


									// Save Horizontal ScrollBar rectangle
	ControlHandle	hScroll = ::GetListHorizontalScrollBar(mMacListH);

	if (hScroll != nil) {
		GetControlBounds(hScroll, &theRect);
		*outPlace << theRect;
	}
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace
// ---------------------------------------------------------------------------
//	Read size and location information stored in a Stream by the
//	SavePlace() function

void
LListBox::RestorePlace(
	LStream		*inPlace)
{
	LPane::RestorePlace(inPlace);

    								// Restore the Display rectangle
	Rect	theRect;
	*inPlace >> theRect;
	(**mMacListH).rView = theRect;

									// Resize the List
	::HidePen();
	::LSize((SInt16) (theRect.right - theRect.left),
			(SInt16) (theRect.bottom - theRect.top), mMacListH);
	::ShowPen();

									// Restore Vertical ScrollBar rectangle

	ControlHandle	vScroll = ::GetListVerticalScrollBar(mMacListH);
	if (vScroll != nil) {
		*inPlace >> theRect;
		::SetControlBounds(vScroll, &theRect);
	}

									// Restore Horizontal ScrollBar rectangle
	ControlHandle	hScroll = ::GetListHorizontalScrollBar(mMacListH);
	if (hScroll != nil) {
		*inPlace >> theRect;
		::SetControlBounds(hScroll, &theRect);
	}
}


PP_End_Namespace_PowerPlant
