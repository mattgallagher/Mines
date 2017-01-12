// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LArray.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	LArray
//		A ordered sequence of fixed-sized items. Positions in the Array are
//		one-based--the first item is at index 1.
//
//	StArrayLocker
//		Constructor locks an Array. Destructor unlocks it.

#ifndef _H_LArray
#define _H_LArray
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LComparator;
class LArrayIterator;

typedef	SInt32			ArrayIndexT;


// ===========================================================================
//	¥ LArray ¥
// ===========================================================================

class	LArray {
	friend class LArrayIterator;
	friend class LLockedArrayIterator;

public:
	enum				{ index_Bad		= 0,
						  index_First	= 1,
						  index_Last	= 0x7FFFFFFF };

						LArray();

						LArray(	UInt32			inItemCount,
								UInt32			inItemSize,
								LComparator*	inComparator = nil,
								Boolean			inKeepSorted = false);

						LArray(	UInt32			inItemSize,
								LComparator*	inComparator = nil,
								Boolean			inKeepSorted = false);

						LArray(	UInt32			inItemSize,
								Handle			inItemsHandle,
								LComparator*	inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

						LArray(	const LArray&	inOriginal);

	LArray&				operator = ( const LArray& inArray );

	virtual				~LArray();

	bool				ValidIndex( ArrayIndexT& ioIndex) const;

	virtual UInt32		GetItemSize( ArrayIndexT inIndex ) const;

	virtual UInt32		GrabItemSize( ArrayIndexT inIndex ) const;

	UInt32				GetCount() const;
	
	bool				IsSorted() const;
	
	void				InvalidateSort();
	
	bool				IsKeptSorted() const;

	Handle				GetItemsHandle() const;

	virtual Boolean		FetchItemAt(
								ArrayIndexT		inAtIndex,
								void*			outItem) const;

	virtual Boolean		FetchItemAt(
								ArrayIndexT		inAtIndex,
								void*			outItem,
								UInt32&			ioItemSize) const;

	virtual ArrayIndexT	FetchIndexOf(
								const void*		inItem,
								UInt32			inItemSize = 0) const;

	bool				ContainsItem(
								const void*		inItem,
								UInt32			inItemSize = 0) const;

	virtual ArrayIndexT	AddItem(
								const void*		inItem,
								UInt32			inItemSize = 0);

	virtual ArrayIndexT	InsertItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inItem,
								UInt32			inItemSize = 0);

	virtual void		RemoveItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex);

	void				RemoveAllItemsAfter( ArrayIndexT inAfterIndex );

	virtual bool		RemoveLastItem( void* outItem );

	virtual bool		RemoveLastItem(
								void*			outItem,
								UInt32&			ioItemSize);

	virtual ArrayIndexT	AssignItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inValue,
								UInt32			inItemSize = 0);

	virtual void		SwapItems(
								ArrayIndexT		inIndexA,
								ArrayIndexT		inIndexB);

	virtual void		MoveItem(
								ArrayIndexT		inFromIndex,
								ArrayIndexT		inToIndex);

	virtual void		Remove(	const void*		inItem,
								UInt32			inItemSize = 0);

	void				Lock() const;
	
	void				Unlock() const;
	
	bool				IsLocked() const;

	virtual void*		GetItemPtr( ArrayIndexT inAtIndex ) const;

	virtual void		AdjustAllocation(
								UInt32			inExtraItems,
								UInt32			inExtraData = 0);

	virtual void		Sort();

	void				SetKeepSorted( Boolean inKeepSorted );

	void				SetComparator(
								LComparator*	inComparator,
								Boolean			inTakeOwnership = true);

	LComparator*		GetComparator() const;

	virtual ArrayIndexT	FetchIndexOfKey( const void* inKey ) const;

	virtual ArrayIndexT	FetchInsertIndexOf(
								const void*		inItem,
								UInt32			inItemSize = 0) const;

	virtual ArrayIndexT	FetchInsertIndexOfKey( const void* inKey ) const;

protected:
	UInt32					mItemSize;
	UInt32					mItemCount;
	SInt32					mDataStored;
	SInt32					mDataAllocated;
	Handle					mItemsH;
	mutable UInt32			mLockCount;
	mutable LArrayIterator*	mIteratorListHead;
	LComparator*			mComparator;
	bool					mOwnsComparator;
	bool					mIsSorted;
	bool					mKeepSorted;

							// Iterator Support

	void				AttachIterator( LArrayIterator* inIterator ) const;

	void				DetachIterator( LArrayIterator* inIterator ) const;

	void				ItemsInserted(
								UInt32			inCount,
								ArrayIndexT		inAtIndex);

	void				ItemsRemoved(
								UInt32			inCount,
								ArrayIndexT		inAtIndex);


							// Internal Handle Maintenance

	virtual void		PeekItem(
								ArrayIndexT		inAtIndex,
								void*			outItem) const;

	virtual void		PokeItem(
								ArrayIndexT		inAtIndex,
								const void*		inItem,
								UInt32			inItemSize);

	virtual void		InternalAdjustAllocation(
								UInt32			inItemAllocation,
								UInt32			inDataAllocation);

	virtual void		AdjustStorage(
								SInt32			inDeltaItems,
								SInt32			inDeltaData);

	virtual void		ShiftItems(
								ArrayIndexT		inStartIndex,
								ArrayIndexT		inEndIndex,
								SInt32			inIndexShift,
								SInt32			inDataShift);

	virtual void		StoreNewItems(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inValue,
								UInt32			inItemSize);

	virtual void		InternalSwapItems(
								ArrayIndexT		inIndexA,
								ArrayIndexT		inIndexB,
								void*			inBuffer);

	virtual void		InternalMoveItem(
								ArrayIndexT		inFromIndex,
								ArrayIndexT		inToIndex,
								void*			inBuffer);

	virtual void		InternalCopyItem(
								ArrayIndexT		inSourceIndex,
								ArrayIndexT		inDestIndex);

							// Searching

	ArrayIndexT			LinearSearch(
								const void*		inItem,
								UInt32			inItemSize) const;

	ArrayIndexT			LinearSearchByKey(
								const void*		inKey) const;

	ArrayIndexT			BinarySearch(
								const void*		inItem,
								UInt32			inItemSize) const;

	ArrayIndexT			BinarySearchByKey(
								const void*		inKey) const;

							// Internal Accessors

	virtual UInt32		GrabItemRangeSize(
								ArrayIndexT		inStartIndex,
								ArrayIndexT		inEndIndex) const;

private:
	void				InitArray(
								UInt32			inItemSize,
								LComparator*	inComparator,
								Boolean			inIsSorted,
								Boolean			inKeepSorted);

	void				CopyArray(
								const LArray&	inOriginal,
								bool			inDestroyOldData);

	void				DestroyArray();
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ GetItemSize											 [inline] [public]
// ---------------------------------------------------------------------------

inline UInt32
LArray::GetItemSize(
	ArrayIndexT	/* inIndex */) const
{
	return mItemSize;
}


// ---------------------------------------------------------------------------
//	¥ GrabItemSize											 [inline] [public]
// ---------------------------------------------------------------------------

inline UInt32
LArray::GrabItemSize(
	ArrayIndexT	/* inIndex */) const
{
	return mItemSize;
}


// ---------------------------------------------------------------------------
//	¥ GetCount												 [inline] [public]
// ---------------------------------------------------------------------------

inline UInt32
LArray::GetCount() const
{
	return mItemCount;
}


// ---------------------------------------------------------------------------
//	¥ IsSorted												 [inline] [public]
// ---------------------------------------------------------------------------

inline bool
LArray::IsSorted() const
{
	return mIsSorted;
}


// ---------------------------------------------------------------------------
//	¥ InvalidateSort										 [inline] [public]
// ---------------------------------------------------------------------------

inline void
LArray::InvalidateSort()
{
	mIsSorted = false;
}


// ---------------------------------------------------------------------------
//	¥ IsKeptSorted											 [inline] [public]
// ---------------------------------------------------------------------------

inline bool
LArray::IsKeptSorted() const
{
	return mKeepSorted;
}


// ---------------------------------------------------------------------------
//	¥ ContainsItem											 [inline] [public]
// ---------------------------------------------------------------------------

inline bool
LArray::ContainsItem(
	const void*		inItem,
	UInt32			inItemSize) const
{
	return (FetchIndexOf(inItem, inItemSize) != index_Bad);
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllItemsAfter									 [inline] [public]
// ---------------------------------------------------------------------------

inline void
LArray::RemoveAllItemsAfter(
	ArrayIndexT		inAfterIndex)
{
	RemoveItemsAt(mItemCount - inAfterIndex, inAfterIndex + 1);
}


// ---------------------------------------------------------------------------
//	¥ IsLocked												 [inline] [public]
// ---------------------------------------------------------------------------

inline bool
LArray::IsLocked() const
{
	return (mLockCount > 0);
}


// ---------------------------------------------------------------------------
//	¥ GetComparator											 [inline] [public]
// ---------------------------------------------------------------------------

inline LComparator*
LArray::GetComparator() const
{
	return mComparator;
}


// ---------------------------------------------------------------------------
//	¥ GrabItemRangeSize									  [inline] [protected]
// ---------------------------------------------------------------------------

inline UInt32
LArray::GrabItemRangeSize(
	ArrayIndexT		inStartIndex,
	ArrayIndexT		inEndIndex) const
{
	return (inEndIndex - inStartIndex + 1) * mItemSize;
}


// ===========================================================================
//	¥ StArrayLocker ¥
// ===========================================================================

class	StArrayLocker {
public:
						StArrayLocker( const LArray& inArray )
							: mArray(inArray)
							{
								mArray.Lock();
							}

						~StArrayLocker()
							{
								mArray.Unlock();
							}

protected:
	const LArray&	mArray;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
