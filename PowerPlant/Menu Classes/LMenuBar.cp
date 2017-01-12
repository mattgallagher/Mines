// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenuBar.cp					PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages a Mac menu bar. Contains a list a Menu objects.
//
//	Resources:
//		'MBAR'		Standard Mac resource for a Menu Bar
//
//	ее Techniques
//
//	е Adding/Removing a Menu depending on runtime conditions
//		During initialization, create a Menu object
//		Store pointer to Menu object in an appropriate place
//		When you want to add the Menu to the MenuBar:
//			LMenuBar::GetCurrentMenuBar()->InstallMenu(myMenu, beforeID);
//			where "myMenu" is the pointer to the Menu object
//				"beforeID" is the ID of the Menu before which to put
//				this Menu (use InstallMenu_AtEnd to put the Menu last or
//				the Toolbox constant hierMenu for a hierarchical or popup)
//		When you want to remove the Menu from the MenuBar:
//			LMenuBar::GetCurrentMenuBar()->RemoveMenu(myMenu);
//
//	е Toggling a Menu item
//		For a menu item that toggles between two states (such as Show Toolbar
//		and Hide Toolbar), you can change the command as well as the text
//		of the menu item.
//
//		The follow code fragment assumes that you have declared oldCommand
//		and newCommand as CommandT variables or constants and newName as
//		as some kind of string.
//
//		ResIDT		theID;
//		MenuHandle	theMenuH;
//		SInt16		theItem;			// Locate oldCommand
//		FindMenuItem(oldCommand, theID, theMenuH, theItem);
//		if (theItem != 0) {				// Replace with newCommand
//			LMenu	*theMenu = LMenuBar::GetCurrentMenuBar->FetchMenu(theID);
//			theMenu->SetCommand(theItem, newCommand);
//			SetMenuItemText(theMenuH, theItem, newName);
//		}

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LMenuBar.h>
#include <LMenu.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UEnvironment.h>
#include <UMemoryMgr.h>

#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

LMenuBar*	LMenuBar::sMenuBar = nil;


// ---------------------------------------------------------------------------
//	е LMenuBar								Constructor				  [public]
// ---------------------------------------------------------------------------

LMenuBar::LMenuBar(
	ResIDT	inMBARid)
{
	sMenuBar		   = this;
	mMenuListHead	   = nil;
	mSupportsMenuEvent = UEnvironment::HasFeature(env_HasAppearance) &&
							CFM_AddressIsResolved_(::MenuEvent);
	mModifierKeys	   = cmdKey;

	StResource	theMBAR(ResType_MenuBar, inMBARid);
	::HLockHi(theMBAR.mResourceH);

									// Install each menu in the MBAR resource
	SInt16	*menuIDP = reinterpret_cast<SInt16 *>(*theMBAR.mResourceH);
	SInt16	numMenus = *menuIDP++;
	for (SInt16 i = 1; i <= numMenus; ++i) {
		try {
			InstallMenu(new LMenu(*menuIDP++), InstallMenu_AtEnd);
		}
									// If installing a Menu fails, keep
		catch (...) { }				//   looping
	}


	#if PP_Target_Classic
									// Populate the Apple Menu
									// Classic only. Carbon does this
									//   automatically.
		MenuHandle	macAppleMenuH = ::GetMenuHandle(MENU_Apple);
		if (macAppleMenuH != nil) {
			::AppendResMenu(macAppleMenuH, ResType_Driver);
		}
	#endif

	::InvalMenuBar();
}


// ---------------------------------------------------------------------------
//	е ~LMenuBar								Destructor				  [public]
// ---------------------------------------------------------------------------

LMenuBar::~LMenuBar()
{
	LMenu	*theMenu = nil;				// Loop through all Menus
	while (FindNextMenu(theMenu)) {
		RemoveMenu(theMenu);			// Remove Menu from MenuBar
		delete theMenu;					// Delete Menu object
		theMenu = nil;
	}
	
	if (this == sMenuBar) {
		sMenuBar = nil;
	}
}


// ---------------------------------------------------------------------------
//	е MenuCommandSelection											  [public]
// ---------------------------------------------------------------------------
//	Handle menu selection with the Mouse and return the command number for
//	the item chosen
//
//	When to Override:
//		To implement alternative menu selection behavior.
//		To change menu commands based on what modifier keys are down

CommandT
LMenuBar::MenuCommandSelection(
	const EventRecord&	inMouseEvent,
	SInt32&				outMenuChoice) const
{
	outMenuChoice = ::MenuSelect(inMouseEvent.where);

	CommandT	menuCmd = cmd_Nothing;
	if (HiWord(outMenuChoice) != 0) {
		menuCmd = FindCommand(HiWord(outMenuChoice), LoWord(outMenuChoice));
	}

	return menuCmd;
}


#pragma mark LMenuBar::SetModifierKeys
// ---------------------------------------------------------------------------
//	е SetModifierKeys												  [public]
// ---------------------------------------------------------------------------
//	void								// Defined inline
//	SetModifierKeys(
//		UInt16		inModifierKeys)
//
//	Specifies which modifier keys must be held down to qualify as a
//	potential menu key equivalent. Uses the Toolbox constants in
//	Events.h to specify modifier keys:
//
//		cmdKey, shiftKey, optionKey, controlKey
//
//	This applies only if the System supports the MenuEvent() call,
//	which was introduced with the Appearance Manager in Mac OS 8.0.
//	Without Appearance, all menu key equivalents must use the command key.
//
//	The default setting specifies that the command key be one of the
//	modifier keys pressed. For example, this allows cmd-K, cmd-option-K,
//	shift-cmd-K, etc.
//
//	Change this setting only if you wish to support key equivalents such
//	as option-K or control-K. Remember to include cmdKey as well. For example,
//
//		theMenuBar->SetModifierKeys( cmdKey + optionKey );
//
//	This will support cmd-K, option-K, control-option-K, etc. That is, any
//	keypress where either the cmd or option key is pressed.
//
//	To allow any keystroke (without any modifier key) to be a menu
//	equivalent, pass the Toolbox constant keyDownMask:
//
//		theMenuBar->SetModifierKeys(keyDownMask);


// ---------------------------------------------------------------------------
//	е FindKeyCommand												  [public]
// ---------------------------------------------------------------------------
//	Return the Command number corresponding to a keystroke
//		Returns cmd_Nothing if the keystroke is not a menu equivalent

CommandT
LMenuBar::FindKeyCommand(
	const EventRecord&	inKeyEvent,
	SInt32&				outMenuChoice) const
{
	CommandT	menuCmd = cmd_Nothing;
	outMenuChoice = 0;

	if (mSupportsMenuEvent) {		// Appearance System
		if ( (mModifierKeys & keyDownMask)  ||
			 (inKeyEvent.modifiers & mModifierKeys) ) {

			outMenuChoice = (SInt32) ::MenuEvent(&inKeyEvent);
		}

	} else {						// Pre-Appearance System
									// Must call MenuKey(), which requires
									//   that the command key be pressed
		if (inKeyEvent.modifiers & cmdKey) {
			char	theChar = (char) (inKeyEvent.message & charCodeMask);
			outMenuChoice = ::MenuKey(theChar);
		}
	}

	if (HiWord(outMenuChoice) != 0) {
									// Keystroke is a menu equivalent
									// Map menu choice to a command number
		menuCmd = FindCommand(HiWord(outMenuChoice), LoWord(outMenuChoice));
	}

	return menuCmd;
}


// ---------------------------------------------------------------------------
//	е FindCommand													  [public]
// ---------------------------------------------------------------------------
//	Return the Command number corresponding to a Menu (ID, item) pair

long
LMenuBar::FindCommand(
	ResIDT	inMENUid,
	SInt16	inItem) const
{
									// Start with synthetic command number
	CommandT	theCommand = -(((SInt32)inMENUid) << 16) - inItem;

	LMenu	*theMenu = mMenuListHead;
	while (theMenu) {				// Search all installed Menus
		if (inMENUid == theMenu->GetMenuID()) {
			theCommand = theMenu->CommandFromIndex(inItem);
			break;
		}
		theMenu = theMenu->GetNextMenu();
	}

	return theCommand;
}


// ---------------------------------------------------------------------------
//	е FindMenuItem													  [public]
// ---------------------------------------------------------------------------
//	Passes back the MENU id, MenuHandle, and item number corresponding to a
//	Command number
//
//	If the Command is not associated with any item in the MenuBar,
//		outMENUid is 0, outMenuHandle is nil, and outItem is 0

void
LMenuBar::FindMenuItem(
	CommandT		inCommand,
	ResIDT&			outMENUid,
	MenuHandle&		outMenuHandle,
	SInt16&			outItem) const
{
	SInt16	theItem = 0;			// Search menu list for the command
	LMenu	*theMenu = mMenuListHead;
	while (theMenu) {
		theItem = theMenu->IndexFromCommand(inCommand);
		if (theItem != 0) {
			break;
		}
		theMenu = theMenu->GetNextMenu();
	}

	if (theItem != 0) {				// Command found, get ID and MenuHandle
		outMENUid = theMenu->GetMenuID();
		outMenuHandle = theMenu->GetMacMenuH();

	} else {						// Command not found
		outMENUid = 0;
		outMenuHandle = nil;
	}
	outItem = theItem;
}


// ---------------------------------------------------------------------------
//	е FindNextCommand												  [public]
// ---------------------------------------------------------------------------
//	Passes back the next command in the MenuBar
//
//	On entry,
//		ioIndex, ioMenuHandle, and ioMenu specify an item in a Menu
//		ioMenuHandle of nil means to start at the beginning, so the
//		next command will be the first one in the MenuBar
//	On exit,
//		ioIndex, ioMenuHandle, and ioMenu specify the next item in
//		the MenuBar. If the next item is in the same menu, ioIndex
//		is incremented by one and ioMenuHandle and ioMenu are unchanged.
//		If the next item is in another menu, ioIndex is one, and
//		ioMenuHandle and ioMenu refer to the next menu.
//		outCommand is the command number associated with that item
//
//	Returns true if the next command exists
//	Returns false if there is no next command
//
//	Use this function to iterate over all commands in the MenuBar:
//
//		LMenuBar	*theMenuBar = LMenuBar::GetCurrentMenuBar();
//		SInt16		menuItem;
//		MenuHandle	macMenuH = nil;
//		LMenu		*theMenu;
//		CommandT	theCommand;
//
//		while (theMenuBar->FindNextCommand(menuItem, macMenuH,
//											theMenu, theCommand)) {
//			// Do something with theCommand
//		}

bool
LMenuBar::FindNextCommand(
	SInt16&			ioIndex,
	MenuHandle&		ioMenuHandle,
	LMenu*			&ioMenu,
	CommandT&		outCommand) const
{
	if (ioMenuHandle == nil) {		// Special case: first time
		ioIndex = 0;				//   Start at beginning of our Menu list
		ioMenu = mMenuListHead;
		if (ioMenu == nil) {
			return false;			// Quick exit if there are no Menus
		}
	}

	bool		cmdFound;
	do {
									// Get MenuHandle for current Menu
		ioMenuHandle = ioMenu->GetMacMenuH();
									// Search in current Menu
		cmdFound = ioMenu->FindNextCommand(ioIndex, outCommand);

		if (!cmdFound) {			// No next command in current Menu
			ioIndex = 0;			// Move to start of next Menu
			ioMenu = ioMenu->GetNextMenu();
		}
									// End search upon finding next command
									//   or reaching end of menu list
	} while (!cmdFound && (ioMenu != nil));

	return cmdFound;
}


// ---------------------------------------------------------------------------
//	е InstallMenu													  [public]
// ---------------------------------------------------------------------------
//	Install a Menu object in the MenuBar

void
LMenuBar::InstallMenu(
	LMenu	*inMenuToInstall,
	ResIDT	inBeforeMENUid)
{
		// It's possible to add a Menu twice to the Mac menu list--
		// once as a regular menu and once as a submenu (hierarchical
		// or popup menu). However, we only need one copy of the
		// menu in our menu list.

	if (!inMenuToInstall->IsInstalled()) {
									// Add it to our singly-linked list
		inMenuToInstall->SetNextMenu(mMenuListHead);
		mMenuListHead = inMenuToInstall;
		inMenuToInstall->SetInstalled(true);
	}

									// Add it to the Mac MenuBar
	MenuHandle	macMenuH = inMenuToInstall->GetMacMenuH();
	::MacInsertMenu(macMenuH, inBeforeMENUid);

									// Search menu items for submenus
									//   and install them also
	SInt16	itemCount = (SInt16) ::CountMenuItems(macMenuH);
	for (SInt16 item = 1; item <= itemCount; item++) {
		SInt16	itemCmd;
		::GetItemCmd(macMenuH, item, &itemCmd);
		if (itemCmd == hMenuCmd) {
			SInt16	subMenuID;		// Submenu found. Get its ID, create
									//   a new Menu object, and recursively
									//   call this function to install it.
									//   Recursion means that sub-submenus
									//   get installed too.
			::GetItemMark(macMenuH, item, &subMenuID);

				// Creating an LMenu for a menu ID that does not exist will
				// throw an exception. If this happens, we should just
				// continue. Users wishing to know about this (it's most
				// likely a typo in the MENU resource or a missing MENU
				// for the submenu) should set gDebugThrow appropriately.
				//
				// However, some extensions dynamically insert hierarchical
				// menus by patching _InsertMenu. In these cases, it's OK
				// not to find the submenu.

			try {
				InstallMenu(new LMenu(subMenuID), hierMenu);
			}

			catch (...) { }
		}
	}

	if (inBeforeMENUid != hierMenu) {
									// Not a submenu, so force a redraw
		::InvalMenuBar();			//   of the MenuBar
	}
}


// ---------------------------------------------------------------------------
//	е RemoveMenu													  [public]
// ---------------------------------------------------------------------------
//	Remove a Menu object from the MenuBar

void
LMenuBar::RemoveMenu(
	LMenu	*inMenuToRemove)
{
									// Search for Menu in our list
	LMenu	*theMenu = mMenuListHead;
	LMenu	*prevMenu = nil;
	while ((theMenu != nil) && (theMenu != inMenuToRemove)) {
		prevMenu = theMenu;
		theMenu = theMenu->GetNextMenu();
	}

	if (theMenu != nil) {			// Menu is in our list
									// Remove it from our singly-linked list
		if (prevMenu == nil) {
			mMenuListHead = inMenuToRemove->GetNextMenu();
		} else {
			prevMenu->SetNextMenu(inMenuToRemove->GetNextMenu());
		}
		inMenuToRemove->SetNextMenu(nil);
		inMenuToRemove->SetInstalled(false);

									// Remove it from the Mac MenuBar
		::MacDeleteMenu(inMenuToRemove->GetMenuID());
		::InvalMenuBar();				// Force redraw of MenuBar
									// ??? don't redraw if a submenu ???
	}
}


// ---------------------------------------------------------------------------
//	е FetchMenu														  [public]
// ---------------------------------------------------------------------------
//	Return the Menu object for the specified MENU resource ID
//
//	Returns nil if there is no such Menu object

LMenu*
LMenuBar::FetchMenu(
	ResIDT	inMENUid) const
{
	LMenu	*theMenu = mMenuListHead;
									// Search menu list until reaching the
									//   end or finding a match
	while ((theMenu != nil) && (theMenu->mMENUid != inMENUid)) {
		theMenu = theMenu->GetNextMenu();
	}
	return theMenu;
}


// ---------------------------------------------------------------------------
//	е FindNextMenu													  [public]
// ---------------------------------------------------------------------------
//	Pass back the next Menu in a MenuBar
//
//	On entry, ioMenu is a pointer to a Menu object
//		Pass nil to get the first Menu
//	On exit, ioMenu is a pointer to the next Menu in the MenuBar
//		ioMenu is nil if there is no next menu
//
//	Returns false if there is no next menu
//
//	Use this function to loop thru all the Menus in a MenuBar:
//		LMenuBar	*theMenuBar = LMenuBar::GetCurrentMenuBar();
//		LMenu	*theMenu = nil;
//		while (theMenuBar->FindNextMenu(theMenu) {
//			// ... do something with theMenu
//		}

bool
LMenuBar::FindNextMenu(
	LMenu*	&ioMenu) const
{
	if (ioMenu == nil) {
		ioMenu = mMenuListHead;

	} else {
		ioMenu = ioMenu->GetNextMenu();
	}

	return (ioMenu != nil);
}

#pragma mark -

// ===========================================================================
//	StUnhiliteMenu
// ===========================================================================
//	Stack-based class to unhilite the currently selected menu title

StUnhiliteMenu::StUnhiliteMenu()
{
	mStartTicks = ::TickCount();
}


StUnhiliteMenu::~StUnhiliteMenu()
{
									// Delay unhiliting long enough for
									//   user to notice it
	while ( ::TickCount() < (mStartTicks + delay_Feedback) ) ;

	::HiliteMenu(0);
}

PP_End_Namespace_PowerPlant
