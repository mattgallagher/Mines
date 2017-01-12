// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMenu.h						PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMenu
#define _H_LMenu
#pragma once

#include <PP_Prefix.h>
#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LMenu {
	friend class LMenuBar;

public:
				LMenu();

				LMenu( ResIDT inMENUid );

				LMenu(	SInt16			inMENUid,
						ConstStringPtr	inTitle,
						bool			inAlwaysThemeSavvy = false);
						
				~LMenu();

	MenuHandle	GetMacMenuH() const		{ return mMacMenuH; }

	ResIDT		GetMenuID() const		{ return mMENUid; }

					// Mapping between Command numbers and Index numbers

	CommandT	CommandFromIndex( SInt16 inIndex ) const;

	CommandT	SyntheticCommandFromIndex( SInt16 inIndex ) const;

	SInt16		IndexFromCommand( CommandT inCommand ) const;

	bool		FindNextCommand(
						SInt16&			ioIndex,
						SInt32&			outCommand) const;

					// Manipulating Items

	void		SetCommand(
						SInt16			inIndex,
						CommandT		inCommand);

	void		InsertCommand(
						ConstStringPtr	inItemText,
						CommandT		inCommand,
						SInt16			inAfterItem);

	void		RemoveCommand( CommandT inCommand );

	void		RemoveItem( SInt16 inItemToRemove );

	void		EnableItem( SInt16 inIndex );

	void		DisableItem( SInt16 inIndex );

	bool		ItemIsEnabled( SInt16 inIndex ) const;

	bool		IsInstalled() const		{ return mIsInstalled; }

	void		SetInstalled( Boolean inInstalled )
					{
						mIsInstalled = inInstalled;
					}

	void		SetUsed( Boolean inUsed )
					{
						mUsed = inUsed;
					}

	bool		IsUsed() const			{ return mUsed; }

	static MenuHandle	MakeNewMacMenu(
								ResIDT			inMENUid,
								ConstStringPtr	inTitle = Str_Empty,
								bool			inAlwaysThemeSavvy = false);

protected:
	LMenu*			mNextMenu;
	MenuHandle		mMacMenuH;
	ResIDT			mMENUid;
	SInt16			mNumCommands;
	CommandT**		mCommandNums;
	bool			mIsInstalled;
	bool			mUsed;

	void		ReadCommandNumbers();

	LMenu*		GetNextMenu() const		{ return mNextMenu; }

	void		SetNextMenu( LMenu* inMenu )
					{
						mNextMenu = inMenu;
					}
};


// ---------------------------------------------------------------------------
//	Stack-based utility class for deleting a MenuHandle

class StMenuHandleDisposer {
public:
						StMenuHandleDisposer( MenuHandle inMenuH );

						~StMenuHandleDisposer();

		operator		MenuHandle()	{ return mMacMenuH; }

protected:
					MenuHandle		mMacMenuH;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
