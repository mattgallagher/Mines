// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableView.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTableView
#define _H_LTableView
#pragma once

#include <UTables.h>
#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LTableGeometry;
class	LTableSelector;
class	LTableStorage;

// ---------------------------------------------------------------------------

class	LTableView : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('tabv') };

		// ¥ Constructors & Destructor

						LTableView();
						
						LTableView(
								const SPaneInfo			&inPaneInfo,
								const SViewInfo			&inViewInfo);
								
						LTableView( LStream* inStream );
								
	virtual				~LTableView();

		// ¥ Setters/Getters

	void				GetTableSize(
								TableIndexT&			outRows,
								TableIndexT&			outCols) const;

	void				SetUseDragSelect( Boolean inUseIt )
							{
								mUseDragSelect = inUseIt;
							}

	void				SetCustomHilite( Boolean inCustom )
							{
								mCustomHilite = inCustom;
							}

	Boolean				GetCustomHilite()	{ return mCustomHilite; }

	void				SetDeferAdjustment( Boolean inDefer );

	Boolean				GetDeferAdjustment()	{ return mDeferAdjustment; }

		// ¥ Row, Col, and Cell Verification

	Boolean				IsValidRow( TableIndexT inRow ) const;
								
	Boolean				IsValidCol( TableIndexT inCol ) const;
								
	Boolean				IsValidCell( const STableCell & inCell ) const;

		// ¥ Row & Col Management

	virtual void		InsertRows(
								UInt32					inHowMany,
								TableIndexT				inAfterRow,
								const void*				inDataPtr = nil,
								UInt32					inDataSize = 0,
								Boolean					inRefresh = Refresh_No);
								
	virtual void		InsertCols(
								UInt32					inHowMany,
								TableIndexT				inAfterCol,
								const void*				inDataPtr = nil,
								UInt32					inDataSize = 0,
								Boolean					inRefresh = Refresh_No);
								
	virtual void		RemoveRows(
								UInt32					inHowMany,
								TableIndexT				inFromRow,
								Boolean					inRefresh);
								
	virtual void		RemoveAllRows( Boolean inRefresh );
	
	virtual void		RemoveCols(
								UInt32					inHowMany,
								TableIndexT				inFromCol,
								Boolean					inRefresh);
								
	virtual void		RemoveAllCols( Boolean inRefresh );
								
	virtual void		AdjustImageSize( Boolean inRefresh );

		// ¥ Accessing Cells

	virtual void		CellToIndex(
								const STableCell&		inCell,
								TableIndexT&			outIndex) const;
								
	virtual void		IndexToCell(
								TableIndexT				inIndex,
								STableCell&				outCell) const;

	virtual Boolean		GetNextCell( STableCell& ioCell ) const;
								
	virtual STableCell	GetFirstSelectedCell() const;
	
	virtual Boolean		GetNextSelectedCell( STableCell& ioCell ) const;

	virtual Boolean		GetPreviousCell( STableCell& ioCell ) const;
								
	virtual Boolean		GetPreviousSelectedCell( STableCell& ioCell ) const;

	virtual void		ScrollCellIntoFrame( const STableCell& inCell );

		//  ¥ Cell Geometry

	virtual void		SetTableGeometry( LTableGeometry* inTableGeometry );

	LTableGeometry*		GetTableGeometry()		{ return mTableGeometry; }

	virtual void		GetImageCellBounds(
								const STableCell&		inCell,
								SInt32&					outLeft,
								SInt32&					outTop,
								SInt32&					outRight,
								SInt32&					outBottom) const;
								
	virtual Boolean		GetLocalCellRect(
								const STableCell&		inCell,
								Rect&					outCellFrame) const;
								
	virtual Boolean		GetCellHitBy(
								const SPoint32&			inImagePt,
								STableCell&				outCell) const;

	virtual UInt16		GetRowHeight( TableIndexT inRow ) const;
								
	virtual void		SetRowHeight(
								UInt16					inHeight,
								TableIndexT				inFromRow,
								TableIndexT				inToRow);

	virtual UInt16		GetColWidth( TableIndexT inCol ) const;
								
	virtual void		SetColWidth(
								UInt16					inWidth,
								TableIndexT				inFromCol,
								TableIndexT				inToCol);

	virtual void		FetchIntersectingCells(
								const Rect&				inLocalRect,
								STableCell&				outTopLeft,
								STableCell&				outBotRight) const;

		// ¥ Selecting Cells

	virtual void		SetTableSelector( LTableSelector* inTableSelector );

	LTableSelector*		GetTableSelector()		{ return mTableSelector; }

	virtual Boolean		CellIsSelected( const STableCell& inCell ) const;
	
	virtual void		SelectCell( const STableCell& inCell );
	
	virtual void		SelectAllCells();
	
	virtual void		UnselectCell( const STableCell& inCell );
								
	virtual void		UnselectAllCells();

	virtual Boolean		ClickSelect(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);

	virtual void		HiliteSelection(
								Boolean					inActively,
								Boolean					inHilite);
								
	virtual void		HiliteCell(
								const STableCell&		inCell,
								Boolean					inHilite);

	virtual void		SelectionChanged();
	
	virtual Boolean		PointsAreClose(
								Point					inFirstPt,
								Point					inSecondPt) const;

		// ¥ Storing Data

	virtual void		SetTableStorage( LTableStorage* inTableStorage );

	LTableStorage*		GetTableStorage()		{ return mTableStorage; }

	virtual void		SetCellData(
								const STableCell&		inCell,
								const void*				inDataPtr,
								UInt32					inDataSize);
								
	virtual void		GetCellData(
								const STableCell&		inCell,
								void*					outDataPtr,
								UInt32&					ioDataSize) const;
								
	virtual Boolean		FindCellData(
								STableCell&				outCell,
								const void*				inDataPtr,
								UInt32					inDataSize) const;

		// ¥ Refreshing

	virtual void		RefreshCell( const STableCell& inCell );
								
	virtual void		RefreshCellRange(
								const STableCell&		inTopLeft,
								const STableCell&		inBotRight);
								
	virtual void		ApplyForeAndBackColors() const;

protected:
	TableIndexT		mRows;
	TableIndexT		mCols;
	LTableGeometry	*mTableGeometry;
	LTableSelector	*mTableSelector;
	LTableStorage	*mTableStorage;
	Boolean			mUseDragSelect;
	Boolean			mCustomHilite;
	Boolean			mDeferAdjustment;

		// ¥ Hiliting

	virtual void		GetHiliteRgn( RgnHandle ioHiliteRgn );
								
	virtual void		HiliteCellActively(
								const STableCell&		inCell,
								Boolean					inHilite);
								
	virtual void		HiliteCellInactively(
								const STableCell&		inCell,
								Boolean					inHilite);
								
	virtual void		ActivateSelf();
	
	virtual void		DeactivateSelf();

		// ¥ Clicking

	virtual void		ClickSelf(
								const SMouseDownEvent&	inMouseDown);

	virtual void		ClickCell(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);

		// ¥ Drawing

	virtual void		DrawSelf();
	
	virtual void		PrintPanelSelf( const PanelSpec& inPanel );
	
	virtual void		DrawCellRange(
								const STableCell&	inTopLeftCell,
								const STableCell&	inBottomRightCell);
	
	virtual void		DrawCell(
								const STableCell&	inCell,
								const Rect&			inLocalRect);
	
	virtual void		DrawBackground();
	

private:
	void				InitTable();
};


// ---------------------------------------------------------------------------

class	StDeferTableAdjustment {
public:
				StDeferTableAdjustment( LTableView* inTable );
				
				~StDeferTableAdjustment();
private:
	LTableView*		mTable;
	Boolean			mSaveDefer;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
