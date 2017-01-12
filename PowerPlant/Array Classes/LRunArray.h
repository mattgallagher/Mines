// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRunArray.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of fixed-size items, where consecutive items
//	with identical data are stored as a single entry. Positions are
//	one-based--the first item is at index 1.

#ifndef _H_LRunArray
#define _H_LRunArray
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LRunArray : public LArray {
public:
						LRunArray(
								UInt32			inItemSize,
								LComparator*	inComparator = nil,
								Boolean			inKeepSorted = false);

						LRunArray(
								UInt32			inItemSize,
								Handle			inItemsHandle,
								Handle			inRunsHandle,
								LComparator*	inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

						LRunArray( const LRunArray& inOriginal );

	LRunArray&			operator = ( const LRunArray& inRunArray );

	virtual				~LRunArray();

	Handle				GetRunsHandle() const	{ return mRunsH; }

	SInt32				GetRunCount() const		{ return mRunCount; }

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

	virtual ArrayIndexT	AssignItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inValue,
								UInt32			inItemSize = 0);

	virtual void*		GetItemPtr( ArrayIndexT	 inAtIndex ) const;

protected:
	Handle			mRunsH;
	SInt32			mRunCount;

	struct SRunRecord {
		ArrayIndexT	start;
		ArrayIndexT	end;
	};

	virtual void		PokeItem(
								ArrayIndexT		inAtIndex,
								const void*		inItem,
								UInt32			inItemSize);

	virtual void		InternalMoveItem(
								ArrayIndexT		inFromIndex,
								ArrayIndexT		inToIndex,
								void*			inBuffer);

	ArrayIndexT			GetRunIndex( ArrayIndexT inItemIndex ) const;

	void*				PtrToRunItem( ArrayIndexT inRunIndex ) const;

	ArrayIndexT			RunStart( ArrayIndexT inRunIndex ) const;

	void				InsertRun(
								ArrayIndexT		inRunIndex,
								UInt32			inCount,
								const void*		inItem);

	void				SplitRun(
								ArrayIndexT		inRunIndex,
								ArrayIndexT		inItemIndex,
								UInt32			inCount,
								const void*		inItem);

	void				ExtendRun(
								ArrayIndexT		inRunIndex,
								UInt32			inCount);

private:
	void				CopyRunArray(
								const LRunArray&	inRunArray,
								bool				inDestroyOldData);

	void				DestroyRunArray();
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ PtrToRunItem										  [inline] [protected]
// ---------------------------------------------------------------------------

inline void*
LRunArray::PtrToRunItem(
	ArrayIndexT		inRunIndex) const
{
	return (*mItemsH + mItemSize * inRunIndex);
}


// ---------------------------------------------------------------------------
//	¥ RunStart											  [inline] [protected]
// ---------------------------------------------------------------------------

inline ArrayIndexT
LRunArray::RunStart(
	ArrayIndexT		inRunIndex) const
{
	return (((SRunRecord*)(*mRunsH))[inRunIndex].start);
}


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
