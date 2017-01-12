// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlView.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LControlView
#define _H_LControlView
#pragma once

#include <LView.h>
#include <LBroadcaster.h>
#include <LControlPane.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#if PP_Uses_Carbon_Events
	#include	<TEventHandler.h>
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LControlSubPane : public LControlPane {
public:
						LControlSubPane(
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

						LControlSubPane(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue);

	virtual				~LControlSubPane();
	
	virtual LPane*		GetContainer() const;

	virtual void		HotSpotResult( SInt16 inHotSpot );

	virtual void		Draw( RgnHandle inSuperDrawRgnH );

	virtual void		Refresh();

	virtual void		Click( SMouseDownEvent& inMouseDown );

	virtual void		AdjustMouse(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
};

// ---------------------------------------------------------------------------

class	LControlView : public LView,
					   public LBroadcaster,
					   public LListener {
public:
						LControlView(
								LStream*			inStream,
								ClassIDT			inImpID);

						LControlView(
								LStream*			inStream);

						LControlView(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								ClassIDT			inImpID,
								SInt16				inControlKind,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								MessageT			inValueMessage = 0,
								SInt32				inValue = 0,
								SInt32				inMinValue = 0,
								SInt32				inMaxValue = 0);

	virtual				~LControlView();

	virtual void		CalcRevealedRect();
	
	bool				CalcPortStructureRect( Rect& outRect ) const;
	
	virtual Boolean		CalcPortExposedRect(
								Rect&		outRect,
								bool		inOKIfHidden = false) const;

	MessageT			GetValueMessage() const
							{
								return mControlSubPane->GetValueMessage();
							}

	void				SetValueMessage( MessageT inValueMessage )
							{
								mControlSubPane->SetValueMessage(inValueMessage);
							}

	virtual void		IncrementValue( SInt32 inIncrement )
							{
								mControlSubPane->IncrementValue(inIncrement);
							}

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
	
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual SInt32		GetValue() const;
	
	virtual void		SetValue( SInt32 inValue );

	void				PokeValue( SInt32 inValue )
							{
								mControlSubPane->PokeValue(inValue);
							}

	SInt32				GetMinValue() const
							{
								return mControlSubPane->GetMinValue();
							}

	void				SetMinValue( SInt32 inMinValue )
							{
								mControlSubPane->SetMinValue(inMinValue);
							}

	SInt32				GetMaxValue() const
							{
								return mControlSubPane->GetMaxValue();
							}

	void				SetMaxValue( SInt32 inMaxValue )
							{
								mControlSubPane->SetMaxValue(inMaxValue);
							}

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

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

	virtual void		ApplyForeAndBackColors() const;

	virtual void		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;

	LControlImp*		GetControlImp() const
							{
								return mControlSubPane->GetControlImp();
							}
							
	LControlSubPane*	GetControlSubPane() const
							{
								return mControlSubPane;
							}

	virtual void		HotSpotResult( SInt16 inHotSpot );

protected:
	virtual void		FinishCreateSelf();

	void				MakeControlSubPane(
								ClassIDT			inImpID,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue,
								LStream*			inStream);

	void				MakeControlSubPane(
								ClassIDT			inImpID,
								SInt16				inControlKind,
								ConstStringPtr		inTitle,
								ResIDT				inTextTraitsID,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue);
	
protected:
	LControlSubPane*	mControlSubPane;

#if PP_Uses_Carbon_Events
	TEventHandler<LControlView>	mDrawEvent;
	
	OSStatus			DoDrawEvent (
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef );
#endif
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
