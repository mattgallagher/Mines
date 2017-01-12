// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRunArray.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of fixed-size items, where consecutive items
//	with identical data are stored as a single entry. Positions are
//	one-based--the first item is at index 1.
//
//	Class maintains two Handles: one stores the data for the items and
//	the other stores Run records identifying consecutive items that
//	have the same data.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LRunArray.h>
#include <LComparator.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant

#pragma mark --- Construction & Destruction ---

// ---------------------------------------------------------------------------
//	¥ LRunArray								Constructor				  [public]
// ---------------------------------------------------------------------------

LRunArray::LRunArray(
	UInt32			inItemSize,
	LComparator*	inComparator,
	Boolean			inKeepSorted)

	: LArray(inItemSize, inComparator, inKeepSorted)
{
	mRunsH		= nil;
	mRunCount	= 0;
}


// ---------------------------------------------------------------------------
//	¥ LRunArray								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from existing Data and Run Handles

LRunArray::LRunArray(
	UInt32			inItemSize,
	Handle			inItemsHandle,
	Handle			inRunsHandle,
	LComparator*	inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)

	: LArray(inItemSize, inItemsHandle, inComparator, inIsSorted,
					inKeepSorted)
{
	mRunsH		= nil;
	mRunCount	= 0;

	if (inRunsHandle != nil) {
		mRunsH		= inRunsHandle;
		mRunCount	= ::GetHandleSize(inRunsHandle) / (SInt32) sizeof(SRunRecord);
	}
}


// ---------------------------------------------------------------------------
//	¥ LRunArray								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LRunArray::LRunArray(
	const LRunArray&	inOriginal)

	: LArray(inOriginal)
{
	CopyRunArray(inOriginal, false);
}


// ---------------------------------------------------------------------------
//	¥ LRunArray								Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LRunArray&
LRunArray::operator=(
	const LRunArray&	inRunArray)
{
	if (IsLocked()) {
		SignalStringLiteral_("Can't assign to a locked RunArray");

	} else if (this != &inRunArray) {	// Check for self assignment

		LArray::operator=(inRunArray);	// Inherited assigment operator

		CopyRunArray(inRunArray, true);	// Copy runs from rhs

	}

	return *this;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ CopyRunArray													 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Copy data members of a RunArray

void
LRunArray::CopyRunArray(
	const LRunArray&	inRunArray,
	bool				inDestroyOldData)
{
	Handle	copyRunsH	 = nil;
	SInt32	copyRunCount = inRunArray.mRunCount;

	if (copyRunCount > 0) {

			// Use HandToHand() to duplicate Handle of runs. This
			// function works in an odd way. You first set the
			// Handle of the intended copy to the original, then
			// the function modifies the copy's Handle value.

		copyRunsH   = inRunArray.mRunsH;
		OSErr	err =  ::HandToHand(&copyRunsH);
		ThrowIfOSErr_(err);				// Most likely out of memory
	}

	if (inDestroyOldData) {				// Duplication succeeded. It's now
		DestroyRunArray();				//   safe to destroy our old data.
	}

	mRunsH		= copyRunsH;
	mRunCount	= copyRunCount;
}


// ---------------------------------------------------------------------------
//	¥ ~LRunArray							Destructor				  [public]
// ---------------------------------------------------------------------------

LRunArray::~LRunArray()
{
	DestroyRunArray();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DestroyRunArray												 [private]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

void
LRunArray::DestroyRunArray()
{
	if (mRunsH != nil) {
		::DisposeHandle(mRunsH);
		mRunsH = nil;
	}
}

#pragma mark --- Public Manipulators ---


// ---------------------------------------------------------------------------
//	¥ AddItem														  [public]
// ---------------------------------------------------------------------------
//	Add one item to Array

ArrayIndexT
LRunArray::AddItem(
	const void*		inItem,
	UInt32			inItemSize)
{
	return InsertItemsAt(1, index_Last, inItem, inItemSize);
}


// ---------------------------------------------------------------------------
//	¥ InsertItemsAt													  [public]
// ---------------------------------------------------------------------------
//	Insert new items into a RunArray at the specified index

ArrayIndexT
LRunArray::InsertItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	if (inCount < 1) {				// Exit if nothing to insert
		return index_Bad;
	}

	if (mLockCount > 0) {
		SignalStringLiteral_("Can't insert into a locked RunArray");
		return index_Bad;
	}

									// Adjust Insertion Index if necessary

	if (mKeepSorted) {				// Find sorted position
		inAtIndex = FetchInsertIndexOf(inItem, inItemSize);
	}

	if (inAtIndex > mItemCount) {		// Enforce upper and lower bounds
										//   for insertion point
		inAtIndex = (ArrayIndexT) mItemCount + 1;
	} else if (inAtIndex < 1) {
		inAtIndex = 1;
	}

	if (mItemCount == 0) {				// Array is currently empty
		InsertRun(0, inCount, inItem);

	} else if (inAtIndex > mItemCount) {
										// Inserting at end of Array
		if (BlocksAreEqual(inItem, PtrToRunItem(mRunCount-1), mItemSize)) {
										// New items are the same as the
										//   last item
			ExtendRun(mRunCount - 1, inCount);

		} else {
			InsertRun(mRunCount, inCount, inItem);
		}

	} else if (inAtIndex == 1) {		// Inserting at beginning of Array
		if (BlocksAreEqual(inItem, PtrToRunItem(0), mItemSize)) {
										// New items are the same as the
										//   first item
			ExtendRun(0, inCount);

		} else {
			InsertRun(0, inCount, inItem);
		}

	} else {							// Inserting into middle of Array

										// Find Run containing the
										//   insertion point
		ArrayIndexT	withinRun = GetRunIndex(inAtIndex);

		if (BlocksAreEqual(inItem, PtrToRunItem(withinRun), mItemSize)) {
										// New items are the same as
										//   those in this Run
			ExtendRun(withinRun, inCount);

		} else if (inAtIndex == RunStart(withinRun)) {
										// Insertion point is at the start
										//   of a Run
			if (BlocksAreEqual(inItem, PtrToRunItem(withinRun - 1), mItemSize)) {
										// New items are the same as those
										//   in the previous Run
				ExtendRun(withinRun - 1, inCount);

			} else {
				InsertRun(withinRun, inCount, inItem);
			}

		} else {						// Inserting into the middle of
										//   a Run
			SplitRun(withinRun, inAtIndex, inCount, inItem);
		}
	}

	mIsSorted   = mKeepSorted;
	mItemCount += inCount;

	return inAtIndex;
}


// ---------------------------------------------------------------------------
//	¥ RemoveItemsAt													  [public]
// ---------------------------------------------------------------------------
//	Remove items from the Array starting at the specified index

void
LRunArray::RemoveItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex)
{
	if (mLockCount > 0) {
		SignalStringLiteral_("Can't remove from a locked RunArray");
		return;
	}

	if (ValidIndex(inAtIndex) && (inCount > 0)) {

		if ((inAtIndex + inCount) > mItemCount) {
										// Deleting to the end of Array
			if (inAtIndex == 1) {
										// Deleting entire Array
				::SetHandleSize(mItemsH, 0);
				::SetHandleSize(mRunsH, 0);
				mRunCount = 0;

			} else {
										// Last Run will be the one containing
										//   the item before the one being
										//   removed
				ArrayIndexT	lastRun = GetRunIndex(inAtIndex - 1);

										// Reduce sizes of item and Run
										//   Handles
				::SetHandleSize(mItemsH, (lastRun + 1) * (SInt32) mItemSize);
				::SetHandleSize(mRunsH, (lastRun + 1) * (SInt32) sizeof(SRunRecord));

				((SRunRecord*)(*mRunsH))[lastRun].end = inAtIndex - 1;
				mRunCount = lastRun + 1;
			}

			mItemCount = (UInt32) inAtIndex - 1;

		} else if (inAtIndex == 1) {	// Deleting from the start of Array

										// First Run will be the one containing
										//   the item after the deleted ones
			ArrayIndexT	firstRun = GetRunIndex((ArrayIndexT) inCount + 1);

			if (firstRun > 0) {
										// Remove Runs from start up to, but
										//   not including, firstRun

										// Shift down data for remaining items
				::BlockMoveData(PtrToRunItem(firstRun), *mItemsH,
								(mRunCount - firstRun) * (SInt32) mItemSize);

										// Shift down remaining Run records
				::BlockMoveData(*mRunsH + firstRun * sizeof(SRunRecord),
								*mRunsH,
								(mRunCount - firstRun) * (SInt32) sizeof(SRunRecord));

										// Reduce sizes of data and runs
										//   Handles to new number of runs
				mRunCount -= firstRun;
				::SetHandleSize(mItemsH, mRunCount * (SInt32) mItemSize);
				::SetHandleSize(mRunsH, mRunCount * (SInt32) sizeof(SRunRecord));
			}

										// Adjust indexes for first Run
			SRunRecord	*run = (SRunRecord*) *mRunsH;
			run[0].start = 1;
			run[0].end -= (ArrayIndexT) inCount;

										// Adjust indexes for the second and
										//   succeeding Runs
			for (ArrayIndexT i = 1; i < mRunCount; i++) {
				run[i].start -= (ArrayIndexT) inCount;
				run[i].end -= (ArrayIndexT) inCount;
			}

			mItemCount -= inCount;

		} else {						// Deleting from the middle

										// The Runs between, but not
										//    including, oneEnd and twoStart
										//    will be deleted
			ArrayIndexT oneEnd = GetRunIndex(inAtIndex - 1);
			ArrayIndexT	twoStart = GetRunIndex(inAtIndex + (ArrayIndexT) inCount);

			if (oneEnd == twoStart) {	// Items to delete are all within
										//    a single Run
				((SRunRecord*)(*mRunsH))[oneEnd].end -= (ArrayIndexT) inCount;

			} else {
				((SRunRecord*)(*mRunsH))[oneEnd].end = inAtIndex - 1;
				((SRunRecord*)(*mRunsH))[twoStart].start = inAtIndex;
				((SRunRecord*)(*mRunsH))[twoStart].end -= (ArrayIndexT) inCount;

				if (twoStart > (oneEnd + 1)) {
										// Items to delete span at least
										//   one entire Run, so one or
										//   more Runs must be removed

										// Shift down item data
					::BlockMoveData(PtrToRunItem(twoStart),
									PtrToRunItem(oneEnd + 1),
									(mRunCount - twoStart) * (SInt32) mItemSize);

										// Shift down Run records
					::BlockMoveData(*mRunsH + twoStart * sizeof(SRunRecord),
									*mRunsH + (oneEnd + 1) * sizeof(SRunRecord),
									(mRunCount - twoStart) * (SInt32) sizeof(SRunRecord));

										// Adjust sizes of item and Run
										//    Handles to new number of Runs
					mRunCount -= (twoStart - oneEnd - 1);
					::SetHandleSize(mItemsH, mRunCount * (SInt32) mItemSize);
					::SetHandleSize(mRunsH, mRunCount * (SInt32) sizeof(SRunRecord));

										// Removing Runs have moved twoStart
										//   next to oneEnd
					twoStart = oneEnd + 1;
				}
			}

										// Adjust indexes for Runs after
										//   twoStart
			SRunRecord	*run = (SRunRecord*) *mRunsH;
			for (ArrayIndexT i = twoStart + 1; i < mRunCount; i++) {
				run[i].start -= (ArrayIndexT) inCount;
				run[i].end -= (ArrayIndexT) inCount;
			}

			mItemCount -= inCount;
		}
										// Set internal size counts
		mDataStored = mDataAllocated = mRunCount * (SInt32) mItemSize;
	}
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
LRunArray::AssignItemsAt(
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
LRunArray::GetItemPtr(
	ArrayIndexT		inItemIndex) const
{
	return (*mItemsH + mItemSize * GetRunIndex(inItemIndex));
}

#pragma mark --- Protected Manipulators ---

// ---------------------------------------------------------------------------
//	¥ PokeItem													   [protected]
// ---------------------------------------------------------------------------
//	Store data for the Item at the specified index
//
//	Used internally to write Item data

void
LRunArray::PokeItem(
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	RemoveItemsAt(1, inAtIndex);
	InsertItemsAt(1, inAtIndex, inItem, inItemSize);
}


// ---------------------------------------------------------------------------
//	¥ InternalMoveItem											   [protected]
// ---------------------------------------------------------------------------
//	Move an item from one position to another in an Array. The net result
//	is the same as removing the item and inserting at a new position.
//
//	Internal routine. Performs no range checking

void
LRunArray::InternalMoveItem(
	ArrayIndexT		inFromIndex,
	ArrayIndexT		inToIndex,
	void*			inBuffer)	// To hold copy of Item at inFromIndex
{
								// Store copy of Item to move
	UInt32	itemSize = GrabItemSize(inFromIndex);
	PeekItem(inFromIndex, inBuffer);

	RemoveItemsAt(1, inFromIndex);
	InsertItemsAt(1, inToIndex, inBuffer, itemSize);
}

#pragma mark --- Run Record Maintenance ---

// ---------------------------------------------------------------------------
//	¥ GetRunIndex												   [protected]
// ---------------------------------------------------------------------------
//	Return the index of the Run containing the specified item
//
//	Run index is zero-based

ArrayIndexT
LRunArray::GetRunIndex(
	ArrayIndexT		inItemIndex) const
{
	ArrayIndexT		runIndex = index_Bad;

	SRunRecord	*run = (SRunRecord*) *mRunsH;
	for (ArrayIndexT i = 0; i < mRunCount; i++) {		// Linear search
		if (inItemIndex <= run[i].end) {
			runIndex = i;
			break;
		}
	}

	return runIndex;
}


// ---------------------------------------------------------------------------
//	¥ PtrToRunItem										  [inline] [protected]
// ---------------------------------------------------------------------------
// Return a pointer to the specified Run item

#pragma mark LRunArray::PtrToRunItem

// void*
// LRunArray::PtrToRunItem(
//		ArrayIndexT		inRunIndex) const


// ---------------------------------------------------------------------------
//	¥ RunStart											  [inline] [protected]
// ---------------------------------------------------------------------------
//	Return the index of the start of a Run

#pragma mark LRunArray::RunStart

// ArrayIndexT
// LRunArray::RunStart(
//		ArrayIndexT		inRunIndex) const


// ---------------------------------------------------------------------------
//	¥ InsertRun													   [protected]
// ---------------------------------------------------------------------------
//	Insert a new Run with the specified location, length, and value

void
LRunArray::InsertRun(
	ArrayIndexT		inRunIndex,
	UInt32			inCount,
	const void*		inItem)
{
										// Add room for another data item
	if (mItemsH == nil) {
		mItemsH = ::NewHandle((mRunCount + 1) * (SInt32) mItemSize);

	} else {
		::SetHandleSize(mItemsH,(mRunCount + 1) * (SInt32) mItemSize);
	}

	ThrowIfMemError_();

										// Add room for another RunRecord
	if (mRunsH == nil) {
		mRunsH = ::NewHandle((mRunCount + 1) * (SInt32) sizeof(SRunRecord));

	} else {
		::SetHandleSize(mRunsH, (mRunCount + 1) * (SInt32) sizeof(SRunRecord));
	}

	OSErr	err = MemError();
	if (err != noErr) {					// Couldn't get space for another RunRecord
										// Reset data back to original size
		::SetHandleSize(mItemsH, mRunCount * (SInt32) mItemSize);
		Throw_(err);
	}

										// Adjust Run indexes
	SRunRecord	*run = (SRunRecord*) *mRunsH;

	if (mRunCount == 0) {				// Array was empty
		run[0].start = 1;
		run[0].end = (ArrayIndexT) inCount;

	} else if (inRunIndex == mRunCount) {
										// Insert at end of Array
		run[mRunCount].start = run[mRunCount - 1].end + 1;
		run[mRunCount].end = run[mRunCount].start + (ArrayIndexT) inCount - 1;

	} else if (inRunIndex < mRunCount) {
										// Shift up item data
		::BlockMoveData(PtrToRunItem(inRunIndex), PtrToRunItem(inRunIndex + 1),
						(mRunCount - inRunIndex) * (SInt32) mItemSize);

										// Shift up run data
		::BlockMoveData(*mRunsH + inRunIndex * sizeof(SRunRecord),
						*mRunsH + (inRunIndex + 1) * sizeof(SRunRecord),
						(mRunCount - inRunIndex) * (SInt32) sizeof(SRunRecord));

		run[inRunIndex].end = run[inRunIndex].start + (ArrayIndexT) inCount - 1;
	}

										// Shift up indexes of Runs past
										//   insertion point
	mRunCount += 1;
	for (ArrayIndexT i = inRunIndex + 1; i < mRunCount; i++) {
		run[i].start += (ArrayIndexT) inCount;
		run[i].end += (ArrayIndexT) inCount;
	}

										// Store data for new items
	::BlockMoveData(inItem, PtrToRunItem(inRunIndex), (SInt32) mItemSize);

										// Set internal size counts
	mDataStored = mDataAllocated = mRunCount * (SInt32) mItemSize;
}


// ---------------------------------------------------------------------------
//	¥ SplitRun													   [protected]
// ---------------------------------------------------------------------------
//	Insert a new Run into the middle of an existing Run, thereby splitting
//	the existing Run into two pieces.

void
LRunArray::SplitRun(
	ArrayIndexT		inRunIndex,
	ArrayIndexT		inItemIndex,
	UInt32			inCount,
	const void*		inItem)
{
										// Add room for two data items
	::SetHandleSize(mItemsH, (mRunCount + 2) * (SInt32) mItemSize);
	ThrowIfMemError_();

										// Add room for two RunRecords
	::SetHandleSize(mRunsH, (mRunCount + 2) * (SInt32) sizeof(SRunRecord));

	OSErr	err = MemError();
	if (err != noErr) {					// Couldn't get space for another RunRecord
										// Reset data back to original size
		::SetHandleSize(mItemsH, mRunCount * (SInt32) mItemSize);
		Throw_(err);
	}

	SRunRecord	*run = (SRunRecord*) *mRunsH;

	if (inRunIndex < mRunCount) {
										// Shift up item data
		::BlockMoveData(PtrToRunItem(inRunIndex + 1), PtrToRunItem(inRunIndex + 3),
						(mRunCount - inRunIndex - 1) * (SInt32) mItemSize);

										// Shift up RunRecords
		::BlockMoveData(*mRunsH + (inRunIndex + 1) * sizeof(SRunRecord),
						*mRunsH + (inRunIndex + 3) * sizeof(SRunRecord),
						(mRunCount - inRunIndex - 1) * (SInt32) sizeof(SRunRecord));

	}
										// Store data for new items
	::BlockMoveData(inItem, PtrToRunItem(inRunIndex + 1), (SInt32) mItemSize);
										// Copy data for second half of split run
	::BlockMoveData(PtrToRunItem(inRunIndex), PtrToRunItem(inRunIndex + 2), (SInt32) mItemSize);

	ArrayIndexT	secondSplitRange = run[inRunIndex].end - inItemIndex;

	run[inRunIndex].end = inItemIndex - 1;
	run[inRunIndex+1].start = inItemIndex;
	run[inRunIndex+1].end = inItemIndex + (ArrayIndexT) inCount - 1;
	run[inRunIndex+2].start = run[inRunIndex+1].end + 1;
	run[inRunIndex+2].end = run[inRunIndex+2].start + secondSplitRange;

										// Shift up indexes of Runs past
										//   insertion point
	mRunCount += 2;
	for (ArrayIndexT i = inRunIndex + 3; i < mRunCount; i++) {
		run[i].start += (ArrayIndexT) inCount;
		run[i].end += (ArrayIndexT) inCount;
	}
										// Set internal size counts
	mDataStored = mDataAllocated = mRunCount * (SInt32) mItemSize;
}


// ---------------------------------------------------------------------------
//	¥ ExtendRun													   [protected]
// ---------------------------------------------------------------------------
//	Extend the length of an existing Run

void
LRunArray::ExtendRun(
	ArrayIndexT		inRunIndex,
	UInt32			inCount)
{
	SRunRecord	*run = (SRunRecord*) *mRunsH;

	run[inRunIndex].end += (ArrayIndexT) inCount;

										// Shift up indexes of Runs past
										//   insertion point
	for (ArrayIndexT i = inRunIndex + 1; i < mRunCount; i++) {
		run[i].start += (ArrayIndexT) inCount;
		run[i].end += (ArrayIndexT) inCount;
	}
}


PP_End_Namespace_PowerPlant
