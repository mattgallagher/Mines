// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAction.h				  	PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an undoable action.
//
//	LAction is used with LUndoer and LCommander to implement undo.
//	You attach an LUndoer object to a Commander (typically an Application
//	or Document). Then create an object of a subclass of LAction whenever
//	the user does something that you want to be undoable, and call
//	PostAction for that Commander.
//
//	Actions are always performed (via Redo) when they're posted.
//	Thereafter, they can be undone (Undo) and redone (Redo) as appropriate.
//
//	If an action has already been done at the time of posting, it is
//	the responsibility of the poster to appropriately set inAlreadyDone
//	to true when posting the Action.
//
//	Finalize is called immediately before an Action is replaced
//	with a new undoable Action.

#ifndef _H_LAction
#define _H_LAction
#pragma once

#include <PP_Prefix.h>
#include <PP_Resources.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAction {
public:
						LAction(
								ResIDT		inStringResID = STRx_RedoEdit,
								SInt16		inStringIndex = str_RedoUndo,
								bool		inAlreadyDone = false);

						LAction( const LAction&	inOriginal );

	LAction&			operator = ( const LAction&	inOriginal );

	virtual				~LAction();

	virtual void		Finalize();

	virtual void		Redo();			//	Also functions as "Do"
	virtual void		Undo();

	bool				IsDone() const;
	virtual Boolean		IsPostable() const;
	virtual Boolean		CanRedo() const;
	virtual Boolean		CanUndo() const;

	virtual void		GetDescription(
								Str255		outRedoString,
								Str255		outUndoString) const;

protected:
	ResIDT				mStringResID;		//	STR# ID for redo menu item
	SInt16				mStringIndex;		//	Index in STR# for redo item
	bool				mIsDone;			//	Is Action done or redone?

						//	Pure Virtual functions. Subclasses must override!
	virtual void		RedoSelf() = 0;
	virtual void		UndoSelf() = 0;
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ IsDone												 [inline] [public]
// ---------------------------------------------------------------------------

inline bool
LAction::IsDone() const
{
	return mIsDone;
}

// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
