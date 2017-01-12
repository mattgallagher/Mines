// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenuController.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Mixin class for Controls that have an associated popup menu

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMenuController.h>
#include <LControlImp.h>
#include <LControlPane.h>
#include <PP_Resources.h>
#include <UDrawingState.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

MenuHandle	LMenuController::sEmptyMenuH = nil;


// ---------------------------------------------------------------------------
//	¥ LMenuController						Constructor				  [public]
// ---------------------------------------------------------------------------

LMenuController::LMenuController(
	LControl*	inControl)
{
	mControl	= inControl;
	mMenuH		= nil;
	mOwnsMenu	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LMenuController						Destructor				  [public]
// ---------------------------------------------------------------------------

LMenuController::~LMenuController()
{
		// Our MenuHandle is never a resource since SetMacMenuH()
		// calls DetachResource() if the menu originally came
		// from a MENU resource. Therefore, we can always call
		// DisposeMenu() to delete the MenuHandle.

	if (mOwnsMenu && (mMenuH != nil)) {
		::DisposeMenu(mMenuH);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMacMenuH													  [public]
// ---------------------------------------------------------------------------
//	Specify the MenuHandle for the Popup Menu
//
//	If inOwnsMenu is true, the MenuController will delete the MenuHandle
//	when it's no longer needed. Use inOwnsMenu of false if you wish to
//	share a MenuHandle among multiple MenuControllers.

void
LMenuController::SetMacMenuH(
	MenuHandle	inMenuH,
	bool		inOwnsMenu)
{
										// Can't own a nil Menu or our
										//   special empty menu
	if ( (inMenuH == nil)  ||  (inMenuH == GetEmptyMenuH()) ) {
		inOwnsMenu = false;
	}
	
	if (inMenuH != mMenuH) {
	
		if (mOwnsMenu) {				// Dispose existing Menu
			::DisposeMenu(mMenuH);
		}
		
		mMenuH	  = inMenuH;
		mOwnsMenu = inOwnsMenu;

		#if PP_Target_Classic
										// If we own the MenuHandle, detach
										//   it if it's a resource so that
										//   we have a private copy.
										//
										//   On Carbon, GetMenu() always returns
										//   a new copy, and a MenuRef is not
										//   a Handle on Mac OS X.

			if (inOwnsMenu && (::HGetState((Handle) inMenuH) & 0x20)) {
				::DetachResource((Handle) inMenuH);
			}

		#endif

		SetMenuMinMax();
		mControl->Draw(nil);
		
	} else {							// New menu same as old one
		mOwnsMenu = inOwnsMenu;			// Could be an ownership change
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMenuID														  [public]
// ---------------------------------------------------------------------------
//	Specify the Menu Resource ID for the Popup Menu
//
//	We detach the Menu resource after getting it so that this popup has
//	its own copy of the MenuHandle. Changes made to the Menu, such as
//	adding or removing items, will not affect any other popup that uses
//	the same Menu ID.
//
//	Note that we always use the MENU resource to set the MenuHandle even
//	if you respecify the same ID. For example, if you do the following:
//
//		SetMenuID(234);
//		// ... Change menu by adding/removing/renaming items
//		SetMenuID(234);
//
//	The menu returns to the original state of the MENU resource. The
//	changes made prior to the second SetMenuID call are lost.

void
LMenuController::SetMenuID(
	ResIDT		inMenuID)
{
	MenuHandle	menuH = nil;

	if (inMenuID != MENU_Unspecified) {
		menuH = ::MacGetMenu(inMenuID);
		ThrowIfNil_(menuH);
	}

	SetMacMenuH(menuH, true);
}


// ---------------------------------------------------------------------------
//	¥ GetMenuID														  [public]
// ---------------------------------------------------------------------------
//	Return the Menu ID of the Popup Menu

ResIDT
LMenuController::GetMenuID() const
{
	ResIDT	menuID = MENU_Unspecified;

	if (mMenuH != nil) {
		menuID = ::GetMenuID(mMenuH);
	}

	return menuID;
}


// ---------------------------------------------------------------------------
//	¥ SetCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------
//	Set the index for the selected menu item

void
LMenuController::SetCurrentMenuItem(
	SInt16	inItem)
{
	mControl->SetValue(inItem);
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------
//	Return the index of the currently selected menu item

SInt16
LMenuController::GetCurrentMenuItem() const
{
	return (SInt16) mControl->GetValue();
}


// ---------------------------------------------------------------------------
//	¥ SetMenuItemText												  [public]
// ---------------------------------------------------------------------------
//	Set the text for the specified menu item

void
LMenuController::SetMenuItemText(
	SInt16			inItem,
	ConstStringPtr	inItemString)
{
	if (mMenuH != nil) {
		::SetMenuItemText(mMenuH, inItem, inItemString);

		if (inItem == GetCurrentMenuItem()) {
			mControl->Draw(nil);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetMenuItemText												  [public]
// ---------------------------------------------------------------------------
//	Get the text for the specified menu item

StringPtr
LMenuController::GetMenuItemText(
	SInt16			inItem,
	StringPtr		outItemString) const
{
	if (mMenuH != nil) {
		::GetMenuItemText(mMenuH, inItem, outItemString);

	} else {
		outItemString[0] = 0;
	}

	return outItemString;
}


// ---------------------------------------------------------------------------
//	¥ AppendMenu													  [public]
// ---------------------------------------------------------------------------
//	Add item(s) to the end of the Popup Menu
//
//	If inIgnoreMetachars is true, adds a single entry with the exactly the
//	text specified  by inItemString to the end of the menu.
//
//	If inIgnoreMetachars is false, calls Toolbox routine MacAppendMenu() to
//	add to the end of the menu. Metacharacters contained within the text
//	of inItemString will affect attributes of the item (and may even
//	specify multiple items). Read the documentation in Inside Mac about
//	MacAppendMenu() for information about metacharacters.
//
//	Normally, you will use inIgnoreMetachars = true (the default value)

void
LMenuController::AppendMenu(
	ConstStringPtr	inItemString,
	bool			inIgnoreMetachars)
{
	if (mMenuH != nil) {
		if (inIgnoreMetachars) {

				// To ignore metacharacters, must call MacAppendMenu()
				// with a dummy item, then set item's text to the
				// specified string

			::MacAppendMenu(mMenuH, Str_Dummy);
			::SetMenuItemText(mMenuH, (SInt16) ::CountMenuItems(mMenuH), inItemString);

		} else {
			::MacAppendMenu(mMenuH, inItemString);
		}

		SetMenuMinMax();
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertMenuItem												  [public]
// ---------------------------------------------------------------------------
//	Insert item(s) in the Popup Menu
//
//	inAfterItem of zero inserts items at the start of the menu.
//	inAfterItem >= number of items inserts items at the end of the menu
//
//	If inIgnoreMetachars is true, adds a single entry with the exactly the
//	text specified  by inItemString after inAfterItem.
//
//	If inIgnoreMetachars is false, calls Toolbox routine MacInsertMenuItem()
//	to insert items into the menu. Metacharacters contained within the text
//	of inItemString will affect attributes of the item (and may even
//	specify multiple items). Read the documentation in Inside Mac about
//	MacAppendMenu() for information about metacharacters.
//
//	Normally, you will use inIgnoreMetachars = true (the default value)

void
LMenuController::InsertMenuItem(
	ConstStringPtr	inItemString,
	SInt16			inAfterItem,
	bool			inIgnoreMetachars)
{
	if (mMenuH != nil) {
									// Enforce bounds for inAfterItem
		SInt16	numItems = (SInt16) ::CountMenuItems(mMenuH);
		if (inAfterItem > numItems) {
			inAfterItem = numItems;
		} else if (inAfterItem < 0) {
			inAfterItem = 0;
		}

		if (inIgnoreMetachars) {

				// To ignore metacharacters, must call MacInsertMenuItem()
				// with a dummy item, then set item's text to the
				// specified string

			::MacInsertMenuItem(mMenuH, Str_Dummy, inAfterItem);
			::SetMenuItemText(mMenuH, (SInt16) (inAfterItem + 1),
								inItemString);

		} else {
			::MacInsertMenuItem(mMenuH, inItemString, inAfterItem);
		}

		SetMenuMinMax();

		SInt16	currItem = GetCurrentMenuItem();
		if (inAfterItem < currItem) {

				// Inserting before current item. Adjust value so that
				// the same item remains the current one. MacInsertMenuItem()
				// can add multiple items using metachars, so we find the
				// number of items inserted by subtracting the count of
				// items before insertion from the current count.

			SetCurrentMenuItem((SInt16) (currItem + ::CountMenuItems(mMenuH)
											- numItems));
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DeleteMenuItem												  [public]
// ---------------------------------------------------------------------------
//	Remove an item from the Popup Menu

void
LMenuController::DeleteMenuItem(
	SInt16		inItem)
{
	if (mMenuH != nil) {
		SInt16	currItem = GetCurrentMenuItem();

		::DeleteMenuItem(mMenuH, inItem);
		SetMenuMinMax();

		if (inItem == currItem) {		// Deleting the current item
			SetCurrentMenuItem(1);		// Make first item the current one
			mControl->Draw(nil);		// Redraw to show new current item

		} if ( (inItem < currItem) && (inItem > 0) ) {
										// Deleting item before current one
			SetCurrentMenuItem((SInt16) (currItem - 1));
		}

	}
}


// ---------------------------------------------------------------------------
//	¥ DeleteAllMenuItems											  [public]
// ---------------------------------------------------------------------------
//	Remove all items from the PopupMenu, leaving a menu with zero items

void
LMenuController::DeleteAllMenuItems()
{
	if (mMenuH != nil) {

			// Delete all items from MenuHandle from last to first.
			// This avoids having to shift down the remaining items
			// after each deletion.

		SInt16	item = (SInt16) ::CountMenuItems(mMenuH);
		while (item > 0) {
			::DeleteMenuItem(mMenuH, item);
			item -= 1;
		}

		SetMenuMinMax();				// Min and Max will both be zero
										// This has the side effect of
										//    setting the value to zero.
	}
}


// ---------------------------------------------------------------------------
//	¥ AppendResMenu													  [public]
// ---------------------------------------------------------------------------
//	Add items whose titles are the names of resources of the specified
//	resource type to the end of the Popup Menu

void
LMenuController::AppendResMenu(
	ResType		inResType)
{
	if (mMenuH != nil) {
		::AppendResMenu(mMenuH, inResType);
		SetMenuMinMax();
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertResMenu													  [public]
// ---------------------------------------------------------------------------
//	Insert items whose titles are the names of resources of the specified
//	resource type to the end of the Popup Menu

void
LMenuController::InsertResMenu(
	ResType		inResType,
	SInt16		inAfterItem)
{
	if (mMenuH != nil) {
		SInt16	numItems = (SInt16) ::CountMenuItems(mMenuH);

		::InsertResMenu(mMenuH, inResType, inAfterItem);
		SetMenuMinMax();

		SInt16	currItem = GetCurrentMenuItem();
		if (inAfterItem < currItem) {

				// Inserting before current item. Adjust value so that
				// the same item remains the current one. Number of items
				// inserted is the difference between the item count now
				// and before the insertion.

			SetCurrentMenuItem((SInt16) (currItem + ::CountMenuItems(mMenuH)
											- numItems));
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMenuMinMax													  [public]
// ---------------------------------------------------------------------------
//	Set the min and max values to match the number of items in the Menu

void
LMenuController::SetMenuMinMax()
{
	SInt32	max = 0;					// Max is zero if there is no menu
	
	if (mMenuH != nil) {
		max = ::CountMenuItems(mMenuH);
	}

	bool	refresh = false;
	
	{									// Don't draw while changing
										//   min and max values
		StPaneVisibleState		hideMenu(mControl, triState_Off);
		
		if (max == 0) {					// Menu has no items
		
										// Need to refresh when removing
										//   all items from the menu that
										//   had some items
			refresh = (mControl->GetMaxValue() > 0);
		
										// Set both min and max to zero.
										//   Set min first so that we don't
										//   temporarily make min > max.
			mControl->SetMinValue(0);
			mControl->SetMaxValue(0);
			
		} else {						// Limits are 1 to max
		
										// Need to refresh when adding
										//   items to a formerly empty menu
			refresh = (mControl->GetMaxValue() == 0);
		
										// Set max first
										//   to avoid (min > max) when both
										//   min and max are currently zero.
			mControl->SetMaxValue(max);
			mControl->SetMinValue(1);
		}
	}

	if (refresh) {						// Need to refresh here, since
		mControl->Refresh();			//   setting min/max sometimes
	}									//   validates update region.
}

#pragma mark -

// ---------------------------------------------------------------------------
//	GetEmptyMenuH											 [static] [public]
// ---------------------------------------------------------------------------
//	Returns MenuHandle to a special empty MenuHandle used internally by
//	popup menu classes.
//
//	NOTE: Pre-Carbon versions of PP just called MacGetMenu(MENU_Empty)
//	whenever it needed the empty menu. But in later OS versions,
//	MacGetMenu() creates a unique copy of the menu. To avoid memory leaks,
//	we only call MacGetMenu() once and store the MenuHandle in a static.

MenuHandle
LMenuController::GetEmptyMenuH()
{
	if (sEmptyMenuH == nil) {
		sEmptyMenuH = ::MacGetMenu(MENU_Empty);
	}

	return sEmptyMenuH;
}


#pragma mark -
// ===========================================================================
//	StPopupMenuSetter
// ===========================================================================
//	Constructor sets the MenuHandle
//	Destructor unsets (sets to nil) the MenuHandle

StPopupMenuSetter::StPopupMenuSetter(
	LControlPane*	inControlPane,
	MenuHandle		inMenuH)
{
	mControlPane	= inControlPane;
	mMenuH			= inMenuH;

	if (inMenuH != nil) {
		inControlPane->FocusDraw();
											// Suppress drawing
		StPaneVisibleState	hidePopup(inControlPane, triState_Off);

			// Use data tag to set the MenuHandle
			// This will implicitly install the Menu in the Toolbox MenuList

		inControlPane->GetControlImp()->SetDataTag(0,
					kControlPopupButtonMenuHandleTag,
					sizeof(MenuHandle), &inMenuH);
	}
}


StPopupMenuSetter::~StPopupMenuSetter()
{
	if (mMenuH != nil) {
		mControlPane->FocusDraw();
											// Suppress drawing
		StPaneVisibleState	hidePopup(mControlPane, triState_Off);

			// Use data tag to set the Imp's MenuHandle to nil

		MenuHandle	nilMenuH = nil;
		mControlPane->GetControlImp()->SetDataTag(0,
					kControlPopupButtonMenuHandleTag,
					sizeof(MenuHandle), &nilMenuH);

			// Some versions of the OS do not automatically uninstall
			// the existing popup MenuHandle from the global menu list.
			// Unfortunately, MacDeleteMenu() takes a MenuID, which is
			// not necessarily unique (multiple menus can have the same
			// MenuID). So, check if our MenuHandle is installed before
			// removing the menu with its ID.
			
		MenuID		theID	= ::GetMenuID(mMenuH);
		MenuHandle	theMenu = ::GetMenuHandle(theID);
		
		if (theMenu == mMenuH) {
			::MacDeleteMenu(theID);
		}
	}
}


PP_End_Namespace_PowerPlant
