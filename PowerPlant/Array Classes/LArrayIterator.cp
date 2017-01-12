// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LArrayIterator.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Iterator for sequential traversal of LArray objects. A LArrayIterator is
//	associated with a single LArray object. It will "do the right thing"
//	if the Array changes during iteration. You can add items, removed items,
//	and even delete the Array itself.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LArrayIterator						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct Iterator for a Array starting at a particular position

LArrayIterator::LArrayIterator(
	const LArray&	inArray,
	ArrayIndexT		inPosition)

	: mArray(inArray)
{
	mNextIterator = nil;
	mCurrIndex	  = index_BeforeStart;
	inArray.AttachIterator(this);
	ResetTo(inPosition);
}


// ---------------------------------------------------------------------------
//	¥ ~LArrayIterator						Destructor				  [public]
// ---------------------------------------------------------------------------

LArrayIterator::~LArrayIterator()
{
	if (mCurrIndex != index_ArrayDied) {
		mArray.DetachIterator(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ResetTo														  [public]
// ---------------------------------------------------------------------------
//	Reset the current item to the specified index value
//
//	The constants from_Start and from_End are recognized.

void
LArrayIterator::ResetTo(
	ArrayIndexT	inPosition)
{
	if (mCurrIndex != index_ArrayDied) {
										// Set current item
		if (inPosition == from_End || inPosition > mArray.GetCount()) {
										// Position is past end of list
			mCurrIndex = index_AfterEnd;
		} else if (inPosition <= index_BeforeStart) {
										// Position is before start of list
			mCurrIndex = index_BeforeStart;
		} else {
			mCurrIndex = inPosition;	// 1 <= inPosition <= numItems
		}
		CalcNextIndex();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Current														  [public]
// ---------------------------------------------------------------------------
//	Pass back a copy of the current item in the Array
//
//	Returns true if the current item exists
//	Returns false if the current item does not exist, which happens when:
//		- Current item was deleted
//		- Current item is past end of Array
//		- Current item is before beginning of Array
//		- Array was deleted

Boolean
LArrayIterator::Current(
	void*	outItem)
{
	return (mCurrIndex == index_ArrayDied) ?
				false :
				mArray.FetchItemAt(mCurrIndex, outItem) != LArray::index_Bad;
}

// ---------------------------------------------------------------------------
//	¥ Current														  [public]
// ---------------------------------------------------------------------------
//	Pass back a copy of the current item in the Array and the item's size
//
//	On input, ioItemSize is the maximum number of bytes to copy
//	On output, ioItemSize is either the size of the item or the
//		input value (if the input value is less than the item size)
//
//	Return value same as above Current() function.
//	Use for Arrays with variable-size elements.

Boolean
LArrayIterator::Current(
	void*		outItem,
	UInt32&		ioItemSize)
{
	return (mCurrIndex == index_ArrayDied) ?
				false :
				(mArray.FetchItemAt(mCurrIndex, outItem, ioItemSize)
							!= LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//	¥ PtrToCurrent													  [public]
// ---------------------------------------------------------------------------
//	Return a pointer to the current item in the Array
//
//	Returns nil if there is no current item

void*
LArrayIterator::PtrToCurrent()
{
	void*			itemPtr = nil;

	if (mArray.ValidIndex(mCurrIndex)) {
		itemPtr = mArray.GetItemPtr(mCurrIndex);
	}

	return itemPtr;
}


// ---------------------------------------------------------------------------
//	¥ PtrToCurrent													  [public]
// ---------------------------------------------------------------------------
//	Return a pointer to the current item in the Array and
//	pass back the item size
//
//	Returns nil and item size of zero if there is no current item

void*
LArrayIterator::PtrToCurrent(
	UInt32&		outItemSize)
{
	outItemSize = 0;
	void*	itemPtr = nil;

	if (mArray.ValidIndex(mCurrIndex)) {
		outItemSize	= mArray.GrabItemSize(mCurrIndex);
		itemPtr		= mArray.GetItemPtr(mCurrIndex);
	}

	return itemPtr;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Next															  [public]
// ---------------------------------------------------------------------------
//	Move to the next item in the Array and pass back a copy of that item
//
//	Returns true if the next item exists
//	Returns false if next item does not exist, which happens when:
//		- Current item is at or past end of the Array
//		- Array was deleted

Boolean
LArrayIterator::Next(
	void*	outItem)
{
	mCurrIndex = mNextIndex;		// Move to next item
	CalcNextIndex();
	return Current(outItem);
}


// ---------------------------------------------------------------------------
//	¥ Next															  [public]
// ---------------------------------------------------------------------------
//	Move to the next item in the Array and pass back a copy of that item
//	and the item's size
//
//	On input, ioItemSize is the maximum number of bytes to copy
//	On output, ioItemSize is either the size of the item or the
//		input value (if the input value is less than the item size)
//
//	Return value same as above Next() function.
//	Use for Arrays with variable-size elements.


Boolean
LArrayIterator::Next(
	void*		outItem,
	UInt32&		ioItemSize)
{
	mCurrIndex = mNextIndex;		// Move to next item
	CalcNextIndex();
	return Current(outItem, ioItemSize);
}


// ---------------------------------------------------------------------------
//	¥ PtrToNext														  [public]
// ---------------------------------------------------------------------------
//	Move to the next item in the Array and return a pointer to that item
//
//	Returns nil if there is no next item

void*
LArrayIterator::PtrToNext()
{
	mCurrIndex = mNextIndex;
	CalcNextIndex();

	return PtrToCurrent();
}


// ---------------------------------------------------------------------------
//	¥ PtrToNext														  [public]
// ---------------------------------------------------------------------------
//	Move to the next item in the Array and return a pointer to that item
//	and the item's size
//
//	Returns nil and item size of zero if there is no next item

void*
LArrayIterator::PtrToNext(
	UInt32&		outItemSize)
{
	mCurrIndex = mNextIndex;
	CalcNextIndex();

	return PtrToCurrent(outItemSize);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Previous														  [public]
// ---------------------------------------------------------------------------
//	Move to the previous item in the Array and pass back a copy of that item
//
//	Returns true if the previous item exists
//	Returns false if the previos item does not exist, which happens when:
//		- Current item is at or before the start of the Array
//		- Array was deleted

Boolean
LArrayIterator::Previous(
	void*	outItem)
{
	CalcPreviousIndex();
	return Current(outItem);
}


// ---------------------------------------------------------------------------
//	¥ Previous														  [public]
// ---------------------------------------------------------------------------
//	Move to the previous item in the Array and pass back a copy of that item
//	and the item's size
//
//	On input, ioItemSize is the maximum number of bytes to copy
//	On output, ioItemSize is either the size of the item or the
//		input value (if the input value is less than the item size)
//
//	Return value same as above Previous() function.
//	Use for Arrays with variable-size elements.

Boolean
LArrayIterator::Previous(
	void*		outItem,
	UInt32&		ioItemSize)
{
	CalcPreviousIndex();
	return Current(outItem, ioItemSize);
}


// ---------------------------------------------------------------------------
//	¥ PtrToPrevious													  [public]
// ---------------------------------------------------------------------------
//	Move to the previous item in the Array and return a pointer to that item
//
//	Returns nil if there is no previous item

void*
LArrayIterator::PtrToPrevious()
{
	CalcPreviousIndex();
	return PtrToCurrent();
}


// ---------------------------------------------------------------------------
//	¥ PtrToPrevious													  [public]
// ---------------------------------------------------------------------------
//	Move to the previous item in the Array and return a pointer to that item
//	and the item's size
//
//	Returns nil and item size of zero if there is no previous item

void*
LArrayIterator::PtrToPrevious(
	UInt32&		outItemSize)
{
	CalcPreviousIndex();
	return PtrToCurrent(outItemSize);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcNextIndex												   [protected]
// ---------------------------------------------------------------------------
//	Calculate mNextIndex value based on value of mCurrIndex

void
LArrayIterator::CalcNextIndex()
{
	if (mCurrIndex == index_AfterEnd) {
		mNextIndex = index_AfterEnd;

	} else if (mCurrIndex != index_ArrayDied) {
									// Next is one after the current
		mNextIndex = mCurrIndex + 1;
		if (mNextIndex > mArray.GetCount()) {
									// We have gone past the end of the Array
			mNextIndex = index_AfterEnd;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcPreviousIndex											   [protected]
// ---------------------------------------------------------------------------
//	Adjust internal indexes to access the previous item in an Array

void
LArrayIterator::CalcPreviousIndex()
{
	if (mCurrIndex == index_Removed) {
									// Current item was removed
									// Previous is the one before the next
		mCurrIndex = mNextIndex - 1;
		if (mNextIndex == index_AfterEnd) {
									// Next is after end, so previous is
									//   the last item
			mCurrIndex = (ArrayIndexT) mArray.GetCount();
		}

	} else if (mCurrIndex == index_AfterEnd) {
									// Previous is last item
		mCurrIndex = (ArrayIndexT) mArray.GetCount();

									// Do nothing if at beginning
	} else if ( (mCurrIndex != index_BeforeStart)  &&
				(mCurrIndex != index_ArrayDied) ) {
		mCurrIndex--;				// Previous is one before current
	}

	CalcNextIndex();
}


// ---------------------------------------------------------------------------
//	¥ SetNextIterator									  [inline] [protected]
// ---------------------------------------------------------------------------

#pragma mark LArrayIterator::SetNextIterator

// void
// LArrayIterator::SetNextIterator(
//		LArrayIterator*	inIterator)


// ---------------------------------------------------------------------------
//	¥ GetNextIterator									  [inline] [protected]
// ---------------------------------------------------------------------------

#pragma mark LArrayIterator::GetNextIterator

// LArrayIterator*
// LArrayIterator::GetNextIterator() const


// ---------------------------------------------------------------------------
//	¥ ArrayDied											  [inline] [protected]
// ---------------------------------------------------------------------------
//	Notify iterator that its Array has been deleted

#pragma mark LArrayIterator::ArrayDied

// void
// LArrayIterator::ArrayDied()


// ---------------------------------------------------------------------------
//	¥ ItemsInserted												   [protected]
// ---------------------------------------------------------------------------
//	Items have been inserted into the Array at the specified index

void
LArrayIterator::ItemsInserted(
	UInt32		inCount,
	ArrayIndexT	inAtIndex)
{
	if (mCurrIndex == index_Removed) {

			// Current item was removed. Keep "next" item the same
			// by adjusting next index if the items were inserted
			// at a lower index.

		if (inAtIndex <= mNextIndex) {
			mNextIndex += (ArrayIndexT) inCount;
		}

	} else {

			// Keep current item the same by adjusting its index
			// if the items were inserted at a lower index.

		if (inAtIndex <= mCurrIndex) {
			mCurrIndex += (ArrayIndexT) inCount;
		}
		CalcNextIndex();
	}
}


// ---------------------------------------------------------------------------
//	¥ ItemsRemoved												   [protected]
// ---------------------------------------------------------------------------
//	Items starting at the specified index have been removed from the Array

void
LArrayIterator::ItemsRemoved(
	UInt32		inCount,
	ArrayIndexT	inAtIndex)
{
	if (inAtIndex <= mCurrIndex) {	// Removed before or at current item

		if ((inAtIndex + inCount) <= mCurrIndex) {
									// All removed items are before the
									//   current item
			mCurrIndex -= (ArrayIndexT) inCount;
			CalcNextIndex();

		} else {					// Current item has been removed
			mNextIndex = inAtIndex;
			if (mNextIndex > mArray.GetCount()) {
				mNextIndex = index_AfterEnd;
			}
			mCurrIndex = index_Removed;
		}
	}
}


PP_End_Namespace_PowerPlant
