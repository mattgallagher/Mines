// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDrawingState.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UDrawingState
#define _H_UDrawingState
#pragma once

#include <URegions.h>
#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

extern const RGBColor	Color_White;
extern const RGBColor	Color_Black;

// ---------------------------------------------------------------------------
#pragma mark UQDGlobals

class	UQDGlobals {
public:
	static void			InitializeToolbox();

	static GrafPtr		GetCurrentPort();

	static WindowPtr	GetCurrentWindowPort();

	static SInt32		GetRandomSeed();

	static void			SetRandomSeed( SInt32 inRandSeed );

	static Cursor*		GetArrow( Cursor* outArrow );

	static void			SetArrow( const Cursor* inArrow );

	static Pattern*		GetDarkGrayPat( Pattern* outPattern );

	static Pattern*		GetLightGrayPat( Pattern* outPattern );

	static Pattern*		GetGrayPat( Pattern* outPattern );

	static Pattern*		GetBlackPat( Pattern* outPattern );

	static Pattern*		GetWhitePat( Pattern* outPattern );


#if PP_Target_Classic
	static void			InitializeToolbox(QDGlobals*	inQDGlobals);

	static QDGlobals*	GetQDGlobals()		{ return sQDGlobals; }

	static void			SetQDGlobals(QDGlobals*	inQDGlobals)
							{
								sQDGlobals = inQDGlobals;
							}

	static void			SetCodeResourceQDGlobals();

private:
	static QDGlobals*	sQDGlobals;
#endif // PP_Target_Classic
};

// ===========================================================================
//	UQDGlobals inline functions

inline SInt32
UQDGlobals::GetRandomSeed()
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsRandomSeed();
	#else
		return sQDGlobals->randSeed;
	#endif
}


inline void
UQDGlobals::SetRandomSeed( SInt32 inRandSeed )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		::SetQDGlobalsRandomSeed(inRandSeed);
	#else
		sQDGlobals->randSeed = inRandSeed;
	#endif
}


inline Cursor*
UQDGlobals::GetArrow( Cursor* outArrow )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsArrow(outArrow);
	#else
		*outArrow = sQDGlobals->arrow;
		return outArrow;
	#endif
}


inline void
UQDGlobals::SetArrow( const Cursor* inArrow )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		::SetQDGlobalsArrow(inArrow);
	#else
		sQDGlobals->arrow = *inArrow;
	#endif
}


inline Pattern*
UQDGlobals::GetDarkGrayPat( Pattern* outPattern )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsDarkGray(outPattern);
	#else
		*outPattern = sQDGlobals->dkGray;
		return outPattern;
	#endif
}


inline Pattern*
UQDGlobals::GetLightGrayPat( Pattern* outPattern )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsLightGray(outPattern);
	#else
		*outPattern = sQDGlobals->ltGray;
		return outPattern;
	#endif
}


inline Pattern*
UQDGlobals::GetGrayPat( Pattern* outPattern )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsGray(outPattern);
	#else
		*outPattern = sQDGlobals->gray;
		return outPattern;
	#endif
}


inline Pattern*
UQDGlobals::GetBlackPat( Pattern* outPattern )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsBlack(outPattern);
	#else
		*outPattern = sQDGlobals->black;
		return outPattern;
	#endif
}


inline Pattern*
UQDGlobals::GetWhitePat( Pattern* outPattern )
{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		return ::GetQDGlobalsWhite(outPattern);
	#else
		*outPattern = sQDGlobals->white;
		return outPattern;
	#endif
}


// ---------------------------------------------------------------------------
#pragma mark StGrafPortSaver

class	StGrafPortSaver {
public:
				StGrafPortSaver ();
				StGrafPortSaver (GrafPtr inGrafPtr);
				~StGrafPortSaver();
				
	void		SetSavedPort( GrafPtr inGrafPtr )	{ mOriginalPort = inGrafPtr; }
	
	void		Restore();
	
private:
	GrafPtr		mOriginalPort;
};

// ---------------------------------------------------------------------------
#pragma mark StColorState

class	StColorState {
public:
				StColorState ();
				~StColorState();

	void		Save();
	void		Restore();

	static void	Normalize();

private:
	RGBColor	mForegroundColor;
	RGBColor	mBackgroundColor;
};

// ---------------------------------------------------------------------------
#pragma mark StColorPenState

class	StColorPenState {
public:
				StColorPenState ();
				~StColorPenState();

	void		Save();
	void		Restore();

	static void	Normalize();
	static void	SetGrayPattern();

private:
	StColorState	mColorState;
	PenState		mPenState;
};

// ---------------------------------------------------------------------------
#pragma mark StTextState

class	StTextState {
public:
				StTextState ();
				~StTextState();

	void		Save();
	void		Restore();

	static void	Normalize();

private:
	SInt16		mFontNumber;
	SInt16		mSize;
	SInt16		mStyle;
	SInt16		mMode;
};

// ---------------------------------------------------------------------------
#pragma mark StClipRgnState

class	StClipRgnState {
public:
				StClipRgnState ();
				StClipRgnState (const Rect&		inClipRect);
				StClipRgnState (RgnHandle		inClipRgnH);
				~StClipRgnState();

	void		Save()			{ ::GetClip(mClipRgn); }
	void		Restore()		{ ::SetClip(mClipRgn); }

	void		ClipToIntersection(RgnHandle	inRgnH);
	void		ClipToIntersection(const Rect&	inRect);

	void		ClipToDifference(RgnHandle		inRgnH);
	void		ClipToDifference(const Rect&	inRect);

	static void	Normalize();

	static void	SectWithCurrent(RgnHandle		inRgnH);
	static void	SectWithCurrent(const Rect&		inRect);

	static void	DiffWithCurrent(RgnHandle		inRgnH);
	static void	DiffWithCurrent(const Rect&		inRect);

	void		ClipToIntersectionRgn(RgnHandle inRgnH)		// Old API
					{
						ClipToIntersection(inRgnH);
					}

private:
	StRegion	mClipRgn;
};

// ---------------------------------------------------------------------------
#pragma mark StOriginState

class	StOriginState {
public:
				StOriginState();
				~StOriginState();
				
	void		Save();
	void		Restore();
	
	static void	Normalize()		{ ::SetOrigin(0, 0); }
	
private:
	Point		mOrigin;
};

// ---------------------------------------------------------------------------
#pragma mark StClipOriginState

class	StClipOriginState {
public:
				StClipOriginState();
				StClipOriginState( const Point& inNewOrigin );
				
				~StClipOriginState();
				
	void		Save();
	void		Restore();
				
	void		SetOrigin( const Point& inNewOrigin );
				
private:
	GrafPtr		mGrafPtr;
	Point		mOrigin;
	StRegion	mClipRgn;
};


// ---------------------------------------------------------------------------
#pragma mark StPortToggler

class	StPortToggler {
public:
				StPortToggler( GrafPtr inTogglePort = nil );
				~StPortToggler();
	
	void		EstablishOriginalPort()		{ ::MacSetPort(mOriginalPort); }
	
	void		EstablishTogglePort()		{ ::MacSetPort(mTogglePort); }
				
private:
	GrafPtr		mOriginalPort;
	GrafPtr		mTogglePort;
};


// ---------------------------------------------------------------------------
#pragma mark StPortOriginState

class	StPortOriginState {
public:
				StPortOriginState( GrafPtr inGrafPtr = nil );
				~StPortOriginState();

	void		Restore();

	static void	Normalize()		{ ::SetOrigin(0, 0); }

private:
	StPortToggler	mPortToggler;	// Current and Input Port
	StOriginState	mOrigin;		// Saved origin of Input Port
};

// ---------------------------------------------------------------------------
#pragma mark StColorPortState

class	StColorPortState {
public:
				StColorPortState( GrafPtr inGrafPtr = nil );
				~StColorPortState();

	void		Restore();

	static void	Normalize();

private:
	StPortToggler		mPortToggler;	// Current and Input Port
	StOriginState		mOriginState;
	StColorPenState		mColorPenState;
	StTextState			mTextState;
	StClipRgnState		mClipRgnState;
};

// ---------------------------------------------------------------------------
#pragma mark StThemeDrawingState

class	StThemeDrawingState {
public:
				StThemeDrawingState();
				StThemeDrawingState( ThemeDrawingState inState );
				~StThemeDrawingState();
				
	static void	Normalize();

private:
	ThemeDrawingState	mThemeDrawingState;
};

// ---------------------------------------------------------------------------
#pragma mark StHidePen

class	StHidePen {
public:
				StHidePen ()		{ ::HidePen(); }
				~StHidePen();
};

// ---------------------------------------------------------------------------
#pragma mark StVisibleRgn

class	StVisibleRgn {
public:
				StVisibleRgn( GrafPtr inPort );

				StVisibleRgn( GrafPtr inPort, RgnHandle inRgnH );

				StVisibleRgn( GrafPtr inPort, const Rect& inRect );

				~StVisibleRgn();
	
	void		SectWithSaved( RgnHandle inRgnH );
	
	void		SectWithCurrent( RgnHandle inRgnH );
	
	void		DiffWithSaved( RgnHandle inRgnH );
	
	void		DiffWithCurrent( RgnHandle inRgnH );

	void		Restore();

private:
	void		Save();

private:
	GrafPtr		mGrafPtr;
	StRegion	mSaveVisRgn;
};

// ---------------------------------------------------------------------------
#pragma mark StCQDProcsState

class	StCQDProcsState	{
public:
				StCQDProcsState(
						bool		inSetProcs,
						CQDProcs*	inCQDProcs,
						CGrafPtr	inPort);

				~StCQDProcsState();

	void		SetProcs(
						bool		inSetProcs,
						CQDProcs*	inCQDProcs);

	void		ResetProcs();

protected:
	CQDProcs*	mCQDProcs;
	CGrafPtr	mPort;
	bool		mSetProcs;
};

// ---------------------------------------------------------------------------
#pragma mark StGDeviceSaver

class	StGDeviceSaver {
public:
				StGDeviceSaver();
				~StGDeviceSaver();

protected:
	CGrafPtr	mSavePort;
	GDHandle	mSaveDevice;
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
