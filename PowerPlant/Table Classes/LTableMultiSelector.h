// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMultiSelector.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages the selection for a TableView than can have multiple selections

#ifndef _H_LTableMultiSelector
#define _H_LTableMultiSelector
#pragma once

#include <UTableHelpers.h>
#include <URegions.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTableMultiSelector : public LTableSelector {
public:
						LTableMultiSelector( LTableView* inTableView );
								
	virtual				~LTableMultiSelector();

	virtual Boolean		CellIsSelected( const STableCell& inCell ) const;
								
	virtual	STableCell	GetFirstSelectedCell() const;
	
	virtual	TableIndexT	GetFirstSelectedRow() const;

	virtual void		SelectCell( const STableCell& inCell );
	
	virtual void		SelectAllCells();

	virtual void		UnselectCell( const STableCell& inCell );
	
	virtual void		UnselectAllCells();

	virtual void		ClickSelect(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);
								
	virtual Boolean		DragSelect(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);

	virtual void		InsertRows(
								UInt32					inHowMany,
								TableIndexT				inAfterRow);
								
	virtual void		InsertCols(
								UInt32					inHowMany,
								TableIndexT				inAfterCol);
								
	virtual void		RemoveRows(
								UInt32					inHowMany,
								TableIndexT				inFromRow);
								
	virtual void		RemoveCols(
								UInt32					inHowMany,
								TableIndexT				inFromCol);
protected:
	virtual void		UnselectAllCells( bool inReportChange );

	virtual void		SelectCellBlock(
								const STableCell&		inCellA,
								const STableCell&		inCellB);
protected:
	StRegion		mSelectionRgn;
	STableCell		mAnchorCell;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
