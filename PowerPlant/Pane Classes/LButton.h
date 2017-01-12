// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LButton.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LButton
#define _H_LButton
#pragma once

#include <LControl.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LButton : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('butn') };

						LButton();
						
						LButton( const LButton& inOriginal );
								
						LButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inClickedMessage,
								OSType				inGraphicsType,
								ResIDT				inNormalID,
								ResIDT				inPushedID);
								
						LButton( LStream* inStream );

	virtual void		SetGraphicsType( OSType inGraphicsType );
								
	virtual void		SetGraphics(
								ResIDT				inNormalID,
								ResIDT				inPushedID);

	virtual Boolean		PointIsInFrame(
								SInt32				inHoriz,
								SInt32				inVert) const;

protected:
	OSType		mGraphicsType;
	ResIDT		mNormalID;
	ResIDT		mPushedID;

	virtual void		DrawSelf();
	
	virtual void		DrawGraphic( ResIDT inGraphicID );

	virtual void		HotSpotAction(
								SInt16				inHotSpot,
								Boolean				inCurrInside,
								Boolean				inPrevInside);
								
	virtual void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
