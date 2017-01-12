// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInPlaceEditField.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LInPlaceEditField.h>

#include <PP_KeyCodes.h>
#include <LString.h>
#include <LView.h>
#include <UNewTextDrawing.h>
#include <UTextTraits.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//		* LInPlaceEditField(SPaneInfo&, ...)
// ---------------------------------------------------------------------------
//	Procedural constructor

LInPlaceEditField::LInPlaceEditField(
	const SPaneInfo&	inPaneInfo,
	Str255				inString,
	ResIDT				inTextTraitsID,
	SInt16				inMaxChars,
	UInt8				inAttributes,
	TEKeyFilterFunc		inKeyFilter,
	LCommander*			inSuper)

: LBroadcasterEditField(inPaneInfo, inString, inTextTraitsID, inMaxChars,
						inAttributes, inKeyFilter, inSuper),
  LAttachment(msg_Event, true)

{
	GetFrameSize(mMaximumSize);
	mHasBox = (inAttributes & editAttr_Box) != 0;
	if (mHasBox) {
		mMaximumSize.width -= 4;
		mMaximumSize.height -= 4;
	}
	mClosingEditField = false;
	LCommander::GetTopCommander()->AddAttachment(this, nil, true);
}


// ---------------------------------------------------------------------------
//		* ~LInPlaceEditField
// ---------------------------------------------------------------------------
//	Destructor

LInPlaceEditField::~LInPlaceEditField()
{
	PutInside(nil);
}


// ---------------------------------------------------------------------------
//		* UserChangedText
// ---------------------------------------------------------------------------
//	When the text changes (for any reason), this hook is called.
//	This is where the edit field gets resized to fit the text being edited.

void
LInPlaceEditField::UserChangedText()
{
	AdjustSize();
}


// ---------------------------------------------------------------------------
//		* AdjustSize
// ---------------------------------------------------------------------------
//	Called by UserChangedText to ensure that the size of the edit field
//	reflects the size of the current text.

void
LInPlaceEditField::AdjustSize()
{

	// Set up drawing environment.

	Str255 newText;
	GetDescriptor(newText);
	FocusDraw();

	SInt16 justification = UTextTraits::SetPortTextTraits(mTextTraitsID);

	// Get size of existing text. Note that we are not interested in
	// the size of the box frame that's drawn around the text.

	SDimension16 currentSize;
	GetFrameSize(currentSize);
	currentSize.width -= 2;

	if (mHasBox) {
		currentSize.width  -= 4;
		currentSize.height -= 4;
	}

	// Get size of new text.

	Rect maxRect = { 0, 0, 32767, mMaximumSize.width };
	Rect textRect;

	LStr255 measureText(newText);

	if (newText[0] == 0) {
		measureText = char_Space;
	}

	UNewTextDrawing::MeasureWithJustification((Ptr) &measureText[1], measureText[0], maxRect, justification, textRect, true);

	if (newText[0] == 0) {
		textRect.right = (SInt16) (textRect.left + 2);
	}

	SDimension16 newSize;
	newSize.width = (SInt16) (textRect.right - textRect.left);
	newSize.height = textRect.bottom;

	if (newSize.width > mMaximumSize.width) {
		newSize.width = mMaximumSize.width;
	}

	if (newSize.height > mMaximumSize.height) {
		newSize.height = mMaximumSize.height;
	}

	// Figure size difference. If unchanged, exit now.

	SInt16 widthDelta  = (SInt16) (newSize.width - currentSize.width);
	SInt16 heightDelta = (SInt16) (newSize.height - currentSize.height);

	if ((widthDelta == 0) && (heightDelta == 0)) {
		return;
	}

	// Reconcile text edit destination rectangle.
	// It often gets confused when inserting characters.

	Rect destRect = (**mTextEditH).destRect;
	Rect frameRect;
	CalcLocalFrameRect(frameRect);

	if (mHasBox)
		::MacInsetRect(&frameRect, 2, 2);

	if (destRect.top < frameRect.top) {
		SInt16 minDestTop = (SInt16) (frameRect.top + newSize.height - textRect.bottom);
		if (destRect.top < minDestTop)
			(**mTextEditH).destRect.top = minDestTop;
	}

	// Move edit field to maintain proper position & erase area left behind.
	// Note that we don't support centered text.

	if (justification == teJustRight) {
		MoveBy(-widthDelta, 0, true);
	}

	LBroadcasterEditField::ResizeFrameBy(widthDelta, heightDelta, true);
	UpdatePort();

}


// ---------------------------------------------------------------------------
//		* ResizeFrameBy
// ---------------------------------------------------------------------------

void
LInPlaceEditField::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		/* inRefresh */)
{
	mMaximumSize.width  += inWidthDelta;
	mMaximumSize.height += inHeightDelta;
	UserChangedText();
}


// ---------------------------------------------------------------------------
//		* StopEditing
// ---------------------------------------------------------------------------

void
LInPlaceEditField::StopEditing()
{
	if (!mClosingEditField) {
		mClosingEditField = true;
		Refresh();
		BroadcastValueMessage();
		delete this;
	}
}


// ---------------------------------------------------------------------------
//		* Execute											[protected]
// ---------------------------------------------------------------------------
//	The in-place editor makes itself an attachment to the top commander
//	(usually the application object). It intercepts mouse-down events
//	that occur elsewhere in the CONTENT area of this window and uses them
//	to trigger a deletion of the in-place edit object.

Boolean
LInPlaceEditField::Execute(
	MessageT	inMessage,
	void*		ioParam)
{
	Boolean	executeHost = true;

	if ((inMessage == mMessage) || (mMessage == msg_AnyMessage)) {

		executeHost = mExecuteHost;

		// See if this is a mouse-down event.

		EventRecord* macEvent = (EventRecord*) ioParam;
		if (macEvent->what == mouseDown) {

			// Mouse down: See what window it is.

			WindowPtr macWindowP;
			SInt16 thePart = ::MacFindWindow(macEvent->where, &macWindowP);

			// This window is closing or another window will be activated,
			// so kill the edit field

			if ( (thePart == inGoAway)  ||
				 ((macWindowP != nil) && (macWindowP != GetMacWindow()))) {
				if (!mClosingEditField) {
					StopEditing();
					return executeHost;
				}
			}

			// If anywhere else in this window's content area, kill the edit field.

			if (thePart == inContent) {
				Rect frame;
				CalcPortFrameRect(frame);
				PortToGlobalPoint(topLeft(frame));
				PortToGlobalPoint(botRight(frame));

				if (!::MacPtInRect(macEvent->where, &frame)) {
					StopEditing();
				}
			}
		}
	}

	return executeHost;
}


// ---------------------------------------------------------------------------
//		* DeactivateSelf										[protected]
// ---------------------------------------------------------------------------
//	If the window is deactivated, that means there was a click in another
//	window or another application. Close the in-place editor.

void
LInPlaceEditField::DeactivateSelf()
{
	LView* superView = GetSuperView();
	Hide();
	StopEditing();
	superView->UpdatePort();
}


// ---------------------------------------------------------------------------
//		* HandleKeyPress										[protected]
// ---------------------------------------------------------------------------
//	Intercept Return and Enter keys. If they are received, close the
//	in-place editor.

Boolean
LInPlaceEditField::HandleKeyPress(
	const EventRecord& inKeyEvent)
{
	UInt16		theChar = (UInt16) (inKeyEvent.message & charCodeMask);

	// Intercept Enter and Return key for immediate value change and destruction.

	EKeyStatus theKeyStatus = keyStatus_Input;
	if (mKeyFilter != nil) {
		theKeyStatus = (*mKeyFilter)(mTextEditH, (UInt16) inKeyEvent.message,
										theChar, inKeyEvent.modifiers);
	}
	if ((theKeyStatus != keyStatus_Input) && (UKeyFilters::IsActionKey((UInt16) inKeyEvent.message))) {
		switch (theChar) {
			case char_Return:
			case char_Enter:
				StopEditing();
				return true;
		}
	}

	// Now do whatever it was we were originally going to do.

	return LBroadcasterEditField::HandleKeyPress(inKeyEvent);

}


// ---------------------------------------------------------------------------
//		* ObeyCommand											[protected]
// ---------------------------------------------------------------------------
//	Intercept commands which will be processed by other commanders.
//	If one occurs, close the in-place editor.

Boolean
LInPlaceEditField::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	switch (inCommand) {

		case cmd_Undo:
		case cmd_Cut:
		case cmd_Copy:
		case cmd_Paste:
		case cmd_Clear:

		case cmd_ActionCut:
		case cmd_ActionPaste:
		case cmd_ActionClear:
		case cmd_ActionTyping:
		case cmd_ActionDeleted:

		case msg_TabSelect:
		case cmd_SelectAll:
			return LBroadcasterEditField::ObeyCommand(inCommand, ioParam);

		default:
			LCommander* super = GetSuperCommander();
			StopEditing();
			return super->ProcessCommand(inCommand, ioParam);
	}
}


// ---------------------------------------------------------------------------
//		* DontBeTarget											[protected]
// ---------------------------------------------------------------------------
//	Overridden to prevent the value message from being sent again.
//	This can happen if closing the in-place editor triggers a window to
//	be opened (such as an alert signalling an invalid value).

void
LInPlaceEditField::DontBeTarget()
{
	if (mClosingEditField)
		LEditField::DontBeTarget();
	else
		LBroadcasterEditField::DontBeTarget();
}


// ---------------------------------------------------------------------------
//		* AlignTextEditRects									[protected]
// ---------------------------------------------------------------------------
//	Overridden to preserve the text scroll position when resizing the
//	edit field. Relies on the UserChangedText override to ensure that the
//	text doesn't scroll out of view.

void
LInPlaceEditField::AlignTextEditRects()
{
	Rect	textFrame;
	if (!CalcLocalFrameRect(textFrame)) {

				// Frame is outside QD Space. Put textFrame  at the
				// upper left limit of QD Space (extreme negative coords).
				// That location is guaranteed to be offscreen (unless
				// you have a control longer than 32K pixels) since PP
				// Image coordinates start at (0,0) and are never negative.

		textFrame.left   = min_Int16;
		textFrame.right  = (SInt16) (textFrame.left + mFrameSize.width);
		textFrame.top    = min_Int16;
		textFrame.bottom = (SInt16) (textFrame.top + mFrameSize.height);
	}

	if (mHasBox)
		::MacInsetRect(&textFrame, 2, 2);

	// Be sure destination rect is reasonable, but preserve
	// scroll position as much as possible.

	(**mTextEditH).destRect.left = textFrame.left;
	(**mTextEditH).destRect.right = textFrame.right;
	(**mTextEditH).destRect.bottom = textFrame.bottom;

	// Set TextEdit view rectangle to be the same as the frame.

	(**mTextEditH).viewRect = textFrame;

	// Let TextEdit adjust line breaks.

	AdjustTextWidth(false);
	::TECalText(mTextEditH);

}

PP_End_Namespace_PowerPlant
