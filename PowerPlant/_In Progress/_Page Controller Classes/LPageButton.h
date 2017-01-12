// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageButton.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

/*
THEORY OF OPERATION

This class implements the button that is used by the page controller to
select pages.

It can render itself in its normal state or selected state, which has the
button appear to slide down. It also displays a title which changes color
when the button is on.

NOTE:  See the page controller header file for more details.

*/

#ifndef _H_LPageButton
#define _H_LPageButton
#pragma once

#include <LGATitleMixin.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// Color Constants

extern const	RGBColor	pageButton_DefaultBackColor;
extern const	RGBColor	pageButton_DefaultFaceColor;
extern const	RGBColor	pageButton_TitleHiliteColor;

// ---------------------------------------------------------------------------

class LPageButton : public LControl,
					public LGATitleMixin {
public:
	enum { class_ID = FOUR_CHAR_CODE('pgbt') };

						LPageButton();

						LPageButton( const LPageButton& inOriginal );

						LPageButton(
								const SPaneInfo& 	inPaneInfo,
								const SControlInfo& inControlInfo,
								ConstStringPtr		inTitle,
								Boolean				inHiliteTitle,
								ResIDT				inTextTraitsID,
								const RGBColor&		inBackColor = pageButton_DefaultBackColor,
								const RGBColor&		inFaceColor = pageButton_DefaultFaceColor,
								const RGBColor&		inPushedTextColor = pageButton_TitleHiliteColor);

	virtual				~LPageButton();


							// Getters

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual	void		CalcTitleRect( Rect& outRect );

	virtual	Boolean		IsSelected() const	{ return GetValue() > 0; }

	virtual	Boolean		IsPushed () const	{ return mPushed; }

	virtual	RgnHandle	CalcButtonRegion();

	virtual	Boolean		IsLeftMostButton();


							// Setters

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual	void		SetPushedState( Boolean	inHiliteState );

	virtual	void		SetValue( SInt32 inValue );

	virtual	void		SetPageButtonBackColor( RGBColor inBackColor )
							{
								mBackColor = inBackColor;
							}

	virtual	void		SetPageButtonFaceColor( RGBColor inFaceColor )
							{
								mFaceColor = inFaceColor;
							}

	virtual	void		SetPageButtonPushedTextColor( RGBColor inPushedTextColor )
							{
								mPushedTextColor = inPushedTextColor;
							}


							// Enabling & Disabling

	virtual	void		EnableSelf();
	
	virtual	void		DisableSelf();


							// Activation

	virtual	void		ActivateSelf();
	
	virtual	void		DeactivateSelf();


							// Drawing

	virtual	void		DrawSelf();


protected:

	Boolean			mPushed;				//	Flag used to handle hiliting of the button
											//		during mouse tracking
	RGBColor		mBackColor;				//	This is the color used by the controller to
											//		draw the background of the area below the
											//		controller, we have it here as well as it
											// 	needs to do the same rendering as the
											//		controller does.
	RGBColor		mFaceColor;				//	We are adding support for alternative face
											//		colors, but the default will always be a
											//		gray9 color
	RGBColor		mPushedTextColor;		//	This is the color that will be used to draw the
											//		button's title when it is pushed


						// Drawing

	virtual	void		DrawPageButtonTitle();


						// Methods for drawing various states at different bit depths

	virtual	void		DrawPageButtonNormalBW();
	
	virtual	void		DrawPageButtonPushedBW();
	
	virtual	void		DrawPageButtonDimmedBW();
	
	virtual	void		DrawPageButtonNormalColor();
	
	virtual	void		DrawPageButtonDimmedColor();
	
	virtual	void		DrawPageButtonPushedColor();


						// Mouse Tracking

	virtual	void		HotSpotAction(
								SInt16		inHotSpot,
								Boolean		inCurrInside,
								Boolean		inPrevInside);

	virtual	void		DoneTracking(
								SInt16		inHotSpot,
								Boolean		inGoodClick);

	virtual	void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
