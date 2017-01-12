// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LArray.cp					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A ordered sequence of fixed-sized items. Positions in the Array are
//	one-based--the first item is at index 1.
//
//	Item data is stored in a resizable Handle block, so items can be
//	dynamically added and removed. Also supports sorting and iteration
//	(using the friend class LArrayIterator).

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LArray.h>
#include <LArrayIterator.h>
#include <LComparator.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


#pragma mark --- Construction & Destruction ---

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LArray								Default Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Creates an empty, unsorted array of pointers

LArray::LArray()
{
	InitArray(sizeof(void*), nil, true, false);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LArray								Constructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Creates an empty array of items of a specified size
//
//	The Comparator object compares Array items and must be allocated
//	in the heap via "new". The Array assumes ownership of the
//	Comparator and is responsible for deleting it.
//
//	inKeepSorted specifies whether to keep the Array sorted when
//	items are inserted or assigned new values.

LArray::LArray(
	UInt32			inItemSize,
	LComparator*	inComparator,
	Boolean			inKeepSorted)
{
	InitArray(inItemSize, inComparator, true, inKeepSorted);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LArray								Constructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Creates an array with space pre-allocated for the specified number of
//	items of the specified size.
//
//	The Comparator object compares Array items and must be allocated
//	in the heap via "new". The Array assumes ownership of the
//	Comparator and is responsible for deleting it.
//
//	inKeepSorted specifies whether to keep the Array sorted when
//	items are inserted or assigned new values.

LArray::LArray(
	UInt32			inItemCount,
	UInt32			inItemSize,
	LComparator*	inComparator,
	Boolean			inKeepSorted)
{
	InitArray(inItemSize, inComparator, true, inKeepSorted);
	AdjustAllocation(inItemCount);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LArray								Constructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Constructor for an Array based on an existing Handle of items
//
//	Array takes ownership of the items Handle

LArray::LArray(
	UInt32			inItemSize,
	Handle			inItemsHandle,
	LComparator*	inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)
{
	InitArray(inItemSize, inComparator, inIsSorted, inKeepSorted);

	if (inItemsHandle != nil) {
		::HUnlock(inItemsHandle);
		mItemsH			= inItemsHandle;
		mDataStored		= ::GetHandleSize(inItemsHandle);
		mDataAllocated	= mDataStored;
		mItemCount		= mDataStored / inItemSize;
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ LArray								Copy Constructor		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LArray::LArray(
	const LArray&	inOriginal)
{
	CopyArray(inOriginal, false);		// Use private copier
}										// See comments there for more info


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ operator =							Assignment Operator		  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Disposes Array's existing data and copies data of the specified Array
//
//	See comments for CopyArray() for detailed information about how
//	the copy is done.

LArray&
LArray::operator = (
	const LArray&	inArray)
{
	if (IsLocked()) {
		SignalStringLiteral_("Can't assign to a locked Array");

	} else if (this != &inArray) {		// Check for self assignment

		CopyArray(inArray, true);		// Copy data from assigned Array
	}

	return *this;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ InitArray														 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Initialize data members

void
LArray::InitArray(
	UInt32			inItemSize,
	LComparator*	inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)
{
	mItemSize			= inItemSize;
	mItemCount			= 0;
	mDataStored			= 0;
	mDataAllocated		= 0;
	mItemsH				= nil;
	mLockCount			= 0;
	mIteratorListHead	= nil;

	mComparator			= inComparator;
	mOwnsComparator		= true;
	mIsSorted			= inIsSorted;
	mKeepSorted			= inKeepSorted;

	if (inComparator == nil) {
		mOwnsComparator = false;
		if (inItemSize == sizeof(long)) {
			mComparator = LLongComparator::GetComparator();
		} else {
			mComparator = LComparator::GetComparator();
		}
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CopyArray														 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Creates a deep copy by duplicating the items in the Array. However,
//	if the items in the Array are pointers to other objects, those
//	other objects aren't duplicated.
//
//	Also duplicates the Comparator if the original owns its Comparator.
//	NOTE: If you have Arrays which own a custom Comparator, you MUST
//	override Clone() for your subclass of LComparator in order for this
//	Array copy constructor to work properly.
//
//	Copy is always unlocked, even if original is locked. This ensures
//	that Lock() and Unlock() calls always balance. If the copy were
//	locked (because the original is locked), then to change the copy
//	you would have to unlock it without ever having called Lock().
//	And tracking the number of times the copy is locked would be
//	a big problem.
//
//	Copy does NOT duplicate original's iterators, since there would be
//	no way to access such iterators. The list of iterators is an
//	internal implementation detail.
//
//	inDestroyData specifies whether to delete this Array's data before
//	storing the copied data. operator= passes true since assigment
//	overwrites an existing Array. Copy constructor passes false since
//	it is initializing a new Array and there is no existing data.

void
LArray::CopyArray(
	const LArray&	inOriginal,
	bool			inDestroyOldData)
{
		// We first try to duplicate the Handle containing the item
		// data and clone the Comparator because doing so could fail
		// due to memory exhaustion. If we do fail, we abort the
		// copy by throwing an exception.

		// Duplicate original's items. Original may have more space
		// allocated than needed to store its items. To save memory,
		// our copy allocates only what it needs to store the data
		// and not any excess space.

	Handle	copyHandle = nil;
	SInt32	sizeToCopy = inOriginal.mDataStored;

	if (sizeToCopy > 0) {
		copyHandle = ::NewHandle(sizeToCopy);		// Allocate space
		ThrowIfMemError_();							// Out of Memory!
													// Copy the data
		::BlockMoveData(*inOriginal.mItemsH, *copyHandle, sizeToCopy);
	}

											// Copy Comparator
	LComparator* copyComparator = nil;

	if (inOriginal.mOwnsComparator) {
										// Original owns its Comparator
		try {							// So we must clone our own copy
			copyComparator = inOriginal.mComparator->Clone();
		}

		catch (...) {							// Clone failed (probably out
			if (copyHandle != nil) {			//   of memory). Copy is not
				::DisposeHandle(copyHandle);	//   complete, so dispose of
			}									//   duplicated items and
			throw;								//   rethrow the exception
		}

	} else {							// Original shares Comparator, so
										//   we can just copy the pointer
		copyComparator	= inOriginal.mComparator;
	}

		// Memory allocations have succeeded. If we were overwriting
		// an existing Array, it's now safe to destroy old data.

	if (inDestroyOldData) {
		DestroyArray();
	}

		// Now we can just set our member variable values from
		// those of the original.

	mItemSize			= inOriginal.mItemSize;
	mItemCount			= inOriginal.mItemCount;
	mDataStored			= sizeToCopy;
	mDataAllocated		= sizeToCopy;
	mItemsH				= copyHandle;
	mComparator			= copyComparator;
	mOwnsComparator 	= inOriginal.mOwnsComparator;
	mIsSorted			= inOriginal.mIsSorted;
	mKeepSorted			= inOriginal.mKeepSorted;

	mLockCount			= 0;			// Copy is always unlocked
	mIteratorListHead	= nil;			// Copy has no attached iterators
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ ~LArray								Destructor				  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

LArray::~LArray()
{
	if (mLockCount > 0) {
		SignalStringLiteral_("Deleting a locked Array");
	}

	DestroyArray();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DestroyArray													 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Destroy internal data of Array

void
LArray::DestroyArray()
{
	if (mOwnsComparator) {			// Delete custom Comparator object
		delete mComparator;
		mComparator = nil;
	}

	if (mItemsH != nil) {			// Dispose Item storage space
		::DisposeHandle(mItemsH);
		mItemsH = nil;
	}

									// Notify Iterators of our death
	LArrayIterator	*theIter = mIteratorListHead;
	while (theIter != nil) {
		theIter->ArrayDied();
		theIter = theIter->GetNextIterator();
	}
}

#pragma mark --- Inspectors ---

// ---------------------------------------------------------------------------
//	¥ ValidIndex													  [public]
// ---------------------------------------------------------------------------
//	Return whether an index is valid (between 1 and the number of items)
//	for the Array.
//
//	If ioIndex is the special flag index_Last, the index's value
//	is changed to the actual index of the last item.

bool
LArray::ValidIndex(
	ArrayIndexT&	ioIndex) const
{
	if (ioIndex == index_Last) {
		ioIndex = (ArrayIndexT) mItemCount;
	}
	return (ioIndex > 0) && (ioIndex <= mItemCount);
}


// ---------------------------------------------------------------------------
//	¥ GetItemSize											 [inline] [public]
// ---------------------------------------------------------------------------
//	Return the size in bytes of an item in the Array

#pragma mark LArray::GetItemSize

// UInt32
// LArray::GetItemSize(
//	ArrayIndexT	inIndex) const					// Defined inline


// ---------------------------------------------------------------------------
//	¥ GrabItemSize											 [inline] [public]
// ---------------------------------------------------------------------------
//	Return the size in bytes of an item in the Array
//
//	Same as GetItemSize() except that the index is assumed to be valid

#pragma mark LArray::GrabItemSize

// UInt32
// LArray::GrabItemSize(
//	ArrayIndexT	inIndex) const					// Defined inline



// ---------------------------------------------------------------------------
//	¥ GetCount												 [inline] [public]
// ---------------------------------------------------------------------------
//	Return the number of items in an Array

#pragma mark LArray::GetCount

// UInt32
// LArray::GetCount() const						// Defined inline


// ---------------------------------------------------------------------------
//	¥ IsSorted												 [inline] [public]
// ---------------------------------------------------------------------------
//	Return whether the Array is currently sorted

#pragma mark LArray::IsSorted

// bool
// LArray::IsSorted() const						// Defined inline


// ---------------------------------------------------------------------------
//	¥ InvalidateSort										 [inline] [public]
// ---------------------------------------------------------------------------
//	Mark Array as unsorted
//
//	Call this function if you alter the contents of the Array through
//	indirect means. For example, if you use GetItemPtr() to get a pointer
//	to an Array item and then modify the item via that pointer.

#pragma mark LArray::InvalidateSort

// void
// LArray::InvalidateSort()


// ---------------------------------------------------------------------------
//	¥ IsKeptSorted											 [inline] [public]
// ---------------------------------------------------------------------------
//	Return whether the Array is kept sorted when items are added

#pragma mark LArray::IsKeptSorted

// bool
// LArray::IsKeptSorted() const


// ---------------------------------------------------------------------------
//	¥ GetItemsHandle												  [public]
// ---------------------------------------------------------------------------
//	Return Handle used to store data for Array items
//
//	Usage Note: Treat the Handle as read-only. Changing the data in the
//	Handle could invalidate the internal state of the Array, particularly if
//	it is sorted
//
//	In general, you should only access the items Handle in order to
//	copy it (either in memory or by writing it to a file).

Handle
LArray::GetItemsHandle() const
{
											// Remove excess allocation
	(const_cast<LArray*>(this))->AdjustAllocation(0);	// Cast away const
	return mItemsH;
}


// ---------------------------------------------------------------------------
//	¥ FetchItemAt													  [public]
// ---------------------------------------------------------------------------
//	Pass back the Item at the specified index
//
//	Returns true if an item exists at inIndex (and sets outItem)
//	Returns false if inIndex is out of range (and leaves outItem unchanged)
//
//	Caller must make sure that outItem points a buffer large enough to
//	hold the item data

Boolean
LArray::FetchItemAt(
	ArrayIndexT		inAtIndex,
	void*			outItem) const
{
	Boolean	itemExists = ValidIndex(inAtIndex);

	if (itemExists) {
		PeekItem(inAtIndex, outItem);
	}

	return itemExists;
}


// ---------------------------------------------------------------------------
//	¥ FetchItemAt													  [public]
// ---------------------------------------------------------------------------
//	Pass back the Item at the specified index
//
//	Returns true if an item exists at inIndex (and sets outItem)
//	Returns false if inIndex is out of range (and leaves outItem unchanged)
//
//	Copies data to the buffer pointed to by outItem. Number of bytes copied
//	is the minimum of the size of the item and ioItemSize.
//
//	On input, ioItemSize is the maximum number of bytes to copy.
//	On output, ioItemSize is the actual number of bytes copied
//
//	Use this form of the function (as opposed to the one above) if you
//	only want to fetch a portion of the data for an item.

Boolean
LArray::FetchItemAt(
	ArrayIndexT		inAtIndex,
	void*			outItem,
	UInt32&			ioItemSize) const
{
	Boolean	itemExists = ValidIndex(inAtIndex);

	if (itemExists) {
		UInt32	dataSize = GrabItemSize(inAtIndex);

		if (dataSize == ioItemSize) {
			PeekItem(inAtIndex, outItem);

		} else {
			if (dataSize > ioItemSize) {
				dataSize = ioItemSize;
			}

			::BlockMoveData(GetItemPtr(inAtIndex), outItem, (SInt32) dataSize);
			ioItemSize = dataSize;
		}
		
	} else {
		ioItemSize = 0;					// Item does not exist
	}

	return itemExists;
}


// ---------------------------------------------------------------------------
//	¥ FetchIndexOf													  [public]
// ---------------------------------------------------------------------------
//	Returns the index of the specified item within the Array
//
//	Returns index_Bad if the item is not in the Array

ArrayIndexT
LArray::FetchIndexOf(
	const void*		inItem,
	UInt32			inItemSize) const
{
	ArrayIndexT	index = index_Bad;

	if (mItemCount > 0) {
		if (inItemSize == 0) {
			inItemSize = mItemSize;
		}

		if (mIsSorted) {
			index = BinarySearch(inItem, inItemSize);
		} else {
			index = LinearSearch(inItem, inItemSize);
		}
	}

	return index;
}


// ---------------------------------------------------------------------------
//	¥ ContainsItem											 [inline] [public]
// ---------------------------------------------------------------------------
//	Return whether the specifed item is in the Array

#pragma mark LArray::ContainsItem

// bool
// LArray::ContainsItem(
//		const void*		inItem,
//		UInt32			inItemSize) const

#pragma mark --- Manipulators ---


// ---------------------------------------------------------------------------
//	¥ AddItem														  [public]
// ---------------------------------------------------------------------------
//	Add one item to Array
//
//	If the Array is not sorted, add the item to the end of the Array.
//
//	For unsorted Arrays, this function is a faster version of
//	InsertItemsAt(1, LArray::index_Last, inItem, inItemSize) since it
//	doesn't have to bother with checking/adjusting the count and
//	insertion index.

ArrayIndexT
LArray::AddItem(
	const void*		inItem,
	UInt32			inItemSize)
{
	if (mKeepSorted) {
		return InsertItemsAt(1, 0, inItem, inItemSize);
	}

	if (mLockCount > 0) {
		SignalStringLiteral_("Can't add item to a locked Array");
		return index_Bad;
	}

	if (inItemSize == 0) {
		inItemSize = mItemSize;
	}

									// Add item at end
	ArrayIndexT	atIndex = (ArrayIndexT) mItemCount + 1;

	mIsSorted = false;				// Array is no longer sorted

	AdjustStorage(1, (SInt32) inItemSize);
	PokeItem(atIndex, inItem, inItemSize);
	ItemsInserted(1, atIndex);

	return atIndex;
}


// ---------------------------------------------------------------------------
//	¥ InsertItemsAt													  [public]
// ---------------------------------------------------------------------------
//	Insert items at the specified position in an Array
//
//	inCount items are inserted into the Array starting at inAtIndex.
//
//	All items are set to the same value, as specified by inItem.
//	inItem may be nil, in which case the data for the inserted items
//	is unspecified (but space is allocated)
//
//	inAtIndex is adjusted if necessary:
//		> to sorted position if Array is kept sorted
//		> to after last item if inAtIndex is too big
//		> to 1 if inAtIndex is too small
//
//	Returns index at which items were inserted. This can differ from
//	the input value of inAtIndex as described in the paragraph above.
//
//	Errors:
//		Insertion can fail if there is not enough memory to store
//		the new items

ArrayIndexT
LArray::InsertItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	if (inCount < 1) {				// Exit if nothing to insert
		return index_Bad;
	}

	if (mLockCount > 0) {
		SignalStringLiteral_("Can't insert into a locked Array");
		return index_Bad;
	}

	if (mKeepSorted && (inItem == nil)) {
		SignalStringLiteral_("Can't insert nil item into a sorted Array");
		return index_Bad;
	}

	if (inItemSize == 0) {
		inItemSize = mItemSize;
	}

									// Adjust Insertion Index if necessary

	if (mKeepSorted) {				// Find sorted position
		if (!mIsSorted) {			// Guarantee that Array is sorted
			Sort();
		}
		inAtIndex = FetchInsertIndexOf(inItem, inItemSize);
	}
									// Check upper and lower bound
	if (inAtIndex > mItemCount) {
									// Index too big, insert at end
		inAtIndex = (ArrayIndexT) mItemCount +1;

	} else if (inAtIndex < 1) {
		inAtIndex = 1;				// Index too small, insert at start
	}

	UInt32		beforeCount = mItemCount;

	AdjustStorage((SInt32) inCount, (SInt32) (inCount * inItemSize));

	if ((inAtIndex <= beforeCount) && (beforeCount > 0)) {
									// Move existing items to make
									//   room for new ones
		ShiftItems(inAtIndex, (ArrayIndexT) beforeCount, (SInt32) inCount,
					(SInt32) (inCount * inItemSize));
	}

	mIsSorted = mKeepSorted;
									// Inserted items are all set
									//   to the same value, if specified
	StoreNewItems(inCount, inAtIndex, inItem, inItemSize);

	ItemsInserted(inCount, inAtIndex);

	return inAtIndex;
}


// ---------------------------------------------------------------------------
//	¥ RemoveItemsAt													  [public]
// ---------------------------------------------------------------------------
//	Remove items from an Array starting at a specified position
//
//	Does nothing if inAtIndex is out of range. Checks if inCount would remove
//	items past the end of the Array, and adjusts it accordingly to remove
//	the items from inAtIndex to the end of the Array. That means you can pass
//	a large number to remove the items from inAtIndex to the end of the Array.

void
LArray::RemoveItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex)
{
	if (mLockCount > 0) {
		SignalStringLiteral_("Can't remove from a locked Array");
		return;
	}

	if (ValidIndex(inAtIndex)) {

		if (inAtIndex + inCount > mItemCount) {
									// Limit inCount to the number of items
									//   from inAtIndex to the end
			inCount = mItemCount - inAtIndex + 1;
		}

		SInt32	bytesRemoved =
				(SInt32) GrabItemRangeSize(inAtIndex, inAtIndex + (SInt32) inCount - 1);

		if (inAtIndex + inCount <= mItemCount) {
									// Removing items from the middle
									// Shift down items that are above
									//   the ones being removed
			ShiftItems(inAtIndex + (SInt32) inCount, (SInt32) mItemCount, (SInt32) -inCount,
					-bytesRemoved);
		}

		AdjustStorage((SInt32) -inCount, -bytesRemoved);

		ItemsRemoved(inCount, inAtIndex);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllItemsAfter									 [inline] [public]
// ---------------------------------------------------------------------------
//	Remove all items after, but not including, the specified index

#pragma mark LArray::RemoveAllItemsAfter

// void
// LArray::RemoveAllItemsAfter(
//		ArrayIndexT		inAfterIndex)


// ---------------------------------------------------------------------------
//	¥ RemoveLastItem												  [public]
// ---------------------------------------------------------------------------
//	Pass back last item and remove it from the Array
//
//	Returns false if the Array is empty or locked
//	Returns true if last item exists and was removed
//
//	Caller must make sure that outItem points to a buffer large enough
//	to hold the item

bool
LArray::RemoveLastItem(
	void*		outItem)
{
	bool	itemRemoved = false;

	if (mItemCount > 0) {

		if (mLockCount > 0) {
			SignalStringLiteral_("Can't remove from a locked Array");

		} else {
			FetchItemAt((ArrayIndexT) mItemCount, outItem);
			RemoveItemsAt(1, (ArrayIndexT) mItemCount);
			itemRemoved = true;
		}
	}

	return itemRemoved;
}


// ---------------------------------------------------------------------------
//	¥ RemoveLastItem												  [public]
// ---------------------------------------------------------------------------
//	Pass back last item and remove it from the Array
//
//	On input, ioItemSize is the maximum number of bytes to copy.
//	On output, ioItemSize is the actual number of bytes copied
//
//	Returns false if the Array is empty or locked
//	Returns true if last item exists and was removed

bool
LArray::RemoveLastItem(
	void*		outItem,
	UInt32&		ioItemSize)
{
	bool	itemRemoved = false;

	if (mItemCount > 0) {

		if (mLockCount > 0) {
			SignalStringLiteral_("Can't remove from a locked Array");

		} else {
			FetchItemAt((ArrayIndexT) mItemCount, outItem, ioItemSize);
			RemoveItemsAt(1, (ArrayIndexT) mItemCount);
			itemRemoved = true;
		}
	}

	return itemRemoved;
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
LArray::AssignItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inValue,
	UInt32			inItemSize)
{
	if (ValidIndex(inAtIndex) && (inCount > 0)) {

		if (mKeepSorted) {
			RemoveItemsAt(inCount, inAtIndex);
			inAtIndex = InsertItemsAt(inCount, inAtIndex,
										inValue, inItemSize);

		} else {
			ArrayIndexT		lastIndex = (ArrayIndexT) (inAtIndex + inCount - 1);
			if (lastIndex > mItemCount) {
				lastIndex = (ArrayIndexT) mItemCount;	// Don't go past end of Array
			}

			if (inItemSize == 0) {
				inItemSize = mItemSize;
			}

			for (ArrayIndexT i = inAtIndex; i <= lastIndex; i++) {
				PokeItem(i, inValue, inItemSize);
			}

			mIsSorted = false;			// Changing items invalidates sort
		}
	} else {
		inAtIndex = index_Bad;
	}

	return inAtIndex;
}


// ---------------------------------------------------------------------------
//	¥ SwapItems														  [public]
// ---------------------------------------------------------------------------
//	Swap the values of the Items at the specified indexes
//
//	Does nothing if either index is out of range or if Array is kept sorted
//	(since swapping could invalidate the sort).

void
LArray::SwapItems(
	ArrayIndexT		inIndexA,
	ArrayIndexT		inIndexB)
{
									// Do nothing if Array is kept sorted or
									//   if either index is out of range
	if (!mKeepSorted && ValidIndex(inIndexA) && ValidIndex(inIndexB)) {

		 							// Allocate buffer to store copy of A
		StPointerBlock	itemBuffer((SInt32) GrabItemSize(inIndexA));
		InternalSwapItems(inIndexA, inIndexB, itemBuffer);
		mIsSorted = false;			// Swap invalidates sort
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveItem														  [public]
// ---------------------------------------------------------------------------
//	Move an item from one position to another in an Array. The net result
//	is the same as removing the item and inserting at a new position.
//
//	Does nothing if either index is out of range or if Array is kept sorted
//	(since moving could invalidate the sort).
//
//	Example:
//				BEFORE						AFTER MoveItem(2, 5)
//		index	1  2  3  4  5  6			1  2  3  4  5  6
//		item	a  b  c  d  e  f			a  c  d  e  b  f

void
LArray::MoveItem(
	ArrayIndexT		inFromIndex,
	ArrayIndexT		inToIndex)
{
									// Do nothing if Array is kept sorted or
									//   if either index is out of range or if
									//   "from" and "to" indexes are the same
	if ( !mKeepSorted &&
		 ValidIndex(inFromIndex) && ValidIndex(inToIndex) &&
		 (inFromIndex != inToIndex) ) {

									// Allocate space to store copy of item
		StPointerBlock	buffer((SInt32) GrabItemSize(inFromIndex));

		InternalMoveItem(inFromIndex, inToIndex, buffer);
		mIsSorted = false;			// Move invalidates sort
	}
}


// ---------------------------------------------------------------------------
//	¥ Remove														  [public]
// ---------------------------------------------------------------------------
//	Remove an item from an Array

void
LArray::Remove(
	const void*		inItem,
	UInt32			inItemSize)
{
	ArrayIndexT	index = FetchIndexOf(inItem, inItemSize);
	if (index != index_Bad) {
		RemoveItemsAt(1, index);
	}
}

#pragma mark --- Low-level Access ---

// ---------------------------------------------------------------------------
//	¥ Lock															  [public]
// ---------------------------------------------------------------------------
//	Lock the Handle that stores the data for the items in the Array
//
//	Class maintains a lock count, so each call to Lock() should be
//	balanced by a corresponding call to Unlock()

void
LArray::Lock() const
{
	mLockCount += 1;					// Lock count is mutable

	if ((mLockCount == 1) && (mItemsH != nil)) {
										// First lock,
		::HLock(mItemsH);				//   so really lock the Handle
	}
}


// ---------------------------------------------------------------------------
//	¥ Unlock														  [public]
// ---------------------------------------------------------------------------
//	Unlock the Handle that stores the data for the items in the Array
//
//	Class maintains a lock count, so each call to Lock() should be
//	balanced by a corresponding call to Unlock()

void
LArray::Unlock() const
{
	SignalIf_(mLockCount == 0);			// Too many Unlocks

	mLockCount -= 1;					// Lock count is mutable

	if ((mLockCount == 0) && (mItemsH != nil)) {
										// Last unlock
		::HUnlock(mItemsH);				//   so really unlock the Handle
	}
}


// ---------------------------------------------------------------------------
//	¥ IsLocked												 [inline] [public]
// ---------------------------------------------------------------------------
//	Return whether the Array is locked, meaning that adding and removing
//	items are not allowed

#pragma mark LArray::IsLocked

// bool
// LArray::IsLocked() const					// Defined inline


// ---------------------------------------------------------------------------
//	¥ GetItemPtr													  [public]
// ---------------------------------------------------------------------------
//	Returns a pointer to the start of an Items data within the internal
//	storage Handle.
//
//	WARNING: This routine performs no bounds checking. If index is not
//	valid, routine may crash or return a pointer to arbitrary memory.
//
//	WARNING: The return pointer references information inside a
//	relocatable block. This pointer will become invalid if the
//	Handle block moves. Call Lock() and then Unlock() where necessary.
//
//	WARNING: For sorted Arrays, be careful when changing the data using
//	the pointer. If your changes alter the sorting order, call
//	InvalidateSort() so that the Array's internal flags correctly reflect
//	the sorting state. Then call Sort() afterwards if you still want
//	the Array to be sorted.

void*
LArray::GetItemPtr(
	ArrayIndexT		inAtIndex) const
{
	return (*mItemsH + (inAtIndex - 1) * mItemSize);
}

#pragma mark --- Storage Management ---

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
//	This means you can remove any excess space by calling
//	AdjustAllocation(0);
//
//	inExtraData, which has a default parameter value of zero,
//	is ignored. For fixed-size items, the number of items determines
//	the amount of data stored.

void
LArray::AdjustAllocation(
	UInt32		inExtraItems,
	UInt32		/* inExtraData */)
{
	InternalAdjustAllocation(0, (mItemCount + inExtraItems) * mItemSize);
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
//
//	Fixed-size item Array only stores data, so inItemAllocation is ignored.

void
LArray::InternalAdjustAllocation(
	UInt32		/* inItemAllocation */,
	UInt32		inDataAllocation)
{
	if (inDataAllocation != mDataAllocated) {

		if (mItemsH == nil) {			// First allocation
			mItemsH = ::NewHandle((SInt32) inDataAllocation);

		} else {						// Resize existing data Handle
			::SetHandleSize(mItemsH, (SInt32) inDataAllocation);
		}

		ThrowIfMemError_();
		mDataAllocated = (SInt32) inDataAllocation;
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustStorage												   [protected]
// ---------------------------------------------------------------------------
//	Called internally when the number of bytes used by Items in the Array
//	changes.
//
//	inDeltaItems is the change in the number of items in the Array
//	inDeltaData is ignored, since the number of items determines the
//		amount of data stored for fixed-size item Arrays
//
//	If the current allocation is too small, this implementation sets the
//	internal allocation size to
//		current_alloc + max(current_alloc, delta_bytes)
//
//	For small adjustments (adding less bytes than what's already allocated),
//	this doubles the allocation.
//
//	For large adjustments (adding more bytes than what's already allocated),
//	this increases the allocation by the number of bytes added.

void
LArray::AdjustStorage(
	SInt32		inDeltaItems,
	SInt32		/* inDeltaData */)
{
	UInt32		newItemCount = mItemCount + inDeltaItems;
	UInt32		newDataStored = newItemCount * mItemSize;

	if (newDataStored > mDataAllocated) {	// Adding items

									// Allocate space for new items
									//   plus extra space to reduce
									//   memory management overhead
									//   on subsequent additions
		UInt32	deltaAlloc = inDeltaItems * mItemSize;
		if (deltaAlloc < mDataAllocated) {
			deltaAlloc = (UInt32) mDataAllocated;
		}

		try {
			InternalAdjustAllocation(0, mDataAllocated + deltaAlloc);
		}

		catch (...) {				// Probably not enough memory
									//   Try allocating just the amount
									//   of space that we need
			InternalAdjustAllocation(0, newDataStored);
		}

	} else if (inDeltaItems < 0) {			// Removing items
		SInt32	currentExtra = mDataAllocated - mDataStored;
		InternalAdjustAllocation(0, newDataStored + currentExtra);
	}

	mItemCount = newItemCount;
	mDataStored = (SInt32) newDataStored;
}


#pragma mark --- Sorting ---

// ---------------------------------------------------------------------------
//	¥ Sort															  [public]
// ---------------------------------------------------------------------------
//	Sort items in the Array

void
LArray::Sort()
{
	if (!mIsSorted && (mItemCount > 1)) {

			// Shellsort Algorithm

		SInt32	hh = 1;						// Find starting "h" value
		SInt32	stopH = (SInt32) mItemCount / 9;
		while (hh < stopH) {
			hh = 3 * hh + 1;
		}

		StPointerBlock	buffer((SInt32) mItemSize);
		StHandleLocker	lockItems(mItemsH);

		for ( ; hh > 0; hh /= 3) {

			for (SInt32 step = hh + 1; step <= mItemCount; step++) {
				PeekItem(step, buffer);

				SInt32	i;
				for (i = step - hh; i > 0; i -= hh) {
					if (mComparator->Compare(buffer, GetItemPtr(i),
												mItemSize, mItemSize) >= 0) {
						break;
					}
					PokeItem(i + hh, GetItemPtr(i), mItemSize);
				}

				PokeItem(i + hh, buffer, mItemSize);
			}
		}
	}

	mIsSorted = true;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ SetKeepSorted													  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Specify whether to keep an Array sorted when items change
//
//	This forces a Sort if inKeepSorted is true

void
LArray::SetKeepSorted(
	Boolean	inKeepSorted)
{
	mKeepSorted = inKeepSorted;

	if (inKeepSorted) {
		Sort();
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ SetComparator													  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Specify the Comparator for items in an Array

void
LArray::SetComparator(
	LComparator*	inComparator,
	Boolean			inTakeOwnership)
{
	if (inComparator == nil) {
		SignalStringLiteral_("nil Comparator not allowed");
		return;
	}

	if (mOwnsComparator) {
		delete mComparator;
	}

	mComparator = inComparator;
	mOwnsComparator = inTakeOwnership;

	if (mItemCount > 1) {		// With more than one item, Array is no
		mIsSorted = false;		//   longer sorted when Comparator changes
	}

	if (mKeepSorted) {
		Sort();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetComparator											 [inline] [public]
// ---------------------------------------------------------------------------

#pragma mark LArray::GetComparator

// LComparator*
// LArray::GetComparator() const


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ FetchIndexOfKey												  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Return the index of the item with the specified Key value

ArrayIndexT
LArray::FetchIndexOfKey(
	const void*		inKey) const
{
	ArrayIndexT	index = index_Bad;

	if (mItemCount > 0) {
		if (mIsSorted) {
			index = BinarySearchByKey(inKey);
		} else {
			index = LinearSearchByKey(inKey);
		}
	}

	return index;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ FetchInsertIndexOf											  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Return the index at which the specified item would be inserted
//
//	Returns index_Last if the Array is not sorted or if item is nil

ArrayIndexT
LArray::FetchInsertIndexOf(
	const void*		inItem,
	UInt32			inItemSize) const
{
	ArrayIndexT		current = index_Last;

	if (mIsSorted && (inItem != nil)) {
		if (inItemSize == 0) {
			inItemSize = mItemSize;
		}

		ArrayIndexT		left = 1;
		ArrayIndexT		right = (ArrayIndexT) mItemCount;
		SInt32			comparison = 0;

		current = 1;

		StArrayLocker	lock(*this);
		while (left <= right) {
			current = (left + right) / 2;

			comparison = mComparator->Compare(GetItemPtr(current), inItem,
										GrabItemSize(current), inItemSize);

			if (comparison == 0) {
				break;
			} else if (comparison > 0) {
				right = current - 1;
			} else {
				left = current + 1;
			}
		}

		if (comparison < 0) {
			current += 1;
		}
	}

	return current;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ FetchInsertIndexOfKey											  [public]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Return the index at which an item with the specified Key would be
//	inserted
//
//	Returns index_Last if the Array is not sorted

ArrayIndexT
LArray::FetchInsertIndexOfKey(
	const void*		inKey) const
{
	ArrayIndexT		current = index_Last;

	if (mIsSorted) {
		ArrayIndexT		left = 1;
		ArrayIndexT		right = (ArrayIndexT) mItemCount;
		SInt32			comparison = 0;

		current = 1;

		StArrayLocker	lock(*this);
		while (left <= right) {
			current = (left + right) / 2;

			comparison = mComparator->CompareToKey(GetItemPtr(current),
											  GrabItemSize(current), inKey);

			if (comparison == 0) {
				break;
			} else if (comparison > 0) {
				right = current - 1;
			} else {
				left = current + 1;
			}
		}

		if (comparison < 0) {
			current += 1;
		}
	}

	return current;
}

#pragma mark --- Iterator Support ---

// ---------------------------------------------------------------------------
//	¥ AttachIterator											   [protected]
// ---------------------------------------------------------------------------
//	Associate an Iterator with an Array

void
LArray::AttachIterator(
	LArrayIterator*		inIterator) const
{
	inIterator->SetNextIterator(mIteratorListHead);
	mIteratorListHead = inIterator;
}


// ---------------------------------------------------------------------------
//	¥ DetachIterator											   [protected]
// ---------------------------------------------------------------------------
//	Dissociate an Iterator from an Array

void
LArray::DetachIterator(
	LArrayIterator*		inIterator) const
{
	LArrayIterator	*currIter = mIteratorListHead;
	LArrayIterator	*prevIter = nil;

	while (currIter != nil) {		// Search for the specified Iterator
		if (currIter == inIterator) {
									// Found it. Adjust linked list.
			if (prevIter == nil) {
				mIteratorListHead = currIter->GetNextIterator();
			} else {
				prevIter->SetNextIterator(currIter->GetNextIterator());
			}
			break;

		} else {					// Continue search
			prevIter = currIter;
			currIter = currIter->GetNextIterator();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ItemsInserted												   [protected]
// ---------------------------------------------------------------------------
//	Notify Iterators associated with an array that items have been inserted

void
LArray::ItemsInserted(
	UInt32			inCount,
	ArrayIndexT		inAtIndex)
{
	LArrayIterator	*theIter = mIteratorListHead;
	while (theIter != nil) {
		theIter->ItemsInserted(inCount, inAtIndex);
		theIter = theIter->GetNextIterator();
	}
}


// ---------------------------------------------------------------------------
//	¥ ItemsRemoved												   [protected]
// ---------------------------------------------------------------------------
//	Notify Iterators associated with an Array that items have been removed

void
LArray::ItemsRemoved(
	UInt32			inCount,
	ArrayIndexT		inAtIndex)
{
	LArrayIterator	*theIter = mIteratorListHead;
	while (theIter != nil) {
		theIter->ItemsRemoved(inCount, inAtIndex);
		theIter = theIter->GetNextIterator();
	}
}

#pragma mark --- Data Storage ---

// ---------------------------------------------------------------------------
//	¥ PeekItem													   [protected]
// ---------------------------------------------------------------------------
//	Pass back the data for the Item at the specified index
//
//	Used internally to read Item data

void
LArray::PeekItem(
	ArrayIndexT		inAtIndex,
	void*			outItem) const
{
	UInt32	itemSize = GrabItemSize(inAtIndex);

	if (itemSize == sizeof(long)) {
		*(long*) outItem = *(long*) GetItemPtr(inAtIndex);

	} else {
		::BlockMoveData(GetItemPtr(inAtIndex), outItem, (SInt32) itemSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ PokeItem													   [protected]
// ---------------------------------------------------------------------------
//	Store data for the Item at the specified index
//
//	Used internally to write Item data

void
LArray::PokeItem(
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	if (inItemSize == sizeof(long)) {
		*(long*) GetItemPtr(inAtIndex) = *(long*) inItem;

	} else {
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
LArray::ShiftItems(
	ArrayIndexT		inStartIndex,
	ArrayIndexT		inEndIndex,
	SInt32			inIndexShift,
	SInt32			/* inDataShift */)
{
	::BlockMoveData( GetItemPtr(inStartIndex),
					 GetItemPtr(inStartIndex + inIndexShift),
					 (SInt32) GrabItemRangeSize(inStartIndex, inEndIndex) );
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
LArray::StoreNewItems(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	if (inItem != nil) {
		ArrayIndexT	lastIndex = inAtIndex + (ArrayIndexT) inCount - 1;
		for (ArrayIndexT i = inAtIndex; i <= lastIndex; i++) {
			PokeItem(i, inItem, inItemSize);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ InternalSwapItems											   [protected]
// ---------------------------------------------------------------------------
//	Swap the values of the Items at the specified indexes
//
//	Internal routine. Performs no range checking

void
LArray::InternalSwapItems(
	ArrayIndexT		inIndexA,
	ArrayIndexT		inIndexB,
	void*			inBuffer)		// Large enough to hold data for A
{
	PeekItem(inIndexA, inBuffer);				// Store Copy of A
	UInt32	sizeOfA = GrabItemSize(inIndexA);

	InternalCopyItem(inIndexB, inIndexA);		// A = B

	PokeItem(inIndexB, inBuffer, sizeOfA);		// B = Copy of A
}


// ---------------------------------------------------------------------------
//	¥ InternalMoveItem											   [protected]
// ---------------------------------------------------------------------------
//	Move an item from one position to another in an Array. The net result
//	is the same as removing the item and inserting at a new position.
//
//	Internal routine. Performs no range checking

void
LArray::InternalMoveItem(
	ArrayIndexT		inFromIndex,
	ArrayIndexT		inToIndex,
	void*			inBuffer)	// To hold copy of Item at inFromIndex
{
								// Store copy of Item to move
	UInt32	itemSize = GrabItemSize(inFromIndex);
	PeekItem(inFromIndex, inBuffer);

	if (inFromIndex < inToIndex) {
								// Move item to a higher index
								// Shift items between "from" and "to"
								//   down one spot
		ShiftItems(inFromIndex + 1, inToIndex, -1, (SInt32) -itemSize);
	} else {
								// Move item to a lower index
								// Shift items between "to" and "from"
								//   up one spot
		ShiftItems(inToIndex, inFromIndex - 1, 1, (SInt32) itemSize);
	}

								// Store item at new position
	PokeItem(inToIndex, inBuffer, itemSize);
}


// ---------------------------------------------------------------------------
//	¥ InternalCopyItem											   [protected]
// ---------------------------------------------------------------------------
//	Set value of destination item to that of the source item
//
//	Internal routine. Performs no range checking

void
LArray::InternalCopyItem(
	ArrayIndexT		inSourceIndex,
	ArrayIndexT		inDestIndex)
{
	PokeItem(inDestIndex, GetItemPtr(inSourceIndex), mItemSize);
}

#pragma mark --- Searching ---

// ---------------------------------------------------------------------------
//	¥ LinearSearch												   [protected]
// ---------------------------------------------------------------------------
//	Return the index of the specified item, searching linearly from the
//	start of the Array

ArrayIndexT
LArray::LinearSearch(
	const void*		inItem,
	UInt32			inItemSize) const
{
	ArrayIndexT	findIndex = 0;		// Search from beginning of Array

	StArrayLocker	lock(*this);

	char	*itemPtr = *mItemsH;
	while (++findIndex <= mItemCount) {
		UInt32	itemSize = GrabItemSize(findIndex);
		if (mComparator->IsEqualTo(inItem, itemPtr, inItemSize, itemSize)) {
			break;
		}
		itemPtr += itemSize;
	}

	if (findIndex > mItemCount) {	// Search stopped because we reached the
		findIndex = index_Bad;		//   end without finding the item
	}

	return findIndex;
}


// ---------------------------------------------------------------------------
//	¥ LinearSearchByKey											   [protected]
// ---------------------------------------------------------------------------
//	Return the index of the item with the specified key, searching linearly
//	from the start of the Array

ArrayIndexT
LArray::LinearSearchByKey(
	const void*		inKey) const
{
	ArrayIndexT	findIndex = 0;		// Search from beginning of Array

	StArrayLocker	lock(*this);

	char	*itemPtr = *mItemsH;
	while (++findIndex <= mItemCount) {
		UInt32	itemSize = GrabItemSize(findIndex);
		if (mComparator->IsEqualToKey(itemPtr, itemSize, inKey)) {
			break;
		}
		itemPtr += itemSize;
	}

	if (findIndex > mItemCount) {	// Search stopped because we reached the
		findIndex = index_Bad;		//   end without finding the item
	}

	return findIndex;
}


// ---------------------------------------------------------------------------
//	¥ BinarySearch												   [protected]
// ---------------------------------------------------------------------------
//	Return the index of the specified item using a binary search
//
//	Assumes that the Array is sorted

ArrayIndexT
LArray::BinarySearch(
	const void*		inItem,
	UInt32			inItemSize) const
{
	ArrayIndexT		left		= 1;
	ArrayIndexT		right		= (ArrayIndexT) mItemCount;
	ArrayIndexT		foundIndex	= index_Bad;

	StArrayLocker	lock(*this);

	while (left <= right) {
		ArrayIndexT	current = (left + right) / 2;

		SInt32	comparison = mComparator->Compare(GetItemPtr(current), inItem,
										  GrabItemSize(current), inItemSize);

		if (comparison == 0) {
			foundIndex = current;
			break;
		} else if (comparison > 0) {
			right = current - 1;
		} else {
			left = current + 1;
		}
	}

	return foundIndex;
}


// ---------------------------------------------------------------------------
//	¥ BinarySearchByKey											   [protected]
// ---------------------------------------------------------------------------
//	Return the index of the item with the specified key using a binary search
//
//	Assumes that the Array is sorted

ArrayIndexT
LArray::BinarySearchByKey(
	const void*		inKey) const
{
	ArrayIndexT		left		= 1;
	ArrayIndexT		right		= (ArrayIndexT) mItemCount;
	ArrayIndexT		foundIndex	= index_Bad;

	StArrayLocker	lock(*this);

	while (left <= right) {
		ArrayIndexT	current = (left + right) / 2;

		SInt32	comparison = mComparator->CompareToKey(GetItemPtr(current),
										 GrabItemSize(current), inKey);

		if (comparison == 0) {
			foundIndex = current;
			break;
		} else if (comparison > 0) {
			right = current - 1;
		} else {
			left = current + 1;
		}
	}

	return foundIndex;
}


// ---------------------------------------------------------------------------
//	¥ GrabItemRangeSize									  [inline] [protected]
// ---------------------------------------------------------------------------
//	Return the size in bytes of a specified range of items

#pragma mark LArray::GrabItemRangeSize

// UInt32
// LArray::GrabItemRangeSize(
//		ArrayIndexT		inStartIndex,
//		ArrayIndexT		inEndIndex) const


PP_End_Namespace_PowerPlant
