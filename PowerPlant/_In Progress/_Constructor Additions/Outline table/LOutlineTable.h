// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineTable.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOutlineTable
#define _H_LOutlineTable
#pragma once

#include <LTableView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LOutlineItem;
struct SOutlineDrawContents;


// ===========================================================================
//		* LOutlineTable
// ===========================================================================
//	An OutlineTable is a table that is designed to present a Finder-like
//	outline display. The theory of operation is that you create one object
//	(derived from LOutlineItem) for each row of the table. The OutlineItem
//	controls the drawing for all cells in that row and provides a mechanism
//	for customizing the display on a row-by-row basis.
//
//	Related classes:
//		LOutlineItem				- Abstract base class for row display controllers
//		LEditableOutlineItem		- Outline item that support in-place editing
//		LOutlineKeySelector			- Provides arrow keys to move selection up/down
//										and expand/collapse outline nodes
//		LInPlaceOutlineKeySelector	- Outline key selector that supports tabbing
//										between in-place edit fields
//		LOutlineMultiSelector		- Provides Finder-like drag selection
//		LOutlineRowSelector			- Selects on a row-by-row basis, not by cells
//

class LOutlineTable : public LTableView {

public:
	enum { class_ID = FOUR_CHAR_CODE('otab') };

							LOutlineTable(
									LStream*			inStream);
	virtual					~LOutlineTable();

	// display accessors

	inline ResIDT			GetTextTraits() const
									{ return mDefaultTxtrID; }
	inline void				SetTextTraits(ResIDT		inDefaultTxtrID)
									{ mDefaultTxtrID = inDefaultTxtrID; }

	inline UInt16			GetFirstLevelIndent() const
									{ return mFirstLevelIndent; }
	inline UInt32			GetMaximumNestingDepth() const
									{ return mMaximumIndent; }

	// item accessors

	inline const LArray&	GetItems() const
									{ return mOutlineItems; }
	inline const LArray&	GetFirstLevelItems() const
									{ return mFirstLevelItems; }

	inline void				GetCurrentInPlaceEditCell(STableCell& outIPECell) const
									{ outIPECell = mCurrentInPlaceEditCell; }

	virtual LOutlineItem*	FindItemForRow(
									TableIndexT			inRow) const;
	TableIndexT				FindRowForItem(
									const LOutlineItem* inOutlineItem) const;

	// row management

	virtual void			InsertItem(
									LOutlineItem*		inOutlineItem,
									LOutlineItem*		inSuperItem,
									LOutlineItem*		inAfterItem,
									Boolean				inRefresh = true,
									Boolean				inAdjustImageSize = true);
	virtual void			RemoveItem(
									LOutlineItem*		inOutlineItem,
									Boolean				inRefresh = true,
									Boolean				inAdjustImageSize = true);

	// column management

	virtual void			InsertCols(
									UInt32				inHowMany,
									TableIndexT			inAfterCol,
									const void*			inDataPtr = nil,
									UInt32				inDataSize = 0,
									Boolean				inRefresh = false);
	virtual void			RemoveCols(
									UInt32				inHowMany,
									TableIndexT			inFromCol,
									Boolean				inRefresh);

	// selecting cells

	virtual void			HiliteCell(
									const STableCell&	inCell,
									Boolean				inHilite);
	virtual void			HiliteSelection(
									Boolean				inActively,
									Boolean				inHilite);
	virtual Boolean			CellHitByMarquee(
									const STableCell&	inCell,
									const Rect&			inMarqueeLocalRect);

	// drawing
	virtual Boolean			GetLocalCellRect(
								const STableCell		&inCell,
								Rect					&outCellFrame) const;


	// refreshing

	virtual void			RefreshCellRange(
									const STableCell&	inTopLeft,
									const STableCell&	inBotRight);
	virtual void			AdjustImageSize(
									Boolean				inRefresh);

	// drawing

protected:
	virtual void			DrawSelf();

	virtual void			DrawCell(
									const STableCell&	inCell,
									const Rect&			inLocalRect);
	virtual void			GetHiliteRgn(
									RgnHandle			ioHiliteRgn);

	// clicking

	virtual void			ClickSelf(
									const SMouseDownEvent&		inMouseDown);
	virtual void			ClickCell(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown);
	virtual void			TrackDrag(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown);
	virtual void			TrackEmptyClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents);

	// table sizing

	void					RecalcMaxIndent();
	virtual void			MaximumIndentChanged();

	// disallowed functions

private:
	virtual void			InsertRows(
									UInt32				inHowMany,
									TableIndexT			inAfterRow,
									const void*			inDataPtr,
									UInt32				inDataSize,
									Boolean				inRefresh);
	virtual void			RemoveRows(
									UInt32				inHowMany,
									TableIndexT			inFromRow,
									Boolean				inRefresh);

	// in-place editing helper

	void					SetCurrentInPlaceEditCell(
									const STableCell&	inCell);


	// data members

protected:
	LArray					mOutlineItems;				// (of LOutlineItem*) all outline items in table
	LArray					mFirstLevelItems;			// (of LOutlineItem*) first-level items in table
	ResIDT					mDefaultTxtrID;				// default text traits
	UInt16					mFirstLevelIndent;			// left margin of table
	UInt16					mMaximumIndent;				// (calculated) deepest pixel indent of outline items
	Boolean					mOkayToAdjustImageSize;		// true if AdjustImageSize should be allowed (optimization)
	STableCell				mCurrentInPlaceEditCell;	// table cell that's currently being edited

	friend class LOutlineItem;
	friend class LEditableOutlineItem;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
