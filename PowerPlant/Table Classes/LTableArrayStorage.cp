// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableArrayStorage.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Stores data for a TableView in an Array

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTableArrayStorage.h>
#include <LVariableArray.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTableArrayStorage					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Associates Storage with a particular TableView
//
//	Use inDataSize > 0 for cells which have the SAME size data
//	Use inDataSize == 0 for cells which have DIFFERENT size data

LTableArrayStorage::LTableArrayStorage(
	LTableView*	inTableView,
	UInt32		inDataSize)

	: LTableStorage(inTableView)
{
	SignalIf_(mTableView == nil);

		// Allocate a new Array to store the data. The ArrayStorage
		// owns this Array, which it will delete in its destructor.

	if (inDataSize > 0) {				// Fixed-size data
		mDataArray = new LArray(inDataSize);

	} else {							// Variable-size data
		mDataArray = new LVariableArray;
	}

	mOwnsArray = true;
}


// ---------------------------------------------------------------------------
//	¥ LTableArrayStorage					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Associates Storage with a particular TableView and specifies the Array
//	to use for storing the data for each cell
//
//	The Array will NOT be deleted when the Storage is deleted

LTableArrayStorage::LTableArrayStorage(
	LTableView*		inTableView,
	LArray*			inDataArray)

	: LTableStorage(inTableView)
{
	SignalIf_(inDataArray == nil);

	mDataArray = inDataArray;
	mOwnsArray = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LTableArrayStorage					Destructor				  [public]
// ---------------------------------------------------------------------------

LTableArrayStorage::~LTableArrayStorage()
{
	if (mOwnsArray) {
		delete mDataArray;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCellData													  [public]
// ---------------------------------------------------------------------------
//	Store data for a particular Cell

void
LTableArrayStorage::SetCellData(
	const STableCell&	inCell,
	const void*			inDataPtr,
	UInt32				inDataSize)
{
	TableIndexT	cellIndex;
	mTableView->CellToIndex(inCell, cellIndex);

	mDataArray->AssignItemsAt(1, (SInt32) cellIndex, inDataPtr, inDataSize);
}


// ---------------------------------------------------------------------------
//	¥ GetCellData													  [public]
// ---------------------------------------------------------------------------
//	Retrieve data for a particular Cell
//
//	If outDataPtr is nil, pass back the size of the Cell data
//
//	If outDataPtr is not nil, it must point to a buffer of at least
//	ioDataSize bytes. On output, ioDataSize is set to the minimum
//	of the Cell data size and the input value of ioDataSize and that
//	many bytes are copied to outDataPtr.

void
LTableArrayStorage::GetCellData(
	const STableCell&	inCell,
	void*				outDataPtr,
	UInt32&				ioDataSize) const
{
	TableIndexT	cellIndex;
	mTableView->CellToIndex(inCell, cellIndex);

	GetCellDataByIndex(cellIndex, outDataPtr, ioDataSize);
}


// ---------------------------------------------------------------------------
//	¥ GetCellDataByIndex											  [public]
// ---------------------------------------------------------------------------
//	Retrieve data for a particular Cell specified by index
//
//	If outDataPtr is nil, pass back the size of the Cell data
//
//	If outDataPtr is not nil, it must point to a buffer of at least
//	ioDataSize bytes. On output, ioDataSize is set to the minimum
//	of the Cell data size and the input value of ioDataSize and that
//	many bytes are copied to outDataPtr.

void
LTableArrayStorage::GetCellDataByIndex(
	TableIndexT		inCellIndex,
	void*			outDataPtr,
	UInt32&			ioDataSize) const
{
	if (outDataPtr == nil) {
		ioDataSize = mDataArray->GetItemSize((SInt32) inCellIndex);

	} else {
		mDataArray->FetchItemAt((SInt32) inCellIndex, outDataPtr, ioDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ FindCellData													  [public]
// ---------------------------------------------------------------------------
//	Pass back the Cell containing the specified data. Returns whether
//	or not such a Cell was found.

Boolean
LTableArrayStorage::FindCellData(
	STableCell&	outCell,
	const void*	inDataPtr,
	UInt32		inDataSize) const
{
	Boolean	found = false;

	SInt32	dataIndex = mDataArray->FetchIndexOf(inDataPtr, inDataSize);

	if (dataIndex != LArray::index_Bad) {
		mTableView->IndexToCell((TableIndexT) dataIndex, outCell);
		found = true;
	}

	return found;
}


// ---------------------------------------------------------------------------
//	¥ InsertRows													  [public]
// ---------------------------------------------------------------------------
//	Insert rows into an ArrayStorage.
//
//	inDataPtr points to the data for the new cells. Each new cell will
//		have the same data.

void
LTableArrayStorage::InsertRows(
	UInt32		inHowMany,
	TableIndexT	inAfterRow,
	const void*	inDataPtr,
	UInt32		inDataSize)
{
	STableCell	startCell(inAfterRow + 1, 1);
	UInt32		startIndex;
	mTableView->CellToIndex(startCell, startIndex);

	TableIndexT	rows, cols;
	mTableView->GetTableSize(rows, cols);

	mDataArray->InsertItemsAt(inHowMany * cols, (SInt32) startIndex,
								inDataPtr, inDataSize);
}


// ---------------------------------------------------------------------------
//	¥ InsertCols													  [public]
// ---------------------------------------------------------------------------
//	Insert columns into an ArrayStorage.
//
//	inDataPtr points to the data for the new cells. Each new cell will
//		have the same data.

void
LTableArrayStorage::InsertCols(
	UInt32		inHowMany,
	TableIndexT	inAfterCol,
	const void*	inDataPtr,
	UInt32		inDataSize)
{
	TableIndexT	rows, cols;
	mTableView->GetTableSize(rows, cols);

	STableCell	theCell(0, inAfterCol + 1);

	for (theCell.row = 1; theCell.row <= rows; theCell.row++) {
		TableIndexT	startIndex;
		mTableView->CellToIndex(theCell, startIndex);
		mDataArray->InsertItemsAt(inHowMany, (SInt32) startIndex, inDataPtr, inDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveRows													  [public]
// ---------------------------------------------------------------------------
//	Removes rows from an ArrayStorage

void
LTableArrayStorage::RemoveRows(
	UInt32		inHowMany,
	TableIndexT	inFromRow)
{
	STableCell	startCell(inFromRow, 1);
	UInt32		startIndex;
	mTableView->CellToIndex(startCell, startIndex);

	TableIndexT	rows, cols;
	mTableView->GetTableSize(rows, cols);

	mDataArray->RemoveItemsAt(inHowMany * cols, (SInt32) startIndex);
}


// ---------------------------------------------------------------------------
//	¥ RemoveCols													  [public]
// ---------------------------------------------------------------------------
//	Removes columns from an ArrayStorage

void
LTableArrayStorage::RemoveCols(
	UInt32		inHowMany,
	TableIndexT	inFromCol)
{
	TableIndexT	rows, cols;					// Table size AFTER removal
	mTableView->GetTableSize(rows, cols);
	
	if (cols == 0) {						// All columns removed
		mDataArray->RemoveAllItemsAfter(0);	// Remove all data
		
	} else {
		STableCell	theCell(0, inFromCol);

		for (theCell.row = 1; theCell.row <= rows; theCell.row++) {
			TableIndexT	startIndex;
			mTableView->CellToIndex(theCell, startIndex);
			mDataArray->RemoveItemsAt(inHowMany, (SInt32) startIndex);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetStorageSize												  [public]
// ---------------------------------------------------------------------------
//	Pass back the number of rows and columns represented by the data
//	in an ArrayStorage

void
LTableArrayStorage::GetStorageSize(
	TableIndexT&	outRows,
	TableIndexT&	outCols)
{
		// An Array is one-dimensional. By default, we assume a
		// single column with each element being a separate row.

	outRows = mDataArray->GetCount();
	outCols = 1;
	if (outRows == 0) {
		outCols = 0;
	}
}


PP_End_Namespace_PowerPlant
