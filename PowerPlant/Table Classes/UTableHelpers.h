// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTableHelpers.h			PowerPlant 2.2.2		©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Contains Classes:
//		LTableGeometry
//		LTableSelector
//		LTableStorage

#ifndef _H_UTableHelpers
#define _H_UTableHelpers
#pragma once

#include <LTableView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableGeometry										  LTableGeometry ¥
// ---------------------------------------------------------------------------
//	Abstract class for maintaining the location, width, and height of each
//	cell in a TableView

class	LTableGeometry {
public:
						LTableGeometry(
								LTableView	*inTableView)
							{
								mTableView = inTableView;
							}

	virtual				~LTableGeometry() { }

	virtual void		GetImageCellBounds(
								const STableCell	&inCell,
								SInt32				&outLeft,
								SInt32				&outTop,
								SInt32				&outRight,
								SInt32				&outBottom)
									const = 0;				// Pure Virtual

	virtual TableIndexT	GetRowHitBy(
								const SPoint32	&inImagePt)
									const = 0;				// Pure Virtual
	virtual TableIndexT	GetColHitBy(
								const SPoint32	&inImagePt)
									const = 0;				// Pure Virtual

	virtual void		GetTableDimensions(
								UInt32		&outWidth,
								UInt32		&outHeight)
									const = 0;				// Pure Virtual

	virtual UInt16		GetRowHeight(
								TableIndexT	inRow)
									const = 0;				// Pure Virtual
	virtual void		SetRowHeight(
								UInt16		inHeight,
								TableIndexT	inFromRow,
								TableIndexT	inToRow)
									= 0;					// Pure Virtual

	virtual UInt16		GetColWidth(
								TableIndexT	inCol)
									const = 0;				// Pure Virtual
	virtual void		SetColWidth(
								UInt16		inWidth,
								TableIndexT inFromCol,
								TableIndexT	inToCol)
									= 0;					// Pure Virtual

	virtual void		InsertRows(
								UInt32		/* inHowMany */,
								TableIndexT	/* inAfterRow */) { }
	virtual void		InsertCols(
								UInt32		/* inHowMany */,
								TableIndexT	/* inAfterCol */) { }
	virtual void		RemoveRows(
								UInt32		/* inHowMany */,
								TableIndexT	/* inFromRow */) { }
	virtual void		RemoveCols(
								UInt32		/* inHowMany */,
								TableIndexT	/* inFromCol */) { }

protected:
	LTableView		*mTableView;
};


// ---------------------------------------------------------------------------
//	¥ LTableSelector										  LTableSelector ¥
// ---------------------------------------------------------------------------
//	Abstract class for maintaining which cells are selected in a TableView

class	LTableSelector {
public:
						LTableSelector(
								LTableView	*inTableView)
							{
								mTableView = inTableView;
							}

	virtual				~LTableSelector() { }

	virtual Boolean		CellIsSelected(
								const STableCell		&inCell) const = 0;
	virtual	STableCell	GetFirstSelectedCell() const = 0;
	virtual	TableIndexT	GetFirstSelectedRow() const = 0;

	virtual void		SelectCell(
								const STableCell		&inCell) = 0;
	virtual void		SelectAllCells() = 0;

	virtual void		UnselectCell(
								const STableCell		&inCell) = 0;
	virtual void		UnselectAllCells() = 0;

	virtual void		ClickSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown) = 0;
	virtual Boolean		DragSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown) = 0;

	virtual void		InsertRows(
								UInt32					/* inHowMany */,
								TableIndexT				/* inAfterRow */) { }
	virtual void		InsertCols(
								UInt32					/* inHowMany */,
								TableIndexT				/* inAfterCol */) { }
	virtual void		RemoveRows(
								UInt32					/* inHowMany */,
								TableIndexT				/* inFromRow */) { }
	virtual void		RemoveCols(
								UInt32					/* inHowMany */,
								TableIndexT				/* inFromCol */) { }

protected:
	LTableView		*mTableView;
};


// ---------------------------------------------------------------------------
//	¥ LTableStorage											   LTableStorage ¥
// ---------------------------------------------------------------------------
//	Abstract class for storing the data associated with each cell in
//	a TableView

class	LTableStorage {
public:
						LTableStorage(
								LTableView			*inTableView)
							{
								mTableView = inTableView;
							}

	virtual				~LTableStorage() { }

	virtual void		SetCellData(
								const STableCell	&inCell,
								const void			*inDataPtr,
								UInt32				inDataSize)
									= 0;					// Pure Virtual
	virtual void		GetCellData(
								const STableCell	&inCell,
								void				*outDataPtr,
								UInt32				&ioDataSize) const
									= 0;					// Pure Virtual
	virtual Boolean		FindCellData(
								STableCell			&outCell,
								const void			*inDataPtr,
								UInt32				inDataSize) const
									= 0;					// Pure Virtual

	virtual void		InsertRows(
								UInt32				inHowMany,
								TableIndexT			inAfterRow,
								const void			*inDataPtr,
								UInt32				inDataSize)
									= 0;					// Pure Virtual
	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol,
								const void			*inDataPtr,
								UInt32				inDataSize)
									= 0;					// Pure Virtual

	virtual void		RemoveRows(
								UInt32				inHowMany,
								TableIndexT			inFromRow)
									= 0;					// Pure Virtual
	virtual void		RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol)
									= 0;					// Pure Virtual

	virtual void		GetStorageSize(
								TableIndexT			&outRows,
								TableIndexT			&outCols)
									= 0;					// Pure Virtual

protected:
	LTableView		*mTableView;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
