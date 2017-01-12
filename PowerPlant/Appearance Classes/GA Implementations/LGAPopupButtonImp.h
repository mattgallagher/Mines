// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPopupButtonImp.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAPopupButtonImp
#define _H_LGAPopupButtonImp
#pragma once

#include <LGATitleMixin.h>
#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	CLASS: LGAPopupButtonImp
// ===========================================================================

class LGAPopupButtonImp : 	public LGAControlImp,
							public LGATitleMixin {
public:
						LGAPopupButtonImp (
								LControlPane*	inControlPane );

						LGAPopupButtonImp (
								LStream*		inStream );

	virtual				~LGAPopupButtonImp ();

	virtual void		Init (	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init (	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0 );


							// Getters

	virtual StringPtr	GetDescriptor(
								Str255			outDescriptor ) const;

	MenuHandle			GetMacMenuH	() const;

	SInt16				GetLabelWidth () const	{ return mLabelWidth; }

	SInt16				GetLabelStyle () const	{ return mLabelStyle; }

	SInt16				GetLabelJust () const	{ return mLabelJust; }

	Boolean				IsPulldownMenu () const	{ return mPulldown; }

	Boolean				IsArrowOnly	() const	{ return mArrowOnly; }

	virtual	Boolean		HasLabel	() const;
	virtual	SInt16		GetMenuFontSize () const;

	virtual	void		GetCurrentItemTitle (
								Str255			outItemTitle );

	virtual	void		CalcLocalPopupFrameRect	(
								Rect			&outRect ) const;
	virtual	void		CalcTitleRect (
								Rect			&outRect );
	virtual	void		CalcLabelRect (
								Rect			&outRect ) const;
	virtual	void		GetPopupMenuPosition	(
								Point			&outPopupLoc );

	virtual	void		GetAllValues (
								SInt32			&outValue,
								SInt32			&outMinValue,
								SInt32			&outMaxValue );

	virtual	ResIDT		GetTextTraitsID () const
							{
								return LGATitleMixin::GetTextTraitsID();
							}


						// Setters

	virtual	void		SetValue(
								SInt32			inValue );
	virtual	void		PostSetValue ();
	virtual void		SetDescriptor (
								ConstStringPtr	inDescriptor );

	virtual void		SetTextTraitsID (
								ResIDT			inTextTraitsID );

	virtual	void		SetLabelWidth	(
								SInt16			inLabelWidth );
	virtual	void		SetLabelStyle	(
								SInt16			inLabelStyle );
	virtual	void		SetLabelJust	(
								SInt16			inLabelJust );
	virtual	void		SetupCurrentMenuItem	(
								MenuHandle		inMenuH,
								SInt16			inCurrentItem );
	virtual	SInt16		FindCurrMarkedMenuItem	(
								MenuHandle		inMenuH );


							// Best Rect

	virtual	Boolean			SupportsCalcBestRect	() const;

	virtual	void			CalcBestControlRect	(
									SInt16		&outWidth,
									SInt16		&outHeight,
									SInt16		&outBaselineOffset ) const;


							// Drawing

	virtual	void		DrawSelf	();
	virtual	void		DrawPopupNormal ();
	virtual	void		DrawPopupHilited ();
	virtual	void		DrawPopupDimmed ();
	virtual	void		DrawPopupTitle	();
	virtual	void		DrawPopupLabel	();
	virtual	void		DrawPopupArrow	();

protected:

	SInt16			mLabelWidth;			//	Width to be used when rendering the label
	SInt16			mLabelStyle;			//	Style to be used for the label
	SInt16			mLabelJust;				//	Justification for the label
	Boolean			mFixedWidth;			//	Does the user want the popup to always be
											//		the specified size or should it be made
											//		to fit the menu width
	Boolean			mUseWindowFont;			//	We don't really use this feature but for now
											//		we will store the flag anyway
	Boolean			mPulldown;				//	Is the menu a pulldown or normal popup
	Boolean			mArrowOnly;				//	Do we want only the popup arrow to be
											//		rendered, this will result in only
											//		a 3D framed arrow to be drawn to the
											//		right of the popup's current item

						// Drawing
													// Methods for drawing various states at
													// different bit depths

	virtual	void		DrawPopupNormalBW	();
	virtual	void		DrawPopupHilitedBW ();
	virtual	void		DrawPopupDimmedBW	();
	virtual	void		DrawPopupNormalColor	();
	virtual	void		DrawPopupDimmedColor	();
	virtual	void		DrawPopupHilitedColor ();


						// Mouse Tracking

	virtual	Boolean		PointInHotSpot (
												Point 	inPoint,
												SInt16		inHotSpot ) const;

	virtual	Boolean		TrackHotSpot (
												SInt16		inHotSpot,
												Point 	inPoint,
												SInt16		inModifiers );

	virtual 	void			HotSpotAction	(
												SInt16 inHotSpot,
												Boolean inCurrInside,
												Boolean inPrevInside );

	virtual 	void			DoneTracking (
												SInt16		inHotSpot,
												Boolean	inGoodTrack );



						// Popup Menu Handling

	virtual	void			HandlePopupMenuSelect (
												Point		inPopupLoc,
												SInt16		inCurrentItem,
												SInt16		&outMenuID,
												SInt16		&outMenuItem );


						// Miscellaneous

	virtual void			FinishCreateSelf();
	virtual	bool			AdjustPopupWidth	();
	virtual	SInt16			CalcMaxPopupItemWidth () const;
	virtual	SInt16			CalcPopupLabelWidth ();
	virtual	void			ResizeForArrowOnly ();
	virtual	void			CalcPopupMinMaxValues (
												MenuHandle	inMenuH,
												SInt32	&outMinValue,
												SInt32	&outMaxValue );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
