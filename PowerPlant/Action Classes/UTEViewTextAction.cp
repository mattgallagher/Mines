// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTEViewTextAction.cp	   	PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Undoable Actions associated with a Toolbox TextEdit record
//
//	Reworked for the undo and autoscrolling mechanisms of LTextEditView

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UTEViewTextAction.h>
#include <LCommander.h>
#include <LPane.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UMemoryMgr.h>
#include <UScrap.h>

#include <LTextEditView.h>

#include <Fonts.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTEStyleTextAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEStyleTextAction::LTEStyleTextAction(
	SInt16			inDescriptionIndex,
	CommandT		inActionCommand,
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane,
	bool			inAlreadyDone)

	: LAction(STRx_RedoEdit, inDescriptionIndex, inAlreadyDone)
{
	mActionCommand	= inActionCommand;
	mMacTEH			= inMacTEH;
	mTextCommander	= inTextCommander;
	mTEView			= dynamic_cast<LTextEditView*>(inTextPane);
	ThrowIfNil_(mTEView);

		// Save current selection range and the selected text,
		// which will be deleted when doing the Action.

	mSelStart		= (*inMacTEH)->selStart;
	mSelEnd			= (*inMacTEH)->selEnd;
	mDeletedTextLen	= mSelEnd - mSelStart;
	mDeletedTextH	= ::NewHandle(mDeletedTextLen);

		// If we can't allocate the memory to save the selected
		// text, we don't throw. We still want to "do" the Action,
		// but it won't be undoable. mDeletedTextH being nil will
		// mark this Action as not postable (undoable). When
		// PostAction() is called, this Action will be "done" and
		// then deleted.

	if (mDeletedTextH != nil) {
		::BlockMoveData(*((**inMacTEH).hText) + mSelStart, *mDeletedTextH,
							mDeletedTextLen);
	}

	mDeletedStyleH = ::TEGetStyleScrapHandle(inMacTEH);
}


// ---------------------------------------------------------------------------
//	¥ ~LTEStyleTextAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEStyleTextAction::~LTEStyleTextAction()
{
	if (mDeletedTextH != nil) {
		::DisposeHandle(mDeletedTextH);
	}

	if (mDeletedStyleH != nil) {
		::DisposeHandle((Handle)mDeletedStyleH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Redo
// ---------------------------------------------------------------------------

void
LTEStyleTextAction::Redo()
{
	if (CanRedo()) {
		RedoSelf();
		mTextCommander->ProcessCommand(mActionCommand, this);
	}

	mIsDone = true;
}


// ---------------------------------------------------------------------------
//	¥ Undo
// ---------------------------------------------------------------------------

void
LTEStyleTextAction::Undo()
{
	if (CanUndo()) {
		UndoSelf();
		mTextCommander->ProcessCommand(mActionCommand, this);
	}

	mIsDone = false;
}


// ---------------------------------------------------------------------------
//	¥ CanRedo
// ---------------------------------------------------------------------------

Boolean
LTEStyleTextAction::CanRedo() const
{
	return (!IsDone() && mTextCommander->IsOnDuty());
}


// ---------------------------------------------------------------------------
//	¥ CanUndo
// ---------------------------------------------------------------------------

Boolean
LTEStyleTextAction::CanUndo() const
{
	return (IsDone() && mTextCommander->IsOnDuty());
}


// ---------------------------------------------------------------------------
//	¥ IsPostable
// ---------------------------------------------------------------------------

Boolean
LTEStyleTextAction::IsPostable() const
{
		// Action is not postable (undoable) if we could not
		// allocate space to store the text that was deleted

	return (mDeletedTextH != nil);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEStyleTextAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);
										// Restore deleted text
	::TESetSelect(mSelStart, mSelStart, mMacTEH);
	StHandleLocker	lock(mDeletedTextH);
	mTEView->Insert(*mDeletedTextH, mDeletedTextLen, mDeletedStyleH);
										// Restore original selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewCutAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewCutAction::LTEViewCutAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTEStyleTextAction(str_Cut, cmd_ActionCut, inMacTEH,
							inTextCommander, inTextPane)
{
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewCutAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);

	Rect theOldRect = (*mMacTEH)->destRect;

	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	::TEDelete(mMacTEH);

										// Put deleted text on clipboard
	UScrap::SetData(ResType_Text, mDeletedTextH);

	if (mDeletedStyleH != nil) {		// Add style info to clipboard
		UScrap::SetData(ResType_TextStyle, (Handle) mDeletedStyleH, false);
	}

	mTEView->ForceAutoScroll(theOldRect);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewPasteAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewPasteAction::LTEViewPasteAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTEStyleTextAction(str_Paste, cmd_ActionPaste, inMacTEH,
							inTextCommander, inTextPane)
{
										// Get text from clipbboard
	StHandleBlock	textH(Size_Zero);
	UScrap::GetData(ResType_Text, textH);

										// Get style from clipboard
	StHandleBlock	styleH(Size_Zero);
	SInt32			styleSize = UScrap::GetData(ResType_TextStyle, styleH);

										// Getting info from clipboard
										//   succeeded. Store Handles.
	mPastedTextH  = textH.Release();

	mPastedStyleH = nil;				// If there is no style data, we
										//   must use a nil Handle rather
										//   than a zero-sized Handle
	if (styleSize > 0) {
		mPastedStyleH = styleH.Release();
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewPasteAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewPasteAction::~LTEViewPasteAction()
{
	if (mPastedTextH != nil) {
		::DisposeHandle(mPastedTextH);
	}

	if (mPastedStyleH != nil) {
		::DisposeHandle(mPastedStyleH);
	}
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewPasteAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);

	Rect theOldRect = (*mMacTEH)->destRect;

	if (mSelStart != mSelEnd) {
		::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	}

	::TEDelete(mMacTEH);
	StHandleLocker	lock(mPastedTextH);
	mTEView->Insert(*mPastedTextH, ::GetHandleSize(mPastedTextH),
						(StScrpHandle) mPastedStyleH);

	mTEView->ForceAutoScroll(theOldRect);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewPasteAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);

	Rect theOldRect = (*mMacTEH)->destRect;
										// Delete text that was pasted
	::TESetSelect(mSelStart, mSelStart + ::GetHandleSize(mPastedTextH), mMacTEH);
	::TEDelete(mMacTEH);
										// Restore text deleted by the paste
	StHandleLocker	lock(mDeletedTextH);
	mTEView->Insert(*mDeletedTextH, mDeletedTextLen, mDeletedStyleH);
										// Restore selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);

	mTEView->ForceAutoScroll(theOldRect);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewClearAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewClearAction::LTEViewClearAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTEStyleTextAction(str_Clear, cmd_ActionClear, inMacTEH,
							inTextCommander, inTextPane)
{
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewClearAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);

	Rect theOldRect = (*mMacTEH)->destRect;
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	::TEDelete(mMacTEH);

	mTEView->ForceAutoScroll(theOldRect);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewTypingAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewTypingAction::LTEViewTypingAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTEStyleTextAction(str_Typing, cmd_ActionTyping, inMacTEH,
							inTextCommander, inTextPane, true)
{
	mTypedTextH  = nil;
	mTypedStyleH = nil;
	mTypingStart = mTypingEnd = mSelStart;
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewTypingAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewTypingAction::~LTEViewTypingAction()
{
		// Notify TextCommander that Action is being deleted.
		// The TextCommander usually stores a reference to a
		// TypingAction in order to add/remove characters
		// as the user performs a typing sequence.

	mTextCommander->ProcessCommand(cmd_ActionDeleted, this);

	if (mTypedTextH != nil) {
		::DisposeHandle(mTypedTextH);
	}

	if (mTypedStyleH != nil) {
		::DisposeHandle(mTypedStyleH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Reset
// ---------------------------------------------------------------------------
//	Re-initialize state of TypingAction

void
LTEViewTypingAction::Reset()
{
	mIsDone = true;

	if (mDeletedTextH != nil) {			// Clear store of deleted text
		::DisposeHandle(mDeletedTextH);
	}
	if (mDeletedStyleH != nil) {
		::DisposeHandle((Handle)mDeletedStyleH);
	}

	if (mTypedTextH != nil) {			// New typing sequence is starting
		::DisposeHandle(mTypedTextH);	//   so delete saved text
		mTypedTextH = nil;
	}
	if (mTypedStyleH != nil) {
		::DisposeHandle(mTypedStyleH);
		mTypedStyleH = nil;
	}

	mSelStart = (*mMacTEH)->selStart;	// Save current selection state
	mSelEnd = (*mMacTEH)->selEnd;
	mDeletedTextLen = mSelEnd - mSelStart;

	mTypingStart = mTypingEnd = mSelStart;

	LCommander::SetUpdateCommandStatus(true);

										// Save currently selected text,
										//   which will be deleted when
										//   the next character is typed
	mDeletedTextH = ::NewHandle(mDeletedTextLen);
	ThrowIfMemFail_(mDeletedTextH);

	::BlockMoveData(*((*mMacTEH)->hText) + mSelStart, *mDeletedTextH,
						mDeletedTextLen);

	mDeletedStyleH = ::TEGetStyleScrapHandle(mMacTEH);
}


// ---------------------------------------------------------------------------
//	¥ InputCharacter
// ---------------------------------------------------------------------------
//	Handle an input character typing action
//
//	This function does not actually put any character into the text. It
//	just saves state for undoing the typing. You should call TEKey()
//	afterwards.

void
LTEViewTypingAction::InputCharacter()
{
	if ( (mTypingEnd != (**mMacTEH).selStart) ||
		 (mTypingEnd != (**mMacTEH).selEnd) ||
		 (mTypingEnd == mTypingStart) ||
		 !IsDone() ) {
									// Selection has changed. Start a
		Reset();					//   fresh typing sequence
	}

	mTypingEnd += 1;
}


// ---------------------------------------------------------------------------
//	¥ BackwardErase
// ---------------------------------------------------------------------------
//	Handle Backward Delete typing action
//
//	Backward delete erases the current selection if one or more characters
//	is selected. If the selection is a single insertion point, then
//	backward delete erases the one character before the insertion point.
//
//	This function does not actually erase any text. It just saves state
//	for undoing the action. You should call TEKey() with a backspace
//	character afterwards.

void
LTEViewTypingAction::BackwardErase()
{
	if ( (mTypingEnd != (*mMacTEH)->selStart) ||
		 (mTypingEnd != (*mMacTEH)->selEnd) ) {
									// Selection has changed. Start a
		Reset();					//   fresh typing sequence

		if (mDeletedTextLen == 0) {
								// No selected text, save the one character
								//   that will be deleted
			::SetHandleSize(mDeletedTextH, 1);
			mDeletedTextLen = 1;
			mTypingStart -= 1;
			**mDeletedTextH = *(*((**mMacTEH).hText) + mTypingStart);

		} else {				// Selection being deleted. Increment end
			mTypingEnd += 1;	//   to counteract decrement done on the
								//   last line of this function.
		}

	} else if (mTypingStart >= (*mMacTEH)->selStart) {
									// Deleting before beginning of typing
		::SetHandleSize(mDeletedTextH, mDeletedTextLen + 1);
		ThrowIfMemError_();

		::BlockMoveData(*mDeletedTextH, *mDeletedTextH + 1, mDeletedTextLen);
		mDeletedTextLen += 1;

		mTypingStart = (SInt16) ((*mMacTEH)->selStart - 1);
		**mDeletedTextH = *(*((*mMacTEH)->hText) + mTypingStart);
	}

	mTypingEnd -= 1;
}


// ---------------------------------------------------------------------------
//	¥ ForwardErase
// ---------------------------------------------------------------------------
//	Handle Forward Delete typing action
//
//	Forward delete erases the current selection if one or more characters
//	is selected. If the selection is a single insertion point, then
//	forward delete erases the one character after the insertion point.
//
//	This function does not actually erase any text. It just saves state
//	for undoing the action. Afterwards, you should select the next
//	character if the selection is empty and call TEDelete().

void
LTEViewTypingAction::ForwardErase()
{
	if ( (mTypingEnd != (*mMacTEH)->selStart) ||
		 (mTypingEnd != (*mMacTEH)->selEnd) ) {
									// Selection has changed. Start a
		Reset();					//   fresh typing sequence

		if (mSelStart == mSelEnd) {
									// Selection is a single insertion point
									// Select next character
			::SetHandleSize(mDeletedTextH, 1);
			ThrowIfMemError_();

			**mDeletedTextH = *(*((*mMacTEH)->hText) + mSelStart);
			mDeletedTextLen = 1;
		}

	} else {						// Selection hasn't changed
									// Select next character
		::SetHandleSize(mDeletedTextH, mDeletedTextLen + 1);
		ThrowIfMemError_();

		*(*mDeletedTextH + mDeletedTextLen) =
				*(*((*mMacTEH)->hText) + mTypingEnd);
		mDeletedTextLen += 1;
	}
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------
//	Redo a TypingAction by restoring the last typing sequence

void
LTEViewTypingAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);

	Rect theOldRect = (*mMacTEH)->destRect;
									// Delete original text
	::TESetSelect(mTypingStart, mTypingStart + mDeletedTextLen, mMacTEH);
	::TEDelete(mMacTEH);
									// Insert typing run
	StHandleLocker	lock(mTypedTextH);
	mTEView->Insert(*mTypedTextH, (mTypingEnd - mTypingStart), (StScrpHandle)mTypedStyleH);

	mTEView->ForceAutoScroll(theOldRect);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------
//	Undo a TypingAction by restoring the text and selection that
//	existed before the current typing sequence started

void
LTEViewTypingAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTEView);
	Rect theOldRect = (*mMacTEH)->destRect;
									// Save current typing run
	if (mTypedTextH == nil) {
		mTypedTextH = ::NewHandle(mTypingEnd - mTypingStart);
		ThrowIfMemFail_(mTypedTextH);
	} else {
		::SetHandleSize(mTypedTextH, mTypingEnd - mTypingStart);
		ThrowIfMemError_();
	}
	::BlockMoveData(*((*mMacTEH)->hText) + mTypingStart, *mTypedTextH,
						mTypingEnd - mTypingStart);
									// Delete current typing run
	::TESetSelect(mTypingStart, mTypingEnd, mMacTEH);

	if (mTypedStyleH != nil) {
		::DisposeHandle(mTypedStyleH);
	}

	mTypedStyleH = (Handle)::TEGetStyleScrapHandle(mMacTEH);

	::TEDelete(mMacTEH);
									// Restore original text
	StHandleLocker	lock(mDeletedTextH);
	mTEView->Insert(*mDeletedTextH, mDeletedTextLen, mDeletedStyleH);

									// Restore original selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);

	mTEView->ForceAutoScroll(theOldRect);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewStyleAction					Constructor				  [public]
// ---------------------------------------------------------------------------
// 	A "base" class for the style actions (font, size, style/face, align/just,
//	color) to encapsulate some common functionality.

LTEViewStyleAction::LTEViewStyleAction(
	SInt16			inDescriptionIndex,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone)

	: LAction(STRx_RedoStyle, inDescriptionIndex, inAlreadyDone)
{
	mCommander	= inCommander;
	mTEView		= dynamic_cast<LTextEditView *>(inPane);
	ThrowIfNil_(mTEView);

	mSelStart	= (**(mTEView->GetMacTEH())).selStart;
	mSelEnd		= (**(mTEView->GetMacTEH())).selEnd;
}


// ---------------------------------------------------------------------------
//	¥ LTEViewStyleAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewStyleAction::~LTEViewStyleAction()
{
}


// ---------------------------------------------------------------------------
//	¥ CanRedo
// ---------------------------------------------------------------------------

Boolean
LTEViewStyleAction::CanRedo() const
{
	// Call inherited and ensure the commander is on duty.
	return LAction::CanRedo() && mCommander->IsOnDuty();
}


// ---------------------------------------------------------------------------
//	¥ CanUndo
// ---------------------------------------------------------------------------

Boolean
LTEViewStyleAction::CanUndo() const
{
	// Call inherited and ensure the commander is on duty.
	return LAction::CanUndo() && mCommander->IsOnDuty();
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewFontAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewFontAction::LTEViewFontAction(
	SInt16			inFont,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone)

	: LTEViewStyleAction(str_Font, inCommander, inPane, inAlreadyDone)
{
	mFont = inFont;

	mTEView->GetFont(mSavedFont);			// Save the current font
}


// ---------------------------------------------------------------------------
//	¥ LTEViewFontAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewFontAction::LTEViewFontAction(
	Str255			inFontName,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone )

	: LTEViewStyleAction(str_Font, inCommander, inPane, inAlreadyDone)
{
	::GetFNum(inFontName, &mFont);			// Get font number from name

	mTEView->GetFont(mSavedFont);			// Save the current font
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewFontAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewFontAction::~LTEViewFontAction()
{
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewFontAction::UndoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	TextStyle theStyle;
	theStyle.tsFont = mSavedFont;

	::TESetSelect( mSelStart, mSelEnd, mTEView->GetMacTEH() );
	::TESetStyle( doFont, &theStyle, false, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();

}

// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewFontAction::RedoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	TextStyle theStyle;
	theStyle.tsFont = mFont;

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle( doFont, &theStyle, false, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();
}

#pragma mark-

// ---------------------------------------------------------------------------
//	¥ LTEViewSizeAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewSizeAction::LTEViewSizeAction(
	SInt16			inSize,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone)

	: LTEViewStyleAction(str_Size, inCommander, inPane, inAlreadyDone)
{
	mSize = inSize;

	mTEView->GetSize( mSavedSize );
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewSizeAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewSizeAction::~LTEViewSizeAction()
{
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewSizeAction::UndoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	TextStyle theStyle;
	theStyle.tsSize = mSavedSize;

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle( doSize, &theStyle, false, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewSizeAction::RedoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	TextStyle theStyle;
	theStyle.tsSize = mSize;

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle( doSize, &theStyle, false, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();
}


#pragma mark-

// ---------------------------------------------------------------------------
//	¥ LTEViewFaceAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewFaceAction::LTEViewFaceAction(
	Style			inFace,
	bool			inToggle,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone)

	: LTEViewStyleAction(str_Face, inCommander, inPane, inAlreadyDone)
{
	mFace	= inFace;
	mToggle = inToggle;

	mTEView->GetStyle(mSavedFace);
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewFaceAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewFaceAction::~LTEViewFaceAction()
{
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewFaceAction::UndoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	SInt16 theMode = doFace;
	TextStyle theStyle;

	if (mToggle) {
		theStyle.tsFace = mFace;
		theMode			+= doToggle;
	} else {
		theStyle.tsFace = mSavedFace;
	}

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle(theMode, &theStyle, false, mTEView->GetMacTEH());

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewFaceAction::RedoSelf()
{
	// set to mFace, if toggle, toggle, else SetStyle

	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	TextStyle theStyle;
	theStyle.tsFace = mFace;

	SInt16 theMode = doFace;

	if (mToggle) {
		theMode += doToggle;
	}

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle( theMode, &theStyle, false, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewAlignAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewAlignAction::LTEViewAlignAction(
	SInt16			inAlign,
	LCommander*		inCommander,
	LPane*			inPane,
	bool			inAlreadyDone)

	: LTEViewStyleAction(str_Alignment, inCommander, inPane, inAlreadyDone)
{
	mAlign		= inAlign;
	mSavedAlign = mTEView->GetAlignment();
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewAlignAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewAlignAction::~LTEViewAlignAction()
{
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewAlignAction::UndoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	::TESetAlignment( mSavedAlign, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();

}

// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewAlignAction::RedoSelf()
{
	Rect oldRect = (*(mTEView->GetMacTEH()))->destRect;

	::TESetAlignment( mAlign, mTEView->GetMacTEH() );

	mTEView->AlignTextEditRects();
	mTEView->AdjustImageToText();
	mTEView->ForceAutoScroll(oldRect);
	mTEView->Refresh();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEViewColorAction					Constructor				  [public]
// ---------------------------------------------------------------------------

LTEViewColorAction::LTEViewColorAction(
	const RGBColor&		inColor,
	LCommander*			inCommander,
	LPane*				inPane,
	bool				inAlreadyDone)

	: LTEViewStyleAction(str_Color, inCommander, inPane, inAlreadyDone)
{
	mColor = inColor;
	mTEView->GetColor(mSavedColor);
}


// ---------------------------------------------------------------------------
//	¥ ~LTEViewColorAction					Destructor				  [public]
// ---------------------------------------------------------------------------

LTEViewColorAction::~LTEViewColorAction()
{
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEViewColorAction::UndoSelf()
{
	TextStyle theStyle;
	theStyle.tsColor = mSavedColor;

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle(doColor, &theStyle, false, mTEView->GetMacTEH());

	mTEView->Refresh();
}

// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEViewColorAction::RedoSelf()
{
	TextStyle theStyle;
	theStyle.tsColor = mColor;

	::TESetSelect(mSelStart, mSelEnd, mTEView->GetMacTEH());
	::TESetStyle(doColor, &theStyle, false, mTEView->GetMacTEH());

	mTEView->Refresh();
}


PP_End_Namespace_PowerPlant
