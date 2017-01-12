// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAImageWellImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAImageWell
#define _H_LGAImageWell
#pragma once

#include <UGraphicUtils.h>
#include <LGAIconSuiteMixin.h>
#include <LGACIconMixin.h>
#include <LGAPictureMixin.h>
#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAImageWellImp : public LGAControlImp,
						public LGAIconSuiteMixin,
						public LGACIconMixin,
						public LGAPictureMixin {
public:
						LGAImageWellImp(
								LControlPane*	inControlPane);

						LGAImageWellImp(
								LStream*		inStream);

	virtual				~LGAImageWellImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

							//	Getters

	bool				IsSelected() const
							{
								return mControlPane->GetValue () == Button_On;
							}

	bool				HasAutoTracking() const
							{
								return mHasAutoTracking;
							}

	virtual	void		CalcLocalGraphicRect(
								Rect&		outRect);


							// Tests for different content types

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

	bool				HasPictRes	() const
							{
								return (mContentType == kControlContentPictRes);
							}

	bool				HasPictHandle () const
							{
								return (mContentType == kControlContentPictHandle);
							}

						//	Setters

	virtual	void		SetResourceID(
								ResIDT		inResID);

	virtual	void		AdjustTransform();


						// Colors

	virtual bool		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;


						// State Changes

	virtual void		EnableSelf();
	virtual void		DisableSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		ShowSelf();


						// Data Handling

	virtual	void		SetDataTag (
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);


	virtual	void		GetDataTag (
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;


						// Drawing

	virtual	void		DrawSelf();


protected:

	bool	mHasAutoTracking;	// Can the image well handling tracking of the mouse
								//		this flag is used when the user wants the well
								//		to behave like a radio button, where a click in
								// 	the well will result in the well becoming selected
								//		causing the border to be framed.  The selection
								//		is turned off by the application when a different
								//		well is selected.
	SInt16	mContentType;		//	Content type the image well will be displaying
	ResIDT	mResourceID;		//	Resource ID for the content that will be dispalyed in
								//		the image well


						// Drawing

	virtual	void		DrawGraphic();

	virtual	void		DrawIconSuiteGraphic();
	virtual	void		DrawCIconGraphic();
	virtual	void		DrawPictGraphic();

	virtual	void		RefreshGraphic();


						// Miscellaneous

	virtual	void		CalcGraphicLocation(
								Point&			outIconLocation);

	virtual void		CalcIconLocation(
								Point&			outIconLocation);

	virtual	void		CalcIconSuiteSize();

	virtual void		CalcPictureLocation(
								Point&			outPictureLocation);

						// Mouse Tracking

	virtual	void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
