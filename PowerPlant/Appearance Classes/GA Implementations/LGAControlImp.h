// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAControlImp.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAControlImp
#define _H_LGAControlImp
#pragma once

#include <LControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAControlImp : public LControlImp {
public:
						LGAControlImp(
								LControlPane*	inControlPane);

						LGAControlImp(
								LStream*		inStream);

	virtual				~LGAControlImp();

	bool				IsPushed() const		{ return mPushed; }

	virtual	void		SetPushedState(
								Boolean			inPushedState);

	virtual	void		EnableSelf();
	virtual	void		DisableSelf();

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual	Boolean		PointInHotSpot(
								Point 			inPoint,
								SInt16			inHotSpot) const;

	virtual	Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual	void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);

protected:
	bool		mPushed;		// Is mouse down inside control?
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
