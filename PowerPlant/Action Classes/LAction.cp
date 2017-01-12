// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAction.cp				  	PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an undoable action.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAction.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAction								Constructor				  [public]
// ---------------------------------------------------------------------------
//	inStringResID is the ID of the STR# resource for the "Redo" description
//
//	inStringIndex is the index in the STR# of the "Redo" description for
//		this Action
//
//	inAlreadyDone specifies whether the Action has already been done
//		and does not need to be redone when posted
//
//	By convention, the "Undo" description is at the same index in the
//		STR# resource with ID = inStringResID + 1

LAction::LAction(
	ResIDT		inStringResID,
	SInt16		inStringIndex,
	bool		inAlreadyDone)
{
	mStringResID	= inStringResID;
	mStringIndex	= inStringIndex;
	mIsDone			= inAlreadyDone;
}


// ---------------------------------------------------------------------------
//	¥ LAction								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LAction::LAction(
	const LAction&	inOriginal)
{
	mStringResID	= inOriginal.mStringResID;
	mStringIndex	= inOriginal.mStringIndex;
	mIsDone			= inOriginal.mIsDone;
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LAction&
LAction::operator = (
	const LAction&	inOriginal)
{
	if (this != &inOriginal) {
		mStringResID	= inOriginal.mStringResID;
		mStringIndex	= inOriginal.mStringIndex;
		mIsDone			= inOriginal.mIsDone;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LAction								Destructor				  [public]
// ---------------------------------------------------------------------------

LAction::~LAction()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Finalize														  [public]
// ---------------------------------------------------------------------------
//	Perform clean up for the Action
//
//	This function gets called when the Action is no longer "current",
//	meaning that another Action has been posted to the Undoer owning
//	this Action. For single "undo" implementations, the Action will
//	be deleted afterwards.

void
LAction::Finalize()
{
}


// ---------------------------------------------------------------------------
//	¥ Redo															  [public]
// ---------------------------------------------------------------------------
//	Wrapper function which calls RedoSelf() if the Action can be redone
//
//	Note that the first time this function is called, "Redo" really
//	means "Do" the Action (unless isAlreadyDone is set to true when
//	creating the Action).

void
LAction::Redo()
{
	if (CanRedo()) {
		RedoSelf();
	}

	mIsDone = true;
}


// ---------------------------------------------------------------------------
//	¥ Undo															  [public]
// ---------------------------------------------------------------------------
//	Wrapper function which calls UndoSelf() if the Action can be undone

void
LAction::Undo()
{
	if (CanUndo()) {
		UndoSelf();
	}

	mIsDone = false;
}


// ---------------------------------------------------------------------------
//	¥ IsDone												 [inline] [public]
// ---------------------------------------------------------------------------
//	Returns the currest state of the Action
//
//	If "done", you can Undo the Action
//	If not "done", you can Redo the Action

#pragma mark LAction::IsDone

//	bool
//	LAction::IsDone() const						// Defined inline


// ---------------------------------------------------------------------------
//	¥ IsPostable													  [public]
// ---------------------------------------------------------------------------
//	Return whether an Action is postable, meaning that it affects the
//	"undo" state.
//
//	This implementation always returns true. Override to return false for
//	actions that are not undoable.

Boolean
LAction::IsPostable() const
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ CanRedo														  [public]
// ---------------------------------------------------------------------------
//	Return whether the Action can be redone

Boolean
LAction::CanRedo() const
{
	return (not IsDone());
}


// ---------------------------------------------------------------------------
//	¥ CanUndo														  [public]
// ---------------------------------------------------------------------------
//	Return whether the Action can be undone

Boolean
LAction::CanUndo() const
{
	return IsDone();
}


// ---------------------------------------------------------------------------
//	¥ GetDescription												  [public]
// ---------------------------------------------------------------------------
//	Pass back the Redo and Undo strings for the Action

void
LAction::GetDescription(
	Str255	outRedoString,
	Str255	outUndoString) const
{
	::GetIndString(outRedoString, mStringResID, mStringIndex);
	::GetIndString(outUndoString, (SInt16) (mStringResID + 1), mStringIndex);
}


PP_End_Namespace_PowerPlant
