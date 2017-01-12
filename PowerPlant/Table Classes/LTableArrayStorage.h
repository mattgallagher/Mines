// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableArrayStorage.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Stores data for a TableView in an Array

#ifndef _H_LTableArrayStorage
#define _H_LTableArrayStorage
#pragma once

#include <UTableHelpers.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LArray;

// ---------------------------------------------------------------------------

class	LTableArrayStorage : public LTableStorage {
public:
						LTableArrayStorage(
								LTableView*			inTableView,
								UInt32				inDataSize);

						LTableArrayStorage(
								LTableView*			inTableView,
								LArray*				inDataArray);

	virtual				~LTableArrayStorage();

	virtual void		SetCellData(
								const STableCell&	inCell,
								const void*			inDataPtr,
								UInt32				inDataSize);

	virtual void		GetCellData(
								const STableCell&	inCell,
								void*				outDataPtr,
								UInt32&				ioDataSize) const;

	void				GetCellDataByIndex(
								TableIndexT			inCellIndex,
								void*				outDataPtr,
								UInt32&				ioDataSize) const;

	virtual Boolean		FindCellData(
								STableCell&			outCell,
								const void*			inDataPtr,
								UInt32				inDataSize) const;

	virtual void		InsertRows(
								UInt32				inHowMany,
								TableIndexT			inAfterRow,
								const void*			inDataPtr,
								UInt32				inDataSize);

	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol,
								const void*			inDataPtr,
								UInt32				inDataSize);

	virtual void		RemoveRows(
								UInt32				inHowMany,
								TableIndexT			inFromRow);

	virtual void		RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol);

	virtual void		GetStorageSize(
								TableIndexT&		outRows,
								TableIndexT&		outCols);

protected:
	LArray			*mDataArray;
	Boolean			mOwnsArray;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
