// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTablePaneHostingGeometry.h	 PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	A variant of LTableMultiGeometry that allows panes to be installed
//	in table cells. The panes move and resize to follow the cell and
//	can bind to the cell frame, much as a pane installed in a regular
//	view binds to its superviews' frame.
//
//	NOTE: Only one pane may be installed in any cell. If you need more
//	than one pane, install a view as the cell pane and use it to contain
//	the other panes.

#ifndef _H_LTablePaneHostingGeometry
#define _H_LTablePaneHostingGeometry
#pragma once

#include <LTableMultiGeometry.h>
#include <LTableArrayStorage.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LTablePaneHostingGeometry : public LTableMultiGeometry {

public:
							LTablePaneHostingGeometry(
									LTableView*		inTableView,
									UInt16			inColWidth,
									UInt16			inRowHeight);
									
	virtual					~LTablePaneHostingGeometry();

	// pane accessors

	virtual LPane*			GetCellPane( const STableCell& inCell );
	
	virtual void			SetCellPane(
									const STableCell&	inCell,
									LPane*				inPane);

	// overrides to adjust pane location

	virtual void			InsertCols(
									UInt32				inHowMany,
									TableIndexT			inAfterCol);
									
	virtual void			InsertRows(
									UInt32				inHowMany,
									TableIndexT			inAfterRow);

	virtual void			RemoveCols(
									UInt32				inHowMany,
									TableIndexT			inFromCol);
									
	virtual void			RemoveRows(
									UInt32				inHowMany,
									TableIndexT			inFromRow);

	virtual void			SetColWidth(
									UInt16				inWidth,
									TableIndexT			inFromCol,
									TableIndexT			inToCol);
									
	virtual void			SetRowHeight(
									UInt16				inHeight,
									TableIndexT			inFromRow,
									TableIndexT			inToRow);

	// resizing helpers

protected:
	virtual void			MoveColumnsBy(
									TableIndexT			inStartCol,
									SInt16				inHorizDelta);
									
	virtual void			MoveRowsBy(
									TableIndexT			inStartRow,
									SInt16				inVertDelta);

	virtual void			MoveCellBy(
									const STableCell&	inCell,
									SInt16				inHorizDelta,
									SInt16				inVertDelta);
									
	virtual void			ResizeCellBy(
									const STableCell&	inCell,
									SInt16				inWidthDelta,
									SInt16				inHeightDelta);


	LTableArrayStorage		mCellPanes;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
