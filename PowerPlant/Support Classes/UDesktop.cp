// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDesktop.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Standard window manipulation routines.
//
//	Implementation:
//		For the most part, these wrapper funtions just call the standard
//		Window Manager traps. There is little or no overhead when using
//		this class as compared with using direct Window Manager calls.
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
#include <LWindow.h>
#include <UCursor.h>
#include <UTBAccessors.h>


PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from a WIND resource for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*	/* inWindow */,
	ResIDT		inWINDid,
	WindowPtr	inBehind)
{
	return ::GetNewCWindow(inWINDid, nil, inBehind);
}


// ---------------------------------------------------------------------------
//	¥ NewDeskWindow											 [static] [public]
// ---------------------------------------------------------------------------
//	Return a new Toolbox Window created from input parameters for the
//	specified Window object

WindowPtr
UDesktop::NewDeskWindow(
	LWindow*		/* inWindow */,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			inProcID,
	Boolean			inHasGoAway,
	WindowPtr		inBehind)
{
									// Make Toolbox Window
	return ::NewCWindow(nil, &inGlobalBounds, inTitle,
							false, inProcID, inBehind, inHasGoAway, 0);
}


// ---------------------------------------------------------------------------
//	¥ WindowIsSelected										 [static] [public]
// ---------------------------------------------------------------------------
//	Return whether a Window is selected, i.e., it is the front window

bool
UDesktop::WindowIsSelected(
	LWindow*	inWindow)
{
	return (inWindow->GetMacWindow() == ::FrontWindow());
}


// ---------------------------------------------------------------------------
//	¥ SelectDeskWindow										 [static] [public]
// ---------------------------------------------------------------------------
//	Select a Window by bringing to the front

void
UDesktop::SelectDeskWindow(
	LWindow*	inWindow)
{
	if (!WindowIsSelected(inWindow)) {
		::SelectWindow(inWindow->GetMacWindow());
	}
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
	Deactivate();						// Suspended Desktop is inactive
}


// ---------------------------------------------------------------------------
//	¥ Resume												 [static] [public]
// ---------------------------------------------------------------------------
//	Call this function when the application receives a resume event.

void
UDesktop::Resume()
{
	Activate();							// Activate Desktop upon Resume
}


// ---------------------------------------------------------------------------
//	¥ Deactivate											 [static] [public]
// ---------------------------------------------------------------------------
//	Deactivate the front window. Call this function before displaying a
//	modal window.
//
//	ModalDialog (and all Alert traps) take over event handling. The
//	deactivate event for the front window will not get handled unless
//	you install a filter proc that properly dispatches the event.
//	However, dismissing the modal dialog generates an activate event
//	that does get handled (because the modal dialog is no longer around
//	to intercept events).

void
UDesktop::Deactivate()
{
	LWindow*	theWindow = LWindow::FetchWindowObject(::FrontWindow());
	if (theWindow != nil) {
		theWindow->Deactivate();
	}
	UCursor::Normalize();			// Show arrow cursor
}


// ---------------------------------------------------------------------------
//	¥ Activate												 [static] [public]
// ---------------------------------------------------------------------------
//	Activate the front window. Call this function after dismissing a
//	modal window.
//
//	Although the front window will normally receive an Activate event
//	when necessary [see Deactivate above], we explicitly Activate it
//	here. Activating a window that's already active does nothing, so
//	it doesn't hurt to call this function whenever you want to Activate
//	the Desktop.

void
UDesktop::Activate()
{
	LWindow*	theWindow = LWindow::FetchWindowObject(::FrontWindow());
	if (theWindow != nil) {
		theWindow->Activate();
	}
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
	return nil;						// No Floaters for UDesktop
}


// ---------------------------------------------------------------------------
//	¥ FetchBottomFloater									 [static] [public]
// ---------------------------------------------------------------------------
//	Returns the bottommost floating Window (visible or not)

LWindow*
UDesktop::FetchBottomFloater()
{
	return nil;						// No floaters for UDesktop
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

	LWindow		*topModal = nil;
	LWindow		*theWindow = LWindow::FetchWindowObject(::FrontWindow());
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
		LWindow*	ppWindow = LWindow::FetchWindowObject(macWindowP);
		if (ppWindow != nil) {			// Front window is a PowerPlant window
										//   so check the Modal attribute
			modalInFront = ppWindow->HasAttribute(windAttr_Modal);
		} else {						// Front window is a Toolbox window
										//   so check the window kind field
			modalInFront = GetWindowKind(macWindowP) == dialogKind;
		}
	}

	return modalInFront;
}


// ---------------------------------------------------------------------------
//	¥ NormalizeWindowOrder									 [static] [public]
// ---------------------------------------------------------------------------
//	Put windows in the proper order
//
//	This function does nothing since the standard Desktop class does not
//	support multiple window layers

void
UDesktop::NormalizeWindowOrder()
{
}


// ---------------------------------------------------------------------------
//	¥ GetBehindWindow									  [static] [protected]
// ---------------------------------------------------------------------------
//	Verify and adjust the inBehind window so that it places the Window at
//	the proper location within its layer

WindowPtr
UDesktop::GetBehindWindow(
	LWindow*	/* inWindow */,
	WindowPtr	inBehind)
{
	return inBehind;				// No adjustment needed, only one layer
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

