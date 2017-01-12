// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleAEAction.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an action for which undo and redo are implemented
//	by sending AppleEvents to the application.

#ifndef _H_LSimpleAEAction
#define _H_LSimpleAEAction
#pragma once

#include <LAction.h>
#include <UAppleEventsMgr.h>
#include <AERegistry.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LSimpleAEAction : public LAction {
public:
						LSimpleAEAction(
								ResIDT			inStringResID = STRx_RedoEdit,
								SInt16			inStringIndex = str_RedoUndo,
								bool			inAlreadyDone = false);

						LSimpleAEAction(
								const LSimpleAEAction&	inOriginal);

	LSimpleAEAction& 	operator=(
								const LSimpleAEAction&	inOriginal);

	virtual				~LSimpleAEAction();

	// AppleEvent Configuration

	virtual void		SetRedoAE(
								AEEventClass	inEventClass,
								AEEventID		inEventID);
	virtual void		SetRedoAE(
								const AppleEvent& inAppleEvent);

	virtual void		SetUndoAE(
								AEEventClass	inEventClass,
								AEEventID		inEventID,
								Boolean			inFeedUndoWithRedoReply = false);
	virtual void		SetUndoAE(
								const AppleEvent& inAppleEvent,
								Boolean			  inFeedUndoWithRedoReply = false);

	virtual void		SetPostUndoAE(
								const AppleEvent& inAppleEvent);
	virtual void		UndoAESetKeyFed(
								DescType		inKey,
								DescType		inFromReplyKey = keyAEResult);
	virtual void		PostUndoAESetKeyFed(
								DescType		inKey,
								DescType		inFromReplyKey = keyAEResult);

	virtual void		RedoAEAdd(
								AEKeyword		theAEKeyword,
								const AEDesc&	inDesc);
	virtual void		UndoAEAdd(
								AEKeyword		theAEKeyword,
								const AEDesc&	inDesc);

	// Undo/Redo Implementation

	virtual void		RedoSelf();
	virtual void		UndoSelf();
	virtual void		Finalize();
	virtual Boolean		CanRedo() const;
	virtual Boolean		CanUndo() const;

	// Accessor Methods

	virtual void		SetRecordOnlyFinalState(
								Boolean			inRecordOnlyFinalState);

	virtual void		GetReplyDesc(
								AEDesc*			outDesc);

	// AppleEvent Implementation

protected:
	virtual void		SendAppleEvent(
								AppleEvent&		inAppleEvent,
								AESendMode		inSendModifiers = 0);

	StAEDescriptor		mRedoEvent;				// AE that performs or redoes the action.
	StAEDescriptor		mUndoEvent;				// AE that undoes mRedoEvent.
	StAEDescriptor		mPostUndoEvent;			// AE that executes of mUndoEvent

	StAEDescriptor		mReply;					// Reply from last AE.
	DescType			mUndoAEKeyFed;			// The undo key parameter being fed by the redo AE.
												//	(typeNull if none).
	DescType			mFromReplyKey;			// Key from reply fed into Undo AE.
	DescType			mPostUndoAEKeyFed;
	DescType			mPostFromReplyKey;
	Boolean				mRecordOnlyFinalState;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
