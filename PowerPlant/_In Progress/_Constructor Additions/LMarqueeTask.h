// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMarqueeTask.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Adapted from:
//		Finder Marquee by Jordan Zimmerman ©1995 by Altura Software, Inc.
//		Used by permission.

#ifndef _H_LMarqueeTask
#define _H_LMarqueeTask
#pragma once

#include <LPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LMarqueeReceiver;


// ===========================================================================
//	¥ LMarqueeTask
// ===========================================================================
//	Adapted from Jordan Zimmerman's Finder Marquee.

class LMarqueeTask {
public:
						LMarqueeTask(
								LMarqueeReceiver&		inReceiver,
								const SMouseDownEvent&	inStartEvent);
						LMarqueeTask(
								LMarqueeReceiver&		inReceiver,
								const Point&			inInitialPoint,
								UInt16					inModifiers = 0);
	virtual				~LMarqueeTask();

	// marquee tracking

	virtual void		DoMarquee();

	// information retreival

	void			GetInitialPoint(Point& outPoint) const;

	void			GetCurrentPoint(Point& outPoint) const;

	void			GetMarqueeRect(Rect& outRect) const;

	inline UInt16		GetInitialModifiers() const
								{ return mInitialModifiers; }
	inline UInt16		GetCurrentModifiers() const
								{ return mCurrentModifiers; }

	// user data pointers

	inline void			SetUserData(UInt32 inUserData)
								{ mUserData = inUserData; }
	inline UInt32		GetUserData() const
								{ return mUserData; }

	// tracking helpers

protected:
	virtual void		StartTracking();
	virtual void		ContinueTracking(
								const Point&		inNewPoint);
	virtual void		EndTracking();

	// utility functions

	void				CalculateMarqueeRect();
	void				DrawMarqueeRect() const;
	void				MakeFrameRegion(
								RgnHandle			inTargetRegion,
								const Rect&			inFrameRect,
								RgnHandle			inWorkRegion);

	// internal initialization

private:
	void				InitMarqueeTask(
								const Point&		inInitialPoint,
								UInt16				inModifiers);


protected:
	LMarqueeReceiver&	mReceiver;				// marquee receiver object
	SPoint32			mAnchorPoint;			// initial mouse down point (image coordinates)
	SPoint32			mCurrentPoint;			// current mouse down point (image coordinates)
	SRect32				mMarqueeRect;			// currently selected rectangle
	Boolean				mMarqueeDrawn;			// true if marquee is currently visible
	Boolean				mWasFlicker;			// true if on last call to ContinueTracking, a "Flicker" marquee redraw was done.

	UInt16				mInitialModifiers;		// modifiers at mousedown time
	UInt16				mCurrentModifiers;		// modifiers at current time
	UInt32				mUserData;				// stash anything here

};


// ===========================================================================
//	¥ LMarqueeReceiver
// ===========================================================================
//	Mixin class for objects that handle marquee selections/drags
//	from LMarqueeTask.

class LMarqueeReceiver {
public:
						LMarqueeReceiver();
	virtual				~LMarqueeReceiver();

	// override these to implement selection or specialized tracking behavior

	virtual void		ConstrainMouse(
								const LMarqueeTask& inMarqueeTask,
								Point&				ioMouseLocal,
								UInt16				inModifiers);

	virtual Boolean		CheckSelectionChanged(
								const LMarqueeTask& inMarqueeTask,
								const Rect&			inOldMarqueeRect,
								const Rect&			inNewMarqueeRect);

	virtual void		UpdateSelection(
								const LMarqueeTask& inMarqueeTask,
								const Rect&			inOldMarqueeRect,
								const Rect&			inNewMarqueeRect);

	// override to specify where drawing occurs

	virtual void		FocusMarqueeArea();

	virtual SPoint32 	GlobalToImagePoint( Point p ) const;
	virtual Rect		ImageToLocalRect( const SRect32& inRect ) const;
	virtual Point		ImageToLocalPoint( const SPoint32& inPoint ) const;
	virtual Boolean		WantsToAutoscroll(Point localP);

		// Pure Virtual. Subclasses must override to return the View in
		// which the marquee is drawn.

	virtual const LView*	GetTargetView() const = 0;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
