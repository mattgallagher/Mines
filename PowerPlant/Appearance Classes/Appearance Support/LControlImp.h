// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlImp.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LControlImp
#define _H_LControlImp
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LControlImp {
public:
						LControlImp( LControlPane* inControlPane );

						LControlImp( LStream* inStream );

	virtual				~LControlImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual void		FinishCreateSelf();

	virtual void		ResizeFrameBy(
								SInt16			inWidthDelta,
								SInt16			inHeightDelta);

	virtual void		MoveBy(	SInt32			inHorizDelta,
								SInt32			inVertDelta);

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
								
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	virtual bool		GetValue( SInt32& outValue ) const;

	virtual void		SetValue( SInt32 inValue );

	virtual void		PostSetValue();

	virtual void		SetMinValue( SInt32 inMinValue );
	
	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual void		GetAllValues(
								SInt32&			outValue,
								SInt32&			outMinValue,
								SInt32&			outMaxValue);

	virtual ControlHandle	GetMacControl() const;

	virtual void		SetScrollViewSize( SInt32 inViewSize );
	
	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;
	
	virtual void		FocusImp();

	virtual SInt16		FindHotSpot( Point inPoint ) const;
	
	virtual Boolean		PointInHotSpot(
								Point			inPoint,
								SInt16			inHotSpot) const;
								
	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);
								
	virtual void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);
								
	virtual void		DoneTracking(
								SInt16			inHotSpot,
								Boolean			inGoodTrack);

	Boolean				CalcLocalFrameRect( Rect& outRect ) const
						{
							return mControlPane->CalcLocalFrameRect(outRect);
						}

	void				Refresh()		{ mControlPane->Refresh(); }

	virtual void		DrawSelf();

	Boolean				IsVisible() const	{ return mControlPane->IsVisible(); }
	virtual void		ShowSelf();
	virtual void		HideSelf();

	Boolean				IsActive() const	{ return mControlPane->IsActive(); }
	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	Boolean				IsEnabled() const	{ return mControlPane->IsEnabled(); }
	virtual void		EnableSelf();
	virtual void		DisableSelf();

	virtual void		Hilite(	SInt16			inPartCode);

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;

	virtual void		SetTextTraitsID(
								ResIDT			inTextTraitsID);

	virtual ResIDT		GetTextTraitsID() const;

	virtual void		SetFontStyle( const ControlFontStyleRec& inStyleRec );

	virtual void		GetFontStyle( ControlFontStyleRec& outStyleRec ) const;

	virtual bool		ApplyForeAndBackColors() const;

	virtual bool		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;

	virtual Boolean		SupportsCalcBestRect() const;

	virtual void		CalcBestControlRect(
								SInt16&			outWidth,
								SInt16&			outHeight,
								SInt16&			outBaselineOffset) const;

	virtual void		Idle();
	
	virtual bool		IdlesAutomatically() const;

	virtual SInt16		DoKeyPress( const EventRecord& inKeyEvent );

	virtual void		BeTarget( ControlFocusPart inFocusPart );

	virtual void		DontBeTarget();

	virtual bool		SetKeyboardFocusPart( ControlFocusPart inFocusPart );
	

protected:
	LControlPane*	mControlPane;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
