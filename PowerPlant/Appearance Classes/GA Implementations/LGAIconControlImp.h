// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAIconControlImp.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAIconControlImp
#define _H_LGAIconControlImp
#pragma once

#include <LGAIconSuiteMixin.h>
#include <LGACIconMixin.h>
#include <UGraphicUtils.h>
#include <LGAControlImp.h>
#include <LControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAIconControlImp : public LGAControlImp,
						  public LGAIconSuiteMixin,
						  public LGACIconMixin {
public:
						LGAIconControlImp(
								LControlPane*	inControlPane);

						LGAIconControlImp(
								LStream*		inStream);

	virtual				~LGAIconControlImp();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0 );


							// Getters

	bool				IsSelected	() const
								{
									return mControlPane->GetValue() > 0;
								}

	bool				HasIconSuite ()
								{
									return mIconType == FOUR_CHAR_CODE('iclx');
								}

	bool				HasCIcon()
								{
									return mIconType == FOUR_CHAR_CODE('cicn');
								}

	bool				WantsTracking()
								{
									return mWantsTracking;
								}

	virtual	void		CalcLocalIconRect(
								Rect&		outRect);


							// Setters

	virtual	void		SetResourceID(
								ResIDT		inResID);

	virtual	void		SetPushedState(
								Boolean		inPushedState);


							// Enabling & Disabling

	virtual void		EnableSelf();
	virtual void		DisableSelf();


							// Activation

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();


							// Data Handling

	virtual	void		SetDataTag (
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr );

	virtual	void		GetDataTag (
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;


							// Drawing

	virtual	void		DrawSelf();

protected:
	SInt32			mIconType;			//	Content type the button will be displaying
										//		this will either be 'iclx' or 'cicn'
	SInt16			mIconAlignment;		//	How is the icon positioned
	ResIDT			mResourceID;		//	The resource ID for the icon or icon suite
	bool			mWantsTracking;		//	Do we want to track the icon when the user
										//		clicks in it


							// Drawing

	virtual	void		DrawIconSuiteGraphic();
	virtual	void		DrawCIconGraphic();

							// Mouse Tracking

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual void		DoneTracking(
								SInt16			inHotSpot,
								Boolean			inGoodTrack);


							// Miscellaneous

	virtual	void		CalcIconLocation(
								Point&			outIconLocation);

	virtual	void		CalcIconSuiteSize();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
