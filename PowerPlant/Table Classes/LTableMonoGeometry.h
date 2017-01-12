// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableMonoGeometry.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Manages geometry of a TableView that has the same width and height for
//	every cell

#ifndef _H_LTableMonoGeometry
#define _H_LTableMonoGeometry
#pragma once

#include <UTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTableMonoGeometry : public LTableGeometry {
public:
						LTableMonoGeometry(
								LTableView			*inTableView,
								UInt16				inColWidth,
								UInt16				inRowHeight);
	virtual				~LTableMonoGeometry() { }

	virtual void		GetImageCellBounds(
								const STableCell	&inCell,
								SInt32				&outLeft,
								SInt32				&outTop,
								SInt32				&outRight,
								SInt32				&outBottom) const;

	virtual TableIndexT	GetRowHitBy(
								const SPoint32		&inImagePt) const;
	virtual TableIndexT	GetColHitBy(
								const SPoint32		&inImagePt) const;

	virtual void		GetTableDimensions(
								UInt32				&outWidth,
								UInt32				&outHeight) const;

	virtual UInt16		GetRowHeight(
								TableIndexT			/* inRow */) const
							{
								return mRowHeight;
							}

	virtual void		SetRowHeight(
								UInt16				inHeight,
								TableIndexT			inFromRow,
								TableIndexT			inToRow);

	virtual UInt16		GetColWidth(
								TableIndexT			/* inCol */) const
							{
								return mColWidth;
							}

	virtual void		SetColWidth(
								UInt16				inWidth,
								TableIndexT			inFromCol,
								TableIndexT			inToCol);

protected:
	UInt16		mColWidth;
	UInt16		mRowHeight;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
