// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFastArrayIterator.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFastArrayIterator
#define _H_LFastArrayIterator
#pragma once

#include <LArrayIterator.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

enum {
	from_Start	= 0L,		// Where to start iterating
	from_End	= -1L
};


// ===========================================================================
//		¥ LFastArrayIterator
// ===========================================================================
//	A faster version of LArrayIterator for arrays that won't
//	change while the array is being iterated.

class LFastArrayIterator : public LArrayIterator {

public:
						LFastArrayIterator(
								const LArray&	inArray,
								ArrayIndexT		inPosition = from_Start);
	virtual				~LFastArrayIterator();

	void				ResetTo(
								ArrayIndexT		inPosition);

	Boolean				Current(void*			outItem);
	Boolean				Current(void*			outItem,
								UInt32&			ioItemSize);

	Boolean				Next(	void*			outItem);
	Boolean				Next(	void*			outItem,
								UInt32&			ioItemSize);

	Boolean				Previous(void*			outItem);
	Boolean				Previous(void*			outItem,
								UInt32&			ioItemSize);

	ArrayIndexT			GetCurrentIndex() const
								{
									return mIndex;
								}

protected:
	void*				mItemPtr;
	SInt32				mItemSize;
	ArrayIndexT			mIndex;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
