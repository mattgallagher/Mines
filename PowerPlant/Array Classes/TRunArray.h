// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TRunArray.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Template-based RunArray class
//
//	Use Note: TRunArray is a template wrapper for LRunArray. You can't store
//	actual objects in a TRunArray or LRunArray. Only use these classes to
//	store pointers to objects (created via new), built-in numerical data
//	types, or data structures (simple structs).

#ifndef _H_TRunArray
#define _H_TRunArray
#pragma once

#include <LRunArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

template <class T> class TRunArray : public LRunArray {
public:
					TRunArray(
							LComparator		*inComparator = nil,
							Boolean			inKeepSorted = false)
								: LRunArray(sizeof(T), inComparator,
									inKeepSorted) { }

					TRunArray(
							Handle			inItemsHandle,
							Handle			inRunsHandle,
							LComparator		*inComparator = nil,
							Boolean			inIsSorted = false,
							Boolean			inKeepSorted = false)
								: LRunArray(sizeof(T), inItemsHandle,
									inRunsHandle, inComparator,
									inIsSorted, inKeepSorted) { }

	virtual			~TRunArray() { }

						// Warning: The const T& returned by operator []
						// functions is a pointer to data within a
						// relocatable Handle block. You may need to
						// lock the Array.

						// Also, operator [] performs no range checking

						// Note that there is no operator[] returning
						// a non-const T&. That's because assigning to
						// an element in a RunArray is not allowed. Doing
						// so would change all elements in the same run,
						// not just the one element.

	const T&		operator [] (
							ArrayIndexT		inAtIndex) const
						{
							return *(T*) GetItemPtr(inAtIndex);
						}

	Boolean			FetchItemAt(
							ArrayIndexT		inAtIndex,
							T				&outItem) const
						{
							return LRunArray::FetchItemAt(inAtIndex, &outItem);
						}

	ArrayIndexT		FetchIndexOf(
							const T			&inItem) const
						{
							return LRunArray::FetchIndexOf(&inItem);
						}

	ArrayIndexT		AddItem(
							const T			&inItem)
						{
							 return LRunArray::AddItem(&inItem, sizeof(T));
						}

	ArrayIndexT		InsertItemsAt(
							UInt32			inCount,
							ArrayIndexT		inAtIndex,
							const T			&inItem)
						{
							return LRunArray::InsertItemsAt(inCount, inAtIndex,
															&inItem, sizeof(T));
						}

	ArrayIndexT		AssignItemsAt(
							UInt32			inCount,
							ArrayIndexT		inAtIndex,
							const T			&inValue)
						{
							return LRunArray::AssignItemsAt(inCount, inAtIndex,
															&inValue);
						}

	void			Remove(
							const T			&inItem)
						{
							LRunArray::Remove(&inItem);
						}

						// WARNING: The T* returned is a pointer to data
						// in a relocatable Handle block. You may need to
						// lock the Array.

	T*				FetchItemPtr(
							ArrayIndexT		inAtIndex) const
						{
							return (T*) GetItemPtr(inAtIndex);
						}

	ArrayIndexT		FetchInsertIndexOf(
							const T			&inItem) const
						{
							return LRunArray::FetchInsertIndexOf(&inItem);
						}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
