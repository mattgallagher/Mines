// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditText.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Text Editing Control
//
//	Implementation of Password variation:
//
//		To support password text entry, we replace the low-level QD procs for
//		drawing and measuring text. Our custom procs always draw or measure
//		the special password character no matter what characters are actually
//		in the field. Before making any call that accesses the text, we
//		install our custom procs, then restore the original procs afterwards.
//
//		With this technique, the password text is actually in the TextEdit
//		record, but it always draws as the password character. So you access
//		the password text the same as normal text using SetText() and
//		GetText(). This differs from the Toolbox EditText control, where
//		there are separate accessors for password and normal text.
//
//		You can set the special password character by calling
//		SetPasswordChar(), which is a static class function. Thus all
//		EditText Panes use the same password character. The default
//		choice is the bullet (¥) character.
//
//		USE WARNING:
//			You should not attach an LUndoer directly to a LEditText.
//			If you do, performing undo will show the password field
//			contents in clear text.
//
//			Instead, attach the LUndoer to a supercommander of the
//			LEditText, such as the enclosing tab group or window.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LEditText.h>
#include <LEditField.h>
#include <LControlImp.h>
#include <LStream.h>
#include <LString.h>
#include <LView.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UGAColorRamp.h>
#include <UTETextAction.h>
#include <UTextTraits.h>
#include <UMemoryMgr.h>
#include <UScrap.h>

#include <Sound.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

CQDProcs	LEditText::sPasswordProcs;		// QD drawing procs struct
bool		LEditText::sProcsInited;		// Have we initialized yet?
UInt8		LEditText::sPasswordChars[4];


// ---------------------------------------------------------------------------
//	¥ LEditText								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LEditText::LEditText(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream)
{
	SInt16		controlKind;
	ResIDT		textTraitsID;
	Str255		title;

	*inStream >> controlKind;
	mIsPassword = (controlKind == kControlEditTextPasswordProc);

	*inStream >> textTraitsID;
	inStream->ReadPString(title);

		// Always use kControlEditTextProc even if this is a password
		// field. This class manages all password handling and the
		// imp always operates as normal editable text.

	MakeControlImp(inImpID, kControlEditTextProc, title, textTraitsID);

	*inStream >> mMaxChars;

	UInt8		attributes;
	*inStream >> attributes;

	SInt8	keyFilterID;
	*inStream >> keyFilterID;
	mKeyFilter = UKeyFilters::SelectTEKeyFilter(keyFilterID);

	if (mKeyFilter == nil) {		// Use default key filter. Typing and
									//   undo won't work right without
									//   a key filter.
		mKeyFilter = &UKeyFilters::PrintingCharField;
	}

	InitEditText(title, textTraitsID, attributes);
}


// ---------------------------------------------------------------------------
//	¥ LEditText								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LEditText::LEditText(
	const SPaneInfo&	inPaneInfo,
	LCommander*			inSuperCommander,
	ConstStringPtr		inInitialText,
	ResIDT				inTextTraitsID,
	MessageT			inMessage,
	SInt16				inMaxChars,
	UInt8				inAttributes,
	TEKeyFilterFunc		inKeyFilter,
	bool				inPasswordField,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID,
						kControlEditTextProc,
						inInitialText, inTextTraitsID, inMessage),
	  LCommander(inSuperCommander)
{
	mMaxChars		= inMaxChars;
	mKeyFilter		= inKeyFilter;
	mIsPassword		= inPasswordField;

	InitEditText(inInitialText, inTextTraitsID, inAttributes);
}


// ---------------------------------------------------------------------------
//	¥ InitEditText							Initializer			   [protected]
// ---------------------------------------------------------------------------

void
LEditText::InitEditText(
	ConstStringPtr	inText,
	ResIDT			inTextTraitsID,
	UInt8			inAttributes)
{
	mTextTraitsID = inTextTraitsID;			// Set now so we can focus

	mTypingAction	= nil;
	mHasFocusRing	= false;
	mHasWordWrap	= (inAttributes & editAttr_WordWrap) != 0;

	StFocusAndClipIfHidden	focus(this);

	Rect	frame;
	CalcLocalFrameRect(frame);

	frame.left  -= frame.right - 10;		// Put initial TE Rects offscreen
	frame.right  = 0;						//   to suppress drawing. Reduce
	frame.top   -= frame.bottom - 10;		//   by 10 to account for box
	frame.bottom = 0;

	mTextEditH = ::TENew(&frame, &frame);
	ThrowIfNil_(mTextEditH);

	if (inText[0] > 0) {					// Set initial text
		::TESetText(inText + 1, inText[0], mTextEditH);
	}

	SetTextTraitsID(inTextTraitsID);		// Set traits for TE Record
	AlignTextEditRects();
	::TESetSelect(0, max_Int16, mTextEditH);	// Select All

									// Set up custom draw procs to handle
									//   password variation
	if (mIsPassword && !sProcsInited) {
		::SetStdCProcs(&sPasswordProcs);
		sPasswordProcs.textProc = NewQDTextUPP(PasswordStdText);
		sPasswordProcs.txMeasProc = NewQDTxMeasUPP(PasswordStdTxMeas);

		if (sPasswordChars[0] == 0) {		// If password char is not set,
			SetPasswordChar(char_Bullet);	//   set it to a bullet
		}

		sProcsInited = true;
	}

		// Turn off autoscroll for centered or right justified text.
		// It doesn't work.

	TextTraitsH	traitsH = UTextTraits::LoadTextTraits(inTextTraitsID);
	if (traitsH != nil) {
		if ( ((**traitsH).justification == teJustRight)  ||
			 ((**traitsH).justification == teJustCenter) ) {
			inAttributes = (UInt8) (inAttributes & ~editAttr_AutoScroll);
		}
	}
									// Set TextEdit attributes

	::TEFeatureFlag(teFAutoScroll,
			((inAttributes & editAttr_AutoScroll) != 0) ?
				teBitSet : teBitClear, mTextEditH);
	::TEFeatureFlag(teFTextBuffering,
			((inAttributes & editAttr_TextBuffer) != 0) ?
				teBitSet : teBitClear, mTextEditH);
	::TEFeatureFlag(teFOutlineHilite,
			((inAttributes & editAttr_OutlineHilite) != 0) ?
				teBitSet : teBitClear, mTextEditH);
	::TEFeatureFlag(teFInlineInput,
			((inAttributes & editAttr_InlineInput) != 0) ?
				teBitSet : teBitClear, mTextEditH);
	::TEFeatureFlag(teFUseTextServices,
			((inAttributes & editAttr_TextServices) != 0) ?
				teBitSet : teBitClear, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ ~LEditText							Destructor				  [public]
// ---------------------------------------------------------------------------

LEditText::~LEditText()
{
	if (mTypingAction != nil) {
		PostAction(nil);
	}

	::TEDispose(mTextEditH);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	SetText((Ptr) inDescriptor + 1, inDescriptor[0]);
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LEditText::GetDescriptor(
	Str255	outDescriptor) const
{
	Size	strLen;
	GetText((Ptr) outDescriptor + 1, 255, &strLen);

	if (strLen > 255) {
		strLen = 255;
	}
	outDescriptor[0] = (UInt8) strLen;

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetValue(
	SInt32		inValue)
{
	LStr255		valStr = inValue;
	SetText(valStr);
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

SInt32
LEditText::GetValue() const
{
	LStr255		valStr;
	GetText(valStr);
	return (SInt32) valStr;
}


// ---------------------------------------------------------------------------
//	¥ SetText														  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetText(
	const void*		inTextPtr,
	Size			inTextLength)
{
	StFocusAndClipIfHidden	focus(this);
	StClipRgnState	clip(nil);
	::TESetText(inTextPtr, inTextLength, mTextEditH);
	SelectAll();
	RefreshText();
}


void
LEditText::SetText(
	const LString	&inString)
{
	SetText(inString.ConstTextPtr(), inString.Length());
}


// ---------------------------------------------------------------------------
//	¥ GetText														  [public]
// ---------------------------------------------------------------------------

void
LEditText::GetText(
	Ptr		outBuffer,
	Size	inBufferLength,
	Size*	outTextLength) const
{
	CharsHandle	theText = ::TEGetText(mTextEditH);
	Size		textLen	= ::GetHandleSize(theText);

	if (outBuffer != nil) {					// Copy text into buffer
		Size	copyLen = textLen;
		if (inBufferLength < textLen) {		// Reduce amount to copy if
			copyLen = inBufferLength;		//   buffer is too small
		}

		::BlockMoveData(*theText, outBuffer, copyLen);
	}

	if (outTextLength != nil) {				// Pass back actual text length
		*outTextLength = textLen;
	}
}


void
LEditText::GetText(
	LString		&outString) const
{
	UInt16	maxLen = (UInt16) (outString.GetMaxLength() - 1);
	Size	strLen;

	GetText(outString.TextPtr(), maxLen, &strLen);

	if (strLen > maxLen) {
		strLen = maxLen;
	}
	outString[0] = (UInt8) strLen;
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	UTextTraits::SetTETextTraits(inTextTraitsID, mTextEditH);
	mTextTraitsID = inTextTraitsID;
}


// ---------------------------------------------------------------------------
//	¥ GetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

ResIDT
LEditText::GetTextTraitsID() const
{
	return mTextTraitsID;
}


// ---------------------------------------------------------------------------
//	¥ SetSelection													  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetSelection(
	ControlEditTextSelectionRec&	inSelection)
{
	StFocusAndClipIfHidden	focus(this);
	::TESetSelect(inSelection.selStart, inSelection.selEnd, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ SelectAll														  [public]
// ---------------------------------------------------------------------------

void
LEditText::SelectAll()
{
	StFocusAndClipIfHidden	focus(this);
	::TESetSelect(0, max_Int16, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ GetSelection													  [public]
// ---------------------------------------------------------------------------

void
LEditText::GetSelection(
	ControlEditTextSelectionRec&	outSelection) const
{
	outSelection.selStart = (**mTextEditH).selStart;
	outSelection.selEnd   = (**mTextEditH).selEnd;
}


// ---------------------------------------------------------------------------
//	¥ GetSelection													  [public]
// ---------------------------------------------------------------------------
//	Passes back an AEDesc of the currently selected text

void
LEditText::GetSelection(
	AEDesc&		outDesc) const
{
	SInt16	selStart = (**mTextEditH).selStart;
	SInt16	selEnd   = (**mTextEditH).selEnd;

	StHandleLocker lock((Handle) (**mTextEditH).hText);

	OSErr err = ::AECreateDesc(typeChar,
							   (*(**mTextEditH).hText + selStart),
							   (selEnd - selStart),
							   &outDesc );

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ HasSelection													  [public]
// ---------------------------------------------------------------------------

bool
LEditText::HasSelection() const
{										// Selection start and end are
										//   different if text is selected
	return ((**mTextEditH).selStart != (**mTextEditH).selEnd);
}


// ---------------------------------------------------------------------------
//	¥ SetPasswordChar										 [static] [public]
// ---------------------------------------------------------------------------
//	Specify the character to use when displaying a password. This is a
//	static function, so the same character is used for all password
//	EditText Panes.
//
//	If you change the password character when EditText Panes are visible,
//	you are responsible for refreshing them to reflect the change.

void
LEditText::SetPasswordChar(
	UInt8	inChar)
{
	sPasswordChars[0] = inChar;			// For efficiency when drawing, we
	sPasswordChars[1] = inChar;			// store an array of 4 of the
	sPasswordChars[2] = inChar;			// password characters
	sPasswordChars[3] = inChar;
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LEditText::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {

		case kControlEditTextTextTag:
		case kControlEditTextPasswordTag:
			SetText((Ptr) inDataPtr, inDataSize);
			break;

		case kControlEditTextTEHandleTag:
			SetMacTEH( *(TEHandle*) inDataPtr );
			break;

		case kControlEditTextSelectionTag:
			SetSelection( *(ControlEditTextSelectionRec*) inDataPtr );
			break;
			
		default:
			LControlPane::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LEditText::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	switch (inTag) {

		case kControlEditTextTextTag:
		case kControlEditTextPasswordTag:
			GetText((Ptr) inBuffer, inBufferSize, outDataSize);
			break;

		case kControlEditTextTEHandleTag:
			if (inBuffer != nil) {
				*(TEHandle*) inBuffer = GetMacTEH();
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(TEHandle);
			}
			break;

		case kControlEditTextSelectionTag:
			if (inBuffer != nil) {
				GetSelection( *(ControlEditTextSelectionRec*) inBuffer );
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(ControlEditTextSelectionRec);
			}
			break;
			
		default:
			LControlPane::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			break;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusDraw														  [public]
// ---------------------------------------------------------------------------

Boolean
LEditText::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean	focused = LControlPane::FocusDraw();
	UTextTraits::SetPortTextTraits(mTextTraitsID);

	Pattern		whitePat;
	::BackPat( UQDGlobals::GetWhitePat(&whitePat) );

	return focused;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LEditText::DrawSelf()
{
	StCQDProcsState	qdProcs(mIsPassword, &sPasswordProcs,
								(CGrafPtr) UQDGlobals::GetCurrentPort());

	Rect	frame;
	CalcLocalFrameRect(frame);

	::MacInsetRect(&frame, 3, 3);				// Account for box
	::EraseRect(&frame);

	if (not IsEnabled() or not IsActive()) {	// Dim text color

		RGBColor	textColor;
		::GetForeColor(&textColor);
		UTextDrawing::DimTextColor(textColor);
		::RGBForeColor(&textColor);
	}

		// A Mac TERec stores a pointer to its owner port  We have to
		// change it to the current port in case we are drawing into
		// a port that is not the owner port. This happens when we are
		// printing or drawing into an offscreen port.

	GrafPtr	savePort = (**mTextEditH).inPort;
	(**mTextEditH).inPort = UQDGlobals::GetCurrentPort();
	::TEUpdate(&frame, mTextEditH);
	(**mTextEditH).inPort = savePort;

	mControlImp->DrawSelf();			// Imp draws box and focus ring
}


// ---------------------------------------------------------------------------
//	¥ HideSelf														  [public]
// ---------------------------------------------------------------------------

void
LEditText::HideSelf()
{
	if (IsOnDuty()) {					// Hidden field can't be the Target
		SwitchTarget(GetSuperCommander());
	}

	LControlPane::HideSelf();
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------

void
LEditText::ActivateSelf()
{
	if (mEnabled == triState_On) {
		Refresh();
	}
}



// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LEditText::DeactivateSelf()
{
	if (IsEnabled()) {
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LEditText::EnableSelf()
{
	if (mActive == triState_On) {
		Draw(nil);
		DontRefresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LEditText::DisableSelf()
{
	if (IsOnDuty()) {					// Disabled field can't be the Target
		SwitchTarget(GetSuperCommander());
	}

	if (mActive == triState_On) {
		Draw(nil);
		DontRefresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf													   [protected]
// ---------------------------------------------------------------------------

void
LEditText::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	if (!IsTarget()) {				// If not the Target, clicking in an
									//   EditField makes it the Target.

		if (::TEFeatureFlag(teFOutlineHilite, teBitTest, mTextEditH)
				== teBitClear) {

			// With outline hiliting OFF, switching target will call
			// TEActivate, which hilites the current selection. Since
			// TEClick will immediately change the selection, this
			// results in an ugly flash as the selection is hilite
			// and then unhilited. To avoid this, we clear the selection.

			FocusDraw();

			StCQDProcsState	qdProcs(mIsPassword, &sPasswordProcs,
								(CGrafPtr) UQDGlobals::GetCurrentPort());

			::TESetSelect(0, 0, mTextEditH);
		}

		SwitchTarget(this);
	}

	if (IsTarget()) {
		FocusDraw();
		AdjustTextWidth(true);

		StCQDProcsState	qdProcs(mIsPassword, &sPasswordProcs,
								(CGrafPtr) UQDGlobals::GetCurrentPort());

		::TEClick( inMouseDown.whereLocal,
				   ((inMouseDown.macEvent.modifiers & shiftKey) != 0),
				   mTextEditH );

		AdjustTextWidth(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouseSelf
// ---------------------------------------------------------------------------
//	EditField uses the standard I-Beam cursor

void
LEditText::AdjustMouseSelf(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			/* ioMouseRgn */)
{
	UCursor::SetIBeam();
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		inWidthDelta and inHeightDelta specify, in pixels, how much larger
//		to make the Frame. Positive deltas increase the size, negative deltas
//		reduce the size.

void
LEditText::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
									// Resize Pane
	LControlPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
	AlignTextEditRects();
}


// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//		inHorizDelta and inVertDelta specify, in pixels, how far to move the
//		Frame (within its surrounding Image). Positive horiz deltas move to
//		the right, negative to the left. Positive vert deltas move down,
//		negative up.

void
LEditText::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LControlPane::MoveBy(inHorizDelta, inVertDelta, inRefresh);
	AlignTextEditRects();
}


// ---------------------------------------------------------------------------
//	¥ AlignTextEditRects
// ---------------------------------------------------------------------------
//	Align the view and destination rectangles of the Toolbox TextEdit
//	record with the Frame of an EditField

void
LEditText::AlignTextEditRects()
{
	Rect	textFrame;
	CalcLocalFrameRect(textFrame);
									// Account for the box and focus ring
	::MacInsetRect(&textFrame, 5, 5);
									// Set TextEdit view and dest rectangles
									//   to be the same as the Frame
	(**mTextEditH).viewRect = textFrame;
	(**mTextEditH).destRect = textFrame;

	AdjustTextWidth(false);

	::TECalText(mTextEditH);		// Let TextEdit adjust line breaks
}


// ---------------------------------------------------------------------------
//	¥ RefreshText
// ---------------------------------------------------------------------------

void
LEditText::RefreshText()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	::MacInsetRect(&frame, 5, 5);			// Text is inside the box and
											//   focus ring
	RefreshRect(frame);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HandleKeyPress												  [public]
// ---------------------------------------------------------------------------

Boolean
LEditText::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
	Boolean		keyHandled	 = true;
	EKeyStatus	theKeyStatus = keyStatus_Input;
	UInt16		theChar		 = (UInt16) (inKeyEvent.message & charCodeMask);
	LCommander	*theTarget	 = GetTarget();

	if (inKeyEvent.modifiers & cmdKey) {	// Always pass up when the command
		theKeyStatus = keyStatus_PassUp;	//   key is down

	} else if (mKeyFilter != nil) {			// Filter key stroke
											// There should be a key filter.
											// Editing keys and undo won't work
											// without a proper key filter.
		theKeyStatus = (*mKeyFilter)(mTextEditH, (UInt16) inKeyEvent.message,
										theChar, inKeyEvent.modifiers);
	}

	StFocusAndClipIfHidden	focus(this);
	StCQDProcsState			qdProcs(false, nil,
									(CGrafPtr) UQDGlobals::GetCurrentPort());

											// Save whether current selection
											//   is empty or not
	bool	beforeSel = (**mTextEditH).selStart == (**mTextEditH).selEnd;

	bool	textChanged = false;
	bool	passUp = false;

	switch (theKeyStatus) {

		case keyStatus_Input: {
			if (TooManyCharacters(1)) {
				::SysBeep(1);
				break;
			}

			qdProcs.SetProcs(mIsPassword, &sPasswordProcs);

			if (mTypingAction == nil) {
				mTypingAction = new LTETypingAction(mTextEditH, this, this);
				PostAction(mTypingAction);
			}

			if (mTypingAction != nil) {
				try {
					mTypingAction->InputCharacter();
				}

				catch (...) {
					PostAction(nil);
				}
			}

			::TEKey((SInt16) theChar, mTextEditH);
			textChanged = true;
			break;
		}

		case keyStatus_TEDelete: {
			if ((**mTextEditH).selEnd > 0) {
				qdProcs.SetProcs(mIsPassword, &sPasswordProcs);

				if (mTypingAction == nil) {
					mTypingAction = new LTETypingAction(mTextEditH, this, this);
					PostAction(mTypingAction);
				}

				if (mTypingAction != nil) {
					try {
						mTypingAction->BackwardErase();
					}

					catch (...) {
						PostAction(nil);
					}
				}

				::TEKey(char_Backspace, mTextEditH);
				textChanged = true;
			}
			break;
		}

		case keyStatus_TECursor: {
			qdProcs.SetProcs(mIsPassword, &sPasswordProcs);
			::TEKey((SInt16) theChar, mTextEditH);
			break;
		}

		case keyStatus_ExtraEdit: {
			switch (theChar) {

				case char_Home:
					qdProcs.SetProcs(mIsPassword, &sPasswordProcs);
					::TESetSelect(0, 0, mTextEditH);
					break;

				case char_End:
					qdProcs.SetProcs(mIsPassword, &sPasswordProcs);
					::TESetSelect(max_Int16, max_Int16, mTextEditH);
					break;

				case char_FwdDelete:
					if ((**mTextEditH).selStart < (**mTextEditH).teLength) {
						qdProcs.SetProcs(mIsPassword, &sPasswordProcs);

						if (mTypingAction == nil) {
							mTypingAction = new LTETypingAction(mTextEditH, this, this);
							PostAction(mTypingAction);
						}

						if (mTypingAction != nil) {
							try {
								mTypingAction->ForwardErase();
							}

							catch (...) {
								PostAction(nil);
							}
						}

						if ((**mTextEditH).selStart == (**mTextEditH).selEnd) {
							::TESetSelect((**mTextEditH).selStart,
								(**mTextEditH).selStart + 1, mTextEditH);
						}

						::TEDelete(mTextEditH);
						textChanged = true;
					}
					break;

				default:
					passUp = true;
					break;
			}
			break;
		}

		case keyStatus_Reject:
			::SysBeep(1);
			break;

		case keyStatus_PassUp:
			passUp = true;
			break;
		
		default:
			break;
	}

	qdProcs.ResetProcs();

	if (textChanged) {
		UserChangedText();
	}

	if (passUp) {
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);
	}

		// If Target is the same before and after keystroke and
		// the selection has changed from empty to not empty or from
		// not empty to empty, we need to update menu commands. Some
		// command such as Cut and Copy depend on whether or not
		// any text is selected.

	if ( (theTarget == GetTarget()) &&
		 (beforeSel != ((**mTextEditH).selStart == (**mTextEditH).selEnd)) ) {

		SetUpdateCommandStatus(true);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------

Boolean
LEditText::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case cmd_Undo: {

				// !!! Warning !!!
				// Although we set the bulleted text qdProcs before performing
				// undo, this doesn't work if the LUndoer is attached
				// directly to the LEditText (the attachment intercepts the
				// command). Instead, attach the LUndoer to a supercommander,
				// such as the enclosing tag group or window. If you don't,
				// performing undo will show the contents in clear text.

			StCQDProcsState		qdProcs(mIsPassword, &sPasswordProcs,
									(CGrafPtr) GetMacPort());
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
		}

		case cmd_Cut:
			PostAction(new LTECutAction(mTextEditH, this, this));
			break;

		case cmd_Copy: {
			UScrap::ClearData();		// Clear first since on Mac OS X
			::TECopy(mTextEditH);		//   TEToScrap does nothing
			::TEToScrap();
			break;
		}

		case cmd_Paste:
		case cmd_Clear: {
			StFocusAndClipIfHidden	focus(this);
			StCQDProcsState		qdProcs(mIsPassword, &sPasswordProcs,
									(CGrafPtr) UQDGlobals::GetCurrentPort());

			if (inCommand == cmd_Paste) {

				if (TooManyCharacters(UScrap::GetData(ResType_Text, nil))) {
					::SysBeep(1);			// Paste would exceed max field size
				} else {
					PostAction(new LTEPasteAction(mTextEditH, this, this));
				}

			} else {
				PostAction(new LTEClearAction(mTextEditH, this, this));
			}
			break;
		}

		case cmd_ActionCut:
		case cmd_ActionPaste:
		case cmd_ActionClear:
		case cmd_ActionTyping:
			UserChangedText();
			break;

		case cmd_ActionDeleted:
			if ((static_cast<LTETypingAction*>(ioParam)) == mTypingAction) {
				mTypingAction = nil;
			}
			break;

		case msg_TabSelect:
			if (!IsEnabled()) {
				cmdHandled = false;
				break;
			} // else FALL THRU to SelectAll()

		case cmd_SelectAll:
			SelectAll();
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
LEditText::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Cut:				// Cut and Copy disabled for Password
		case cmd_Copy:				// Otherwise, enable if text is selected
			outEnabled = not mIsPassword && HasSelection();
			break;

		case cmd_Clear:				// Check if selection is not empty
			outEnabled = HasSelection();
			break;

		case cmd_Paste:				// Check if TEXT is in the Scrap
			outEnabled = UScrap::HasData(ResType_Text);
			break;

		case cmd_SelectAll:			// Check if any characters are present
			outEnabled = (**mTextEditH).teLength > 0;
			break;

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ UserChangedText
// ---------------------------------------------------------------------------
//	Text of EditText has changed as a result of user action
//
//	Broadcasts value message with a pointer to "this" object as the parameter.

void
LEditText::UserChangedText()
{
	if (mValueMessage != msg_Nothing) {
		BroadcastMessage(mValueMessage, this);
	}
}


// ---------------------------------------------------------------------------
//	¥ BeTarget													   [protected]
// ---------------------------------------------------------------------------

void
LEditText::BeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	StCQDProcsState			qdProcs(mIsPassword, &sPasswordProcs,
									(CGrafPtr) UQDGlobals::GetCurrentPort());

	::TEActivate(mTextEditH);

	mHasFocusRing = true;
	RedrawImp();
											// Flash insertion point
	StartIdling( ::TicksToEventTime( ::GetCaretTime() ) );
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget												   [protected]
// ---------------------------------------------------------------------------

void
LEditText::DontBeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	StCQDProcsState			qdProcs(mIsPassword, &sPasswordProcs,
									(CGrafPtr) UQDGlobals::GetCurrentPort());

	::TEDeactivate(mTextEditH);

	mHasFocusRing = false;
	RedrawImp();

	StopIdling();
}


// ---------------------------------------------------------------------------
//	¥ RedrawImp													   [protected]
// ---------------------------------------------------------------------------
//	Draw the control imp
//
//	The EditText must already be focused

void
LEditText::RedrawImp()
{
	if (IsVisible()) {
		if ( (sBitDepth > 0)  ||
			 UDrawingUtils::IsPortBuffered(GetMacPort()) ) {

			mControlImp->DrawSelf();

		} else {
			Rect	frame;
			CalcLocalFrameRect(frame);

			StColorDrawLoop		drawLoop(frame);

			while (drawLoop.NextDepth(sBitDepth)) {
				sDeviceHasColor = UDrawingUtils::DeviceSupportsColor(
									drawLoop.GetCurrentDevice());
				mControlImp->DrawSelf();
			}
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AdjustTextWidth
// ---------------------------------------------------------------------------
//	Adjust the width of the destination rectangle of the Toolbox TextEdit
//	record
//
//	This function does nothing if WordWrap is ON. If WordWrap is OFF, this
//	function sets the width of the TextEdit destination rectangle to either
//	the width of the text or a very large number, depending on the value
//	of inShrinkToText.
//
//	This adjustment is needed to make autoscrolling work properly when
//	WordWrap is off. While entering text, the destination rectangle should be
//	very wide so that the text doesn't word wrap. However, while clicking,
//	it should be just as wide as the text so that the EditField does not
//	autoscroll past the edge of the text.

void
LEditText::AdjustTextWidth(
	bool	inShrinkToText)
{
	if (!mHasWordWrap) {			// Adjust only if WordWrap is OFF

		Rect	textFrame;			// Get size of editable text area
		CalcLocalFrameRect(textFrame);
		::MacInsetRect(&textFrame, 5, 5);

		SInt16	destWidth = 4000;	// Very Wide

		if (inShrinkToText) {		// Width of the Text in the EditField

			Point	startPoint = ::TEGetPoint(0, mTextEditH);
			Point	endPoint = ::TEGetPoint((**mTextEditH).teLength,
												mTextEditH);
			destWidth = (SInt16) (endPoint.h - startPoint.h);

			if (destWidth < textFrame.right - textFrame.left) {
				destWidth = (SInt16) (textFrame.right - textFrame.left);
			}
		}

									// Direction to extend dest rect depends
									//   on the text justification
		SInt16	just = (**mTextEditH).just;
		if (just == teFlushDefault) {
									// For left justificaton, GetSysDirection
									//    returns teFlushDefault
									//    For right, teFlushRight
			just = ::GetSysDirection();
		}

		switch (just) {

			case teFlushLeft:		// Text fixed on left and grows right
			case teJustLeft:
				(**mTextEditH).destRect.right = (SInt16) ((**mTextEditH).destRect.left + destWidth);
				break;

			case teFlushRight:		// Text grows to the left
				(**mTextEditH).destRect.left = (SInt16) ((**mTextEditH).destRect.right - destWidth);
				break;

			case teCenter: {		// Text grows left and right
				SInt16	center = (SInt16) ((textFrame.left + textFrame.right) / 2);
				(**mTextEditH).destRect.left  = (SInt16) (center - 2000);
				(**mTextEditH).destRect.right = (SInt16) (center + 2000);
				break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ TooManyCharacters
// ---------------------------------------------------------------------------
//	Return whether adding the specified number of characters will exceed
//	the maximum allowed. This function assumes that the characters being
//	added will replace the current selection.

Boolean
LEditText::TooManyCharacters(
	SInt32	inCharsToAdd)
{
	return ( ((**mTextEditH).teLength +
			  inCharsToAdd -
			  ((**mTextEditH).selEnd - (**mTextEditH).selStart))
			 > mMaxChars );
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PasswordStdText									  [static] [protected]
// ---------------------------------------------------------------------------
//	Custom QD Proc for drawing text of password item. All characters in
//	a password draw as the character specified by calling SetPasswordChar().

pascal void
LEditText::PasswordStdText(
	SInt16		inByteCount,
	const void*	/* inTextPtr */,
	Point		inNumer,
	Point		inDenom)
{
	while (inByteCount > 0) {
		SInt16	count = sizeof(sPasswordChars);
		if (inByteCount < count) {
			count = inByteCount;
		}
										// Default text drawing routine
		::StdText(count, sPasswordChars, inNumer, inDenom);

		inByteCount -= count;
	}
}


// ---------------------------------------------------------------------------
//	¥ PasswordStdTxMeas									  [static] [protected]
// ---------------------------------------------------------------------------
//	Custom QD Proc for measuring text of password item. All characters in
//	a password draw as the character specified by calling SetPasswordChar().

pascal SInt16
LEditText::PasswordStdTxMeas(
	SInt16		inByteCount,
	const void*	/* inTextPtr */,
	Point*		ioNumer,
	Point*		ioDenom,
	FontInfo*	inFontInfo)
{
		// Note that we use a do/while loop so we call StdTxMeas()
		// even if inByteCount is zero. This is important since that
		// routine may adjust ioNumer and ioDenom even if there is
		// no text to measure.

	SInt16	width = 0;
	do {
		SInt16	count = sizeof(sPasswordChars);
		if (inByteCount < count) {
			count = inByteCount;
		}
		width += ::StdTxMeas(count, sPasswordChars, ioNumer, ioDenom, inFontInfo);
		inByteCount -= count;
	} while (inByteCount > 0);

	return width;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------

void
LEditText::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	StColorPortState	savePortState(GetMacPort());

	if (not HasSelection() && FocusExposed()) {
		StCQDProcsState	qdProcs(mIsPassword, &sPasswordProcs,
									(CGrafPtr) UQDGlobals::GetCurrentPort());
		::TEIdle(mTextEditH);
		LView::OutOfFocus(nil);
	}
}


PP_End_Namespace_PowerPlant
