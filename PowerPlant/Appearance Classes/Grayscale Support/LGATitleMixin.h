// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATitleMixin.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

/*
	THEORY OF OPERATION

This class provides some  behavior that can be mixed in to another class that needs to
display a title.

There is one pure virtual function CalcTitleRect() that has to be overridden so that a
valid rect can be calculated for the rendering of the title.  This would be called
from the title drawing code.

Fields are provided for the title, a flag that indicates whether the title should be
hilited, and a color that is used to hilite the title.

NOTE: accessors have not been provided for the title, as it was intend that this mixin
be used with controls so the user would have to override GetDescriptor() and
SetDescriptor() to get at the title.

*/

#ifndef _H_LGATitleMixin
#define _H_LGATitleMixin
#pragma once

#include <LString.h>
#include <LControl.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGATitleMixin {
public:
						LGATitleMixin();

						LGATitleMixin( const LGATitleMixin& inOriginal );

	virtual				~LGATitleMixin();

							// Getters

	bool				HasTitle() const		{ return (mTitle.Length() != 0); }

	ResIDT				GetTextTraitsID() const	{ return mTextTraitsID; }

	bool				GetHiliteTitle() const	{ return mHiliteTitle; }

	RGBColor			GetTitleHiliteColor() const	{ return mTitleHiliteColor; }

	virtual	void		CalcTitleRect( Rect& outRect ) = 0;

							// Setters

	virtual	void		SetTextTraitsID( ResIDT inTextTraitID );

	virtual	void		SetHiliteTitle( bool inHiliteTitle );

	virtual	void		SetTitleHiliteColor( RGBColor inHiliteColor );

protected:
	LStr255			mTitle;					//	The title for the control if there is one,
											//		if the control has no title this is set
											//		to be an empty string
	ResIDT			mTextTraitsID;			//	ID of the text trait that will be used to
											//		render the title
	bool			mHiliteTitle;			//	Does title get hilited
	RGBColor		mTitleHiliteColor;		//	Color to be used when render the title in
											//		hilite mode
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
