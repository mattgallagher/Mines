// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTETextAction.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Undoable Actions associated with a Toolbox TextEdit record

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UTETextAction.h>
#include <LCommander.h>
#include <LPane.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UMemoryMgr.h>
#include <UScrap.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTETextAction							Constructor				  [public]
// ---------------------------------------------------------------------------

LTETextAction::LTETextAction(
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
	mTextPane		= inTextPane;

		// Save current selection range and the selected text,
		// which will be deleted when doing the Action.

	mSelStart		= (**inMacTEH).selStart;
	mSelEnd			= (**inMacTEH).selEnd;
	mDeletedTextLen = mSelEnd - mSelStart;
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
}


// ---------------------------------------------------------------------------
//	¥ ~LTETextAction						Destructor				  [public]
// ---------------------------------------------------------------------------

LTETextAction::~LTETextAction()
{
	if (mDeletedTextH != nil) {
		::DisposeHandle(mDeletedTextH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Redo
// ---------------------------------------------------------------------------

void
LTETextAction::Redo()
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
LTETextAction::Undo()
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
LTETextAction::CanRedo() const
{
	return (!IsDone() && mTextCommander->IsOnDuty());
}


// ---------------------------------------------------------------------------
//	¥ CanUndo
// ---------------------------------------------------------------------------

Boolean
LTETextAction::CanUndo() const
{
	return (IsDone() && mTextCommander->IsOnDuty());
}

// ---------------------------------------------------------------------------
//	¥ IsPostable
// ---------------------------------------------------------------------------

Boolean
LTETextAction::IsPostable() const
{
		// Action is not postable (undoable) if we could not
		// allocate space to store the text that was deleted

	return (mDeletedTextH != nil);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTETextAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
										// Restore deleted text
	::TESetSelect(mSelStart, mSelStart, mMacTEH);
	StHandleLocker	lock(mDeletedTextH);
	::TEInsert(*mDeletedTextH, mDeletedTextLen, mMacTEH);
										// Restore original selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
}

#pragma mark -
// ===========================================================================
//	¥ LTECutAction
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LTECutAction							Constructor				  [public]
// ---------------------------------------------------------------------------

LTECutAction::LTECutAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTETextAction(str_Cut, cmd_ActionCut, inMacTEH,
						inTextCommander, inTextPane)
{
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTECutAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
										// Delete selected text
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	::TEDelete(mMacTEH);
										// Put deleted text on clipboard
	UScrap::SetData(ResType_Text, mDeletedTextH);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEPasteAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEPasteAction::LTEPasteAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTETextAction(str_Paste, cmd_ActionPaste, inMacTEH,
							inTextCommander, inTextPane)
{
	StHandleBlock	textH(Size_Zero);
	UScrap::GetData(ResType_Text, textH);

	mPastedTextH = textH.Release();
}


// ---------------------------------------------------------------------------
//	¥ ~LTEPasteAction						Destructor				  [public]
// ---------------------------------------------------------------------------

LTEPasteAction::~LTEPasteAction()
{
	if (mPastedTextH != nil) {
		::DisposeHandle(mPastedTextH);
	}
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEPasteAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	::TEDelete(mMacTEH);
	StHandleLocker	lock(mPastedTextH);
	::TEInsert(*mPastedTextH, ::GetHandleSize(mPastedTextH), mMacTEH);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------

void
LTEPasteAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
										// Delete text that was pasted
	::TESetSelect(mSelStart, mSelStart + ::GetHandleSize(mPastedTextH),
					mMacTEH);
	::TEDelete(mMacTEH);
										// Restore text deleted by the paste
	StHandleLocker	lock(mDeletedTextH);
	::TEInsert(*mDeletedTextH, mDeletedTextLen, mMacTEH);
										// Restore selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTEClearAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTEClearAction::LTEClearAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTETextAction(str_Clear, cmd_ActionClear, inMacTEH,
						inTextCommander, inTextPane)
{
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------

void
LTEClearAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
	::TEDelete(mMacTEH);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LTETypingAction						Constructor				  [public]
// ---------------------------------------------------------------------------

LTETypingAction::LTETypingAction(
	TEHandle		inMacTEH,
	LCommander*		inTextCommander,
	LPane*			inTextPane)

	: LTETextAction(str_Typing, cmd_ActionTyping, inMacTEH,
						inTextCommander, inTextPane, true)
{
	mTypedTextH  = nil;
	mTypingStart = mTypingEnd = mSelStart;
}


// ---------------------------------------------------------------------------
//	¥ ~LTETypingAction						Destructor				  [public]
// ---------------------------------------------------------------------------

LTETypingAction::~LTETypingAction()
{
		// Notify TextCommander that Action is being deleted.
		// The TextCommander usually stores a reference to a
		// TypingAction in order to add/remove characters
		// as the user performs a typing sequence.

	mTextCommander->ProcessCommand(cmd_ActionDeleted, this);

	if (mTypedTextH != nil) {
		::DisposeHandle(mTypedTextH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Reset
// ---------------------------------------------------------------------------
//	Re-initialize state of TypingAction

void
LTETypingAction::Reset()
{
	mIsDone = true;

	if (mDeletedTextH != nil) {			// Clear store of deleted text
		::DisposeHandle(mDeletedTextH);
		mDeletedTextH = nil;
	}

	if (mTypedTextH != nil) {			// New typing sequence is starting
		::DisposeHandle(mTypedTextH);	//   so delete saved text
		mTypedTextH = nil;
	}

	mSelStart = (**mMacTEH).selStart;	// Save current selection state
	mSelEnd = (**mMacTEH).selEnd;
	mDeletedTextLen = mSelEnd - mSelStart;

	mTypingStart = mTypingEnd = mSelStart;

	LCommander::SetUpdateCommandStatus(true);

										// Save currently selected text,
										//   which will be deleted when
										//   the next character is typed
	mDeletedTextH = ::NewHandle(mDeletedTextLen);
	ThrowIfMemFail_(mDeletedTextH);

	::BlockMoveData(*((**mMacTEH).hText) + mSelStart, *mDeletedTextH,
							mDeletedTextLen);
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
LTETypingAction::InputCharacter()
{
	if ( (mTypingEnd != (**mMacTEH).selStart) ||
		 (mTypingEnd != (**mMacTEH).selEnd) ||
		 (mTypingEnd == mTypingStart) ||
		 !IsDone() ) {
								// Selection has changed. Start a
		Reset();				//   fresh typing sequence
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
LTETypingAction::BackwardErase()
{
	if ( (mTypingEnd != (**mMacTEH).selStart) ||
		 (mTypingEnd != (**mMacTEH).selEnd) ) {
								// Selection has changed. Start a
		Reset();				//   fresh typing sequence

		if (mDeletedTextLen == 0) {
								// No selected text, save the one character
								//   that will be deleted
			::SetHandleSize(mDeletedTextH, 1);

			mDeletedTextLen = 1;
			mTypingStart   -= 1;
			**mDeletedTextH = *(*((**mMacTEH).hText) + mTypingStart);

		} else {				// Selection being deleted. Increment end
			mTypingEnd += 1;	//   to counteract decrement done on the
								//   last line of this function.
		}

	} else if (mTypingStart >= (**mMacTEH).selStart) {
								// Deleting before beginning of typing
		::SetHandleSize(mDeletedTextH, mDeletedTextLen + 1);
		ThrowIfMemError_();

		::BlockMoveData(*mDeletedTextH, *mDeletedTextH + 1, mDeletedTextLen);
		mDeletedTextLen += 1;

		mTypingStart = (SInt16) ((**mMacTEH).selStart - 1);
		**mDeletedTextH = *(*((**mMacTEH).hText) + mTypingStart);
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
LTETypingAction::ForwardErase()
{
	if ( (mTypingEnd != (**mMacTEH).selStart) ||
		 (mTypingEnd != (**mMacTEH).selEnd) ) {
								// Selection has changed. Start a
		Reset();				//   fresh typing sequence

		if (mSelStart == mSelEnd) {
								// Selection is a single insertion point
								// Select next character
			::SetHandleSize(mDeletedTextH, 1);
			ThrowIfMemError_();

			**mDeletedTextH = *(*((**mMacTEH).hText) + mSelStart);
			mDeletedTextLen = 1;
		}

	} else {					// Selection hasn't changed
								// Select next character
		::SetHandleSize(mDeletedTextH, mDeletedTextLen + 1);
		ThrowIfMemError_();

		*(*mDeletedTextH + mDeletedTextLen) =
									*(*((**mMacTEH).hText) + mTypingEnd);
		mDeletedTextLen += 1;
	}
}


// ---------------------------------------------------------------------------
//	¥ RedoSelf
// ---------------------------------------------------------------------------
//	Redo a TypingAction by restoring the last typing sequence

void
LTETypingAction::RedoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
										// Delete original text
	::TESetSelect(mTypingStart, mTypingStart + mDeletedTextLen, mMacTEH);
	::TEDelete(mMacTEH);
										// Insert typing run
	StHandleLocker	lock(mTypedTextH);
	::TEInsert(*mTypedTextH, (mTypingEnd - mTypingStart), mMacTEH);
}


// ---------------------------------------------------------------------------
//	¥ UndoSelf
// ---------------------------------------------------------------------------
//	Undo a TypingAction by restoring the text and selection that
//	existed before the current typing sequence started

void
LTETypingAction::UndoSelf()
{
	StFocusAndClipIfHidden	focus(mTextPane);
										// Save current typing run
	if (mTypedTextH == nil) {
		mTypedTextH = ::NewHandle(mTypingEnd - mTypingStart);
		ThrowIfMemFail_(mTypedTextH);

	} else {
		::SetHandleSize(mTypedTextH, mTypingEnd - mTypingStart);
		ThrowIfMemError_();
	}

	::BlockMoveData(*((**mMacTEH).hText) + mTypingStart, *mTypedTextH,
						mTypingEnd - mTypingStart);
										// Delete current typing run
	::TESetSelect(mTypingStart, mTypingEnd, mMacTEH);
	::TEDelete(mMacTEH);
										// Restore original text
	StHandleLocker	lock(mDeletedTextH);
	::TEInsert(*mDeletedTextH, mDeletedTextLen, mMacTEH);
										// Restore original selection
	::TESetSelect(mSelStart, mSelEnd, mMacTEH);
}


PP_End_Namespace_PowerPlant
