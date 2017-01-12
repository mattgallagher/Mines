// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTEViewTextAction.h		   	PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UTEViewTextAction
#define _H_UTEViewTextAction
#pragma once

#include <LAction.h>
#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LCommander;
class LPane;
class LTextEditView;

// ---------------------------------------------------------------------------

class	LTEStyleTextAction : public LAction {
public:
						LTEStyleTextAction(
								SInt16		inDescriptionIndex,
								MessageT	inActionCommand,
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane,
								bool		inAlreadyDone = false);

	virtual				~LTEStyleTextAction();

	virtual void		Redo();
	virtual void		Undo();

	virtual Boolean		CanRedo() const;
	virtual Boolean		CanUndo() const;

	virtual	Boolean		IsPostable() const;

protected:
	LCommander*			mTextCommander;
	LTextEditView*		mTEView;
	TEHandle			mMacTEH;
	MessageT			mActionCommand;

	Handle				mDeletedTextH;
	SInt32				mDeletedTextLen;
	SInt16				mSelStart;
	SInt16				mSelEnd;
	StScrpHandle		mDeletedStyleH;

	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class	LTEViewCutAction : public LTEStyleTextAction {
public:
						LTEViewCutAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEViewCutAction() { }

protected:
	virtual void		RedoSelf();
};

// ---------------------------------------------------------------------------

class	LTEViewPasteAction : public LTEStyleTextAction {
public:
						LTEViewPasteAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEViewPasteAction();

protected:
	Handle				mPastedTextH;
	Handle				mPastedStyleH;

	virtual void		RedoSelf();
	virtual void		UndoSelf();
};


// ---------------------------------------------------------------------------

class	LTEViewClearAction : public LTEStyleTextAction {
public:
						LTEViewClearAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEViewClearAction() { }

protected:
	virtual void		RedoSelf();
};

// ---------------------------------------------------------------------------

class	LTEViewTypingAction : public LTEStyleTextAction {
public:
						LTEViewTypingAction(
								TEHandle	inMacTEH,
								LCommander*	inTextCommander,
								LPane*		inTextPane);

	virtual				~LTEViewTypingAction();

	virtual void		Reset();
	virtual void		InputCharacter();
	virtual void		BackwardErase();
	virtual void		ForwardErase();

protected:
	Handle				mTypedTextH;
	Handle				mTypedStyleH;
	SInt16				mTypingStart;
	SInt16				mTypingEnd;

	virtual void		RedoSelf();
	virtual void		UndoSelf();
};


// ---------------------------------------------------------------------------

class LTEViewStyleAction : public LAction {
public:
						LTEViewStyleAction(
								SInt16		inDescriptionIndex,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

	virtual				~LTEViewStyleAction();

	virtual Boolean		CanRedo() const;
	virtual Boolean		CanUndo() const;

protected:
	LCommander*			mCommander;
	LTextEditView*		mTEView;

	SInt16				mSelStart;
	SInt16				mSelEnd;
};

// ---------------------------------------------------------------------------

class LTEViewFontAction : public LTEViewStyleAction {
public:
						LTEViewFontAction(
								SInt16		inFont,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

						LTEViewFontAction(
								Str255		inFontName,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

	virtual				~LTEViewFontAction();

protected:
	SInt16				mFont;
	SInt16				mSavedFont;

	virtual	void		RedoSelf();
	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class LTEViewSizeAction : public LTEViewStyleAction {
public:
						LTEViewSizeAction(
								SInt16		inSize,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

	virtual				~LTEViewSizeAction();

protected:
	SInt16				mSize;
	SInt16				mSavedSize;

	virtual	void		RedoSelf();
	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class LTEViewFaceAction : public LTEViewStyleAction {
public:
						LTEViewFaceAction(
								Style		inFace,
								bool		inToggle,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

	virtual				~LTEViewFaceAction();

protected:
	Style				mFace;
	Style				mSavedFace;
	bool				mToggle;

	virtual	void		RedoSelf();
	virtual void		UndoSelf();
};

// ---------------------------------------------------------------------------

class LTEViewAlignAction : public LTEViewStyleAction {
public:
						LTEViewAlignAction(
								SInt16		inAlign,
								LCommander*	inCommander,
								LPane*		inPane,
								bool		inAlreadyDone = false);

	virtual				~LTEViewAlignAction();

protected:
	SInt16				mAlign;
	SInt16				mSavedAlign;

	virtual	void		RedoSelf();
	virtual	void		UndoSelf();
};

// ---------------------------------------------------------------------------

class LTEViewColorAction : public LTEViewStyleAction {
public:
						LTEViewColorAction(
								const RGBColor&	inColor,
								LCommander*		inCommander,
								LPane*			inPane,
								bool			inAlreadyDone = false);

	virtual				~LTEViewColorAction();

protected:
	RGBColor			mColor;
	RGBColor			mSavedColor;

	virtual	void		RedoSelf();
	virtual	void		UndoSelf();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif

