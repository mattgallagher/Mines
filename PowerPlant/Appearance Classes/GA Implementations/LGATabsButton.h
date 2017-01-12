// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATabsButton.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGATabsButton
#define _H_LGATabsButton
#pragma once

#include <LGATitleMixin.h>
#include <LGAIconSuiteMixin.h>
#include <UGAColorRamp.h>
#include <LControl.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGATabsButton : public LControl,
					  public LGATitleMixin,
					  public LGAIconSuiteMixin {

public:
	enum { class_ID = FOUR_CHAR_CODE('gtab') };

						LGATabsButton();

						LGATabsButton(
								const LGATabsButton&	inOriginal);

						LGATabsButton(
								const SPaneInfo& 		inPaneInfo,
								const SControlInfo& 	inControlInfo,
								ConstStringPtr			inTitle,
								Boolean					inHiliteTitle = true,
								ResIDT					inTextTraitsID = 0,
								ResIDT					inIconSuiteID = 0,
								Boolean					inWantsLargeTab = false);

	virtual				~LGATabsButton();

	virtual StringPtr	GetDescriptor(
								Str255					outDescriptor) const;

	virtual	void		CalcTitleRect(
								Rect&					outRect);

	virtual	void		CalcLocalIconRect(
								Rect&					outRect);

	virtual	RgnHandle	CalcTabButtonRegion() const;
	virtual	RgnHandle	CalcSmallTabButtonRegion() const;
	virtual	RgnHandle	CalcLargeTabButtonRegion() const;

	bool				IsSelected() const		{ return GetValue() > 0; }

	bool				IsPushed() const		{ return mPushed; }

	bool				IsLeftMostButton();

	bool				WantsLargeTab() const	{ return mWantsLargeTab; }

	bool				HasIconSuite() const	{ return mIconSuiteH != nil; }


							// Setters

	virtual void		SetDescriptor(
								ConstStringPtr	inDescriptor);

	virtual	void		SetPushedState(
								Boolean	inPushedState);

	virtual	void		SetValue(
								SInt32	inValue);


							// Activation

	virtual	void		ActivateSelf();
	virtual	void		DeactivateSelf();


							// Enabling & Disabling

	virtual void		EnableSelf();
	virtual void		DisableSelf();


							// Drawing

	virtual	void		DrawSelf();

	virtual	void		DrawTabButtonTitle();

	virtual	void		DrawIconSuite();


protected:

	// ¥ Methods for drawing various states at different bit depths
	virtual	void		DrawTabButtonNormalBW();
	virtual	void		DrawTabButtonOnBW();
	virtual	void		DrawTabButtonHilitedBW();
	virtual	void		DrawTabButtonDimmedBW();
	virtual	void		DrawTabButtonNormalColor();
	virtual	void		DrawTabButtonOnColor();
	virtual	void		DrawTabButtonDimmedColor();
	virtual	void		DrawTabButtonHilitedColor();

	virtual	void		DrawSmallTabLeftSlope();
	virtual	void		DrawSmallTabRightSlope();
	virtual	void		DrawLargeTabLeftSlope();
	virtual	void		DrawLargeTabRightSlope();


							// Mouse Tracking

	virtual	Boolean		PointIsInFrame(
								SInt32		inHorizPort,
								SInt32		inVertPort) const;

	virtual	void		HotSpotAction(
								SInt16		inHotSpot,
								Boolean		inCurrInside,
								Boolean		inPrevInside);

	virtual	void		DoneTracking(
								SInt16 		inHotSpot,
								Boolean		inGoodTrack);

							// Miscellaneous

	virtual void		CalcIconLocation(
								Point&		outIconLocation);

	virtual	void		CalcIconSuiteSize();


private:
	bool		mPushed;			//	Flag used to handle hiliting of the button
									//		during mouse tracking
	bool		mWantsLargeTab;		//	True for large tab, false for small tab


};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
