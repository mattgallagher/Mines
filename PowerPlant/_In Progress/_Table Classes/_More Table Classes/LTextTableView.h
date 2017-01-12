// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextTableView.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	Simple subclass of LTableView that just draws strings

/*
     INSTRUCTIONS

1)
    in the Constructor
	Create an LView object with a class id of 'Ttab'.


2)
	Register the class type somewhere, possibly in YourApp::RegisterClasses()
	LTextTableView::RegisterClass();

	in FinishCreateSelf()
3)
	get a pointer to the control with a cast like:
		mFindField = (LTextTableView *)FindPaneByID('Edit');

	instead of the normal
		mFindField = (LTableView *)FindPaneByID('Edit');

*/

#ifndef _H_LTextTableView
#define _H_LTextTableView
#pragma once

#include <LTableView.h>
#include <LBroadcaster.h>
#include <UNavigableTable.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant
	class LFocusBox;

class LCellSizer;
class LTableDragger;
class LTableSelState;

#pragma options align=mac68k

typedef struct {
	MessageT	doubleClickMessage;
	UInt32		bufferSize;
	SInt16		textTraitsID;
	MessageT	singleClickMessage;
} STextTableViewInfo;

#pragma options align=reset

class	LTextTableView : public LTableView, public LNavigableTable, public LBroadcaster {
public:
	enum 				{ class_ID = FOUR_CHAR_CODE('Ttab') };
	static LTextTableView*	CreateFromStream(
								LStream					*inStream);

		// ¥ Constructors & Destructor

						LTextTableView();

						LTextTableView(
								const SPaneInfo			&inPaneInfo,
								const SViewInfo			&inViewInfo,
								MessageT				inSingleClickMsg,
								MessageT				inDoubleClickMsg,
								UInt32					inBufferSize=255,
								SInt16 					inTextTraitsID=0);

						LTextTableView(LStream			*inStream);

	virtual				~LTextTableView();
	static void 	RegisterClass ();

	virtual SInt32	GetValue() const;
	virtual void	SetValue(SInt32 inValue);

	LTableSelState*	GetSavedSelection() const;
	void	SetSavedSelection(const LTableSelState*);

	UInt16		GetNumFrozenRows() const;
	void			SetNumFrozenRows(UInt16);

	Boolean		GetFrozenRowsSelectable() const;
	void			SetFrozenRowsSelectable(Boolean);

	virtual void		ScrollCellIntoFrame(
								const STableCell		&inCell);

	MessageT		GetDoubleClickMessage() const;
	void			SetDoubleClickMessage(MessageT);
	virtual void	DoubleClicked();

	MessageT		GetSingleClickMessage() const;
	void			SetSingleClickMessage(MessageT);
	virtual void	SingleClicked();

	virtual Boolean	ClickSelect(const STableCell& inCell,const SMouseDownEvent& inMouseDown);
	virtual void ClickCell(const STableCell&, const SMouseDownEvent&);
	virtual void Click(SMouseDownEvent	&);
	virtual Boolean FocusDraw(LPane* inSubPane=nil);
	virtual void DrawCell(	const STableCell&,	const Rect&);
	virtual void		DrawSelf();
	virtual Boolean		GetLocalCellRect(
								const STableCell		&inCell,
								Rect					&outCellFrame) const;
	virtual Boolean		GetCellHitBy(
								const SPoint32			&inImagePt,
								STableCell				&outCell) const;

	virtual void		ScrollBits(
								SInt32				inLeftDelta,
								SInt32				inTopDelta);

		//  ¥ Cell Sizing
	void			SetCellSizer(LCellSizer*);

		//  ¥ Drag and Drop
	void 			SetTableDragger(LTableDragger*);

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(
								SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);

private:
	void				InitTextTable(
								MessageT				inSingleClickMsg,
								MessageT				inDoubleClickMsg,
								UInt32					inBufferSize,
								SInt16 					inTextTraitsID);

protected:
	virtual void		ShowSelf();
	virtual void		HideSelf();
	virtual UInt16 FrozenRowsHeight() const;

// data storage
protected:
	UInt32			mCellBufferSize;
	char*			mCellBuffer;	// owned
	MessageT		mSingleClickMessage, mDoubleClickMessage;
	LCellSizer*		mCellSizer;		// owned
	LTableDragger*	mDragger;		// owned
	SInt16 			mTextTraitsID;
	UInt16		mNumFrozenRows, mFrozenRowUnderlineThickness;
	Boolean	mFrozenRowsSelectable;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
