// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBevelButton.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LBevelButton
#define _H_LBevelButton
#pragma once

#include <LControlPane.h>
#include <LMenuController.h>

#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LBevelButton : public LControlPane,
					   public LMenuController {
public:
	enum { class_ID		= FOUR_CHAR_CODE('bbut'),
		   imp_class_ID	= FOUR_CHAR_CODE('ibbt') };

						LBevelButton(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LBevelButton(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt16			inBevelProc,
								SInt16			inBehavior,
								SInt16			inContentType,
								SInt16			inContentResID,
								ResIDT			inTextTraits,
								ConstStringPtr	inTitle,
								SInt16			inInitialValue,
								SInt16			inTitlePlacement,
								SInt16			inTitleAlignment,
								SInt16			inTitleOffset,
								SInt16			inGraphicAlignment,
								Point			inGraphicOffset,
								ClassIDT		inImpID = imp_class_ID);

						LBevelButton(
								const SPaneInfo	&inPaneInfo,
								MessageT		inValueMessage,
								SInt16			inBevelProc,
								ResIDT			inMenuID,
								SInt16			inMenuPlacement,
								SInt16			inContentType,
								SInt16			inContentResID,
								ResIDT			inTextTraits,
								ConstStringPtr	inTitle,
								SInt16			inInitialValue,
								SInt16			inTitlePlacement,
								SInt16			inTitleAlignment,
								SInt16			inTitleOffset,
								SInt16			inGraphicAlignment,
								Point			inGraphicOffset,
								Boolean			inCenterPopupGlyph,
								ClassIDT		inImpID = imp_class_ID);

	virtual				~LBevelButton();

	virtual void		SetValue( SInt32 inValue );

	void				SetContentInfo( ControlButtonContentInfo& inInfo );
	
	void				GetContentInfo( ControlButtonContentInfo& outInfo ) const;

	void				SetTransform( IconTransformType inTransform );
	
	IconTransformType	GetTransform() const;

	void				SetTextAlignment( ControlButtonTextAlignment inAlignment );
	
	ControlButtonTextAlignment	GetTextAlignment() const;

	void				SetTextOffset( SInt16 inOffset );
	
	SInt16				GetTextOffset() const;

	void				SetGraphicAlignment( ControlButtonGraphicAlignment inAlignment );
	
	ControlButtonGraphicAlignment	GetGraphicAlignment() const;

	void				SetGraphicOffset( Point inOffset );
	
	void				GetGraphicOffset( Point& outOffset ) const;

	void				SetTextPlacement( ControlButtonTextPlacement inPlacement );
	
	ControlButtonTextPlacement	GetTextPlacement() const;

	virtual void		SetCurrentMenuItem( SInt16 inItem );

	virtual SInt16		GetCurrentMenuItem() const;

	virtual void		SetMenuMinMax();

	void				SetCenterPopupGlyph( Boolean inCenter );
	
	Boolean				GetCenterPopupGlyph() const;

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

protected:
	SInt16			mBehavior;		// Push, Toggle, Sticky, or Popup
	SInt16			mMenuChoice;

	virtual void		HotSpotResult( SInt16 inHotSpot );

private:
	void				InitBevelButton(
								SInt16		inInitialValue,
								SInt16		inTitlePlacement,
								SInt16		inTitleAlignment,
								SInt16		inTitleOffset,
								SInt16		inGraphicAlignment,
								Point		inGraphicOffset,
								Boolean		inCenterPopupGlyph);
};


const SInt16	kControlBehaviorPopup = -1;


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
