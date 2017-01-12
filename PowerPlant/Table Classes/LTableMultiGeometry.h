// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMultiGeometry.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages geometry of a TableView where rows and columns can have
//	different sizes

#ifndef _H_LTableMultiGeometry
#define _H_LTableMultiGeometry
#pragma once

#include <UTableHelpers.h>
#include <LRunArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTableMultiGeometry : public LTableGeometry {
public:
						LTableMultiGeometry(
								LTableView*			inTableView,
								UInt16				inColWidth,
								UInt16				inRowHeight);

	virtual				~LTableMultiGeometry();

	virtual void		GetImageCellBounds(
								const STableCell&	inCell,
								SInt32&				outLeft,
								SInt32&				outTop,
								SInt32&				outRight,
								SInt32&				outBottom) const;

	virtual TableIndexT	GetRowHitBy( const SPoint32& inImagePt ) const;

	virtual TableIndexT	GetColHitBy( const SPoint32& inImagePt ) const;

	virtual void		GetTableDimensions(
								UInt32&				outWidth,
								UInt32&				outHeight) const;

	virtual UInt16		GetRowHeight(
								TableIndexT			inRow) const;

	virtual void		SetRowHeight(
								UInt16				inHeight,
								TableIndexT			inFromRow,
								TableIndexT			inToRow);

	virtual UInt16		GetColWidth( TableIndexT inCol ) const;

	virtual void		SetColWidth(
								UInt16				inWidth,
								TableIndexT			inFromCol,
								TableIndexT			inToCol);

	virtual void		InsertRows(
								UInt32				inHowMany,
								TableIndexT			inAfterRow);

	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol);

	virtual void		RemoveRows(
								UInt32				inHowMany,
								TableIndexT			inFromRow);

	virtual void		RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol);

protected:
	LRunArray			mRowHeights;
	LRunArray			mColWidths;
	UInt16				mDefaultRowHeight;
	UInt16				mDefaultColWidth;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
