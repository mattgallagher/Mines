// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenuBar.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMenuBar
#define _H_LMenuBar
#pragma once

#include <PP_Prefix.h>

#include <Events.h>
#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LMenu;

const ResIDT	InstallMenu_AtEnd	= 0;

// ---------------------------------------------------------------------------

class	LMenuBar {
public:
						LMenuBar( ResIDT inMBARid );

	virtual 			~LMenuBar();

	virtual CommandT	MenuCommandSelection(
								const EventRecord&	inMouseEvent,
								SInt32&				outMenuChoice) const;

	virtual CommandT	FindKeyCommand(
								const EventRecord&	inKeyEvent,
								SInt32&				outMenuChoice) const;

	CommandT			FindCommand(
								ResIDT				inMENUid,
								SInt16				inItem) const;

	void				FindMenuItem(
								CommandT			inCommand,
								ResIDT&				outMENUid,
								MenuHandle&			outMenuHandle,
								SInt16&				outItem) const;

	bool				FindNextCommand(
								SInt16&				ioIndex,
								MenuHandle&			ioMenuHandle,
								LMenu*				&ioMenu,
								CommandT&			outCommand) const;

	void				InstallMenu(
								LMenu*				inMenu,
								ResIDT				inBeforeMENUid);

	void				RemoveMenu( LMenu* inMenu );

	LMenu*				FetchMenu( ResIDT inMENUid ) const;

	bool				FindNextMenu( LMenu* &ioMenu ) const;

	void				SetModifierKeys( UInt16 inModifierKeys )
							{
								mModifierKeys = inModifierKeys;
							}

	UInt16				GetModifierKeys() const	{ return mModifierKeys; }

	static LMenuBar*	GetCurrentMenuBar()		{ return sMenuBar; }

protected:
	static	LMenuBar*	sMenuBar;

	LMenu*				mMenuListHead;
	UInt16				mModifierKeys;
	bool				mSupportsMenuEvent;
};


// ---------------------------------------------------------------------------
//	StUnhiliteMenu
// ---------------------------------------------------------------------------
//	Stack-based class to unhilite the currently selected menu title

class	StUnhiliteMenu {
public:
						StUnhiliteMenu();
						~StUnhiliteMenu();

protected:
	UInt32		mStartTicks;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
