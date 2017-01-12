// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdControlImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStdControlImp
#define _H_LStdControlImp
#pragma once

#include <LControlImp.h>
#include <Controls.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStdControlImp : public LControlImp {
public:
											// ¥ Initializers
						LStdControlImp( LStream* inStream = nil );

	virtual				~LStdControlImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

											// ¥ Accessors & Manipuators
	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	virtual void		SetValue( SInt32 inValue );

	virtual void		SetMinValue( SInt32 inMinValue );

	virtual void		SetMaxValue( SInt32	 inMaxValue );

	virtual void		GetAllValues(
								SInt32&			outValue,
								SInt32&			outMinValue,
								SInt32&			outMaxValue);

	virtual ControlHandle	GetMacControl() const;

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );

	virtual ResIDT		GetTextTraitsID() const;

	void				SetActionProc( ControlActionUPP inActionProc )
							{
								::SetControlAction(mMacControlH, inActionProc);
							}

											// ¥ Mouse Tracking
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

											// ¥ Drawing
	virtual void		FocusImp();
	virtual void		DrawSelf();

											// ¥ State Changes
	virtual void		ShowSelf();
	virtual void		HideSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		EnableSelf();
	virtual void		DisableSelf();

	virtual void		Hilite( SInt16 inPartCode );

	virtual void		ResizeFrameBy(
								SInt16			inWidthDelta,
								SInt16			inHeightDelta);

	virtual void		MoveBy(	SInt32			inHorizDelta,
								SInt32			inVertDelta);

protected:
	ControlHandle		mMacControlH;
	SInt16				mControlKind;
	ResIDT				mTextTraitsID;

	virtual void		MakeMacControl(
								ConstStringPtr	inTitle,
								SInt32			inRefCon);
								
	void				ApplyTextTraits();

	virtual void		AlignControlRect();

	virtual void		AdjustControlBounds( Rect& ioBounds );

	static Boolean		ValueIsInStdRange( SInt32 inValue );

	SInt16				CalcSmallValue(
								SInt32			inBigValue,
								SInt32			inBigMinimum,
								SInt32			inBigMaximum);

	SInt32				CalcBigValue( SInt16 inSmallValue );

	bool				ApplySmallSettings(
								SInt32			inValue,
								SInt32			inMinValue,
								SInt32			inMaxValue);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
