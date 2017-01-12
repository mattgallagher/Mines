// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TArray.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Template-based Array class
//
//	¥ Use Note: TArray is a template wrapper for LArray. You can't store
//	actual objects in a TArray or LArray. Only use TArray and LArray to
//	store pointers to objects (created via new), built-in numerical data
//	types, or data structures (simple structs).

#ifndef _H_TArray
#define _H_TArray
#pragma once

#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

template <class T> class TArray : public LArray {
public:
					TArray()
						: LArray(sizeof(T), (LComparator*) nil, false) { }

					TArray(	LComparator		*inComparator,
							Boolean			inKeepSorted = false)
						: LArray(sizeof(T), inComparator, inKeepSorted) { }

					TArray( UInt32			inItemCount,
							LComparator		*inComparator = nil,
							Boolean			inKeepSorted = false)
						: LArray(inItemCount, sizeof(T),
									inComparator, inKeepSorted) { }

					TArray(	Handle			inItemsHandle,
							LComparator		*inComparator = nil,
							Boolean			inIsSorted = false,
							Boolean			inKeepSorted = false)
						: LArray(sizeof(T), inItemsHandle, inComparator,
									inIsSorted, inKeepSorted) { }

	virtual			~TArray() { }

						// Warning: The T& returned by both operator []
						// functions is a pointer to data within a
						// relocatable Handle block. You may need to
						// lock the Array.

						// Also, operator [] performs no range checking

	T&				operator [] ( ArrayIndexT inAtIndex )
						{
							return *(T*) (*mItemsH + (inAtIndex - 1) * sizeof(T));
						}

	const T&		operator [] ( ArrayIndexT inAtIndex ) const
						{
							return *(T*) (*mItemsH + (inAtIndex - 1) * sizeof(T));
						}

	Boolean			FetchItemAt(
							ArrayIndexT		inAtIndex,
							T&				outItem) const
						{
							return LArray::FetchItemAt(inAtIndex, &outItem);
						}

	ArrayIndexT		FetchIndexOf( const T& inItem ) const
						{
							return LArray::FetchIndexOf(&inItem);
						}

	bool			ContainsItem( const T& inItem ) const
						{
							return LArray::ContainsItem(&inItem);
						}

	ArrayIndexT		AddItem( const T& inItem )
						{
							 return LArray::AddItem(&inItem);
						}

	ArrayIndexT		InsertItemsAt(
							UInt32			inCount,
							ArrayIndexT		inAtIndex,
							const T&		inItem)
						{
							return LArray::InsertItemsAt(inCount, inAtIndex,
															&inItem, sizeof(T));
						}

	ArrayIndexT		AssignItemsAt(
							UInt32			inCount,
							ArrayIndexT		inAtIndex,
							const T&		inValue)
						{
							return LArray::AssignItemsAt(inCount, inAtIndex,
															&inValue);
						}

	void			Remove( const T& inItem )
						{
							LArray::Remove(&inItem);
						}

	bool			RemoveLastItem( T& outItem )
						{
							return LArray::RemoveLastItem(&outItem);
						}

						// WARNING: The T* returned is a pointer to data
						// in a relocatable Handle block. You may need to
						// lock the Array.

	T*				FetchItemPtr( ArrayIndexT inAtIndex ) const
						{
							return (T*) (*mItemsH + (inAtIndex - 1) * sizeof(T));
						}

	ArrayIndexT		FetchInsertIndexOf( const T& inItem ) const
						{
							return LArray::FetchInsertIndexOf(&inItem);
						}

private:									// Prevent warnings about hiding
	using LArray::FetchItemAt;				// inherited virtual functions
	using LArray::FetchIndexOf;
	using LArray::AddItem;
	using LArray::InsertItemsAt;
	using LArray::AssignItemsAt;
	using LArray::Remove;
	using LArray::RemoveLastItem;
	using LArray::FetchInsertIndexOf;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
