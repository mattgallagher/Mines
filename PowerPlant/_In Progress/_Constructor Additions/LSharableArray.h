// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharableArray.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSharableArray
#define _H_LSharableArray
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LSharable;


// ===========================================================================
//		¥ LSharableArray
// ===========================================================================
//	An array that keeps a list of LSharable* pointers. The use count
//	of each sharable object is incremented while it is in the array,
//	so the array will not have any dangling pointers.

class LSharableArray : public LArray {

public:
						LSharableArray(
								LComparator*	inComparator = nil,
								Boolean			inKeepSorted = false);

						LSharableArray(
								Handle			inItemsHandle,
								LComparator*	inComparator = nil,
								Boolean			inIsSorted = false,
								Boolean			inKeepSorted = false);

						LSharableArray( const LSharableArray& inOriginal );

	LSharableArray&		operator = ( const LSharableArray& inOriginal );

	virtual				~LSharableArray();

	// Sharable item manipulators.

	virtual void		InsertItemAt(
								ArrayIndexT		inAtIndex,
								LSharable*		inItem);

	virtual void		RemoveItem( LSharable* inItem );

	virtual void		RemoveItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex);


private:
	void				InitSharableArray();

	// These functions cannot be used with LSharableArrays.

	virtual ArrayIndexT	InsertItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inItem,
								UInt32			inItemSize = 0);

	virtual void		Remove(	const void*		inItem,
								UInt32			inItemSize = 0);

	virtual ArrayIndexT	AssignItemsAt(
								UInt32			inCount,
								ArrayIndexT		inAtIndex,
								const void*		inValue,
								UInt32			inItemSize = 0);

	virtual void		MoveItem(
								ArrayIndexT		inFromIndex,
								ArrayIndexT		inToIndex);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
