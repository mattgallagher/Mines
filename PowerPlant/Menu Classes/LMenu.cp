// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenu.cp					PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages a Mac menu. Maintains a mapping between Menu items and
//	Command numbers.
//
//	Dependencies:
//		LMenuBar (declared as a friend class in the interface)
//
//	Resources:
//		'MENU'		Standard Mac resource for a Menu
//		'Mcmd'		Command numbers for each item in the Menu
//
//	ее Command Numbers
//	Commands are 32-bit, signed integer values.
//	PowerPlant reserves command numbers -999 to +999.
//
//		е Synthetic Command Numbers
//		A synthetic command number has the Menu ID in the high 16 bits
//		and the item number in the low 16 bits, with the result negatated.
//			syntheticCmd = - (MenuID << 16) - ItemNumber
//		A synthetic command is the negative of the value returned by
//		the Toolbox traps MenuSelect and MenuKey.
//		You can extract the components as follows:
//			MenuID = HiWord(-syntheticCmd);
//			ItemNumber = LoWord(-syntheticCmd);
//		Alternatively, the LCommander class has an IsSyntheticCommand
//		static function that extracts this information.
//
//		You must use Menu ID numbers between 1 and 32767. Therefore, a
//		command number is synthetic if it is negative and the hi word
//		of its negation is not zero (which would correspond to a zero
//		Menu ID).
//
//		Synthetic commands numbers range from $80000000 (Menu ID 32767)
//		to $FFFF0000 (Menu ID 1).
//
//		е Negative command numbers
//		Negative command numbers from $FFFF0001 to $FFFFFFFF (-65535 to -1)
//		are not valid synthetic command numbers. Programs can use command
//		numbers in this range. (As mentioned above PowerPlant reserves
//		-999 to +999). However, the PowerPlant Application class treats
//		positive and negative command numbers differently.
//
//		The Application class updates the enabled state of menu items
//		with positive command numbers after every event when the
//		"update command status" flag is on. It does not update the state
//		of menu items with negative command numbers. You might want to use
//		a negative command number for a menu item that is always enabled
//		(except when its entire menu is disabled; disabling an entire menu
//		overrides the settings of individual menu items).
//
//	ее Command Table
//	The command table (mCommandNums) is a handle of command numbers. The
//	table always starts with the first menu item. For example, if
//	mNumCommands is 6, then the commands in the table map to items 1 to 6.
//
//	ее CommandFromIndex
//	CommandFromIndex returns the command for a particular menu item. It
//	returns a synthetic command number in two cases:
//		1) The menu item is greater than the size of the command table.
//		This happens when you add menu items at runtime (for example, by
//		calling AppendResMenu for a Font menu or maintaining a Windows menu).
//
//		2) The item has a command of cmd_UseMenuItem (value of -1).
//
//	ее Using the Toolbox Menu Manager
//	You should use direct Toolbox traps for all menu operations except
//	adding and removing items that have command numbers. You can get the
//	MenuHandle for a Menu object from the GetMacMenuH function.
//
//	Note that is OK to add and remove items that don't have command numbers.
//	For example, the Windows menu in some programs have some fixed items
//	at the top, then a variable list of the names of open windows. Using
//	LMenu, only the fixed items would have command numbers. The items for
//	the open windows would not have command numbers, so you can call
//	the Toolbox traps MacInsertMenuItem and DeleteMenuItem for those items.
//	For items with command numbers, you must use the LMenu functions
//	InsertCommand, RemoveCommand, and RemoveItem.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LMenu.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UEnvironment.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	е LMenu									Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMenu::LMenu()
{
	mMENUid		 = resID_Undefined;
	mNextMenu	 = nil;
	mNumCommands = 0;
	mCommandNums = nil;
	mIsInstalled = false;
	mUsed		 = false;
	mMacMenuH	 = nil;
}


// ---------------------------------------------------------------------------
//	е LMenu									Constructor from MENU	  [public]
// ---------------------------------------------------------------------------
//	This is equivalent to the Toolbox MacGetMenu() call

LMenu::LMenu(
	ResIDT	inMENUid)
{
	mMENUid		 = inMENUid;
	mNextMenu	 = nil;
	mNumCommands = 0;
	mCommandNums = nil;
	mIsInstalled = false;
	mUsed		 = false;

		// On Carbon, MacGetMenu() will return a unique copy of
		// the menu, initialized with data from the MENU resource.
		// On Classic, it will return the existing MenuHandle if
		// a menu with the same ID has already been created.

	mMacMenuH = ::MacGetMenu(inMENUid);

	ThrowIfNil_(mMacMenuH);				// MENU not found

		// PowerPlant requires that the MENU resource ID be the same
		// as the Menu ID stored in the resource data. Signal an error
		// if they are different.

	SignalIf_(inMENUid != ::GetMenuID(mMacMenuH));

	ReadCommandNumbers();
}


// ---------------------------------------------------------------------------
//	е LMenu
// ---------------------------------------------------------------------------
//	Construct Menu object from a ID and a Title
//
//	This is equivalent to the Toolbox NewMenu() call
//
//	inAlwaysThemeSavvy specifies if the menu is Appearance theme-savvy
//	regardless of the "Sytem-wide Platinum Appearance" setting in the
//	Appearance control panel. The default value of this parameter is false.

LMenu::LMenu(
	SInt16			inMENUid,
	ConstStringPtr	inTitle,
	bool			inAlwaysThemeSavvy)
{
	mMENUid		 = inMENUid;
	mNextMenu	 = nil;
	mNumCommands = 0;
	mCommandNums = nil;
	mIsInstalled = false;

	mMacMenuH = MakeNewMacMenu(inMENUid, inTitle, inAlwaysThemeSavvy);
}


// ---------------------------------------------------------------------------
//	е ~LMenu								Destructor				  [public]
// ---------------------------------------------------------------------------

LMenu::~LMenu()
{
	if (mMacMenuH != nil) {

		// Error to delete Menu without first de-installing it.
		// De-installing is *not* the same as removing the Menu from
		// the Mac MenuBar. An installed Menu is under the control
		// of a client (usually some kind of MenuBar class), and can
		// not be deleted until that client has relinquished control.
		// Since the Menu object has no knowledge of its clients, we
		// can't notify them. All we can do is signal an error.

		SignalIf_(mIsInstalled);

		::MacDeleteMenu(mMENUid);			// Remove Menu from Mac MenuBar

		#if TARGET_API_MAC_CARBON

			::DisposeMenu(mMacMenuH);

		#else

			// Free MenuHandle. It could be a resource (from calling
			// MacGetMenu) or just a Handle (from calling NewMenu).
			// Note: Menu is never a Handle on Carbon.

			if (::HGetState((Handle) mMacMenuH) & 0x20) {
											// Bit 5 is set. It's a resource.
				::ReleaseResource((Handle) mMacMenuH);
			} else {
				::DisposeMenu(mMacMenuH);
			}
		#endif
	}

	if (mCommandNums != nil) {			// Free command number table
		::DisposeHandle((Handle) mCommandNums);
	}
}


// ---------------------------------------------------------------------------
//	е ReadCommandNumbers
// ---------------------------------------------------------------------------
//	Get command numbers from the 'Mcmd' resource
//
//	A 'Mcmd" resource with the same ID as the 'MENU' resource contains a
//	list of command numbers. The format of a 'Mcmd' resource is:
//		Number of Commands: n		(2 bytes)
//		Command Num for Item 1		(4 bytes)
//		   ...
//		Command Num for Item n		(4 bytes)

void
LMenu::ReadCommandNumbers()
{
	SInt16**	theMcmdH = (SInt16**) ::GetResource(ResType_MenuCommands, mMENUid);

	if (theMcmdH != nil) {
		if (::GetHandleSize((Handle) theMcmdH) > 0) {
			mNumCommands = (*theMcmdH)[0];
			if (mNumCommands > 0) {

					// Our command numbers list is the same as the 'Mcmd'
					// resource without the 2-byte count at the top. So we
					// can reuse the 'Mcmd' resource Handle by detaching it,
					// shifting the command numbers down by 2-bytes, and
					// resizing it.

				::DetachResource((Handle) theMcmdH);
				::HNoPurge((Handle) theMcmdH);
				mCommandNums = (SInt32**) theMcmdH;
				SInt32	commandsSize = mNumCommands * (SInt32) sizeof(SInt32);
				::BlockMoveData(*theMcmdH+1, *mCommandNums, commandsSize);
				::SetHandleSize((Handle) mCommandNums, commandsSize);
			}
		} else {
			::ReleaseResource((Handle) theMcmdH);
		}
	}
}


// ---------------------------------------------------------------------------
//	е CommandFromIndex
// ---------------------------------------------------------------------------
//	Return the command number for a particular Menu item

CommandT
LMenu::CommandFromIndex(
	SInt16	inIndex) const
{
									// Table lookup if index is in range
	SInt32	theCommand = cmd_UseMenuItem;
	if (inIndex <= mNumCommands) {
		theCommand = (*mCommandNums)[inIndex-1];
	}

	if (theCommand == cmd_UseMenuItem) {

			// Command number is a special flag, either because it
			// was set that way in the command table or because the
			// index was greater than the command table size.

			// In this case, we return a synthetic command number,
			// which has the MENU id and item number embedded

		theCommand = SyntheticCommandFromIndex(inIndex);
	}

	return theCommand;
}


// ---------------------------------------------------------------------------
//	е SyntheticCommandFromIndex
// ---------------------------------------------------------------------------
//	Return the synthetic command number for a particular Menu item
//
//	A synthetic command number has the MENU id in the high 16 bits and
//	the item number in the low 16 bits, and the resulting 32-bit number
//	is negated (to distinguish it from regular command numbers). The
//	synthetic command number is the negative of the value that would be
//	returned by the Toolbox trap MenuSelect for the menu item.

CommandT
LMenu::SyntheticCommandFromIndex(
	SInt16	inIndex) const
{
	return (-(((SInt32)mMENUid) << 16) - inIndex);
}


// ---------------------------------------------------------------------------
//	е IndexFromCommand
// ---------------------------------------------------------------------------
//	Return the Menu item index number for a particular command number
//		Return 0 if the command number is not used for this Menu

SInt16
LMenu::IndexFromCommand(
	CommandT	inCommand) const
{
		// Search for Command number in Command list for this Menu

	if (mCommandNums != nil) {
		CommandT	*commandsP = *mCommandNums;
		for (SInt16 theIndex = 0; theIndex < mNumCommands; theIndex++) {
			if (inCommand == *commandsP++) {
				return (SInt16) (theIndex + 1);
			}
		}
	}

		// Command not found in Command list
		// If it is a synthetic command, extract the index number from
		//   the low 16 bits of the command number

	if (((SInt16) ((-inCommand) >> 16)) > 0) {
		return (SInt16) (-inCommand);
	}

	return 0;				// Command not found
}


// ---------------------------------------------------------------------------
//	е FindNextCommand
// ---------------------------------------------------------------------------
//	Pass back the next command in the Menu
//		On entry, ioIndex is the position of an item (0 is allowed)
//		On exit, ioIndex is the position of the next item
//			If ioIndex is greater than the number of commands in the Menu
//				function returns false (and outCommand is unchanged).
//			Otherwise, function returns true and returns the command number
//				of the item after the input index
//
//	Use this function to iterate over all commands in a Menu:
//		SInt16	index = 0;
//		SInt32	command;
//		while (theMenu->FindNextCommand(index, command)) {
//			// Do something with command
//		}

bool
LMenu::FindNextCommand(
	SInt16		&ioIndex,
	CommandT	&outCommand) const
{
	if (ioIndex < 0) {					// Safety check for negative items
		ioIndex = 0;					// Set to zero so index is valid
	}

	bool	cmdFound = false;
	if (ioIndex < mNumCommands) {		// Index is in range
										// Lookup command number
		outCommand = (*mCommandNums)[ioIndex++];
		cmdFound = true;
	}

	return cmdFound;
}


// ---------------------------------------------------------------------------
//	е SetCommand
// ---------------------------------------------------------------------------
//	Set the command number for a Menu item

void
LMenu::SetCommand(
	SInt16		inIndex,
	CommandT	inCommand)
{
									// Do nothing if inIndex is out of range
	SInt16	menuLength = (SInt16) ::CountMenuItems(mMacMenuH);
	if ((inIndex > 0)   &&  (inIndex <= menuLength)) {
		if (inIndex > mNumCommands) {
									// Setting command for item beyond end
									//   end of command list

									// Grow command list
			if (mCommandNums == nil) {
				mCommandNums = (SInt32**) ::NewHandle(
											inIndex * (Size) sizeof(CommandT));
				ThrowIfMemFail_(mCommandNums);
			} else {
				::SetHandleSize((Handle) mCommandNums,
								inIndex * (Size) sizeof(CommandT));
				ThrowIfMemError_();
			}

			SInt16	origNumCommands = mNumCommands;
			mNumCommands = inIndex;

			if (inIndex > origNumCommands + 1) {

				// There are items between the last original command
				// and the one to set. We must set the command for
				// these items to cmd_UseMenuItem.
				// Example:
				//		3 items in command list originally
				//		SetCommand for item 6
				//		Items 4, 5, 6 previously had no entry in command list
				//			(command list was shorter than number of items
				//			in the Menu)
				//		Set command for items 4 and 5 to cmd_UseMenuItem

				SInt16		newDefaults = (SInt16) (inIndex - origNumCommands - 1);
				CommandT	*cp = *mCommandNums + origNumCommands;
				do {
					*cp++ = cmd_UseMenuItem;
				} while (--newDefaults);
			}
		}
									// Store command for this item
		(*mCommandNums)[inIndex - 1] = inCommand;
	}
}


// ---------------------------------------------------------------------------
//	е InsertCommand
// ---------------------------------------------------------------------------
//	Insert an item with the specified text and command number after a
//	particular item
//
//	NOTE: This function does not support adding multiple menu items
//		using "Return" or "Semicolon" characters within inItemText

void
LMenu::InsertCommand(
	ConstStringPtr	inItemText,
	CommandT		inCommand,
	SInt16			inAfterItem)
{
									// Determine insertion index. It's
									//   usually one plus the after index.
	SInt16	menuLength = (SInt16) ::CountMenuItems(mMacMenuH);
	SInt16	itemToInsert = (SInt16) (inAfterItem + 1);
	if (itemToInsert < 1) {
		itemToInsert = 1;			// Insert at beginning of Menu
	} else if (itemToInsert > menuLength) {
									// Append to end of Menu
		itemToInsert = (SInt16) (menuLength + 1);
	}
									// Put item in Mac Menu
	::MacInsertMenuItem(mMacMenuH, inItemText, (SInt16) (itemToInsert - 1));

	if (itemToInsert <= mNumCommands) {
									// Inserting into middle of list
									// Command count increases by one
									//   and command table grows
		::SetHandleSize((Handle) mCommandNums,
			(mNumCommands + 1) * (Size) sizeof(CommandT));
		ThrowIfMemError_();
		mNumCommands++;
									// Shift up commands beyond
									//    insertion point
		::BlockMoveData(*mCommandNums + itemToInsert - 1,
				  *mCommandNums + itemToInsert,
				  (mNumCommands - itemToInsert) * (Size) sizeof(CommandT));
	}
									// Store command for inserted item
	SetCommand(itemToInsert, inCommand);
}


// ---------------------------------------------------------------------------
//	е RemoveCommand
// ---------------------------------------------------------------------------
//	Remove the Menu item with the specified command

void
LMenu::RemoveCommand(
	CommandT	inCommand)
{
										// Lookup index for command
	SInt16	itemToRemove = IndexFromCommand(inCommand);
	if (itemToRemove != 0) {			// Command found
		RemoveItem(itemToRemove);		// Remove associated menu item
	}
}


// ---------------------------------------------------------------------------
//	е RemoveItem
// ---------------------------------------------------------------------------
//	Remove the Menu item at the specified position

void
LMenu::RemoveItem(
	SInt16	inItemToRemove)
{
	SignalIf_(inItemToRemove <= 0);

										// Remove item from Mac Menu
	::DeleteMenuItem(mMacMenuH, inItemToRemove);

	if (inItemToRemove <= mNumCommands) {
										// Item to remove has a command
										// Shift down commands above the
										//   item to remove
		::BlockMoveData(*mCommandNums + inItemToRemove,
				  *mCommandNums + inItemToRemove - 1,
				  (mNumCommands - inItemToRemove) * (Size) sizeof(CommandT));

										// Decrease count of commands and
										//   shrink command table
		::SetHandleSize((Handle) mCommandNums,
						--mNumCommands * (Size) sizeof(CommandT));
	}
}


// ---------------------------------------------------------------------------
//	е ItemIsEnabled
// ---------------------------------------------------------------------------
//	Return whether an item at the specified position is enabled

bool
LMenu::ItemIsEnabled(
	SInt16	inIndex) const				// One-based index
{
#if TARGET_API_MAC_CARBON

	return ::IsMenuItemEnabled(mMacMenuH, (UInt16) inIndex);

#else

	bool	enabled = true;
	if (inIndex < 32) {
		enabled = ((**mMacMenuH).enableFlags & (1L << inIndex)) != 0;
	}
	return enabled;

#endif
}


// ---------------------------------------------------------------------------
//	е EnableItem
// ---------------------------------------------------------------------------

void
LMenu::EnableItem(
	SInt16	inIndex)
{
#if TARGET_API_MAC_CARBON

	::MacEnableMenuItem(mMacMenuH, (MenuItemIndex) inIndex);

#else

	::EnableItem(mMacMenuH, inIndex);

#endif
}


// ---------------------------------------------------------------------------
//	е DisableItem
// ---------------------------------------------------------------------------

void
LMenu::DisableItem(
	SInt16	inIndex)
{
#if TARGET_API_MAC_CARBON

	::DisableMenuItem(mMacMenuH, (MenuItemIndex) inIndex);

#else

	::DisableItem(mMacMenuH, inIndex);

#endif
}


// ---------------------------------------------------------------------------
//	е MakeNewMacMenu										 [static] [public]
// ---------------------------------------------------------------------------

MenuHandle
LMenu::MakeNewMacMenu(
	SInt16			inMENUid,
	ConstStringPtr	inTitle,
	bool			inAlwaysThemeSavvy)
{
	MenuHandle	menuH = ::NewMenu(inMENUid, inTitle);
	ThrowIfNil_(menuH);

#if TARGET_API_MAC_CARBON

	#pragma unused(inAlwaysThemeSavvy)

#else

	if (inAlwaysThemeSavvy) {

		// The following code is from Apple's Appearance SDK sample code

		if ( UEnvironment::HasFeature(env_HasAppearance101) ) {

			// ::NewMenu() always uses MDEF 0. We create a theme-savvy
			// menu by changing the defproc in the MenuHandle to the
			// theme MDEF.

			Handle themeDefProc = ::GetResource( ResType_MenuDefProc,
												 kMenuStdMenuProc);

			if (themeDefProc != nil) {
				(*menuH)->menuProc = themeDefProc;
			}
		}
	}

#endif

	return menuH;
}

#pragma mark -

// ===========================================================================
//	StMenuHandleDisposer
// ===========================================================================
//	Stack-based class for disposing of a MenuHandle

StMenuHandleDisposer::StMenuHandleDisposer(
	MenuHandle		inMenuH)
{
	mMacMenuH = inMenuH;
}


StMenuHandleDisposer::~StMenuHandleDisposer()
{
	if (mMacMenuH != nil) {
		::MacDeleteMenu(::GetMenuID(mMacMenuH));	// Remove from menu list

		#if PP_Target_Carbon			// Menu is never a Handle on Carbon

			::DisposeMenu(mMacMenuH);

		#else

			// How to dispose the MenuHandle depends on whether it
			// was created via MacGetMenu() [it's a MENU Resource] or
			// via NewMenu() [it's a regular Handle].

			// NOTE: Menus must not be purgeable, so we don't have
			// to worry about HGetState() failing for a purged Handle.

			if (::HGetState((Handle)mMacMenuH) & 0x20) {
				::ReleaseResource((Handle)mMacMenuH);

			} else {
				::DisposeMenu(mMacMenuH);
			}

		#endif
	}
}


PP_End_Namespace_PowerPlant
