// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPictureMixin.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAPictureMixin
#define _H_LGAPictureMixin
#pragma once

#include <LPane.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAPictureMixin {
public:
						LGAPictureMixin();

						LGAPictureMixin( const LGAPictureMixin& inOriginal );

	virtual				~LGAPictureMixin();

	PicHandle			GetPictureHandle() const;

	ResIDT				GetPictureResourceID() const	{ return mPictureResID; }

	bool				HasPictureHandle() const		{ return mPictureH != nil; }

	bool				DisposesPictureH() const		{ return mDisposePictureH; }

	virtual	SDimension16	GetPictureSize() const;

	virtual	void		GetPictureFrame( Rect& outFrame );

	virtual	void		CalcLocalPictureRect( Rect& outRect );

	virtual	void		SetPictureHandle(
								PicHandle	inPictureH,
								bool		inDisposesHandle = false);

	virtual	void		SetPictureResourceID( ResIDT inResID );

	void				DisposePictureHandle();

protected:

	PicHandle		mPictureH;				//	Handle to the actual picture
	ResIDT			mPictureResID;			//	Resource ID of picture we will be drawing
	bool			mDisposePictureH;		//	Do we dispose of picture when finished?

	virtual	void		CalcPictureLocation( Point& outIconLocation );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
