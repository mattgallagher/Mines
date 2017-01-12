// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditField.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Pane containing editable text
//
//	Use an EditField for small amounts of monostyled text, such as a text
//	entry field in a dialog box. EditField uses the Toolbox TextEdit
//	engine. It is not a View, so you can't put an EditField in a Scroller.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UTETextAction.h>
#include <LEditField.h>
#include <LStream.h>
#include <LView.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UScrap.h>
#include <UTextTraits.h>

#include <Script.h>
#include <Sound.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LEditField							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LEditField::LEditField()
{
	InitEditField(editAttr_Box);
}


// ---------------------------------------------------------------------------
//	¥ LEditField							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LEditField::LEditField(
	const LEditField&	inOriginal)

	: LPane(inOriginal),
	  LCommander(inOriginal)
{
	StFocusAndClipIfHidden	focus(this);

	mKeyFilter		= inOriginal.mKeyFilter;
	mMaxChars		= inOriginal.mMaxChars;
	mHasBox			= inOriginal.mHasBox;
	mHasWordWrap	= inOriginal.mHasWordWrap;
	mTypingAction	= nil;

	Rect	viewRect;
	viewRect.left	= (SInt16) -mFrameSize.width;
	viewRect.right	= 0;
	viewRect.top	= (SInt16) -mFrameSize.height;
	viewRect.bottom	= 0;

	mTextEditH = ::TENew(&viewRect, &viewRect);

	SetTextTraitsID(inOriginal.mTextTraitsID);

	Str255	theText;
	SetDescriptor(inOriginal.GetDescriptor(theText));

	AlignTextEditRects();
}


// ---------------------------------------------------------------------------
//	¥ LEditField							Constructor				  [public]
// ---------------------------------------------------------------------------

LEditField::LEditField(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString,
	ResIDT				inTextTraitsID,
	SInt16				inMaxChars,
	Boolean				inHasBox,
	Boolean				inHasWordWrap,
	TEKeyFilterFunc		inKeyFilter,
	LCommander*			inSuper)

	: LPane(inPaneInfo),
	  LCommander(inSuper)
{
	UInt8	attributes = 0;
	if (inHasBox) {
		attributes += editAttr_Box;
	}
	if (inHasWordWrap) {
		attributes += editAttr_WordWrap;
	}
	InitEditField(attributes);

	if (inString[0] > 0) {
		::TESetText(inString + 1, inString[0], mTextEditH);
	}
	SetTextTraitsID(inTextTraitsID);

	mMaxChars		= inMaxChars;
	mHasBox			= inHasBox;
	mHasWordWrap	= inHasWordWrap;
	mKeyFilter		= inKeyFilter;

	AlignTextEditRects();
	SelectAll();
}


// ---------------------------------------------------------------------------
//	¥ LEditField							Constructor				  [public]
// ---------------------------------------------------------------------------

LEditField::LEditField(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString,
	ResIDT				inTextTraitsID,
	SInt16				inMaxChars,
	UInt8				inAttributes,
	TEKeyFilterFunc		inKeyFilter,
	LCommander*			inSuper)

	: LPane(inPaneInfo),
	  LCommander(inSuper)
{
	InitEditField(inAttributes);

	if (inString[0] > 0) {
		::TESetText(inString + 1, inString[0], mTextEditH);
	}
	SetTextTraitsID(inTextTraitsID);
	mMaxChars  = inMaxChars;
	mKeyFilter = inKeyFilter;

	AlignTextEditRects();
	SelectAll();
}


// ---------------------------------------------------------------------------
//	¥ LEditField							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LEditField::LEditField(
	LStream*	inStream)

	: LPane(inStream)
{
	Str255	initialText;
	ResIDT	textTraitsID;
	SInt16	maxChars;
	UInt8	attributes;
	SInt8	keyFilterID;

	*inStream >> initialText;
	*inStream >> textTraitsID;
	*inStream >> maxChars;
	*inStream >> attributes;
	*inStream >> keyFilterID;

		// Turn off autoscroll for centered and right justified text.
		// It doesn't work.

	TextTraitsH	traitsH = UTextTraits::LoadTextTraits(textTraitsID);
	if (traitsH != nil) {
		if ( ((**traitsH).justification == teJustRight)  ||
			 ((**traitsH).justification == teJustCenter) ) {
			attributes = (UInt8) (attributes & ~editAttr_AutoScroll);
		}
	}

	InitEditField(attributes);

	if (initialText[0] > 0) {
		::TESetText(initialText+1, initialText[0], mTextEditH);
	}
	SetTextTraitsID(textTraitsID);
	mMaxChars = maxChars;

		// Determine which function to use to filter keystrokes

	mKeyFilter = UKeyFilters::SelectTEKeyFilter(keyFilterID);

	if (mKeyFilter == nil) {		// Use default key filter. Typing and
									//   undo won't work right without
									//   a key filter.
		mKeyFilter = UKeyFilters::PrintingCharField;
	}

	AlignTextEditRects();
	SelectAll();
}


// ---------------------------------------------------------------------------
//	¥ InitEditField
// ---------------------------------------------------------------------------
//	Initialize member variables of a EditField to default values

void
LEditField::InitEditField(
	UInt8	inAttributes)
{
	mKeyFilter		= nil;
	mMaxChars		= 255;
	mTextTraitsID	= 0;
	mTypingAction	= nil;

	StFocusAndClipIfHidden	focus(this);

		// Put initial TE Rects offscreen. They will be aligned
		// with the Frame later. PP coordinates always start at (0,0),
		// so negative coords are always offscreen.

	Rect	viewRect;
	viewRect.left	= (SInt16) -mFrameSize.width;
	viewRect.right	= 0;
	viewRect.top	= (SInt16) -mFrameSize.height;
	viewRect.bottom	= 0;

	mTextEditH = ::TENew(&viewRect, &viewRect);

		// Set optional features of an EditField

	mHasBox = (inAttributes & editAttr_Box) != 0;
	mHasWordWrap = (inAttributes & editAttr_WordWrap) != 0;

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
//	¥ ~LEditField							Destructor				  [public]
// ---------------------------------------------------------------------------

LEditField::~LEditField()
{
	if (mTypingAction != nil) {
		PostAction(nil);
	}

	if (mTextEditH != nil) {
		::TEDispose(mTextEditH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return the first 255 characters of the EditField as a Pascal string
//
//	Caller must allocate a Str255 variable for storing the string

StringPtr
LEditField::GetDescriptor(
	Str255	outDescriptor) const
{
	CharsHandle	theText = ::TEGetText(mTextEditH);
	long	len = ::GetHandleSize(theText);
	if (len > 255) {					// String is limited to 255 chars
		len = 255;
	}
	::BlockMoveData(*theText, outDescriptor+1, len);
	outDescriptor[0] = (UInt8) len;
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Set the contents of an EditField from a Pascal string

void
LEditField::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	StFocusAndClipIfHidden	focus(this);
	StClipRgnState	clip(nil);
	::TESetText(inDescriptor+1, StrLength(inDescriptor), mTextEditH);
	SelectAll();
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------
//	Return the integer value represented by the contents of an EditField
//
//	An empty or non-numerical EditField evaluates to zero

SInt32
LEditField::GetValue() const
{
	SInt32	theNumber = 0;
	Str255	str;

	GetDescriptor(str);
	if (str[0] > 0) {
		::StringToNum(str, &theNumber);
	}
	return theNumber;
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set the contents of an EditField to the string representation of a
//	specified integer number

void
LEditField::SetValue(
	SInt32	inValue)
{
	Str255	numberAsString;
	::NumToString(inValue, numberAsString);
	SetDescriptor(numberAsString);
}


// ---------------------------------------------------------------------------
//	¥ SetMaxChars
// ---------------------------------------------------------------------------
//	Specify the maximum number of characters that an EditField can contain

void
LEditField::SetMaxChars(
	SInt16	inMaxChars)
{
	mMaxChars = inMaxChars;
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID
// ---------------------------------------------------------------------------
//	Specify the resource ID of the TextTraits for an EditField
//
//	This function updates the line height to fit the text characteristics.

void
LEditField::SetTextTraitsID(
	ResIDT	inTextTraitsID)
{
	mTextTraitsID = inTextTraitsID;
	UTextTraits::SetTETextTraits(mTextTraitsID, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ UseWordWrap
// ---------------------------------------------------------------------------
//	Specify whether the EditField word wraps to its frame

void
LEditField::UseWordWrap(
	Boolean	inSetting)
{
	if (mHasWordWrap != inSetting) {
		mHasWordWrap = inSetting;
		AlignTextEditRects();
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ HasSelection													  [public]
// ---------------------------------------------------------------------------

bool
LEditField::HasSelection() const
{										// Selection start and end are
										//   different if text is selected
	return ((**mTextEditH).selStart != (**mTextEditH).selEnd);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------
//	Prepare for drawing in the EditField

Boolean
LEditField::FocusDraw(
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
//	Draw an EditField

void
LEditField::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	if (mHasBox) {				// Draw Border if necessary
		DrawBox();
		::MacInsetRect(&frame, 2, 2);
	}
	
	Pattern		whitePat;
	::BackPat(UQDGlobals::GetWhitePat(&whitePat));
	
	::EraseRect(&frame);

		// A Mac TERec stores a pointer to its owner port  We have to
		// change it to the current port in case we are drawing into
		// a port that is not the owner port. This happens when we are
		// printing or drawing into an offscreen port.

	GrafPtr	savePort = (**mTextEditH).inPort;
	(**mTextEditH).inPort = UQDGlobals::GetCurrentPort();

	::TEUpdate(&frame, mTextEditH);

	(**mTextEditH).inPort = savePort;
}


// ---------------------------------------------------------------------------
//	¥ DrawBox
// ---------------------------------------------------------------------------
//	Draw box around an EditField

void
LEditField::DrawBox()
{
		// Box around an EditField is outset from the Text by 2 pixels.
		// The box itself is 1 pixel thick, drawn in the foreground color
		// of the Pane (not necessarily the same as the text color). If
		// the EditField is disabled, the box draws with a gray pattern.
		//
		// The 1 pixel rectangle between the box and the text draws in
		// the background color of the text.

	StColorState	saveColors;			// Preserve color state
	Rect			frame;
	CalcLocalFrameRect(frame);
	::PenNormal();

		// Draw empty area between Box and Text. On entry, the fore
		// and back colors for the Text are in effect (as set in FocusDraw).
		// Use Text back color as the fore color for the empty area.

	::MacInsetRect(&frame, 1, 1);
	RGBColor	emptyColor;
	::GetBackColor(&emptyColor);
	::RGBForeColor(&emptyColor);
	::MacFrameRect(&frame);

		// Draw border around EditField

	::MacInsetRect(&frame, -1, -1);

	if (mEnabled != triState_On) {		// Disabled EditField has gray border
		StColorPenState::SetGrayPattern();
	}

	ApplyForeAndBackColors();
	::MacFrameRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	Hide an EditField. An invisible EditField can't be OnDuty.

void
LEditField::HideSelf()
{
	if (IsOnDuty()) {				// Shouldn't be on duty when invisible
		SwitchTarget(GetSuperCommander());
	}
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf
// ---------------------------------------------------------------------------

void
LEditField::EnableSelf()
{
	if (mHasBox && FocusExposed()) {
		DrawBox();
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf
// ---------------------------------------------------------------------------

void
LEditField::DisableSelf()
{
	if (IsOnDuty()) {				// Shouldn't be on duty when disabled
		SwitchTarget(GetSuperCommander());
	}

	if (mHasBox && FocusExposed()) {
		DrawBox();
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside an EditField

void
LEditField::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
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
			::TESetSelect(0, 0, mTextEditH);
		}

		SwitchTarget(this);
	}

	if (IsTarget()) {
		FocusDraw();
		AdjustTextWidth(true);
		::TEClick(inMouseDown.whereLocal,
					((inMouseDown.macEvent.modifiers & shiftKey) != 0),
					mTextEditH);
		AdjustTextWidth(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouseSelf
// ---------------------------------------------------------------------------
//	EditField uses the standard I-Beam cursor

void
LEditField::AdjustMouseSelf(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			/* ioMouseRgn */)
{
	UCursor::SetIBeam();
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Handle standard editing commands

Boolean
LEditField::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case cmd_Cut:
			PostAction(new LTECutAction(mTextEditH, this, this));
			break;

		case cmd_Copy:
			UScrap::ClearData();		// Clear first since on Mac OS X
			::TECopy(mTextEditH);		//   TEToScrap does nothing
			::TEToScrap();
			break;

		case cmd_Paste: {
			if (TooManyCharacters(UScrap::GetData(ResType_Text, nil))) {
				::SysBeep(1);			// Paste would exceed max field size
			} else {
				PostAction(new LTEPasteAction(mTextEditH, this, this));
			}
			break;
		}

		case cmd_Clear:
			PostAction(new LTEClearAction(mTextEditH, this, this));
			break;

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
LEditField::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Cut:				// Cut, Copy, and Clear enabled
		case cmd_Copy:				//   if something is selected
		case cmd_Clear:
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
//	¥ SetKeyFilter
// ---------------------------------------------------------------------------
//	Specify the function for filtering keystrokes

void
LEditField::SetKeyFilter(
	TEKeyFilterFunc	inKeyFilter)
{
	mKeyFilter = inKeyFilter;
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress
// ---------------------------------------------------------------------------
//	Handle key stroke directed at an EditField
//
//	Return true if the EditField handles the keystroke

Boolean
LEditField::HandleKeyPress(
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

											// Save whether current selection
											//   is empty or not
	bool beforeSel = (**mTextEditH).selStart == (**mTextEditH).selEnd;

	switch (theKeyStatus) {

		case keyStatus_Input:
				// Check if we are at the character limit
				// ### Not two-byte char compatible
			if (TooManyCharacters(1)) {
				::SysBeep(1);
				break;
			}

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
			UserChangedText();
			break;

		case keyStatus_TEDelete: {
			if ((**mTextEditH).selEnd > 0) {
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
				UserChangedText();
			}
			break;
		}

		case keyStatus_TECursor: {
			::TEKey((SInt16) theChar, mTextEditH);
			break;
		}

		case keyStatus_ExtraEdit: {
			switch (theChar) {

				case char_Home:
					::TESetSelect(0, 0, mTextEditH);
					break;

				case char_End:
					::TESetSelect(max_Int16, max_Int16, mTextEditH);
					break;

				case char_FwdDelete:
					if ((**mTextEditH).selStart < (**mTextEditH).teLength) {
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
						UserChangedText();
					}
					break;

				default:
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
					break;
			}
			break;
		}

		case keyStatus_Reject:
			::SysBeep(1);
			break;

		case keyStatus_PassUp:
			keyHandled = LCommander::HandleKeyPress(inKeyEvent);
			break;
		
		default:
			break;
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
//	¥ SelectAll
// ---------------------------------------------------------------------------
//	Select entire contents of an EditField

void
LEditField::SelectAll()
{
	StFocusAndClipIfHidden	focus(this);
	::TESetSelect(0, max_Int16, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ AlignTextEditRects
// ---------------------------------------------------------------------------
//	Align the view and destination rectangles of the Toolbox TextEdit
//	record with the Frame of an EditField

void
LEditField::AlignTextEditRects()
{
	Rect	textFrame;
	if (!CalcLocalFrameRect(textFrame)) {

				// Frame is outside QD Space. Put textFrame  at the
				// upper left limit of QD Space (extreme negative coords).
				// That location is guaranteed to be offscreen (unless
				// you have a field longer than 32K pixels) since PP
				// Image coordinates start at (0,0) and are never negative.

		textFrame.left		= min_Int16;
		textFrame.right		= (SInt16) (textFrame.left + mFrameSize.width);
		textFrame.top		= min_Int16;
		textFrame.bottom	= (SInt16) (textFrame.top + mFrameSize.height);
	}

	if (mHasBox) {
		::MacInsetRect(&textFrame, 2, 2);
	}
									// Set TextEdit view and dest rectangles
									//   to be the same as the Frame
	(**mTextEditH).viewRect = textFrame;
	(**mTextEditH).destRect = textFrame;

	AdjustTextWidth(false);

	::TECalText(mTextEditH);		// Let TextEdit adjust line breaks
}


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
LEditField::AdjustTextWidth(
	Boolean	inShrinkToText)
{
	if (!mHasWordWrap) {			// Adjust only if WordWrap is OFF

		Rect	textFrame;			// Get size of editable text area
		CalcLocalFrameRect(textFrame);
		if (mHasBox) {
			::MacInsetRect(&textFrame, 2, 2);
		}

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
LEditField::TooManyCharacters(
	SInt32	inCharsToAdd)
{
	return ( ((**mTextEditH).teLength +
			  inCharsToAdd -
			  ((**mTextEditH).selEnd - (**mTextEditH).selStart))
			 > mMaxChars );
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
LEditField::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
									// Resize Pane
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
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
LEditField::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LPane::MoveBy(inHorizDelta, inVertDelta, inRefresh);
	AlignTextEditRects();
}


// ---------------------------------------------------------------------------
//	¥ BeTarget
// ---------------------------------------------------------------------------
//	EditField is becoming the Target

void
LEditField::BeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	::TEActivate(mTextEditH);		// Show active selection
									// Idle time used to flash the cursor
	StartIdling( ::TicksToEventTime( ::GetCaretTime() ) );
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget
// ---------------------------------------------------------------------------
//	EditField is no longer the Target
//
//	Remove EditField from IdleQueue

void
LEditField::DontBeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	::TEDeactivate(mTextEditH);		// Show inactive selection
	StopIdling();					// Stop flashing the cursor
}


// ---------------------------------------------------------------------------
//	¥ SpendTime
// ---------------------------------------------------------------------------
//	Idle time: Flash the insertion cursor

void
LEditField::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	StColorPortState	savePortState(GetMacPort());

	if (not HasSelection() && FocusExposed()) {
		::TEIdle(mTextEditH);
		LView::OutOfFocus(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ UserChangedText
// ---------------------------------------------------------------------------
//	Text of EditField has changed as a result of user action
//
//	Override to validate field and/or dynamically update as the user
//	types. This function is not called by SetDescriptor, which is typically
//	used to programatically change the text.

void
LEditField::UserChangedText()
{
}


// ---------------------------------------------------------------------------
//	¥ GetSelection
// ---------------------------------------------------------------------------
//	Passes back an AEDesc of the currently selected text

void
LEditField::GetSelection(
	AEDesc&		outDesc) const
{
	SInt16	selStart = (**mTextEditH).selStart;
	SInt16	selEnd   = (**mTextEditH).selEnd;

	StHandleLocker lock((Handle) (**mTextEditH).hText);

	OSErr err = ::AECreateDesc(typeChar,
							   (*(**mTextEditH).hText + selStart),
							   (selEnd - selStart),
							   &outDesc);

	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ SavePlace
// ---------------------------------------------------------------------------
//	Save TextEdit rectangles

void
LEditField::SavePlace(
	LStream*	outPlace)
{
	LPane::SavePlace(outPlace);

	Rect	viewRect = (**mTextEditH).viewRect;
	*outPlace << viewRect;

	Rect	destRect = (**mTextEditH).destRect;
	*outPlace << destRect;
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace
// ---------------------------------------------------------------------------
//	Save TextEdit rectangles

void
LEditField::RestorePlace(
	LStream*	inPlace)
{
	LPane::RestorePlace(inPlace);

	Rect	viewRect;
	*inPlace >> viewRect;
	(**mTextEditH).viewRect = viewRect;

	Rect	destRect;
	*inPlace >> destRect;
	(**mTextEditH).destRect = destRect;
}


PP_End_Namespace_PowerPlant
