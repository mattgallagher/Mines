// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMControlImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMControlImp.h>
#include <LView.h>
#include <PP_Resources.h>
#include <UAppearance.h>
#include <UControlMgr.h>
#include <UDrawingUtils.h>
#include <UEnvironment.h>
#include <UQDOperators.h>
#include <UTextTraits.h>
#include <UTBAccessors.h>

#include <LowMem.h>
#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const OSType	PropType_PowerPlant		= FOUR_CHAR_CODE('MWPP');
const OSType	PropTag_ControlPane		= FOUR_CHAR_CODE('Pane');
const OSType	PropTag_ControlImp		= FOUR_CHAR_CODE('CImp');


// ---------------------------------------------------------------------------
//	Function Prototypes for Toolbox callback functions

static pascal OSStatus
CustomControlColorProc( ControlHandle, SInt16, SInt16, Boolean );


// ---------------------------------------------------------------------------
//	¥ LAMControlImp					Stream/Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMControlImp::LAMControlImp(
	LStream*	inStream)

	: LStdControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ MakeMacControl											   [protected]
// ---------------------------------------------------------------------------

void
LAMControlImp::MakeMacControl(
	ConstStringPtr	inTitle,
	SInt32			inRefCon)
{
	Rect	frame;					// Get Frame in Local coordinates
	if (!CalcLocalFrameRect(frame)) {

			// Frame is outside of QuickDraw space. NewControl requires
			// a rectangle in QuickDraw space, so we have to use an
			// artificial location. The Control won't get drawn until
			// its SuperView scrolls it into view.

		SDimension16	frameSize;
		mControlPane->GetFrameSize(frameSize);

		::MacSetRect(&frame, 0, 0, frameSize.width, frameSize.height);
	}

	StFocusAndClipIfHidden	focus(mControlPane);

			// Controls must be created within a Window. First
			// try the port containing the ControlPane. If that's
			// nil, the ControlPane hasn't been installed yet so
			// we use the current port and make it the responsibility
			// of the caller to set the port correctly.

	WindowPtr		macWindowP = mControlPane->GetMacWindow();
	if (macWindowP == nil) {
		macWindowP = UQDGlobals::GetCurrentWindowPort();
	}

									// Check if there is a root control
	ControlHandle	rootControl;

	OSErr	err = ::GetRootControl(macWindowP, &rootControl);

	if (err == errNoRootControl) {

			// There is no root control, so we must create one.
			// This happens for the first AM control in a window.

		err = ::CreateRootControl(macWindowP, &rootControl);

		if (err == errControlsAlreadyExist) {

			// This error (-30589) means that a Standard (old-style)
			// Toolbox control was created in the window before the
			// first Appearance Toolbox control. If you are mixing old
			// and new style Toolbox controls in the same window, you
			// must create a new style one first.
			//
			// If you are creating a window from a PPob resource, you
			// can satisfy this requirement by opening the PPob in
			// Constructor and choosing "Show Object Hierarchy" from
			// the Layout menu. In the Hierarchy window, arrange the
			// Panes via drag-and-drop so that an Appearance control
			// is created before any old-style control.
			//
			// Note that LListBox and LScroller will create old-style
			// scroll bar controls, and that the Appearance EditText
			// pane is not a new-style controls (since it doesn't
			// actually use a ControlHandle). Use LScrollerView
			// in place of LScroller/LActiveScroller.
			//
			// If you are installing controls at runtime, create an
			// Appearance control first, or call ::CreateRootControl()
			// yourself before creating any Toolbox controls.
			//
			// This workaround is not necessary on Mac OS X, where all
			// windows automatically get a root control.

			SignalStringLiteral_("Must create an Appearance control before "
						 		 "creating an old-style control. See comments "
								 "in the source file.");
		}

		ThrowIfOSErr_(err);
	}

			// Create Toolbox Control. During construction Panes are
			// always invisible and disabled, so we pass "false" for
			// the "visible" parameter to NewControl. Since NewControl
			// always creates enabled controls, we then explicitly
			// disable the control.

	mMacControlH = ::NewControl(macWindowP, &frame, inTitle, false,
								(SInt16) mControlPane->PeekValue(),
								(SInt16) mControlPane->GetMinValue(),
								(SInt16) mControlPane->GetMaxValue(),
								mControlKind, inRefCon);

	ThrowIfNil_(mMacControlH);

	::DeactivateControl(mMacControlH);

			// We need to create a custom control color proc in order to
			// set up the correct background and text color when drawing
			// a control. However, this isn't supported on Mac OS 68K.

	#if !(TARGET_OS_MAC && TARGET_CPU_68K)
	
		if (UEnvironment::HasFeature(env_HasAppearance11)) {
		
			static StControlColorUPP	sControlColorUPP(CustomControlColorProc);
			
			sControlColorUPP.SetColorProc(mMacControlH);
			
				// Store ControlImp and ControlPane as properties so we
				// can retrieve them within the custom color proc
				
			LAMControlImp*	theImp = this;
			
			::SetControlProperty(mMacControlH, PropType_PowerPlant, PropTag_ControlImp, sizeof(LAMControlImp*), &theImp);
			::SetControlProperty(mMacControlH, PropType_PowerPlant, PropTag_ControlPane, sizeof(LControlPane*), &mControlPane);
		}
		
	#endif

	if (mTextTraitsID != 0) {				// Use custom text traits
		ResIDT	saveID = mTextTraitsID;		// Save and set member var to 0
		mTextTraitsID = 0;					//   so SetTextTraits() knows
		SetTextTraitsID(saveID);			//   the value is changing
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LAMControlImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LAMControlImp::~LAMControlImp()
{
}


#if PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf
// ---------------------------------------------------------------------------

void
LAMControlImp::FinishCreateSelf()
{
	ControlRef		ctlRef = GetMacControl();
	EventTargetRef	tgtRef = ::GetControlEventTarget(ctlRef);
	mDrawEvent.Install(tgtRef, kEventClassControl, kEventControlDraw, this, &LAMControlImp::DoDrawEvent);
}
#endif


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LAMControlImp::TrackHotSpot(
	SInt16		/* inHotSpot */,
	Point		inPoint,
	SInt16		inModifiers)
{
		// HandleControlClick handles tracking and returns kControlNoPart
		// if the mouse is released outside the HotSpot

	SInt16	origValue = ::GetControlValue(mMacControlH);
	Boolean	releasedInHotSpot =
		::HandleControlClick( mMacControlH, inPoint,
								(EventModifiers) inModifiers,
								(ControlActionUPP) (-1) )
									!= kControlNoPart;

		// Control Manager can change the value while tracking.
		// If it did, we need to call SetValue() to update the
		// class's copy of the value.

	SInt16	currValue = ::GetControlValue(mMacControlH);
	if (currValue != origValue) {
		mControlPane->SetValue(currValue);
	}

	return releasedInHotSpot;
}


// ---------------------------------------------------------------------------
//	¥ FocusImp														  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::FocusImp()
{
	::PenNormal();

		// With AM 1.1, the custom color proc will set the proper
		// background and text colors.
		
#if !(TARGET_OS_MAC && TARGET_CPU_68K)
	
	if (not UEnvironment::HasFeature(env_HasAppearance11)) {
		mControlPane->GetContainer()->ApplyForeAndBackColors();
	}
	
#else								// 68K code doesn't have color proc, but
									// might still be running under emulation
									// on a PPC with AM 1.1
	mControlPane->GetContainer()->ApplyForeAndBackColors();
	
#endif
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::DrawSelf()
{
		// Most Control Manager calls that draw turn OFF fractional font
		// widths and preferring outline fonts. However,
		// DrawControlInCurrentPort() does not. It respects the current
		// settings. This causes problems for programs that turn ON
		// fractional widths and/or preferring outline fonts because
		// control text is not drawn consistently.
		//
		// To work around this Toolbox bug, we turn OFF fractional widths
		// and set "outline preferred" to false, draw the controls, then
		// restore the settings.
		
	UInt8	saveFract = ::LMGetFractEnable();
	::SetFractEnable(false);

	Boolean	saveOutline = ::GetOutlinePreferred();
	::SetOutlinePreferred(false);

	::DrawControlInCurrentPort(mMacControlH);
	
	::SetOutlinePreferred(saveOutline);
	::SetFractEnable(saveFract != 0);
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::ShowSelf()
{
	::SetControlVisibility(mMacControlH, true, Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ HideSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::HideSelf()
{
	::SetControlVisibility(mMacControlH, false, Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::ActivateSelf()
{
		// Control Manager does not have separate active and enabled
		// states. We call the Toolbox ActivateControl only if the
		// PP Pane is both active and enabled.

	if (mControlPane->GetEnabledState() == triState_On) {

		// PP Panes don't usually draw when activating. Therefore,
		// we suppress the automatic drawing done by the Control
		// Manager and force a refresh so things will draw during
		// the next update event.

		mControlPane->FocusDraw();
		StClipRgnState	dontDraw(nil);

		::ActivateControl(mMacControlH);

		RefreshControlPane();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::DeactivateSelf()
{
		// Do nothing if Toolbox control is already inactive. This happens
		// because the Toolbox does not have separate active and enabled
		// states. If we deactivate and control that is disabled, this
		// is a PP state change but not a Toolbox one.

	if (::IsControlActive(mMacControlH)) {

		// PP Panes don't usually draw when deactivating. Therefore,
		// we suppress the automatic drawing done by the Control
		// Manager and force a refresh so things will draw during
		// the next update event.

		mControlPane->FocusDraw();
		StClipRgnState	dontDraw(nil);

		::DeactivateControl(mMacControlH);

		RefreshControlPane();
	}
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::EnableSelf()
{
		// Control Manager does not have separate active and enabled
		// states. We call the Toolbox ActivateControl only if the
		// PP Pane is both active and enabled.

	if (mControlPane->GetActiveState() == triState_On) {

		// PP Panes don't usually draw when deactivating. Therefore,
		// we suppress the automatic drawing done by the Control
		// Manager and force a refresh so things will draw during
		// the next update event.

		mControlPane->FocusDraw();
		StClipRgnState	dontDraw(nil);

		::ActivateControl(mMacControlH);

		RefreshControlPane();
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::DisableSelf()
{
	DeactivateSelf();
}


// ---------------------------------------------------------------------------
//	¥ RefreshControlPane										   [protected]
// ---------------------------------------------------------------------------
//	Invalidate area covered by bounds of Toolbox control
//
//	This differs from LPane::Refresh() in that we invalidate the entire
//	frame of the Pane instead of just the revealed area. We need to do
//	this to undo the automatic validation performed by the ControlManager
//	in some cases.

void
LAMControlImp::RefreshControlPane()
{
	if (IsControlVisible(mMacControlH)) {	// Only refresh if visible
		Rect	refreshRect;
		mControlPane->CalcPortStructureRect(refreshRect);
		mControlPane->InvalPortRect(&refreshRect);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
		// Setting a data tag may change the size of the structure
		// region of the control. For example, making a push button
		// the default button may put a ring around it.
		//
		// If the structure region becomes smaller, we need to
		// refresh the control pane at its original size so that
		// the area behind the control redraws properly.

	Rect	beforeStructure;			// Get structure before change
	GetStructureOffsets(beforeStructure);
	
										// Make the change. We need to
										//   focus because the control
										//   manager could draw.

	StFocusAndClipIfHidden	focus(mControlPane);
	
#ifdef Debug_Signal
	OSErr err =
#endif
		::SetControlData(mMacControlH, inPartCode, inTag,
						inDataSize, (Ptr) inDataPtr);

	SignalIfOSErr_(err);				// Not setting data isn't fatal

	Rect	afterStructure;				// Get structure after change
	GetStructureOffsets(afterStructure);
	
	if ( (beforeStructure.left   > afterStructure.left)  ||
		 (beforeStructure.top    > afterStructure.top)   ||
		 (beforeStructure.right  > afterStructure.right) ||
		 (beforeStructure.bottom > afterStructure.bottom) ) {
		 
		 	// Before structure is bigger. We refresh the
		 	// ControlPane at the "before" size, then tell
		 	// the ControlPane that the structure changed.
		 
		mControlPane->Refresh();
		mControlPane->ImpStructureChanged();
		
	} else {
			// After structure is bigger or the same.
			// We tell the ControlPane the structure may
			// have changed, then do an immediate redraw.
	
		mControlPane->ImpStructureChanged();
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	OSErr err = ::GetControlData(mMacControlH, inPartCode, inTag,
					inBufferSize, (Ptr) inBuffer, outDataSize);

	ThrowIfOSErr_(err);					// Most likely an invalid tag
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------
//	Set the font style for a control using a Text Traits resource

void
LAMControlImp::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();						// Force redraw to show font change
		
		ControlFontStyleRec	styleRec;
		
		if (inTextTraitsID == Txtr_SystemFont) {
										// Setting no flags will apply the
			styleRec.flags = 0;			//   default system font style
			
		} else {
		
			TextTraitsRecord	textTraits;
			UTextTraits::LoadTextTraits(mTextTraitsID, textTraits);

			styleRec.flags	= kControlUseFontMask +
								kControlUseFaceMask +
								kControlUseSizeMask +
								kControlUseModeMask +
								kControlUseJustMask;

			styleRec.font	= textTraits.fontNumber;
			styleRec.size	= textTraits.size;
			styleRec.style	= textTraits.style;
			styleRec.mode	= textTraits.mode;
			styleRec.just	= textTraits.justification;
			
				// Text Traits does not have a flag to indicate the
				// default color. We interpret black to mean the default
				// color (which it is in most Themes). Thus, we only
				// set the font color if it isn't black.
				//
				// If you really want to use black text, specify a color
				// that's very close to black, such as RGB (1, 1, 1).

			if (textTraits.color != Color_Black) {
				styleRec.flags	  += kControlUseForeColorMask;
				styleRec.foreColor = textTraits.color;
			}
		}
		
		SetFontStyle(styleRec);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::SetFontStyle(
	const ControlFontStyleRec&	inStyleRec)
{
	OSErr err = ::SetControlFontStyle(mMacControlH, &inStyleRec);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::GetFontStyle(
	ControlFontStyleRec&	outStyleRec) const
{
	OSErr err = ::GetControlData(mMacControlH, 0, kControlFontStyleTag,
					sizeof(ControlFontStyleRec), (Ptr) &outStyleRec, nil);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ SetScrollViewSize												  [public]
// ---------------------------------------------------------------------------
//	Used only for scroll bars to set the size of the view being scrolled
//	in order to implement proportional scroll bar thumbs

void
LAMControlImp::SetScrollViewSize(
	SInt32		inViewSize)
{
	StFocusAndClipIfHidden	focus(mControlPane);
	
	UAppearance::SetControlViewSize(mMacControlH, inViewSize);
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------
//	Controls may draw outside the Frame of its ControlPane. The bounds within
//	which all drawing occurs is the Structure. The Structure offsets are
//	values >= 0 that specify how far outside the Frame the Structure is
//	on each side.
//
//	Returns result of calling UAppearance::GetControlRegion(), which will
//	be errMessageNotSupported if AM version is less than 1.1.

OSStatus
LAMControlImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	outOffsets = Rect_0000;
	
									// Control rect is be the same as
									//   the Frame
	Rect		controlRect;
	::GetControlBounds(mMacControlH, &controlRect);
	
									// Ask control for its structure region
	StRegion	structureRgn;
	OSStatus	status = UAppearance::GetControlRegion(
											mMacControlH,
											kControlStructureMetaPart,
											structureRgn);
	
	if (status == noErr) {
		Rect	structureBounds;
		structureRgn.GetBounds(structureBounds);
		
									// Set offset for each side where
									//   structure is outside the control rect
		
		if (structureBounds.left < controlRect.left) {
			outOffsets.left = (SInt16) (controlRect.left - structureBounds.left);
		}
		
		if (structureBounds.right > controlRect.right) {
			outOffsets.right = (SInt16) (structureBounds.right - controlRect.right);
		}
		
		if (structureBounds.top < controlRect.top) {
			outOffsets.top = (SInt16) (controlRect.top - structureBounds.top);
		}
		
		if (structureBounds.bottom > controlRect.bottom) {
			outOffsets.bottom = (SInt16) (structureBounds.bottom - controlRect.bottom);
		}
	}
	
	return status;
}


// ---------------------------------------------------------------------------
//	¥ SupportsCalcBestRect											  [public]
// ---------------------------------------------------------------------------

Boolean
LAMControlImp::SupportsCalcBestRect() const
{
	UInt32	features = 0;			// Check if control supports this feature
	::GetControlFeatures(mMacControlH, &features);
	
	return ((features & kControlSupportsCalcBestRect) != 0);
}


// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::CalcBestControlRect(
	SInt16&		outWidth,
	SInt16&		outHeight,
	SInt16&		outBaselineOffset) const
{
	Rect	bestRect;
	::GetBestControlRect(mMacControlH, &bestRect, &outBaselineOffset);
	
	outWidth  = (SInt16) (bestRect.right - bestRect.left);
	outHeight = (SInt16) (bestRect.bottom - bestRect.top);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Idle															  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::Idle()
{
	StColorPortState	savePortState(mControlPane->GetMacPort());
	
	if (mControlPane->FocusExposed()) {
		::SendControlMessage(mMacControlH, kControlMsgIdle, 0);
		LView::OutOfFocus(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ IdlesAutomatically											  [public]
// ---------------------------------------------------------------------------

bool
LAMControlImp::IdlesAutomatically() const
{
		// Check control feature flags to see if it idles
		// automatically with a Timer

	UInt32	features = 0;
	
	OSErr	err = ::GetControlFeatures(mMacControlH, &features);
	ThrowIfOSErr_(err);
	
	return ((features & kControlIdlesWithTimer) != 0);
}


// ---------------------------------------------------------------------------
//	¥ DoKeyPress													  [public]
// ---------------------------------------------------------------------------
//	Handle keyboard input in a control
//
//	Returns the control part code that was hit by the key event

SInt16
LAMControlImp::DoKeyPress(
	const EventRecord&	inKeyEvent)
{
	StFocusAndClipIfHidden	focus(mControlPane);

	return  ::HandleControlKey(	mMacControlH,
								(SInt16) (inKeyEvent.message & keyCodeMask),
								(SInt16) (inKeyEvent.message & charCodeMask),
								inKeyEvent.modifiers);
}


// ---------------------------------------------------------------------------
//	¥ BeTarget														  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::BeTarget(
	ControlFocusPart	inFocusPart)
{
	if (inFocusPart != kControlFocusNoPart) {
		StFocusAndClipIfHidden	focus(mControlPane);

		::SetKeyboardFocus(::GetControlOwner(mMacControlH), mMacControlH,
							inFocusPart);
	}
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget													  [public]
// ---------------------------------------------------------------------------

void
LAMControlImp::DontBeTarget()
{
	StFocusAndClipIfHidden	focus(mControlPane);

	::SetKeyboardFocus(::GetControlOwner(mMacControlH), mMacControlH,
						kControlFocusNoPart);
}


// ---------------------------------------------------------------------------
//	¥ SetKeyboardFocusPart											  [public]
// ---------------------------------------------------------------------------
//	Set the keyboard focus to the specified part of the control.
//
//	This function does not set the PowerPlant Target. The control should
//	already be the Target beforehand. A typical use of this function is
//	to move the focus within a multipart control in response to the tab key.
//
//	Returns false if the part could not be focused. When passing
//	kControlFocusNextPart or kControlFocusPrevPart, this means that the
//	focus was at the end/start of the subparts. In which case, the focus
//	should move to the next/previous Control or wrap around if there
//	are no other Controls in the tab group.

bool
LAMControlImp::SetKeyboardFocusPart(
	ControlFocusPart		inFocusPart)
{
	StFocusAndClipIfHidden	focus(mControlPane);

		// Call the CDEF directly. The high level routines such as
		// SetKeyboardFocus() have unwanted side effects and don't
		// give us the info we need about wrap around situations.

	SInt32	result = ::SendControlMessage(mMacControlH, kControlMsgFocus, (void*) inFocusPart);

	return (result != kControlFocusNoPart);
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors										  [public]
// ---------------------------------------------------------------------------

bool
LAMControlImp::ApplyForeAndBackColors() const
{
	bool	applied = false;
	
		// Check if Toolbox control has a custom background. If so,
		// let it set the background. If not, we return false and
		// some other SuperView will set the background.

	UInt32	features;
	::GetControlFeatures(mMacControlH, &features);
	
	if (features & kControlHasSpecialBackground) {

			// Tell Toolbox control to set up its background. It could be a
			// color or a pattern.
			
		ControlBackgroundRec	backgroundRec;
		bool					hasColor;
		mControlPane->GetDeviceInfo(backgroundRec.depth, hasColor);
		backgroundRec.colorDevice = hasColor;
		
		::SendControlMessage(mMacControlH, kControlMsgSetUpBackground,
								&backgroundRec);
	
								// Controls don't have their own forecolor.
								// Use SuperView's forecolor.
		RGBColor	foreColor;
		mControlPane->GetContainer()->GetForeAndBackColors(&foreColor, nil);
		::RGBForeColor(&foreColor);
		
		applied = true;			// We have applied the colors
	}
	
	return applied;
}


// ---------------------------------------------------------------------------
//	¥ ApplyTextColor
// ---------------------------------------------------------------------------

void
LAMControlImp::ApplyTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive)
{
	TextTraitsRecord	textTraits;
	UTextTraits::LoadTextTraits(mTextTraitsID, textTraits);

	if (textTraits.color == Color_Black) {

		mControlPane->GetEmbeddedTextColor(inDepth, inHasColor, inIsActive,
											textTraits.color);

	} else if (!IsActive() || !IsEnabled()) {
		UTextDrawing::DimTextColor(textTraits.color);
	}

	::RGBForeColor(&textTraits.color);
}


#if PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ DoDrawEvent
// ---------------------------------------------------------------------------
//	Since draw events can occur asynchronously with respect to the PowerPlant
//	draw system, make sure we're properly focused.

OSStatus
LAMControlImp::DoDrawEvent (
	EventHandlerCallRef				/* inCallRef */,
	EventRef						/* inEventRef */ ) 
{
	mControlPane->FocusDraw();
	
	return eventNotHandledErr;
}
#endif


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CustomControlColorProc
// ---------------------------------------------------------------------------
//	Callback function for setting the background or text color for a
//	Toolbox control
//
//	The background should be set to the content color/pattern for the
//	container of the control, not the control itself, so that erasing
//	will correctly draw what is behind the control.

static pascal OSStatus
CustomControlColorProc(
	ControlHandle	inControl,
	SInt16			inMessage,
	SInt16			inDepth,
	Boolean			inIsColor)
{
	OSStatus	status = noErr;

	try {
		if (inMessage == kControlMsgSetUpBackground) {
		
			LControlPane*	thePane = nil;
			LPane*			container = nil;
			status = ::GetControlProperty(inControl, PropType_PowerPlant, PropTag_ControlPane, sizeof(LControlPane*), nil, &thePane);
			if ((status == noErr) && (thePane != nil)) {
				container = thePane->GetContainer();
			}
			
			if (container != nil) {
				container->ApplyForeAndBackColors();
			}
		
		} else if (inMessage == kControlMsgApplyTextColor) {
		
			LAMControlImp*	theImp = nil;
			status = ::GetControlProperty(inControl, PropType_PowerPlant, PropTag_ControlImp, sizeof(LAMControlImp*), nil, &theImp);
			if ((status == noErr) && (theImp != nil)) {
				theImp->ApplyTextColor(inDepth, inIsColor, ::IsControlActive(inControl));
			}
		}
	}
	
	catch (...) { }
	
	return status;
}


PP_End_Namespace_PowerPlant
