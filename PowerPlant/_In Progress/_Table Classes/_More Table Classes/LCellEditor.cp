// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellEditor.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple class for editing any textual data in a table, used by
//	LTableEditableSelector. Much of this is copied from LEditableOutlineItem.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTableEditableSelector.h>
#include <LCellEditor.h>
#include <LTextTableView.h>
#include <LInPlaceEditField.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LEditableOutlineItem()
// ---------------------------------------------------------------------------
//	Constructor

LCellEditor::LCellEditor()
{
	mEditField = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LEditableOutlineItem
// ---------------------------------------------------------------------------
//	Destructor

LCellEditor::~LCellEditor()
{
	if (mEditField != nil) {
		mEditField->Hide();		// it will go away with the window, if not before
		mEditField->RemoveAllAttachments();
	}
}


/*
// ---------------------------------------------------------------------------
//	¥ StartInPlaceEdit
// ---------------------------------------------------------------------------
//	Starts in-place editing on this cell immediately.

void
LCellEditor::StartInPlaceEdit(
	const STableCell&	inCell)
{

	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	Rect editRect;
	editRect.top = drawInfo.prTextFrame.top + 1;
	editRect.bottom = drawInfo.prTextFrame.bottom - 1;
	editRect.left = drawInfo.ioCellBounds.left + 2;
	editRect.right = drawInfo.ioCellBounds.right - 3;

	if (drawInfo.outHasIcon)
		editRect.left += 22;

	// Get edit field configuration.

	ResIDT textTraitsID = mOutlineTable->GetTextTraits();
	SInt16 maxChars = 255;
	UInt8 attributes = editAttr_Box | editAttr_WordWrap | editAttr_AutoScroll | editAttr_InlineInput | editAttr_TextServices;
	NewKeyFilterFunc keyFilter = UNewKeyFilters::PrintingCharField;

	ConfigureInPlaceEdit(inCell, textTraitsID, maxChars, attributes, keyFilter);

	if (attributes & editAttr_Box)
		::MacInsetRect(&editRect, -2, -2);

	// Fill in default pane info.

	SPaneInfo paneInfo = { 0,							// paneID
					editRect.right - editRect.left,		// width
					editRect.bottom - editRect.top,		// height
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

	// Tell the table about the edit cell.

// 	mOutlineTable->SetCurrentInPlaceEditCell(inCell);

	// Add attachment so we know when edit field goes away.

	mEditField->AddAttachment(new LCellEditFieldWatcher(this));

}

*/

PP_End_Namespace_PowerPlant
