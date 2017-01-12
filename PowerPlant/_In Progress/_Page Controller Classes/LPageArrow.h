// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageArrow.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LPageArrow
#define _H_LPageArrow
#pragma once

#include <LControl.h>
#include <LPageButton.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LPageArrow : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('lpga') };

						LPageArrow();

						LPageArrow( const LPageArrow& inOriginal );

						LPageArrow(
								const SPaneInfo& 		inPaneInfo,
								const SControlInfo& 	inControlInfo,
								bool					inFacesLeft,
								const RGBColor&			inBackColor = pageButton_DefaultBackColor,
								const RGBColor&			inFaceColor = pageButton_DefaultFaceColor);

	virtual				~LPageArrow();

	bool				IsSelected() const		{ return GetValue() > 0; }

	bool				IsPushed() const		{ return mPushed; }

	bool				FacesLeft() const		{ return mFacesLeft; }

	virtual	void		CalcLocalArrowRect( Rect& outRect ) const;

	virtual	void		SetPushedState( Boolean inHiliteState );

	void				SetArrowBackColor( const RGBColor& inBackColor )
							{
								mBackColor = inBackColor;
							}

	void				SetArrowFaceColor( const RGBColor& inFaceColor )
							{
								mFaceColor = inFaceColor;
							}

	virtual	void		EnableSelf();
	
	virtual	void		DisableSelf();

	virtual	void		ActivateSelf();
	
	virtual	void		DeactivateSelf();

	virtual	void		DrawSelf();

protected:
	bool			mPushed;			// Pushed flag
	bool			mFacesLeft;			// true = left pointing; false = right pointing
	RGBColor		mBackColor;			// Background color for the control, this is
										//		the background as used by the controller
	RGBColor		mFaceColor;			// Face color for the control, this is allows the
										//		controls to each have different colors if
										//		desired by the user

	virtual	void		DrawPageArrowNormal();
	
	virtual	void		DrawPageArrowPushed();
	
	virtual	void		DrawPageArrowDimmed();

	virtual	void		DrawPageArrowNormalBW();

	virtual	void		DrawPageArrowNormalColor();
	
	virtual	void		DrawPageArrowDimmedColor();


	virtual	Boolean		PointIsInFrame(
								SInt32			inHorizPort,
								SInt32			inVertPort) const;
																									// Override
	virtual void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);
																									// Override
	virtual void		HotSpotResult( SInt16 inHotSpot );
																									// Override
private:
	virtual	void		DrawArrowChoice(
								SInt16			inChoice,
								const Rect&		inFrame);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
