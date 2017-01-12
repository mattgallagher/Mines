// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LVariableArray.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of variable-size items. Positions in the list are
//	one-based--the first item is at index 1.
//
//	Class maintains two Handles, one stores the data for each item and the
//	other stores the offsets within the data Handle to the start of the
//	data for each item.
//
//	The offsets Handle contains one more entry than the number of items.
//	The last entry is the size of the data Handle, which is the offset
//	of the "next" item (if there was one). This simplifies operations
//	on the offsets Handle dealing with the last item.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LVariableArray.h>
#include <LComparator.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LVariableArray						Constructor				  [public]
// ---------------------------------------------------------------------------

LVariableArray::LVariableArray(
	LComparator*	inComparator,
	Boolean			inKeepSorted)

	: LArray(0, inComparator, inKeepSorted)
{
	mItemOffsetsH   = nil;				// Empty Array
	mItemsAllocated = 0;
}


// ---------------------------------------------------------------------------
//	¥ LVariableArray						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LVariableArray::LVariableArray(
	const LVariableArray&	inOriginal)

	: LArray(inOriginal)
{
	CopyVariableArray(inOriginal, false);
}


// ---------------------------------------------------------------------------
//	¥ LVariableArray						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from existing offsets and data Handles
//
//	Does not validate input Handles.
//		inItemsHandle must a valid Handle containing the data for each item
//		inOffsetsHandle must be a valid Handle containing the offsets (long
//			integers) within inItemsHandle to the data for each item, plus
//			an extra offset at the end that is the size of inItemsHandle

LVariableArray::LVariableArray(
	Handle			inItemsHandle,
	ArrayOffsetH	inOffsetsHandle,
	LComparator*	inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)

	: LArray(0, nil, inComparator, inIsSorted, inKeepSorted)
{
	mItemOffsetsH   = nil;
	mItemsAllocated = 0;

	if (inItemsHandle != nil) {
		::HUnlock(inItemsHandle);
		mItemsH			= inItemsHandle;
		mDataStored		= ::GetHandleSize(inItemsHandle);
		mDataAllocated	= mDataStored;

		SignalIf_(inOffsetsHandle == nil);

		mItemOffsetsH	= inOffsetsHandle;
		mItemCount		= ::GetHandleSize((Handle) inOffsetsHandle) /
								sizeof(ArrayOffsetT) - 1;
		mItemsAllocated	= mItemCount;
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ operator =							Assignment Operator		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Disposes VariableArray's existing data and copies data of the
//	specified VariableArray

LVariableArray&
LVariableArray::operator = (
	const LVariableArray&	inOriginal)
{
	if (IsLocked()) {
		SignalStringLiteral_("Can't assign to a locked VariableArray");

	} else if (this != &inOriginal) {		// Check for self assignment

		LArray::operator=(inOriginal);			// Base class assignment

		CopyVariableArray(inOriginal, true);	// Copy data from rhs
	}

	return *this;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CopyVariableArray												 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Copy data members of a VariableArray

void
LVariableArray::CopyVariableArray(
	const LVariableArray&	inOriginal,
	bool					inDestroyOldData)
{
		// Duplicate original's item offsets. Original may have
		// more space allocated than needed to store the item
		// offsets. To save memory, our copy allocates only what
		// it needs to store the offsets and not any excess space.

	UInt32			copyItemsAllocated	= inOriginal.mItemCount;
	ArrayOffsetH	copyOffsetsH		= nil;

	if (copyItemsAllocated > 0) {
		SInt32	offsetsSize = (SInt32) ((copyItemsAllocated + 1) *
												sizeof(ArrayOffsetT));
		copyOffsetsH = (ArrayOffsetH) ::NewHandle(offsetsSize);
		ThrowIfMemError_();

		::BlockMoveData(*inOriginal.mItemOffsetsH, *copyOffsetsH,
							offsetsSize);
	}

	if (inDestroyOldData) {		// Duplication succeeded. It's now safe
		DestroyVariableArray();	//   to delete our old data.
	}

	mItemsAllocated	= copyItemsAllocated;
	mItemOffsetsH	= copyOffsetsH;
}


// ---------------------------------------------------------------------------
//	¥ ~LVariableArray							Destructor			  [public]
// ---------------------------------------------------------------------------

LVariableArray::~LVariableArray()
{
	DestroyVariableArray();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DestroyVariableArray											 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

void
LVariableArray::DestroyVariableArray()
{
	if (mItemOffsetsH != nil) {		// Delete Handle of item offsets
		::DisposeHandle((Handle) mItemOffsetsH);
		mItemOffsetsH   = nil;
		mItemsAllocated = 0;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetItemSize													  [public]
// ---------------------------------------------------------------------------
//	Return the size in bytes of an item in the VariableArray

UInt32
LVariableArray::GetItemSize(
	ArrayIndexT		inIndex) const
{
	UInt32	itemSize = 0;

	if (ValidIndex(inIndex)) {
		itemSize = (*mItemOffsetsH)[inIndex] - (*mItemOffsetsH)[inIndex - 1];
	}

	return itemSize;
}


// ---------------------------------------------------------------------------
//	¥ GrabItemSize												   [protected]
// ---------------------------------------------------------------------------
//	Return the size in bytes of the specified item
//
//	Same as GetItemSize() except it doesn't validate inIndex

UInt32
LVariableArray::GrabItemSize(
	ArrayIndexT		inIndex) const
{
	return (*mItemOffsetsH)[inIndex] - (*mItemOffsetsH)[inIndex - 1];
}


// ---------------------------------------------------------------------------
//	¥ GetOffsetsHandle												  [public]
// ---------------------------------------------------------------------------
//	Return Handle used to store offsets to the data for Array items
//
//	Usage Note: Treat the Handle as read-only. Changing the data in the
//	Handle could invalidate the internal state of the Array.
//
//	In general, you should only access the offsets Handle in order to
//	copy it (either in memory or by writing it to a file).

Handle
LVariableArray::GetOffsetsHandle() const
{
											// Remove excess allocation
											// Cast away const
	(const_cast<LVariableArray*>(this))->AdjustAllocation(0, 0);
	return (Handle) mItemOffsetsH;
}


// ---------------------------------------------------------------------------
//	¥ AssignItemsAt													  [public]
// ---------------------------------------------------------------------------
//	Assign the same value to Items starting at the specified index
//
//	inValue is a pointer to the item data. The Array makes and stores
//	a copy of the item data.
//
//	Does nothing if inIndex is out of range
//
//	Returns index of first "assigned" item. This may be different from
//	inAtIndex if the array is sorted. Returns LArray::index_Bad if
//	inAtIndex is out of range.

ArrayIndexT
LVariableArray::AssignItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inValue,
	UInt32			inItemSize)
{
	if (ValidIndex(inAtIndex) && (inCount > 0)) {

		RemoveItemsAt(inCount, inAtIndex);
		inAtIndex = InsertItemsAt(inCount, inAtIndex, inValue, inItemSize);

	} else {
		inAtIndex = index_Bad;
	}

	return inAtIndex;
}


// ---------------------------------------------------------------------------
//	¥ GetItemPtr													  [public]
// ---------------------------------------------------------------------------
//	Return a pointer to the start of an Item's data within the internal
//	storage Handle
//
//	Performs no error checking on the Index.
//
//	WARNING: Unless you have called Lock(), the returned pointer points
//	to data within a relocatable Handle block.

void*
LVariableArray::GetItemPtr(
	ArrayIndexT		inIndex) const
{
	return (*mItemsH + (*mItemOffsetsH)[inIndex - 1]);
}


// ---------------------------------------------------------------------------
//	¥ AdjustAllocation												  [public]
// ---------------------------------------------------------------------------
//	Adjust the size of the Handle used to store Array items.
//
//	You can call this routine to pre-allocate space for additional
//	items to be added later. This can reduce the calls to the
//	Toolbox routine SetHandleSize() when new items are inserted.
//
//	inExtraItems is an actual value, not relative.
//	So if there is currently space for 2 extra items and you call
//	AdjustAllocation(3), you will get 3 extra items, not 5.
//
//	inExtraData is the number of bytes of additional space to allocate
//	for storing the data for the Array items

void
LVariableArray::AdjustAllocation(
	UInt32		inExtraItems,
	UInt32		inExtraData)
{
	InternalAdjustAllocation(mItemCount + inExtraItems,
							 mDataStored + inExtraData);
}


// ---------------------------------------------------------------------------
//	¥ InternalAdjustAllocation									   [protected]
// ---------------------------------------------------------------------------
//	Called internally to change the size of the storage used
//
//	The amount of storage allocated may be larger than the amount
//	actually used.
//
//	inItemAllocation is the number of items to allocate space for
//	inDataAllocation is the amount of data to allocate space for

void
LVariableArray::InternalAdjustAllocation(
	UInt32		inItemAllocation,
	UInt32		inDataAllocation)
{
	LArray::InternalAdjustAllocation(inItemAllocation, inDataAllocation);

	if (inItemAllocation != mItemsAllocated) {
		SInt32	offsetsSize = (SInt32) ((inItemAllocation + 1) * sizeof(ArrayOffsetT));

		if (mItemOffsetsH == nil) {	// First allocation
			mItemOffsetsH = (ArrayOffsetH) ::NewHandle(offsetsSize);
			ThrowIfMemFail_(mItemOffsetsH);
			(*mItemOffsetsH)[0] = 0;

			if (mItemsH == nil) {	// LArray hasn't allocated a data Handle
									//   because all items have zero length
									//   data. Allocate an empty data Handle
									//   so our item pointers are valid.
				mItemsH = ::NewHandle(0);
				mDataAllocated = 0;
			}

		} else {					// Resize existing allocation
			::SetHandleSize((Handle) mItemOffsetsH, offsetsSize);
			if (mItemsAllocated == 0) {
				(*mItemOffsetsH)[0] = 0;
			}
		}

		ThrowIfMemError_();
		mItemsAllocated = inItemAllocation;
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustStorage												   [protected]
// ---------------------------------------------------------------------------
//	Called internally when the number of bytes used by Items in the Array
//	changes.
//
//	inDeltaItems is the change in the number of items in the Array
//	inDeltaData is the change in the amount of data stored for all items

void
LVariableArray::AdjustStorage(
	SInt32		inDeltaItems,
	SInt32		inDeltaData)
{
	UInt32		newItemCount = mItemCount + inDeltaItems;
	SInt32		newDataStored = mDataStored + inDeltaData;

	UInt32		newItemAllocation = mItemsAllocated + inDeltaItems;
	if (newItemCount > mItemsAllocated) {	// Adding items
		newItemAllocation = mItemCount + inDeltaItems;
	}

	UInt32		newDataAllocation = (UInt32) (mDataAllocated + inDeltaData);

	InternalAdjustAllocation(newItemAllocation, newDataAllocation);

	mItemCount = newItemCount;
	mDataStored = newDataStored;

	(*mItemOffsetsH)[newItemCount] = (ArrayOffsetT) newDataStored;
}


// ---------------------------------------------------------------------------
//	¥ Sort															  [public]
// ---------------------------------------------------------------------------
//	Sort items in the Array

void
LVariableArray::Sort()
{
	if (mIsSorted || (mItemCount <= 1)) {
		return;
	}

		// Shellsort Algorithm

	SInt32	hh = 1;						// Find starting "h" value
	SInt32	stopH = (SInt32) mItemCount / 9;
	while (hh < stopH) {
		hh = 3 * hh + 1;
	}

	for ( ; hh > 0; hh /= 3) {

		for (SInt32 step = hh + 1; step <= mItemCount; step++) {
			UInt32	stepItemSize = GrabItemSize(step);
			StPointerBlock	buffer((SInt32) stepItemSize);
			PeekItem(step, buffer);

			SInt32	i;
			for (i = step - hh; i > 0; i -= hh) {
				{
					StHandleLocker	lockItems(mItemsH);
					if (mComparator->Compare(buffer, GetItemPtr(i),
									stepItemSize, GrabItemSize(i)) >= 0) {
						break;
					}
				}
				InternalCopyItem(i, i + hh);
			}

			PokeItem(i + hh, buffer, stepItemSize);
		}
	}

	mIsSorted = true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PokeItem													   [protected]
// ---------------------------------------------------------------------------
//	Store data for the Item at the specified index
//
//	Used internally to write Item data

void
LVariableArray::PokeItem(
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	SInt32	sizeChange = (SInt32) (inItemSize - GrabItemSize(inAtIndex));

	if (sizeChange != 0) {

		SInt32	moveSize = (SInt32) GrabItemRangeSize(inAtIndex + 1, (ArrayIndexT) mItemCount);

		if (sizeChange > 0) {		// Grow item data storage
			AdjustStorage(0, sizeChange);
		}

		if (inAtIndex < mItemCount) {
									// Not the last item, so we have to
									//    move the data for the items
									//    beyond the target item
			::BlockMoveData(GetItemPtr(inAtIndex + 1),
						(Ptr) GetItemPtr(inAtIndex) + inItemSize, moveSize);
		}

									// Adjust offsets
		for (ArrayIndexT i = inAtIndex; i < mItemCount; i++) {
			(*mItemOffsetsH)[i] += (ArrayOffsetT) sizeChange;
		}

		if (sizeChange < 0) {		// Shrink storage
			AdjustStorage(0, sizeChange);
		}
	}

									// Store data for the item
	if (inItemSize > 0) {
		::BlockMoveData(inItem, GetItemPtr(inAtIndex), (SInt32) inItemSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ ShiftItems												   [protected]
// ---------------------------------------------------------------------------
//	Moves items within the Handle used for internal storage
//	Moves items in the range inStartIndex to inEndIndex (inclusive)
//
//	For internal use. Performs no bounds checking.

void
LVariableArray::ShiftItems(
	ArrayIndexT		inStartIndex,
	ArrayIndexT		inEndIndex,
	SInt32			inIndexShift,
	SInt32			inDataShift)
{
								// Move Item data
	::BlockMoveData( GetItemPtr(inStartIndex),
					 (SInt8*) GetItemPtr(inStartIndex) + inDataShift,
					 (SInt32) GrabItemRangeSize(inStartIndex, inEndIndex) );

								// Adjust data offsets
	if (inIndexShift > 0) {
		for (ArrayIndexT i = inEndIndex; i >= inStartIndex; i--) {
			(*mItemOffsetsH)[i + inIndexShift - 1] =
						(*mItemOffsetsH)[i - 1] + inDataShift;
		}
	} else {
		for (ArrayIndexT i = inStartIndex; i <= inEndIndex; i++) {
			(*mItemOffsetsH)[i + inIndexShift] =
						(*mItemOffsetsH)[i] + inDataShift;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ StoreNewItems												   [protected]
// ---------------------------------------------------------------------------
//	Store values within the internal storage Handle.
//	Items all have the same value, and space must already have
//	been allocated for them.
//
//	For internal use. Performs no bounds checking.

void
LVariableArray::StoreNewItems(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	ArrayIndexT	lastIndex = (ArrayIndexT) (inAtIndex + inCount - 1);

										// Adjust offsets
	for (ArrayIndexT j = inAtIndex + 1; j <= lastIndex; j++) {
		(*mItemOffsetsH)[j - 1] = (*mItemOffsetsH)[j - 2] + inItemSize;
	}

	if (inItem != nil) {				// Store item value
		for (ArrayIndexT i = inAtIndex; i <= lastIndex; i++) {
			::BlockMoveData(inItem, GetItemPtr(i), (SInt32) inItemSize);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ InternalCopyItem											   [protected]
// ---------------------------------------------------------------------------
//	Set the item at inDestIndex to a copy of the item at inSourceIndex
//
//	Internal routine. Performs no range checking.

void
LVariableArray::InternalCopyItem(
	ArrayIndexT		inSourceIndex,
	ArrayIndexT		inDestIndex)
{
	SInt32	copySize = (SInt32) GrabItemSize(inSourceIndex);
	SInt32	sizeChange = (SInt32) (copySize - GrabItemSize(inDestIndex));

	if (sizeChange != 0) {

		SInt32	moveSize = (SInt32) GrabItemRangeSize(inDestIndex + 1, (ArrayIndexT) mItemCount);

		if (sizeChange > 0) {		// Grow item data storage
			AdjustStorage(0, sizeChange);
		}

		if (inDestIndex < mItemCount) {
									// Not the last item, so we have to
									//    move the data for the items
									//    beyond the destination item
			::BlockMoveData(GetItemPtr(inDestIndex + 1),
						(Ptr) GetItemPtr(inDestIndex) + copySize, moveSize);
		}

									// Adjust offsets
		for (ArrayIndexT i = inDestIndex; i < mItemCount; i++) {
			(*mItemOffsetsH)[i] += (ArrayOffsetT) sizeChange;
		}

		if (sizeChange < 0) {		// Shrink storage
			AdjustStorage(0, sizeChange);
		}
	}

									// Copy data to destination
	::BlockMoveData( GetItemPtr(inSourceIndex), GetItemPtr(inDestIndex),
					 copySize);
}


// ---------------------------------------------------------------------------
//	¥ GrabItemRangeSize											   [protected]
// ---------------------------------------------------------------------------
//	Return the size in bytes of the items from inStartIndex to
//	inEndIndex, inclusive

UInt32
LVariableArray::GrabItemRangeSize(
	ArrayIndexT		inStartIndex,
	ArrayIndexT		inEndIndex) const
{
	return (*mItemOffsetsH)[inEndIndex] - (*mItemOffsetsH)[inStartIndex-1];
}


// ---------------------------------------------------------------------------
//	¥ GetItemOffset										  [inline] [protected]
// ---------------------------------------------------------------------------
//	Return the offset in bytes of the start of the item within the
//	data Handle

#pragma mark LVariableArray::GetItemOffset

// UInt32
// LVariableArray::GetItemOffset(
//		ArrayIndexT		inAtIndex)


PP_End_Namespace_PowerPlant
