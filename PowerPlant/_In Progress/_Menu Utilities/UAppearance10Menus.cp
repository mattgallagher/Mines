// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAppearance10Menus.cp		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Part of PowerPlant's menu utility classes to ease the support of Menu
//	Manager 1.0, Menu Manager 2.0, Appearance 1.0.x and 1.1, and other
//	continuing developments of the Mac OS Menu Manager.
//
//	This file implements support for the Menu Manager that was introduced
//	by Appearance 1.0.x (in Mac OS 8). No checks are made in the code to
//	ensure the Appearance Manager is present. You must perform this
//	check (or use UConditionalMenus instead).

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <UAppearance10Menus.h>

#if TARGET_API_MAC_CARBON

		// Carbon does not support Appearance 1.0. You must use
		// UAppearance Menus instead. Remove this file from Carbon targets.

	#error "Carbon does not support Appearance 1.0 Menus"

#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ EnableItem
// ---------------------------------------------------------------------------
//	Enables the given menu item. Pass zero to enable the entire menu.
//
//	Caller is responsible for ensuring the menubar is properly updated
//	(calling ::InvalMenuBar() or ::DrawMenuBar())

void
UAppearance10Menus::EnableItem(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem )
{
	::EnableItem(inMacMenuH, static_cast<SInt16>(inMenuItem));
}


// ---------------------------------------------------------------------------
//	¥ DisableItem
// ---------------------------------------------------------------------------
//	Disables the given menu item. Pass zero to disable the entire menu.
//
//	Caller is responsible for ensuring the menubar is properly updated
//	(calling ::InvalMenuBar() or ::DrawMenuBar())

void
UAppearance10Menus::DisableItem(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem )
{
	::DisableItem(inMacMenuH, static_cast<SInt16>(inMenuItem));
}


// ---------------------------------------------------------------------------
//	¥ IsItemEnabled
// ---------------------------------------------------------------------------
//	Returns true (false) if the given menu item is enabled (disabled)

bool
UAppearance10Menus::IsItemEnabled(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem )
{
	// Item is enabled if:
	//   Entire menu is enabled  AND
	//      (  (Item > 31)  OR  (Bit #Item in flag is set) )

	return ((*inMacMenuH)->enableFlags & 0x00000001)  &&
			( (inMenuItem > 31)  ||
			((*inMacMenuH)->enableFlags & (1L << inMenuItem)) );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetItemHierID
// ---------------------------------------------------------------------------
//	For the given menu item, establish it as having a submenu.
//
//	This only wraps up some toolbox routines. It does NOT actually insert
//	the submenu. You should use LMenuBar::InstallMenu() to do this, e.g.:
//
//		// Get the MenuHandle to our "super-menu"
//		MenuHandle	mainMenuH = theMainLMenu->GetMacMenuH();
//
//		// create (from a 'MENU' resource, ID #200)
//		LMenu		*theSubMenu = new LMenu(200);
//
//		// Set the first item in the menu to be the one with the submenu
//		UMenus::SetItemHierID(mainMenuH, 1, theSubMenu->GetMenuID());
//
//		// install the submenu
//		LMenuBar::GetCurrentMenuBar()->InstallMenu(theSubMenu, hierMenu);
//

void
UAppearance10Menus::SetItemHierID(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem,
	ResIDT			inMenuID )
{
	OSErr err = ::SetMenuItemHierarchicalID(inMacMenuH, (SInt16) inMenuItem, inMenuID );
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetItemHierID
// ---------------------------------------------------------------------------
//	Obtain the menu ID for a given menu item's submenu

SInt16
UAppearance10Menus::GetItemHierID(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem )
{
	SInt16	theID = 0;

	OSErr err = ::GetMenuItemHierarchicalID(inMacMenuH, (SInt16) inMenuItem, &theID );
	ThrowIfOSErr_(err);

	return theID;
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ SetItemTextEncoding
// ---------------------------------------------------------------------------
//	Sets the text encoding for the specified menu item's text.

void
UAppearance10Menus::SetItemTextEncoding(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem,
	TextEncoding	inEncoding )
{
	OSErr err = ::SetMenuItemTextEncoding(inMacMenuH, (SInt16) inMenuItem, inEncoding);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetItemTextEncoding
// ---------------------------------------------------------------------------
//	Gets the text encoding for the specified menu item's text

TextEncoding
UAppearance10Menus::GetItemTextEncoding(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem )
{
	TextEncoding	theEncoding = 0;

	OSErr err = ::GetMenuItemTextEncoding(inMacMenuH, (SInt16) inMenuItem, &theEncoding);
	ThrowIfOSErr_(err);

	return theEncoding;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetItemKeyEquivalent
// ---------------------------------------------------------------------------
//	Allows one to specify the keyboard equivalent for the given menu item.
//
//	One can specify the key (N for New, X for Cut), the modifiers (cmd,
//	option, control, shift), and an optional glyph (a symbol representing
//	the key, e.g. "F1", useful for "unprintables").
//
//	inMacMenuH is a valid MenuHandle to the MENU you wish to change
//
//	inMenuItem is the item of inMacMenuH you wish to change. No range
//	checking is performed to ensure inMenuItem is within bounds.
//
//	inKey is the target key. You can specify zero to clear the keyboard
//	equivalent for this item. If zero is specified, inModifiers and
//	inGlyph are ignored. The Appearance Manager allows "unprintable"
//	characters to be used as key equivs (delete, return, F-keys, arrows,
//	etc.). If you specify one of these, you MUST ensure inGlyph is set
//	to the cooresponding symbol (the raw hex values do not necessarily
//	match). No check is made to ensure these values match. Furthermore,
//	if you specify an unprintable and the Appearance Manager is not
//	present, your mileage may vary....
//
//	inModifiers contains the modifiers (cmd, shift, option, control). The
//	command key is assumed by default. You should utilize the Appearance
//	modifier constants (kMenuNoModifiers, kMenuShiftModifier,
//	kMenuOptionModifier, kMenuControlModifier, kMenuNoCommandModifier). If
//	you wish no command key, you must explicitly remove it. There is
//	no way to remove the command key if Appearance is not present.
//	Furthermore, if Appearance is not present, only the command key will
//	be used (in essence, if no Appearance and inKey != 0, this field
//	is essentially ignored and cmdKey is assumed).
//
//	inGlyph specifies the printed glyph for the key equivalent. This is
//	only necessary under the Appearance Manager (otherwise it is ignored
//	and the inKey is used as the glyph). If zero is specified (default),
//	inKey will be used as the glyph.
//
//	See "Inside Macintosh: Mac OS 8 Toolbox Reference"'s chapter on the
//	Menu Manager for more information.

void
UAppearance10Menus::SetItemKeyEquivalent(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem,
	SInt16			inKey,
	UInt8			inModifiers,
	SInt16			inGlyph )
{
	// set the command.

	::SetItemCmd( inMacMenuH, (SInt16) inMenuItem, inKey );

	if ( inKey != 0 ) {
		// set the modifiers

		OSErr err = ::SetMenuItemModifiers( inMacMenuH, (SInt16) inMenuItem, inModifiers );
		ThrowIfOSErr_(err);

		// set the glyph

		if ( inGlyph == 0 ) {
			inGlyph = inKey;
		}

		err = ::SetMenuItemKeyGlyph( inMacMenuH, (SInt16) inMenuItem, inGlyph );
		ThrowIfOSErr_(err);
	}
}


// ---------------------------------------------------------------------------
//	¥ÊGetItemKeyEquivalent
// ---------------------------------------------------------------------------
//	For the given menu item, return the various keyboard equivalent
//	information.
//
//	If outKey is non-zero, this specifies the key for the equivalent.
//	If it is zero, there is no equivalent for this item. The values
//	returned in outModifiers and outGlyph should be ignored.
//
//	If outModifiers is zero (and outKey is non-zero), then the command
//	key is the modifier (consistant with how the toolbox and the
//	kMenuModifers constants are setup).
//
//	If outKey is non-zero, outGlyph will specify the glyph used.
//
//	Regardless if we're under the Appearance Manager or not, the
//	correct values will be returned for key, modifiers, and glyph.

void
UAppearance10Menus::GetItemKeyEquivalent(
	MenuHandle		inMacMenuH,
	UInt16			inMenuItem,
	SInt16			&outKey,
	UInt8			&outModifiers,
	SInt16			&outGlyph )
{
	outKey		 = 0;
	outModifiers = kMenuNoModifiers;
	outGlyph	 = 0;

	::GetItemCmd(inMacMenuH, (SInt16) inMenuItem, &outKey);

	if (outKey != 0) {

		OSErr err = ::GetMenuItemModifiers(inMacMenuH, (SInt16) inMenuItem, &outModifiers);
		ThrowIfOSErr_(err);

		err = ::GetMenuItemKeyGlyph(inMacMenuH, (SInt16) inMenuItem, &outGlyph);
		ThrowIfOSErr_(err);
	}
}


PP_End_Namespace_PowerPlant
