// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlPane.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LControlPane
#define _H_LControlPane
#pragma once

#include <LControl.h>
#include <Controls.h>
#include <ControlDefinitions.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LControlImp;

// ---------------------------------------------------------------------------

class	LControlPane : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('cpan') };

						LControlPane(
								LStream*			inStream,
								ClassIDT			inImpID);

						LControlPane( LStream* inStream );

						LControlPane(
								const SPaneInfo&	inPaneInfo,
								ClassIDT			inImpID,
								SInt16				inControlKind,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								MessageT			inValueMessage = 0,
								SInt32				inValue = 0,
								SInt32				inMinValue = 0,
								SInt32				inMaxValue = 0,
								SInt32				inRefCon = 0);

						LControlPane(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue);

	virtual				~LControlPane();

	void				MakeControlImp(
								ClassIDT			inImpID,
								LStream*			inStream);

	void				MakeControlImp(
								ClassIDT			inImpID,
								SInt16				inControlKind,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								SInt32				inRefCon = 0);

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		MoveBy(	SInt32				inHorizDelta,
								SInt32				inVertDelta,
								Boolean				inRefresh);

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif

	SInt32				PeekValue()			{ return mValue; }

	void				PokeValue( SInt32 inValue )
							{
								mValue = inValue;
							}

	virtual SInt32		GetValue() const;

	virtual void		SetValue( SInt32 inValue );

	virtual void		SetMinValue( SInt32 inMinValue );

	void				PokeMinValue( SInt32 inMinValue )
							{
								mMinValue = inMinValue;
							}

	virtual void		SetMaxValue( SInt32 inMaxValue );

	virtual Boolean		FocusDraw( LPane* inSubPane = nil );

	void				GetAllValues(
								SInt32&				outValue,
								SInt32&				outMinValue,
								SInt32&				outMaxValue);

	virtual void		SetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inDataSize,
								void*				inDataPtr);

	virtual void		GetDataTag(
								SInt16				inPartCode,
								FourCharCode		inTag,
								Size				inBufferSize,
								void*				inBuffer,
								Size*				outDataSize = nil) const;

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );

	virtual ResIDT		GetTextTraitsID() const;

	virtual void		SetFontStyle( ControlFontStyleRec& inStyleRec );

	virtual void		GetFontStyle( ControlFontStyleRec& outStyleRec );
	
	void				ImpStructureChanged();
	
	bool				CalcPortStructureRect( Rect& outRect ) const;
	
	virtual Boolean		CalcPortExposedRect(
								Rect&		outRect,
								bool		inOKIfHidden = false) const;

	Boolean				SupportsCalcBestRect() const;

	void				CalcBestControlRect(
								SInt16&				outWidth,
								SInt16&				outHeight,
								SInt16&				outBaselineOffset) const;

	virtual SInt16		FindHotSpot( Point inPoint ) const;

	virtual Boolean		PointInHotSpot(
								Point				inPoint,
								SInt16				inHotSpot) const;

	virtual Boolean		TrackHotSpot(
								SInt16				inHotSpot,
								Point				inPoint,
								SInt16				inModifiers);

	virtual void		HotSpotAction(
								SInt16				inHotSpot,
								Boolean				inCurrInside,
								Boolean				inPrevInside);

	virtual void		DoneTracking(
								SInt16				inHotSpot,
								Boolean				inGoodTrack);

	virtual void		DoTrackAction(
								SInt16				inHotSpot,
								SInt32				inValue);

	LControlImp*		GetControlImp() const	{ return mControlImp; }
	
	virtual LPane*		GetContainer() const;

	virtual void		PortToLocalPoint( Point &ioPoint ) const;
	
	virtual void		LocalToPortPoint( Point &ioPoint) const;

protected:
	virtual void		FinishCreateSelf();

	virtual void		DrawSelf();
	virtual void		ShowSelf();
	virtual void		HideSelf();
	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();
	virtual void		EnableSelf();
	virtual void		DisableSelf();

protected:
	LControlImp*		mControlImp;
	Rect				mStructureOffsets;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
