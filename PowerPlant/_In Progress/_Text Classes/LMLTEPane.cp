// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMLTEPane.cp				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================


#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMLTEPane.h>
#include <LStream.h>
#include <LView.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

TXNFontMenuObject	LMLTEPane::sTXNFontMenuObject = nil;
ResIDT				LMLTEPane::sFontMenuID = 0;
ResIDT				LMLTEPane::sStartHierMenuID = 0;
ResIDT				LMLTEPane::sEndHierMenuID = 0;
bool				LMLTEPane::sMLTEIsInitialized = false;

// ---------------------------------------------------------------------------
//	¥ Initialize											 [static] [public]
// ---------------------------------------------------------------------------
//	Toolbox initialization for MLTE
//
//	You MUST call this function at the beginning of your program when using
//	any MLTE routines. The documentation for MLTE state that you should
//	initialize it as soon as possbile after initailzing the Mac Toolbox.
//
//	You should also call LMLTEPane::Terminate() at the end of your program

void
LMLTEPane::Initialize(
	ResIDT								inFontMenuID,
	ResIDT								inStartHierMenuID,
	TXNMacOSPreferredFontDescription	inFonts[],
	ItemCount							inFontCount,
	TXNInitOptions						inOptions)
{
	OSStatus	status;					// Toolbox initialization for MLTE
	
	status = ::TXNInitTextension(inFonts, inFontCount, inOptions);
	
	ThrowIfOSStatus_(status);
	
	sMLTEIsInitialized = true;			// Set flag so we know MLTE has been
										//   initialized
										
										// Create MLTE Font menu
	if (inFontMenuID > 0) {
		status = ::TXNNewFontMenuObject( ::GetMenuHandle(inFontMenuID),
										 inFontMenuID,
										 inStartHierMenuID,
										 &sTXNFontMenuObject );
							
		ThrowIfOSStatus_(status);
		
		sFontMenuID = inFontMenuID;
		sStartHierMenuID = inStartHierMenuID;
		
			// $$$ Clumsy Feature
			//
			// There doesn't seem to be a good way to handle
			// what menu IDs to use for hierarchical items in the
			// Font menu. Read the comments for ::TXNNewFontMenuObject
			// in <MacTextEditor.h>. The allowable IDs to use depend on
			// the OS version and there is no way to query MLTE about
			// how many menu IDs it needs.
			//
			// For now, we hardwire a maximum number of 75 and check
			// that the menu ID falls within this range when hadling
			// menu selections in our ObeyCommand() function.
		
		sEndHierMenuID = (ResIDT) (sStartHierMenuID + 75);
	}
}


// ---------------------------------------------------------------------------
//	¥ Terminate												 [static] [public]
// ---------------------------------------------------------------------------
//	Toolbox clean up for MLTE
//
//	Apple's docs say you need to call the MLTE termination function so that
//	Textension can close down any TSM connections and do other clean up.

void
LMLTEPane::Terminate()
{
	::TXNTerminateTextension();
	
	if (sTXNFontMenuObject != nil) {
		::TXNDisposeFontMenuObject(sTXNFontMenuObject);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LMLTEPane						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMLTEPane::LMLTEPane()
{
	
}


// ---------------------------------------------------------------------------
//	¥ LMLTEPane						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LMLTEPane::LMLTEPane(
	LStream*	inStream)
	
	: LPane(inStream)
{
	TXNFrameOptions					frameOptions;
	TXNFrameType					frameType;
	TXNFileType						fileType;
	TXNPermanentTextEncodingType	encodingType;

	*inStream >> frameOptions;
	*inStream >> frameType;
	*inStream >> fileType;
	*inStream >> encodingType;
	
	InitMLTEPane(nil, frameOptions, frameType, fileType, encodingType);
}


// ---------------------------------------------------------------------------
//	¥ LMLTEPane						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LMLTEPane::LMLTEPane(
	const SPaneInfo&	inPaneInfo,
	LCommander*			inSuperCommander,
	const FSSpec*		inFileSpec,
	TXNFrameOptions		inFrameOptions,
	TXNFrameType		inFrameType,
	TXNFileType			inFileType,
	TXNPermanentTextEncodingType	inEncodingType)
	
	: LPane(inPaneInfo),
	  LCommander(inSuperCommander)
{
	InitMLTEPane(inFileSpec, inFrameOptions, inFrameType,
					inFileType, inEncodingType);
}


// ---------------------------------------------------------------------------
//	¥ LMLTEPane						Destructor				  [public]
// ---------------------------------------------------------------------------

LMLTEPane::~LMLTEPane()
{
	if (mTXNObject != nil) {
		::TXNDeleteObject(mTXNObject);
	}
}


// ---------------------------------------------------------------------------
//	¥ InitMLTEPane
// ---------------------------------------------------------------------------

void
LMLTEPane::InitMLTEPane(
	const FSSpec*		inFileSpec,
	TXNFrameOptions		inFrameOptions,
	TXNFrameType		inFrameType,
	TXNFileType			inFileType,
	TXNPermanentTextEncodingType	inEncodingType)
{
									// You MUST call LMLTEPane::Initialize()
									//   at the beginning of your program
	SignalIf_( not sMLTEIsInitialized );

	StFocusAndClipIfHidden	focus(this);

	Rect	frame;
	CalcLocalFrameRect(frame);

	OSStatus status = ::TXNNewObject( inFileSpec,
									  GetMacWindow(),
									  &frame,
									  inFrameOptions,
									  inFrameType,
									  inFileType,
									  inEncodingType,
									  &mTXNObject,
									  &mTXNFrameID,
									  0 );
	
	ThrowIfOSStatus_(status);
	
									// Use CoreGraphics if present
	CGContextRef	context = GetCGContext();

	if (context != nil) {
		SetCGContext(context);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LMLTEPane::GetDescriptor(
	Str255	outDescriptor) const
{
	outDescriptor[0] = 0;
	
	Handle	dataH = nil;

	OSStatus	status = ::TXNGetDataEncoded( mTXNObject,
											  kTXNStartOffset,
											  kTXNEndOffset,
											  &dataH,
											  kTXNTextData );
											  
		// $$$ Bug
		// This doesn't seem to work right. Passing kTXNTextData
		// still retrieves the contents as Unicode text.
		
	SignalIfOSStatus_(status);
											  
	if ((status == noErr)  &&  (dataH != nil)) {
											  
		Size	textLength = ::GetHandleSize(dataH);
		if (textLength > 255) {
			textLength = 255;
		}

		::BlockMoveData(*dataH, outDescriptor+1, textLength);
		outDescriptor[0] = (UInt8) textLength;
		
	}
	
	if (dataH != nil) {
		::DisposeHandle(dataH);
	}

	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------

void
LMLTEPane::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	ThrowIfOSStatus_( ::TXNSetData( mTXNObject,
									kTXNTextData,
									(void*) (inDescriptor + 1),
									inDescriptor[0],
									kTXNStartOffset,
									kTXNEndOffset ) );
}


// ---------------------------------------------------------------------------
//	¥ SetTextPtr
// ---------------------------------------------------------------------------

void
LMLTEPane::SetTextPtr(
	const void*		inTextP,
	ByteCount		inTextLength)
{
	ThrowIfOSStatus_( ::TXNSetData( mTXNObject,
									kTXNTextData,
									(void*) inTextP,
									inTextLength,
									kTXNStartOffset,
									kTXNEndOffset ) );
}


#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------
//	Return contents as a CFString
//
//	CFStringRef will be nil if we fail. This will happen if the MLTE object
//	contains data other than text (pictures, movies, etc.) or no text at all.

CFStringRef
LMLTEPane::CopyCFDescriptor() const
{
	CFStringRef		stringRef = nil;
	
	Handle		dataH  = nil;
	OSStatus	status = ::TXNGetDataEncoded( mTXNObject,
											  kTXNStartOffset,
											  kTXNEndOffset,
											  &dataH,
											  kTXNUnicodeTextData );
											  
	SignalIfOSStatus_(status);
											  
	if ((status == noErr)  &&  (dataH != nil)) {
		Size	textLength = (Size) (::GetHandleSize(dataH) / sizeof(UniChar));
		
		::HLock(dataH);
		
		stringRef = ::CFStringCreateWithCharacters( nil,
													(UniChar*) *dataH,
													textLength );
	}
	
	if (dataH != nil) {
		::DisposeHandle(dataH);
	}

	return stringRef;
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LMLTEPane::SetCFDescriptor(
	CFStringRef		inStringRef)
{
	SInt32			strLength = ::CFStringGetLength(inStringRef);
	
		// Try to get a pointer to the characters in the CFString
		// itself. If we can, we don't have to copy the characters.
	
	const UniChar*	textPtr	  = ::CFStringGetCharactersPtr(inStringRef);

	UniChar*		buffer = nil;
	
	if (textPtr == nil) {
	
			// Couldn't get a pointer to the CFString characters. We
			// need to allocate a buffer and copy from the CFString
	
		buffer = new UniChar[strLength];
		::CFStringGetCharacters(inStringRef, CFRangeMake(0, strLength), buffer);
		
		textPtr = buffer;
	}
	
	OSStatus	status = ::TXNSetData( mTXNObject,
									   kTXNUnicodeTextData,
									   (void*) textPtr,
									   strLength * sizeof(UniChar),
									   kTXNStartOffset,
									   kTXNEndOffset );
									   
	delete[] buffer;
	
	ThrowIfOSStatus_(status);
}

#endif


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Handle standard editing commands

Boolean
LMLTEPane::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	ResIDT	theMenuID;
	SInt16	theMenuItem;
	if (IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {

		if ( (theMenuID == sFontMenuID)  ||
			 ( (theMenuID >= sStartHierMenuID) &&
			   (theMenuID <= sEndHierMenuID) ) ) {
		
			::TXNDoFontMenuSelection(mTXNObject, sTXNFontMenuObject,
										theMenuID, theMenuItem);
		
		} else {
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
		}
		
	} else {

		switch (inCommand) {
		
			case cmd_Undo:
				::TXNUndo(mTXNObject);
				break;
				
			case cmd_Redo:
				::TXNRedo(mTXNObject);
				break;
		
			case cmd_Cut:
				::TXNCut(mTXNObject);
				break;
				
			case cmd_Copy:
				::TXNCopy(mTXNObject);
				break;
				
			case cmd_Paste:
				::TXNPaste(mTXNObject);
				break;
				
			case cmd_Clear:
				::TXNClear(mTXNObject);
				break;
				
			case cmd_SelectAll:
				::TXNSelectAll(mTXNObject);
				break;

			default:
				cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
				break;
		}
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
LMLTEPane::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	ResIDT	theMenuID;
	SInt16	theMenuItem;
	if (IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {

		if (theMenuID == sFontMenuID) {
			outEnabled = true;
			
			::TXNPrepareFontMenu(mTXNObject, sTXNFontMenuObject);
			
		} else {
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
		}
		
	} else {

		switch (inCommand) {
		
			case cmd_Undo: {
				TXNActionKey	actionKey;
				outEnabled = ::TXNCanUndo(mTXNObject, &actionKey);
				
				SInt16	strIndex;
				
				if (not outEnabled) {
					strIndex = 1;
				} else if (actionKey == kTXNUndoLastAction) {
					strIndex = 2;
				} else {
					strIndex = (SInt16) (actionKey + 3);
				}
				
				::GetIndString(outName, 160, strIndex);
				break;
			}
			
			case cmd_Redo: {
				TXNActionKey	actionKey;
				outEnabled = ::TXNCanRedo(mTXNObject, &actionKey);
			
				SInt16	strIndex;
				
				if (not outEnabled) {
					strIndex = 1;
				} else if (actionKey == kTXNUndoLastAction) {
					strIndex = 2;
				} else {
					strIndex = (SInt16) (actionKey + 3);
				}
				
				::GetIndString(outName, 161, strIndex);
				break;
			}
		
			case cmd_Cut:
			case cmd_Clear:
				outEnabled = not ::TXNIsSelectionEmpty(mTXNObject);
				break;
			
			case cmd_Copy:
				outEnabled = not ::TXNIsSelectionEmpty(mTXNObject);
				break;
		
			case cmd_Paste:
				outEnabled = TXNIsScrapPastable();
				break;
				
			case cmd_SelectAll:
				outEnabled = true;
				break;

			default:
				LCommander::FindCommandStatus(inCommand, outEnabled,
										outUsesMark, outMark, outName);
				break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress
// ---------------------------------------------------------------------------
//	Handle key stroke directed at an EditField
//
//	Return true if the EditField handles the keystroke

Boolean
LMLTEPane::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
	FocusDraw();
	::TXNKeyDown(mTXNObject, &inKeyEvent);
	
	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BeTarget
// ---------------------------------------------------------------------------
//	EditField is becoming the Target

void
LMLTEPane::BeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	
	::TXNConvertFromPublicScrap();
	
	::TXNFocus(mTXNObject, true);
	::TXNActivate(mTXNObject, mTXNFrameID, true);
	
	#if PP_MLTE_Manual_Idle
		StartIdling();				// Idle time used to flash the cursor
	#endif
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget
// ---------------------------------------------------------------------------
//	EditField is no longer the Target

void
LMLTEPane::DontBeTarget()
{
	StFocusAndClipIfHidden	focus(this);
	
	::TXNFocus(mTXNObject, false);
	::TXNActivate(mTXNObject, mTXNFrameID, false);
	
	::TXNConvertToPublicScrap();

	#if PP_MLTE_Manual_Idle
		StopIdling();				// Stop flashing the cursor
	#endif
}


#if PP_MLTE_Manual_Idle

// ---------------------------------------------------------------------------
//	¥ SpendTime
// ---------------------------------------------------------------------------
//	Idle time: Flash the insertion cursor
//
//	MLTE installs a timer task to flash the cursor on CarbonLib 1.3.1 or
//	later. For earlier CarbonLib versions and Classic, we must call
//	TXNIdle() ourselves.

void
LMLTEPane::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	if (FocusExposed()) {
		::TXNIdle(mTXNObject);
	}
}

#endif


// ---------------------------------------------------------------------------
//	¥ AdjustMouseSelf
// ---------------------------------------------------------------------------
//	Set cursor and mouse region

void
LMLTEPane::AdjustMouseSelf(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			ioMouseRgn)
{
	::TXNAdjustCursor(mTXNObject, ioMouseRgn);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------

void
LMLTEPane::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
									// Draw theme-compliant border
	ThemeDrawState	state = kThemeStateInactive;
	
	if (IsActive() and IsEnabled()) {
		state = kThemeStateActive;
	}

	::DrawThemeListBoxFrame(&frame, state);
	
	::TXNDraw(mTXNObject, nil);
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------

void
LMLTEPane::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	if (not IsTarget()) {			// If not the Target, clicking in an
									//   EditField makes it the Target.
		SwitchTarget(this);
	}

	if (IsTarget()) {
		FocusDraw();
		::TXNClick(mTXNObject, &inMouseDown.macEvent);
	}
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
LMLTEPane::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
									// Resize Pane
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
	
	::TXNResizeFrame(mTXNObject, mFrameSize.width, mFrameSize.height,
						mTXNFrameID);
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
LMLTEPane::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LPane::MoveBy(inHorizDelta, inVertDelta, inRefresh);
	
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	::TXNSetFrameBounds(mTXNObject, frame.top, frame.left, frame.bottom,
						frame.right, mTXNFrameID);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusDraw														  [public]
// ---------------------------------------------------------------------------

Boolean
LMLTEPane::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean	revealed = LPane::FocusDraw();
	
		// To keep the MLTE happy, we must set the
		// origin to (0, 0). This invalidates PP's focus cache
		// and shifts the clipping region.
		
										// Get port bounds and clipping
										//   region before changing origin
	GrafPtr		port = UQDGlobals::GetCurrentPort();
	Rect		portRect;
	::GetPortBounds(port, &portRect);
	
	StRegion	clipRgn;
	::GetClip(clipRgn);
	
	::SetOrigin(0, 0);
	
										// After origin change, PP focus
	LView::OutOfFocus(nil);				//   is no longer valid

										// We want to clip to the same
										//   screen area as before, so we
										//   must offset the clip region
										//   to account for the origin change
	clipRgn.OffsetBy((SInt16) -portRect.left, (SInt16) -portRect.top);
	::SetClip(clipRgn);

	return revealed;
}


// ---------------------------------------------------------------------------
//	¥ PortToLocalPoint
// ---------------------------------------------------------------------------
//	Convert point from Port to Local coordinates

void
LMLTEPane::PortToLocalPoint(
	Point&	/* ioPoint */) const
{
}		// No conversion needed. Local coords are the same as Port coords.


// ---------------------------------------------------------------------------
//	¥ LocalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Local to Port coordinates

void
LMLTEPane::LocalToPortPoint(
	Point&	/* ioPoint */) const
{
}		// No conversion needed. Local coords are the same as Port coords.

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetOneControlTag
// ---------------------------------------------------------------------------

void
LMLTEPane::SetOneControlTag(
	TXNControlTag		inTag,
	TXNControlData		inData)
{
	ThrowIfOSStatus_( ::TXNSetTXNObjectControls( mTXNObject, false, 1,
												 &inTag, &inData ) );
}


// ---------------------------------------------------------------------------
//	¥ SetCGContext
// ---------------------------------------------------------------------------

void
LMLTEPane::SetCGContext(
	CGContextRef	inCGContext)
{
	TXNControlData	data;
	data.uValue = (UInt32) inCGContext;
	
	SetOneControlTag(kATSUCGContextTag, data);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetOneTypeAttribute
// ---------------------------------------------------------------------------

void
LMLTEPane::SetOneTypeAttribute(
	TXNTypeAttributes*	inAttribute,
	TXNOffset			inStartOffset,
	TXNOffset			inEndOffset)
{
	ThrowIfOSStatus_( ::TXNSetTypeAttributes( mTXNObject, 1, inAttribute,
											  inStartOffset, inEndOffset ) );
}


// ---------------------------------------------------------------------------
//	¥ SetOneTypeAttributeForSelection
// ---------------------------------------------------------------------------

void
LMLTEPane::SetOneTypeAttributeForSelection(
	TXNTypeAttributes*	inAttribute)
{
	ThrowIfOSStatus_( ::TXNSetTypeAttributes( mTXNObject, 1, inAttribute,
											  kTXNUseCurrentSelection,
											  kTXNUseCurrentSelection ) );
}


// ---------------------------------------------------------------------------
//	¥ SetFontName
// ---------------------------------------------------------------------------

void
LMLTEPane::SetFontName(
	ConstStringPtr	inName)
{
	TXNTypeAttributes	attribute;
	
	attribute.tag	= kTXNQDFontNameAttribute;
	attribute.size	= kTXNQDFontNameAttributeSize;
	attribute.data.dataPtr = (void*) inName;
									  
	SetOneTypeAttributeForSelection(&attribute);
}


// ---------------------------------------------------------------------------
//	¥ SetFontSize
// ---------------------------------------------------------------------------

void
LMLTEPane::SetFontSize(
	Fixed	inSize)
{
	TXNTypeAttributes	attribute;
	
	attribute.tag	= kTXNQDFontSizeAttribute;
	attribute.size	= kTXNFontSizeAttributeSize;
	attribute.data.dataValue = (UInt32) inSize;
									  
	SetOneTypeAttributeForSelection(&attribute);
}


// ---------------------------------------------------------------------------
//	¥ SetFontStyle
// ---------------------------------------------------------------------------

void
LMLTEPane::SetFontStyle(
	Style	inStyle)
{
	TXNTypeAttributes	attribute;
	
	attribute.tag	= kTXNQDFontStyleAttribute;
	attribute.size	= kTXNQDFontStyleAttributeSize;
	attribute.data.dataValue = inStyle;
									  
	SetOneTypeAttributeForSelection(&attribute);
}


// ---------------------------------------------------------------------------
//	¥ SetFontColor
// ---------------------------------------------------------------------------

void
LMLTEPane::SetFontColor(
	const RGBColor&	inColor)
{
	TXNTypeAttributes	attribute;
	
	attribute.tag	= kTXNQDFontColorAttribute;
	attribute.size	= kTXNQDFontColorAttributeSize;
	attribute.data.dataPtr = (void*) &inColor;
									  
	SetOneTypeAttributeForSelection(&attribute);
}

PP_End_Namespace_PowerPlant
