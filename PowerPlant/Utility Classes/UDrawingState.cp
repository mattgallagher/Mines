// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDrawingState.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes for saving/restoring the drawing state
//
//	For each class, the Constructor saves the current state and the
//	Destructor restores it, so you can use stack-based objects to
//	automatically save and restore the state.
//
//	There are separate Save and Restore functions in case you want to
//	explicitly change the state (at other than construct and destruct time).
//
//	Finally, each class has a Normalize function which sets the state of
//	the current GrafPort to default values (same as those set by OpenPort).
//
//	NOTE: Except for StGrafPortSaver, StPortOriginState, and StColorPortState,
//	all classes assume that the current port does not change between calls
//	to the constructor and destructor. If your code (or some routine you
//	call) could change the port, you should use an StGrafPortSaver to
//	save/restore the port. For example,
//
//		StColorState	saveColors;
//		StGrafPortSaver	savePort;
//
//			// Code here that changes fore/back colors
//			//  and might change the current port
//
//	It's important to declare the StGrafPortSave AFTER the other class.
//	Destructors get called in inverse order of declaration, so the port
//	gets restored by ~StGrafPortSaver before the colors get restored
//	by ~StColorState.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDrawingState.h>
#include <UAppearance.h>
#include <UTBAccessors.h>

#include <Dialogs.h>
#include <Fonts.h>
#include <LowMem.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Color Constants

const RGBColor	Color_White	= { 65535, 65535, 65535 };
const RGBColor	Color_Black = { 0, 0, 0 };


// ===========================================================================
//	UQDGlobals - Classic Version
// ===========================================================================
#if PP_Target_Classic

#pragma mark === Classic ===

// ---------------------------------------------------------------------------
//	Pointer to the QuickDraw globals

QDGlobals*	UQDGlobals::sQDGlobals;


// ---------------------------------------------------------------------------
//	¥ InitializeToolbox												 [Classic]
// ---------------------------------------------------------------------------
//	Standalone applications should call this function near the beginning
//	of the program initialize the standard Toolbox managers

void
UQDGlobals::InitializeToolbox()
{
	sQDGlobals = &qd;

	::InitGraf(&sQDGlobals->thePort);		// Toolbox Managers
	::InitFonts();
	::InitWindows();
	::InitMenus();
	::TEInit();
	::InitDialogs(nil);
}


// ---------------------------------------------------------------------------
//	¥ InitializeToolbox												 [Classic]
// ---------------------------------------------------------------------------
//	Standalone applications should call this function near the beginning
//	of the program to set the QD globals pointer and initialize the
//	standard Toolbox managers

void
UQDGlobals::InitializeToolbox(
	QDGlobals	*inQDGlobals)
{
	sQDGlobals = inQDGlobals;

	::InitGraf(&sQDGlobals->thePort);		// Toolbox Managers
	::InitFonts();
	::InitWindows();
	::InitMenus();
	::TEInit();
	::InitDialogs(nil);
}


// ---------------------------------------------------------------------------
//	¥ SetCodeResourceQDGlobals										 [Classic]
// ---------------------------------------------------------------------------
//	Code resources should call this function to set the QD globals pointer

void
UQDGlobals::SetCodeResourceQDGlobals()
{
		// The memory layout of the A5 world is:
		//
		//				QuickDraw Globals struct
		//					"Other" fields
		//		A5 -->		Current GrafPtr
		//
		// Where the Current GrafPtr is the last field in the
		// QDGlobals struct. To get a pointer to the beginning of the
		// QDGlobals struct, we need to back up from A5 by the
		// size of the "other fields", which is the difference between
		// the size of the entire QDGlobals struct and the size of
		// the current GrafPtr field.
		//
		// Note that LMGetCurrentA5() returns a pointer to the A5 value
		// and not the value itself.

	sQDGlobals = (QDGlobalsPtr) (*(char**) LMGetCurrentA5() -
									(sizeof(QDGlobals) - sizeof(GrafPtr)));
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentPort												 [Classic]
// ---------------------------------------------------------------------------
//	Returns a pointer to the current GrafPort

GrafPtr
UQDGlobals::GetCurrentPort()
{
	GrafPtr		currPort;
	::GetPort(&currPort);
	return currPort;
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentWindowPort											 [Classic]
// ---------------------------------------------------------------------------
//	Returns a pointer to the current WindowPtr
//
//	Warning: This function just typecasts the current GrafPtr to a WindowPtr.
//	The caller must be sure that the current port is a Window.

WindowPtr
UQDGlobals::GetCurrentWindowPort()
{
	return (WindowPtr) GetCurrentPort();
}

#endif // PP_Target_Classic

#pragma mark -

// ===========================================================================
//	UQDGlobals - Carbon Version
// ===========================================================================
#if PP_Target_Carbon

#pragma mark === Carbon ===

// ---------------------------------------------------------------------------
//	¥ InitializeToolbox												  [Carbon]
// ---------------------------------------------------------------------------
//	Carbon automatically initializes the Toolbox, so this function is empty.

void
UQDGlobals::InitializeToolbox()
{
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentPort												  [Carbon]
// ---------------------------------------------------------------------------
//	Returns a pointer to the current GrafPort

GrafPtr
UQDGlobals::GetCurrentPort()
{
	GrafPtr		currPort;
	::GetPort(&currPort);
	return currPort;
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentWindowPort											  [Carbon]
// ---------------------------------------------------------------------------
//	Returns a pointer to the current WindowPtr

WindowPtr
UQDGlobals::GetCurrentWindowPort()
{
	return ::GetWindowFromPort(UQDGlobals::GetCurrentPort());
}

#endif // PP_Target_Carbon

#pragma mark -

// ===========================================================================
//	StGrafPortSaver
// ===========================================================================
//
//	Constructor saves current GrafPort. Destructor restores it.

StGrafPortSaver::StGrafPortSaver()
{
	::GetPort(&mOriginalPort);			// Save current port
}


StGrafPortSaver::StGrafPortSaver(
	GrafPtr		inGrafPtr)
{
	::GetPort(&mOriginalPort);			// Save Current Port
	
	if (inGrafPtr != nil) {				// Switch to new port
		::MacSetPort(inGrafPtr);
	}
}


StGrafPortSaver::~StGrafPortSaver()
{
	Restore();
}


void
StGrafPortSaver::Restore()
{
	if (mOriginalPort != nil) {			// Make saved port the current one
		::MacSetPort(mOriginalPort);
	}
}

#pragma mark -

// ===========================================================================
//	StColorState
// ===========================================================================
//
//	Color State Info:
//		Foreground Color
//		Background Color

StColorState::StColorState()
{
	Save();
}


StColorState::~StColorState()
{
	Restore();
}


void
StColorState::Save()
{
	::GetForeColor(&mForegroundColor);
	::GetBackColor(&mBackgroundColor);
}


void
StColorState::Restore()
{
	::RGBForeColor(&mForegroundColor);
	::RGBBackColor(&mBackgroundColor);
}


void
StColorState::Normalize()
{
	::RGBForeColor(&Color_Black);
	::RGBBackColor(&Color_White);
}

#pragma mark -

// ===========================================================================
//	StColorPenState
// ===========================================================================
//
//	Color Pen State Info:
//		Foreground Color
//		Background Color
//		Pen Location
//		Pen Size
//		Pen Mode
//		Pen Pattern

StColorPenState::StColorPenState()
{
	::GetPenState(&mPenState);
}


StColorPenState::~StColorPenState()
{
	::SetPenState(&mPenState);
}


void
StColorPenState::Save()
{
	mColorState.Save();
	::GetPenState(&mPenState);
}


void
StColorPenState::Restore()
{
	mColorState.Restore();
	::SetPenState(&mPenState);
}


void
StColorPenState::Normalize()
{
	StColorState::Normalize();
	::PenNormal();
}


void
StColorPenState::SetGrayPattern()
{
	Pattern	grayPat;
	::PenPat(UQDGlobals::GetGrayPat(&grayPat));
}

#pragma mark -

// ===========================================================================
//	StTextState
// ===========================================================================
//
//	Text State Info:
//		Font Number
//		Text Size
//		Text Style
//		Text Mode

StTextState::StTextState()
{
	Save();
}


StTextState::~StTextState()
{
	Restore();
}


void
StTextState::Save()
{
	GrafPtr		currPort = UQDGlobals::GetCurrentPort();

	mFontNumber	= ::GetPortTextFont(currPort);
	mSize		= ::GetPortTextSize(currPort);
	mStyle		= ::GetPortTextFace(currPort);
	mMode		= ::GetPortTextMode(currPort);
}


void
StTextState::Restore()
{
	::TextFont(mFontNumber);
	::TextSize(mSize);
	::TextFace(mStyle);
	::TextMode(mMode);
}


void
StTextState::Normalize()
{
	::TextFont(systemFont);
	::TextSize(0);
	::TextFace(0);
	::TextMode(srcOr);
}

#pragma mark -

// ===========================================================================
//	StClipRgnState
// ===========================================================================
//
//	ClipRgn State Info:
//		Clipping Region

StClipRgnState::StClipRgnState()
{
	Save();
}


StClipRgnState::StClipRgnState(
	const Rect&	inClipRect)
{
	Save();
	::ClipRect(&inClipRect);
}


StClipRgnState::StClipRgnState(
	RgnHandle	inClipRgnH)
{
	Save();

	if (inClipRgnH != nil) {
		::SetClip(inClipRgnH);
	} else {
		::ClipRect(&Rect_0000);
	}
}


StClipRgnState::~StClipRgnState()
{
	Restore();
}


// ---------------------------------------------------------------------------
//	¥ ClipToIntersection
// ---------------------------------------------------------------------------
//	Sets the clipping region of the current port to the intersection of
//	the saved clip region and the specified region/rectangle

void
StClipRgnState::ClipToIntersection(
	RgnHandle	inRgnH)
{
	StRegion	overlap;
	::SectRgn(mClipRgn, inRgnH, overlap);
	::SetClip(overlap);
}


void
StClipRgnState::ClipToIntersection(
	const Rect	&inRect)
{
	StRegion	rgn(inRect);
	ClipToIntersection(rgn);
}


// ---------------------------------------------------------------------------
//	¥ ClipToDifference
// ---------------------------------------------------------------------------
//	Sets the clipping region of the current port to the difference of
//	the saved clip region and the specified region/rectangle

void
StClipRgnState::ClipToDifference(
	RgnHandle	inRgnH)
{
	StRegion	difference;
	::DiffRgn(mClipRgn, inRgnH, difference);
	::SetClip(difference);
}


void
StClipRgnState::ClipToDifference(
	const Rect	&inRect)
{
	StRegion	rgn(inRect);
	ClipToDifference(rgn);
}


void
StClipRgnState::Normalize()
{
	Rect	wideOpen = { min_Int16, min_Int16, max_Int16, max_Int16 };
	::ClipRect(&wideOpen);
}


// ---------------------------------------------------------------------------
//	¥ SectWithCurrent
// ---------------------------------------------------------------------------
//	Sets the clipping region of the current port to the intersection of
//	the current clip region and the specified region/rectangle

void
StClipRgnState::SectWithCurrent(
	RgnHandle	inRgnH)
{
	StRegion	clipRgn;
	::GetClip(clipRgn);
	::SectRgn(clipRgn, inRgnH, clipRgn);
	::SetClip(clipRgn);
}


void
StClipRgnState::SectWithCurrent(
	const Rect	&inRect)
{
	StRegion	rgn(inRect);
	SectWithCurrent(rgn);
}


// ---------------------------------------------------------------------------
//	¥ DiffWithCurrent
// ---------------------------------------------------------------------------
//	Sets the clipping region of the current port to the difference of
//	the current clip region and the specified region/rectangle

void
StClipRgnState::DiffWithCurrent(
	RgnHandle	inRgnH)
{
	StRegion	clipRgn;
	::GetClip(clipRgn);
	::DiffRgn(clipRgn, inRgnH, clipRgn);
	::SetClip(clipRgn);
}


void
StClipRgnState::DiffWithCurrent(
	const Rect	&inRect)
{
	StRegion	rgn(inRect);
	DiffWithCurrent(rgn);
}

#pragma mark -

// ===========================================================================
//	SOriginState
// ===========================================================================

StOriginState::StOriginState()
{
	Save();
}


StOriginState::~StOriginState()
{
	Restore();
}


void
StOriginState::Save()
{
	GrafPtr	currPort;
	::GetPort(&currPort);

	#if ACCESSOR_CALLS_ARE_FUNCTIONS

		Rect	portRect;
		::GetPortBounds(currPort, &portRect);
		mOrigin = topLeft(portRect);

	#else

		mOrigin = topLeft(currPort->portRect);

	#endif
}


void
StOriginState::Restore()
{
	::SetOrigin(mOrigin.h, mOrigin.v);
}

#pragma mark -

// ===========================================================================
//	StClipOriginState
// ===========================================================================
//
//	Clip Origin State Info
//		Graf Port
//		Port Origin
//		Clipping Region
//
//	The Toolbox call ::SetOrigin() does not move the clipping region. The
//	SetOrigin() call in this class offsets the clipping region so that the
//	same area of the port is clipped.

StClipOriginState::StClipOriginState()
{
	Save();
}


StClipOriginState::StClipOriginState(
	const Point&	inNewOrigin)
{
	Save();
	SetOrigin(inNewOrigin);
}


StClipOriginState::~StClipOriginState()
{
	Restore();
}


void
StClipOriginState::Save()
{
	mGrafPtr = UQDGlobals::GetCurrentPort();

	Rect	portRect;
	::GetPortBounds(mGrafPtr, &portRect);
	mOrigin = topLeft(portRect);
	
	::GetClip(mClipRgn);
}


void
StClipOriginState::Restore()
{
	::MacSetPort(mGrafPtr);
	::SetOrigin(mOrigin.h, mOrigin.v);
	::SetClip(mClipRgn);
}


void
StClipOriginState::SetOrigin(
	const Point&	inNewOrigin)
{
	::MacSetPort(mGrafPtr);
	
	Rect	portRect;
	::GetPortBounds(mGrafPtr, &portRect);
	
	StRegion	clipRgn;
	::GetClip(clipRgn);
	
	::SetOrigin(inNewOrigin.h, inNewOrigin.v);
	
	clipRgn.OffsetBy( (SInt16) (inNewOrigin.h - portRect.left),
					  (SInt16) (inNewOrigin.v - portRect.top) );
	::SetClip(clipRgn);
}

#pragma mark -

// ===========================================================================
//	StPortToggler
// ===========================================================================
//
//	Constructor saves the current port (called the original port) and sets
//	the port to the input port (called the toggle port)
//	
//	Desstruotor restores the current port to the original port

StPortToggler::StPortToggler(
	GrafPtr		inTogglePort)
{
	::GetPort(&mOriginalPort);			// Save original current port
	
	mTogglePort  = inTogglePort;
	if (inTogglePort == nil) {			// Consider nil toggle port to mean
		mTogglePort = mOriginalPort;	//   the original port
	}
	
	if (mTogglePort != mOriginalPort) {
		::MacSetPort(mTogglePort);		// Set current port to toggle port
	}
}


StPortToggler::~StPortToggler()
{
	::MacSetPort(mOriginalPort);		// Restore original port
}

#pragma mark -

// ===========================================================================
//	StPortOriginState
// ===========================================================================
//
//	The constructor saves the current port, sets the port to the
//	input port, then saves the port origin of the input port.
//	If the input port is nil, it is considered to mean the current port.
//
//	The destructor restores the origin of the input port, then sets
//	the current port to the port that was current when the constructor
//	was called.

StPortOriginState::StPortOriginState(
	GrafPtr		inGrafPtr)
	
	: mPortToggler(inGrafPtr)
{
}


StPortOriginState::~StPortOriginState()
{
	mPortToggler.EstablishTogglePort();
	
		// Destructors of member objects will:
		//		Restore origin of toggle port
		//		Restore original current port
}


void
StPortOriginState::Restore()
{
	mPortToggler.EstablishTogglePort();
	mOrigin.Restore();
	mPortToggler.EstablishOriginalPort();
}

#pragma mark -

// ===========================================================================
//	StColorPortState
// ===========================================================================
//
//	The constructor saves the current port, sets the port to the
//	input port, then saves the following data for the new port:
//
//		Port Origin
//		ColorPen State
//		Text State
//		Clipping Region
//
//	If the input port is nil, it is considered to mean the current port.
//
//	The destructor restores the stae information of the input port,
//	then sets the current port to the port that was current when the
//	constructor was called.

StColorPortState::StColorPortState(
	GrafPtr		inGrafPtr)
	
	: mPortToggler(inGrafPtr)
{
}


StColorPortState::~StColorPortState()
{
	mPortToggler.EstablishTogglePort();
	
		// Destructors for member objects will:
		//		Restore properties of toggle port
		//		Restore original current port
}


void
StColorPortState::Restore()
{
	mPortToggler.EstablishTogglePort();

	mOriginState.Restore();
	mColorPenState.Restore();
	mTextState.Restore();
	mClipRgnState.Restore();
	
	mPortToggler.EstablishOriginalPort();
}


void
StColorPortState::Normalize()
{
	StOriginState::Normalize();
	StColorPenState::Normalize();
	StTextState::Normalize();
	StClipRgnState::Normalize();
}

#pragma mark -

// ===========================================================================
//	StThemeDrawingState
// ===========================================================================

StThemeDrawingState::StThemeDrawingState()
{
	UAppearance::GetThemeDrawingState(&mThemeDrawingState);
}


StThemeDrawingState::StThemeDrawingState(
	ThemeDrawingState	inState)
{
	UAppearance::GetThemeDrawingState(&mThemeDrawingState);
	
	UAppearance::SetThemeDrawingState(inState, false);
}


StThemeDrawingState::~StThemeDrawingState()
{
	if (mThemeDrawingState != nil) {
		UAppearance::SetThemeDrawingState(mThemeDrawingState, true);
	}
}


void
StThemeDrawingState::Normalize()
{
	UAppearance::NormalizeThemeDrawingState();
}

#pragma mark -

// ===========================================================================
//	StHidePen
// ===========================================================================
//
//	Constructor (inline) hides pen. Destructor shows pen.

StHidePen::~StHidePen()
{
	::ShowPen();
}

#pragma mark -

// ===========================================================================
//	StVisibleRgn
// ===========================================================================
//
//	Constructor saves the current visible region of a GrafPort
//	Destructor restores the saved visible region
//
//	Manipulators change the visible region to the intersection or difference
//	with another region
//
//	Alternate contructors save and change to an intersection

// ---------------------------------------------------------------------------
//	¥ StVisibleRgn							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Save the current visible region of the specified port

StVisibleRgn::StVisibleRgn(
	GrafPtr		inPort)
{
	mGrafPtr = inPort;
	
	Save();
}


// ---------------------------------------------------------------------------
//	¥ StVisibleRgn							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Save the current visible region of the specified port, and set the new
//	visible region to the intersection of the current one and the input region
//
//	Caller retains ownership of the input region, which is not changed
//
//	inRgnH may be nil, which means an empty region. This suppresses all
//	drawing in the port.

StVisibleRgn::StVisibleRgn(
	GrafPtr		inPort,
	RgnHandle	inRgnH)
{
	mGrafPtr = inPort;
	
	Save();
	SectWithCurrent(inRgnH);
}


// ---------------------------------------------------------------------------
//	¥ StVisibleRgn							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Save the current visible region of the specified port, and set the new
//	visible region to the intersection of the current one and the input
//	rectangle

StVisibleRgn::StVisibleRgn(
	GrafPtr		inPort,
	const Rect&	inRect)
{
	mGrafPtr = inPort;
	
	Save();
	SectWithCurrent(StRegion(inRect));
}


// ---------------------------------------------------------------------------
//	¥ StVisibleRgn							Destructor				  [public]
// ---------------------------------------------------------------------------
//	Restores the visible region to that saved upon construction

StVisibleRgn::~StVisibleRgn()
{
	Restore();
}


// ---------------------------------------------------------------------------
//	¥ SectWithSaved													  [public]
// ---------------------------------------------------------------------------
//	Set port's visible region to the intersection of the saved visible region
//	and the specified region
//
//	Caller retains ownership of the input region, which is not changed
//	by this function
//
//	inRgnH may be nil, which means an empty region

void
StVisibleRgn::SectWithSaved(
	RgnHandle	inRgnH)
{
	if (mGrafPtr != nil) {
	
		StRegion	newVisRgn;		// Start with an empty region
		
		if (inRgnH != nil) {
			newVisRgn = mSaveVisRgn;	// Copy saved visRgn and convert
										//   to local coordinates
			Rect	portBounds;
			::GetPortBounds(mGrafPtr, &portBounds);
			
			newVisRgn.OffsetBy(portBounds.left, portBounds.top);
			
			newVisRgn &= inRgnH;		// Intersect with input region
		}
									// Change port's visRgn
		::SetPortVisibleRegion(mGrafPtr, newVisRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ SectWithCurrent												  [public]
// ---------------------------------------------------------------------------
//	Set port's visible region to the intersection of the current visible
//	region and the specified region
//
//	Caller retains ownership of the input region, which is not changed
//	by this function
//
//	inRgnH may be nil, which means an empty region

void
StVisibleRgn::SectWithCurrent(
	RgnHandle	inRgnH)
{
	if (mGrafPtr != nil) {
	
		StRegion	newVisRgn;		// Start with an empty region
		
		if (inRgnH != nil) {		// Input region exists, so intersect
									//   it with the current visRgn
			::GetPortVisibleRegion(mGrafPtr, newVisRgn);
			newVisRgn &= inRgnH;
		}
									// Change port's visRgn
		::SetPortVisibleRegion(mGrafPtr, newVisRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ DiffWithSaved													  [public]
// ---------------------------------------------------------------------------
//	Set port's visible region to the difference between the saved visible
//	region and the specified region
//
//	Caller retains ownership of the input region, which is not changed
//	by this function
//
//	inRgnH may be nil, which means an empty region

void
StVisibleRgn::DiffWithSaved(
	RgnHandle	inRgnH)
{
	if ( (mGrafPtr != nil) && (inRgnH != nil) ) {
	
		StRegion	newVisRgn(mSaveVisRgn);		// Copy saved visRgn
		
		Rect	portBounds;						// Convert to local coordinates
		::GetPortBounds(mGrafPtr, &portBounds);
		
		newVisRgn.OffsetBy(portBounds.left, portBounds.top);
		
		newVisRgn -= inRgnH;					// Remove input region

		::SetPortVisibleRegion(mGrafPtr, newVisRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ DiffWithCurrent												  [public]
// ---------------------------------------------------------------------------
//	Set port's visible region to the difference between the current visible
//	region and the specified region
//
//	Caller retains ownership of the input region, which is not changed
//	by this function
//
//	inRgnH may be nil, which means an empty region

void
StVisibleRgn::DiffWithCurrent(
	RgnHandle	inRgnH)
{
	if ( (mGrafPtr != nil) && (inRgnH != nil) ) {
	
		StRegion	newVisRgn;					// Copy current visRgn
		::GetPortVisibleRegion(mGrafPtr, newVisRgn);
		
		newVisRgn -= inRgnH;					// Remove input region

		::SetPortVisibleRegion(mGrafPtr, newVisRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ Restore														  [public]
// ---------------------------------------------------------------------------
//	Restore port's visible region to the saved one

void
StVisibleRgn::Restore()
{
	if (mGrafPtr != nil) {
	
			// Since the port origin may change between when we last
			// saved the visible region, we always use normalized
			// coordinates [origin at (0, 0)] when saving and restoring.
		
		StPortOriginState saveOrigin(mGrafPtr);
		StPortOriginState::Normalize();
		
		::SetPortVisibleRegion(mGrafPtr, mSaveVisRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ Save															 [private]
// ---------------------------------------------------------------------------
//	Save a copy of the visible region of this object's port

void
StVisibleRgn::Save()
{
	if (mGrafPtr != nil) {
	
			// Since the port origin may change between when we save
			// and later restore the visible region, we always use
			// normalized coordinates [origin at (0, 0)] when saving
			// and restoring.
		
		StPortOriginState saveOrigin(mGrafPtr);
		StPortOriginState::Normalize();
		
		::GetPortVisibleRegion(mGrafPtr, mSaveVisRgn);
	}
}

#pragma mark -

// ===========================================================================
//	StCQDProcsState
// ===========================================================================
//
//	Constructor sets the QD drawing procs for a Port (if inSetProcs is true)
//	Destructor restores original QD drawing procs

StCQDProcsState::StCQDProcsState(
	bool		inSetProcs,
	CQDProcs*	inCQDProcs,
	CGrafPtr	inPort)
{
	mSetProcs	= inSetProcs;
	mPort		= inPort;
	mCQDProcs	= nil;

	if (inPort != nil) {

		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			mCQDProcs	= ::GetPortGrafProcs(inPort);
		#else
			mCQDProcs	= inPort->grafProcs;
		#endif

		SetProcs(inSetProcs, inCQDProcs);
	}
}


StCQDProcsState::~StCQDProcsState()
{
	ResetProcs();
}


void
StCQDProcsState::SetProcs(
	bool		inSetProcs,
	CQDProcs*	inCQDProcs)
{
	if (inSetProcs && (mPort != nil)) {

		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			::SetPortGrafProcs(mPort, inCQDProcs);
		#else
			mPort->grafProcs = inCQDProcs;
		#endif

		mSetProcs = true;
	}
}


void
StCQDProcsState::ResetProcs()
{
	if (mSetProcs && (mPort != nil)) {

		#if ACCESSOR_CALLS_ARE_FUNCTIONS
			::SetPortGrafProcs(mPort, mCQDProcs);
		#else
			mPort->grafProcs = mCQDProcs;
		#endif

		mSetProcs = false;
	}
}

#pragma mark -

// ===========================================================================
//	StGDeviceSaver
// ===========================================================================
//
//	Constructor gets the current CGrafPtr and GDevice
//	Destructor restores them

StGDeviceSaver::StGDeviceSaver()
{
	::GetGWorld(&mSavePort, &mSaveDevice);
}


StGDeviceSaver::~StGDeviceSaver()
{
	::SetGWorld(mSavePort, mSaveDevice);
}


PP_End_Namespace_PowerPlant
