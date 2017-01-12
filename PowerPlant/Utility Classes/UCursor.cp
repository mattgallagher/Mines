// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCursor.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UCursor.h>
#include <PP_Resources.h>
#include <TArrayIterator.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//	UCursor
// ===========================================================================
//	There are 3 implementations for the functions is this namespace.
//	Preprocessor symbol specifies which one is used.

#if ( PP_Option_Cursor_Standard +			\
	  PP_Option_Cursor_Theme +				\
	  PP_Option_Cursor_Conditional ) > 1
	  
	  #error Multiple cursor options specified. Only one is allowed.
#endif


// ---------------------------------------------------------------------------
//	¥ Normalize
// ---------------------------------------------------------------------------

void
UCursor::Normalize()
{
	::InitCursor();
}


// ---------------------------------------------------------------------------
//	¥ SetCursorID
// ---------------------------------------------------------------------------

void
UCursor::SetCursorID(
	ResIDT	inCURSid)
{
	if (inCURSid == 0) {
	
		SetArrow();
		
	} else {
		CursHandle	cursorH = ::MacGetCursor(inCURSid);
		
		if (cursorH == nil) {
		
				// CURS resource not found. If you are running on
				// a System without Appearance 1.1 and use the
				// ThemeCursor constants, you need to add
				// "PP Cursors.rsrc" to your project (or copy the
				// CURS resources that you need into your resource file).
		
			SignalStringLiteral_("Cursor not found");
			
			SetArrow();
			
		} else {
			::MacSetCursor(*cursorH);
		}
	}
}

#pragma mark -
#pragma mark === Standard ===

// ---------------------------------------------------------------------------
//	Standard Implementation

#if PP_Option_Cursor_Standard

// ---------------------------------------------------------------------------
//	¥ SetThemeCursor
// ---------------------------------------------------------------------------

void
UCursor::SetThemeCursor(
	ThemeCursor	inCursor)
{	
		// Map ThemeCursor to standard cursors
		// All Systems have Arrow, IBeam, Cross, Plus, and Watch cursors

	switch (inCursor) {
	
		case kThemeArrowCursor:
			SetArrow();
			break;
		
		case kThemeIBeamCursor:
			SetCursorID(iBeamCursor);
			break;
		
		case kThemeCrossCursor:
			SetCursorID(crossCursor);
			break;
		
		case kThemePlusCursor:
			SetCursorID(plusCursor);
			break;
		
		case kThemeWatchCursor:
			SetCursorID(watchCursor);
			break;
			
		default: {
		
				// Without the Appearance Manager, the System
				// may not have certain Theme cursors. Appearance.h
				// defines a ThemeCursor enum with consecutive numbers.
				// As of Universal Interfaces 3.4, there are 18 Theme
				// cursors. The file "PP Cursors.rsrc" contains CURS
				// resources for these cursors (excluding the 5
				// standard ones), with the resource IDs being
				// CURS_ThemeBase plus the ThemeCursor enum value.
				
			ResIDT	cursID = (ResIDT) (inCursor + CURS_ThemeBase);
			SetCursorID(cursID);
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetArrow
// ---------------------------------------------------------------------------

void
UCursor::SetArrow()
{
	static Cursor	sArrowCursor = *UQDGlobals::GetArrow(&sArrowCursor);
	
	::MacSetCursor(&sArrowCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetIBeam
// ---------------------------------------------------------------------------

void
UCursor::SetIBeam()
{
	SetCursorID(iBeamCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetCross
// ---------------------------------------------------------------------------

void
UCursor::SetCross()
{
	SetCursorID(crossCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetPlus
// ---------------------------------------------------------------------------

void
UCursor::SetPlus()
{
	SetCursorID(plusCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetWatch
// ---------------------------------------------------------------------------

void
UCursor::SetWatch()
{
	SetCursorID(watchCursor);
}

#endif // PP_Option_Cursor_Standard

#pragma mark -
#pragma mark === Theme ===

// ---------------------------------------------------------------------------
//	Theme Implementation

#if PP_Option_Cursor_Theme

// ---------------------------------------------------------------------------
//	¥ SetThemeCursor
// ---------------------------------------------------------------------------
#include <Sound.h>
void
UCursor::SetThemeCursor(
	ThemeCursor	inCursor)
{
	::SetThemeCursor(inCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetArrow
// ---------------------------------------------------------------------------

void
UCursor::SetArrow()
{
	::SetThemeCursor(kThemeArrowCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetIBeam
// ---------------------------------------------------------------------------

void
UCursor::SetIBeam()
{
	::SetThemeCursor(kThemeIBeamCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetCross
// ---------------------------------------------------------------------------

void
UCursor::SetCross()
{
	::SetThemeCursor(kThemeCrossCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetPlus
// ---------------------------------------------------------------------------

void
UCursor::SetPlus()
{
	::SetThemeCursor(kThemePlusCursor);
}


// ---------------------------------------------------------------------------
//	¥ SetWatch
// ---------------------------------------------------------------------------

void
UCursor::SetWatch()
{
	::SetThemeCursor(kThemeWatchCursor);
}

#endif // PP_Option_Cursor_Theme

#pragma mark -
#pragma mark === Conditional ===

// ---------------------------------------------------------------------------
//	Conditional Implementation
//
//	Even though the routine SetThemeCursor() exists in all versions of
//	CarbonLib, calling that routine will crash if the underlying System
//	does not have Appearance 1.1. This unfortunate situation occurs in
//	a configuration such as Mac OS 8.1 running CarbonLib 1.0.4 and
//	Appearance 1.0.x.

#if PP_Option_Cursor_Conditional

// ---------------------------------------------------------------------------
//	¥ SetThemeCursor
// ---------------------------------------------------------------------------

void
UCursor::SetThemeCursor(
	ThemeCursor	inCursor)
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {
	
		::SetThemeCursor(inCursor);
		
	} else {

			// Map ThemeCursor to standard cursors
			// All Systems have Arrow, IBeam, Cross, Plus, and Watch cursors

		switch (inCursor) {
		
			case kThemeArrowCursor:
				SetArrow();
				break;
			
			case kThemeIBeamCursor:
				SetCursorID(iBeamCursor);
				break;
			
			case kThemeCrossCursor:
				SetCursorID(crossCursor);
				break;
			
			case kThemePlusCursor:
				SetCursorID(plusCursor);
				break;
			
			case kThemeWatchCursor:
				SetCursorID(watchCursor);
				break;
				
			default: {
			
					// Without the Appearance Manager, the System
					// may not have certain Theme cursors. Appearance.h
					// defines a ThemeCursor enum with consecutive numbers.
					// As of Universal Interfaces 3.4, there are 18 Theme
					// cursors. The file "PP Cursors.rsrc" contains CURS
					// resources for these cursors (excluding the 5
					// standard ones), with the resource IDs being
					// CURS_ThemeBase plus the ThemeCursor enum value.
					
				ResIDT	cursID = (ResIDT) (inCursor + CURS_ThemeBase);
				SetCursorID(cursID);
				break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetArrow
// ---------------------------------------------------------------------------

void
UCursor::SetArrow()
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {

		::SetThemeCursor(kThemeArrowCursor);
		
	} else {
		static Cursor	sArrowCursor = *UQDGlobals::GetArrow(&sArrowCursor);
	
		::MacSetCursor(&sArrowCursor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetIBeam
// ---------------------------------------------------------------------------

void
UCursor::SetIBeam()
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		::SetThemeCursor(kThemeIBeamCursor);
	} else {
		SetCursorID(iBeamCursor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCross
// ---------------------------------------------------------------------------

void
UCursor::SetCross()
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		::SetThemeCursor(kThemeCrossCursor);
	} else {
		SetCursorID(crossCursor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPlus
// ---------------------------------------------------------------------------

void
UCursor::SetPlus()
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		::SetThemeCursor(kThemePlusCursor);
	} else {
		SetCursorID(plusCursor);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetWatch
// ---------------------------------------------------------------------------

void
UCursor::SetWatch()
{
	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		::SetThemeCursor(kThemeWatchCursor);
	} else {
		SetCursorID(watchCursor);
	}
}

#endif // PP_Option_Cursor_Conditional

#pragma mark -
// ===========================================================================
//	LCursor
// ===========================================================================
//	Abstract base class for a Cursor object
//
//	Concrete subclasses must implement the Apply() function, which sets
//	the shape of the system cursor

// ---------------------------------------------------------------------------
//	¥ LCursor								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCursor::LCursor()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LCursor								Destructor				  [public]
// ---------------------------------------------------------------------------

LCursor::~LCursor()
{
}


// ---------------------------------------------------------------------------
//	¥ Animate
// ---------------------------------------------------------------------------
//	Call repeatedly to animate a Cursor
//
//		inAnimationStep - An animated cursor is conceptually a list of
//							of individual cursors which are applied
//							consecutively in a loop. This parameter specifies
//							stage in the loop.
//
//							LAnimatedCursor increments the parameter value
//							on each successive call. This folows how the
//							parameter is used by the Toolbox for animating
//							Theme Cursors.
//
//	Subclasses should override to support cursor animation

void
LCursor::Animate(
	UInt32	/* inAnimationStep */) const
{
	Apply();				// Base class doesn't handle animation so we
}							//   just apply our cursor shape


#pragma mark -
// ===========================================================================
//	LThemeCursor
// ===========================================================================
//	A cursor which uses the Appearance Manager to set the Theme cursor.
//	See <Appearance.h> for the list of ThemeCursor constants.
//
//	This class requires the Appearance Manager 1.1. If you program can run
//	on systems without AM 1.1, you should perform a runtime check
//	before creating instances of this class.

// ---------------------------------------------------------------------------
//	¥ LThemeCursor							Constructor				  [public]
// ---------------------------------------------------------------------------

LThemeCursor::LThemeCursor(
	ThemeCursor	inThemeCursor)
{
	mThemeCursor = inThemeCursor;
}


// ---------------------------------------------------------------------------
//	¥ ~LThemeCursor							Destructor				  [public]
// ---------------------------------------------------------------------------

LThemeCursor::~LThemeCursor()
{
}


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------

void
LThemeCursor::Apply() const
{
	::SetThemeCursor(mThemeCursor);
}


// ---------------------------------------------------------------------------
//	¥ Animate														  [public]
// ---------------------------------------------------------------------------
//	In Appearance 1.1, the Toolbox supports animation for the following
//	Theme Cursors:
//
//		kThemeWatchCursor
//		kThemeCountingUpHandCursor
//		kThemeCountingDownHandCursor
//		kThemeCountingUpAndDownHandCursor
//		kThemeSpinningCursor

void
LThemeCursor::Animate(
	UInt32	inAnimationStep) const
{
	::SetAnimatedThemeCursor(mThemeCursor, inAnimationStep);
}


#pragma mark -
// ===========================================================================
//	LBWCursor
// ===========================================================================
//	A black & white curosr
//
//	The system stores black & white cursors in 'CURS' resources

// ---------------------------------------------------------------------------
//	¥ LBWCursor								Constructor				  [public]
// ---------------------------------------------------------------------------

LBWCursor::LBWCursor(
	ResIDT	inCursorID)					// 'CURS' resource ID
{
	CursHandle	cursorH = ::MacGetCursor(inCursorID);
	
	if (cursorH == nil) {				// Couldn't load cursor
	
		SignalStringLiteral_("Cursor not found");
		
		UQDGlobals::GetArrow(&mCursor);	// Use arrow by default
		
	} else {							// Copy cursor bitmap into our
		mCursor = **cursorH;			//   member variable
		ReleaseResource((Handle) cursorH);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LBWCursor							Destructor				  [public]
// ---------------------------------------------------------------------------

LBWCursor::~LBWCursor()
{
}


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------

void
LBWCursor::Apply() const
{
	::MacSetCursor(&mCursor);
}


#pragma mark -
// ===========================================================================
//	LBWCursorList
// ===========================================================================
//	A Cursor which stores a list of black & white cursor IDs and supports
//	animation by looping through that list

// ---------------------------------------------------------------------------
//	Data structures needed for parsing 'acur' resources

struct	SCursorIDArray {
	SInt16	resID;
	SInt16	padding;
};

struct	SacurResource {
	SInt16	numCursors;
	SInt16	padding;
	
	SCursorIDArray	cursors[1];
};


// ---------------------------------------------------------------------------
//	¥ LBWCursorList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an 'acur' resource
//
//	An 'acur' is a system resource type specify a list of cursor IDs

LBWCursorList::LBWCursorList(
	ResIDT	inAcurID)
{
	StResource		acurRes(ResType_AnimatedCursor, inAcurID);
	StHandleLocker	acurLock(acurRes);
	
	SacurResource*	acurP = (SacurResource*) *acurRes.Get();
	
	mCursorCount = acurP->numCursors;
	
	ThrowIf_( mCursorCount <= 0 );
	
	mCursorList	 = new Cursor[mCursorCount];
	
	for (SInt16 i = 0; i < mCursorCount; i++) {
	
		CursHandle	cursorH = ::MacGetCursor(acurP->cursors[i].resID);
		
		if (cursorH == nil) {
		
			SignalStringLiteral_("Cursor not found");
			
		} else {
		
			mCursorList[i] = **cursorH;
			ReleaseResource((Handle) cursorH);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LBWCursorList							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from a list of 'CURS' resource IDs

LBWCursorList::LBWCursorList(
	UInt32		inNumCursors,			// Number of cursors in list
	ResIDT*		inCursorIDs)			// Array of cursor IDs
{
		// We create an array which stores the actual bitmaps
		// of all the cursors. This gives fast access and we
		// don't have to manage Handles.
		
	mCursorCount = inNumCursors;
	mCursorList  = new Cursor[inNumCursors];
	
	for (UInt32 i = 0; i < inNumCursors; i++) {
	
		CursHandle	cursorH = ::MacGetCursor(inCursorIDs[i]);
		
		if (cursorH == nil) {
		
			SignalStringLiteral_("Cursor not found");
			
		} else {
		
			mCursorList[i] = **cursorH;
			ReleaseResource((Handle) cursorH);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LBWCursorList						Destructor				  [public]
// ---------------------------------------------------------------------------

LBWCursorList::~LBWCursorList()
{
	delete[] mCursorList;
}


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------

void
LBWCursorList::Apply() const
{
	::MacSetCursor(&mCursorList[0]);	// Use first cursor in list
}


// ---------------------------------------------------------------------------
//	¥ Animate														  [public]
// ---------------------------------------------------------------------------

void
LBWCursorList::Animate(
	UInt32	inAnimationStep) const
{
		// inAnimationStep is an increasing counter. It does not loop.
		// Thus we determine the cursor to use by taking the modulus
		// with the number of cursors in our list

	UInt32	index = inAnimationStep % mCursorCount;
	
	::MacSetCursor(&mCursorList[index]);
}


#pragma mark -
// ===========================================================================
//	LColorCursor
// ===========================================================================
//	A Color Cursor
//
//	The system stores color cursors in 'crsr' resources

// ---------------------------------------------------------------------------
//	¥ LColorCursor							Constructor				  [public]
// ---------------------------------------------------------------------------

LColorCursor::LColorCursor(
	ResIDT	inCrsrID)					// 'crsr' resource ID
{
	mCursor = ::GetCCursor(inCrsrID);	// Load Handle to color cursor
	
	ThrowIfNil_(mCursor);
}


// ---------------------------------------------------------------------------
//	¥ ~LColorCursor							Destructor				  [public]
// ---------------------------------------------------------------------------

LColorCursor::~LColorCursor()
{
	::DisposeCCursor(mCursor);			// Dispose Handle to color cursor
}


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------

void
LColorCursor::Apply() const
{
	::SetCCursor(mCursor);
}


#pragma mark -
// ===========================================================================
//	LColorCursorList
// ===========================================================================
//	A Cursor which stores a list of colors cursors and supports
//	animation by looping through that list

// ---------------------------------------------------------------------------
//	¥ LColorCursorList						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an 'acur' resource
//
//	Although 'acur' resources traditionally store IDs of black & white
//	cursors, we just treat them as a list of ID numbers

LColorCursorList::LColorCursorList(
	ResIDT	inAcurID)				// 'acur' resource ID
{
	StResource		acurRes(ResType_AnimatedCursor, inAcurID);
	StHandleLocker	acurLock(acurRes);
	
	SacurResource*	acurP = (SacurResource*) *acurRes.Get();
	
	mCursorCount = acurP->numCursors;
	
	ThrowIf_( mCursorCount <= 0 );
	
	mCursorList = new CCrsrHandle[mCursorCount];
	
	for (SInt16 i = 0; i < mCursorCount; i++) {
	
		mCursorList[i] = ::GetCCursor(acurP->cursors[i].resID);
		
		if (mCursorList[i] == nil) {
		
			SignalStringLiteral_("Color Cursor not found");
			
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LColorCursorList						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from a list of 'crsr' resource IDs

LColorCursorList::LColorCursorList(
	UInt32		inNumCursors,			// Number of cursors in list
	ResIDT*		inCursorIDs)			// Array of 'crsr' IDs
{
	mCursorCount = inNumCursors;
	
	mCursorList = new CCrsrHandle[inNumCursors];
	
	for (UInt32 i = 0; i < inNumCursors; i++) {
	
		mCursorList[i] = ::GetCCursor(inCursorIDs[i]);
		
		if (mCursorList[i] == nil) {
		
			SignalStringLiteral_("Color Cursor not found");
			
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LColorCursorList						Destructor				  [public]
// ---------------------------------------------------------------------------

LColorCursorList::~LColorCursorList()
{
		// Unlike black and white cursors

	for (UInt32 i = 0; i < mCursorCount; i++) {
	
		::DisposeCCursor(mCursorList[i]);
	}
	
	delete[] mCursorList;
}


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------

void
LColorCursorList::Apply() const
{
	::SetCCursor(mCursorList[0]);	// Use first cursor in list
}


// ---------------------------------------------------------------------------
//	¥ Animate														  [public]
// ---------------------------------------------------------------------------

void
LColorCursorList::Animate(
	UInt32	inAnimationStep) const
{
		// inAnimationStep is an increasing counter. It does not loop.
		// Thus we determine the cursor to use by taking the modulus
		// with the number of cursors in our list

	UInt32	index = inAnimationStep % mCursorCount;
	
	::SetCCursor(mCursorList[index]);
}


#pragma mark -
// ===========================================================================
//	LAnimatedCursor
// ===========================================================================
//	An animated cursor

// ---------------------------------------------------------------------------
//	¥ LAnimatedCursor						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LAnimatedCursor::LAnimatedCursor()
{
	mCursor = nil;

	mInitialDelay = 120;
	mTickInterval = 10;
	mNextTick	  = 0;
	mAnimationStep = 0;
}


// ---------------------------------------------------------------------------
//	¥ LAnimatedCursor						Constructor				  [public]
// ---------------------------------------------------------------------------

LAnimatedCursor::LAnimatedCursor(
	LCursor*	inCursor)
{
	mCursor = inCursor;
	
	mInitialDelay = 120;		// Ticks to delay before starting animation
	mTickInterval = 10;			// Tick interval between animation steps
	mNextTick	  = 0;			// Time to move to next animation step
	mAnimationStep = 0;			// Index of current animation step
}


// ---------------------------------------------------------------------------
//	¥ ~LAnimatedCursor						Destructor				  [public]
// ---------------------------------------------------------------------------

LAnimatedCursor::~LAnimatedCursor()
{
	delete mCursor;
}


// ---------------------------------------------------------------------------
//	¥ SpecifyCursor													  [public]
// ---------------------------------------------------------------------------

void
LAnimatedCursor::SpecifyCursor(
	LCursor*	inCursor)
{
	if (inCursor != mCursor) {
		delete mCursor;
	}
	
	mCursor = inCursor;
}


// ---------------------------------------------------------------------------
//	¥ SetAnimationTimes												  [public]
// ---------------------------------------------------------------------------

void
LAnimatedCursor::SetAnimationTimes(
	UInt32	inInitialDelay,
	UInt32	inTickInterval)
{
	mInitialDelay = inInitialDelay;
	mTickInterval = inTickInterval;
}	


// ---------------------------------------------------------------------------
//	¥ Apply															  [public]
// ---------------------------------------------------------------------------
//	Set the cursor shape
//
//	This resets the animation to the first step and re-establishes the
//	initial delay for subsequent animation.

void
LAnimatedCursor::Apply()
{
	mAnimationStep = 0;
	mNextTick = ::TickCount() + mInitialDelay;

	if (mCursor != nil) {
		mCursor->Apply();
	}
}


// ---------------------------------------------------------------------------
//	¥ Animate														  [public]
// ---------------------------------------------------------------------------
//	Animate the Cursor

void
LAnimatedCursor::Animate()
{
	UInt32	currTick = ::TickCount();	// Move to next animation step
										//   if enough ticks have passed
	if (currTick >= mNextTick) {
	
		if (mCursor != nil) {			// Animation step is an always
										//   increasing counter
			mCursor->Animate( ++mAnimationStep );
		}
		
		mNextTick = currTick + mTickInterval;
	}
}


PP_End_Namespace_PowerPlant
