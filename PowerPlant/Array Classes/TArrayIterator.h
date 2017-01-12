// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TArrayIterator.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Template-based Iterator classes for use with TArray

#ifndef _H_TArrayIterator
#define _H_TArrayIterator
#pragma once

#include <LArrayIterator.h>
#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ TArrayIterator
// ---------------------------------------------------------------------------

template <class T> class TArrayIterator : public LArrayIterator {
public:
					TArrayIterator(
							const TArray<T>	&inArray,
							ArrayIndexT		inPosition = LArrayIterator::from_Start)

						: LArrayIterator(inArray, inPosition) { }

					~TArrayIterator() { }

	Boolean			Current( T &outItem )
						{
							return LArrayIterator::Current(&outItem);
						}

	Boolean			Next( T &outItem )
						{
							return LArrayIterator::Next(&outItem);
						}

	Boolean			Previous( T &outItem )
						{
							return LArrayIterator::Previous(&outItem);
						}

	T*				PtrToCurrent()
						{
							return (T*) LArrayIterator::PtrToCurrent();
						}

	T*				PtrToNext()
						{
							return (T*) LArrayIterator::PtrToNext();
						}

	T*				PtrToPrevious()
						{
							return (T*) LArrayIterator::PtrToPrevious();
						}
};


// ---------------------------------------------------------------------------
//	¥ TLockedArrayIterator
// ---------------------------------------------------------------------------

template <class T> class TLockedArrayIterator : public TArrayIterator<T> {
public:
					TLockedArrayIterator(
							const TArray<T>	&inArray,
							ArrayIndexT		inPosition = LArrayIterator::from_Start)

						: TArrayIterator<T>(inArray, inPosition)
						{
							TArrayIterator<T>::mArray.Lock();
						}

					~TLockedArrayIterator()
						{
							TArrayIterator<T>::mArray.Unlock();
						}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
