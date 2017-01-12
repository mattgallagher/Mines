// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGABevelButtonImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGABevelButtonImp
#define _H_LGABevelButtonImp
#pragma once

#include <UGraphicUtils.h>
#include <LGATitleMixin.h>
#include <LGAIconSuiteMixin.h>
#include <LGACIconMixin.h>
#include <LGAPictureMixin.h>
#include <LGAControlImp.h>

#include <PP_Messages.h>
#include <LControlImp.h>

#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


#pragma options align=mac68k

typedef struct	SBevelButtonInfo {
	SInt16				bevelProcID;
	SInt16				menuID;
	SInt16				contentType;
	bool				offsetContents;
	bool				multiValueMenu;
	SInt16				behavior;
	SInt16				resourceID;
	bool				useWindowFont;
	SInt16				arrowRight;
	SInt16				bevelWidth;
	ConstStringPtr		title;
	ResIDT				textTraitID;
	SInt16				initialValue;
	SInt16				titlePlacement;
	SInt16				titleAlignment;
	SInt16				titleOffset;
	SInt16				graphicAlignment;
	Point				graphicOffset;
	Boolean				centerPopupGlyph;
} SBevelButtonInfo;

#pragma options align=reset

// ---------------------------------------------------------------------------

class LGABevelButtonImp : public LGAControlImp,
						  public LGATitleMixin,
						  public LGAIconSuiteMixin,
						  public LGACIconMixin,
						  public LGAPictureMixin {
public:
						LGABevelButtonImp(
								LControlPane*	inControlPane);

						LGABevelButtonImp (
								LStream*		inStream);

	virtual				~LGABevelButtonImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);


							// Getters

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	SInt16				GetBevelWidth() const
									{
										return mBevelWidth;
									}

	SInt16				GetControlBehavior() const
									{
										return mBehavior;
									}

	Boolean				IsSelected() const
									{
										return mControlPane->GetValue() > 0;
									}

	virtual	void		CalcTitleRect( Rect& outRect );

	virtual	void		CalcLocalGraphicRect( Rect& outRect );

	virtual	ResIDT		GetTextTraitsID() const
									{
										return LGATitleMixin::GetTextTraitsID();
									}


										// Popup Menus

	virtual	SInt16		GetMenuFontSize () const;

	virtual	MenuHandle	GetMacMenuH	() const;
	virtual SInt16		GetMenuValue() const;
	virtual void		SetMenuValue( SInt16 inItem );

	virtual	void		GetCurrentItemTitle( Str255 outItemTitle );

	virtual	void		GetPopupMenuPosition( Point& outPopupLoc );

	bool				HasPopupToRight() const
									{
										return 	mArrowRight;
									}

	bool				HasPopup() const
									{
										return	GetMacMenuH() != nil;
									}

	bool				HasMultiValueMenu	() const
									{
										return	mMultiValueMenu;
									}

	virtual	Boolean		WantsLargePopupIndicator () const;

	bool				WantsPopupGlyphCentered	() const
									{
										return	mCenterPopupGlyph;
									}


										// Tests for different content types

	bool				HasTextOnly	() const
									{
										return mContentType == kControlContentTextOnly;
									}
	bool				HasIconSuiteRes () const
									{
										return (mContentType == kControlContentIconSuiteRes);
									}
	bool				HasIconSuiteHandle () const
									{
										return (mContentType == kControlContentIconSuiteHandle);
									}
	bool				HasIconSuiteContent () const
									{
										return (HasIconSuiteRes () || HasIconSuiteHandle ());
									}
	bool				HasCIconRes	() const
									{
										return (mContentType == kControlContentCIconRes);
									}
	bool				HasCIconHandle	() const
									{
										return (mContentType == kControlContentCIconHandle);
									}
	bool				HasCIconContent () const
									{
										return (HasCIconRes () || HasCIconHandle ());
									}
	bool				HasPictRes () const
									{
										return (mContentType == kControlContentPictRes);
									}
	bool				HasPictHandle () const
									{
										return (mContentType == kControlContentPictHandle);
									}
	bool				HasPictContent() const
									{
										return (HasPictRes () || HasPictHandle ());
									}


						// Setters

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );


	virtual	void		SetResourceID( ResIDT inResID );

	virtual	void		SetBevelWidth(
								SInt16		inBevelWidth,
								Boolean		inRedraw = true );

	virtual	void		SetPushedState(
								Boolean		inPushedState );

	virtual	void		PostSetValue();

	virtual	void		SetupCurrentMenuItem(
								MenuHandle		inMenuH,
								SInt16			inCurrentItem);

	virtual	void		AdjustTitleHiliting(
								Boolean		inMouseInButton = false,
								Boolean 	inRedrawTitle = true);

	virtual	void		AdjustTransform(
								Boolean		inMouseInButton = false,
								Boolean 	inRedrawIcon = true);


						// Colors

	virtual bool		GetForeAndBackColors (
								RGBColor		*outForeColor,
								RGBColor		*outBackColor) const;

						// Enabling & Disabling

	virtual void		EnableSelf();
	virtual void		DisableSelf();


						// Activation

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

						// Visibility

	virtual void		ShowSelf();


						// Data Tag Handling

	virtual	void		SetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inDataSize,
								void*				inDataPtr);

	virtual	void		GetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inBufferSize,
								void*				inBuffer,
								Size				*outDataSize = nil) const;

						// Drawing

	virtual	void		DrawSelf();

	virtual	void		DrawGraphic( SInt16 inDepth );

	virtual	void		DrawBevelButtonNormal();
	virtual	void		DrawBevelButtonPushed();
	virtual	void		DrawBevelButtonDimmed();


protected:

	SInt16		mContentType;		//	Content type the button will be displaying
	bool		mOffsetContents;	//	Flag that tells us if we need to shift the
									//	contents of the button when it is pressed
	bool		mMultiValueMenu;	//	Does the menu support having multiple marked
									//	items within the menu
	SInt16		mBehavior;			//	Control mode used to determine how control
									//	responds to a mouse click in its hot spot
	SInt16		mResourceID;		//	Resource ID for the content type
	bool		mUseWindowFont;		//	We don't relly use this but we have it here for
									//	completeness
	bool		mHasPopup;			//	Does button have a popup
	bool		mArrowRight;		//	Display popup to the right of the button if
									//	true otherwise display it below
	bool		mCenterPopupGlyph;	//	This option can be set which will result
									//	in the popup glyph being drawn centered
									//	vertically in the control
	SInt16		mBevelWidth;		//	Width of the bevels drawn on the edge of the
									//	this is used to control how many shades are
									//	used to draw the edges of the button
									//	NOTE: this value does not account for the
									//	frame around the button, just the actual
									//	bevels
	SInt16		mTitlePlacement;	//	Where to position the title
	SInt16		mTitleAlignment;	//	How is the title aligned
	SInt16		mTitleOffset;		//	Amount the title is offset from a graphic
	SInt16		mGraphicAlignment;	//	How is the graphic positioned in button
	Point		mGraphicOffset;		//	Amount the graphic can be offset from the
									//	top and left edges of the button


	virtual void		FinishCreateSelf();

						// Drawing
											// Methods for drawing various states at different bit
											// depths

	virtual	void		DrawBevelButtonNormalBW();
	virtual	void		DrawBevelButtonPushedBW();
	virtual	void		DrawBevelButtonDimmedBW();

	virtual	void		DrawBevelButtonNormalColor();
	virtual	void		DrawBevelButtonPushedColor();
	virtual	void		DrawBevelButtonDimmedColor();

	virtual	void		DrawPopupGlyph();

	virtual	void		DrawButtonTitle( SInt16 inDepth );
	virtual	void		DrawIconSuiteGraphic();
	virtual	void		DrawCIconGraphic( SInt16 inDepth );
	virtual	void		DrawPictGraphic( SInt16 inDepth );

	virtual	void		WipeGraphic();
	virtual	void		WipeTitle();


						// Mouse Tracking

	virtual	Boolean		TrackHotSpot(
								SInt16		inHotSpot,
								Point 		inPoint,
								SInt16		inModifiers);

	virtual void		HotSpotAction(
								SInt16		inHotSpot,
								Boolean		inCurrInside,
								Boolean		inPrevInside);

	virtual void		DoneTracking(
								SInt16		inHotSpot,
								Boolean		inGoodTrack);

	virtual	void		HandlePopupMenuSelect(
								Point		inPopupLoc,
								SInt16		inCurrentItem,
								SInt16&		outMenuID,
								SInt16&		outMenuItem);

						// Miscellaneous

	virtual	void		CalcGraphicLocation( Point& outIconLocation );

	virtual void		CalcIconLocation( Point& outIconLocation );

	virtual	void		CalcIconSuiteSize();

	virtual void		CalcPictureLocation( Point& outPictureLocation );

	virtual	SInt16		CalcTitleWidth();
	virtual	SInt16		CalcTitleHeight();

	virtual	void		ClipToBevelButtonFace();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
