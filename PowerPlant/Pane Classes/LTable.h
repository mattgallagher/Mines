// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTable.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTable
#define _H_LTable
#pragma once

#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LArray;

typedef	UInt32	TableIndexT;

typedef struct	TableCellT {
	TableIndexT	row;
	TableIndexT	col;
} TableCellT, *TableCellP, **TableCellH;

#pragma options align=mac68k

typedef struct	STableInfo {
	SInt32		numberOfRows;
	SInt32		numberOfCols;
	SInt32		rowHeight;
	SInt32		colWidth;
	SInt32		cellDataSize;
} STableInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LTable : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('tabl') };

					LTable();
					
					LTable(	const SPaneInfo&	inPaneInfo,
							const SViewInfo&	inViewInfo,
							SInt32				inNumberOfRows,
							SInt32				inNumberOfCols,
							SInt32				inRowHeight,
							SInt32				inColWidth,
							SInt32				inCellDataSize);
							
					LTable( LStream* inStream );
					
	virtual			~LTable();

	void			GetTableSize(
							TableIndexT&	outRows,
							TableIndexT&	outCols) const;
	
	Boolean			IsValidCell( const TableCellT &inCell ) const;
	
	Boolean			EqualCell(
							const TableCellT&	inCellA,
							const TableCellT&	inCellB) const;

	virtual void	InsertRows(
							SInt32		inHowMany,
							TableIndexT	inAfterRow,
							void*		inCellData);
							
	virtual void	InsertCols(
							SInt32		inHowMany,
							TableIndexT	inAfterCol,
							void*		inCellData);
								
	virtual void	RemoveRows(
							SInt32		inHowMany,
							TableIndexT	inFromRow);
	
	virtual void	RemoveCols(
							SInt32		inHowMany,
							TableIndexT	inFromCol);

	virtual void	SetRowHeight(
							SInt16		inHeight,
							TableIndexT	inFrom,
							TableIndexT	inTo);
							
	virtual void	SetColWidth(
							SInt16		inWidth,
							TableIndexT inFrom,
							TableIndexT inTo);

	void			SetCellDataSize( SInt32 inCellDataSize );
	
	virtual void	SetCellData(
							const TableCellT&	inCell,
							void*				inData);
	
	virtual void	GetCellData(
							const TableCellT&	inCell,
							void*				outData);

	virtual void	SelectCell( const TableCellT& inCell );
	
	void			GetSelectedCell( TableCellT& outCell ) const;
	
	virtual void	ApplyForeAndBackColors() const;

protected:
	TableIndexT		mRows;
	TableIndexT		mCols;
	SInt32			mRowHeight;
	SInt32			mColWidth;
	LArray			*mCellData;
	TableCellT		mSelectedCell;

	virtual void	ClickSelf( const SMouseDownEvent& inMouseDown );
	
	virtual void	DrawSelf();
	
	virtual void	DrawBackground();

	virtual void	HiliteCell( const TableCellT& inCell );
	
	virtual void	UnhiliteCell( const TableCellT& inCell );

	virtual void	ClickCell(
							const TableCellT&		inCell,
							const SMouseDownEvent&	inMouseDown);
							
	virtual void	DrawCell( const TableCellT& inCell );

	virtual void	ActivateSelf();
	
	virtual void	DeactivateSelf();

	virtual Boolean	FetchLocalCellFrame(
							const TableCellT&	inCell,
							Rect&				outCellFrame);
									
	virtual void	FetchCellHitBy(
							const SPoint32&	inImagePt,
							TableCellT&		outCell);

	SInt32			FetchCellDataIndex( const TableCellT& inCell );

private:
	void			InitTable(
							SInt32	inNumberOfRows,
							SInt32	inNumberOfCols,
							SInt32	inRowHeight,
							SInt32	inColWidth,
							SInt32	inCellDataSize);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
