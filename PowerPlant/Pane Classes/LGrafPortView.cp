// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGrafPortView.cp			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A GrafPortView is a top-level PowerPlant View (it's SuperView should be
//	nil) that can be hosted inside a non-PowerPlant window. This allows you
//	to use PowerPlant Panes in other application frameworks or in externals
//	such as HyperCard XMCDs.
//
//	When you create a GrafPortView, you should set the current port to
//	the Toolbox window (or other valid GrafPort) in which you want to
//	place the GrafPortView. Then call LGrafPortView::CreateGrafPortView()
//	to create and initialize the GrafPortView from a PPob resource.
//
//	A GrafPortView acts as the interface between PowerPlant Panes and
//	"foreign" code that knows nothing about PowerPlant. You are responsible
//	for calling proper GrafPortView functions at certain times. Unlike other
//	PowerPlant Panes, GrafPortView always preserves Port settings.
//
//	Initializing PowerPlant
//		Since GrafPortViews can be used without PowerPlant's Application and
//		Window classes, you must initialize some low-level PowerPlant
//		classes yourself.
//
//		* QuickDraw Globals:
//			Call UQDGlobals::SetQDGlobals(&qd) at the beginning of your
//			program. If you are writing a code resource, you must get a
//			pointer to the qd globals (by looking at A5 on a 68K).
//
//		* UEnvironment:
//			Set the flag indicating if the machine has color QuickDraw:
//
//			SInt32		qdVersion = gestaltOriginalQD;
//			Gestalt(gestaltQuickdrawVersion, &qdVersion);
//			UEnvironment::SetFeature(env_SupportsColor,
//									 (qdVersion > gestaltOriginalQD));
//
//	Creating GrafPortViews:
//		* You can create GrafPortViews from PPob resources by registering
//		the LGrafPortView class and then calling CreateGrafPortView().
//
//		* You can also create GrafPortViews manually by using the constructor
//		that takes a SPaneInfo& and SViewInfo& parameters (same as LView).
//
//	Window Events:
//		* Update Event
//			Call UpdatePort() if the GrafPortView is the only thing in
//			the Window. This takes cares of calling BeginUpdate/EndUpdate.
//
//			Call Draw(nil) to draw the GrafPortView if there are other things
//			in the Window. In this case, you must call BeginUpdate/EndUpdate
//			yourself.
//
//		* Activate Event
//			Call Activate()
//
//		* Deactivate Event
//			Call Deactivate()
//
//		* Mouse Down Event
//			Call ClickInContent()
//
//	"Target" Events:
//		* Key Presses
//			Call DoKeyPress();
//
//		* Menu Commands
//			Call DispatchCommand();
//
//	Idle Time:
//		Call DoIdle()
//
//
//	WARNING: Do not delete the GrafPort containing the LGrafPortView
//	when responding to an event that you pass to the LGrafPortView.
//
//	For example, if you have a button in the LGrafPortView that closes
//	the window when clicked, don't delete the window in a LListener object
//	that listens to the button. Instead, set a flag and delete the window
//	after the call to LGrafPortView::ClickInContent() returns.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGrafPortView.h>
#include <LPeriodical.h>
#include <PP_Resources.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UReanimator.h>
#include <UTBAccessors.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ CreateGrafPortView									 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new GrafPortView object initialized from a PPob resource
//
//	GrafPortView is put inside the current GrafPort

LGrafPortView*
LGrafPortView::CreateGrafPortView(
	ResIDT			inGrafPortViewID,
	LCommander*		inSuperCommander)
{
	StColorPortState	mSavePort(UQDGlobals::GetCurrentPort());

	SetDefaultView(nil);				// GrafPortView has no SuperView
	SetDefaultCommander(inSuperCommander);
	
	LGrafPortView	*theGrafPortView =
		UReanimator::ReanimateObjects<LGrafPortView>( ResType_PPob,
													  inGrafPortViewID);
	
	ThrowIfNil_(theGrafPortView);											  
													  
	try {												  
		theGrafPortView->FinishCreate();
	}
	
	catch (...) {						// FinishCreate failed. GrafPortView
		delete theGrafPortView;			//   is in an inconsistent state.
		throw;							//   Delete it and rethrow.
	}

	OutOfFocus(nil);
	return theGrafPortView;
}


// ---------------------------------------------------------------------------
//	¥ LGrafPortView							Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	GrafPortView is put inside the current GrafPort
//	SuperCommander is the default Commander

LGrafPortView::LGrafPortView()
{
	::GetPort(&mGrafPtr);
	InitGrafPortView();
}


// ---------------------------------------------------------------------------
//	¥ LGrafPortView							Constructor				  [public]
// ---------------------------------------------------------------------------
//	GrafPortView is put inside the specified GrafPort
//	SuperCommander is the default Commander

LGrafPortView::LGrafPortView(
	GrafPtr		inGrafPtr)
{
	mGrafPtr = inGrafPtr;
	InitGrafPortView();
}


// ---------------------------------------------------------------------------
//	¥ LGrafPortView							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	GrafPortView is put inside the current GrafPort

LGrafPortView::LGrafPortView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	LCommander*			inSuperCommander)

	: LView(inPaneInfo, inViewInfo),
	  LCommander(inSuperCommander)
{
	::GetPort(&mGrafPtr);
	SignalIf_(mGrafPtr == nil);

	if (mVisible == triState_Latent) {
		mVisible = triState_On;
	}

	mActive = triState_Off;

	if (mEnabled == triState_Latent) {
		mEnabled = triState_On;
	}

	mForeColor = Color_Black;
	mBackColor = Color_White;
}


// ---------------------------------------------------------------------------
//	¥ LGrafPortView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGrafPortView::LGrafPortView(
	LStream*	inStream)

	: LView(inStream)
{
	::GetPort(&mGrafPtr);
	SignalIf_(mGrafPtr == nil);

	if (mVisible == triState_Latent) {
		mVisible = triState_On;
	}

	mActive = triState_Off;

	if (mEnabled == triState_Latent) {
		mEnabled = triState_On;
	}

	mForeColor = Color_Black;
	mBackColor = Color_White;
}


// ---------------------------------------------------------------------------
//	¥ ~LGrafPortView						Destructor				  [public]
// ---------------------------------------------------------------------------

LGrafPortView::~LGrafPortView()
{
		// Delete SubPanes now, rather than from LView's destructor,
		// so that this object still exists while destroying SubPanes.
		// Some Panes access their Port during destruction, which would
		// cause errors if the full LGrafPortView object is not available.

	DeleteAllSubPanes();
}


// ---------------------------------------------------------------------------
//	¥ InitGrafPortView						Initializer				 [private]
// ---------------------------------------------------------------------------

void
LGrafPortView::InitGrafPortView()
{
	SignalIf_(mGrafPtr == nil);

	mFrameLocation.h  = mFrameLocation.v = 0;

	Rect	portRect;
	::GetPortBounds(mGrafPtr, &portRect);

	mFrameSize.width  = (SInt16) (portRect.right - portRect.left);
	mFrameSize.height = (SInt16) (portRect.bottom - portRect.top);

	mFrameBinding.left =
		mFrameBinding.top =
		mFrameBinding.right =
		mFrameBinding.bottom = true;

	CalcPortFrameRect(mRevealedRect);

	mVisible = triState_On;
	mActive  = triState_Off;
	mEnabled = triState_On;

	mForeColor = Color_Black;
	mBackColor = Color_White;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetMacPort													  [public]
// ---------------------------------------------------------------------------
//	Return the Toolbox GrafPort associated with a LGrafPortView

GrafPtr
LGrafPortView::GetMacPort() const
{
	return mGrafPtr;
}


// ---------------------------------------------------------------------------
//	¥ GetMacWindow													  [public]
// ---------------------------------------------------------------------------
//	Return the Toolbox WindowPtr associated with a LGrafPortView

WindowPtr
LGrafPortView::GetMacWindow() const
{
	return ::GetWindowFromPort(mGrafPtr);
}


// ---------------------------------------------------------------------------
//	¥ EstablishPort													  [public]
// ---------------------------------------------------------------------------
//	Make GrafPortView the current Port
//
//	Usage Note: If you call this function directly, you should call
//		LView::OutOfFocus(nil), since changing the current port may
//		invalidate the Focus.

Boolean
LGrafPortView::EstablishPort()
{
	::MacSetPort(mGrafPtr);
	return true;
}


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors										  [public]
// ---------------------------------------------------------------------------
//	Set the foreground and background colors of the current port
//
//	The GrafPortView or one of its SubPanes must already be focused

void
LGrafPortView::ApplyForeAndBackColors() const
{
	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);
}


// ---------------------------------------------------------------------------
//	¥ SetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------
//	Sprecify the foreground and/or background colors of a GrafPortView
//
//	Specify nil for inForeColor and/or inBackColor to leave that
//	color trait unchanged

void
LGrafPortView::SetForeAndBackColors(
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor)
{
	if (inForeColor != nil) {
		mForeColor = *inForeColor;
	}

	if (inBackColor != nil) {
		mBackColor = *inBackColor;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------
//	Pass back the foreground and/or background colors
//
//	Pass nil for outForeColor and/or outBackColor to not retrieve that color

void
LGrafPortView::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = mForeColor;
	}

	if (outBackColor != nil) {
		*outBackColor = mBackColor;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InvalPortRect													  [public]
// ---------------------------------------------------------------------------

void
LGrafPortView::InvalPortRect(
	const Rect*		inRect)
{
	StPortOriginState	portOriginState(mGrafPtr);
	::SetOrigin(0, 0);

#if PP_Target_Carbon

	::InvalWindowRect(GetMacWindow(), inRect);

#else

	::InvalRect(inRect);

#endif
}


// ---------------------------------------------------------------------------
//	¥ InvalPortRgn													  [public]
// ---------------------------------------------------------------------------

void
LGrafPortView::InvalPortRgn(
	RgnHandle	inRgnH)
{
	StPortOriginState	portOriginState(mGrafPtr);
	::SetOrigin(0, 0);

#if PP_Target_Carbon

	::InvalWindowRgn(GetMacWindow(), inRgnH);

#else

	::InvalRgn(inRgnH);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ValidPortRect													  [public]
// ---------------------------------------------------------------------------

void
LGrafPortView::ValidPortRect(
	const Rect*		inRect)
{
	StPortOriginState	portOriginState(mGrafPtr);
	::SetOrigin(0, 0);

#if PP_Target_Carbon

	::ValidWindowRect(GetMacWindow(), inRect);

#else

	::ValidRect(inRect);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ValidPortRgn													  [public]
// ---------------------------------------------------------------------------

void
LGrafPortView::ValidPortRgn(
	RgnHandle	inRgnH)
{
	StPortOriginState	portOriginState(mGrafPtr);
	::SetOrigin(0, 0);

#if PP_Target_Carbon

	::ValidWindowRgn(GetMacWindow(), inRgnH);

#else

	::ValidRgn(inRgnH);

#endif
}


// ---------------------------------------------------------------------------
//	¥ GlobalToPortPoint												  [public]
// ---------------------------------------------------------------------------
//	Convert a point from global (screen) coordinates to a GrafPort's
//	coordinates

void
LGrafPortView::GlobalToPortPoint(
	Point&	ioPoint) const
{
#if TARGET_API_MAC_CARBON			// Can't use portBits technique [below]
									//   under Carbon
	GrafPtr	savePort;
	::GetPort(&savePort);

	Rect	portRect;
	GetPortBounds(mGrafPtr, &portRect);
	Point	saveOrigin = topLeft(portRect);

	::MacSetPort(mGrafPtr);
	::SetOrigin(0, 0);

	::GlobalToLocal(&ioPoint);

	::SetOrigin(saveOrigin.h, saveOrigin.v);
	::MacSetPort(savePort);

#else
		// GrafPorts have a reference to the pixel image of the main screen--
		// portBits for B&W and portPixMap for color GrafPorts. The bounds
		// of the pixel image specify the alignment of the *local* GrafPort
		// coordinates with *global* coordinates:
		//		localPt = globalPt + topLeft(imageBounds)
		//
		// To convert from *local* to *port* coordinates, we offset by
		// the top left of the GrafPort's port rectangle:
		//		portPt = localPt - topLeft(portRect)
		//
		// Therefore,
		//		portPt = globalPt + topLeft(imageBounds) - topLeft(portRect)

									// Assume a B&W Port
	Point	localOffset = topLeft(mGrafPtr->portBits.bounds);
	if (UDrawingUtils::IsColorGrafPort(mGrafPtr)) {
									// Nope, it's a color Port
		CGrafPtr	colorPortP = (CGrafPtr) mGrafPtr;
		localOffset = topLeft((**(colorPortP->portPixMap)).bounds);
	}

	ioPoint.h += (localOffset.h - mGrafPtr->portRect.left);
	ioPoint.v += (localOffset.v - mGrafPtr->portRect.top);

#endif
}


// ---------------------------------------------------------------------------
//	¥ PortToGlobalPoint												  [public]
// ---------------------------------------------------------------------------
//	Convert a point from Port to Global (screen) coordinates
//		[see discussion above for GlobalToPortPoint() for comments]

void
LGrafPortView::PortToGlobalPoint(
	Point&	ioPoint) const
{
#if PP_Target_Carbon				// Can't use portBits technique [below]
									//   under Carbon
	GrafPtr	savePort;
	::GetPort(&savePort);

	Rect	portRect;
	GetPortBounds(mGrafPtr, &portRect);
	Point	saveOrigin = topLeft(portRect);

	::MacSetPort(mGrafPtr);
	::SetOrigin(0, 0);

	::LocalToGlobal(&ioPoint);

	::SetOrigin(saveOrigin.h, saveOrigin.v);
	::MacSetPort(savePort);

#else
									// Assume a B&W Window
	Point	localOffset = topLeft(mGrafPtr->portBits.bounds);
	if (UDrawingUtils::IsColorGrafPort(mGrafPtr)) {
									// Nope, it's a color Window
		CGrafPtr	colorPortP = (CGrafPtr) mGrafPtr;
		localOffset = topLeft((**(colorPortP->portPixMap)).bounds);
	}

	ioPoint.h -= (localOffset.h - mGrafPtr->portRect.left);
	ioPoint.v -= (localOffset.v - mGrafPtr->portRect.top);

#endif
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ UpdatePort													  [public]
// ---------------------------------------------------------------------------
//	Redraw invalidated area of the GrafPortView

void
LGrafPortView::UpdatePort()
{
	StColorPortState	savePortState(mGrafPtr);
	OutOfFocus(nil);
	::SetOrigin(0, 0);					// Set Port coordinates

#if PP_Target_Carbon

	::BeginUpdate(GetMacWindow());

	Rect	portBounds;
	::EraseRect( ::GetPortBounds(mGrafPtr, &portBounds) );

	try {								// Draw what's visible in the Port
		StRegion	theVisRgn;
		::GetPortVisibleRegion(mGrafPtr, theVisRgn);
		Draw(theVisRgn);
	}

	catch (...) {
		SignalStringLiteral_("Drawing exception caught in LGrafPortView::UpdatePort");
	}

	::EndUpdate(GetMacWindow());

#else

	::BeginUpdate(mGrafPtr);

	::EraseRect(&mGrafPtr->portRect);

	try {								// Draw what's visible in the Port
		StRegion	theVisRgn(mGrafPtr->visRgn);
		Draw(theVisRgn);
	}

	catch (...) {
		SignalStringLiteral_("Drawing exception caught in LGrafPortView::UpdatePort");
	}

	::EndUpdate(mGrafPtr);

#endif

	OutOfFocus(nil);
}


// ---------------------------------------------------------------------------
//	¥ Draw															  [public]
// ---------------------------------------------------------------------------
//	Draw a GrafPortView

void
LGrafPortView::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	StColorPortState	savePortState(mGrafPtr);
	OutOfFocus(nil);

	LView::Draw(inSuperDrawRgnH);

	OutOfFocus(nil);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw a GrafPortView
//
//	This function erases the GrafPortView's Frame. Perhaps there should be
//	an option for this ???

void
LGrafPortView::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	ApplyForeAndBackColors();
	::EraseRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ Show															  [public]
// ---------------------------------------------------------------------------
//	Show a GrafPortView

void
LGrafPortView::Show()
{
	if (mVisible == triState_Off) {
		mVisible = triState_On;
		Refresh();
		ShowSelf();
		LView::Show();
	}
}


// ---------------------------------------------------------------------------
//	¥ Activate														  [public]
// ---------------------------------------------------------------------------
//	Activate a GrafPortView

void
LGrafPortView::Activate()
{
	if (mActive == triState_Off) {
		StColorPortState	savePortState(mGrafPtr);
		OutOfFocus(nil);

		mActive = triState_On;
		ActivateSelf();
		LView::Activate();

			// Normally the active Window contains the Target. If this
			// Window can be the Target, restore the Target to what
			// it was when the Window was last active. Don't do this
			// if the Window is already on duty (which happens when
			// the Window is a Superior of a Window that was just
			// deactivated).

		if (!IsOnDuty()) {
			RestoreTarget();
		}

		OutOfFocus(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ Deactivate													  [public]
// ---------------------------------------------------------------------------
//	Deactivate a GrafPortView

void
LGrafPortView::Deactivate()
{
	StColorPortState	savePortState(mGrafPtr);
	OutOfFocus(nil);

	LView::Deactivate();

	if (IsOnDuty()) {

		// Switch target to SuperCommander to prevent an
		// inactive Window from containing the Target

		SwitchTarget(GetSuperCommander());
	}

	OutOfFocus(nil);
}

#pragma mark -

// ===========================================================================
//	USE NOTE
// ===========================================================================
//	When responding to mouse, keyboard, or menu commands, do not delete
//	the GrafPort containing an LGrafPortView from within a call to
//	ClickInContent(), DoKeyPress(), DoIdle(), or DispatchCommand().
//
//	If you need to delete the GrafPort in response to some event (for
//	example, clicking in a Close button), set a flag that you can check
//	after the call to the LGrafPortView function returns.

// ---------------------------------------------------------------------------
//	¥ ClickInContent												  [public]
// ---------------------------------------------------------------------------
//	Respond to a mouse click inside a GrafPortView

void
LGrafPortView::ClickInContent(
	const EventRecord&	inMacEvent)
{
	if (IsEnabled()) {
		StColorPortState	savePortState(mGrafPtr);
		OutOfFocus(nil);

		SMouseDownEvent		theMouseDown;
		theMouseDown.wherePort = inMacEvent.where;

		FocusDraw();
		::GlobalToLocal(&theMouseDown.wherePort);
		LocalToPortPoint(theMouseDown.wherePort);

		theMouseDown.whereLocal = theMouseDown.wherePort;
		theMouseDown.macEvent = inMacEvent;
		theMouseDown.delaySelect = false;

		Click(theMouseDown);
		OutOfFocus(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ DoKeyPress													  [public]
// ---------------------------------------------------------------------------
//	Respond to a key press for a GrafPortView

bool
LGrafPortView::DoKeyPress(
	const EventRecord&	inMacEvent)
{
	bool	keyHandled = false;
									// Pass keystroke to the Target
	LCommander*		theTarget = GetTarget();
	if (theTarget != nil) {
		StColorPortState	savePortState(mGrafPtr);
		keyHandled = theTarget->ProcessKeyPress(inMacEvent);
		OutOfFocus(nil);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ DoIdle														  [public]
// ---------------------------------------------------------------------------
//	Use Idle time for a GrafPortView

void
LGrafPortView::DoIdle(
	const EventRecord&	inMacEvent)
{
										// Give time to Idlers
	StColorPortState	savePortState(mGrafPtr);
	LPeriodical::DevoteTimeToIdlers(inMacEvent);
	OutOfFocus(nil);
}


// ---------------------------------------------------------------------------
//	¥ DispatchCommand												  [public]
// ---------------------------------------------------------------------------
//	Handle a Command for a GrafPortView

void
LGrafPortView::DispatchCommand(
	CommandT	inCommand,
	void*		ioParam)
{
									// Pass Command to the Target
	LCommander*		theTarget = GetTarget();
	if (theTarget != nil) {
		StColorPortState	savePortState(mGrafPtr);
		theTarget->ProcessCommand(inCommand, ioParam);
		OutOfFocus(nil);
	}
}


PP_End_Namespace_PowerPlant
