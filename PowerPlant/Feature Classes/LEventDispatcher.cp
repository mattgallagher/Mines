// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventDispatcher.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Dispatches Toolbox Events to the proper objects

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LEventDispatcher.h>
#include <LCommander.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LPeriodical.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <UCursor.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <UWindows.h>

#include <AEInteraction.h>
#include <CarbonEvents.h>

#if !TARGET_RT_MAC_MACHO
	#include <DiskInit.h>
#endif

#include <LowMem.h>
#include <MacWindows.h>
#include <Sound.h>
#include <ToolUtils.h>


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LEventDispatcher*		LEventDispatcher::sCurrentDispatcher = nil;
EventRecord				LEventDispatcher::sCurrentEvent;


// ---------------------------------------------------------------------------
//	¥ LEventDispatcher						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LEventDispatcher::LEventDispatcher()
{
	mSaveDispatcher = LEventDispatcher::GetCurrentEventDispatcher();
	sCurrentDispatcher = this;

	::HiliteMenu(0);				// If new EventDispatcher is created as
									//   a result of a menu command, menu
									//   title should be unhilited
}


// ---------------------------------------------------------------------------
//	¥ ~LEventDispatcher						Destructor				  [public]
// ---------------------------------------------------------------------------

LEventDispatcher::~LEventDispatcher()
{
	sCurrentDispatcher = mSaveDispatcher;
}


// ---------------------------------------------------------------------------
//	¥ DispatchEvent													  [public]
// ---------------------------------------------------------------------------
//	Process a Toolbox Event

void
LEventDispatcher::DispatchEvent(
	const EventRecord	&inMacEvent)
{
	sCurrentEvent = inMacEvent;

	switch (inMacEvent.what)
	{
		case mouseDown:
			AdjustCursor(inMacEvent);
			EventMouseDown(inMacEvent);
			break;

		case mouseUp:
			EventMouseUp(inMacEvent);
			break;

		case keyDown:
			EventKeyDown(inMacEvent);
			break;

		case autoKey:
			EventAutoKey(inMacEvent);
			break;

		case keyUp:
			EventKeyUp(inMacEvent);
			break;

		case diskEvt:
			EventDisk(inMacEvent);
			break;

		case updateEvt:
			EventUpdate(inMacEvent);
			break;

		case activateEvt:
			EventActivate(inMacEvent);
			break;

		case osEvt:
			EventOS(inMacEvent);
			break;

		case kHighLevelEvent:
			EventHighLevel(inMacEvent);
			break;

		default:
			UseIdleTime(inMacEvent);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustCursor
// ---------------------------------------------------------------------------
//	Adjust the shape of the cursor (mouse pointer)

void
LEventDispatcher::AdjustCursor(
	const EventRecord&	inMacEvent)
{
	bool		useArrow = true;	// Assume cursor will be the Arrow

									// Find out where the mouse is
	WindowPtr	macWindowP;
	Point		globalMouse = inMacEvent.where;
	WindowPartCode	part = ::MacFindWindow(globalMouse, &macWindowP);
	
	mMouseRgn.Clear();				// Start with an empty mouse region
	
	if (macWindowP != nil) {		// Mouse is inside a window
		
		LWindow*	theWindow = LWindow::FetchWindowObject(macWindowP);
		
		if ( (theWindow != nil)  &&			// Mouse is inside an active
			 theWindow->IsActive()  &&		//   and enabled PowerPlant
			 theWindow->IsEnabled() ) {		//   window
			 
			useArrow = false;
			Point	portMouse = globalMouse;
			theWindow->GlobalToPortPoint(portMouse);

			if (part == inContent) {
				theWindow->AdjustContentMouse(portMouse, inMacEvent, mMouseRgn);

			} else {
				theWindow->AdjustStructureMouse(part, inMacEvent, mMouseRgn);
			}
		}
	}
	
	if (mMouseRgn.IsEmpty()) {		// No Pane set the mouse region

		mMouseRgn = ::GetGrayRgn();	// Gray region is desktop minus menu bar
		
									// Add bounds of main device so mouse
									//   region includes the menu bar
		GDHandle	mainGD = ::GetMainDevice();
		mMouseRgn += (**mainGD).gdRect;
		
									// Exclude structure regions of all
									//   active windows
		UWindows::ExcludeActiveStructures(mMouseRgn);
	}
	
	
	if (useArrow) {					// Window didn't set the cursor
		UCursor::SetArrow();		// Default cursor is the arrow
	}
}


// ---------------------------------------------------------------------------
//	¥ EventMouseDown
// ---------------------------------------------------------------------------
//	Respond to a Mouse Down event

void
LEventDispatcher::EventMouseDown(
	const EventRecord&	inMacEvent)
{
	WindowPtr	macWindowP;
	SInt16		thePart = ::MacFindWindow(inMacEvent.where, &macWindowP);

	switch (thePart) {

		case inMenuBar:
			ClickMenuBar(inMacEvent);
			break;

		case inSysWindow:
			#if PP_Target_Classic
				::SystemClick(&inMacEvent, macWindowP);
			#endif
			break;

		case inDesk: {			// Usually, this happens when the user
								//   clicks outside the program's windows
								//   when a truly modal window is active.
			LWindow *frontW = LWindow::FetchWindowObject(::FrontWindow());
			if ( (frontW != nil) &&
				 frontW->HasAttribute(windAttr_Modal) ) {
		
				::SysBeep(1);	// Beep informs the user that no context
			}					//   switch is allowed
			break;
		}

		case inContent:
			LCommander::SetUpdateCommandStatus(true);
			// Fall Thru
		case inDrag:
		case inGrow:
		case inGoAway:
		case inZoomIn:
		case inZoomOut:
		case inCollapseBox:
		case inProxyIcon:

				// Check for the special case of clicking in an underlying
				// window when the front window is modal.

			if (macWindowP != ::FrontWindow()) {
								// Clicked Window is not the front one
				LWindow *frontW = LWindow::FetchWindowObject(::FrontWindow());
				if ( (frontW != nil) &&
					 frontW->HasAttribute(windAttr_Modal) ) {
					 			// Front Window is Modal
					 			// Change part code to a special number.
					 			//   The only exception is a command-Drag,
					 			//   which moves, but does not re-order,
					 			//   an underlying window.
					 if ( (thePart != inDrag) ||
					 	  !(inMacEvent.modifiers & cmdKey) ) {
						thePart = click_OutsideModal;
					}
				}
			}

								// Dispatch click to Window object
			LWindow	*theWindow = LWindow::FetchWindowObject(macWindowP);
			if (theWindow != nil) {
				theWindow->HandleClick(inMacEvent, thePart);
			}
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickMenuBar
// ---------------------------------------------------------------------------
//	Respond to a mouse click in the menu bar

void
LEventDispatcher::ClickMenuBar(
	const EventRecord&	inMacEvent)
{
	LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();

	if (theMenuBar != nil) {
		StUnhiliteMenu	unhiliter;		// Destructor unhilites menu title
		SInt32			menuChoice;
		CommandT		menuCmd = theMenuBar->MenuCommandSelection(
												inMacEvent, menuChoice);

		if (menuCmd != cmd_Nothing) {
			if (LCommander::GetTarget() != nil) {
				LCommander::SetUpdateCommandStatus(true);
				LCommander::GetTarget()->ProcessCommand(menuCmd, &menuChoice);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ EventMouseUp
// ---------------------------------------------------------------------------
//	Respond to a Mouse Up event

void
LEventDispatcher::EventMouseUp(
	const EventRecord&	inMacEvent)
{
		// Let Pane in which the preceding Mouse Down occurred handle
		// the Mouse Up event

	LPane	*paneClicked = LPane::GetLastPaneClicked();
	if (paneClicked != nil) {
		paneClicked->EventMouseUp(inMacEvent);
	}
}


// ---------------------------------------------------------------------------
//	¥ EventKeyDown
// ---------------------------------------------------------------------------
//	Respond to a Key Down Event

void
LEventDispatcher::EventKeyDown(
	const EventRecord&	inMacEvent)
{
		// Check if the keystroke is a Menu Equivalent
	SInt32		menuChoice;
	CommandT	keyCommand	= cmd_Nothing;
	LMenuBar*	theMenuBar	= LMenuBar::GetCurrentMenuBar();

	if (theMenuBar != nil) {
		keyCommand = theMenuBar->FindKeyCommand(inMacEvent, menuChoice);
	}

	if (LCommander::GetTarget() != nil) {
		if (keyCommand != cmd_Nothing) {
			StUnhiliteMenu	unhiliter;		// Destructor unhilites menu title
			LCommander::SetUpdateCommandStatus(true);
			LCommander::GetTarget()->ProcessCommand(keyCommand, &menuChoice);
		} else {
			LCommander::GetTarget()->ProcessKeyPress(inMacEvent);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ EventAutoKey
// ---------------------------------------------------------------------------
//	Respond to an auto-key event

void
LEventDispatcher::EventAutoKey(
	const EventRecord&	inMacEvent)
{
									// Do NOT check for Menu Equivalent
									// Handle the same as a single keystroke
	if (LCommander::GetTarget() != nil) {
		LCommander::GetTarget()->ProcessKeyPress(inMacEvent);
	}
}


// ---------------------------------------------------------------------------
//	¥ EventKeyUp
// ---------------------------------------------------------------------------
//	Respond to a Key Up event
//
//	By default, the system masks out Key Up events.

void
LEventDispatcher::EventKeyUp(
	const EventRecord&	/* inMacEvent */)
{
}


// ---------------------------------------------------------------------------
//	¥ EventDisk
// ---------------------------------------------------------------------------
//	Respond to a Disk-inserted event

void
LEventDispatcher::EventDisk(
	const EventRecord&	inMacEvent)
{
#if TARGET_API_MAC_CARBON			// OS handles disk init under Carbon
	#pragma unused(inMacEvent)

#else

	if (HiWord(inMacEvent.message) != noErr) {

		// Error mounting disk. Ask if user wishes to format it.

		::DILoad();
		::DIBadMount(Point_00, inMacEvent.message);
		::DIUnload();
	}

#endif
}


// ---------------------------------------------------------------------------
//	¥ EventUpdate
// ---------------------------------------------------------------------------
//	Respond to an Update event

void
LEventDispatcher::EventUpdate(
	const EventRecord&	inMacEvent)
{
	LWindow	*theWindow = LWindow::FetchWindowObject(
									(WindowPtr) inMacEvent.message);
	if (theWindow != nil) {
		theWindow->UpdatePort();
	}
}


// ---------------------------------------------------------------------------
//	¥ EventActivate
// ---------------------------------------------------------------------------
//	Respond to an Activate (or Deactivate) Event

void
LEventDispatcher::EventActivate(
	const EventRecord&	inMacEvent)
{
	LWindow	*theWindow = LWindow::FetchWindowObject(
									(WindowPtr) inMacEvent.message);
	if (theWindow != nil) {
		if (inMacEvent.modifiers & activeFlag) {
			LCommander::SetUpdateCommandStatus(true);
			theWindow->Activate();
		} else {
			theWindow->Deactivate();
									// No need to update status now.
									//   An Activate event must be pending,
									//   and it will update the status.
			LCommander::SetUpdateCommandStatus(false);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ EventOS
// ---------------------------------------------------------------------------
//	Respond to an OS event (MouseMoved, Suspend, Resume)

void
LEventDispatcher::EventOS(
	const EventRecord&	inMacEvent)
{
	UInt8	osEvtFlag = (UInt8) (((UInt32) inMacEvent.message) >> 24);

	if (osEvtFlag == mouseMovedMessage) {
		UseIdleTime(inMacEvent);
	} else if (osEvtFlag == suspendResumeMessage) {
		if (inMacEvent.message & resumeFlag) {
			EventResume(inMacEvent);
		} else {
			EventSuspend(inMacEvent);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ EventResume
// ---------------------------------------------------------------------------
//	Respond to a Resume event

void
LEventDispatcher::EventResume(
	const EventRecord&	/* inMacEvent */)
{
	LCommander::SetUpdateCommandStatus(true);

	UDesktop::Resume();				// Desktop handles resuming Windows

	if (LCommander::GetTarget() == nil) {

			// Some Commander must be the Target when the program is
			// in the foreground. No Window (or Window SubCommander)
			// became the Target when the Desktop resumed, so we need
			// to restore the Target here. The Target will be some
			// Commander that's not part of a Window, possibly the
			// Application itself.

		LCommander	*topCommander = LCommander::GetTopCommander();
		if (topCommander != nil) {
			topCommander->RestoreTarget();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ EventSuspend
// ---------------------------------------------------------------------------
//	Respond to a Suspend event

void
LEventDispatcher::EventSuspend(
	const EventRecord&	/* inMacEvent */)
{
	::HiliteMenu(0);				// Unhighlight menu titles
	UDesktop::Suspend();			// Desktop handles suspending Windows

		// When in the background, the program does not receive
		// keystrokes and menu commands. Therefore, no Commander
		// is on duty, and the Target should be nil.

	LCommander::ForceTargetSwitch(nil);
}


// ---------------------------------------------------------------------------
//	¥ EventHighLevel
// ---------------------------------------------------------------------------
//	Respond to a High Level (Apple) Event

void
LEventDispatcher::EventHighLevel(
	const EventRecord&	inMacEvent)
{
	LCommander::SetUpdateCommandStatus(true);
	::AEProcessAppleEvent(&inMacEvent);
}


// ---------------------------------------------------------------------------
//	¥ UseIdleTime
// ---------------------------------------------------------------------------
//	Respond to a NULL or MouseMoved Event

void
LEventDispatcher::UseIdleTime(
	const EventRecord&	inMacEvent)
{
	sCurrentEvent = inMacEvent;

	LPeriodical::DevoteTimeToIdlers(inMacEvent);
}


// ---------------------------------------------------------------------------
//	¥ UpdateMenus
// ---------------------------------------------------------------------------
//	Update the status of all menu items
//
//	General Strategy:
//	(1) Designate every Menu as "unused".
//	(2) Iterate through all menu commands, asking the Target chain for
//		the status of each one. Enable/Disable, mark, and adjust the
//		name of each menu accordingly. Enabling any item in a Menu
//		designates that Menu as "used".
//	(3)	Iterate through each menu, asking for the status of a special
//		synthetic command that indicates the entire menu. At this time,
//		Commanders can perform operations that affect the menu as a whole.
//		Menus that are desinated "used" are enabled; "unused" ones
//		are disabled.

void
LEventDispatcher::UpdateMenus()
{
	LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();

	if (theMenuBar == nil) {
		return;
	}

	SInt16		menuItem;
	MenuHandle	macMenuH = nil;
	LMenu*		theMenu = nil;
	CommandT	theCommand;
	Boolean		isEnabled;
	Boolean		usesMark;
	UInt16		mark;
	Str255		itemName;
	LCommander*	theTarget = LCommander::GetTarget();

									// Designate each menu as "unused"
	while (theMenuBar->FindNextMenu(theMenu)) {
		theMenu->SetUsed(false);
	}

									// Loop thru each menu item that has an
									//   associated command
	while (theMenuBar->FindNextCommand(menuItem, macMenuH,
										theMenu, theCommand)) {

									// Don't change menu item state for
									//   special commands (all negative
									//   values except cmd_UseMenuItem)
		if ((theCommand > 0) || (theCommand == cmd_UseMenuItem)) {

									// For commands that depend on the menu
									//   item, get synthetic command number
			if (theCommand == cmd_UseMenuItem) {
				theCommand = theMenu->SyntheticCommandFromIndex(menuItem);
			}

									// Ask Target if command is enabled,
									//   if the menu item should be marked,
									//   and if the name should be changed
			isEnabled   = false;
			usesMark    = false;
			itemName[0] = 0;

			if (theTarget != nil) {
				theTarget->ProcessCommandStatus(theCommand, isEnabled,
										usesMark, mark, itemName);
			}

				// Adjust the state of each menu item as needed.
				// Also designate as "used" the Menu containing an
				// enabled item.

			if (isEnabled) {
				theMenu->EnableItem(menuItem);
				theMenu->SetUsed(true);
			} else {
				theMenu->DisableItem(menuItem);
			}

			if (usesMark) {
				::SetItemMark(macMenuH, menuItem, (SInt16) mark);
			}

			if (itemName[0] > 0) {
				::SetMenuItemText(macMenuH, menuItem, itemName);
			}

		} else if (theCommand < 0) {
									// Don't change state of items with
									//   negative command numbers
			if (theMenu->ItemIsEnabled(menuItem)) {
				theMenu->SetUsed(true);
			}

		} else {					// Item has command number 0
			theMenu->DisableItem(menuItem);
		}

	}

	if (theTarget != nil) {
									// Loop thru each menu
		theMenu = nil;
		while (theMenuBar->FindNextMenu(theMenu)) {

				// The "command" for an entire Menu is the synthetic command
				// number for item zero (negative number that has the Menu ID
				// in the high word and 0 in the low word.

			theCommand = theMenu->SyntheticCommandFromIndex(0);

				// The Target chain now has the opportunity to do something
				// to the Menu as a whole, as a call for this special
				// synthetic command is made only once for each menu.
				// For example, a text handling Commander could put a check
				// mark next to the current font in a Fonts menu.
				//
				// The isEnabled parameter [outEnabled as an argument to
				// FindCommandStatus()] should be set to true to enable
				// a menu that does not contain commands that have
				// already been explicitly enabled. For example, a Fonts
				// menu typically has no associated commands (the menu
				// items all use synthetic commands), so a Commander
				// must set outEnabled to true when asked the status
				// of the synthetic command corresponding the Fonts menu.
				//
				// The mark information is ignored, as is the itemName.
				// It would be possible to use the itemName to facilitate
				// dynamic changing of menu titles. However, this is bad
				// interface design which we don't want to encourage.

			isEnabled   = false;
			usesMark    = false;
			itemName[0] = 0;

			theTarget->ProcessCommandStatus(theCommand, isEnabled,
											usesMark, mark, itemName);

				// Menu is "used" if it contains commands that were
				// explicity enabled in the loop above, or if this
				// last call passes back "true" for isEnabled.

			if (isEnabled) {
				theMenu->SetUsed(true);
			}

				// Enable all "used" Menus and disable "unused" ones.
				// Force redraw of MenuBar if the enabled state of the
				// Menu changes.

			bool	wasEnabled = theMenu->ItemIsEnabled(0);
			bool	nowEnabled = true;

			if (theMenu->IsUsed()) {
				theMenu->EnableItem(0);
			} else {
				theMenu->DisableItem(0);
				nowEnabled = false;
			}

			if (wasEnabled != nowEnabled) {
				::InvalMenuBar();
			}
		}
		
			// Mac OS X automatically puts "Quit" and "Preferences"
			// items in the Application menu. The technique for
			// enabling/disabling these items is different from
			// regular ones.
			
		#if TARGET_API_MAC_CARBON
		
			if (UEnvironment::HasFeature(env_HasAquaTheme)) {
			
										// Quit menu item
				theTarget->ProcessCommandStatus(cmd_Quit,
					isEnabled, usesMark, mark, itemName);
					
				if (isEnabled) {
					::EnableMenuCommand(nil, kHICommandQuit);
				} else {
					::DisableMenuCommand(nil, kHICommandQuit);
				}
			
										// Preferences menu item
				theTarget->ProcessCommandStatus(cmd_Preferences,
					isEnabled, usesMark, mark, itemName);
					
				if (isEnabled) {
					::EnableMenuCommand(nil, kHICommandPreferences);
				} else {
					::DisableMenuCommand(nil, kHICommandPreferences);
				}
			}
		
		#endif
	}
}


// ---------------------------------------------------------------------------
//	¥ ExecuteAttachments
// ---------------------------------------------------------------------------
//	Tell all associated Attachments to execute themselves for the
//	specified message
//
//	The return value specifies whether the default Host action should be
//	executed. The value is false if any Attachment's Execute() function
//	returns false, otherwise it's true.

Boolean
LEventDispatcher::ExecuteAttachments(
	MessageT		inMessage,
	void*			ioParam)
{
	Boolean	executeHost = true;

		// Execute the Attachments for the EventDispatcher that was
		// in control before this one took over

	if (mSaveDispatcher != nil) {
		executeHost = mSaveDispatcher->ExecuteAttachments(inMessage, ioParam);
	}

		// Inherited function will execute Attachments for this object

	return (executeHost &&
			LAttachable::ExecuteAttachments(inMessage, ioParam));
}


PP_End_Namespace_PowerPlant
