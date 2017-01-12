// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGASliderImp.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LGASliderImp
#define _H_LGASliderImp
#pragma once

#include <LGAControlImp.h>
#include <UGWorld.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGASliderImp : public LGAControlImp {
public:
						LGASliderImp(
								LControlPane*	inControlPane);
						LGASliderImp(
								LStream*		inStream);

	virtual				~LGASliderImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

													// ¥ Drawing
	virtual	void		DrawSelf();

	virtual	void		DrawIndicatorTrack(
								SInt16			inDepth);

	virtual	void		DrawIndicator(
								SInt16			inDepth,
								const Rect&		inIndicatorRect,
								bool			inGhost = false,
								bool			inPressed = false);

	virtual	void		DrawTickMarks(
								SInt16			inDepth);

	virtual	void		DrawOneTickMark(
								SInt16			inDepth,
								const Rect&		inMarkRect,
								SInt16			inPosition);

	virtual	void		PlotPixMap (
								SInt16			inChoice,
								SInt16			inRowCount,
								SInt16			inColumnCount,
								const Rect&		inFrame);

													// ¥ Mouse Tracking
	virtual SInt16		FindHotSpot(
								Point			inPoint) const;

	virtual Boolean		PointInHotSpot(
								Point			inPoint,
								SInt16			inHotSpot) const;

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual	void		SetValue(
								SInt32			inValue);

protected:
	LGWorld*		mThumbWorld;
	SInt16			mNumTickMarks;
	SInt16			mLastPosition;
	bool			mHorizontal;
	bool			mHasLiveFeedback;
	bool			mNonDirectional;
	bool			mReverseDirection;

	virtual	void		CalcIndicatorRect(
								Rect&			outRect,
								SInt16			inPosition) const;

	virtual	void		CalcTrackRect(
								Rect&			outRect) const;

	virtual	void		CalcTickMarkRect(
								Rect&			outRect) const;

	virtual	SInt32		CalcTrackLength() const;

	virtual	RgnHandle	BuildIndicatorRgn(
								const Rect&		inFrame);

	void				EraseIndicator(
								SInt16			inDepth,
								const Rect&		inIndicatorRect);

	void				MoveIndicator(
								SInt16			inFromPos,
								SInt16			inToPos,
								bool			inGhost = false,
								bool			inPressed = false);

	void				FindIndicatorLimits(
								SInt16&			outMin,
								SInt16&			outMax) const;

	SInt32				PositionToValue(
								SInt16			inPosition) const;

	SInt16				ValueToPosition(
								SInt32			inValue) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
