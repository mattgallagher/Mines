// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMControlImp.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMControlImp
#define _H_LAMControlImp
#pragma once

#include <LStdControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#if PP_Uses_Carbon_Events
	#include	<TEventHandler.h>
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMControlImp : public LStdControlImp {
public:
						LAMControlImp( LStream* inStream = nil );

	virtual				~LAMControlImp();

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual void		FocusImp();
	virtual void		DrawSelf();

	virtual void		ShowSelf();
	virtual void		HideSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		EnableSelf();
	virtual void		DisableSelf();

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

	virtual void		SetTextTraitsID( ResIDT inTextTraitsID );

	virtual void		SetFontStyle( const ControlFontStyleRec& inStyleRec );

	virtual void		GetFontStyle( ControlFontStyleRec& outStyleRec ) const;

	virtual void		SetScrollViewSize( SInt32 inViewSize );
	
	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;

	virtual Boolean		SupportsCalcBestRect() const;

	virtual void		CalcBestControlRect(
								SInt16&			outWidth,
								SInt16&			outHeight,
								SInt16&			outBaselineOffset) const;

	virtual	void		Idle();
	
	virtual bool		IdlesAutomatically() const;

	virtual SInt16		DoKeyPress( const EventRecord& inKeyEvent );

	virtual void		BeTarget( ControlFocusPart inFocusPart );
								
	virtual void		DontBeTarget();

	virtual bool		SetKeyboardFocusPart( ControlFocusPart inFocusPart );
	
	virtual bool		ApplyForeAndBackColors() const;

	virtual void		ApplyTextColor(
								SInt16			inDepth,
								bool			inHasColor,
								bool			inIsActive);

protected:

	virtual void		MakeMacControl(
								ConstStringPtr	inTitle,
								SInt32			inRefCon);

	virtual void		RefreshControlPane();

#if PP_Uses_Carbon_Events
	virtual void		FinishCreateSelf();

	TEventHandler<LAMControlImp>	mDrawEvent;
	
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
