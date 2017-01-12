// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTETextAction.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Undoable Actions associated with a Toolbox TextEdit record

#ifndef _H_UTETextAction
#define _H_UTETextAction
#pragma once

#include <LAction.h>
#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LCommander;
class	LPane;

// ---------------------------------------------------------------------------

class	LTETextAction : public LAction {
public:
						LTETextAction(
								SInt16		inDescriptionIndex,
								MessageT	inActionCommand,
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane,
								bool		inAlreadyDone = false);

	virtual				~LTETextAction();

	virtual void		Redo();
	virtual void		Undo();

	virtual Boolean		CanRedo() const;
	virtual Boolean		CanUndo() const;

	virtual Boolean		IsPostable() const;

protected:
	LCommander*		mTextCommander;
	LPane*			mTextPane;
	TEHandle		mMacTEH;
	MessageT		mActionCommand;

	Handle			mDeletedTextH;
	SInt32			mDeletedTextLen;
	SInt16			mSelStart;
	SInt16			mSelEnd;

	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class	LTECutAction : public LTETextAction {
public:
						LTECutAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTECutAction() { }

protected:
	virtual void		RedoSelf();
};

// ---------------------------------------------------------------------------

class	LTEPasteAction : public LTETextAction {
public:
						LTEPasteAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEPasteAction();

protected:
	Handle			mPastedTextH;

	virtual void		RedoSelf();
	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class	LTEClearAction : public LTETextAction {
public:
						LTEClearAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEClearAction() { }

protected:
	virtual void		RedoSelf();
};

// ---------------------------------------------------------------------------

class	LTETypingAction : public LTETextAction {
public:
						LTETypingAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTETypingAction();

	virtual void		Reset();
	virtual void		InputCharacter();
	virtual void		BackwardErase();
	virtual void		ForwardErase();

protected:
	Handle			mTypedTextH;
	SInt16			mTypingStart;
	SInt16			mTypingEnd;

	virtual void		RedoSelf();
	virtual void		UndoSelf();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif

