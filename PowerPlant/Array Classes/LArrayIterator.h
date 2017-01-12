// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LArrayIterator.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	LArrayIterator
//		Iterator for sequential traversal of an Array which adapts to
//		changes in the Array. This means you can add items, remove items,
//		or even delete the Array while iterating.
//
//	LLockedArrayIterator
//		Constructor locks Array; destructor unlocks Array. Use for Arrays
//		which aren't changed while iterating where you want to access a
//		pointer to the Array elements.
//
// ---------------------------------------------------------------------------
//	Using Iterators
//
//		Normally, you'll declare iterator objects on the stack to loop
//		through the items in an Array from within a function. You
//		can access the current, next, and previous Array items.
//
//		There are two ways to access items: get a copy or get a pointer.
//		Getting a copy is necessary if the Array may change while iterating

#ifndef _H_LArrayIterator
#define _H_LArrayIterator
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	¥ LArrayIterator ¥
// ===========================================================================

class	LArrayIterator {
	friend class LArray;
public:
						// ¥ Constants

	enum			{ from_Start	= 0L,		// Where to start iterating
					  from_End		= -1L };

												// ¥ Special Array Indexes
	enum			{ index_BeforeStart	= 0L,	// Next is first item
					  index_AfterEnd	= -2L,	// Prev is last item
					  index_Removed		= -4L,	// Current item was removed
					  index_ArrayDied	= -6L};	// Array was deleted

						// ¥ Constructor / Destructor

					LArrayIterator(
							const LArray&		inArray,
							ArrayIndexT			inPosition = from_Start);
							
					~LArrayIterator();

						// ¥ Positioning

	void			ResetTo( ArrayIndexT inPosition );

	ArrayIndexT		GetCurrentIndex() const
						{
							return mCurrIndex;
						}

						// ¥ Get Copy of fixed-size element

	Boolean			Current( void* outItem );
	
	Boolean			Next( void* outItem );
	
	Boolean			Previous( void* outItem );

						// ¥ Get Copy of variable-size element

	Boolean			Current(
							void				*outItem,
							UInt32				&ioItemSize);

	Boolean			Next(	void				*outItem,
							UInt32				&ioItemSize);

	Boolean			Previous(
							void				*outItem,
							UInt32				&ioItemSize);

						// ¥ Get pointer to fixed-size element

	void*			PtrToCurrent();
	void*			PtrToNext();
	void*			PtrToPrevious();

						// ¥ Get pointer to variable-size element

	void*			PtrToCurrent( UInt32& outItemSize );
	
	void*			PtrToNext( UInt32& outItemSize );
	
	void*			PtrToPrevious( UInt32& outItemSize );


protected:
	LArrayIterator	*mNextIterator;
	const LArray	&mArray;
	ArrayIndexT		mCurrIndex;
	ArrayIndexT		mNextIndex;

	void			CalcNextIndex();
	void			CalcPreviousIndex();

	void			SetNextIterator( LArrayIterator* inIterator );

	LArrayIterator*	GetNextIterator() const;

	void			ArrayDied();

	void			ItemsInserted(
							UInt32				inCount,
							ArrayIndexT			inAtIndex);
	void			ItemsRemoved(
							UInt32				inCount,
							ArrayIndexT			inAtIndex);
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ SetNextIterator									  [inline] [protected]
// ---------------------------------------------------------------------------

inline void
LArrayIterator::SetNextIterator(
	LArrayIterator*	inIterator)
{
	mNextIterator = inIterator;
}


// ---------------------------------------------------------------------------
//	¥ GetNextIterator									  [inline] [protected]
// ---------------------------------------------------------------------------

inline LArrayIterator*
LArrayIterator::GetNextIterator() const
{
	return mNextIterator;
}


// ---------------------------------------------------------------------------
//	¥ ArrayDied											  [inline] [protected]
// ---------------------------------------------------------------------------

inline void
LArrayIterator::ArrayDied()
{
	mCurrIndex = mNextIndex = index_ArrayDied;
}


// ===========================================================================
//	¥ LLockedArrayIterator ¥
// ===========================================================================

class LLockedArrayIterator : public LArrayIterator {
public:
					LLockedArrayIterator(
							const LArray		&inArray,
							ArrayIndexT			inPosition = from_Start)
						: LArrayIterator(inArray, inPosition)
						{
							mArray.Lock();
						}

					~LLockedArrayIterator()
						{
							mArray.Unlock();
						}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
