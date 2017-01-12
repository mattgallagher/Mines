// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCicnButton.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A button that draws itself using 'cicn' resources.

#ifndef _H_LCicnButton
#define _H_LCicnButton
#pragma once

#include <LControl.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCicnButton : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('cicn') };

						LCicnButton();

						LCicnButton( const LCicnButton& inOriginal );

						LCicnButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inClickedMessage,
								ResIDT				inNormalID,
								ResIDT				inPushedID);

						LCicnButton( LStream* inStream );

	virtual				~LCicnButton();

	virtual void		SetCicns(
								ResIDT				inNormalID,
								ResIDT				inPushedID);

protected:
	ResIDT			mNormalID;
	ResIDT			mPushedID;
	CIconHandle		mNormalCicnH;
	CIconHandle		mPushedCicnH;

	virtual void		DrawSelf();

	virtual SInt16		FindHotSpot( Point inPoint ) const;

	virtual Boolean		PointInHotSpot(
								Point				inPoint,
								SInt16				inHotSpot) const;

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
