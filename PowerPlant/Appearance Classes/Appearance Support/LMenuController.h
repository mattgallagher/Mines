// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenuController.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMenuController
#define _H_LMenuController
#pragma once

#include <PP_Prefix.h>
#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LControl;
class	LControlPane;

// ---------------------------------------------------------------------------

class	LMenuController  {
public:
						LMenuController( LControl* inControl );

	virtual				~LMenuController();

	virtual void		SetMacMenuH(
								MenuHandle		inMenuH,
								bool			inOwnsMenu = true);

	MenuHandle			GetMacMenuH() const		{ return mMenuH; }

	void				SetMenuID( ResIDT inMenuID );

	ResIDT				GetMenuID() const;

	virtual void		SetCurrentMenuItem( SInt16 inItem );

	virtual SInt16		GetCurrentMenuItem() const;

	virtual void		SetMenuItemText(
								SInt16			inItem,
								ConstStringPtr	inItemString);

	virtual StringPtr	GetMenuItemText(
								SInt16			inItem,
								StringPtr		outItemString) const;

	LControl*			GetControl() const		{ return mControl; }

	virtual void		AppendMenu(
								ConstStringPtr	inItemString,
								bool			inIgnoreMetachars = true);

	virtual void		InsertMenuItem(
								ConstStringPtr	inItemString,
								SInt16			inAfterItem,
								bool			inIgnoreMetachars = true);

	virtual void		DeleteMenuItem( SInt16 inItem );

	virtual void		DeleteAllMenuItems();

	virtual void		AppendResMenu( ResType inResType );

	virtual void		InsertResMenu(
								ResType			inResType,
								SInt16			inAfterItem);

	virtual void		SetMenuMinMax();

public:
	static MenuHandle	GetEmptyMenuH();
	

protected:
	LControl*			mControl;
	MenuHandle			mMenuH;
	bool				mOwnsMenu;

private:
	static MenuHandle	sEmptyMenuH;
};


// ===========================================================================

class	StPopupMenuSetter {
public:
	StPopupMenuSetter( LControlPane* inControlPane, MenuHandle inMenuH );
	~StPopupMenuSetter();

private:
	LControlPane*	mControlPane;
	MenuHandle		mMenuH;
};


// ===========================================================================

										// Toolbox Popup Button CDEF uses this
										//   ID tell the button not to try to
										//   get its MENU from a resource
const ResIDT	MENU_Unspecified = -12345;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
