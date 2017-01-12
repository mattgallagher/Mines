// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCarbonEvents.cp			PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Carbon Event handler functions suitable for use with LEventHandlerFunctor

#include <UCarbonEvents.h>
#include <LCommander.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UCursor.h>
#include <UEnvironment.h>

#include <MacWindows.h>
#include <Menus.h>

#include <Sound.h>	// $$$ DEBUG

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ DoSuspend
// ---------------------------------------------------------------------------
//	Handler for suspend event (now called application deactivate)
//		[ kEventClassApplication, kEventAppDeactivated ]

OSStatus
UCarbonEvents::DoSuspend(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	::HiliteMenu(0);				// Unhighlight menu titles
	::HideFloatingWindows();

		// When in the background, the program does not receive
		// keystrokes and menu commands. Therefore, no Commander
		// is on duty, and the Target should be nil.
		
	LCommander::ForceTargetSwitch(nil);

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ DoResume
// ---------------------------------------------------------------------------
//	Handler for resume event (now called application activated)
//		[ kEventClassApplication, kEventAppActivated ]

OSStatus
UCarbonEvents::DoResume(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	::ShowFloatingWindows();
	
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

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ DoProcessCommand
// ---------------------------------------------------------------------------
//	Handler for Process Command Events
//		[ kEventClassCommand, kEventProcessCommand ]

OSStatus
UCarbonEvents::DoProcessCommand(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
	LCommander*	theTarget = LCommander::GetTarget();
	
	if (theTarget == nil) {				// We must have a Target in order to
		return eventNotHandledErr;		//   process commands
	}
	
	OSStatus	result = eventNotHandledErr;
	
	HICommand	theCommand;				// Get info about the command
	
	::GetEventParameter( inEventRef,
						 kEventParamDirectObject,
						 typeHICommand,
						 nil,
						 sizeof(HICommand),
						 nil,
						 &theCommand);
	
	CommandT	commandID = (CommandT) theCommand.commandID;
	SInt16		menuIndex = 0;
	
		// As a special case, we map 'quit' and 'pref' to the
		// standard PP command numbers for Quit and Preferences
	
	if (commandID == kHICommandQuit) {
		commandID = cmd_Quit;

	} else if (commandID == kHICommandPreferences) {
		commandID = cmd_Preferences;
	
	} else if ( (theCommand.attributes & kHICommandFromMenu) &&
		 		(commandID == cmd_Nothing) ) {
		 		
		 	// The command is a menu selection without a HI Command
		 	// number. Look up the PP command corresponding to
		 	// the menu choice.
	
		menuIndex = (SInt16) theCommand.menu.menuItemIndex;
		commandID = LMenuBar::GetCurrentMenuBar()->FindCommand(
								::GetMenuID(theCommand.menu.menuRef),
								menuIndex);
	}

	if (theTarget->ProcessCommand(commandID, &menuIndex)) {
		result = noErr;					// We handled the command
		::HiliteMenu(0);
	}
	
	return result;
}


// ---------------------------------------------------------------------------
//	¥ DoMenuEnableItems
// ---------------------------------------------------------------------------
//	Handler for enabling/disablings menu items
//		[ kEventClassMenu, kEventMenuEnableItems ]

OSStatus
UCarbonEvents::DoMenuEnableItems(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
		// This event is sent on a per menu basis. We start by
		//		- Getting the PP MenuBar
		//		- Extracting the MenuRef from the event
		//		- Getting the PP Menu from the MenuRef
		//
		// If any of these steps fails, we return eventNotHandledErr

	LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();

	if (theMenuBar == nil) {
		return eventNotHandledErr;
	}
	
	OSStatus	status;
	MenuRef		macMenu;
	
	status = ::GetEventParameter( inEventRef,
								  kEventParamDirectObject,
								  typeMenuRef, nil,
								  sizeof(MenuRef), nil,
								  &macMenu );
								  
	if (status != noErr) {
		return eventNotHandledErr;
	}
	
	LMenu*	theMenu  = theMenuBar->FetchMenu( ::GetMenuID(macMenu) );
	
	if (theMenu == nil) {
		return eventNotHandledErr;
	}
	
		// We loop through each command in the menu, asking the
		// Target for the status of the command.
	
	LCommander*	theTarget = LCommander::GetTarget();
	
	SInt16		menuItem = 0;
	CommandT	theCommand;
	Boolean		isEnabled;
	Boolean		usesMark;
	UInt16		mark;
	Str255		itemName;
	
	while (theMenu->FindNextCommand(menuItem, theCommand)) {
	
								// Adjust state of each menu item as needed
		isEnabled	= false;
		usesMark	= false;
		itemName[0] = 0;
		
		if (theTarget != nil) {
			theTarget->ProcessCommandStatus(theCommand, isEnabled,
											usesMark, mark, itemName);
		}
		
		UpdateMenuItem( macMenu, (MenuItemIndex) menuItem, isEnabled,
						usesMark, (CharParameter) mark, itemName );
	}
	
		// Finally, we ask the Target for the status of the menu
		// as a whole. This is where commanders can adjust items
		// that don't have associated command numbers, or multiple
		// items which are related. For example, putting check marks
		// in the Font and Size menus.
	
	if (theTarget != nil) {
	
			// With Aqua Menus (Mac OS X), the Quit and Preferences items
			// are in the application menu and have HI Commands rather
			// than PP commands. To support both Aqua and Classic, we
			// explicitly update those items, mapping the HI Command ID
			// to the PowerPlant Command ID.
	
		if ( (theMenu->GetMenuID() == MENU_Apple) &&
			 UEnvironment::HasFeature(env_HasAquaTheme) ) {
			 
			UpdateCommandMenuItem(kHICommandQuit, cmd_Quit);
			UpdateCommandMenuItem(kHICommandPreferences, cmd_Preferences);
		}			
	
		theCommand = theMenu->SyntheticCommandFromIndex(0);
		
		isEnabled	= false;
		usesMark	= false;
		itemName[0] = 0;
	
		theTarget->ProcessCommandStatus(theCommand, isEnabled,
										usesMark, mark, itemName);
	}
	
		// Check if the menu title needs to be enabled or disabled.
		// This happens if the menu has gone from having no enabled
		// items to having at least one enabled item, or vice versa.
	
	bool	titleShouldBeEnabled = ::MenuHasEnabledItems(macMenu);
	
	if (::IsMenuItemEnabled(macMenu, 0) != titleShouldBeEnabled) {
		if (titleShouldBeEnabled) {
			::MacEnableMenuItem(macMenu, 0);
		} else {
			::DisableMenuItem(macMenu, 0);
		}
		::InvalMenuBar();			// Force redraw of menu titles
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ DoKeyDown
// ---------------------------------------------------------------------------
//	Handler for keystrokes
//		[ kEventClassKeyboard, kEventRawKeyDown ]
//		[ kEventClassKeyboard, kEventRawKeyRepeat ]

OSStatus
UCarbonEvents::DoKeyDown(
	EventHandlerCallRef	inCallRef,
	EventRef			inEventRef)
{
	OSStatus	status;
										// Give default handler a chance to
										//   process the event first
	status = ::CallNextEventHandler(inCallRef, inEventRef);
	
	if (status == noErr) {
		return noErr;
	}

										// Fill in a generic EventRecord
	EventRecord		keyEvent;
	MakeOldEventRecord(inEventRef, keyEvent);
	
	keyEvent.what = keyDown;
	
	char		charCode;				// Get character typed
	
	status = ::GetEventParameter( inEventRef,
								  kEventParamKeyMacCharCodes,
								  typeChar, nil,
								  sizeof(char), nil,
								  &charCode );
	
	UInt32		keyCode;				// Get key code
	
	status = ::GetEventParameter( inEventRef,
								   kEventParamKeyCode,
								   typeUInt32, nil,
								   sizeof(UInt32), nil,
								   &keyCode );
								   
	keyEvent.message = (keyCode << 8) + charCode;
						 
	OSStatus	result = eventNotHandledErr;

										// Target handles keystrokes
	if (LCommander::GetTarget() != nil) {

		if (LCommander::GetTarget()->ProcessKeyPress(keyEvent)) {
			result = noErr;
		}
	}

	return result;
}


// ---------------------------------------------------------------------------
//	¥ DoMouseMoved
// ---------------------------------------------------------------------------
//	Handler for mouse moved events
//		[ kEventClassMouse, kEventMouseMoved ]

OSStatus
UCarbonEvents::DoMouseMoved(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	UCursor::SetArrow();
	
	return noErr;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ MakeEventRecord
// ---------------------------------------------------------------------------
//	Fill in fields of an EventRecord from an EventRef
//
//	We get the when, where, and modifiers fields from the EventRef.
//	The what and message fields are set to zero.

void
UCarbonEvents::MakeOldEventRecord(
	EventRef		inEventRef,
	EventRecord&	outOldEvent)
{
										// Initialize to default values

	outOldEvent.what	  = nullEvent;
	outOldEvent.message	  = 0;
	outOldEvent.when	  = ::EventTimeToTicks( ::GetEventTime(inEventRef) );
	outOldEvent.where.h	  = 0;
	outOldEvent.where.v	  = 0;
	outOldEvent.modifiers = 0;
						 
			// Try to get mouse location and modifier keys state from the
			// EventRef. We don't care if the calls fail. If the EventRef
			// does not have the requested parameter, the default values
			// set above will be used.
			
	::GetEventParameter( inEventRef,
						 kEventParamMouseLocation,
						 typeQDPoint, nil,
						 sizeof(Point), nil,
						 &outOldEvent.where );
						 
	UInt32	keyModifiers;				// kEventParamKeyModifiers is a UInt32

	::GetEventParameter( inEventRef,
						 kEventParamKeyModifiers,
						 typeUInt32, nil,
						 sizeof(UInt32), nil,
						 &keyModifiers );
						 				// EventModifiers is a UInt16
	outOldEvent.modifiers = (EventModifiers) keyModifiers;
}


// ---------------------------------------------------------------------------
//	¥ UpdateCommandMenuItem
// ---------------------------------------------------------------------------
//	Update the status of a menu item that has both a HICommand ID and
//	a PowerPlant command ID

void
UCarbonEvents::UpdateCommandMenuItem(
	UInt32		inHICommandID,
	CommandT	inPPCommand)
{
	LCommander*	theTarget = LCommander::GetTarget();
	
	if (theTarget == nil) return;
	
	MenuRef			macMenu = nil;	// Find first occurrence of the
	MenuItemIndex	itemIndex = 0;	//   HICommand in the menu bar
	
	OSStatus status = ::GetIndMenuItemWithCommandID( nil,
													 inHICommandID,
													 1,
													 &macMenu,
													 &itemIndex );
													 
	if (status != noErr) return;	// HICommand not found

	Boolean		isEnabled;			// Find status of PP command
	Boolean		usesMark = false;
	UInt16		mark;
	Str255		itemName;
	itemName[0] = 0;
	
	theTarget->ProcessCommandStatus( inPPCommand, isEnabled,
									 usesMark, mark, itemName );
	
	UpdateMenuItem( macMenu, itemIndex, isEnabled,
					usesMark, (CharParameter) mark, itemName );							 
}


// ---------------------------------------------------------------------------
//	¥ UpdateMenuItem
// ---------------------------------------------------------------------------

void
UCarbonEvents::UpdateMenuItem(
	MenuRef			inMacMenu,
	MenuItemIndex	inIndex,
	bool			inIsEnabled,
	bool			inUsesMark,
	CharParameter	inMarkChar,
	ConstStringPtr	inName)
{
	if (inIsEnabled) {
		::MacEnableMenuItem(inMacMenu, inIndex);
	} else {
		::DisableMenuItem(inMacMenu, inIndex);
	}
	
	if (inUsesMark) {
		::SetItemMark(inMacMenu, (short) inIndex, inMarkChar);
	}
	
	if (inName[0] > 0) {
		::SetMenuItemText(inMacMenu, (short) inIndex, inName);
	}
}


PP_End_Namespace_PowerPlant
