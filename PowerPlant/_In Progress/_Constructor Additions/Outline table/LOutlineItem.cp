// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineItem.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOutlineItem.h>
#include <LOutlineTable.h>

#include <LArray.h>
#include <LArrayIterator.h>
#include <LDragAndDrop.h>
#include <LString.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UEventMgr.h>

#include <UNewTextDrawing.h>

#include <Drag.h>
#include <Events.h>
#include <Icons.h>
#include <LowMem.h>
#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	Disclosure triangle icons
// ===========================================================================

Handle LOutlineItem::sIconHierCondensed		= nil;
Handle LOutlineItem::sIconHierExpanded		= nil;
Handle LOutlineItem::sIconHierBeingFlipped	= nil;


// ===========================================================================
//	Resource IDs
// ===========================================================================

const ResIDT	ICN_HierCondensed		= 900;
const ResIDT	ICN_HierExpanded		= 901;
const ResIDT	ICN_HierBeingFlipped	= 902;


// ---------------------------------------------------------------------------
//	¥ LOutlineItem							Default Constructor		  [pulbic]
// ---------------------------------------------------------------------------

LOutlineItem::LOutlineItem()
{
	mOutlineTable	= nil;
	mSuperItem		= nil;
	mSubItems		= nil;

	mIndentSize		= 22;
	mIndentDepth	= 0;
	mLeftEdge		= 0;
	mExpanded		= false;
	mFlipping		= false;

	if (sIconHierCondensed == nil) {
		::GetIconSuite(&sIconHierCondensed, ICN_HierCondensed, svAllSmallData);
		::GetIconSuite(&sIconHierExpanded, ICN_HierExpanded, svAllSmallData);
		::GetIconSuite(&sIconHierBeingFlipped, ICN_HierBeingFlipped, svAllSmallData);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LOutlineItem							Destructor				  [public]
// ---------------------------------------------------------------------------
// 	Do not delete an LOutlineItem directly!
//	Use LOutlineTable::RemoveItem instead!

LOutlineItem::~LOutlineItem()
{
	delete mSubItems;
}

#pragma mark -
#pragma mark ** accessors

// ---------------------------------------------------------------------------
//	¥ DeepCountSubItems
// ---------------------------------------------------------------------------
//	Return the total number of subitems contained by this item,
//	including those which are nested in other subitems.

UInt32
LOutlineItem::DeepCountSubItems() const
{

	UInt32 count = 0;

	if (mSubItems != nil) {
		LArrayIterator iter(*mSubItems);
		LOutlineItem* item;

		while (iter.Next(&item)) {
			count += 1 + item->DeepCountSubItems();
		}
	}

	return count;
}

#pragma mark -
#pragma mark ** disclosure triangle

// ---------------------------------------------------------------------------
//	¥ Expand
// ---------------------------------------------------------------------------
//	Open the disclosure triangle for this item and show all subitems.
//	NOTE: You must override ExpandSelf to actually create the subitems.

void
LOutlineItem::Expand()
{
	if (mExpanded or not CanExpand()) {
		return;							// No need to expand
	}

										// Redraw the disclosure triangle
	mExpanded = true;
	RefreshDisclosureTriangle();

	// Call the override hook. ExpandSelf actually does the work
	// of creating subitems.

	ExpandSelf();

}


// ---------------------------------------------------------------------------
//	¥ DeepExpand
// ---------------------------------------------------------------------------
//	Expand this node and all subnodes. Typically called when the
//	disclosure triangle is clicked with the option key down.

void
LOutlineItem::DeepExpand()
{
	Expand();							// Expand this row.

										// Deep-expand all subitems.
	if (mSubItems != nil) {
		LArrayIterator iter(*mSubItems);
		LOutlineItem* item;

		while (iter.Next(&item)) {
			item->DeepExpand();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Collapse
// ---------------------------------------------------------------------------
//	Remove all subitems of this item.

void
LOutlineItem::Collapse()
{

	// Redraw the disclosure triangle.

	mExpanded = false;
	RefreshDisclosureTriangle();

	// Allow subclass to collapse data model.

	CollapseSelf();

	// Remove the subitems.

	if (mSubItems != nil) {
		LArrayIterator iter(*mSubItems);
		LOutlineItem* item;

		while (iter.Next(&item)) {
			mOutlineTable->RemoveItem(item, true, false);
		}
		mOutlineTable->AdjustImageSize(true);
	}
}


// ---------------------------------------------------------------------------
//	¥ CanExpand
// ---------------------------------------------------------------------------
//	Return true if this item can have subitems (i.e. it should have
//	a disclosure triangle.) Default always return false. Override if
//	you have subitems to display.

Boolean
LOutlineItem::CanExpand() const
{
	return false;				// override hook
}


// ---------------------------------------------------------------------------
//	¥ RefreshDisclosureTriangle
// ---------------------------------------------------------------------------
//	Cause the disclosure triangle for this item to be redrawn.
//	Ignored if there is no disclosure triangle.

void
LOutlineItem::RefreshDisclosureTriangle()
{
	if (mOutlineTable != nil) {
		Rect frame;
		if (CalcLocalDisclosureTriangleRect(frame)) {
			mOutlineTable->RefreshRect(frame);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalDisclosureTriangleRect
// ---------------------------------------------------------------------------
//	Determine the location of the disclosure triangle in local
//	coordinates of the table view. By default, the disclosure triangle
//	is located two pixels from the left of the first cell in the table.
//	Override if you wish different placement.
//
//	Returns true if the triangle can be found in 16-bit coordinates,
//	false if not.

Boolean
LOutlineItem::CalcLocalDisclosureTriangleRect(
	Rect&	outTriangleRect) const
{
	Boolean		isValid = false;

	STableCell cell(FindRowForItem(), 1);

	if ( (cell.row != 0)  &&
		 mOutlineTable->GetLocalCellRect(cell, outTriangleRect) ) {

		SInt16 mid = (SInt16) ((outTriangleRect.top + outTriangleRect.bottom) >> 1);

		outTriangleRect.top		= (SInt16) (mid - 8);
		outTriangleRect.bottom	= (SInt16) (mid + 8);
		outTriangleRect.left   += 2;
		outTriangleRect.right	= (SInt16) (outTriangleRect.left + 16);

		isValid = true;
	}

	return isValid;
}

#pragma mark -
#pragma mark ** row display helpers

// ---------------------------------------------------------------------------
//	¥ FindRowForItem
// ---------------------------------------------------------------------------
//	Find the table row index that corresponds to this item.

TableIndexT
LOutlineItem::FindRowForItem() const
{
	return mOutlineTable->FindRowForItem(this);
}


// ---------------------------------------------------------------------------
//	¥ ScrollItemIntoFrame
// ---------------------------------------------------------------------------
//	Scroll the table as necessary to ensure that this item is visible.

void
LOutlineItem::ScrollItemIntoFrame()
{
	STableCell cell;
	cell.row = FindRowForItem();
	cell.col = 1;

	mOutlineTable->ScrollCellIntoFrame(cell);
}


// ---------------------------------------------------------------------------
//	¥ RefreshSelection
// ---------------------------------------------------------------------------
//	Generate an update event for any part of this cell which shows
//	selection (i.e. highlighted icons or text).

void
LOutlineItem::RefreshSelection(
	const STableCell& inCell)
{
	SOutlineDrawContents drawInfo;		// Find out what to draw here
	GetDrawContents(inCell, drawInfo);

	if (drawInfo.outShowSelection) {	// Skip if no selection

										// Get port origin so we can convert
										//   from local to  port coords
		Point	portOrigin;
		mOutlineTable->GetPortOrigin(portOrigin);

										// If there's an icon, refresh it

		if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil)) {
			StRegion	iconRgn;

			::IconSuiteToRgn(iconRgn, &drawInfo.prIconFrame,
							 (SInt16) drawInfo.outIconAlign, drawInfo.outIconSuite);

			mOutlineTable->RefreshRgn(iconRgn);
		}

										// If there's text, refresh it
		mOutlineTable->RefreshRect(drawInfo.prTextFrame);
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeDragRegion
// ---------------------------------------------------------------------------
//	Can be called by outline table to make a gray region for dragging.

void
LOutlineItem::MakeDragRegion(
	const STableCell&	inCell,
	RgnHandle			outGlobalDragRgn,
	Rect&				outGlobalItemBounds)
{
	// Make sure this cell is visible.

	Rect ignore;
	if (!mOutlineTable->GetLocalCellRect(inCell, ignore))
		return;

	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Get a temporary working region.

	StRegion	tempRgn;

	// If there's an icon, add it to the region.

	if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil) && !::EmptyRect(&drawInfo.prIconFrame)) {
		Rect globalIconRect = drawInfo.prIconFrame;

		mOutlineTable->LocalToPortPoint(topLeft(globalIconRect));
		mOutlineTable->LocalToPortPoint(botRight(globalIconRect));
		mOutlineTable->PortToGlobalPoint(topLeft(globalIconRect));
		mOutlineTable->PortToGlobalPoint(botRight(globalIconRect));

		::IconSuiteToRgn(tempRgn, &globalIconRect, (SInt16) drawInfo.outIconAlign, drawInfo.outIconSuite);
		::MacUnionRgn(outGlobalDragRgn, tempRgn, outGlobalDragRgn);
	}

	// If there's text, add it to the region.

	if ((drawInfo.prTextWidth > 0) && !::EmptyRect(&drawInfo.prTextFrame)) {

		Rect globalTextRect = drawInfo.prTextFrame;

		mOutlineTable->LocalToPortPoint(topLeft(globalTextRect));
		mOutlineTable->LocalToPortPoint(botRight(globalTextRect));
		mOutlineTable->PortToGlobalPoint(topLeft(globalTextRect));
		mOutlineTable->PortToGlobalPoint(botRight(globalTextRect));

		tempRgn = globalTextRect;
		::MacUnionRgn(outGlobalDragRgn, tempRgn, outGlobalDragRgn);

		outGlobalItemBounds = globalTextRect;
	}
	else {
		outGlobalItemBounds = Rect_0000;
	}
}

#pragma mark -
#pragma mark ** row display helpers

// ---------------------------------------------------------------------------
//	¥ CanDoInPlaceEdit
// ---------------------------------------------------------------------------
//	Return true if this cell can do in-place editing. Always returns false
//	since the base LOutlineItem class doesn't know about editing.
//	LEditableOutlineItem overrides this function to return true when
//	appropriate.

Boolean
LOutlineItem::CanDoInPlaceEdit(
	const STableCell&	/* inCell */)
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ ShowsSelection
// ---------------------------------------------------------------------------
//	Return true if this cell shows a selection indication.

Boolean
LOutlineItem::ShowsSelection(
	const STableCell&	inCell)
{
	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	return drawInfo.outShowSelection;
}

#pragma mark -
#pragma mark ** override hooks

// ---------------------------------------------------------------------------
//	¥ FinishCreate												   [protected]
// ---------------------------------------------------------------------------
//	Called by the outline table once the item has been installed in the
//	table. Can be used for any initialization that requires a valid row
//	number to be availble.

void
LOutlineItem::FinishCreate()
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ UpdateRowSize												   [protected]
// ---------------------------------------------------------------------------
//	Called when the item is installed in the table. This override hook
//	allows you to update the row size for this item if needed.

void
LOutlineItem::UpdateRowSize()
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ CalcRowHeightForCell										   [protected]
// ---------------------------------------------------------------------------
//	Calculate the optimal row height for the text in this row.

SInt16
LOutlineItem::CalcRowHeightForCell(
	const STableCell&	inCell)
{

	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Measure the font height and ensure that
	// there's space for at least one line of text.

	mOutlineTable->FocusDraw();
	UTextTraits::SetPortTextTraits(&drawInfo.outTextTraits);

	FontInfo fi;
	::GetFontInfo(&fi);

	SInt16 minRowHeight = (SInt16) (fi.ascent + fi.descent + 2);

	// If drawing multi-line text, measure the entire size of the string.

	if ((drawInfo.outMultiLineText) && (drawInfo.outTextString[0] > 0)) {

		Rect maxSize = { 0, 0, 32767, (SInt16) (drawInfo.ioCellBounds.right - drawInfo.ioCellBounds.left - 6) };
		Rect textSize;

		UNewTextDrawing::MeasureWithJustification(
				(Ptr) &drawInfo.outTextString[1],
				drawInfo.outTextString[0],
				maxSize,
				drawInfo.outTextTraits.justification,
				textSize);

		if (minRowHeight < textSize.bottom + 3)
			minRowHeight = (SInt16) (textSize.bottom + 3);

	}

	return minRowHeight;

}


// ---------------------------------------------------------------------------
//	¥ GetDrawContentsSelf										   [protected]
// ---------------------------------------------------------------------------
//	Override to specify the contents of each cell. Most standard behaviors
//	(drawing, clicking, selecting) are handled automatically for you by
//	other methods of LOutlineItem. All you need to do is fill in fields
//	of the SOutlineDrawContents structure that is passed to this routine.
//
//	A typical GetDrawContentsSelf override will contain a switch statement
//	for the column number, then will assign to either the text string or
//	the icon field.
//
//	The default method provides row and cell number for debugging purposes.

void
LOutlineItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
#ifdef Debug_Signal			// Debugging code: Override to replace.
							//   Draws row, col number in each cell.

	LStr255 cellStr((SInt32) inCell.row);
	cellStr += StringLiteral_(", ");
	cellStr += LStr255((SInt32) inCell.col);
	LString::CopyPStr(cellStr, ioDrawContents.outTextString);

	ioDrawContents.outShowSelection = true;
	if (inCell.col > 1) {
		ioDrawContents.outTextTraits.justification = teJustRight;
	}

#else
	#pragma unused(inCell, ioDrawContents)
#endif
}


// ---------------------------------------------------------------------------
//	¥ DrawRowAdornments											   [protected]
// ---------------------------------------------------------------------------
//	Override if you need to draw some sort of row adornment (i.e.
//	background shading for the row). Called by LOutlineTable::DrawSelf.

void
LOutlineItem::DrawRowAdornments(
	const Rect&		/* inLocalRowRect */)
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ ExpandSelf												   [protected]
// ---------------------------------------------------------------------------
//	Override to create subitems. Called only by Expand() and only when
//	this item goes from the collapsed to the expanded state.

void
LOutlineItem::ExpandSelf()
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ CollapseSelf												   [protected]
// ---------------------------------------------------------------------------
//	Override if you need to know when this item becomes collapsed.
//	Called only by Collapse() and only when this item goes from the
//	expanded to the collapsed state.
//
//	NOTE: Collapse() will automatically remove all subitems. You do not
//	need to override CollapseSelf for this purpose.

void
LOutlineItem::CollapseSelf()
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ InsertCols												   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineView::InsertCols whenever columns are inserted.
//	Provided as an override hook.

void
LOutlineItem::InsertCols(
	UInt32			/* inHowMany */,
	TableIndexT		/* inAfterCol */,
	const void*		/* inDataPtr */,
	UInt32			/* inDataSize */,
	Boolean			/* inRefresh */)
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols												   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineView::RemoveCols whenever columns are removed.
//	Provided as an override hook.

void
LOutlineItem::RemoveCols(
	UInt32			/* inHowMany */,
	TableIndexT		/* inFromCol */,
	Boolean			/* inRefresh */)
{
	// override hook
}

#pragma mark -
#pragma mark ** cell contents

// ---------------------------------------------------------------------------
//	¥ GetDrawContents											   [protected]
// ---------------------------------------------------------------------------
//	Called by DrawCell and ClickCell to obtain a complete description
//	of the contents of this cell. Override GetDrawContentsSelf() to
//	customize the cell contents.

void
LOutlineItem::GetDrawContents(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	ResetDrawContents(inCell, ioDrawContents);
	GetDrawContentsSelf(inCell, ioDrawContents);
	PrepareDrawContents(inCell, ioDrawContents);
}


// ---------------------------------------------------------------------------
//	¥ ResetDrawContents											   [protected]
// ---------------------------------------------------------------------------
//	Called only by GetDrawContents(). Resets the fields of the
//	SOutlineDrawContents structure to reasonable defaults.

void
LOutlineItem::ResetDrawContents(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{

	// Calculcate cell bounds.

	mOutlineTable->FocusDraw();
	mOutlineTable->GetLocalCellRect(inCell, ioDrawContents.ioCellBounds);
	if (inCell.col == 1) {
		ioDrawContents.ioCellBounds.left += mLeftEdge;
		if (ioDrawContents.ioCellBounds.left > ioDrawContents.ioCellBounds.right)
			ioDrawContents.ioCellBounds.left = ioDrawContents.ioCellBounds.right;
	}

	// Reset basic drawing info.

	ioDrawContents.outShowSelection = false;

	// Reset icon drawing info.

	ioDrawContents.outHasIcon = false;
	ioDrawContents.outIconSuite = nil;
	ioDrawContents.outIconTransform = 0;
	ioDrawContents.outIconAlign = nil;
	ioDrawContents.outDoTruncation = true;

	// Reset text drawing info.

	ioDrawContents.outTextString[0] = 0;
	ioDrawContents.prTruncatedString[0] = 0;
	ioDrawContents.outCanDoInPlaceEdit = false;
	ioDrawContents.outMultiLineText = false;

	TextTraitsH traitsH = UTextTraits::LoadTextTraits(mOutlineTable->GetTextTraits());
	if (traitsH != nil)
		::BlockMoveData(*traitsH, &(ioDrawContents.outTextTraits), ::GetHandleSize((Handle) traitsH));
	else
		UTextTraits::LoadSystemTraits(ioDrawContents.outTextTraits);

	// Figure default baseline.
	// By default, the title is vertically centered within the cell.

	UTextTraits::SetPortTextTraits(&ioDrawContents.outTextTraits);

	FontInfo fi;
	::GetFontInfo(&fi);

	SInt16 midSelf = (SInt16) ((ioDrawContents.ioCellBounds.bottom + ioDrawContents.ioCellBounds.top) >> 1);
	SInt16 textHeight = (SInt16) (fi.ascent + fi.descent);
	SInt16 textTop = (SInt16) (midSelf - (textHeight >> 1));
	ioDrawContents.outTextBaseline = (SInt16) (textTop + fi.ascent);

}


// ---------------------------------------------------------------------------
//	¥ PrepareDrawContents										   [protected]
// ---------------------------------------------------------------------------
//	Called only by GetDrawContents. Truncates the text string to the
//	size of the cell. Calculates string width and placement.
//	Calculates icon placement.

void
LOutlineItem::PrepareDrawContents(
	const STableCell&		/* inCell */,
	SOutlineDrawContents&	ioDrawContents)
{

	// Make a copy of the string for truncation.

	LString::CopyPStr(ioDrawContents.outTextString, ioDrawContents.prTruncatedString);

	// Make sure the text will fit within the cell's bounds.

	if (ioDrawContents.prTruncatedString[0] > 0) {

		// Cache the string width to reduce calls to ::StringWidth().

		mOutlineTable->FocusDraw();
		UTextTraits::SetPortTextTraits(&ioDrawContents.outTextTraits);
		ioDrawContents.prTextWidth = ::StringWidth(ioDrawContents.prTruncatedString);

		// Calculate the outer limits of the text frame.

		ioDrawContents.prTextFrame = ioDrawContents.ioCellBounds;
		if (ioDrawContents.outHasIcon)
			ioDrawContents.prTextFrame.left += 22;

		// See if we're drawing single-line or multi-line text.

		if (ioDrawContents.outMultiLineText) {

			// Multi-line text. Measure the text, but do not truncate it.

			Rect maxRect = ioDrawContents.prTextFrame;
			::MacInsetRect(&maxRect, 3, 1);
			maxRect.top++;

			UNewTextDrawing::MeasureWithJustification(
					(Ptr) &ioDrawContents.outTextString[1],
					ioDrawContents.outTextString[0],
					maxRect,
					ioDrawContents.outTextTraits.justification,
					ioDrawContents.prTextFrame);

			::MacInsetRect(&ioDrawContents.prTextFrame, -3, -1);

		}
		else {

			// Single-line text. Truncate string if necessary.

			SInt16 maxWidth = (SInt16) (ioDrawContents.prTextFrame.right - ioDrawContents.prTextFrame.left - 6);
			if (maxWidth < 0)
				maxWidth = 0;

			// This looks redundant, but it actually saves time because StringWidth is fast (native?)
			// and TruncString is slow (emulated?).

			if ( ioDrawContents.prTextWidth > maxWidth  && ioDrawContents.outDoTruncation ) {
				::TruncString(maxWidth, ioDrawContents.prTruncatedString, truncEnd);
				ioDrawContents.prTextWidth = ::StringWidth(ioDrawContents.prTruncatedString);
			}

			// Convert default justification to system.

			if (ioDrawContents.outTextTraits.justification == teFlushDefault)
				ioDrawContents.outTextTraits.justification = ::GetSysDirection();

			// Create a text bounds that is used for hit testing and drawing.
			// Choose horizontal placement.

			ioDrawContents.prTextFrame.right = ioDrawContents.prTextFrame.left;
			if (ioDrawContents.prTextWidth)
				ioDrawContents.prTextFrame.right += ioDrawContents.prTextWidth + 6;

			SInt16 moveBy = 0;
			switch (ioDrawContents.outTextTraits.justification) {
				case teJustCenter:
					moveBy = (SInt16) ((ioDrawContents.ioCellBounds.right - ioDrawContents.prTextFrame.right) >> 1);
					break;
				case teJustRight:
					moveBy = (SInt16) (ioDrawContents.ioCellBounds.right - ioDrawContents.prTextFrame.right);
					break;
			}

			ioDrawContents.prTextFrame.left += moveBy;
			ioDrawContents.prTextFrame.right += moveBy;

			// Set vertical placement.

			FontInfo fi;
			::GetFontInfo(&fi);
			ioDrawContents.prTextFrame.top = (SInt16) (ioDrawContents.outTextBaseline - fi.ascent - 1);
			ioDrawContents.prTextFrame.bottom = (SInt16) (ioDrawContents.outTextBaseline + fi.descent + 1);

			// Constrain text frame to cell boundaries.
			// This ensures that highlighting won't be left behind as
			// selection changes.

			if (ioDrawContents.prTextFrame.top < ioDrawContents.ioCellBounds.top)
				ioDrawContents.prTextFrame.top = ioDrawContents.ioCellBounds.top;
			if (ioDrawContents.prTextFrame.bottom > ioDrawContents.ioCellBounds.bottom)
				ioDrawContents.prTextFrame.bottom = ioDrawContents.ioCellBounds.bottom;

		}
	}
	else {

		// No text, set width & frame to empty.

		ioDrawContents.prTextWidth = 0;
		ioDrawContents.prTextFrame = Rect_0000;
	}

	// Prepare icon frame. By default icon is drawn as a 16x16 icon
	// vertically centered and horizontally left-aligned in the cell.

	if (ioDrawContents.outHasIcon) {
		ioDrawContents.prIconFrame = ioDrawContents.ioCellBounds;
		ioDrawContents.prIconFrame.top = (SInt16) (((ioDrawContents.prIconFrame.top + ioDrawContents.prIconFrame.bottom) >> 1) - 8);
		ioDrawContents.prIconFrame.bottom = (SInt16) (ioDrawContents.prIconFrame.top + 16);
		ioDrawContents.prIconFrame.right = (SInt16) (ioDrawContents.prIconFrame.left + 16);
	}
	else {
		ioDrawContents.prIconFrame = Rect_0000;
	}
}

#pragma mark -
#pragma mark ** drawing

// ---------------------------------------------------------------------------
//	¥ DrawCell													   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineTable::DrawCell to draw the contents of a cell.
//	Can draw an icon and text as specified by the GetDrawContentsSelf
//	override hook. Override DrawCell only if you need to draw something else.

void
LOutlineItem::DrawCell(
	const STableCell&	inCell,
	const Rect&			inLocalCellRect)
{
	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Draw disclosure triangle for first column.

	if ((inCell.col == 1) && CanExpand()) {
		DrawDisclosureTriangle();
	}

	// If there's an icon, draw it.

	if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil)) {
		if (drawInfo.outShowSelection && mOutlineTable->CellIsSelected(inCell)) {
			drawInfo.outIconTransform |= ttSelected;
		}

		if (drawInfo.prIconFrame.right < inLocalCellRect.right) { // don't draw if rect is outside cell.
			::PlotIconSuite(&drawInfo.prIconFrame, (SInt16) drawInfo.outIconAlign,
							(SInt16) drawInfo.outIconTransform, drawInfo.outIconSuite);
		}
	}

	// If there's text, draw it.

	if (drawInfo.prTextWidth > 0) {

		// If text is selected, erase selection rectangle first.
		// This covers the case where the selection is drawn on top of
		// an area that is shaded.

		if (drawInfo.outShowSelection && mOutlineTable->CellIsSelected(inCell)) {
			RGBColor saveColor;
			::GetForeColor(&saveColor);
			::RGBForeColor(&Color_White);

			if (mOutlineTable->IsActive()) {
				::PaintRect(&drawInfo.prTextFrame);
			} else {
				::PenNormal();
				::MacFrameRect(&drawInfo.prTextFrame);
			}

			::RGBForeColor(&saveColor);
		}

		// Draw the text now.

		if (drawInfo.outMultiLineText) {

			// Multi-line text.

			Rect textRect = drawInfo.prTextFrame;
			::MacInsetRect(&textRect, 3, 1);
			textRect.right += 2;

			UTextDrawing::DrawWithJustification(
					(Ptr) &drawInfo.outTextString[1],
					drawInfo.outTextString[0],
					textRect,
					drawInfo.outTextTraits.justification,
					false);
		}
		else {

			// Single-line text. Just use QuickDraw.

			::MoveTo((SInt16) (drawInfo.prTextFrame.left + 3), drawInfo.outTextBaseline);
			::DrawString(drawInfo.prTruncatedString);
		}

		// If cell is selected, draw highlighting now.

		if (drawInfo.outShowSelection && mOutlineTable->CellIsSelected(inCell)) {
			StColorPenState::Normalize();
			::LMSetHiliteMode((UInt8) (::LMGetHiliteMode() & 0x7F));
			::MacInvertRect(&drawInfo.prTextFrame);
			if (!mOutlineTable->IsActive()) {
				Rect tempRect = drawInfo.prTextFrame;
				UDrawingUtils::SetHiliteModeOn();
				::MacInsetRect(&tempRect, 1, 1);
				::MacInvertRect(&tempRect);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawDisclosureTriangle									   [protected]
// ---------------------------------------------------------------------------
//	Draws the disclosure triangle for this row. Override this method
//	if you need to draw something other than an icon. Override
//	CalcLocalDisclosureTriangleRect if you need to adjust the placement
//	of the icon.

void
LOutlineItem::DrawDisclosureTriangle()
{

	// Find the location of the disclosure triangle.
	// If none, abort.

	Rect iconRect;
	if (!CalcLocalDisclosureTriangleRect(iconRect)) {
		return;
	}

	// Find the state of the dislosure triangle
	// and draw the appropriate icon.

	Handle iconSuite = IsExpanded() ? sIconHierExpanded: sIconHierCondensed;
	if (mFlipping) {
		iconSuite = sIconHierBeingFlipped;
	}

	if (iconSuite != nil) {
		::PlotIconSuite(&iconRect, kAlignNone, kTransformNone, iconSuite);
	}
}


// ---------------------------------------------------------------------------
//	¥ ShadeRow													   [protected]
// ---------------------------------------------------------------------------
//	Not actually used by LOutlineItem, but provided as a utility that
//	may be called from your DrawRowAdornments method. Shades the row
//	in a color of your choosing.

void
LOutlineItem::ShadeRow(
	const RGBColor&	inColor,
	const Rect&		inLocalRowRect)
{
	Rect shadeRect = inLocalRowRect;
	::MacInsetRect(&shadeRect, 1, 1);
	shadeRect.bottom += 1;

	mOutlineTable->FocusDraw();
	::RGBBackColor(&inColor);
	::EraseRect(&shadeRect);
}

#pragma mark -
#pragma mark ** clicking

// ---------------------------------------------------------------------------
//	¥ ClickCell													   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineTable::ClickCell whenever a mouse-down event is
//	detected within the frame of this cell. The default method handles
//	most behaviors correctly, based on information from your
//	GetDrawContentsSelf override.

void
LOutlineItem::ClickCell(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{

	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Test disclosure triangle for first column.

	if ((inCell.col == 1) && CanExpand()) {
		Rect disclosureFrame;
		CalcLocalDisclosureTriangleRect(disclosureFrame);
		if (::MacPtInRect(inMouseDown.whereLocal, &disclosureFrame)) {
			TrackDisclosureTriangle(inMouseDown);
			return;
		}
	}

	// If there's an icon, hit test it.

	if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil)) {
		if (::PtInIconSuite(inMouseDown.whereLocal, &drawInfo.prIconFrame, (SInt16) drawInfo.outIconAlign, drawInfo.outIconSuite)) {
			TrackContentClick(inCell, inMouseDown, drawInfo, false);
			return;
		}
	}

	// If there's text, hit test it.

	if (::MacPtInRect(inMouseDown.whereLocal, &drawInfo.prTextFrame)) {
		TrackContentClick(inCell, inMouseDown, drawInfo, true);
		return;
	}

	// Didn't hit anything, let outline table handle it.

	TrackEmptyClick(inCell, inMouseDown, drawInfo);
}


// ---------------------------------------------------------------------------
//	¥ TrackDisclosureTriangle									   [protected]
// ---------------------------------------------------------------------------
//	Called by ClickCell whenever a mouse-down is detected within the
//	disclosure triangle. Tracks the mouse movement until mouse-up and
//	expands or collapses this item as appropriate.

void
LOutlineItem::TrackDisclosureTriangle(
	const SMouseDownEvent& inMouseDown)
{

	// Decide where to draw disclosure triangle.

	mOutlineTable->FocusDraw();

	Rect disclosureFrame;
	CalcLocalDisclosureTriangleRect(disclosureFrame);

	Rect trackRect = disclosureFrame;
	::MacInsetRect(&trackRect, 1, 1);

	Boolean drawnSelected = false;
	Boolean wasExpanded = IsExpanded();

	// Continue tracking until the mouse is up.

	while (::StillDown()) {
		Point mouseLoc;
		::GetMouse(&mouseLoc);

		Boolean isInIcon = ::MacPtInRect(mouseLoc, &trackRect);
		if (isInIcon != drawnSelected) {
			::PlotIconSuite(&disclosureFrame, kAlignNone, isInIcon ? kTransformSelected : kTransformNone,
						wasExpanded ? sIconHierExpanded : sIconHierCondensed);
			drawnSelected = isInIcon;
		}
	}

	// Mouse released. Check mouse location on the mouse-up event.

	EventRecord macEvent;
	UEventMgr::GetMouseUp(macEvent);

	mOutlineTable->GlobalToPortPoint(macEvent.where);
	mOutlineTable->PortToLocalPoint(macEvent.where);
	drawnSelected = ::MacPtInRect(macEvent.where, &trackRect);

	// If it ended up in the control, flip it.

	if (drawnSelected) {

		// Draw the icon flipped.

		mFlipping = true;
		RefreshDisclosureTriangle();
		mOutlineTable->UpdatePort();

		// Wait 3 ticks.

		unsigned long ignore;
		::Delay(3, &ignore);

		// Change the expand status.

		mFlipping = false;
		if (wasExpanded)
			Collapse();
		else if (inMouseDown.macEvent.modifiers & optionKey)
			DeepExpand();
		else
			Expand();

		// Force a redraw.

		RefreshDisclosureTriangle();
		mOutlineTable->UpdatePort();
	}
}


// ---------------------------------------------------------------------------
//	¥ TrackContentClick											   [protected]
// ---------------------------------------------------------------------------
//	Called by ClickCell whenever a mouse-down event occurs within
//	the contents (icon or text) of this cell. Selects or deselects this
//	cell as appropriate and tests for dragging or double-clicking.

void
LOutlineItem::TrackContentClick(
	const STableCell&			inCell,
	const SMouseDownEvent&		inMouseDown,
	const SOutlineDrawContents&	inDrawContents,
	Boolean						inHitText)
{
	// If item isn't selected, try to select it now.

	Boolean wasSelected = mOutlineTable->CellIsSelected(inCell);
	if (!wasSelected) {
		if (!(inMouseDown.macEvent.modifiers & shiftKey))
			mOutlineTable->UnselectAllCells();
		mOutlineTable->SelectCell(inCell);
		mOutlineTable->UpdatePort();
	}

	// See if mouse moves. If it does, track a drag from it

	if (::StillDown() &&
		LDragAndDrop::DragAndDropIsPresent() &&
		::WaitMouseMoved(inMouseDown.macEvent.where)) {
		TrackDrag(inCell, inMouseDown, inDrawContents);
		return;
	}

	// Mouse didn't move, see if this was a double-click.

	if (LPane::GetClickCount() > 1) {
		DoubleClick(inCell, inMouseDown, inDrawContents, inHitText);
		return;
	}

	// If none of the above, and shift key was down, deselect this cell.

	if (wasSelected && (inMouseDown.macEvent.modifiers & shiftKey)) {
		mOutlineTable->UnselectCell(inCell);
		mOutlineTable->UpdatePort();
		return;
	}

	// Simple click and item is still selected, call it a single click.

	if (mOutlineTable->CellIsSelected(inCell)) {
		SingleClick(inCell, inMouseDown, inDrawContents, inHitText);
		return;
	}
}


// ---------------------------------------------------------------------------
//	¥ TrackEmptyClick											   [protected]
// ---------------------------------------------------------------------------
//	Called by ClickCell whenever a mouse-down event occurs within
//	the frame of the cell, but outside the content (icon, text, or
//	disclosure triangle).
//
//	By default, it passes the mouse event back to the table which will
//	either deselect all cells or perform a drag selection.

void
LOutlineItem::TrackEmptyClick(
	const STableCell&			inCell,
	const SMouseDownEvent&		inMouseDown,
	const SOutlineDrawContents&	inDrawContents)
{
	mOutlineTable->TrackEmptyClick(inCell, inMouseDown, inDrawContents);
}


// ---------------------------------------------------------------------------
//	¥ TrackDrag													   [protected]
// ---------------------------------------------------------------------------
//	Called by TrackContentClick when a drag is originated from within
//	the content of this cell.

void
LOutlineItem::TrackDrag(
	const STableCell&			inCell,
	const SMouseDownEvent&		inMouseDown,
	const SOutlineDrawContents&	/* inDrawContents */)
{
	if (mOutlineTable != nil) {
		mOutlineTable->TrackDrag(inCell, inMouseDown);
	}
}


// ---------------------------------------------------------------------------
//	¥ SingleClick												   [protected]
// ---------------------------------------------------------------------------
//	Called by TrackContentClick when the user single-clicks on the
//	content of this cell (text or icon). This is an override hook.
//	No default behavior is provided.

void
LOutlineItem::SingleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	// override hook
}


// ---------------------------------------------------------------------------
//	¥ DoubleClick												   [protected]
// ---------------------------------------------------------------------------
//	Called by TrackContentClick when the user double-clicks on the
//	content of this cell (text or icon). This is an override hook.
//	No default behavior is provided.

void
LOutlineItem::DoubleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	// override hook
}

#pragma mark -
#pragma mark ** subitem list

// ---------------------------------------------------------------------------
//	¥ AddSubItem												   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineTable::InsertItem whenever a new outline item is
//	inserted as a child of this item.

void
LOutlineItem::AddSubItem(
	LOutlineItem*	inSubItem,
	LOutlineItem*	inAfterItem)
{

	// Make sure there's a subitem list.

	if (mSubItems == nil) {
		mSubItems = new LArray(sizeof (LOutlineItem*));
	}

	// Add this item to the list.

	mSubItems->InsertItemsAt(1, mSubItems->FetchIndexOf(&inAfterItem) + 1, &inSubItem);

	// Update the subitem's nesting depth.

	inSubItem->mIndentDepth = (UInt16) (mIndentDepth + 1);
}


// ---------------------------------------------------------------------------
//	¥ RemoveSubItem												   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineTable::RemoveItem whenever an outline item that
//	is a child of this item is removed from the table.

void
LOutlineItem::RemoveSubItem(
	LOutlineItem*	inSubItem)
{
	if (mSubItems != nil) {			// Remove the item from our subitem list
		mSubItems->Remove(&inSubItem);
	}
}

#pragma mark -
#pragma mark ** drag selection

// ---------------------------------------------------------------------------
//	¥ CellHitByMarquee											   [protected]
// ---------------------------------------------------------------------------
//	Called by LOutlineTable::CellHitByMarquee to determine if this cell
//	is hit by the current selection marquee.

Boolean
LOutlineItem::CellHitByMarquee(
	const STableCell&	inCell,
	const Rect&			inMarqueeLocalRect)
{

	// Find out what's drawn here.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// If there's text, hit test it.

	Rect scratchRect;
	if (::SectRect(&inMarqueeLocalRect, &drawInfo.prTextFrame, &scratchRect)) {
		return true;
	}

	// Test disclosure triangle for first column.

	if ((inCell.col == 1) && CanExpand()) {
		Rect disclosureFrame;
		CalcLocalDisclosureTriangleRect(disclosureFrame);
		::MacInsetRect(&disclosureFrame, 1, 1);
		if (::SectRect(&inMarqueeLocalRect, &disclosureFrame, &scratchRect)) {
			return true;
		}
	}

	// If there's an icon, hit test it.

	if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil)) {
		if (::SectRect(&inMarqueeLocalRect, &drawInfo.prIconFrame, &scratchRect)) {
			return true;
		}
	}

	// Didn't hit anything.

	return false;
}


PP_End_Namespace_PowerPlant
