// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDesktop.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility class for manipulating windows.
//
//	In order to implement floating windows, you can't call certain Window
//	Manager routines, namely SelectWindow, ShowWindow, HideWindow, and
//	DragWindow. You also have to make sure new windows are created at the
//	proper position in the window list. This class provides wrapper functions
//	for these Window Manager routines.
//
//		Window Manager Trap				UDesktop Wrapper Function
//		-------------------				-------------------------
//		GetNewWindow/GetNewCWindow		NewDeskWindow
//		SelectWindow					SelectDeskWindow
//		ShowWindow						ShowDeskWindow
//		HideWindow						HideDeskWindow
//		DragWindow						DragDeskWindow
//
//	The UDesktop routines take Window objects as parameters rather than
//	Toolbox window pointers.
//
//	Other Functions:
//
//		WindowIsSelected	- Checks if Window is at the front of its layer
//		Suspend				- Call when program receives a suspend event
//		Resume				- Call when program receives a resume event
//		Deactivate			- Call before displaying a modal dialog
//		Activate			- Call after dismissing a modal dialog
//
//		FetchTopRegular		- Returns topmost window in Regular layer
//		FetchTopFloater		- Returns topmost window in Floater layer
//		FetchBottomFloater	- Returns bottommost window in Floater layer
//		FetchTopModal		- Returns topmost window in Modal layer
//		FetchBottomModal	- Returns bottommost window in Modal layer
//		FrontWindowIsModal	- Checks if front window is Modal
//		NormalizeWindowOrder- Puts windows in the proper order

#ifndef _H_UDesktop
#define _H_UDesktop
#pragma once

#include <PP_Prefix.h>
#include <MacWindows.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LWindow;

// ---------------------------------------------------------------------------

class	UDesktop {
public:
	static WindowPtr	NewDeskWindow(
								LWindow*		inWindow,
								ResIDT			inWINDid,
								WindowPtr		inBehind);

	static WindowPtr	NewDeskWindow(
								LWindow*		inWindow,
								const Rect&		inGlobalBounds,
								ConstStringPtr	inTitle,
								SInt16			inProcID,
								Boolean			inHasGoAway,
								WindowPtr		inBehind);

	static bool			WindowIsSelected	(LWindow*	inWindow);
	static void			SelectDeskWindow	(LWindow*	inWindow);
	static void			ShowDeskWindow		(LWindow*	inWindow);
	static void			HideDeskWindow		(LWindow*	inWindow);

	static void			DragDeskWindow(
								LWindow*			inWindow,
								const EventRecord&	inMacEvent,
								const Rect&			inDragRect);

	static void			Suspend();
	static void			Resume();

	static void			Deactivate();
	static void			Activate();

	static LWindow*		FetchTopRegular();
	static LWindow*		FetchTopFloater();
	static LWindow*		FetchBottomFloater();
	static LWindow*		FetchTopModal();
	static LWindow*		FetchBottomModal();

	static bool			FrontWindowIsModal();
	static void			NormalizeWindowOrder();

protected:
	static WindowPtr	GetBehindWindow(
								LWindow*		inWindow,
								WindowPtr		inBehind);
};


// ===========================================================================

class StDesktopDeactivator {
public:
	StDesktopDeactivator();
	~StDesktopDeactivator();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
