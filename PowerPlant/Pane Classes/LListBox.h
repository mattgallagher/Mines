// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LListBox.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LListBox
#define _H_LListBox
#pragma once

#include <LPane.h>
#include <LCommander.h>
#include <LBroadcaster.h>

#include <Lists.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LFocusBox;

#pragma options align=mac68k

typedef struct {
	UInt8		hasHorizScroll;
	UInt8		hasVertScroll;
	UInt8		hasGrow;
	UInt8		hasFocusBox;
	MessageT	doubleClickMessage;
	SInt16		textTraitsID;
	SInt16		LDEFid;
	SInt16		numberOfItems;
} SListBoxInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class LListBox : public LPane,
				 public LCommander,
				 public LBroadcaster {
public:
	enum { class_ID = FOUR_CHAR_CODE('lbox') };

						LListBox();
						LListBox(
								const LListBox		&inOriginal);
						LListBox(
								const SPaneInfo		&inPaneInfo,
								Boolean				inHasHorizScroll,
								Boolean				inHasVertScroll,
								Boolean				inHasGrow,
								Boolean				inHasFocusBox,
								MessageT			inDoubleClickMessage,
								SInt16				inTextTraitsID,
								SInt16				inLDEFid,
								LCommander			*inSuper);
						LListBox(
								LStream				*inStream);
	virtual				~LListBox();

	ListHandle			GetMacListH() const		{ return mMacListH; }
	LFocusBox*			GetFocusBox()			{ return mFocusBox; }

	virtual SInt32		GetValue() const;
	virtual void		SetValue(
								SInt32				inValue);

	virtual StringPtr	GetDescriptor(
								Str255				outDescriptor) const;
	virtual void		SetDescriptor(
								ConstStringPtr		inDescriptor);

	MessageT			GetDoubleClickMessage() const
												{ return mDoubleClickMessage; }

	void				SetDoubleClickMessage(
								MessageT			inMessage)
						{
							mDoubleClickMessage = inMessage;
						}

	virtual Boolean		FocusDraw(
								LPane				*inSubPane = nil);
	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void				*ioParam = nil);
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean				&outEnabled,
								Boolean				&outUsesMark,
								UInt16				&outMark,
								Str255				outName);
	virtual Boolean		HandleKeyPress(
								const EventRecord&	inKeyEvent);

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(
								SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);

	virtual void		SelectOneCell(
								Cell				inCell);
	virtual void		SelectAllCells();
	virtual void		UnselectAllCells();
	virtual Boolean		GetLastSelectedCell(
								Cell				&outCell);
	virtual void		MakeCellVisible(
								Cell				inCell);

	virtual void		SavePlace(
								LStream				*outPlace);
	virtual void		RestorePlace(
								LStream				*inPlace);

protected:
	ListHandle		mMacListH;
	MessageT		mDoubleClickMessage;
	LFocusBox		*mFocusBox;
	ResIDT			mTextTraitsID;
	Boolean			mHasGrow;

	virtual void		DrawSelf();
	virtual void		ClickSelf(
								const SMouseDownEvent	&inMouseDown);

	virtual void		BeTarget();
	virtual void		DontBeTarget();

	virtual void		ShowSelf();
	virtual void		HideSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		DoNavigationKey(
								const EventRecord	&inKeyEvent);
	virtual void		DoTypeSelection(
								const EventRecord	&inKeyEvent);

private:
	void				InitListBox(
								Boolean				inHasHorizScroll,
								Boolean				inHasVertScroll,
								Boolean				inHasGrow,
								Boolean				inHasFocusBox,
								MessageT			inDoubleClickMessage,
								SInt16				inTextTraitsID,
								SInt16				inLDEFid);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
