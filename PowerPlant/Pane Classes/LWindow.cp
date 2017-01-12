// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindow.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LWindow.h>
#include <LModelProperty.h>
#include <LStream.h>
#include <LWindowEventHandlers.h>

#include <PP_Messages.h>
#include <PP_Resources.h>
#include <TArrayIterator.h>
#include <UAppearance.h>
#include <UAppleEventsMgr.h>
#include <UCursor.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UEnvironment.h>
#include <UEventMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMachOFunctions.h>
#include <UMemoryMgr.h>
#include <UProcessMgr.h>
#include <UReanimator.h>
#include <UTBAccessors.h>
#include <UWindows.h>

#include <AEPackObject.h>
#include <AEObjects.h>
#include <AERegistry.h>
#include <MacWindows.h>
#include <Sound.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	$$$ Constant missing in Mac OS X 10.0.x headers
//
//	The constant kSheetAlertWindowClass is defined in Universal Interfaces 3.4
//	but not in the framework headers for Mac OS X 10.0.x. To support the
//	latter system, we define the constant locally.
//
//	Apple may add this constant to a future version of the Mac OS X headers.
//	At this time, there is no documented way to determine a version number
//	for framework headers, so we define the constant for all MachO targets.

namespace	Toolbox {

	#if TARGET_RT_MAC_MACHO
	
		const WindowClass	kSheetAlertWindowClass = 15L;
		
	#else
	
		const WindowClass	kSheetAlertWindowClass = ::kSheetAlertWindowClass;

	#endif
}


// ---------------------------------------------------------------------------
//	Constants

const DescType	pWindowPosition = FOUR_CHAR_CODE('ppos');
const SInt16	length_Big		= 16000;


// ---------------------------------------------------------------------------
//	Class Variables

TArray<WindowPtr>	LWindow::sWindowList;

// ---------------------------------------------------------------------------
//	¥ CreateWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a newly created Window object initialized from a PPob resource

LWindow*
LWindow::CreateWindow(
	ResIDT			inWindowID,
	LCommander*		inSuperCommander)
{
	SetDefaultCommander(inSuperCommander);
	SetDefaultAttachable(nil);
	
	StDeleter<LWindow>	theWindow(
		UReanimator::ReanimateObjects<LWindow>(ResType_PPob, inWindowID) );
		
	ThrowIfNil_(theWindow.Get());
	
	theWindow->FinishCreate();
	
	if (theWindow->HasAttribute(windAttr_ShowNew)) {
		theWindow->Show();
	}

	return theWindow.Release();
}

#if PP_Uses_WindowMgr20_Routines

// ---------------------------------------------------------------------------
//	¥ CreateWindow											 [static] [public]
// ---------------------------------------------------------------------------

LWindow*
LWindow::CreateWindow(
	WindowPtr	inMacWindow,
	ResIDT		inPPobID,
	LCommander*	inSuperCommander)
{
	StDeleter<LWindow>	theWindow(new LWindow(inMacWindow, inSuperCommander));
	
	LView::SetDefaultView(theWindow);
	LCommander::SetDefaultCommander(theWindow);
	LAttachable::SetDefaultAttachable(nil);

	LView*	theView =
		UReanimator::ReanimateObjects<LView>(ResType_PPob, inPPobID);

	ThrowIfNil_(theView);
	
	theView->PutInside(theWindow, false);
	
	theWindow->ExpandSubPane(theView, true, true);

	theWindow->FinishCreate();
	
	return theWindow.Release();
}

#endif // PP_Uses_WindowMgr20_Routines


// ---------------------------------------------------------------------------
//	¥ LWindow								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LWindow::LWindow()
{
	mMacWindowP			 = nil;
	mStandardSize.width  = max_Int16;
	mStandardSize.height = max_Int16;
	mMoveOnlyUserZoom	 = false;
	mAttributes			 = windAttr_Regular;

	::MacSetRect(&mMinMaxSize, 0, 0, length_Big, length_Big);
	mUserBounds = Rect_0000;

	SetModelKind(cWindow);
	SetUseSubModelList(true);

	mForeColor = Color_Black;
	mBackColor = Color_White;

	mActiveBackgroundBrush	 = 0;
	mInactiveBackgroundBrush = 0;
	mActiveTextColor		 = 0;
	mInactiveTextColor		 = 0;
	
	mCGContext = nil;
	
	#if PP_Uses_Carbon_Events
		mEventHandlers = nil;
	#endif
}


// ---------------------------------------------------------------------------
//	¥ LWindow								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct Window from the data in a struct

LWindow::LWindow(
	const SWindowInfo&	inWindowInfo)
{
	InitWindow(inWindowInfo);

	SetModelKind(cWindow);
	SetUseSubModelList(true);
}


// ---------------------------------------------------------------------------
//	¥ LWindow								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a Window from a WIND Resource with the specified attributes
//	and SuperCommander

LWindow::LWindow(
	ResIDT			inWINDid,
	UInt32			inAttributes,
	LCommander*		inSuperCommander)

	: LCommander(inSuperCommander)
{
	mStandardSize.width  = max_Int16;
	mStandardSize.height = max_Int16;
	mMoveOnlyUserZoom	 = false;
	mAttributes			 = inAttributes;
	mCGContext			 = nil;
	
	#if PP_Uses_Carbon_Events
		mEventHandlers = nil;
	#endif

	::MacSetRect(&mMinMaxSize, 0, 0, length_Big, length_Big);

	SetModelKind(cWindow);
	SetUseSubModelList(true);

	MakeMacWindow(inWINDid);
	FocusDraw();
	::GetForeColor(&mForeColor);
	::GetBackColor(&mBackColor);
}


// ---------------------------------------------------------------------------
//	¥ LWindow								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LWindow::LWindow(
	LCommander*		inSuperCommander,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			inProcID,
	UInt32			inAttributes,
	WindowPtr		inBehind)

	: LCommander(inSuperCommander)
{
	mAttributes = inAttributes;

	mMacWindowP = UDesktop::NewDeskWindow(this, inGlobalBounds, inTitle,
								inProcID, HasAttribute(windAttr_CloseBox),
								inBehind);
	ThrowIfNil_(mMacWindowP);

	InitWindow();
}

#if PP_Uses_WindowMgr20_Routines

// ---------------------------------------------------------------------------
//	¥ LWindow								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LWindow::LWindow(
	WindowClass			inWindowClass,
	WindowAttributes	inAttributes,
	const Rect&			inBounds,
	LCommander*			inSuperCommander)

	: LCommander(inSuperCommander)
{
	SignalIfNot_( CFM_AddressIsResolved_(CreateNewWindow) );

	OSStatus	status = ::CreateNewWindow( inWindowClass,
											inAttributes,
											&inBounds,
											&mMacWindowP );
	ThrowIfOSStatus_(status);
	
	SetAttrsFromMacWindow();
	
	InitWindow();
}


// ---------------------------------------------------------------------------
//	¥ LWindow								Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct a LWindow from an existing Mac Window

LWindow::LWindow(
	WindowPtr		inMacWindow,
	LCommander*		inSuperCommander)

	: LCommander(inSuperCommander)
{
	mMacWindowP = inMacWindow;
	
	SetAttrsFromMacWindow();
	
	InitWindow();
}

#endif

// ---------------------------------------------------------------------------
//	¥ LWindow								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LWindow::LWindow(
	LStream*	inStream)
{
	SWindowInfo	windowInfo;
	*inStream >> windowInfo.WINDid;
	*inStream >> windowInfo.layer;
	*inStream >> windowInfo.attributes;
	*inStream >> windowInfo.minimumWidth;
	*inStream >> windowInfo.minimumHeight;
	*inStream >> windowInfo.maximumWidth;
	*inStream >> windowInfo.maximumHeight;
	*inStream >> windowInfo.standardSize.width;
	*inStream >> windowInfo.standardSize.height;
	*inStream >> windowInfo.userCon;
	InitWindow(windowInfo);
	SetDefaultView(this);

	SetModelKind(cWindow);
	SetUseSubModelList(true);
}


// ---------------------------------------------------------------------------
//	¥ ~LWindow								Destructor				  [public]
// ---------------------------------------------------------------------------

LWindow::~LWindow()
{
	if (mMacWindowP != nil) {
										// Hide window to ensure proper
										//   ordering of remaining windows
		UDesktop::HideDeskWindow(this);

			// Delete subpanes *before* disposing of Toolbox WindowRecord.
			// The LView destructor will delete subpanes, but it gets
			// called after this destructor (since LWindow is derived
			// from LView). If we dispose of the Toolbox WindowRecord first,
			// it causes problems for Panes based on other Toolbox data
			// structures, such as standard Controls and TextEdit, that
			// store their own reference to a WindowRecord.

		DeleteAllSubPanes();
										// Forget this WindowPtr
		sWindowList.Remove(mMacWindowP);
		
		#if TARGET_API_MAC_CARBON && PP_Uses_Window_CGContext
		
			if (mCGContext != nil) {	// Release Core Graphics context
				UMachOFunctions::CGContextRelease(mCGContext);
			}
			
		#endif
		
		#if PP_Uses_Carbon_Events
		
			delete mEventHandlers;
		
		#endif

		::DisposeWindow(mMacWindowP);	// Kill Toolbox Window
		mMacWindowP = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ InitWindow
// ---------------------------------------------------------------------------
//	Private Initializer
//
//	mAttributes and mMacWindowP must be set beforehand. This function sets
//	all other instance variables to default values.

void
LWindow::InitWindow()
{
	mStandardSize.width  = max_Int16;
	mStandardSize.height = max_Int16;
	mMoveOnlyUserZoom	 = false;
	mCGContext			 = nil;
	
	#if PP_Uses_Carbon_Events
		mEventHandlers = nil;
	#endif


	::MacSetRect(&mMinMaxSize, 0, 0, length_Big, length_Big);

	sWindowList.AddItem(mMacWindowP);

	SetWindowKind(mMacWindowP, PP_Window_Kind);

									// Stuff object pointer in refcon
	::SetWRefCon(mMacWindowP, (long) this);

									// Window Frame and Image are the same
									//   as its portRect
	Rect	portBounds;
	::GetWindowPortBounds(mMacWindowP, &portBounds);

	ResizeFrameTo((SInt16) (portBounds.right - portBounds.left),
				  (SInt16) (portBounds.bottom - portBounds.top),
				  Refresh_No);

	ResizeImageTo(portBounds.right - portBounds.left,
				  portBounds.bottom - portBounds.top,
				  Refresh_No);

	CalcRevealedRect();

									// Initial size and location are the
									// "user" state for zooming
	CalcPortFrameRect(mUserBounds);
	PortToGlobalPoint(topLeft(mUserBounds));
	PortToGlobalPoint(botRight(mUserBounds));

	mVisible = triState_Off;
	mActive = triState_Off;
	mEnabled = triState_Off;
	if (HasAttribute(windAttr_Enabled)) {
		mEnabled = triState_On;
	}

	FocusDraw();
	::GetForeColor(&mForeColor);
	::GetBackColor(&mBackColor);

	SetModelKind(cWindow);
	SetUseSubModelList(true);

	mActiveBackgroundBrush	 = 0;
	mInactiveBackgroundBrush = 0;
	mActiveTextColor		 = 0;
	mInactiveTextColor		 = 0;
}


// ---------------------------------------------------------------------------
//	¥ InitWindow
// ---------------------------------------------------------------------------
//	Private Initializer from data in a struct

void
LWindow::InitWindow(
	const SWindowInfo	&inWindowInfo)
{
	mAttributes = inWindowInfo.attributes;

								// Set Attribute for Window Layer
	EWindAttr	windLayerAttr = windAttr_Regular;
	if (inWindowInfo.layer == windLayer_Modal) {
		windLayerAttr = windAttr_Modal;
	} else if (inWindowInfo.layer == windLayer_Floating) {
		windLayerAttr = windAttr_Floating;
	}
	SetAttribute(windLayerAttr);

								// Set Min and Max Window Size
	mMinMaxSize.left = inWindowInfo.minimumWidth;
	mMinMaxSize.top = inWindowInfo.minimumHeight;
	mMinMaxSize.right = inWindowInfo.maximumWidth;
	if (mMinMaxSize.right < 0) {
		mMinMaxSize.right = length_Big;
	}
	mMinMaxSize.bottom = inWindowInfo.maximumHeight;
	if (mMinMaxSize.bottom < 0) {
		mMinMaxSize.bottom = length_Big;
	}

								// Set Standard Size for Zooming
	mStandardSize = inWindowInfo.standardSize;
	if (mStandardSize.width < 0) {
		mStandardSize.width = max_Int16;
	}
	if (mStandardSize.height < 0) {
		mStandardSize.height = max_Int16;
	}
	mMoveOnlyUserZoom = false;

	SetUserCon(inWindowInfo.userCon);
	
	mCGContext = nil;

	MakeMacWindow(inWindowInfo.WINDid);
	::SetPortWindowPort(mMacWindowP);
								// Get current window colors
	::GetForeColor(&mForeColor);
	::GetBackColor(&mBackColor);

	FocusDraw();
}


// ---------------------------------------------------------------------------
//	¥ MakeMacWindow
// ---------------------------------------------------------------------------
//	Make a new Mac Window from a WIND resource template

void
LWindow::MakeMacWindow(
	SInt16		inWINDid)
{
	SetPaneID(inWINDid);

	mMacWindowP = UDesktop::NewDeskWindow(this, inWINDid, window_InFront);
	ThrowIfNil_(mMacWindowP);

									// Remember this WindowPtr
	sWindowList.AddItem(mMacWindowP);

		// Inside Mac says that an Application may set the windowKind
		// field of a WindowRecord to any value greater than 8 if
		// desired. PowerPlant requires that the windowKind for
		// windows associated with a LWindow object be >= PP_Window_Kind,
		// which is a large constant. For such windows, PowerPlant
		// stores a pointer to the LWindow object in the refCon.
		// If you wish to use unique kinds for your windows (it's the
		// only way to differentiate LWindow subclasses from just looking
		// at a Toolbox WindowRecord), store the kind number in the
		// refCon field of the WIND resource.

									// Get WIND resource to check refCon
	SWINDResourceH	theWIND = (SWINDResourceH) ::GetResource(ResType_MacWindow, inWINDid);
	SInt16	kind = (SInt16) (**theWIND).refCon;
	if (kind < PP_Window_Kind) {	// Use value from refCon if it is
		kind = PP_Window_Kind;		//   greater than our special number
	}
	SetWindowKind(mMacWindowP, kind);
	::ReleaseResource((Handle) theWIND);

									// Stuff object pointer in refcon
	::SetWRefCon(mMacWindowP, (long) this);

									// Window Frame and Image are the same
									//   as its portRect
	Rect	portBounds;
	::GetWindowPortBounds(mMacWindowP, &portBounds);

	ResizeFrameTo((SInt16) (portBounds.right - portBounds.left),
				  (SInt16) (portBounds.bottom - portBounds.top),
				  Refresh_No);

	ResizeImageTo(portBounds.right - portBounds.left,
				  portBounds.bottom - portBounds.top,
				  Refresh_No);

	CalcRevealedRect();

									// Initial size and location are the
									// "user" state for zooming
	CalcPortFrameRect(mUserBounds);
	PortToGlobalPoint(topLeft(mUserBounds));
	PortToGlobalPoint(botRight(mUserBounds));

	mVisible = triState_Off;
	mActive = triState_Off;
	mEnabled = triState_Off;
	if (HasAttribute(windAttr_Enabled)) {
		mEnabled = triState_On;
	}

	mActiveBackgroundBrush	 = 0;
	mInactiveBackgroundBrush = 0;
	mActiveTextColor		 = 0;
	mInactiveTextColor		 = 0;
}

#if PP_Uses_WindowMgr20_Routines

// ---------------------------------------------------------------------------
//	¥ SetAttrsFromMacWindow
// ---------------------------------------------------------------------------
//	Set the PowerPlant window attributes from the attributes of the
//	underlying Mac window

void
LWindow::SetAttrsFromMacWindow()
{
	SignalIfNot_( CFM_AddressIsResolved_(GetWindowClass) );

		// Map Mac WindowClass to PowerPlant window layer.

	WindowClass		windClass;
	::GetWindowClass(mMacWindowP, &windClass);
	
	switch (windClass) {
	
		case kAlertWindowClass:
		case kMovableAlertWindowClass:
		case kModalWindowClass:
		case kMovableModalWindowClass:
		case kSheetWindowClass:
		case Toolbox::kSheetAlertWindowClass:
			mAttributes = windAttr_Modal;
			break;
			
		case kFloatingWindowClass:
		case kUtilityWindowClass:
		case kHelpWindowClass:
		case kToolbarWindowClass:
			mAttributes = windAttr_Floating;
			break;
			
		default:
			mAttributes = windAttr_Regular;
			break;
	}
	
	WindowAttributes	macAttrs;
	::GetWindowAttributes(mMacWindowP, &macAttrs);
	
	if (macAttrs & kWindowCloseBoxAttribute) {
		SetAttribute(windAttr_CloseBox);
	}
	
									// No Mac attr for whether window has
									//   a title bar. Assume it does.
									//   This attr is only used for the
									//   pHasTitleBar AppleEvent property.
	SetAttribute(windAttr_TitleBar);
	
	if (macAttrs & kWindowResizableAttribute) {
		SetAttribute(windAttr_Resizable);
		SetAttribute(windAttr_SizeBox);
	}
	
	if ( (macAttrs & kWindowHorizontalZoomAttribute)  ||
		 (macAttrs & kWindowVerticalZoomAttribute) ) {
		 
		SetAttribute(windAttr_Zoomable);
	}
	
	SetAttribute(windAttr_Enabled);	// Assume window handles clicks
	
	if (not HasAttribute(windAttr_Floating)) {
									// Non-floating windows are Targetable
		SetAttribute(windAttr_Targetable);
	}
	
	if (HasAttribute(windAttr_Floating)) {
									// Floating windows process the click
									//   that selects thw window as a
									//   regular mouse click
		SetAttribute(windAttr_GetSelectClick);
	}
	
	if (macAttrs & kWindowHideOnSuspendAttribute) {
		SetAttribute(windAttr_HideOnSuspend);
	}
	
		// windAttr_DelaySelect if OFF by default
		
		// winAttr_EraseOnUpdate is OFF by default
}

#endif // PP_Uses_WindowMgr20_Routines


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf
// ---------------------------------------------------------------------------

void
LWindow::FinishCreateSelf()
{
	#if PP_Uses_Carbon_Events
	
		mEventHandlers = new LWindowEventHandlers(this);
		
		mEventHandlers->InstallEventHandlers();
		
	#endif
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FetchWindowObject												  [static]
// ---------------------------------------------------------------------------
//	Return the PowerPlant Window object associated with a Mac WindowPtr
//
//	Returns nil if the WindowPtr is not a PowerPlant Window

LWindow*
LWindow::FetchWindowObject(
	WindowPtr	inWindowP)
{
	LWindow*	ppWindow = nil;
									// LWindow class mainatins a list of
									//   all WindowPtr's that we created
	if ( (inWindowP != nil)  &&
		 sWindowList.ContainsItem(inWindowP) ) {

									// Object pointer is in the refCon
			ppWindow = (LWindow*) ::GetWRefCon(inWindowP);
	}

	return ppWindow;
}


// ---------------------------------------------------------------------------
//	¥ GetMacPort
// ---------------------------------------------------------------------------
//	Return Toolbox GrafPort associated with a Window object

GrafPtr
LWindow::GetMacPort() const
{
	return (GrafPtr) GetWindowPort(mMacWindowP);
}


// ---------------------------------------------------------------------------
//	¥ GetMacWindow
// ---------------------------------------------------------------------------
//	Return Toolbox WindowPtr associated with a Window object

WindowPtr
LWindow::GetMacWindow() const
{
	return mMacWindowP;
}


// ---------------------------------------------------------------------------
//	¥ GetCGContext													  [public]
// ---------------------------------------------------------------------------
//	Return Core Graphics context for this Window
//
//	Depending on your needs, you may want to call GetSyncedCGContext()
//	instead. That call will synchronize the CGContext origin with the
//	window's port.

CGContextRef
LWindow::GetCGContext() const
{
#if TARGET_API_MAC_CARBON && PP_Uses_Window_CGContext

	if ( (mCGContext == nil)  &&
		 CFM_AddressIsResolved_(CreateCGContextForPort) ) {
		 
		::CreateCGContextForPort( GetMacPort(), &mCGContext );
	}
	
#endif
	
	return mCGContext;
}


// ---------------------------------------------------------------------------
//	¥ GetSyncedCGContext											  [public]
// ---------------------------------------------------------------------------
//	Synchronizes CGContext coordinate system with the window's port before
//	returning the CGContextRef.
//
//	The natural CG coordinate system has (0,0) at the bottom left. QD has
//	(0,0) at the top left. Sync'ing is needed when the QD port rect (i.e.,
//	the window bounds) changes size.

CGContextRef
LWindow::GetSyncedCGContext() const
{
#if TARGET_API_MAC_CARBON && PP_Uses_Window_CGContext

	GetCGContext();						// Will create CGContext if necessary
	
	if (mCGContext != nil) {
		::SyncCGContextOriginWithPort(mCGContext, GetMacPort());
	}
	
#endif
	
	return mCGContext;
}


// ---------------------------------------------------------------------------
//	¥ GetGlobalBounds
// ---------------------------------------------------------------------------
//	Pass back the port rectangle of a Window in global coords

void
LWindow::GetGlobalBounds(
	Rect	&outBounds) const
{
	CalcPortFrameRect(outBounds);
	PortToGlobalPoint(topLeft(outBounds));
	PortToGlobalPoint(botRight(outBounds));
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return the title of a Window

StringPtr
LWindow::GetDescriptor(
	Str255	outDescriptor) const
{
	::GetWTitle(mMacWindowP, outDescriptor);
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Set the title of a Window

void
LWindow::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	::SetWTitle(mMacWindowP, inDescriptor);
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------

CFStringRef
LWindow::CopyCFDescriptor() const
{
	CFStringRef	stringRef = nil;

	if (CFM_AddressIsResolved_(CopyWindowTitleAsCFString)) {
		::CopyWindowTitleAsCFString(mMacWindowP, &stringRef);
		
	} else {
		stringRef = LView::CopyCFDescriptor();
	}
	
	return stringRef;
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LWindow::SetCFDescriptor(
	CFStringRef	inStringRef)
{
	if (CFM_AddressIsResolved_(SetWindowTitleWithCFString)) {
		::SetWindowTitleWithCFString(mMacWindowP, inStringRef);
	} else {
		LView::SetCFDescriptor(inStringRef);
	}
}

#endif

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HandleClick
// ---------------------------------------------------------------------------
//	Respond to a click on a Window
//
//	The inPart parameter is the part code returned by the Toolbox FindWindow
//	routine.

void
LWindow::HandleClick(
	const EventRecord	&inMacEvent,
	SInt16				inPart)
{
	switch (inPart) {

		case click_OutsideModal:	// Special case for clicking outside
			::SysBeep(1);			//   a modal window
			break;

		case inContent:
			ClickInContent(inMacEvent);
			break;

		case inDrag:
		case inProxyIcon:
			ClickInTitleBar(inMacEvent, inPart);
			break;

		case inGrow:
			ClickInGrow(inMacEvent);
			break;

		case inGoAway:
			ClickInGoAway(inMacEvent);
			break;

		case inZoomIn:
		case inZoomOut:
			ClickInZoom(inMacEvent, inPart);
			break;

//		case inCollapseBox:			// This part never gets passed. WDEF
//			break;					//   handles it automatically
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInContent
// ---------------------------------------------------------------------------
//	Respond to a click in the content region of a Window

void
LWindow::ClickInContent(
	const EventRecord	&inMacEvent)
{
									// Enabled Windows respond to clicks
	Boolean		respondToClick = HasAttribute(windAttr_Enabled);

									// Set up our extended event record
	SMouseDownEvent		theMouseDown;
	theMouseDown.wherePort	 = inMacEvent.where;
	GlobalToPortPoint(theMouseDown.wherePort);
	theMouseDown.whereLocal	 = theMouseDown.wherePort;
	theMouseDown.macEvent	 = inMacEvent;
	theMouseDown.delaySelect = false;

	if (!UDesktop::WindowIsSelected(this)) {
									// Window is not in front, we might
									//   need to select it
		bool	doSelect = true;
		if (HasAttribute(windAttr_DelaySelect)) {
									// Delay selection until after handling
									//   the click (called click-through)
			theMouseDown.delaySelect = true;
			Click(theMouseDown);

									// After click-through, we select the
									//   Window if the mouse is still down
									//   or the mouse up occurred inside
									//   this Window.

			EventRecord	mouseUpEvent;
			if (!::StillDown() && UEventMgr::GetMouseUp(mouseUpEvent)) {
									// Check location of mouse up event
				WindowPtr	upWindow;
				::MacFindWindow(mouseUpEvent.where, &upWindow);
				doSelect = (upWindow == mMacWindowP);
			}
		}

		if (doSelect) {				// Selecting a Window brings it to the
									//   front of its layer and activates it
			Select();
			respondToClick = HasAttribute(windAttr_GetSelectClick);
		}
	}

	if (respondToClick) {
		if (!theMouseDown.delaySelect) {
			Click(theMouseDown);
		} else {
			sLastPaneClicked = nil;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInTitleBar
// ---------------------------------------------------------------------------

void
LWindow::ClickInTitleBar(
	const EventRecord&	inMacEvent,
	SInt16				inPart)
{
	bool	handled = false;

#if PP_Uses_WindowMgr20_Routines

	if ( UEnvironment::HasFeature(env_HasWindowMgr20) and
		 UDesktop::WindowIsSelected(this) ) {

			// Window Mgr 2.0 supports proxy icons and window path popups

		if (::IsWindowPathSelectClick(mMacWindowP, &inMacEvent)) {

										// Show path popup menu
			SInt32		itemSelected;
			
			OSStatus	status = ::WindowPathSelect(
										mMacWindowP, nil, &itemSelected);

			if ((status == noErr)  &&  (LoWord(itemSelected) > 1)) {
										// User selected an enclosing
										// folder. Switch to the Finder
										// to reveal the folder.
				UProcessMgr::ActivateFinder();
			}
			
			handled = (status != errWindowDoesNotHaveProxy);

		} else if (inPart == inProxyIcon) {	// Use default handler for dragging
											//   the proxy icon
			handled = ::TrackWindowProxyDrag(mMacWindowP, inMacEvent.where) == noErr;
		}
	}
	
#else

	#pragma unused(inPart)

#endif // PP_Uses_WindowMgr20_Routines

	if (not handled) {					// Click not handled yet, so treat
		ClickInDrag(inMacEvent);		//   it as a normal window drag
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInDrag
// ---------------------------------------------------------------------------
//	Handle click in drag region of a Window

void
LWindow::ClickInDrag(
	const EventRecord	&inMacEvent)
{
										// Save old bounds (content region
										//	is in global coords)
#if TARGET_API_MAC_CARBON

	Rect	bounds;
	::GetWindowBounds(mMacWindowP, kWindowContentRgn, &bounds);

#else

	Rect	bounds = (*((WindowPeek)mMacWindowP)->contRgn)->rgnBBox;

#endif

	Rect	dragRect;
	::GetRegionBounds(::GetGrayRgn(), &dragRect);

	::MacInsetRect(&dragRect, 4, 4);
	UDesktop::DragDeskWindow(this, inMacEvent, dragRect);

		// DragWindow will move the window. However, send AppleEvent to
		// set the bounds (but don't execute it), so that Script engines
		// can record the action.

										// Compare old and new bounds
#if TARGET_API_MAC_CARBON

	Rect	newBounds;
	::GetWindowBounds(mMacWindowP, kWindowContentRgn, &newBounds);

#else

	Rect	newBounds = (*((WindowPeek)mMacWindowP)->contRgn)->rgnBBox;

#endif

	if (not ::MacEqualRect(&bounds, &newBounds)) {

		Point	newPosition = topLeft(newBounds);

										// Send but don't execute
										//   SetPosition AppleEvent
		SendAESetPosition(newPosition, ExecuteAE_No);

		AdjustUserBounds();				// Moving Window changes user
										//   bounds for zooming
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInGrow
// ---------------------------------------------------------------------------

void
LWindow::ClickInGrow(
	const EventRecord	&inMacEvent)
{
										// Get current bounds (global coords)
#if TARGET_API_MAC_CARBON

	Rect	bounds;
	::GetWindowBounds(mMacWindowP, kWindowContentRgn, &bounds);

#else

	Rect	bounds = (*((WindowPeek)mMacWindowP)->contRgn)->rgnBBox;

#endif

	Rect	sizeRect = mMinMaxSize;		// GrowWindow trap is off by one
	sizeRect.right += 1;				//   when growing window to its max
	sizeRect.bottom += 1;				//   size. Mostly likely caused by
										//   using the PinRect trap.

			  	  						// Let Toolbox draw the grow image
			  	  						//   newSize has the new height in
			  	  						//   the hi word, width in lo word
	long	newSize = ::GrowWindow(mMacWindowP, inMacEvent.where, &sizeRect);

	if (newSize != 0) {					// If window size changed ...

										// GrowWindow can return a bigger size
										//   than the maximum if the user
										//   holds down the command key
										//   while growing the window.
										//   We assume that the programmer
										//   knows what the maximum size is,
										//   and limit the size accordingly.
		SInt16	newWidth = LoWord(newSize);
		if (newWidth > mMinMaxSize.right) {
			newWidth = mMinMaxSize.right;
		}

		SInt16	newHeight = HiWord(newSize);
		if (newHeight > mMinMaxSize.bottom) {
			newHeight = mMinMaxSize.bottom;
		}

										// Compute new bounds (global coords)
		bounds.right  = (SInt16) (bounds.left + newWidth);
		bounds.bottom = (SInt16) (bounds.top  + newHeight);

										// Change the window size
		SendAESetBounds(&bounds, ExecuteAE_No);
		DoSetBounds(bounds);
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInGoAway
// ---------------------------------------------------------------------------
//	Handle a click inside the close box of a Window

void
LWindow::ClickInGoAway(
	const EventRecord	&inMacEvent)
{
	if (::TrackGoAway(mMacWindowP, inMacEvent.where)) {
		ProcessCommand(cmd_Close);
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickInZoom
// ---------------------------------------------------------------------------
//	Handle a click inside the zoom box of a Window

void
LWindow::ClickInZoom(
	const EventRecord	&inMacEvent,
	SInt16				inZoomDirection)
{
	if (::TrackBox(mMacWindowP, inMacEvent.where, inZoomDirection)) {
		SendAESetZoom();
	}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ UpdatePort
// ---------------------------------------------------------------------------
//	Redraw invalidated area of the Window
//
//	The Mac WindowPtr maintains an update region that defines the area
//	that needs to be redrawn.

void
LWindow::UpdatePort()
{
								// Quick exit if there's nothing to update
#if TARGET_API_MAC_CARBON

	{
		StRegion	updateRgn;
		::GetWindowRegion(mMacWindowP, kWindowUpdateRgn, updateRgn);

		if (::EmptyRgn(updateRgn)) {
			return;
		}
	}

#else

	if (::EmptyRgn(((WindowPeek) mMacWindowP)->updateRgn)) {
		return;
	}

#endif

	GrafPtr		originalPort = UQDGlobals::GetCurrentPort();

	::SetPortWindowPort(mMacWindowP);
	::SetOrigin(0,0);
	OutOfFocus(nil);

	::BeginUpdate(mMacWindowP);

	try {

		#if ACCESSOR_CALLS_ARE_FUNCTIONS

			StRegion	visRgn;
			::GetPortVisibleRegion(GetWindowPort(mMacWindowP), visRgn);
			Draw(visRgn);

		#else

			Draw(mMacWindowP->visRgn);

		#endif
	}

	catch (...) {
		SignalStringLiteral_("Drawing exception caught in LWindow::UpdatePort");
	}

	::EndUpdate(mMacWindowP);

	::MacSetPort(originalPort);
	OutOfFocus(nil);
}


// ---------------------------------------------------------------------------
//	¥ EstablishPort
// ---------------------------------------------------------------------------
//	Make Window the current Port
//
//	Usage Note: If you call this function directly, you should call
//		LView::OutOfFocus(nil), since changing the current port may
//		invalidate the Focus.

Boolean
LWindow::EstablishPort()
{
	Boolean	portSet = (mMacWindowP != nil);
	if (portSet && (UQDGlobals::GetCurrentPort() != GetMacPort())) {
		::SetPortWindowPort(mMacWindowP);
	}
	return portSet;
}


// ---------------------------------------------------------------------------
//	¥ InvalPortRect
// ---------------------------------------------------------------------------

void
LWindow::InvalPortRect(
	const Rect	*inRect)
{
	StPortOriginState	portOriginState((GrafPtr) ::GetWindowPort(mMacWindowP));
	::SetOrigin(0, 0);

#if TARGET_API_MAC_CARBON

	::InvalWindowRect(mMacWindowP, inRect);

#else

	::InvalRect(inRect);

#endif
}


// ---------------------------------------------------------------------------
//	¥ InvalPortRgn
// ---------------------------------------------------------------------------

void
LWindow::InvalPortRgn(
	RgnHandle	inRgnH)
{
	StPortOriginState	portOriginState((GrafPtr) ::GetWindowPort(mMacWindowP));
	::SetOrigin(0, 0);

#if TARGET_API_MAC_CARBON

	::InvalWindowRgn(mMacWindowP, inRgnH);

#else

	::InvalRgn(inRgnH);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ValidPortRect
// ---------------------------------------------------------------------------

void
LWindow::ValidPortRect(
	const Rect	*inRect)
{
	StPortOriginState	portOriginState((GrafPtr) ::GetWindowPort(mMacWindowP));
	::SetOrigin(0, 0);

#if TARGET_API_MAC_CARBON

	::ValidWindowRect(mMacWindowP, inRect);

#else

	::ValidRect(inRect);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ValidPortRgn
// ---------------------------------------------------------------------------

void
LWindow::ValidPortRgn(
	RgnHandle	inRgnH)
{
	StPortOriginState	portOriginState((GrafPtr) ::GetWindowPort(mMacWindowP));
	::SetOrigin(0, 0);

#if TARGET_API_MAC_CARBON

	::ValidWindowRgn(mMacWindowP, inRgnH);

#else

	::ValidRgn(inRgnH);

#endif
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		inWidthDelta and inHeightDelta specify, in pixels, how much larger
//		to make the Frame. Positive deltas increase the size, negative deltas
//		reduce the size.
//
//	Note that this function only changes the internal Frame size of the
//	Window object. To change the actual size (on screen) of the Window,
//	call ResizeWindowBy() or ResizeWindowTo().

void
LWindow::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	Rect	beforeFrame;
	CalcPortFrameRect(beforeFrame);

	mFrameSize.width  += inWidthDelta;
	mFrameSize.height += inHeightDelta;

	CalcRevealedRect();
	OutOfFocus(this);

	if (inRefresh) {
		Rect	afterRect;
		CalcPortFrameRect(afterRect);

		StRegion	beforeRgn(beforeFrame);
		StRegion	refreshRgn(afterRect);

		refreshRgn -= beforeRgn;

		if (HasAttribute(windAttr_SizeBox)) {
			Rect	sizeBox = beforeFrame;
			sizeBox.left = (SInt16) (sizeBox.right - 15);
			sizeBox.top = (SInt16) (sizeBox.bottom - 15);
			::RectRgn(beforeRgn, &sizeBox);
			::MacUnionRgn(refreshRgn, beforeRgn, refreshRgn);

			sizeBox = afterRect;
			sizeBox.left = (SInt16) (sizeBox.right - 15);
			sizeBox.top = (SInt16) (sizeBox.bottom - 15);
			beforeRgn = sizeBox;
			refreshRgn += beforeRgn;
		}

		InvalPortRgn(refreshRgn);
	}

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->AdaptToSuperFrameSize(inWidthDelta, inHeightDelta, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ Select
// ---------------------------------------------------------------------------

void
LWindow::Select()
{
	if (not UDesktop::WindowIsSelected(this)) {
		SendSelfAE(kAEMiscStandards, kAESelect, ExecuteAE_No);
		UDesktop::SelectDeskWindow(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustContentMouse
// ---------------------------------------------------------------------------
//	Adjust cursor when mouse is inside Window's content region

void
LWindow::AdjustContentMouse(
	Point				inPortPt,
	const EventRecord&	inMacEvent,
	RgnHandle			outMouseRgn)
{
									// Active and Enabled subpane will
									//   set the cursor. The mouseRgn
									//   is in Port coords.
	AdjustMouse(inPortPt, inMacEvent, outMouseRgn);
	
									// Convert mouse region from Port
									//   to Global coords
	Point	globalPt;
	Rect	rgnBounds;
	
	::GetRegionBounds(outMouseRgn, &rgnBounds);
	
	globalPt = topLeft(rgnBounds);
	PortToGlobalPoint(globalPt);
	
	::OffsetRgn( outMouseRgn, (SInt16) (globalPt.h - rgnBounds.left),
							  (SInt16) (globalPt.v - rgnBounds.top) );
							  
							  		// Subtract structure regions of windows
							  		//   above us from the mouse region
	UWindows::ExcludeStructuresAbove(mMacWindowP, outMouseRgn);
}


// ---------------------------------------------------------------------------
//	¥ AdjustStructureMouse
// ---------------------------------------------------------------------------
//	Adjust cursor when mouse is inside Window's structure region

void
LWindow::AdjustStructureMouse(
	WindowPartCode		/* inPart */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			outMouseRgn)
{
	UCursor::SetArrow();

	#if TARGET_API_MAC_CARBON
	
		::GetWindowRegion(mMacWindowP, kWindowStructureRgn, outMouseRgn);
		
		StRegion	contentRgn;
		
		::GetWindowRegion(mMacWindowP, kWindowContentRgn, contentRgn);
		
		::DiffRgn(outMouseRgn, contentRgn, outMouseRgn);
	
	#else
	
		::DiffRgn( ((WindowPeek) mMacWindowP)->strucRgn,
				   ((WindowPeek) mMacWindowP)->contRgn,
				   outMouseRgn );
	
	#endif
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Show
// ---------------------------------------------------------------------------

void
LWindow::Show()
{
	if (mVisible == triState_Off) {
		mVisible = triState_On;
		LView::Show();				// With CarbonEvents, windows can draw
									// before actually being shown on screen.
									// The drawing occurs during ShowWindow().
									// By making subpanes visible first, we
									// can draw properly in ShowSelf().
		ShowSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------

void
LWindow::ShowSelf()
{
	UDesktop::ShowDeskWindow(this);
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------

void
LWindow::HideSelf()
{
	UDesktop::HideDeskWindow(this);
	Deactivate();
}


// ---------------------------------------------------------------------------
//	¥ Activate
// ---------------------------------------------------------------------------

void
LWindow::Activate()
{
	if ( (mActive == triState_Off)  &&  IsVisible() ) {
		mActive = triState_On;
		ActivateSelf();
		LView::Activate();

			// Normally the active Window contains the Target. If this
			// Window can be the Target, restore the Target to what
			// it was when the Window was last active. Don't do this
			// if the Window is already on duty (which happens when
			// the Window is a Superior of a Window that was just
			// deactivated).

		if (HasAttribute(windAttr_Targetable) && !IsOnDuty()) {
			RestoreTarget();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf
// ---------------------------------------------------------------------------

void
LWindow::ActivateSelf()
{
	#if !(TARGET_OS_MAC && TARGET_CPU_68K)

		if ( (mActiveBackgroundBrush != 0)  &&
			 UEnvironment::HasFeature(env_HasAppearance11) ) {
			 
			::SetThemeWindowBackground(mMacWindowP, mActiveBackgroundBrush, false);
		}
		
	#endif

	::HiliteWindow(mMacWindowP, true);
	DrawSizeBox();
}


// ---------------------------------------------------------------------------
//	¥ Deactivate
// ---------------------------------------------------------------------------

void
LWindow::Deactivate()
{
	LView::Deactivate();

	if (IsOnDuty()) {

		// Switch target to SuperCommander to prevent an
		// inactive Window from containing the Target

		ForceTargetSwitch(GetSuperCommander());
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf
// ---------------------------------------------------------------------------

void
LWindow::DeactivateSelf()
{
	#if !(TARGET_OS_MAC && TARGET_CPU_68K)

		if ( (mInactiveBackgroundBrush != 0)  &&
			 UEnvironment::HasFeature(env_HasAppearance11) ) {
			 
			::SetThemeWindowBackground(mMacWindowP, mInactiveBackgroundBrush, false);
		}
		
	#endif

	::HiliteWindow(mMacWindowP, false);
	DrawSizeBox();
}


// ---------------------------------------------------------------------------
//	¥ Enable
// ---------------------------------------------------------------------------

void
LWindow::Enable()
{
	if (mEnabled == triState_Off) {
		mEnabled = triState_On;
		EnableSelf();
		LView::Enable();
	}
}


// ---------------------------------------------------------------------------
//	¥ Suspend
// ---------------------------------------------------------------------------

void
LWindow::Suspend()
{
	if (HasAttribute(windAttr_HideOnSuspend)) {
		SuperHide();

	} else if (IsActive()) {
		Deactivate();
	}
}


// ---------------------------------------------------------------------------
//	¥ Resume
// ---------------------------------------------------------------------------

void
LWindow::Resume()
{
	Boolean	modalInFront = UDesktop::FrontWindowIsModal();

	if (HasAttribute(windAttr_HideOnSuspend)) {
		SuperShow();
	}

	if (HasAttribute(windAttr_Floating) && not modalInFront) {
		Activate();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AttemptClose
// ---------------------------------------------------------------------------
//	Try to close a Window as a result of direct user action

void
LWindow::AttemptClose()
{
									// Get approval from SuperCommander
	if ((mSuperCommander == nil) || mSuperCommander->AllowSubRemoval(this)) {

		 							// Send Close AE for recording only
		SendSelfAE(kAECoreSuite, kAEClose, ExecuteAE_No);
		delete this;
	}
}


// ---------------------------------------------------------------------------
//	¥ DoClose
// ---------------------------------------------------------------------------
//	Close a Window

void
LWindow::DoClose()
{
									// Get approval from SuperCommander
	if ((mSuperCommander == nil) || mSuperCommander->AllowSubRemoval(this)) {
		delete this;
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------

void
LWindow::DrawSelf()
{
	if (HasAttribute(windAttr_EraseOnUpdate)) {
		ApplyForeAndBackColors();

		Rect	bounds;
		::EraseRect( ::GetWindowPortBounds(mMacWindowP, &bounds) );
	}

	DrawSizeBox();
}


// ---------------------------------------------------------------------------
//	¥ DrawSizeBox
// ---------------------------------------------------------------------------
//	Draw standard size box for resizable Windows

void
LWindow::DrawSizeBox()
{
	if (HasAttribute(windAttr_SizeBox)) {

			// The Toolbox trap DrawGrowIcon draws the size box in
			// the lower right corner of a Window, but it also outlines
			// the typical scroll bar areas at the right and bottom of
			// a Window. We want this routine to work for Windows that
			// don't necessarily have standard scroll bars, so we
			// temporarily change the clipping region to draw just the
			// size box.

		FocusDraw();

		Rect	sizeBox;
		::GetWindowPortBounds(mMacWindowP, &sizeBox);

		sizeBox.left = (SInt16) (sizeBox.right - 15);
		sizeBox.top =  (SInt16) (sizeBox.bottom - 15);

		StClipRgnState	saveClip(sizeBox);
		::DrawGrowIcon(mMacWindowP);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------
//	Set the foreground and background colors of the current port
//
//	The Window or one of its SubPanes must already be focused

void
LWindow::ApplyForeAndBackColors() const
{
	::RGBForeColor(&mForeColor);

	ThemeBrush	backBrush = 0;

	if (UEnvironment::HasFeature(env_HasAppearance101)) {

		backBrush = mInactiveBackgroundBrush;
		if (IsActive()) {
			backBrush = mActiveBackgroundBrush;
		}
	}

	SInt16	bitDepth;
	bool	hasColor;
	GetDeviceInfo(bitDepth, hasColor);

	if (backBrush != 0) {
		UAppearance::SetThemeBackground(backBrush, bitDepth, hasColor);

	} else {							// No background Brush

										// Must reset backpat to white since
										//   some Themes set it differently
		Pattern	whitePat;
		::BackPat( UQDGlobals::GetWhitePat(&whitePat) );

			// Normally, we will use the background color for
			// the Window. However, we use a white background if
			// the current Theme is not in color.

		RGBColor	backColor = mBackColor;

		if ( UEnvironment::HasFeature(env_HasAppearance101)  &&
			 not ::IsThemeInColor(bitDepth, hasColor) ) {

			backColor = Color_White;
		}

		::RGBBackColor(&backColor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetForeAndBackColors
// ---------------------------------------------------------------------------
//	Sprecify the foreground and/or background colors of a Window
//
//	Specify nil for inForeColor and/or inBackColor to leave that
//	color trait unchanged

void
LWindow::SetForeAndBackColors(
	const RGBColor	*inForeColor,
	const RGBColor	*inBackColor)
{
	if (inForeColor != nil) {
		mForeColor = *inForeColor;
	}

	if (inBackColor != nil) {
		mBackColor = *inBackColor;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors
// ---------------------------------------------------------------------------
//	Pass back the foreground and/or background colors of a Window
//
//	Pass nil for outForeColor and/or outBackColor to not retrieve that color

void
LWindow::GetForeAndBackColors(
	RGBColor	*outForeColor,
	RGBColor	*outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = mForeColor;
	}

	if (outBackColor != nil) {

			// Normally, we will use the background color for
			// the Window. However, we use a white background if
			// the current Theme is not in color.

		SInt16	bitDepth;
		bool	hasColor;
		GetDeviceInfo(bitDepth, hasColor);

		if ( UEnvironment::HasFeature(env_HasAppearance101)  &&
			 not ::IsThemeInColor(bitDepth, hasColor) ) {

			*outBackColor = Color_White;

		} else {
			*outBackColor = mBackColor;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetThemeData
// ---------------------------------------------------------------------------

void
LWindow::SetThemeData(
	ThemeBrush		inActiveBackBrush,
	ThemeBrush		inInactiveBackBrush,
	ThemeTextColor	inActiveTextColor,
	ThemeTextColor	inInactiveTextColor)
{
	mActiveBackgroundBrush	 = inActiveBackBrush;
	mInactiveBackgroundBrush = inInactiveBackBrush;
	mActiveTextColor		 = inActiveTextColor;
	mInactiveTextColor		 = inInactiveTextColor;
	
	#if !(TARGET_OS_MAC && TARGET_CPU_68K)
	
		ThemeBrush	backBrush = 0;

		if (UEnvironment::HasFeature(env_HasAppearance11)) {

			backBrush = mInactiveBackgroundBrush;
			if (IsActive()) {
				backBrush = mActiveBackgroundBrush;
			}
		}
		
		if (backBrush != 0) {
			::SetThemeWindowBackground(mMacWindowP, backBrush, false);
		}
		
	#endif
}


// ---------------------------------------------------------------------------
//	¥ GetEmbeddedTextColor
// ---------------------------------------------------------------------------
//	Pass back the color for drawing text within a Window

void
LWindow::GetEmbeddedTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive,
	RGBColor&	outColor) const
{
	ThemeTextColor	color = mInactiveTextColor;
	if (inIsActive) {
		color = mActiveTextColor;
	}

	if (color != 0) {
		UAppearance::GetThemeTextColor(color, inDepth, inHasColor,
										inIsActive, &outColor);

	} else {
		LView::GetEmbeddedTextColor(inDepth, inHasColor, inIsActive, outColor);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GlobalToPortPoint
// ---------------------------------------------------------------------------
//	Convert a point from global (screen) coordinates to a Window's Port
//	coordinates

void
LWindow::GlobalToPortPoint(
	Point	&ioPoint) const
{
#if TARGET_API_MAC_CARBON			// Can't use portBits technique [below]
									//   under Carbon
	GrafPtr	savePort;
	::GetPort(&savePort);

	Rect	portRect;
	::GetWindowPortBounds(mMacWindowP, &portRect);
	Point	saveOrigin = topLeft(portRect);

	::SetPortWindowPort(mMacWindowP);
	::SetOrigin(0, 0);

	::GlobalToLocal(&ioPoint);

	::SetOrigin(saveOrigin.h, saveOrigin.v);
	::MacSetPort(savePort);

#else
		// Windows have a reference to the pixel image of the main screen--
		// portBits for B&W and portPixMap for color windows. The bounds
		// of the pixel image specify the alignment of the *local* Window
		// coordinates with *global* coordinates:
		//		localPt = globalPt + topLeft(imageBounds)
		//
		// To convert from *local* to *port* coordinates, we offset by
		// the top left of the Window's port rectangle:
		//		portPt = localPt - topLeft(portRect)
		//
		// Therefore,
		//		portPt = globalPt + topLeft(imageBounds) - topLeft(portRect)

									// Assume a B&W Window
	Point	localOffset = topLeft(mMacWindowP->portBits.bounds);
	if (UDrawingUtils::IsColorGrafPort(mMacWindowP)) {
									// Nope, it's a color Window
		CGrafPtr	colorPortP = (CGrafPtr) mMacWindowP;
		localOffset = topLeft((**(colorPortP->portPixMap)).bounds);
	}

	ioPoint.h += (SInt16) (localOffset.h - mMacWindowP->portRect.left);
	ioPoint.v += (SInt16) (localOffset.v - mMacWindowP->portRect.top);

#endif
}


// ---------------------------------------------------------------------------
//	¥ PortToGlobalPoint
// ---------------------------------------------------------------------------
//	Convert a point from Port to Global (screen) coordinates
//		[see discussion above for GlobalToPortPoint() for comments]

void
LWindow::PortToGlobalPoint(
	Point	&ioPoint) const
{
#if TARGET_API_MAC_CARBON			// Can't use portBits technique [below]
									//   under Carbon
	GrafPtr	savePort;
	::GetPort(&savePort);

	Rect	portRect;
	::GetWindowPortBounds(mMacWindowP, &portRect);
	Point	saveOrigin = topLeft(portRect);

	::SetPortWindowPort(mMacWindowP);
	::SetOrigin(0, 0);

	::LocalToGlobal(&ioPoint);

	::SetOrigin(saveOrigin.h, saveOrigin.v);
	::MacSetPort(savePort);

#else
									// Assume a B&W Window
	Point	localOffset = topLeft(mMacWindowP->portBits.bounds);
	if (UDrawingUtils::IsColorGrafPort(mMacWindowP)) {
									// Nope, it's a color Window
		CGrafPtr	colorPortP = (CGrafPtr) mMacWindowP;
		localOffset = topLeft((**(colorPortP->portPixMap)).bounds);
	}

	ioPoint.h -= (SInt16) (localOffset.h - mMacWindowP->portRect.left);
	ioPoint.v -= (SInt16) (localOffset.v - mMacWindowP->portRect.top);

#endif
}


// ---------------------------------------------------------------------------
//	¥ ExpandSubPane
// ---------------------------------------------------------------------------
//	Resize a SubPane so that it is the same size as the Window,
//	horizontally and/or vertically

void
LWindow::ExpandSubPane(
	LPane	*inSub,
	Boolean	inExpandHoriz,
	Boolean	inExpandVert)
{
	SDimension16	subSize;
	inSub->GetFrameSize(subSize);
	SPoint32		subLocation;
	inSub->GetFrameLocation(subLocation);

		// SubPane overlaps 1-pixel window border

	if (inExpandHoriz) {
		subSize.width = (SInt16) (mFrameSize.width + 2);
		subLocation.h = -1;
	}

	if (inExpandVert) {
		subSize.height = (SInt16) (mFrameSize.height + 2);
		subLocation.v = -1;
	}

	inSub->PlaceInSuperFrameAt(subLocation.h, subLocation.v, false);
	inSub->ResizeFrameTo(subSize.width, subSize.height, false);
}


// ===========================================================================
// ¥ Sending Apple Events								Sending Apple Events ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ SendAESetPosition
// ---------------------------------------------------------------------------
//	AppleEvent for moving a Window to a new position
//
//	inPosition is the location for the top left corner of the Window's
//		port rectangle, in Global coordinates
//	Set inExecuteAE to true to actually move the Window, false to
//	just send the event for script recording purposes. You'll use the
//	false value when you have already moved the window in response to
//	tracking user actions (the Toolbox trap DragWindow move the Window).

void
LWindow::SendAESetPosition(
	Point	inPosition,
	Boolean	inExecuteAE)
{
	try {
		LModelProperty	positionProperty(pWindowPosition, this, false);
		positionProperty.SendSetDataAE(typeQDPoint, (Ptr) &inPosition,
											sizeof(Point), inExecuteAE);
	}

	catch (...) {
		if (inExecuteAE) {
			DoSetPosition(inPosition);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SendAESetBounds
// ---------------------------------------------------------------------------
//	AppleEvent for changing the size of a Window
//
//	inBounds is the new port rectangle for the Window, in Global coordinates.
//	Set inExecuteAE to true to actually resize the Window, false to
//	just send the event for script recording purposes.

void
LWindow::SendAESetBounds(
	Rect	*inBounds,					// New bounds in global coords
	Boolean	inExecuteAE)
{
	try {
		LModelProperty	positionProperty(pBounds, this, false);
		positionProperty.SendSetDataAE(typeQDRectangle, (Ptr) inBounds,
											sizeof(Rect), inExecuteAE);
	}

	catch (...) {
		if (inExecuteAE) {
			DoSetBounds(*inBounds);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SendAESetZoom
// ---------------------------------------------------------------------------
//	AppleEvent for zooming a Window
//
//	This function figures out whether to zoom in or out based on the
//	current window size and location

void
LWindow::SendAESetZoom()
{
										// Determine zoom direction
	Rect	stdBounds;
	Boolean	zoomToStdState = !CalcStandardBounds(stdBounds);

	try {
		LModelProperty	zoomProperty(pIsZoomed, this, false);
		zoomProperty.SendSetDataAE(typeBoolean, (Ptr) &zoomToStdState,
											sizeof(Boolean), ExecuteAE_No);
	}

	catch (...) { }

	DoSetZoom(zoomToStdState);
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Handle a command

Boolean
LWindow::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case cmd_Close:
			AttemptClose();
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
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
LWindow::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Close:
			outEnabled = HasAttribute(windAttr_CloseBox);
			break;

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ===========================================================================
// ¥ AppleEvent Object Model Support		 AppleEvent Object Model Support ¥
// ===========================================================================
#pragma mark -
// ---------------------------------------------------------------------------
//	¥ MakeSelfSpecifier
// ---------------------------------------------------------------------------
//	Make an Object Specifier for a Window

void
LWindow::MakeSelfSpecifier(
	AEDesc&	inSuperSpecifier,
	AEDesc&	outSelfSpecifier) const
{
	if (mSuperModel == nil) {

			// For Windows, we often use an "abbreviated" container
			// hierarchy where the SuperModel of a Window is null
			// rather than being the Application.

		DescType		keyForm;
		StAEDescriptor	keyData;
		OSErr			err;

			// Specify by name if the Window's model object name
			// is not empty

		Str255	name;
		GetModelName(name);

		if (name[0] != 0) {
			keyForm = formName;
			keyData.Assign(name);

		} else {			// Specify by position in the window list

			SInt32	windowIndex = UWindows::FindWindowIndex(mMacWindowP);

			keyForm = formAbsolutePosition;
			keyData.Assign(windowIndex);
		}

		err = ::CreateObjSpecifier(	cWindow,
									&inSuperSpecifier,
									keyForm,
									keyData,
									false,		// Don't dispose inputs
									&outSelfSpecifier);
		ThrowIfOSErr_(err);

	} else {
		LModelObject::MakeSelfSpecifier(inSuperSpecifier, outSelfSpecifier);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetAEProperty
// ---------------------------------------------------------------------------

void
LWindow::GetAEProperty(
	DescType		inProperty,
	const AEDesc&	inRequestedType,
	AEDesc&			outPropertyDesc) const
{
	OSErr	err;

	switch (inProperty) {

		case pName: {					// Window Title
			Str255	theName;
			::GetWTitle(mMacWindowP, theName);
			err = ::AECreateDesc(typeChar, (Ptr) theName + 1,
								StrLength(theName), &outPropertyDesc);
			break;
		}

		case pWindowPosition: {		// Top left of Frame in Global coords
			Point	thePosition = Point_00;
			PortToGlobalPoint(thePosition);
			err = ::AECreateDesc(typeQDPoint, (Ptr) &thePosition,
								sizeof(Point), &outPropertyDesc);
			break;
		}

		case pBounds: {				// Frame in Global coords
			Rect	theBounds;
			GetGlobalBounds(theBounds);
			err = ::AECreateDesc(typeQDRectangle, (Ptr) &theBounds,
								sizeof(Rect), &outPropertyDesc);
			break;
		}

		case pIndex: {				// Front-to-back position index
			SInt32	theIndex = UWindows::FindWindowIndex(mMacWindowP);
			err = ::AECreateDesc(typeLongInteger, (Ptr) &theIndex,
								sizeof(SInt32), &outPropertyDesc);
			break;
		}

		case pIsZoomed: {			// Is Window at Standard state?
			Rect	stdBounds;
			Boolean	isZoomed = CalcStandardBounds(stdBounds);
			err = ::AECreateDesc(typeBoolean, (Ptr) &isZoomed,
								sizeof(Boolean), &outPropertyDesc);
			break;
		}

		case pHasCloseBox:
			GetAEWindowAttribute(windAttr_CloseBox, outPropertyDesc);
			break;

		case pHasTitleBar:
			GetAEWindowAttribute(windAttr_TitleBar, outPropertyDesc);
			break;

		case pIsFloating:
			GetAEWindowAttribute(windAttr_Floating, outPropertyDesc);
			break;

		case pIsModal:
			GetAEWindowAttribute(windAttr_Modal, outPropertyDesc);
			break;

		case pIsResizable:
			GetAEWindowAttribute(windAttr_Resizable, outPropertyDesc);
			break;

		case pIsZoomable:			// Can Window be zoomed?
			GetAEWindowAttribute(windAttr_Zoomable, outPropertyDesc);
			break;

		case pVisible: {
			Boolean	isVis = IsVisible();
			err = ::AECreateDesc(typeBoolean, (Ptr) &isVis,
								sizeof(Boolean), &outPropertyDesc);
			break;
		}

		default:
			LModelObject::GetAEProperty(inProperty, inRequestedType,
											outPropertyDesc);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetAEProperty
// ---------------------------------------------------------------------------

void
LWindow::SetAEProperty(
	DescType		inProperty,
	const AEDesc&	inValue,
	AEDesc&			outAEReply)
{
	switch (inProperty) {

		case pName: {
			Str255	theName;
			UExtractFromAEDesc::ThePString(inValue, theName, sizeof(theName));
			SetDescriptor(theName);
			break;
		}

		case pWindowPosition: {
			Point	thePosition;
			UExtractFromAEDesc::ThePoint(inValue, thePosition);
			DoSetPosition(thePosition);
			break;
		}

		case pBounds: {
			Rect	theBounds;
			UExtractFromAEDesc::TheRect(inValue, theBounds);
			DoSetBounds(theBounds);
			break;
		}

		case pIsZoomed: {
			Boolean	isZoomed;
			UExtractFromAEDesc::TheBoolean(inValue, isZoomed);
			DoSetZoom(isZoomed);
			break;
		}

		case pVisible: {
			Boolean	makeVisible;
			UExtractFromAEDesc::TheBoolean(inValue, makeVisible);
			if (makeVisible) {
				Show();
			} else {
				Hide();
			}
			break;
		}

		default:
			LModelObject::SetAEProperty(inProperty, inValue, outAEReply);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ AEPropertyExists
// ---------------------------------------------------------------------------

bool
LWindow::AEPropertyExists(
	DescType	inProperty) const
{
	bool	exists = false;

	switch (inProperty) {

		case pName:
		case pWindowPosition:
		case pBounds:
		case pIndex:
		case pIsZoomed:
		case pHasCloseBox:
		case pHasTitleBar:
		case pIsFloating:
		case pIsModal:
		case pIsResizable:
		case pIsZoomable:
		case pVisible:
			exists = true;
			break;

		default:
			exists = LModelObject::AEPropertyExists(inProperty);
			break;
	}

	return exists;
}


// ---------------------------------------------------------------------------
//	¥ GetModelName
// ---------------------------------------------------------------------------
//	Return the name of a Window as an AppleEvent model object

StringPtr
LWindow::GetModelName(
	Str255	outModelName) const
{
	return GetDescriptor(outModelName);
}


// ---------------------------------------------------------------------------
//	¥ GetAEWindowAttribute
// ---------------------------------------------------------------------------

void
LWindow::GetAEWindowAttribute(
	EWindAttr	inAttribute,
	AEDesc&		outPropertyDesc) const
{
	Boolean	attrIsSet = HasAttribute(inAttribute);
	OSErr	err = ::AECreateDesc(typeBoolean, (Ptr) &attrIsSet,
								sizeof(Boolean), &outPropertyDesc);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent
// ---------------------------------------------------------------------------

void
LWindow::HandleAppleEvent(
	const AppleEvent	&inAppleEvent,
	AppleEvent			&outAEReply,
	AEDesc				&outResult,
	long				inAENumber)
{
	switch (inAENumber) {

		case ae_Close:
			DoClose();
			break;

		case ae_Select:
			UDesktop::SelectDeskWindow(this);
			break;

		default:
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,
												outResult, inAENumber);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ DoSetPosition
// ---------------------------------------------------------------------------
//	Change the location of a Window
//
//	The top left corner of the Window's port rectangle is placed at
//	inPosition, which is in global coordinates

void
LWindow::DoSetPosition(
	Point	inPosition)					// Top left in global coords
{
	::MacMoveWindow(mMacWindowP, inPosition.h, inPosition.v, false);

	AdjustUserBounds();
}


// ---------------------------------------------------------------------------
//	¥ AdjustUserBounds
// ---------------------------------------------------------------------------
//	Adjust the UserBounds for zooming after moving a Window

void
LWindow::AdjustUserBounds()
{
	Rect	stdBounds;

	if (CalcStandardBounds(stdBounds)) {

			// Window is at the standard state. Don't change the
			// size of the user bounds, but offset it so that it
			// coincides with the current location of the Window.

		Point	offset = Point_00;
		PortToGlobalPoint(offset);
		offset.h -= mUserBounds.left;
		offset.v -= mUserBounds.top;
		::MacOffsetRect(&mUserBounds, offset.h, offset.v);

	} else {

			// Window is not at the standard state. The current
			// size and location becomes new user bounds.

		CalcPortFrameRect(mUserBounds);
		PortToGlobalPoint(topLeft(mUserBounds));
		PortToGlobalPoint(botRight(mUserBounds));
	}

	mMoveOnlyUserZoom = false;
}


// ---------------------------------------------------------------------------
//	¥ MoveWindowTo
// ---------------------------------------------------------------------------
//	Move the top left corner of the Window's port rect to the specified
//	location in Global coords

void
LWindow::MoveWindowTo(
	SInt16	inHoriz,
	SInt16	inVert)
{
	Point	position;
	position.h = inHoriz;
	position.v = inVert;
	DoSetPosition(position);
}


// ---------------------------------------------------------------------------
//	¥ MoveWindowBy
// ---------------------------------------------------------------------------
//	Move Window's location on screen by the specified number of pixels
//
//	Positive horizontal deltas move Window to the left; negative to the right
//	Positive vertical deltas move Window down; negative up

void
LWindow::MoveWindowBy(
	SInt16	inHorizDelta,
	SInt16	inVertDelta)
{
	Rect	theBounds;
	GetGlobalBounds(theBounds);

	Point	position;
	position.h = (SInt16) (theBounds.left + inHorizDelta);
	position.v = (SInt16) (theBounds.top + inVertDelta);
	DoSetPosition(position);
}


// ---------------------------------------------------------------------------
//	¥ CalcStandardBounds
// ---------------------------------------------------------------------------
//	Calculate the bounds of the Window at standard state and return whether
//	it is at standard state. The standard state depends on the screen
//	containing the largest area of the Window and the current standard size.
//
//	outStdBounds: Port rectangle of Window at standard size, global coords

Boolean
LWindow::CalcStandardBounds(
	Rect&	outStdBounds) const
{
									// Find GDevice containing largest
									//   portion of Window
	GDHandle	dominantDevice = UWindows::FindDominantDevice(
							UWindows::GetWindowStructureRect(mMacWindowP));

	if (dominantDevice == nil) {	// Window is offscreen, so use the
									//   main scren
		dominantDevice = ::GetMainDevice();
	}
	Rect		screenRect = (**dominantDevice).gdRect;
	
									// Must compensate for MenuBar on the
									//   main screen
	if (dominantDevice == ::GetMainDevice()) {
		screenRect.top += ::GetMBarHeight();
	}
	
	#if TARGET_API_MAC_CARBON		// CarbonLib 1.3 or later has a call
									//   to get the available screen rect,
									//   which accounts for the menu bar
									//   and the dock
	
		if (CFM_AddressIsResolved_(GetAvailableWindowPositioningBounds)) {
		
			::GetAvailableWindowPositioningBounds(dominantDevice, &screenRect);
		}
		
	#endif

	CalcStandardBoundsForScreen(screenRect, outStdBounds);

	Rect		contentRect = UWindows::GetWindowContentRect(mMacWindowP);
	return ::MacEqualRect(&outStdBounds, &contentRect);
}


// ---------------------------------------------------------------------------
//	¥ CalcStandardBoundsForScreen
// ---------------------------------------------------------------------------
//	Calculate the bounds of the Window if it was at a Standard (zoomed out)
//	state on a Screen with the specified bounds.
//
//	inScreenBounds: Bounding box of screen in global coordinates
//	outStdBounds: Port rectangle of Window at standard size, global coords

void
LWindow::CalcStandardBoundsForScreen(
	const Rect&	inScreenBounds,
	Rect&		outStdBounds) const
{
									// Structure and Content regions are
									//   in global coordinates
	Rect	strucRect = UWindows::GetWindowStructureRect(mMacWindowP);
	Rect	contRect  = UWindows::GetWindowContentRect(mMacWindowP);

									// Structure can be (and usually is)
									//   larger than Content
	Rect	border;
	border.left   = (SInt16) (contRect.left - strucRect.left);
	border.right  = (SInt16) (strucRect.right - contRect.right);
	border.top    = (SInt16) (contRect.top - strucRect.top);
	border.bottom = (SInt16) (strucRect.bottom - contRect.bottom);

						 			// Don't zoom too close to edge of screen
	SInt16	screenWidth  = (SInt16) (inScreenBounds.right - inScreenBounds.left - 4);
	SInt16	screenHeight = (SInt16) (inScreenBounds.bottom - inScreenBounds.top - 4);

									// Standard dimensions are the minimum
									//   of mStandardSize and the size of
									//   the screen
	SInt16	stdWidth = mStandardSize.width;
	if (stdWidth > screenWidth - (border.left + border.right)) {
		stdWidth = (SInt16) (screenWidth - (border.left + border.right));
	}

	SInt16	stdHeight = mStandardSize.height;
	if (stdHeight > screenHeight - (border.top + border.bottom)) {
		stdHeight = (SInt16) (screenHeight - (border.top + border.bottom));
	}

									// Standard position is the point closest
									//   to the current position at which
									//   the Window will be all on screen

									// Move window horizontally so that left
									//   or right edge of Struction region is
									//   2 pixels from the edge of the screen
	SInt16	stdLeft = contRect.left;
	if (stdLeft < inScreenBounds.left + border.left + 2) {
		stdLeft = (SInt16) (inScreenBounds.left + border.left + 2);
	} else if (stdLeft > inScreenBounds.right - stdWidth - border.right - 2) {
		stdLeft = (SInt16) (inScreenBounds.right - stdWidth - border.right - 2);
	}

									// Move window vertically so that top
									//   or bottom edge of Struction region is
									//   2 pixels from the edge of the screen
	SInt16	stdTop = contRect.top;
	if (stdTop < inScreenBounds.top + border.top + 2) {
		stdTop = (SInt16) (inScreenBounds.top + border.top + 2);
	} else if (stdTop > inScreenBounds.bottom - stdHeight - border.bottom - 2) {
		stdTop = (SInt16) (inScreenBounds.bottom - stdHeight - border.bottom - 2);
	}

	outStdBounds.left   = stdLeft;
	outStdBounds.right  = (SInt16) (stdLeft + stdWidth);
	outStdBounds.top    = stdTop;
	outStdBounds.bottom = (SInt16) (stdTop + stdHeight);
}


// ---------------------------------------------------------------------------
//	¥ DoSetZoom
// ---------------------------------------------------------------------------
//	Zoom window to either the Standard or User state

void
LWindow::DoSetZoom(
	Boolean	inZoomToStdState)
{
	if (!HasAttribute(windAttr_Zoomable)) {
		ThrowOSErr_(errAENotModifiable);
	}

	Rect	currBounds = UWindows::GetWindowContentRect(mMacWindowP);
	Rect	zoomBounds;

	if (inZoomToStdState) {			// Zoom to Standard state
		if (CalcStandardBounds(zoomBounds)) {
			return;					// Already at Standard state
		}

	} else {						// Zoom to User state
		zoomBounds = mUserBounds;

		if (mMoveOnlyUserZoom) {	// Special case for zooming a Window
									//   that is at standard size, but
									//   is partially offscreen
			zoomBounds.right = (SInt16) (zoomBounds.left +
								(currBounds.right - currBounds.left));
			zoomBounds.bottom = (SInt16) (zoomBounds.top +
								(currBounds.bottom - currBounds.top));
		}
	}

	SInt16	zoomWidth  = (SInt16) (zoomBounds.right - zoomBounds.left);
	SInt16	zoomHeight = (SInt16) (zoomBounds.bottom - zoomBounds.top);
	mMoveOnlyUserZoom = false;

		// To avoid unnecessary redraws, we check to see if the
		// current and zoom states are either the same size
		// or at the same location

	if ( ((currBounds.right - currBounds.left) == zoomWidth) &&
		 ((currBounds.bottom - currBounds.top) == zoomHeight) ) {
									// Same size, just move
		::MacMoveWindow(mMacWindowP, zoomBounds.left, zoomBounds.top, false);
		mMoveOnlyUserZoom = true;

	} else if (::EqualPt(topLeft(currBounds), topLeft(zoomBounds))) {
									// Same location, just resize
		::SizeWindow(mMacWindowP, zoomWidth, zoomHeight, false);
		ResizeFrameTo(zoomWidth, zoomHeight, true);

	} else {						// Different size and location
									// Zoom appropriately
		FocusDraw();
		ApplyForeAndBackColors();

		Rect	windowBounds;
		::EraseRect( ::GetWindowPortBounds(mMacWindowP, &windowBounds));

		if (inZoomToStdState) {
			SetWindowStandardState(mMacWindowP, &zoomBounds);
			::ZoomWindow(mMacWindowP, inZoomOut, false);
		} else {
			SetWindowStandardState(mMacWindowP, &currBounds);
			SetWindowUserState(mMacWindowP, &zoomBounds);
			::ZoomWindow(mMacWindowP, inZoomIn, false);
		}

		ResizeFrameTo(zoomWidth, zoomHeight, false);
	}
}


// ---------------------------------------------------------------------------
//	¥ DoSetBounds
// ---------------------------------------------------------------------------
//	Change size and location of a Window
//
//	inBounds, in global coords, specifies the new size and location of
//	the Window's port rectangle

void
LWindow::DoSetBounds(
	const Rect&		inBounds)			// Bounds in global coords
{
									// Set size and location of Toolbox
									//   WindowRecord
	::SizeWindow(mMacWindowP, (SInt16) (inBounds.right - inBounds.left),
							  (SInt16) (inBounds.bottom - inBounds.top), false);
	::MacMoveWindow(mMacWindowP, inBounds.left, inBounds.top, false);

									// Set our Frame
	ResizeFrameTo((SInt16) (inBounds.right - inBounds.left),
				  (SInt16) (inBounds.bottom - inBounds.top), true);

	SDimension16	frameSize;		// For Windows, Image is always the
	GetFrameSize(frameSize);		//   same size as its Frame
	ResizeImageTo(frameSize.width, frameSize.height, false);

	AdjustUserBounds();				// Changing Bounds establishes a
									//   new User state for zooming
}


// ---------------------------------------------------------------------------
//	¥ AdaptToBoundsChange
// ---------------------------------------------------------------------------
//	Adapt Window's internal state to a change in the size and/or location
//	of the associated Toolbox window
//
//	This function should be called from a Carbon Event handler for
//	kEventWindowBoundsChanged and other places where you change the
//	Toolbox window size/location without using the functions of this class.
//
//	The inWhatChanged parameter is the same as the "attributes" parameter
//	for the kEventWindowBoundsChanged Carbon Event.

void
LWindow::AdaptToBoundsChange(
	UInt32	inWhatChanged)
{
	Rect	bounds;
	::GetPortBounds( GetMacPort(), &bounds );
		
		// We assume the change occurred externally if the "user drag"
		// or "user resize" flag is set for inWhatChanged.

	if (inWhatChanged & kWindowBoundsChangeUserResize) {
										// Window size changed

										// Set our Frame
		ResizeFrameTo((SInt16) (bounds.right - bounds.left),
					  (SInt16) (bounds.bottom - bounds.top), true);

		SDimension16	frameSize;		// For Windows, Image is always the
		GetFrameSize(frameSize);		//   same size as its Frame
		ResizeImageTo(frameSize.width, frameSize.height, false);
	}
	
	if ( inWhatChanged &
		 (kWindowBoundsChangeUserDrag | kWindowBoundsChangeUserResize) ) {
		 								// User changed the bounds
		 
		 								// Record AppleEvent
		SendAESetBounds(&bounds, ExecuteAE_No);
		
		 								// Change in window location or
										//   size affects the user bounds
		AdjustUserBounds();				//   for zooming
	}
}


// ---------------------------------------------------------------------------
//	¥ ResizeWindowTo
// ---------------------------------------------------------------------------
//	Change the size of a Window to the specified width and height (in pixels)

void
LWindow::ResizeWindowTo(
	SInt16	inWidth,
	SInt16	inHeight)
{
	Rect	theBounds;
	GetGlobalBounds(theBounds);

	theBounds.right  = (SInt16) (theBounds.left + inWidth);
	theBounds.bottom = (SInt16) (theBounds.top + inHeight);
	DoSetBounds(theBounds);
}


// ---------------------------------------------------------------------------
//	¥ ResizeWindowBy
// ---------------------------------------------------------------------------
//	Change the size of a Window by the specified number of pixels

void
LWindow::ResizeWindowBy(
	SInt16	inWidthDelta,
	SInt16	inHeightDelta)
{
	Rect	theBounds;
	GetGlobalBounds(theBounds);

	theBounds.right += inWidthDelta;
	theBounds.bottom += inHeightDelta;
	DoSetBounds(theBounds);
}


// ---------------------------------------------------------------------------
//	¥ FindWindowByID
// ---------------------------------------------------------------------------
//	Return the Window object with the specified Pane ID number
//
//	Loops through all Windows from front to back searching for the first one
//	with the specified Pane ID. Note that it's common to use the same
//	PPob (and WIND) resource to create multiple windows, all of which have
//	the same Pane ID. This function returns the frontmost match, whether it
//	is visible or not.
//
//	Returns nil if no match found.

LWindow*
LWindow::FindWindowByID(
	PaneIDT		inWindowID)
{
	LWindow		*foundWindow = nil;
	WindowPtr	theWindowP = ::GetWindowList();

	while (theWindowP) {
		LWindow *window = FetchWindowObject(theWindowP);
		if ((window != nil) && (window->GetPaneID() == inWindowID)) {
			foundWindow = window;
			break;
		}
		theWindowP = GetNextWindow(theWindowP);
	}

	return foundWindow;
}

#pragma mark -
// ===========================================================================
// ¥ LWindowThemeAttachment
// ===========================================================================

LWindowThemeAttachment::LWindowThemeAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	*inStream >> mActiveBackgroundBrush;
	*inStream >> mInactiveBackgroundBrush;
	*inStream >> mActiveTextColor;
	*inStream >> mInactiveTextColor;

	mMessage = msg_FinishCreate;	// We only handle this message
}


Boolean
LWindowThemeAttachment::Execute(
	MessageT	inMessage,
	void*		ioParam)
{
	Boolean		executeHost = true;

	if (inMessage == msg_FinishCreate) {
		LWindow*	window = reinterpret_cast<LWindow*>(ioParam);

		window->SetThemeData(mActiveBackgroundBrush, mInactiveBackgroundBrush,
							 mActiveTextColor, mInactiveTextColor);
		executeHost = mExecuteHost;
		delete this;
	}

	return executeHost;
}


PP_End_Namespace_PowerPlant
