// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPushButtonImp.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAPushButtonImp
#define _H_LGAPushButtonImp
#pragma once

#include <LGATitleMixin.h>
#include <LGACIconMixin.h>
#include <LGAControlImp.h>
#include <LControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAPushButtonImp : public LGAControlImp,
						 public LGATitleMixin,
						 public LGACIconMixin {
public:
						LGAPushButtonImp(
								LControlPane*	inControlPane);

						LGAPushButtonImp(
								LStream*		inStream);

	virtual				~LGAPushButtonImp();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0 );


							// Getters

	virtual StringPtr	GetDescriptor (
								Str255	outDescriptor ) const;

	virtual	void		CalcTitleRect (
								Rect	&outRect );

	virtual	void		CalcLocalCIconRect (
												Rect	&outRect );

	bool				IsDefaultButton () const
									{
										return mDefault;
									}

	bool				HasIcon	() const
									{
										return mHasIcon;
									}

	bool				HasIconOnLeft () const
									{
										return mIconOnLeft;
									}

	virtual	ResIDT		GetTextTraitsID () const
									{
										return LGATitleMixin::GetTextTraitsID ();
									}


							// Setters

	virtual void		SetDescriptor (
												ConstStringPtr	inDescriptor );

	virtual void		SetTextTraitsID (
												ResIDT	inTextTraitsID );

	virtual	void		SetDefaultButton (
												Boolean inDefault,
												Boolean	inRedraw = true );

	virtual	void		SetPushedState	(
												Boolean	inPushedState );

	virtual	void		SetValue	(
												SInt32	inValue );


							// Enabling

	virtual	void		EnableSelf ();
	virtual	void		DisableSelf	();


							//	Activation

	virtual void		ActivateSelf ();
	virtual void		DeactivateSelf ();


							//	Drawing

	virtual	void		DrawSelf ();

	virtual	void		DrawPushButton(
								SInt16	inDepth);
	virtual	void		DrawButtonTitle (
								SInt16	inDepth);
	virtual	void		DrawPushButtonIcon (
								SInt16	inDepth);
	virtual	void		DrawDefaultOutline (
								SInt16	inDepth);


							//	Data Handling

	virtual	void		SetDataTag (
												SInt16				inPartCode,
												FourCharCode	inTag,
												Size				inDataSize,
												void*				inDataPtr );

	virtual	void		GetDataTag (
												SInt16				inPartCode,
												FourCharCode	inTag,
												Size				inBufferSize,
												void*				inBuffer,
												Size				*outDataSize = nil ) const;

protected:

	bool			mDefault;		//	This flag is used to indicate whether the control is
									//		the default which means that it would then possibly
									//		do additional drawing of some form of outline
	bool			mHasIcon;		//	Are we displaying an icon in this push button
	bool			mIconOnLeft;	//	Is the icon on the left or the right

private:
	SInt16			mTransform;		//	Used to draw the icon if present in its various states


protected:

							// Methods for drawing various states at different bit depths

	virtual	void		DrawButtonNormalBW ();
	virtual	void		DrawButtonHilitedBW ();
	virtual	void		DrawButtonDimmedBW ();
	virtual	void		DrawButtonNormalColor ();
	virtual	void		DrawButtonDimmedColor ();
	virtual	void		DrawButtonHilitedColor ();


							//	MouseTracking

	virtual	Boolean		PointInHotSpot (
												Point 	inPoint,
												SInt16		inHotSpot ) const;

	virtual	void		DoneTracking (
												SInt16		inHotSpot,
												Boolean	inGoodTrack );


							// Miscellaneous

	virtual void		CalcCIconLocation	(
												Point	&outIconLocation );

	virtual	void		ClipToPushButtonFace	();

	virtual	SInt16		CalcTitleWidth	();


};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
