// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGALittleArrowsImp.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGALittleArrowsImp
#define _H_LGALittleArrowsImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------

class LGALittleArrowsImp :	public LGAControlImp {
public:
						LGALittleArrowsImp(
								LControlPane*	inControlPane);

						LGALittleArrowsImp(
								LStream*		inStream);

	virtual				~LGALittleArrowsImp ();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(
								LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual	void		CalcLocalUpArrowRect(
								Rect&			outRect) const;

	virtual	void		CalcLocalDownArrowRect(
								Rect&			outRect) const;

	virtual	void		CalcLocalLittleArrowRect(
								Rect&			outRect) const;

	virtual	void		DrawSelf();

protected:
	SInt32			mFirstDelay;

	virtual	void		DrawLittleArrowsNormal(
								Boolean			inUpArrow,
								SInt16			inDepth);

	virtual	void		DrawLittleArrowsHilited(
								Boolean			inUpArrow,
								SInt16			inDepth);

	virtual	void		DrawActualArrow(
								const Rect&		inRect,
								Boolean			inUpArrow);

	virtual SInt16		FindHotSpot(
								Point			inPoint) const;

	virtual Boolean		PointInHotSpot(
								Point			inPoint,
			 					SInt16			inHotSpot) const;

	virtual	Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual	void		HotSpotAction(
								SInt16			inHotSpot,
								Boolean			inCurrInside,
								Boolean			inPrevInside);

	virtual void		DoneTracking(
								SInt16			inHotSpot,
								Boolean			inGoodTrack);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
