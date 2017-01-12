// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UWMgr20Desktop.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Window manipulation routines using Window Manager 2.0, which supports
//	modal, floating, and document window layers
//
//	Use Note:
//		Window Manager 2.0 is present on Mac OS 8.5 or later. Do not use
//		this implementation for programs which may run on earlier systems.
//
//		There is one exception. If you are building a Carbon program which
//		supports CarbonLib 1.0.x running on Mac OS 8.1, you can use this
//		file if you put
//
//			#define PP_Implement_RepositionWindow 1
//
//		in your prefix file. This enables a PP implementation of the
//		Toolbox call RepositionWindow(), which is missing from
//		CarbonLib 1.0.4 and earlier. See the comments in PP_Macros.h
//		which describes this flag for more details.
//
//	Implementation:
//		For the most part, these wrapper funtions just call the Window
//		Manager 2.0 traps. There is little or no overhead when using
//		this class as compared with using direct Window Manager calls.
//
//	Usage Note:
//		When using the PowerPlant LWindow class, you must include some
//		file in your project which implements the class defined in
//		UDesktop.h. Do not include more than one implementation or you
//		will get linker errors.
//
//		You must call ::InitFloatingWindows() instead of ::InitWindows()
//		when initializing the Toolbox.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDesktop.h>
#include <LWindow.h>
#include <PP_Resources.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UTBAccessors.h>
#include <UWindows.h>

#if (TARGET_OS_MAC && TARGET_CPU_68K)
									// Window Manager 2.0 does not exist
									//   on 68K Macs. Use UDesktop.cp or
									//   UFloatingDesktop.cp instead.
	#error UWMgr20Desktop is not supported for 68K Mac Targets
#endif

#if PP_Target_Carbon
	#pragma ANSI_strict off
	#warning UWMgr20Desktop is not recommended for Carbon Targets. Use UCarbonDesktop instead.
	#pragma ANSI_strict reset
#endif


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Prototype for custom window definition

static pascal long PlainBoxWindowDef(
	short		varCode,
	WindowRef	window,
	short		message,
	long		param);
	

// ---------------------------------------------------------------------------
//	¥ NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from a WIND resource for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*	inWindow,
	ResIDT		inWINDid,
	WindowPtr	inBehind)
{
										// Map from PP window layer to
										//   Toolbox WindowClass
	WindowClass		windClass = kDocumentWindowClass;
	if (inWindow->HasAttribute(windAttr_Modal)) {
		windClass = kMovableModalWindowClass;
	} else if (inWindow->HasAttribute(windAttr_Floating)) {
		windClass = kFloatingWindowClass;
	}

		// Set Toolbox WindowAttributes based on PP LWindow atrributes

	WindowAttributes	windAttr = kWindowNoAttributes;

	#if PP_Uses_Carbon_Events
		windAttr += kWindowStandardHandlerAttribute;
	#endif

										// PP has no option for this, so
										//   make all non-modal windows
										//   collapsible
	if (windClass >= kFloatingWindowClass) {
		windAttr += kWindowCollapseBoxAttribute;
	}

	if (inWindow->HasAttribute(windAttr_Resizable)) {
		windAttr += kWindowResizableAttribute;
	}
	
			// CreateNewWindow() does not support modal windows that
			// have a close box or a zoom box

	if (inWindow->HasAttribute(windAttr_CloseBox)) {
	
		if (windClass == kMovableModalWindowClass) {
			SignalStringLiteral_("Modal window with close box not supported");
		} else {
			windAttr += kWindowCloseBoxAttribute;
		}
	}

	if (inWindow->HasAttribute(windAttr_Zoomable)) {
	
		if (windClass == kMovableModalWindowClass) {
			SignalStringLiteral_("Modal window with zoom box not supported");
		} else {
			windAttr += kWindowFullZoomAttribute;
		}
	}

		// We need to get the WIND resource to find the:
		//		- Side Title option for Floaters
		//		- Window Bounds
		//		- Window Title
		//		- Window Positioning Constant

	Handle	theWIND = ::GetResource(ResType_MacWindow, inWINDid);

	ThrowIfResFail_(theWIND);

	StHandleLocker	lockWIND(theWIND);

	SWINDResourceP	theWINDInfo = (SWINDResourceP) *theWIND;

										// Side Title option for Floaters
	if ( (windClass == kFloatingWindowClass)  &&
		 (theWINDInfo->procID & kSideFloaterVariantCode) ) {

		windAttr += kWindowSideTitlebarAttribute;
	}

										// Now we can create Toolbox window
	WindowPtr	windowP;
	
	#if TARGET_API_MAC_CARBON
										// Carbon does not support plainDBox
										//   so we use a custom implementation
		if (theWINDInfo->procID == plainDBox) {
			WindowDefSpec	wdefSpec;
			wdefSpec.defType = kWindowDefProcPtr;
			wdefSpec.u.defProc = NewWindowDefUPP(PlainBoxWindowDef);
			
			ThrowIfOSStatus_( ::CreateCustomWindow(&wdefSpec, windClass,
										windAttr, &theWINDInfo->bounds, &windowP) );

		} else
	#endif
	
		ThrowIfOSStatus_( ::CreateNewWindow(windClass, windAttr,
										&theWINDInfo->bounds, &windowP) );

										// Set Window title using title from
										//   WIND resource
	::SetWTitle(windowP, theWINDInfo->title);

		// On Mac OS 8.6, there seems to be a bug with SendBehind.
		// If you call InitWindows, it works. If you call
		// InitFloatingWindows, it does nothing.

	if (inBehind != window_InFront) {
		::SendBehind(windowP, inBehind);
	}

		// The Window Positioning Constant comes after the title. It is
		// word aligned.

	unsigned char*	posP = theWINDInfo->title + theWINDInfo->title[0] + 1;
	if ((theWINDInfo->title[0] & 0x0001) == 0) {
		posP += 1;						// Align to 2-byte boundary
	}

	UInt16	posConstant = *(UInt16*) posP;

		// The System 7 Window Positioning Constants and the new Window
		// Positioning Methods have different values. So, we must map to
		// the new values.

	WindowPositionMethod	posMethod = 0;
	
		// Parent window is always the top non-floaing window
		
	WindowPtr				parentWindow = ::FrontNonFloatingWindow();

	switch (posConstant) {

		case kWindowCenterMainScreen:
			posMethod = kWindowCenterOnMainScreen;
			break;

		case kWindowAlertPositionMainScreen:
			posMethod = kWindowAlertPositionOnMainScreen;
			break;

		case kWindowStaggerMainScreen:
			posMethod = kWindowCascadeOnMainScreen;
			break;
			
				// For positioning on a parent window, we
				// change to using the main screen if the
				// parent window is nil

		case kWindowCenterParentWindow:
			posMethod = kWindowCenterOnParentWindow;
			if (parentWindow == nil) {
				posMethod = kWindowCenterOnMainScreen;
			}
			break;

		case kWindowAlertPositionParentWindow:
			posMethod = kWindowAlertPositionOnParentWindow;
			if (parentWindow == nil) {
				posMethod = kWindowAlertPositionOnMainScreen;
			}
			break;

		case kWindowStaggerParentWindow:
			posMethod = kWindowCascadeOnParentWindow;
			if (parentWindow == nil) {
				posMethod = kWindowCascadeOnMainScreen;
			}
			break;
			
				// For positioning on a parent window screen, we
				// change to using the main screen if the parent
				// window is nil. System 7 Window Positioning
				// behaves that way, but RepositionWindow does
				// no positioning if the parent window is nil.

		case kWindowCenterParentWindowScreen:
			posMethod = kWindowCenterOnParentWindowScreen;
			if (parentWindow == nil) {
				posMethod = kWindowCenterOnMainScreen;
			}
			break;

		case kWindowAlertPositionParentWindowScreen:
			posMethod = kWindowAlertPositionOnParentWindowScreen;
			if (parentWindow == nil) {
				posMethod = kWindowAlertPositionOnMainScreen;
			}
			break;

		case kWindowStaggerParentWindowScreen:
			posMethod = kWindowCascadeOnParentWindowScreen;
			if (parentWindow == nil) {
				posMethod = kWindowCascadeOnMainScreen;
			}
			break;

	}


	if (posMethod != 0) {					// Set the window position
	
		#if PP_Implement_RepositionWindow
		
				// Use PP implementation if Toolbox routine doesn't exist.
				// Toolbox RepositionWindow is not implemented in
				// CarbonLib 1.0.4 and earlier when running on Mac OS 8.1
		
			if (RepositionWindow != (void*) kUnresolvedCFragSymbolAddress) {
				::RepositionWindow(windowP, parentWindow, posMethod);
			} else {
			
				UWindows::RepositionWindow(windowP, parentWindow, posMethod);
			}
		
		#else
		
			::RepositionWindow(windowP, parentWindow, posMethod);
		
		#endif
	}

			// If there is a 'wctb' resource with the same ID as
			// the 'WIND', set the window's background color to
			// the content color from the color table.

	StResource	wctbResource(ResType_WindowColorTable, inWINDid,
								Throw_No);

	RGBColor	contentColor;
	if (UWindows::GetColorTableEntry( (CTabHandle) wctbResource.Get(),
									  wContentColor,
									  contentColor ) ) {

		StGrafPortSaver	savePort;
		::SetPortWindowPort(windowP);
		::RGBBackColor(&contentColor);
	}

	return windowP;
}


// ---------------------------------------------------------------------------
//	¥ NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from input parameters for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*		inWindow,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			/* inProcID */,
	Boolean			inHasGoAway,
	WindowPtr		inBehind)
{
										// Map from PP window layer to
										//   Toolbox WindowClass
	WindowClass		windClass = kDocumentWindowClass;
	if (inWindow->HasAttribute(windAttr_Modal)) {
		windClass = kMovableModalWindowClass;
	} else if (inWindow->HasAttribute(windAttr_Floating)) {
		windClass = kFloatingWindowClass;
	}

		// Set Toolbox WindowAttributes based on PP LWindow atrributes

	WindowAttributes	windAttr = kWindowNoAttributes;

	#if PP_Uses_Carbon_Events
		windAttr += kWindowStandardHandlerAttribute;
	#endif

										// PP has no option for this, so
										//   make all non-modal windows
										//   collapsible
	if (windClass >= kFloatingWindowClass) {
		windAttr += kWindowCollapseBoxAttribute;
	}

	if (inHasGoAway) {
		windAttr += kWindowCloseBoxAttribute;
	}

	if (inWindow->HasAttribute(windAttr_Zoomable)) {
		windAttr += kWindowFullZoomAttribute;
	}

	if (inWindow->HasAttribute(windAttr_Resizable)) {
		windAttr += kWindowResizableAttribute;
	}

										// Now we can create Toolbox window
	WindowPtr	windowP;
	ThrowIfOSStatus_( ::CreateNewWindow(windClass, windAttr,
										&inGlobalBounds, &windowP) );

	::SetWTitle(windowP, inTitle);

		// On Mac OS 8.6, there seems to be a bug with SendBehind.
		// If you call InitWindows, it works. If you call
		// InitFloatingWindows, it does nothing.

	if (inBehind != window_InFront) {
		::SendBehind(windowP, inBehind);
	}

	return windowP;
}


// ---------------------------------------------------------------------------
//	¥ WindowIsSelected										 [static] [public]
// ---------------------------------------------------------------------------
//	Return whether a Window is selected, i.e., it is at the front
//	of its layer

bool
UDesktop::WindowIsSelected(
	LWindow*	inWindow)
{
	bool		isSelected;
									// Separate check for each layer

	if (inWindow->HasAttribute(windAttr_Modal)) {
									// ¥ Modal
		isSelected = (inWindow == FetchTopModal());

	} else if (inWindow->HasAttribute(windAttr_Floating)) {
									// ¥ Floater
		isSelected = (inWindow == FetchTopFloater());

	} else {						// ¥ Regular
		isSelected = (inWindow == FetchTopRegular());
	}

	return isSelected;
}


// ---------------------------------------------------------------------------
//	¥ SelectDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Select a Window by bringing to the front

void
UDesktop::SelectDeskWindow(
	LWindow*	inWindow)
{
	::SelectWindow(inWindow->GetMacWindow());
}


// ---------------------------------------------------------------------------
//	¥ ShowDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Make a Window visible

void
UDesktop::ShowDeskWindow(
	LWindow*	inWindow)
{
	::MacShowWindow(inWindow->GetMacWindow());
}


// ---------------------------------------------------------------------------
//	¥ HideDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Make a Window invisible

void
UDesktop::HideDeskWindow(
	LWindow*	inWindow)
{
	::HideWindow(inWindow->GetMacWindow());
}


// ---------------------------------------------------------------------------
//	¥ DragDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Move the position of a Window

void
UDesktop::DragDeskWindow(
	LWindow*			inWindow,
	const EventRecord&	inMacEvent,
	const Rect&			inDragRect)
{
										// Drag without command key
										// Select before dragging
	if (!(inMacEvent.modifiers & cmdKey)) {
		inWindow->Select();
	}

	::DragWindow(inWindow->GetMacWindow(), inMacEvent.where,
					&inDragRect);
}


// ---------------------------------------------------------------------------
//	¥ Suspend												 [static] [public]
// ---------------------------------------------------------------------------
//	Call this function when the application receives a suspend event.

void
UDesktop::Suspend()
{
									// Tell each PP Window to Suspend
	LWindow*	theWindow;
	WindowPtr	macWindowP = ::GetWindowList();

	while (macWindowP != nil) {
		theWindow = LWindow::FetchWindowObject(macWindowP);
		if (theWindow != nil) {		// Skip over non-PP windows
			theWindow->Suspend();
		}
		macWindowP = ::MacGetNextWindow(macWindowP);
	}
}


// ---------------------------------------------------------------------------
//	¥ Resume												 [static] [public]
// ---------------------------------------------------------------------------
//	Call this function when the application receives a resume event.

void
UDesktop::Resume()
{
									// Tell each PP Window to Resume
	LWindow*	theWindow;
	WindowPtr	macWindowP = ::GetWindowList();

	while (macWindowP != nil) {
		theWindow = LWindow::FetchWindowObject(macWindowP);
		if (theWindow != nil) {		// Skip over non-PP windows
			theWindow->Resume();
		}
		macWindowP = ::MacGetNextWindow(macWindowP);
	}
	
									// Activate the top Modal or Regular
									//   Window
	LWindow*	windowToActivate = FetchTopModal();
	if (windowToActivate == nil) {
		windowToActivate = FetchTopRegular();
	}
	
	if (windowToActivate != nil) {
		windowToActivate->Activate();
	}
}


// ---------------------------------------------------------------------------
//	¥ Deactivate											 [static] [public]
// ---------------------------------------------------------------------------
//	Deactivate the Desktop. Call this function before displaying a
//	modal window.

void
UDesktop::Deactivate()
{
	UCursor::Normalize();			// Show arrow cursor
}


// ---------------------------------------------------------------------------
//	¥ Activate												 [static] [public]
// ---------------------------------------------------------------------------
//	Activate the Desktop. Call this function after dismissing a
//	modal window.

void
UDesktop::Activate()
{
}


// ---------------------------------------------------------------------------
//	¥ FetchTopRegular										 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the topmost visible Regular Window

LWindow*
UDesktop::FetchTopRegular()
{
		// Start with FrontWindow (which always returns a visible
		// window) and search thru window list until finding the
		// first visible regular window.

	WindowPtr	macWindowP = ::FrontWindow();
	LWindow*	theWindow;
	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if ( theWindow->HasAttribute(windAttr_Regular)  &&
			 IsWindowVisible(macWindowP) ) {
			break;
		}
		macWindowP = GetNextWindow(macWindowP);
	}

	return theWindow;
}


// ---------------------------------------------------------------------------
//	¥ FetchTopFloater										 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the topmost visible Floating Window

LWindow*
UDesktop::FetchTopFloater()
{
		// Start with FrontWindow (which always returns a visible
		// window) and search thru window list until finding the
		// first visible floating window.

	WindowPtr	macWindowP = ::FrontWindow();
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if ( theWindow->HasAttribute(windAttr_Floating)  &&
			 IsWindowVisible(macWindowP) ) {
			break;
		}
		macWindowP = ::GetNextWindow(macWindowP);
	}

	return theWindow;
}


// ---------------------------------------------------------------------------
//	¥ FetchBottomFloater									 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the bottommost floating Window (visible or not)

LWindow*
UDesktop::FetchBottomFloater()
{
		// Ugh. We have to use the Low Memory global WindowList to
		// get the frontmost window (since FrontWindow() only returns
		// the first *visible* window). There is no way to search from
		// back to front, so we have to search the entire window list
		// from front to back and remember the last floater that
		// we find.

	WindowPtr	macWindowP = ::GetWindowList();
	LWindow*	bottomFloater = nil;
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if (theWindow->HasAttribute(windAttr_Floating)) {
			bottomFloater = theWindow;
		}
		macWindowP = ::GetNextWindow(macWindowP);
	}

	return bottomFloater;
}


// ---------------------------------------------------------------------------
//	¥ FetchTopModal											 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the topmost visible Modal Window

LWindow*
UDesktop::FetchTopModal()
{
		// Modal Windows must be in front of all others. Therefore, we
		// only have to check the FrontWindow. If it's Modal, then it's
		// the top Modal. If it's not Modal, then there is no top Modal.

	LWindow*	topModal = nil;
	LWindow*	theWindow = LWindow::FetchWindowObject(::FrontWindow());

	if ( (theWindow != nil) && theWindow->HasAttribute(windAttr_Modal) ) {
		topModal = theWindow;
	}

	return topModal;
}


// ---------------------------------------------------------------------------
//	¥ FetchBottomModal										 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the bottommost modal Window (visible or not)

LWindow*
UDesktop::FetchBottomModal()
{
		// Ugh. We have to use the Low Memory global WindowList to
		// get the frontmost window (since FrontWindow() only returns
		// the first *visible* window). There is no way to search from
		// back to front, so we have to search the entire window list
		// from front to back and remember the last modal that
		// we find.

	WindowPtr	macWindowP = ::GetWindowList();
	LWindow*	bottomModal = nil;
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if (theWindow->HasAttribute(windAttr_Modal)) {
			bottomModal = theWindow;
		}
		macWindowP = GetNextWindow(macWindowP);
	}

	return bottomModal;
}


// ---------------------------------------------------------------------------
//	¥ FrontWindowIsModal									 [static] [public]
// ---------------------------------------------------------------------------
//	Return whether the front window is modal

bool
UDesktop::FrontWindowIsModal()
{
	bool		modalInFront = false;
	WindowPtr	macWindowP = ::FrontWindow();

	if (macWindowP != nil) {			// There is a front window

		WindowClass		windClass;
		OSStatus	status = ::GetWindowClass(macWindowP, &windClass);

		if (status == noErr) {
			modalInFront = (windClass <= kMovableModalWindowClass);
		}
	}

	return modalInFront;
}


// ---------------------------------------------------------------------------
//	¥ NormalizeWindowOrder									 [static] [public]
// ---------------------------------------------------------------------------
//	Put windows in the proper order

void
UDesktop::NormalizeWindowOrder()
{										// Do nothing. WMgr 2.0 maintains
}										//   the proper window order


// ---------------------------------------------------------------------------
//	¥ GetBehindWindow									  [static] [protected]
// ---------------------------------------------------------------------------
//	Verify and adjust the inBehind window so that it places the Window at
//	the proper location within its layer

WindowPtr
UDesktop::GetBehindWindow(
	LWindow*	/* inWindow */,
	WindowPtr	inBehind)
{										// Do nothing. WMgr 2.0 does the
	return inBehind;					//   right thing
}


#pragma mark -
// ===========================================================================
//	StDesktopDeactivator
// ===========================================================================
//	Stack-based class for Deactivating and later Activating the Desktop

StDesktopDeactivator::StDesktopDeactivator()
{
	UDesktop::Deactivate();
}


StDesktopDeactivator::~StDesktopDeactivator()
{
	UDesktop::Activate();
}

#pragma mark -
// ===========================================================================
//	PlainBoxWindowDef
// ===========================================================================
//	Custom definition proc for a window with a one-pixel rectangular frame
//
//	The Classic Toolbox implements this window kind as the plainDBox proc ID.
//	However, Carbon does not support this, so we provide our own
//	implementation of this useful window kind.

static pascal long
PlainBoxWindowDef(
	short		/* inVarCode */,
	WindowRef	inWindow,
	short		inMessage,
	long		inParam)
{
	long	result = 0;

    switch (inMessage) {

		case kWindowMsgGetFeatures: {
			*(OptionBits*) inParam = kWindowCanGetWindowRegion
									| kWindowDefSupportsColorGrafPort;
			result = 1;
		}

		case kWindowMsgGetRegion: {
			GetWindowRegionRec* rgnRec = (GetWindowRegionRec*) inParam;

			if ( (rgnRec->regionCode == kWindowContentRgn)  ||
                 (rgnRec->regionCode == kWindowStructureRgn) ) {

				Rect portBounds;
				::GetWindowBounds(inWindow, kWindowGlobalPortRgn, &portBounds);
				::RectRgn(rgnRec->winRgn, &portBounds);

				if (rgnRec->regionCode == kWindowStructureRgn) {
					::InsetRgn(rgnRec->winRgn, -1, -1);
				}
			}
			
			break;
		}

		case kWindowMsgDraw: {
			Rect	portBounds;
			::GetWindowBounds(inWindow, kWindowGlobalPortRgn, &portBounds);
			::InsetRect(&portBounds, -1, -1);
			::FrameRect(&portBounds);
			break;
        }
        
        case kWindowMsgHitTest:
			result = wInContent;
			break;

//		default:
//			break;
    }
    
    return result;
}


PP_End_Namespace_PowerPlant

