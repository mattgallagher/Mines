// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditableOutlineItem.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LEditableOutlineItem.h>
#include <LInPlaceEditField.h>
#include <LOutlineTable.h>
#include <PP_KeyCodes.h>

PP_Begin_Namespace_PowerPlant

UInt32		LEditableOutlineItem::sInPlaceEditTickCount;
Rect		LEditableOutlineItem::sInPlaceEditTextFrame;
STableCell	LEditableOutlineItem::sInPlaceEditCell;


// ===========================================================================
//	LOutlineEditFieldWatcher
// ===========================================================================
//	A private utility class used by LEditableOutlineItem.

#pragma mark *** LOutlineEditFieldWatcher ***

class LOutlineEditFieldWatcher : public LAttachment {

protected:
						LOutlineEditFieldWatcher(LEditableOutlineItem* inItem)
								{ mItem = inItem; }
	virtual				~LOutlineEditFieldWatcher()
								{ mItem->StopInPlaceEdit(); }

	LEditableOutlineItem* mItem;

	friend class LEditableOutlineItem;

};

// ===========================================================================

#pragma mark -
#pragma mark *** LEditableOutlineItem ***

// ---------------------------------------------------------------------------
//	¥ LEditableOutlineItem					Constructor
// ---------------------------------------------------------------------------

LEditableOutlineItem::LEditableOutlineItem()
{
	mEditField = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LEditableOutlineItem					Destructor
// ---------------------------------------------------------------------------

LEditableOutlineItem::~LEditableOutlineItem()
{
	if (mEditField != nil) {
		mEditField->Hide();		// it will go away with the window, if not before
		mEditField->RemoveAllAttachments();
	}
}


// ===========================================================================

#pragma mark -
#pragma mark ** in-place editing accessors

// ---------------------------------------------------------------------------
//	¥ CanDoInPlaceEdit
// ---------------------------------------------------------------------------
//	Return true if this cell can support in-place editing.

Boolean
LEditableOutlineItem::CanDoInPlaceEdit(
	const STableCell&	inCell)
{
	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	return drawInfo.outCanDoInPlaceEdit;
}


// ---------------------------------------------------------------------------
//	¥ StartInPlaceEdit
// ---------------------------------------------------------------------------
//	Starts in-place editing on this cell immediately.

void
LEditableOutlineItem::StartInPlaceEdit(
	const STableCell&	inCell)
{

	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Record which cell is being edited.

	sInPlaceEditCell = inCell;

	// Make sure there's a valid location for edit field even if the text is empty.

	if (drawInfo.outTextString[0] == 0) {
		drawInfo.outTextString[0] = 1;
		drawInfo.outTextString[1] = char_Space;
		PrepareDrawContents(inCell, drawInfo);
		drawInfo.outTextString[0] = 0;
	}

	// Calculate bounds for edit field.

	Rect editRect;
	editRect.top    = (SInt16) (drawInfo.prTextFrame.top + 1);
	editRect.bottom = (SInt16) (drawInfo.prTextFrame.bottom - 1);
	editRect.left   = (SInt16) (drawInfo.ioCellBounds.left + 2);
	editRect.right  = (SInt16) (drawInfo.ioCellBounds.right - 3);

	// Adjust bounds for icon if there is one.

	if (drawInfo.outHasIcon) {
		editRect.left += 22;
	}

	// Get edit field configuration.

	ResIDT textTraitsID = mOutlineTable->GetTextTraits();
	SInt16 maxChars = 255;
	UInt8 attributes = editAttr_Box | editAttr_AutoScroll | editAttr_InlineInput | editAttr_TextServices;
	TEKeyFilterFunc keyFilter = &UKeyFilters::PrintingCharField;

	if (drawInfo.outMultiLineText) {
		attributes |= editAttr_WordWrap;
		keyFilter = &UKeyFilters::PrintingCharAndCRField;
	}

	ConfigureInPlaceEdit(inCell, textTraitsID, maxChars, attributes, keyFilter);

	if (attributes & editAttr_Box)
		::MacInsetRect(&editRect, -2, -2);

	// Tell the table about the edit cell.

	sInPlaceEditCell = inCell;
	mOutlineTable->SetCurrentInPlaceEditCell(inCell);

	// Fill in default pane info.

	SPaneInfo paneInfo = { 0,							// paneID
					(SInt16) (editRect.right - editRect.left),	// width
					(SInt16) (editRect.bottom - editRect.top),	// height
					true,								// visible
					true,								// enabled
					{ false, false, false, false },		// bindings
					editRect.left,						// left
					editRect.top,						// top
					0,									// userCon
					mOutlineTable };					// superView

	// Create edit field.

	mEditField = new LInPlaceEditField(
					paneInfo,							// inPaneInfo
					drawInfo.outTextString,				// inString
					textTraitsID,						// inTextTraitsID
					maxChars,							// inMaxChars
					attributes,							// inAttributes
					keyFilter,							// inKeyFilter
					LCommander::GetTarget());			// inSuper

	ThrowIfNil_(mEditField);
	mEditField->FinishCreate();
	mEditField->UserChangedText();
	LCommander::SwitchTarget(mEditField);

	// Add attachment so we know when edit field goes away.

	mEditField->AddAttachment(new LOutlineEditFieldWatcher(this));

}

// ===========================================================================

#pragma mark -
#pragma mark ** in-place editing implementation

// ---------------------------------------------------------------------------
//	¥  SingleClick												   [protected]
// ---------------------------------------------------------------------------
//	Called by TrackContentClick when the user single-clicks on the
//	content of the cell. If the text was hit and the outCanDoInPlaceEdit
//	flag in the draw contents struct was set, triggers in-place editing
//	after one of the following conditions are met:
//
//		1. 60 ticks elapse with no other mouse action
//		2. mouse moves outside this cell
//		3. the tab or return key is pressed

void
LEditableOutlineItem::SingleClick(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown,
	const SOutlineDrawContents&	inDrawContents,
	Boolean					inHitText)
{

	// If mouse didn't go down in text (or text isn't editable), no in-place editing.

	if ((!inHitText) || !(inDrawContents.outCanDoInPlaceEdit))
		return;

	// Start in-place editing after 60 ticks (1 second).

	sInPlaceEditTickCount = inMouseDown.macEvent.when;
	sInPlaceEditCell = inCell;

	// Record location of text to be edited. If mouse moves outside this area,
	// we start in-place editing immediately. Record the location in global
	// coordinates to speed testing in the SpendTime method.

	sInPlaceEditTextFrame = inDrawContents.prTextFrame;

	mOutlineTable->LocalToPortPoint(topLeft(sInPlaceEditTextFrame));
	mOutlineTable->LocalToPortPoint(botRight(sInPlaceEditTextFrame));

	mOutlineTable->PortToGlobalPoint(topLeft(sInPlaceEditTextFrame));
	mOutlineTable->PortToGlobalPoint(botRight(sInPlaceEditTextFrame));

	// We use the repeater mechanism to watch events until one of these
	// conditions is satisfied.

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ SpendTime													   [protected]
// ---------------------------------------------------------------------------
//	If SingleClick determines that in-place editing should be started,
//	the outline item is made into a repeater. SpendTime checks the
//	timeout and mouse location and triggers in-place editing if
//	conditions are appropriate.

void
LEditableOutlineItem::SpendTime(
	const EventRecord&		inMacEvent)
{

	// If a key-down or mouse-down event occurs, adbandon in-place editing.

	switch (inMacEvent.what) {
		case mouseDown:
			if (inMacEvent.when == sInPlaceEditTickCount)
				break;
		case keyDown:
			StopRepeating();
			return;
	}

	// Check mouse location. If mouse moved outside the cell, edit now.

	if (!::MacPtInRect(inMacEvent.where, &sInPlaceEditTextFrame)) {
		StartInPlaceEdit(sInPlaceEditCell);
		StopRepeating();
		return;
	}

	// Check timeout. If 60 ticks have elapsed, edit now.

	if (inMacEvent.when >= sInPlaceEditTickCount + 60) {
		StartInPlaceEdit(sInPlaceEditCell);
		StopRepeating();
	}
}


// ---------------------------------------------------------------------------
//	¥ ConfigureInPlaceEdit										   [protected]
// ---------------------------------------------------------------------------
//	Override hook. Override if you wish to configure the text traits,
//	maximum characters, edit field attributes, or key filter of the
//	in-place edit field.

void
LEditableOutlineItem::ConfigureInPlaceEdit(
	const STableCell&	/* inCell */,
	ResIDT&				/* outTextTraitsID */,
	SInt16&				/* outMaxChars */,
	UInt8&				/* outAttributes */,
	TEKeyFilterFunc&	/* outKeyFilter */)
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ StopInPlaceEdit											   [protected]
// ---------------------------------------------------------------------------
//	Called when the in-place edit field goes away.

void
LEditableOutlineItem::StopInPlaceEdit()
{
	mEditField = nil;

	TableIndexT rows, cols;
	mOutlineTable->GetTableSize(rows, cols);
	mOutlineTable->RefreshCell(sInPlaceEditCell);

	STableCell emptyCell(0, 0);
	mOutlineTable->SetCurrentInPlaceEditCell(emptyCell);
}


// ---------------------------------------------------------------------------
//	¥ PrepareDrawContents										   [protected]
// ---------------------------------------------------------------------------
//	Overriden to hide the text being edited while
//	the in-place edit field is active.

void
LEditableOutlineItem::PrepareDrawContents(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{

	// If there's an in-place editor for this row, we
	// hide the text for the cell being edited.

	if (mEditField != nil) {

		// Hide text if this cell is being edited.

		if (sInPlaceEditCell == inCell) {

			if (mEditField != nil) {
				if (!mEditField->ClosingEditField())
					ioDrawContents.outTextString[0] = 0;
			}
		}
	}

	// Now do normal preparation.

	LOutlineItem::PrepareDrawContents(inCell, ioDrawContents);
}


PP_End_Namespace_PowerPlant
