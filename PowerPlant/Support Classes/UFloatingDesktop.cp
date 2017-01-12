// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UFloatingDesktop.cp			PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	This module supports three window layers: Modal, Floating, and Regular
//
//	Modal windows are always in front, and all other windows are inactive
//	when a modal window is present.
//
//	Floating windows are always active, except when a Modal is active.
//
//	Regular windows are beneath all Modal and Floating windows. The top
//	Regular window is active, except when a Modal is active. All other
//	Regular windows are inactive.
//
//	Usage Note:
//		When using the PowerPlant LWindow class, you must include some
//		file in your project which implements the class defined in
//		UDesktop.h. Do not include more than one implementation or you
//		will get linker errors.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDesktop.h>

#if PP_Target_Classic			// Only works for classic targets

#include <LWindow.h>
#include <PP_Resources.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UScreenPort.h>

#include <LowMem.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt32		Drag_Aborted = 0x80008000;


// ---------------------------------------------------------------------------
// 	Static Globals

static Boolean		sSuspended		= false;
static Boolean		sAboutToSuspend = false;


// ---------------------------------------------------------------------------
//	е NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from a WIND resource for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*	inWindow,
	ResIDT		inWINDid,
	WindowPtr	inBehind)
{
	inBehind = GetBehindWindow(inWindow, inBehind);

									// Always create windows as initially
									//   invisible
	SWINDResourceH	theWIND = (SWINDResourceH) ::GetResource(ResType_MacWindow, inWINDid);
	ThrowIfResFail_(theWIND);
	(**theWIND).visible = false;
									// Make Toolbox Window
	WindowPtr	macWindowP = ::GetNewCWindow(inWINDid, nil, inBehind);

	return macWindowP;
}


// ---------------------------------------------------------------------------
//	е NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from input parameters for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*		inWindow,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			inProcID,
	Boolean			inHasGoAway,
	WindowPtr		inBehind)
{
	inBehind = GetBehindWindow(inWindow, inBehind);

									// Make Toolbox Window
	WindowPtr	macWindowP = ::NewCWindow(nil, &inGlobalBounds, inTitle,
							false, inProcID, inBehind, inHasGoAway, 0);

	return macWindowP;
}


// ---------------------------------------------------------------------------
//	е WindowIsSelected										 [static] [public]
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
									// е Modal
		isSelected = (inWindow == FetchTopModal());

	} else if (inWindow->HasAttribute(windAttr_Floating)) {
									// е Floater
		isSelected = (inWindow == FetchTopFloater());

	} else {						// е Regular
		isSelected = (inWindow == FetchTopRegular());
	}

	return isSelected;
}


// ---------------------------------------------------------------------------
//	е SelectDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Select a Window
//
//	Selecting a Window does the following:
//		> Deactivates other windows as necessary
//		> Brings the Window to the front of its layer
//		> Activates the Window

void
UDesktop::SelectDeskWindow(
	LWindow*	inWindow)
{
	if (WindowIsSelected(inWindow)) {
		return;						// Do nothing if already selected
	}

									// If modal is active or we are suspended,
									// we won't have to activate this window
	Boolean		activateWind = !(FrontWindowIsModal() || sSuspended);

									// ее Deactivate other Windows and
									// ее Find window to put this one behind
	LWindow*	putBehindW;

	if (inWindow->HasAttribute(windAttr_Modal)) {
									// е Modal
		Deactivate();				// Deactivate all windows
		putBehindW = nil;			// Modals are always in front

		activateWind = !sSuspended;	// Activate this Window unless we
									//   we are suspended

	} else if (inWindow->HasAttribute(windAttr_Floating)) {
									// е Floater
									// No deactivates are necessary because
									//   Floaters don't affect other windows
									// Floaters go behind Modal windows
		putBehindW = FetchBottomModal();

	} else {						// е Regular
		LWindow*	topRegular = FetchTopRegular();
		if (topRegular != nil) {	// Deactivate current top Regular
			topRegular->Deactivate();
		}
									// Regulars go behind Floaters and Modals
									//   Find Window to put this one behind.
									//   It's either the bottom Floater or
									//   the bottom Modal.
		putBehindW = FetchBottomFloater();
		if (putBehindW == nil) {
									// No Floaters, so check Modals
			putBehindW = FetchBottomModal();
		}
	}

									// ее Place Window at proper position
	WindowPtr	windToSelect = inWindow->GetMacWindow();
	if (putBehindW == nil) {
		::BringToFront(windToSelect);
	} else {
		::SendBehind(windToSelect, putBehindW->GetMacWindow());
	}

	if (activateWind) {				// ее Force an Activate event rather than
									//   calling Activate directly. If
									//	 showing multiple windows at once,
									//   a later window might be in front
									//   of this one. We only want to activate
									//   the one window that will be active
									//   when we return to the event loop.
		::LMSetCurActivate(windToSelect);
	}
}


// ---------------------------------------------------------------------------
//	е ShowDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Make a Window visible

void
UDesktop::ShowDeskWindow(
	LWindow*	inWindow)
{
	WindowPtr	windToShow = inWindow->GetMacWindow();

									// ее Deactivate windows if this window
									// is at the front of its layer

									// If modal is active or we are suspended,
									// we won't have to activate this window
	Boolean		willBeActive = !(FrontWindowIsModal() || sSuspended);

	if (inWindow->HasAttribute(windAttr_Modal)) {
									// е Modal
									// Check if this will be the front
									//   modal window. Search WindowList
									//   until reaching this window or
									//   finding a visible modal window
		WindowPtr	wp = ::LMGetWindowList();
		willBeActive = !sSuspended;	// No need to activate if we are suspended
		if (willBeActive) {
			while (wp != windToShow) {
				LWindow*	theWindow = LWindow::FetchWindowObject(wp);
				if ( (theWindow != nil)  &&
					 theWindow->HasAttribute(windAttr_Modal)  &&
					 theWindow->IsVisible() ) {
					willBeActive = false;
					break;
				}
				wp = (WindowPtr) ((WindowPeek) wp)->nextWindow;
			}
		}

		if (willBeActive) {			// Deactivate Desktop if this will
			Deactivate();			//   be the front modal window
		}

	} else if (inWindow->HasAttribute(windAttr_Regular)) {
									// е Regular
									// Check if this will be the front
									//   regular window. Search WindowList
									//   until reaching this window or
									//   finding a visible regular window
		if (willBeActive) {
			WindowPtr	wp = ::LMGetWindowList();
			while (wp != windToShow) {
				LWindow*	theWindow = LWindow::FetchWindowObject(wp);
				if ( (theWindow != nil)  &&
					 theWindow->HasAttribute(windAttr_Regular)  &&
					 theWindow->IsVisible() ) {
					willBeActive = false;
					break;
				}
				wp = (WindowPtr) ((WindowPeek) wp)->nextWindow;
			}
		}

		if (willBeActive) {			// Deactivate current front Regular
			LWindow	*currFront = FetchTopRegular();
			if (currFront != nil) {
				currFront->Deactivate();
			}
		}
	}

	::ShowHide(windToShow, true);	// Make this Window visible

	if (willBeActive) {				// Activate this Window if necessary

		if (inWindow->HasAttribute(windAttr_Floating)) {
									// Immediately activate Floaters
			inWindow->Activate();

		} else {					// Force an Activate event rather than
									//   calling Activate directly. If
									//	 showing multiple windows at once,
									//   a later window might be in front
									//   of this one. We only want to activate
									//   the one window that will be active
									//   when we return to the event loop.

			::LMSetCurActivate(windToShow);
		}
	}
}


// ---------------------------------------------------------------------------
//	е HideDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Make a Window invisible

void
UDesktop::HideDeskWindow(
	LWindow*	inWindow)
{
	WindowPtr	windToHide = inWindow->GetMacWindow();

	if (sAboutToSuspend) {			// Just hide the window and exit
									//   No need to worry about activating
									//   other windows now
		::ShowHide(windToHide, false);
		return;
	}

									// If Window was at the top of its layer,
									//   we may have to activate others
	if (inWindow->HasAttribute(windAttr_Modal)) {
									// е Modal
									// Activate Desktop if this Window is
									//   the front Modal
		Boolean		wasInFront = (windToHide == FrontWindow());
		::ShowHide(windToHide, false);
		if (wasInFront  &&  !sSuspended) {
			Activate();
		}

	} else if (inWindow->HasAttribute(windAttr_Floating)) {
									// е Floater
									// No activating necessary
		::ShowHide(windToHide, false);

	} else if (inWindow->HasAttribute(windAttr_Regular)) {
									// е Regular
									// Activate next Regular if this Window
									//   was the active Regular
		LWindow*	currFront = FetchTopRegular();
		::ShowHide(windToHide, false);

		if (inWindow == currFront) {
									// Window was the front Regular
									// Activate the new front Regular
									// and put this Window behind it
			LWindow*	newFront = FetchTopRegular();
			if (newFront != nil) {
				if (!(FrontWindowIsModal() || sSuspended)) {
					newFront->Activate();
				}
				::SendBehind(windToHide, newFront->GetMacWindow());
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	е DragDeskWindow										 [static] [public]
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

	if (::WaitMouseUp()) {				// Continue if mouse hasn't been
										//   released

		GrafPtr		savePort;			// Use ScreenPort for dragging
		GetPort(&savePort);
		GrafPtr		screenPort = UScreenPort::GetScreenPort();

										// Make a copy of the structure
										//   region of the Window
		WindowPtr	windowToDrag = inWindow->GetMacWindow();
		StRegion	dragRgn(((WindowPeek) windowToDrag)->strucRgn);

		if (screenPort != nil) {
			::MacSetPort(screenPort);
			StColorPenState::Normalize();
										// Clip to gray region, punching
										//   out the area covered by windows
										//   that are above the one being
										//   dragged
			::SetClip(::GetGrayRgn());
			WindowPtr	macWindowP = ::FrontWindow();
			while (macWindowP != windowToDrag) {
				::DiffRgn(screenPort->clipRgn, ((WindowPeek) macWindowP)->strucRgn,
							screenPort->clipRgn);
				macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
			}

		} else {						// ScreenPort not available. Probably
			::GetWMgrPort(&screenPort);	//   low memory. Use WindowMgr port
			::MacSetPort(screenPort);	//   and don't worry about clipping
			::SetClip(::GetGrayRgn());	//   out other windows.
		}

										// Let user drag a dotted outline
										//   of the Window
		SInt32	dragDistance = ::DragGrayRgn(dragRgn, inMacEvent.where,
											&inDragRect, &inDragRect,
											noConstraint, nil);
		::MacSetPort(savePort);

			// Check if the Window moved to a new, valid location.
			// DragGrayRgn returns the constant Drag_Aborted if the
			// user dragged outside the DragRect, which cancels
			// the drag operation. We also check for the case where
			// the drag finished at its original location (zero distance).

		SInt16	horizDrag = LoWord(dragDistance);
		SInt16	vertDrag = HiWord(dragDistance);

		if ( (dragDistance != Drag_Aborted)  &&
			 (horizDrag != 0 || vertDrag != 0) ) {

				// A valid drag occurred. horizDrag and vertDrag are
				// distances from the current location. We need to
				// get the new location for the Window in global
				// coordinates. The bounding box of the Window's
				// content region gives the current location in
				// global coordinates. Add drag distances to this
				// to get the new location.

			Point	wPos =
				topLeft((**(((WindowPeek) windowToDrag)->contRgn)).rgnBBox);
			::MacMoveWindow(windowToDrag, (SInt16) (horizDrag + wPos.h),
							(SInt16) (vertDrag + wPos.v), false);
		}
	}
}


// ---------------------------------------------------------------------------
//	е Suspend												 [static] [public]
// ---------------------------------------------------------------------------
//	Call this function when the application receives a suspend event.

void
UDesktop::Suspend()
{
	sAboutToSuspend = true;

	LWindow*	theWindow;
	WindowPtr	macWindowP = ::LMGetWindowList();

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		theWindow->Suspend();
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	sAboutToSuspend = false;
	sSuspended = true;
}


// ---------------------------------------------------------------------------
//	е Resume												 [static] [public]
// ---------------------------------------------------------------------------
//	Call this function when the application receives a resume event.

void
UDesktop::Resume()
{
	sSuspended = false;

									// Tell all Windows to Resume
	LWindow*	theWindow;
	WindowPtr	macWindowP = ::LMGetWindowList();

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		theWindow->Resume();
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}
									// Activate the top Modal or Regular
									//   Window
	theWindow = LWindow::FetchWindowObject(FrontWindow());
	if ((theWindow != nil) && theWindow->HasAttribute(windAttr_Floating)) {
									// Top Window is Floating, search for
									//   the top Regular
		theWindow = FetchTopRegular();
	}
	if (theWindow != nil) {
		theWindow->Activate();
	}

	::LMSetCurActivate(nil);		// Suppress any Activate event
}


// ---------------------------------------------------------------------------
//	е Deactivate											 [static] [public]
// ---------------------------------------------------------------------------
//	Deactivate the Desktop. Call this function before displaying a
//	modal window.
//
//	ModalDialog (and all Alert traps) take over event handling. The
//	deactivate event for the front window will not get handled unless
//	you install a filter proc that properly dispatches the event.
//	However, dismissing the modal dialog generates an activate event
//	that does get handled (because the modal dialog is no longer around
//	to intercept events).
//
//	This function deactivates all windows.

void
UDesktop::Deactivate()
{
		// Start with the front (visible) window, and traverse the
		// linked list of Windows maintained by the Mac Window Manager
		// Note: Although there shouldn't be any active windows behind
		// an inactive window, we can't just stop the loop upon
		// encountering an inactive window. There could be active
		// windows behind invisible windows (that are inactive).

	WindowPtr	macWindowP = ::FrontWindow();
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		theWindow->Deactivate();
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	UCursor::Normalize();			// Show arrow cursor
}


// ---------------------------------------------------------------------------
//	е Activate												 [static] [public]
// ---------------------------------------------------------------------------
//	Activate the Desktop. Call this function after dismissing a
//	modal window.
//
//	If the front window is modal, this function activates it. If it's
//	not modal, this function activates all floating windows and the
//	front regular window.

void
UDesktop::Activate()
{
	NormalizeWindowOrder();

		// Starting with the front window, activate all visible windows up
		// to and including the first visible non-floating window.

		// Why this works:
		// > If the front window is modal, it gets activated and
		// the loop stops because it's not floating.
		// > If the front window is floating, windows keep getting
		// activated until the first regular window is activated.
		// > If the front window is regular, it gets activated and
		// the loop stops because it's not floating.

	WindowPtr	macWindowP = ::FrontWindow();
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if (theWindow->IsVisible()) {
			theWindow->Activate();
			if (!theWindow->HasAttribute(windAttr_Floating)) {
				break;
			}
		}
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	::LMSetCurActivate(nil);		// Clear any pending Activate and
	::LMSetCurDeactive(nil);		//   Deactivate actions
}


// ---------------------------------------------------------------------------
//	е FetchTopRegular										 [static] [public]
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
			 ((WindowPeek) macWindowP)->visible ) {
			break;
		}
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	return theWindow;
}


// ---------------------------------------------------------------------------
//	е FetchTopFloater										 [static] [public]
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
			 ((WindowPeek) macWindowP)->visible ) {
			break;
		}
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	return theWindow;
}


// ---------------------------------------------------------------------------
//	е FetchBottomFloater									 [static] [public]
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

	WindowPtr	macWindowP = ::LMGetWindowList();
	LWindow*	bottomFloater = nil;
	LWindow*	theWindow;
	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if (theWindow->HasAttribute(windAttr_Floating)) {
			bottomFloater = theWindow;
		}
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	return bottomFloater;
}


// ---------------------------------------------------------------------------
//	е FetchTopModal											 [static] [public]
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
//	е FetchBottomModal										 [static] [public]
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

	WindowPtr	macWindowP  = ::LMGetWindowList();
	LWindow*	bottomModal = nil;
	LWindow*	theWindow;

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != nil) {
		if (theWindow->HasAttribute(windAttr_Modal)) {
			bottomModal = theWindow;
		}
		macWindowP = (WindowPtr) ((WindowPeek) macWindowP)->nextWindow;
	}

	return bottomModal;
}


// ---------------------------------------------------------------------------
//	е FrontWindowIsModal									 [static] [public]
// ---------------------------------------------------------------------------
//	Return whether the front window is modal

bool
UDesktop::FrontWindowIsModal()
{
	bool		modalInFront = false;
	WindowPtr	macWindowP = ::FrontWindow();

	if (macWindowP != nil) {			// There is a front window
		LWindow*	ppWindow = LWindow::FetchWindowObject(macWindowP);
		if (ppWindow != nil) {			// Front window is a PowerPlant window
										//   so check the Modal attribute
			modalInFront = ppWindow->HasAttribute(windAttr_Modal);
		} else {						// Front window is a Toolbox window
										//   so check the window kind field
			modalInFront = ((WindowPeek)macWindowP)->windowKind == dialogKind;
		}
	}

	return modalInFront;
}


// ---------------------------------------------------------------------------
//	е NormalizeWindowOrder									 [static] [public]
// ---------------------------------------------------------------------------
//	Put windows in the proper order: Modals, Floaters, Regulars
//
//	Call this function when the window order might be jumbled. This can
//	happen when dealing with non-PowerPlant windows. In particular,
//	StandardGetFile calls HideWindow, which can re-order windows if
//	there are invisible windows.

void
UDesktop::NormalizeWindowOrder()
{
		// Put modal windows in front

	WindowPtr	currWindowP = ::LMGetWindowList();
	WindowPtr	nextWindowP;
	WindowPtr	behindWindowP = nil;

	while (currWindowP) {
		nextWindowP = (WindowPtr) ((WindowPeek) currWindowP)->nextWindow;
		LWindow	*ppWindow = LWindow::FetchWindowObject(currWindowP);
		if (ppWindow != nil  &&  ppWindow->HasAttribute(windAttr_Modal)) {
			if (behindWindowP == nil) {
				if (currWindowP != ::LMGetWindowList()) {
					::BringToFront(currWindowP);
				}
			} else if ( (currWindowP != (WindowPtr) ((WindowPeek) behindWindowP)->nextWindow) &&
						(currWindowP != behindWindowP) ) {
				::SendBehind(currWindowP, behindWindowP);
			}
			behindWindowP = currWindowP;
		}
		currWindowP = nextWindowP;
	}

		// Put floating windows behind last modal

	currWindowP = ::LMGetWindowList();

	while (currWindowP) {
		nextWindowP = (WindowPtr) ((WindowPeek) currWindowP)->nextWindow;
		LWindow	*ppWindow = LWindow::FetchWindowObject(currWindowP);
		if (ppWindow != nil  &&  ppWindow->HasAttribute(windAttr_Floating)) {
			if (behindWindowP == nil) {
				if (currWindowP != ::LMGetWindowList()) {
					::BringToFront(currWindowP);
				}
			} else if ( (currWindowP != (WindowPtr) ((WindowPeek) behindWindowP)->nextWindow) &&
						(currWindowP != behindWindowP) ) {
				::SendBehind(currWindowP, behindWindowP);
			}
			behindWindowP = currWindowP;
		}
		currWindowP = nextWindowP;
	}

		// At this point, the modals and floaters are in front, so the
		// remaining windows in the rear must be Regular windows or
		// non-PowerPlant windows
}


// ---------------------------------------------------------------------------
//	е GetBehindWindow									  [static] [protected]
// ---------------------------------------------------------------------------
//	Verify and adjust the inBehind window so that it places the Window at
//	the proper location within its layer

WindowPtr
UDesktop::GetBehindWindow(
	LWindow*	inWindow,
	WindowPtr	inBehind)
{

		// Verify and adjust inBehind window
		// If inBehind is in front, adjust it so that it places the
		// window at the front of its layer.
		// If inBehind is in back, adjust it so that it places the
		// window at the back of its layer.
		// If inBehind is another window, make sure that the inBehind
		// window is in the same layer as the window to create

	LWindow*	behindWindow;
	if (inBehind == window_InFront) {
		if (inWindow->HasAttribute(windAttr_Floating)) {
										// Floaters go behind bottom Modal
			behindWindow = FetchBottomModal();
			if (behindWindow != nil) {
				inBehind = behindWindow->GetMacWindow();
			}

		} else if (inWindow->HasAttribute(windAttr_Regular)) {
										// Regulars go behind Modals and
										//   Floaters
			behindWindow = FetchBottomFloater();
			if (behindWindow == nil) {
				behindWindow = FetchBottomModal();
			}
			if (behindWindow != nil) {
				inBehind = behindWindow->GetMacWindow();
			}
		}

	} else if (inBehind == window_InBack) {
		if (inWindow->HasAttribute(windAttr_Modal)) {
			behindWindow = FetchBottomModal();
			if (behindWindow != nil) {
				inBehind = behindWindow->GetMacWindow();
			} else {
				inBehind = window_InFront;
			}

		} else if (inWindow->HasAttribute(windAttr_Floating)) {
			behindWindow = FetchBottomFloater();
			if (behindWindow == nil) {
				behindWindow = FetchBottomModal();
			}
			if (behindWindow != nil) {
				inBehind = behindWindow->GetMacWindow();
			} else {
				inBehind = window_InFront;
			}
		}

	} else {
		behindWindow = LWindow::FetchWindowObject(inBehind);

			// If there's a mismatch between the layers of the
			// Window and the behind Window, put the Window at
			// the top of its layer

		if (inWindow->HasAttribute(windAttr_Modal)) {
			if (!behindWindow->HasAttribute(windAttr_Modal)) {
				inBehind = window_InFront;
			}

		} else if (inWindow->HasAttribute(windAttr_Floating)) {
			if (!behindWindow->HasAttribute(windAttr_Floating)) {
				inBehind = window_InFront;
				behindWindow = FetchBottomModal();
				if (behindWindow != nil) {
					inBehind = behindWindow->GetMacWindow();
				}
			}
		} else {
			if (!behindWindow->HasAttribute(windAttr_Regular)) {
				inBehind = window_InFront;
				behindWindow = FetchBottomFloater();
				if (behindWindow != nil) {
					inBehind = behindWindow->GetMacWindow();
				}
			}
		}
	}

	return inBehind;
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


PP_End_Namespace_PowerPlant

#endif // PP_Target_Classic
