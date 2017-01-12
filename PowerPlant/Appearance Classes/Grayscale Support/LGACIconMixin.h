// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACIconMixin.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGACIconMixin
#define _H_LGACIconMixin
#pragma once

#include <LPane.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGACIconMixin {
public:
							LGACIconMixin();
							
							LGACIconMixin( const LGACIconMixin& inOriginal );

	virtual					~LGACIconMixin();


	CIconHandle				GetCIconSuiteH() const	{ return mCIconH; }

	ResIDT					GetCIconResourceID() const	{ return mCIconResID; }

	SDimension16			GetCIconSize();

	bool					GetHiliteCIcon() const	{ return mHiliteCIcon; }

	bool					GetOffsetCIconOnHilite() const
								{
									return mOffsetCIconOnHilite;
								}

	bool					GetClickInCIcon() const	{ return mClickInCIcon; }

	bool					DisposesCIconH() const	{ return mDisposeCIconH; }

	virtual	void			LoadCIconHandle();

	virtual	void			CalcLocalCIconRect( Rect& outRect );


						// Setters

	virtual	void			SetCIconH(
									CIconHandle	inCIconH,
									bool		inDisposesHandle = false);

	virtual	void			SetCIconResourceID( ResIDT inResID );

	void					SetHiliteCIcon( bool inHiliteCIcon )
								{
									mHiliteCIcon = inHiliteCIcon;
								}

	void					SetOffsetCIconOnHilite( bool inOffsetCIcon )
								{
									mOffsetCIconOnHilite = inOffsetCIcon;
								}

	void					SetClickInCIcon( bool inClickInCIcon )
								{
									mClickInCIcon = inClickInCIcon;
								}


protected:

	CIconHandle		mCIconH;				//	Handle to the actual color icon
	ResIDT			mCIconResID;			//	Resource ID of icon we will be drawing
	Point			mCIconLocation;			//	Location of the icon within the pane
	bool			mHiliteCIcon;			//	Does icon hilite when button is pressed
	bool			mOffsetCIconOnHilite;	//	Does icon move down and to right when hilited
	bool			mClickInCIcon;			//	This flag is used to tell if hit testing
											//		should only be handled in the actual mask
											//		of the icon, instead of the entire
											//		pane frame.
	bool			mDisposeCIconH;			//	Specifies if we are responsible for disposing
											//		of the icon handle


						// Miscellaneous
	virtual void				CalcCIconLocation	(
											Point	&outIconLocation );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
