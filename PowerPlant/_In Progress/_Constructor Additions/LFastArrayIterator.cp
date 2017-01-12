// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFastArrayIterator.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFastArrayIterator.h>

PP_Begin_Namespace_PowerPlant

const SInt32 ArrayIndex_ArrayDied	= -6;	// Array was deleted


// ---------------------------------------------------------------------------
//	¥ LFastArrayIterator
// ---------------------------------------------------------------------------
//	Constructor

LFastArrayIterator::LFastArrayIterator(
	const LArray&	inArray,
	ArrayIndexT		inPosition)

	: LArrayIterator((const_cast<LArray&>(inArray)), inPosition)
{
	inArray.Lock();
	mItemSize = (SInt32) inArray.GetItemSize(1);
	ResetTo(inPosition);
}


// ---------------------------------------------------------------------------
//	¥ ~LFastArrayIterator
// ---------------------------------------------------------------------------
//	Destructor

LFastArrayIterator::~LFastArrayIterator()
{
	mArray.Unlock();
}


// ---------------------------------------------------------------------------
//	¥ ResetTo
// ---------------------------------------------------------------------------
//	Move the iterator pointer to a new location in the array.

void
LFastArrayIterator::ResetTo(
	ArrayIndexT	inPosition)
{
	if (mCurrIndex == ArrayIndex_ArrayDied)
		return;

	if ((inPosition > mArray.GetCount()) || (inPosition < 0))
		inPosition = (SInt32) mArray.GetCount() + 1;

	if (mArray.GetCount() < 1)
		mItemPtr = nil;
	else
		mItemPtr = mArray.GetItemPtr(inPosition);

	mIndex = inPosition;
}


// ---------------------------------------------------------------------------
//	¥ Current
// ---------------------------------------------------------------------------
//	Return a pointer to the current item.

Boolean
LFastArrayIterator::Current(
	void*	outItem)
{
	UInt32 size = (UInt32) mItemSize;
	return Current(outItem, size);
}


// ---------------------------------------------------------------------------
//	¥ Current
// ---------------------------------------------------------------------------
//	Return a pointer to the current item.

Boolean
LFastArrayIterator::Current(
	void*		outItem,
	UInt32&		ioItemSize)
{

	// Do range check first.

	if ((mIndex < 1) || (mIndex > mArray.GetCount()) || (mCurrIndex == ArrayIndex_ArrayDied))
		return false;

	// Fill in actual item size.

	if (ioItemSize == 0)
		ioItemSize = (UInt32) mItemSize;

	// Return item. Optimize for 4-byte values (long ints or pointers).

	if (ioItemSize == sizeof (long))
		*(long*) outItem = *(long*) mItemPtr;
	else
		::BlockMoveData(mItemPtr, outItem, (SInt32) ioItemSize);

	return true;

}


// ---------------------------------------------------------------------------
//	¥ Next
// ---------------------------------------------------------------------------
//	Advance the iterator by one position and return a pointer to the
//	new item.

Boolean
LFastArrayIterator::Next(
	void*	outItem)
{
	UInt32 size = (UInt32) mItemSize;
	return Next(outItem, size);
}


// ---------------------------------------------------------------------------
//	¥ Next
// ---------------------------------------------------------------------------
//	Advance the iterator by one position and return a pointer to the
//	new item.

Boolean
LFastArrayIterator::Next(
	void*		outItem,
	UInt32&		ioItemSize)
{
	if (mCurrIndex == ArrayIndex_ArrayDied)
		return false;

	if ((mIndex >= 0) && (mIndex <= mArray.GetCount())) {
		mIndex++;
		mItemPtr = (void *)((char *)mItemPtr + mItemSize);
		return Current(outItem, ioItemSize);
	}
	else
		return false;
}


// ---------------------------------------------------------------------------
//	¥ Previous
// ---------------------------------------------------------------------------
//	Back up the iterator by one position and return a pointer to the
//	new item.

Boolean
LFastArrayIterator::Previous(
	void*	outItem)
{
	UInt32 size = (UInt32) mItemSize;
	return Previous(outItem, size);
}


// ---------------------------------------------------------------------------
//	¥ Previous
// ---------------------------------------------------------------------------
//	Back up the iterator by one position and return a pointer to the
//	new item.

Boolean
LFastArrayIterator::Previous(
	void*		outItem,
	UInt32&		ioItemSize)
{
	if (mCurrIndex == ArrayIndex_ArrayDied)
		return false;

	if ((mIndex > 0) && (mIndex <= (mArray.GetCount() + 1))) {
		mIndex--;
		mItemPtr = (void *)((char *)mItemPtr - mItemSize);
		return Current(outItem, ioItemSize);
	}
	else
		return false;
}

PP_End_Namespace_PowerPlant
