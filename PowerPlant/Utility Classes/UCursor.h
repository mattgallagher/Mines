// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCursor.h			  		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UCursor
#define _H_UCursor
#pragma once

#include <PP_Prefix.h>
#include <TArray.h>
#include <Appearance.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	UCursor
// ---------------------------------------------------------------------------

namespace UCursor {

	void		Normalize();
	
	void		SetCursorID( ResIDT inCURSid );
	
	void		SetThemeCursor( ThemeCursor inCursor );
	
	void		SetArrow();
	
	void		SetIBeam();
	
	void		SetCross();
	
	void		SetPlus();
	
	void		SetWatch();
	
		
		// These functions are for compatibility with the old
		// UCursor API. Please update to the new function names.
	
	inline void		InitTheCursor()	{ Normalize(); }	// Deprecated
	
	inline void		SetTheCursor( ResIDT inCURSid )		// Deprecated
						{
							SetCursorID(inCURSid);
						}
}


// ---------------------------------------------------------------------------
//	LCursor
// ---------------------------------------------------------------------------

class LCursor {
public:
					LCursor();
					
	virtual			~LCursor();
						
	virtual void	Apply() const = 0;
	
	virtual void	Animate( UInt32 inAnimationStep ) const;
};


// ---------------------------------------------------------------------------
//	LThemeCursor
// ---------------------------------------------------------------------------

class LThemeCursor : public LCursor {
public:
					LThemeCursor( ThemeCursor inThemeCursor );
	
	virtual			~LThemeCursor();
	
	virtual void	Apply() const;
	
	virtual void	Animate( UInt32 inAnimationStep ) const;

protected:
	ThemeCursor		mThemeCursor;
};


// ---------------------------------------------------------------------------
//	LBWCursor
// ---------------------------------------------------------------------------

class LBWCursor : public LCursor {
public:
					LBWCursor( ResIDT inCursorID );
						
	virtual			~LBWCursor();
						
	virtual void	Apply() const;
	
protected:
	Cursor			mCursor;
};


// ---------------------------------------------------------------------------
//	LBWCursorList
// ---------------------------------------------------------------------------

class LBWCursorList : public LCursor {
public:
					LBWCursorList( ResIDT inAcurID );
					
					LBWCursorList(
							UInt32		inNumCursors,
							ResIDT*		inCursorIDs);
						
	virtual			~LBWCursorList();
						
	virtual void	Apply() const;
	
	virtual void	Animate( UInt32 inAnimationStep ) const;
	
protected:
	Cursor*			mCursorList;
	UInt32			mCursorCount;
};


// ---------------------------------------------------------------------------
//	LColorCursor
// ---------------------------------------------------------------------------

class LColorCursor : public LCursor {
public:
					LColorCursor( ResIDT inCrsrID );
						
	virtual			~LColorCursor();
						
	virtual void	Apply() const;
	
protected:
	CCrsrHandle		mCursor;
};


// ---------------------------------------------------------------------------
//	LColorCursorList
// ---------------------------------------------------------------------------

class LColorCursorList : public LCursor {
public:
					LColorCursorList( ResIDT inAcurID );
					
					LColorCursorList(
							UInt32		inNumCursors,
							ResIDT*		inCursorIDs);
						
	virtual			~LColorCursorList();
						
	virtual void	Apply() const;
	
	virtual void	Animate( UInt32 inAnimationStep ) const;
	
protected:
	CCrsrHandle*	mCursorList;
	UInt32			mCursorCount;
};


// ---------------------------------------------------------------------------
//	LAnimatedCursor
// ---------------------------------------------------------------------------

class LAnimatedCursor {
public:
						LAnimatedCursor();
						
						LAnimatedCursor( LCursor* inCursor );
								
						~LAnimatedCursor();
						
	void				SpecifyCursor( LCursor* inCursor );
								
	void				SetAnimationTimes(
								UInt32		inInitialDelay,
								UInt32		inTickInterval);
							
	void				Apply();
	
	void				Animate();
	
protected:
	LCursor*			mCursor;
	UInt32				mInitialDelay;
	UInt32				mTickInterval;
	UInt32				mNextTick;
	UInt32				mAnimationStep;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
