// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LVariableArray.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	An ordered collection of variable-size items. Positions in the list are
//	one-based--the first item is at index 1.

#ifndef _H_LVariableArray
#define _H_LVariableArray
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LVariableArray : public LArray {
public:
	typedef		UInt32	ArrayOffsetT, *ArrayOffsetP, **ArrayOffsetH;

						LVariableArray(
								LComparator*	inComparator = nil,
								Boolean			inKeepSorted = false);

						LVariableArray(
								const LVariableArray	&inOriginal);

						LVariableArray(
								Handle			inItemsHandle,
								ArrayOffsetH	inOffsetsHandle,
								LComparator*	inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

	LVariableArray&		operator = ( const LVariableArray& inVariableArray );

	virtual				~LVariableArray();

	virtual UInt32		GetItemSize( ArrayIndexT inIndex ) const;

	virtual UInt32		GrabItemSize( ArrayIndexT inIndex ) const;

	Handle				GetOffsetsHandle() const;

	virtual ArrayIndexT	AssignItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inValue,
								UInt32			inItemSize);

	virtual void*		GetItemPtr( ArrayIndexT inAtIndex ) const;

	virtual void		AdjustAllocation(
								UInt32			inExtraItems,
								UInt32			inExtraData = 0);

	virtual void		Sort();

protected:
	ArrayOffsetH		mItemOffsetsH;
	UInt32				mItemsAllocated;

							// Internal Handle Maintenance

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

	virtual void		InternalCopyItem(
								ArrayIndexT		inSourceIndex,
								ArrayIndexT		inDestIndex);

							// Internal Accessors

	virtual UInt32		GrabItemRangeSize(
								ArrayIndexT		inStartIndex,
								ArrayIndexT		inEndIndex) const;

	UInt32				GetItemOffset( ArrayIndexT inAtIndex );
	
private:
	void				CopyVariableArray(
								const LVariableArray&	inOriginal,
								bool					inDestroyOldData);

	void				DestroyVariableArray();
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ GetItemOffset										  [inline] [protected]
// ---------------------------------------------------------------------------

inline UInt32
LVariableArray::GetItemOffset(
	ArrayIndexT		inAtIndex)
{
	return (*mItemOffsetsH)[inAtIndex - 1];
}


// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
