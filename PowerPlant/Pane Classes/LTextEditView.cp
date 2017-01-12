// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextEditView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Implements basic text editing via TextEdit.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTextEditView.h>
#include <TArrayIterator.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <PP_KeyCodes.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UKeyFilters.h>
#include <UMemoryMgr.h>
#include <UScrap.h>
#include <UTextTraits.h>
#include <UTEViewTextAction.h>

#include <Fonts.h>
#include <Sound.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LTextEditView*			LTextEditView::sTextEditViewP;


// ---------------------------------------------------------------------------
//	¥ LTextEditView							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LTextEditView::LTextEditView()
{
	mTextAttributes = 0;
	InitTextEditView(0);
}


// ---------------------------------------------------------------------------
//	¥ LTextEditView							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTextEditView::LTextEditView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	UInt16				inTextAttributes,
	ResIDT				inTextTraitsID)

	: LView(inPaneInfo, inViewInfo)
{
	mTextAttributes = inTextAttributes;
	InitTextEditView(inTextTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ LTextEditView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTextEditView::LTextEditView(
	LStream*	inStream)

	: LView(inStream)
{
	*inStream >> mTextAttributes;

	ResIDT	textTraitsID;
	*inStream >> textTraitsID;

	ResIDT	initialTextID;
	*inStream >> initialTextID;

	InitTextEditView(textTraitsID);

	StResource	initialTextRes(ResType_Text, initialTextID, false);

	if (initialTextRes.mResourceH != nil) {

		StResource	initialStyleRes;
		if ( HasAttribute(textAttr_MultiStyle) ) {
			initialStyleRes.GetResource(ResType_TextStyle, initialTextID, false);
		}

		SetTextHandle(initialTextRes,
					(StScrpHandle) initialStyleRes.mResourceH);

		::TESetSelect(0, 0, mTextEditH);
		AlignTextEditRects();
		AdjustImageToText();
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LTextEditView						Destructor				  [public]
// ---------------------------------------------------------------------------

LTextEditView::~LTextEditView()
{
	if ( mClickLoopUPP != nil ) {
		DisposeTEClickLoopUPP(mClickLoopUPP);
	}

	if ( mTextEditH != nil ) {
		::TEDispose(mTextEditH);
		mTextEditH = nil;
	}

	// We don't dispose of the autscroll callback cause that would
	// affect any other LTextEditView instances	we have around.

	if (mTypingAction != nil) {			// Clean out the undo queue
		try {
			PostAction(nil);
		} catch (...) {
			// don't let the exception propagate
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ InitTextEditView						[private]
// ---------------------------------------------------------------------------
//	Private initializer

void
LTextEditView::InitTextEditView(
	ResIDT inTextTraitsID )
{
	mTypingAction = nil;
	mTextTraitsID = inTextTraitsID;
	mClickLoopUPP = nil;

	// TE Record relies on the current port for some of its settings

	StFocusAndClipIfHidden	focus(this);

	// Put initial TE Rects offscreen. They will be aligned
	// with the Frame later. PP coordinates always start at (0,0),
	// so negative coords are always offscreen.

	Rect	viewRect;
	::MacSetRect(	&viewRect,
					static_cast<SInt16>(-mFrameSize.width),
					static_cast<SInt16>(-mFrameSize.height),
					0,
					0 );

	if ( HasAttribute( textAttr_MultiStyle ) ) {

		// TextTraits are not used in multi-style text edit, however
		// they are used this once to set the initial text traits
		// for the TextEdit instance.

		mTextTraitsID = -1;
		TextTraitsH theTxtrH = UTextTraits::LoadTextTraits(inTextTraitsID);

		TextStyle	theStyle;
		SInt16		justification = teFlushDefault;

		if ( theTxtrH != nil ) {
			theStyle.tsFont			= (*theTxtrH)->fontNumber;
			theStyle.tsFace			= static_cast<UInt8>((*theTxtrH)->style);
			theStyle.tsSize			= (*theTxtrH)->size;
			theStyle.tsColor		= (*theTxtrH)->color;
			justification			= (*theTxtrH)->justification;

			::ReleaseResource(reinterpret_cast<Handle>(theTxtrH));
		} else {
			// We'll just use some basic defaults

			theStyle.tsFont			= ::GetAppFont();
			theStyle.tsFace			= normal;
			theStyle.tsSize			= ::GetDefFontSize();
			theStyle.tsColor.red	= 0;
			theStyle.tsColor.green	= 0;
			theStyle.tsColor.blue	= 0;
		}

		::TextFont(theStyle.tsFont);		// Set Port Font and Size so TE
		::TextSize(theStyle.tsSize);		//   uses the correct settings for
											//   its internal tables

		mTextEditH = ::TEStyleNew(&viewRect, &viewRect);
		ThrowIfNil_(mTextEditH);

		::TESetStyle( doAll, &theStyle, false, mTextEditH );
		::TESetAlignment( justification, mTextEditH );

	} else {
		mTextEditH = ::TENew(&viewRect, &viewRect);
		ThrowIfNil_(mTextEditH);

		// Set the initial text traits

		UTextTraits::SetTETextTraits(mTextTraitsID, mTextEditH);

		SPoint32	scrollUnit;
		scrollUnit.h = 4;
		scrollUnit.v = (*mTextEditH)->lineHeight;
		SetScrollUnit(scrollUnit);
	}

	// If word wrap is on, then the Image width is always the
	// same as the Frame width, which forces text to wrap to
	// the Frame.

	// If the Image width is zero (or negative), the user
	// probably forgot to set it. To accommodate this error,
	// we set the Image width to the Frame width. However, the
	// Image will not change if the Frame resizes.

	if (HasAttribute(textAttr_WordWrap) || (mImageSize.width <= 0)) {
		mImageSize.width = mFrameSize.width;
	}

	// outline hiliting

	::TEFeatureFlag(teFOutlineHilite,
					HasAttribute(textAttr_OutlineHilite),
					mTextEditH);

	AlignTextEditRects();
										// Set the clickloop
	if (HasAttribute(textAttr_AutoScroll)) {
		sTextEditViewP = this;
		SetClickLoop( MyClickLoop );
	} else {
		sTextEditViewP = nil;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ClickSelf								[protected, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	if (!HasAttribute(textAttr_Selectable)) {
		return;
	}

	if (!IsTarget()) {				// If not the Target, clicking in an
									//   TextEdit makes it the Target.
									// Since TEClick will set a new selection
									//   range, clear the current selection
									//   range to avoid an ugly selection flash.
		FocusDraw();

		// If autoscrolling, TESetSelect can cause autoscrolling. That means well
		// but can cause some cosmetic problems, especially if selStart is not
		// visible (you'll see a jump).

		if ( HasAttribute(textAttr_AutoScroll) )
			::TEFeatureFlag( teFAutoScroll, teBitClear, mTextEditH );

		::TESetSelect(0, 0, mTextEditH);

		if ( HasAttribute(textAttr_AutoScroll) )
			::TEFeatureFlag( teFAutoScroll, teBitSet, mTextEditH );

		// resync everything

		AlignTextEditRects();

		// force a redraw (to clean up the display). Draw(nil) causes a lot
		// less flicker than Refresh().

		Draw(nil);

		// and make ourselves the target.

		SwitchTarget(this);
	}

	if (IsTarget()) {
		FocusDraw();
		::TEClick( inMouseDown.whereLocal,
				   ((inMouseDown.macEvent.modifiers & shiftKey) != 0),
				   mTextEditH);
		LView::OutOfFocus(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ MyClickLoop							[static, public]
// ---------------------------------------------------------------------------
//	This is the clikLoop callback used to help autoscrolling while
//	selecting
//
//	On Classic 68K, this Toolbox callback passes its parameters in
//	registers, so the C prototype has no function parameters.
//
//	On other runtimes, the callback has a TERec* parameter.

#pragma mark LTextEditView::MyClickLoop

#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM	// Classic 68K

pascal Boolean
LTextEditView::MyClickLoop()		// No parameters on the stack

#else

pascal Boolean
LTextEditView::MyClickLoop(
	TERec* /* inTERec */)			// One parameter on the stack

#endif
{
	try {

		// Make sure we're in there to do the voodoo that this do

		if (sTextEditViewP != nil) {
			Point	where;
			::GetMouse( &where );

			if ( !::MacPtInRect(where, &(*(sTextEditViewP->GetMacTEH()))->viewRect)) {
				sTextEditViewP->ClickAutoScroll(where);
			}
		}
	}

	catch (...) { }		// Can't let an exception propagate back to Toolbox

	return true;
}


// ---------------------------------------------------------------------------
//	¥ SetClickLoop							[public, virtual]
// ---------------------------------------------------------------------------
//	Allows you to specify your own clikLoop.  Disposes of the existing
//	clikLoop, if any.  Also since the point of a clikLoop is for autoscrolling
//	if we aren't set to autoscroll, we set that now.

#pragma mark LTextEditView::SetClickLoop

#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM	// Classic 68K

void
LTextEditView::SetClickLoop( void* inClickLoop )

#else

void
LTextEditView::SetClickLoop( TEClickLoopProcPtr inClickLoop )

#endif
{
	if (inClickLoop != nil) {

		if (mClickLoopUPP != nil) {			// Dispose of the previous clikLoop
			DisposeTEClickLoopUPP(mClickLoopUPP);
			mClickLoopUPP = nil;
		}

		mClickLoopUPP = NewTEClickLoopUPP( (TEClickLoopProcPtr) inClickLoop );
		ThrowIfNil_(mClickLoopUPP);

		(*mTextEditH)->clickLoop = mClickLoopUPP;

		::TEFeatureFlag( teFAutoScroll, teBitSet, mTextEditH );
		mTextAttributes |= textAttr_AutoScroll;	// set the autoscroll bit on
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ForceAutoScroll						[protected, virtual]
// ---------------------------------------------------------------------------
//	This routine works with TextEdit's autoscrolling capabilities
//	to keep all things (e.g. the scrollbars) in sync

void
LTextEditView::ForceAutoScroll(
	const Rect	&inOldDestRect)
{
	if (!HasAttribute(textAttr_AutoScroll)) {	// If we're not autoscrolling,
		return;									//   we can leave
	}

	FocusDraw();

	::TESelView(mTextEditH);			// TextEdit autoscroll

										// Now line everything up
	SInt32 leftDelta = inOldDestRect.left - (*mTextEditH)->destRect.left;
	SInt32 topDelta = inOldDestRect.top - (*mTextEditH)->destRect.top;

	if (leftDelta || topDelta) {
		mImageLocation.h -=  leftDelta;
		mImageLocation.v -= topDelta;
		CalcPortOrigin();
		LView::OutOfFocus(this);

 		AlignTextEditRects();

		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane*	theSub;
		while (iterator.Next(theSub)) {
			theSub->AdaptToSuperScroll(leftDelta, topDelta);
		}

		if (mSuperView) {
			mSuperView->SubImageChanged(this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickAutoScroll						[protected, virtual]
// ---------------------------------------------------------------------------
//	This is the routine ultimately used in the clikLoop to perform
//	the scrolling while clicking.

void
LTextEditView::ClickAutoScroll(
	const Point	&inLocalPoint)
{
	LView::AutoScrollImage(inLocalPoint);

	FocusDraw();			// Since autoscrolling occurs during a Toolbox
							// callback, we need to set the focus to what
							// the Toolbox expects
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress							[public, virtual]
// ---------------------------------------------------------------------------

Boolean
LTextEditView::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	Boolean		keyHandled	 = true;
	EKeyStatus	theKeyStatus = keyStatus_Input;
	SInt16		theKey		 = (SInt16) (inKeyEvent.message & charCodeMask);
	LCommander*	theTarget	 = GetTarget();

	if (inKeyEvent.modifiers & cmdKey) {	// Always pass up when the command
		theKeyStatus = keyStatus_PassUp;	//   key is down
	} else {

		theKeyStatus = UKeyFilters::PrintingCharField(inKeyEvent);
	}

	if ((theKeyStatus == keyStatus_PassUp) && (theKey == char_Return)) {
		theKeyStatus = keyStatus_Input;		// Special case for Return key
	}

		// Pass up when text is not editable. The only exception is
		// cursor keys when the text is selectable.

	if ( not HasAttribute(textAttr_Editable)  and
		 not ( HasAttribute(textAttr_Selectable) and
		 	   (theKeyStatus == keyStatus_TECursor) ) ) {
	
		theKeyStatus = keyStatus_PassUp;
	}

	SInt16	lineCount = (*mTextEditH)->nLines;
	Rect oldDestRect  = (*mTextEditH)->destRect;

	StFocusAndClipIfHidden	focus(this);
	
											// Save whether current selection
											//   is empty or not
	bool	beforeSel = (**mTextEditH).selStart == (**mTextEditH).selEnd;
	
	bool	textChanged = false;
	bool	passUp = false;

	switch (theKeyStatus) {

		case keyStatus_Input: {
											// Check for a 32k of text limit
			if ((*mTextEditH)->teLength >= kMaxChars) {
				::SysBeep(3); // do something to complain
				SignalStringLiteral_("Cannot enter more than 32k of text");

			} else {

				if (mTypingAction == nil) {
					mTypingAction = new LTEViewTypingAction(mTextEditH, this, this);
					PostAction(mTypingAction);
				}

				if (mTypingAction != nil) {
					try {
						mTypingAction->InputCharacter();
					} catch(...) {
						PostAction(nil);
					}
				}

				::TEKey(theKey, mTextEditH);
				ForceAutoScroll(oldDestRect);

				textChanged = true;
			}
			break;
		}


		case keyStatus_TEDelete: {

			if ((*mTextEditH)->selEnd > 0) {
				if (mTypingAction == nil) {
					mTypingAction = new LTEViewTypingAction(mTextEditH, this, this);
					PostAction(mTypingAction);
				}

				if (mTypingAction != nil) {
					try {
						mTypingAction->BackwardErase();
					} catch (...) {
						PostAction(nil);
					}
				}

				::TEKey(char_Backspace, mTextEditH);
				ForceAutoScroll(oldDestRect);
				textChanged = true;
			}
			break;
		}

		case keyStatus_TECursor: {
			::TEKey(theKey, mTextEditH);
			ForceAutoScroll(oldDestRect);
			break;
		}

		case keyStatus_ExtraEdit: {
			if (theKey == char_FwdDelete) {

				if ((*mTextEditH)->selStart < (*mTextEditH)->teLength) {
					if (mTypingAction == nil) {
						mTypingAction = new LTEViewTypingAction(mTextEditH, this, this);
						PostAction(mTypingAction);
					}

					if (mTypingAction != nil) {
						try {
							mTypingAction->ForwardErase();
						} catch (...) {
							PostAction(nil);
						}
					}

					if ((**mTextEditH).selStart == (**mTextEditH).selEnd) {
						::TESetSelect((**mTextEditH).selStart,
							(**mTextEditH).selStart + 1, mTextEditH);
					}

					::TEDelete(mTextEditH);
					ForceAutoScroll(oldDestRect);
					textChanged = true;
				}
			} else {
				passUp = true;
			}
			break;
		}

		case keyStatus_Reject: {
			::SysBeep(1);
			break;
		}

		case keyStatus_PassUp: {
			passUp = true;
			break;
		}
		
		default:
			break;
	}

	if (textChanged) {
		UserChangedText();
	}

	if (passUp) {
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);
	}
	
	if (theTarget == GetTarget()) {		// Target is the same before and
										//   after the keystroke
										
										// Number of lines changed
		if (lineCount != (*mTextEditH)->nLines) {
			AdjustImageToText();
		}
		
			// Selection has changed from empty to not empty or from
			// not empty to empty. Since commands such as Cut and Copy
			// depend on whether there is selection, we must update
			// the command status.
			
		if (beforeSel != ((**mTextEditH).selStart == (**mTextEditH).selEnd)) {
			SetUpdateCommandStatus(true);
		}
	}
	
	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy							[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LView::ResizeFrameBy(inWidthDelta, inHeightDelta, Refresh_No);

	if (HasAttribute(textAttr_WordWrap)) {
		ResizeImageTo(mFrameSize.width, mImageSize.height, Refresh_No);
	}

	AlignTextEditRects();
	// AlignTextEditRects calls TECalText if the local frame rect is
	// within QD space. So it's silly for us to call this again and
	// just waste time. Let's make a check tho for sanity....

	Rect textFrame;
	if ( !CalcLocalFrameRect(textFrame ) ) {
		SignalStringLiteral_("Local frame not in QD space");
	} else {
		//::TECalText(mTextEditH);
	}

	LView::OutOfFocus(this);
	AdjustImageToText();

	if (inRefresh) {				// It's safe to refresh now that
		Refresh();					//   everything is in synch
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveBy								[public, virtual]
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//		inHorizDelta and inVertDelta specify, in pixels, how far to move the
//		Frame (within its surrounding Image). Positive horiz deltas move to
//		the right, negative to the left. Positive vert deltas move down,
//		negative up.

void
LTextEditView::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LView::MoveBy(inHorizDelta, inVertDelta, inRefresh);
	AlignTextEditRects();
}


// ---------------------------------------------------------------------------
//	¥ ScrollImageBy							[public, virtual]
// ---------------------------------------------------------------------------
//	Scroll the Text

void
LTextEditView::ScrollImageBy(
	SInt32		inLeftDelta,		// Pixels to scroll horizontally
	SInt32		inTopDelta,			// Pixels to scroll vertically
	Boolean		inRefresh)
{
	::MacOffsetRect(&(*mTextEditH)->viewRect,
					static_cast<SInt16>(inLeftDelta),
					static_cast<SInt16>(inTopDelta) );

	LView::ScrollImageBy(inLeftDelta, inTopDelta, inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand							[public, virtual]
// ---------------------------------------------------------------------------

Boolean
LTextEditView::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	if ( mTextEditH == nil )
		return cmdHandled;

	Rect oldDestRect  = (*mTextEditH)->destRect;

	switch (inCommand) {

		case cmd_Cut: {
			PostAction( new LTEViewCutAction(mTextEditH, this, this));
			AdjustImageToText();
			ForceAutoScroll(oldDestRect);
			break;
		}

		case cmd_Copy: {
			UScrap::ClearData();
			::TECopy(mTextEditH);
			if (not HasAttribute(textAttr_MultiStyle)) {
				::TEToScrap();
			}
			break;
		}

		case cmd_Paste: {
			// Check to ensure that no more than 32k of text will
			// be present after the paste...

										// Check size of Scrap
			SInt32	scrapSize = UScrap::GetData(ResType_Text, nil);

			if ( scrapSize + 				// If the size of the scrap plus
				(*mTextEditH)->teLength -	// the size of the current text
											// minus any possible selection
				( (*mTextEditH)->selEnd - (*mTextEditH)->selStart )
				<= kMaxChars ) { 		// Is less than 32k, then we can go...

				PostAction( new LTEViewPasteAction(mTextEditH, this, this) );
				AdjustImageToText();
				ForceAutoScroll(oldDestRect);

			} else {					// Else we can't do it....so complain

				::SysBeep(2); 			// Complain
				SignalStringLiteral_("Paste would exceed 32k of data");
				Throw_(err_32kLimit);
			}
			break;
		}

		case cmd_Clear: {
			PostAction( new LTEViewClearAction(mTextEditH, this, this ));
			AdjustImageToText();
			ForceAutoScroll(oldDestRect);
			break;
		}

		case cmd_ActionCut:
		case cmd_ActionPaste:
		case cmd_ActionClear:
		case cmd_ActionTyping: {
			AdjustImageToText();
			ForceAutoScroll(oldDestRect);
			UserChangedText();
			break;
		}

		case cmd_ActionDeleted: {
			if (mTypingAction == static_cast<LTEViewTypingAction*>(ioParam)) {
				mTypingAction = nil;
			}
			break;
		}

		case msg_TabSelect: {
			if (!IsEnabled()) {
				cmdHandled = false;
				break;
			} // else FALL THRU to SelectAll()
		}

		case cmd_SelectAll: {
			SelectAll();
			AdjustImageToText();
			ForceAutoScroll(oldDestRect);
			break;
		}

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus							[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	outUsesMark = false;

	switch (inCommand) {

		case cmd_Copy: {			// Copy enabled if something is selected
			outEnabled = HasSelection();
			break;
		}

		case cmd_Cut:				// Cut and Clear enabled if editabled
		case cmd_Clear:	{			//   and something is selected
			outEnabled = HasAttribute(textAttr_Editable) && HasSelection();
			break;
		}

		case cmd_Paste: {			// Paste enabled if editable and
									//   TEXT is on the Scrap
			outEnabled = HasAttribute(textAttr_Editable) &&
							UScrap::HasData(ResType_Text);
			break;
		}

		case cmd_SelectAll:	{		// Check if any characters are present
			outEnabled = HasAttribute(textAttr_Selectable) &&
							((*mTextEditH)->teLength > 0);
			break;
		}

		default: {
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
		}
	}
}

// ---------------------------------------------------------------------------
//	¥ SetValue												[public, virtual]
// ---------------------------------------------------------------------------
//	Set the text in the TextEditView to the string representation of the
//	specified integer number.

void
LTextEditView::SetValue(
	SInt32	inValue )
{
	Str255	numAsString;
	::NumToString(inValue, numAsString);
	SetDescriptor(numAsString);
}


// ---------------------------------------------------------------------------
//	¥ GetValue												[public, virtual]
// ---------------------------------------------------------------------------
//	Return the integer value represented by the contents of a TextEditView.
//
//	An empty or non-numerical TextEditView evaluates to zero.

SInt32
LTextEditView::GetValue() const
{
	Str255	str;
	GetDescriptor(str);

	SInt32	theNumber = 0;
	if (StrLength(str) > 0) {
		::StringToNum(str, &theNumber);
	}

	return theNumber;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor											[public, virtual]
// ---------------------------------------------------------------------------
//	Will set the text to the given string, replacing any and all text
//	already there

void
LTextEditView::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	SetTextPtr((Ptr)&inDescriptor[1], StrLength(inDescriptor), nil);
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor											[public, virtual]
// ---------------------------------------------------------------------------
//	Return up to the first 255 characters of the TextEditView as a Pascal
//	string. Caller must allocate a Str255 variable for storing the string.

StringPtr
LTextEditView::GetDescriptor(
	Str255	outDescriptor ) const
{
	CharsHandle	theRawText = ::TEGetText(mTextEditH);

	Size textLength = ::GetHandleSize(theRawText);
	if (textLength > 255) {
		textLength = 255;
	}

	::BlockMoveData(*theRawText, outDescriptor+1, textLength);
	outDescriptor[0] = static_cast<UInt8>(textLength);

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetTextHandle							[public, virtual]
// ---------------------------------------------------------------------------
//	Set the text in the LTextEditView to the contents of the specified Handle
//
//	The LTextEditView copies the data in the Handle, so the caller retains
//	ownership of the Handle (and should dispose of it as needed)

void
LTextEditView::SetTextHandle(
	Handle			inTextH,
	StScrpHandle	inStyleH )
{
	StHandleLocker	lock(inTextH);
	SetTextPtr(*inTextH, ::GetHandleSize(inTextH), inStyleH );
}


// ---------------------------------------------------------------------------
//	¥ SetTextPtr							[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::SetTextPtr(
	const void*		inTextP,
	SInt32			inTextLen,
	StScrpHandle	inStyleH )
{
	if (inTextLen <= kMaxChars) {

		StFocusAndClipIfHidden	focus(this);

		if (HasAttribute(textAttr_MultiStyle)) {

			StHandleBlock	styleH((Handle) inStyleH, false);

			if (inStyleH == nil) {		// No input style
										// Use style of beginning of text
				SInt16	savedStart = (*mTextEditH)->selStart;
				SInt16	savedEnd   = (*mTextEditH)->selEnd;

				(*mTextEditH)->selStart = 0;
				(*mTextEditH)->selEnd   = 0;

				styleH.Adopt((Handle) ::TEGetStyleScrapHandle(mTextEditH));

				(*mTextEditH)->selStart = savedStart;
				(*mTextEditH)->selEnd   = savedEnd;
			}

			::TESetText(inTextP, inTextLen, mTextEditH);
			::TEUseStyleScrap(0, inTextLen, (StScrpHandle) styleH.Get(), false, mTextEditH);
			::TECalText(mTextEditH);

		} else {
			::TESetText(inTextP, inTextLen, mTextEditH);
		}

		AdjustImageToText();
		Refresh();

	} else {
		::SysBeep(3);  					// complain
		SignalStringLiteral_("Cannot insert more than 32k of text");
		Throw_(err_32kLimit);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetTextHandle							[public]
// ---------------------------------------------------------------------------
//	Return a Handle to the text in the LTextEditView
//
//	The Handle is the actual Handle used by the Toolbox TextEdit record.
//	Treat this Handle as read-only

Handle
LTextEditView::GetTextHandle()
{
	return static_cast<Handle>(::TEGetText(mTextEditH));
}


// ---------------------------------------------------------------------------
//	¥ Insert								[public, virtual]
// ---------------------------------------------------------------------------
//	TEInsert does not check against the 32k text limit, so we have to.
//
//	Call this routine instead when you'd call TEInsert or TEStyleInsert.
//	Will optionally recalculate, autoscroll, and refresh the text if desired.

void
LTextEditView::Insert(
	const void*		inText,
	SInt32			inLength,
	StScrpHandle	inStyleH,
	Boolean			inRefresh )
{
	Rect oldDestRect;

	if (inLength + (*mTextEditH)->teLength -
		( (*mTextEditH)->selEnd - (*mTextEditH)->selStart ) <= kMaxChars) {

		StFocusAndClipIfHidden	focus(this);
		oldDestRect = (*mTextEditH)->destRect;

		if ( HasAttribute(textAttr_MultiStyle) && (inStyleH != nil) ) {
			::TEStyleInsert(inText, inLength, inStyleH, mTextEditH);
		} else {
			::TEInsert(inText, inLength, mTextEditH);
		}

	} else {
		::SysBeep(3);
		SignalStringLiteral_("Insert would exceed 32k of text");
		Throw_(err_32kLimit);
	}

	// Force a redraw. The TextEdit internals are updated, so we need to
	// reflect this fact.

	if ( inRefresh ) {
		AdjustImageToText();
		ForceAutoScroll( oldDestRect );
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID						[public, virtual]
// ---------------------------------------------------------------------------
//	Specify the resource ID of the TextTraits for an TextEdit
//
//	This function updates the line height to fit the text characteristics.
//
//	We need to ensure that the new text traits will not blow the 32k limit
//	on the view rect.

void
LTextEditView::SetTextTraitsID(
	ResIDT	inTextTraitsID)
{
	// text traits are not used in multistyle textedit

	if ( HasAttribute(textAttr_MultiStyle) )
		return;
										// Get the original dest rect
	Rect oldDestRect  = (*mTextEditH)->destRect;

										// Save the original text traits ID,
	ResIDT	oldTxtrID = mTextTraitsID;	//   in case things fail

	mTextTraitsID = inTextTraitsID;		// Store the new text traits ID

										// Now try to apply the desired trait
	UTextTraits::SetTETextTraits(mTextTraitsID, mTextEditH);

										// Check to ensure we haven't blown
										//   the 32k on the view rect
	if (TETooBig()) {
										// If we'd blow the height, revert
										//   back to the old text traits
		UTextTraits::SetTETextTraits(oldTxtrID, mTextEditH);
		mTextTraitsID = oldTxtrID;

		SignalStringLiteral_("Requested text traits are too large");
		Throw_(err_32kLimit);
	}

	SPoint32	scrollUnit;
	scrollUnit.h = 4;
	scrollUnit.v = (*mTextEditH)->lineHeight;
	SetScrollUnit(scrollUnit);

	AlignTextEditRects();
	AdjustImageToText();
	ForceAutoScroll(oldDestRect);
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ CalcTEHeight							[public, virtual]
// ---------------------------------------------------------------------------
//	Calculates the height of the TextEdit record.  Does a little bit to
//	ensure for a special case

SInt32
LTextEditView::CalcTEHeight()
{
	SInt32	result;
	SInt16	length;

	result = ::TEGetHeight(max_Int16, 0, mTextEditH);
	length = (*mTextEditH)->teLength;

		// Text Edit doesn't return the height of the last character, if
		// that character is a <cr>. So if we see that, we go grab the
		// height of that last character and add it into the total height.

	if ((length) && ((*(*mTextEditH)->hText)[length-1] == char_Return)) {
		TextStyle	theStyle;
		SInt16		theHeight;
		SInt16		theAscent;

		::TEGetStyle(length, &theStyle, &theHeight, &theAscent, mTextEditH);
		result += theHeight;
	}

	return result;

}


// ---------------------------------------------------------------------------
//	¥ TETooBig								[public, virtual]
// ---------------------------------------------------------------------------
//	Just a little utility routine to make the 32k check a little easier to do

Boolean
LTextEditView::TETooBig()
{
	return (CalcTEHeight() > max_Int16);
}

// ---------------------------------------------------------------------------
//	¥ HasAttribute							[public, virtual]
// ---------------------------------------------------------------------------
// Utility routine to determine if a given LTextEditView has the requested
// attribute.
//
//	Declared inline in the header file


// ---------------------------------------------------------------------------
//	¥ FocusDraw								[public, virtual]
// ---------------------------------------------------------------------------

Boolean
LTextEditView::FocusDraw(
	LPane	*inSubPane)
{
	Boolean	focused = LView::FocusDraw(inSubPane);
	
	if (focused) {
		StColorPenState::Normalize();
		Pattern		whitePat;
		::BackPat(UQDGlobals::GetWhitePat(&whitePat));
		
		if (not HasAttribute(textAttr_MultiStyle)) {
			UTextTraits::SetPortTextTraits(mTextTraitsID);
		}
	}

	return focused;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf								[protected, virtual]
// ---------------------------------------------------------------------------
//	Draw a TextEditView

void
LTextEditView::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	::EraseRect(&frame);

		// A Mac TERec stores a pointer to its owner port  We have to
		// change it to the current port in case we are drawing into
		// a port that is not the owner port. This happens when we are
		// printing or drawing into an offscreen port.

	GrafPtr	savePort = (*mTextEditH)->inPort;
	(*mTextEditH)->inPort = UQDGlobals::GetCurrentPort();

	::TEUpdate(&frame, mTextEditH);

	(*mTextEditH)->inPort = savePort;
}


// ---------------------------------------------------------------------------
//	¥ PrintPanelSelf								[protected, virtual]
// ---------------------------------------------------------------------------
//	Print a TextEditView

void
LTextEditView::PrintPanelSelf(
	const PanelSpec&	/* inPanel */)
{
	Rect	frame;
	CalcLocalFrameRect(frame);

		// A Mac TERec stores a pointer to its owner port  We have to
		// change it to the current port, which will be a Printer port.
		
	GrafPtr	savePort = (**mTextEditH).inPort;
	(**mTextEditH).inPort = UQDGlobals::GetCurrentPort();
	
		// Remove the selection when printing. We change the fields
		// of the TERec directly because TESetSelect() draws when
		// it changes the selection.
	
	SInt16	selStart = (**mTextEditH).selStart;
	SInt16	selEnd   = (**mTextEditH).selEnd;
	
	(**mTextEditH).selStart = 0;
	(**mTextEditH).selEnd   = 0;

	::TEUpdate(&frame, mTextEditH);

	(**mTextEditH).selStart = selStart;
	(**mTextEditH).selEnd   = selEnd;

	(**mTextEditH).inPort = savePort;
}


// ---------------------------------------------------------------------------
//	¥ HideSelf								[protected, virtual]
// ---------------------------------------------------------------------------
//	Hide an LTextEditView. An invisible LTextEditView can't be OnDuty.

void
LTextEditView::HideSelf()
{
	if (IsOnDuty()) {				// Shouldn't be on duty when invisible
		SwitchTarget(GetSuperCommander());
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouseSelf						[protected, virtual]
// ---------------------------------------------------------------------------
//	TextEdit uses the standard I-Beam cursor

void
LTextEditView::AdjustMouseSelf(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			/* ioMouseRgn */)
{
	if (HasAttribute(textAttr_Selectable)) {
		UCursor::SetIBeam();
	} else {
		UCursor::SetArrow();
	}
}


// ---------------------------------------------------------------------------
//	¥ SelectAll								[public, virtual]
// ---------------------------------------------------------------------------
//	Select entire contents of an TextEdit

void
LTextEditView::SelectAll()
{
	if (HasAttribute(textAttr_Selectable)) {
		StFocusAndClipIfHidden	focus(this);
		::TESetSelect(0, max_Int16, mTextEditH);
	}
}


// ---------------------------------------------------------------------------
//	¥ AlignTextEditRects					[public, virtual]
// ---------------------------------------------------------------------------
//	Align the view and destination rectangles of the Toolbox TextEdit
//	record with the Frame of a TextEdit

void
LTextEditView::AlignTextEditRects()
{
	Rect	textFrame;
	if ( (mSuperView != nil) &&
		 FocusDraw() &&
		 CalcLocalFrameRect(textFrame)) {
									// TextEdit view rect same as Frame
									//   in local coords
		(*mTextEditH)->viewRect = textFrame;

									// TextEdit dest rect same as Image
									//   in local coords
		SPoint32	imagePt;
		imagePt.h = imagePt.v = 0;
		ImageToLocalPoint(imagePt, topLeft((*mTextEditH)->destRect));

									// Bottom of dest rect is ignored
		imagePt.h = mImageSize.width;
		ImageToLocalPoint(imagePt, botRight((*mTextEditH)->destRect));

		::TECalText(mTextEditH);	// Let TextEdit adjust line breaks
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustImageToText						[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::AdjustImageToText()
{
	ResizeImageTo(mImageSize.width, CalcTEHeight(), Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ BeTarget								[protected, virtual]
// ---------------------------------------------------------------------------
//	TextEdit is becoming the Target

void
LTextEditView::BeTarget()
{
	StFocusAndClipIfHidden	focus(this);

	::TEActivate(mTextEditH);		// Show active selection

									// Idle time used to flash the cursor
	StartIdling( ::TicksToEventTime( ::GetCaretTime() ) );

	sTextEditViewP = nil;
	if ( HasAttribute(textAttr_AutoScroll) ) {
		sTextEditViewP = this;		// Ensure we autoscroll correctly
	}
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget							[protected, virtual]
// ---------------------------------------------------------------------------
//	TextEdit is no longer the Target
//
//	Remove TextEdit from IdleQueue

void
LTextEditView::DontBeTarget()
{
	StFocusAndClipIfHidden	focus(this);

	::TEDeactivate(mTextEditH);		// Show inactive selection

	StopIdling();					// Stop flashing the cursor

	sTextEditViewP = nil;			// To keep autoscrolling clean
}


// ---------------------------------------------------------------------------
//	¥ SpendTime								[public, virtual]
// ---------------------------------------------------------------------------
//	Idle time: Flash the insertion cursor

void
LTextEditView::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	StColorPortState	savePortState(GetMacPort());

	if (HasAttribute(textAttr_Selectable) && not HasSelection() && FocusExposed()) {
		::TEIdle(mTextEditH);
		OutOfFocus(nil);
	}
}

// ---------------------------------------------------------------------------
//	¥ UserChangedText						[public, virtual]
// ---------------------------------------------------------------------------
//	Text of TextEdit has changed as a result of user action
//
//	Override to validate field and/or dynamically update as the user
//	types. This function is not called by SetDescriptor, which is typically
//	used to programatically change the text.

void
LTextEditView::UserChangedText()
{
	// nothing
}


// ---------------------------------------------------------------------------
//	¥ GetSelection
// ---------------------------------------------------------------------------
//	Passes back an AEDesc of the currently selected text

void
LTextEditView::GetSelection(
	AEDesc	&outDesc) const
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
//	¥ SetSelectionRange
// ---------------------------------------------------------------------------

void
LTextEditView::SetSelectionRange(
	SInt16	inStartPos,
	SInt16	inEndPos)
{
	::TESetSelect(inStartPos, inEndPos, mTextEditH);
}


// ---------------------------------------------------------------------------
//	¥ HasSelection													  [public]
// ---------------------------------------------------------------------------

bool
LTextEditView::HasSelection() const
{										// Selection start and end are
										//   different if text is selected
	return ((**mTextEditH).selStart != (**mTextEditH).selEnd);
}


// ---------------------------------------------------------------------------
//	¥ SaveStateForUndo						[protected, virtual]
// ---------------------------------------------------------------------------

STextEditUndoH
LTextEditView::SaveStateForUndo()
{
	STextEditUndoH	theUndoH = reinterpret_cast<STextEditUndoH>
									(::NewHandle(sizeof(STextEditUndo)));
	ThrowIfMemFail_(theUndoH);

	Handle	currTextH		= (*mTextEditH)->hText;
	ThrowIfOSErr_(::HandToHand(&currTextH));
	(*theUndoH)->textH		= currTextH;
	(*theUndoH)->selStart	= (*mTextEditH)->selStart;
	(*theUndoH)->selEnd		= (*mTextEditH)->selEnd;

	return theUndoH;
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace							[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::RestorePlace(
	LStream*	inPlace)
{
	LView::RestorePlace(inPlace);

	AlignTextEditRects();
}



// ---------------------------------------------------------------------------
//	¥ SetFont								[public, virtual]
// ---------------------------------------------------------------------------
//	Given a font number, set the font

void
LTextEditView::SetFont(
	SInt16 inFontNumber )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewFontAction( inFontNumber, this, this ) );

		UserChangedText();
	}
}

// ---------------------------------------------------------------------------
//	¥ SetFont								[public, virtual]
// ---------------------------------------------------------------------------
//	Given a font name, set the font

void
LTextEditView::SetFont(
	ConstStringPtr inFontName )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewFontAction( const_cast<StringPtr>(inFontName), this, this ) );

		UserChangedText();
	}
}

// ---------------------------------------------------------------------------
//	¥ GetFont								[public, virtual]
// ---------------------------------------------------------------------------
//	Determine the font over the selection range, passing back the font number.

Boolean
LTextEditView::GetFont(
	SInt16	&outFontNum )
{
	TextStyle theStyle;
	SInt16 theMode = doFont;
	Boolean isContinuous = ::TEContinuousStyle( &theMode, &theStyle, mTextEditH );

	outFontNum = theStyle.tsFont;

	return isContinuous;
}


// ---------------------------------------------------------------------------
//	¥ GetFont								[public, virtual]
// ---------------------------------------------------------------------------
//	Determine the font over the selection range, passing back the font name

Boolean
LTextEditView::GetFont(
	Str255 outName )
{
	TextStyle theStyle;
	SInt16 theMode = doFont;
	Boolean isContinuous = ::TEContinuousStyle( &theMode, &theStyle, mTextEditH );

	::GetFontName( theStyle.tsFont, outName );

	return isContinuous;
}


// ---------------------------------------------------------------------------
//	¥ SetSize								[public, virtual]
// ---------------------------------------------------------------------------

void
LTextEditView::SetSize(
	SInt16 inSize )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewSizeAction( inSize, this, this ) );

		UserChangedText();
	}
}

// ---------------------------------------------------------------------------
//	¥ GetSize								[public, virtual]
// ---------------------------------------------------------------------------

Boolean
LTextEditView::GetSize(
	SInt16 &outSize )
{
	TextStyle theStyle;
	SInt16 theMode = doSize;
	Boolean isContinuous = ::TEContinuousStyle( &theMode, &theStyle, mTextEditH );

	outSize = theStyle.tsSize;

	return isContinuous;
}


// ---------------------------------------------------------------------------
//	¥ ChangeFontSizeBy						[public, virtual]
// ---------------------------------------------------------------------------
//	Will only work if the font over the selection range is constant.  Returns
//	true if change was successful.

Boolean
LTextEditView::ChangeFontSizeBy(
	SInt16	inDeltaSize)
{
	SInt16	currentSize;
	Boolean canChange = GetSize(currentSize);

	if ( canChange ) {
		SetSize((SInt16) (currentSize + inDeltaSize));
	}

	return canChange;
}


// ---------------------------------------------------------------------------------
//	¥ SetStyle								[public, virtual]
// ---------------------------------------------------------------------------------

void
LTextEditView::SetStyle(
	Style	inStyle )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewFaceAction( inStyle, false, this, this ) );

		UserChangedText();
	}
}


// ---------------------------------------------------------------------------------
//	¥ GetStyle								[public, virtual]
// ---------------------------------------------------------------------------------

Boolean
LTextEditView::GetStyle(
	Style &outStyle )
{
	TextStyle theStyle;
	SInt16 theMode = doFace;
	Boolean isContinuous = ::TEContinuousStyle( &theMode, &theStyle, mTextEditH );

	outStyle = theStyle.tsFace;

	return isContinuous;
}


// ---------------------------------------------------------------------------------
//	¥ ToggleStyleAttribute					[public, virtual]
// ---------------------------------------------------------------------------------

void
LTextEditView::ToggleStyleAttribute(
	Style inStyleAttr )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewFaceAction( inStyleAttr, true, this, this ) );

		UserChangedText();
	}
}


// ---------------------------------------------------------------------------------
//	¥ SetAlignment							[public, virtual]
// ---------------------------------------------------------------------------------

void
LTextEditView::SetAlignment(
	SInt16 inAlign )
{
	PostAction( new LTEViewAlignAction( inAlign, this, this ) );

	UserChangedText();
}


// ---------------------------------------------------------------------------------
//	¥ GetAlignment							[public, virtual]
// ---------------------------------------------------------------------------------

SInt16
LTextEditView::GetAlignment()
{
	return (*mTextEditH)->just;
}


// ---------------------------------------------------------------------------------
//	¥ SetColor								[public, virtual]
// ---------------------------------------------------------------------------------

void
LTextEditView::SetColor(
	const RGBColor &inColor )
{
	if (HasAttribute(textAttr_MultiStyle)) {

		PostAction( new LTEViewColorAction( inColor, this, this ) );

		UserChangedText();
	}
}

// ---------------------------------------------------------------------------------
//	¥ GetColor								[public, virtual]
// ---------------------------------------------------------------------------------

Boolean
LTextEditView::GetColor(
	RGBColor &outColor )
{
	TextStyle theStyle;
	SInt16 theMode = doColor;
	Boolean isContinuous = ::TEContinuousStyle( &theMode, &theStyle, mTextEditH );

	outColor = theStyle.tsColor;

	return isContinuous;
}

PP_End_Namespace_PowerPlant
