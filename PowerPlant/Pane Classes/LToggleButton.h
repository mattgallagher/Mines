// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LToggleButton.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Button that can be On or Off and that has an optional transition states
//	for animating between the On/Off states. The graphics for the Button can
//	be either an icon family ('ICN#"), icon ('ICON'), or picture ('PICT').

#ifndef _H_LToggleButton
#define _H_LToggleButton
#pragma once

#include <LControl.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LToggleButton : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('tbut') };

						LToggleButton();

						LToggleButton( const LToggleButton& inOriginal );

						LToggleButton(
								const SPaneInfo		&inPaneInfo,
								MessageT			inClickedMessage,
								OSType				inGraphicsType,
								ResIDT				inOnID,
								ResIDT				inOnClickID,
								ResIDT				inOffID,
								ResIDT				inOffClickID,
								ResIDT				inTransitionID);
								
						LToggleButton( LStream* inStream );

	virtual				~LToggleButton();

	virtual void		SetGraphicsType( OSType inGraphicsType );
	
	virtual void		SetGraphics(
								ResIDT				inOnID,
								ResIDT				inOnClickID,
								ResIDT				inOffID,
								ResIDT				inOffClickID,
								ResIDT				inTransitionID);

	virtual void		SetValue( SInt32 inValue );
								
	virtual Boolean		PointIsInFrame(
								SInt32				inHoriz,
								SInt32				inVert) const;

protected:
	OSType			mGraphicsType;
	ResIDT			mOnID;
	ResIDT			mOnClickID;
	ResIDT			mOffID;
	ResIDT			mOffClickID;
	ResIDT			mTransitionID;

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
