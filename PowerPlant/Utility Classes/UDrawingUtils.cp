// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDrawingUtils.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDrawingUtils.h>
#include <LString.h>
#include <LTimerTaskFunctor.h>
#include <UEnvironment.h>
#include <UEventMgr.h>

#include <Events.h>
#include <FixMath.h>
#include <Folders.h>
#include <Gestalt.h>
#include <LowMem.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================
// ¥ UDrawingUtils											   UDrawingUtils ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ IsColorGrafPort
// ---------------------------------------------------------------------------
//	Return whether a Toolbox GrafPort supports color

bool
UDrawingUtils::IsColorGrafPort(
	GrafPtr		inMacPort)
{
	return ::IsPortColor(inMacPort);
}


// ---------------------------------------------------------------------------
//	¥ GetPortPixelDepth
// ---------------------------------------------------------------------------
//	Return the number of bits per pixel for a GrafPort

SInt16
UDrawingUtils::GetPortPixelDepth(
	GrafPtr		inMacPort)
{
	SInt16	depth = 1;				// Black & white is 1 bit per pixel

	PixMapHandle	pixMapH = nil;	// Only color ports have PixMapHandles
	if (::IsPortColor(inMacPort)) {
		pixMapH = ::GetPortPixMap(reinterpret_cast<CGrafPtr>(inMacPort));
	}
	
	if (pixMapH != nil) {
		depth = ::GetPixDepth(pixMapH);
	}

	return depth;
}


// ---------------------------------------------------------------------------
//	¥ IsActiveScreenDevice
// ---------------------------------------------------------------------------
//	Return whether a GDevice represents an active screen

bool
UDrawingUtils::IsActiveScreenDevice(
	GDHandle	inDevice)
{
	return (::TestDeviceAttribute(inDevice, screenDevice) &&
			::TestDeviceAttribute(inDevice, screenActive));
}


// ---------------------------------------------------------------------------
//	¥ DeviceSupportsColor
// ---------------------------------------------------------------------------
//	Return whether a GDevice supports color

bool
UDrawingUtils::DeviceSupportsColor(
	GDHandle	inDevice)
{
	return ::TestDeviceAttribute(inDevice, gdDevType);
}


// ---------------------------------------------------------------------------
//	¥ SetHiliteModeOn
// ---------------------------------------------------------------------------
//	Turn on the QuickDraw hilite mode so that the next draw of an inverted
//	shape occurs in hilite mode

void
UDrawingUtils::SetHiliteModeOn()
{
	LMSetHiliteMode((UInt8) (LMGetHiliteMode() & (~(1 << hiliteBit))));
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsPortBuffered
// ---------------------------------------------------------------------------
//	Return whether the Port has an offscreen buffer

bool
UDrawingUtils::IsPortBuffered(
	GrafPtr		inMacPort)
{
	#if TARGET_API_MAC_CARBON
	
		return ::QDIsPortBuffered(inMacPort);
		
	#else
	
		#pragma unused(inMacPort)
		
		return false;			// Port is never buffered in Classic
	
	#endif
}


// ---------------------------------------------------------------------------
//	¥ FlushPortBuffer
// ---------------------------------------------------------------------------
//	Flush a region of the Port's buffer to the screen

void
UDrawingUtils::FlushPortBuffer(
	GrafPtr		inMacPort,
	RgnHandle	inRegion)				// In Port coordinates
{
	#if TARGET_API_MAC_CARBON
	
		StPortOriginState	saveState(inMacPort);
		::SetOrigin(0, 0);
		::QDFlushPortBuffer(inMacPort, inRegion);
		
	#else
								// Port is never buffered in Classic
		#pragma unused(inMacPort, inRegion)
	
	#endif
}


// ---------------------------------------------------------------------------
//	¥ FlushPortBuffer
// ---------------------------------------------------------------------------
//	Flush a rectangle of the Port's buffer to the screen

void
UDrawingUtils::FlushPortBuffer(
	GrafPtr		inMacPort,
	const Rect&	inRect)					// In Port coordinates
{
	#if TARGET_API_MAC_CARBON
	
		if (IsPortBuffered(inMacPort)) {
			StPortOriginState	saveState(inMacPort);
			::SetOrigin(0, 0);
			::QDFlushPortBuffer(inMacPort, StRegion(inRect));
		}
		
	#else
								// Port is never buffered in Classic
		#pragma unused(inMacPort, inRect)
	
	#endif
}

#pragma mark -

// ===========================================================================
// ¥ StColorDrawLoop										 StColorDrawLoop ¥
// ===========================================================================
//	Class for coordinating drawing in an area which may span multiple
//	devices (monitors) with different bit depths.
//
//	How to use:
//		You will usually create an StColorDrawLoop on the stack inside
//		a function which draws. This implementation is not dependent
//		on PowerPlant Panes, so you can use this class with any drawing
//		code.
//
//		The constructor takes a Rect in the local coordinates of the
//		current port.
//
//		The NextDepth(SInt16 depth) function passes back the depth of the
//		next device and returns true. After reaching the last device,
//		the depth is zero and the function returns false. You would normally
//		call NextDepth() as the condition in a "while" loop.
//
//		When NextDepth() returns with a valid depth, it has already set
//		the clipping region to the intersection of the specified Rect, the
//		current device, and the original clipping region. Therefore, you can
//		just draw "everything" and rely on the clipping region to properly
//		restrict the drawing. If you need access to the current device,
//		you can call the GetCurrentDevice() member function.
//
//		The destructor restores the clipping region to what it was when
//		the constructor was called.
//
//	Example:
//
//		The body of a DrawSelf() function would look something like this:
//
//		Rect	frame;
//		if (CalcLocalFrameRect(frame)) {
//			StDeviceLoop	theLoop(frame);
//			SInt16			depth;
//			while (theLoop.NextDepth(depth)) {
//				switch (depth) {
//
//					case 1:		// Black & white
//						break;
//
//					case 4:		// 16 colors
//						break;
//
//					case 8:		// 256 colors
//						break;
//
//					case 16:	// Thousands of colors
//						break;
//
//					case 32:	// Millions of colors
//						break;
//				}
//			}
//		}
//
//		You would add the appropriate drawing code to each depth case.

StColorDrawLoop::StColorDrawLoop(
	const Rect	&inLocalRect)
{
	mGlobalRect = inLocalRect;			// Convert to Global coords
	::LocalToGlobal(&topLeft(mGlobalRect));
	::LocalToGlobal(&botRight(mGlobalRect));

	mCurrentDevice = nil;
}


StColorDrawLoop::~StColorDrawLoop()
{
}


// ---------------------------------------------------------------------------
//	¥ NextDepth														  [public]
// ---------------------------------------------------------------------------
//	Move to Next Device in drawing loop
//
//	Pass back bit depth of the Device
//	Return false, and set bit depth to 0, if there are no more devices

bool
StColorDrawLoop::NextDepth(
	SInt16	&outDepth)
{
	bool	stillLooping = false;

	GrafPtr	port = UQDGlobals::GetCurrentPort();

	if ( UDrawingUtils::IsPortBuffered(port) ) {
	
			// For buffered ports, we don't need to loop over the screen
			// devices. We just need to draw once at the bit depth of
			// the buffer.
			//
			// On the first call, mCurrentDevice is nil. We pass back the
			// bit depth of the buffer, set mCurrentDevice to the main
			// device, and return true.
			//
			// On subsequent calls (detected by mCurrentDevice not being
			// nil), we pass back a depth of zero and return false.
		
		outDepth = 0;
		if (mCurrentDevice == nil) {
			outDepth = UDrawingUtils::GetPortPixelDepth(port);
			mCurrentDevice = ::GetMainDevice();
			stillLooping = true;
		}
	
	} else {							// Non-buffered port
										// Do the normal device loop
		stillLooping = NextDevice(outDepth);
		
	}
	
	return stillLooping;
}


// ---------------------------------------------------------------------------
//	¥ NextDevice												   [protected]
// ---------------------------------------------------------------------------
//	Move to Next Device in drawing loop
//
//	Pass back bit depth of the Device
//	Return false, and set bit depth to 0, if there are no more devices

bool
StColorDrawLoop::NextDevice(
	SInt16	&outDepth)
{
	outDepth = 0;

	if (mCurrentDevice == nil) {		// First call. Start at beginning
										//   of GDevice list.
		mCurrentDevice = ::GetDeviceList();

	} else {							// Subsequent call
		mCurrentDevice = ::GetNextDevice(mCurrentDevice);
	}

	while (mCurrentDevice != nil) {

			// Find active screen device that intersects our
			// target rectangle

		if (UDrawingUtils::IsActiveScreenDevice(mCurrentDevice)) {
			Rect	deviceRect = (**mCurrentDevice).gdRect;
			Rect	intersection;

			if (::SectRect(&mGlobalRect, &deviceRect, &intersection)) {

					// Set clipping region to the intersection of the target
					// rectangle, the screen rectangle, and the original
					// clipping region

				::GlobalToLocal(&topLeft(intersection));
				::GlobalToLocal(&botRight(intersection));
				mClipState.ClipToIntersection(intersection);
				outDepth = (**((**mCurrentDevice).gdPMap)).pixelSize;
				break;				// Exit device loop
			}
		}

		mCurrentDevice = ::GetNextDevice(mCurrentDevice);
	}

	return (mCurrentDevice != nil);
}

#pragma mark -

// ===========================================================================
// ¥ StDeviceLoop												StDeviceLoop ¥
// ===========================================================================
//	Subclass of StColorDrawLoop that works with both Color and old-style
//	Black and White QuickDraw.

StDeviceLoop::StDeviceLoop(
	const Rect	&inLocalRect)
		: StColorDrawLoop(inLocalRect)
{
										// Check if machine has ColorQD
	SInt32		qdVersion = gestaltOriginalQD;
	::Gestalt(gestaltQuickdrawVersion, &qdVersion);
	mHasOldQD = (qdVersion == gestaltOriginalQD);
	mOldQDPending = true;
}


StDeviceLoop::~StDeviceLoop()
{
}


bool
StDeviceLoop::NextDepth(
	SInt16	&outDepth)
{
	outDepth = 0;

	if (mHasOldQD) {					// No ColorQD, so there can only
		if (mOldQDPending) {			//   be 1-bit pixel depth
			Rect	localRect = mGlobalRect;
			::GlobalToLocal(&topLeft(localRect));
			::GlobalToLocal(&botRight(localRect));
			::ClipRect(&localRect);
			outDepth = 1;
			mOldQDPending = false;
			return true;
		}
		return false;
	}

										// ColorQD exists, so call
										//   inherited function
	return StColorDrawLoop::NextDepth(outDepth);
}

#pragma mark -

// ===========================================================================
// ¥ UMarchingAnts											   UMarchingAnts ¥
// ===========================================================================
//	Utility functions for drawing marching ants, which is typically used
//	for an animated selection marquee.
//
//	Typically, you will use this class within a Pane that is also a
//	Periodical. For example, assuming your class maintains a Rect called
//	mSelRect that stores the current selection rectangle:
//
//	void CMyPaneWithSelection::SpendTime(const EventRecor&)
//	{
//		FocusDraw();
//		StColorState::Normalize();	// Draw in black and white
//
//			// Animate selection if this Pane is active and enough
//			// time has passed for the next march to occur
//
//		if (IsActive() && UMarchingAnts::BeginMarch()) {
//			::MacFrameRect(&mSelRect);
//			UMarchingAnts::EndMarch();
//		}
//	}
//
//	Ants appear to march because the UMarchingAnts class stores a pen pattern
//	that it rotates between successive calls.

Pattern	UMarchingAnts::sAntsPattern = {0xF8, 0xF1, 0xE3, 0xC7,
								  	   0x8F, 0x1F, 0x3E, 0x7C};
UInt32	UMarchingAnts::sNextAntMarchTime = 0;

const SInt16		interval_AntMarch = 3;


Boolean
UMarchingAnts::BeginMarch()
{
	Boolean	marching = (::TickCount() >= sNextAntMarchTime);

	if (marching) {
		UseAntsPattern();
	}

	return marching;
}


void
UMarchingAnts::EndMarch()
{
	sNextAntMarchTime = ::TickCount() + interval_AntMarch;

	unsigned char	lastRow = sAntsPattern.pat[7];
	for (SInt16 i = 7; i > 0; i--) {
		sAntsPattern.pat[i] = sAntsPattern.pat[i - 1];
	}
	sAntsPattern.pat[0] = lastRow;
}


void
UMarchingAnts::UseAntsPattern()
{
	::PenNormal();
	::PenPat(&sAntsPattern);
}

#pragma mark -

// ===========================================================================
// ¥ LMarchingAnts											   LMarchingAnts ¥
// ===========================================================================
//	Utility class for drawing marching ants, which is typically used for an
//	animated selection marquee.
//
//	This class is similar to UMarchingAnts, except that this class stores
//	its data in member variables rather than static class variables.
//	Therefore, you can have multiple instances of LMarchingAnts, whereas
//	a program can only use UMarchingAnts for one thing at a time.

LMarchingAnts::LMarchingAnts(
	UInt32		inMarchInterval)
{
	mMarchInterval		= inMarchInterval;
	mNextAntMarchTime	= 0;

								// Ants Pattern is a diagonal stripe

	mAntsPattern.pat[0]	= 0xF8;		//  1 1 1 1 1 0 0 0
	mAntsPattern.pat[1]	= 0xF1;		//  1 1 1 1 0 0 0 1
	mAntsPattern.pat[2]	= 0xE3;		//  1 1 1 0 0 0 1 1
	mAntsPattern.pat[3]	= 0xC7;		//  1 1 0 0 0 1 1 1
	mAntsPattern.pat[4]	= 0x8F;		//	1 0 0 0 1 1 1 1
	mAntsPattern.pat[5]	= 0x1F;		//	0 0 0 1 1 1 1 1
	mAntsPattern.pat[6]	= 0x3E;		//	0 0 1 1 1 1 1 0
	mAntsPattern.pat[7]	= 0x7C;		//	0 1 1 1 1 1 0 0
}


LMarchingAnts::~LMarchingAnts()
{
}


Boolean
LMarchingAnts::BeginMarch()
{
	Boolean	marching = (::TickCount() >= mNextAntMarchTime);

	if (marching) {
		UseAntsPattern();
	}

	return marching;
}


void
LMarchingAnts::EndMarch()
{
	mNextAntMarchTime = ::TickCount() + mMarchInterval;

		// Rotate bytes of Pattern to move the ants

	unsigned char	lastRow = mAntsPattern.pat[7];
	for (SInt16 i = 7; i > 0; i--) {
		mAntsPattern.pat[i] = mAntsPattern.pat[i - 1];
	}
	mAntsPattern.pat[0] = lastRow;
}


void
LMarchingAnts::UseAntsPattern()
{
	::PenNormal();
	::PenPat(&mAntsPattern);
}

#pragma mark -

// ===========================================================================
// ¥ UTextDrawing												UTextDrawing ¥
// ===========================================================================
//	Utility functions for drawing text

// ---------------------------------------------------------------------------
//	¥ DrawWithJustification	
// ---------------------------------------------------------------------------
//	Draw justified text within a rectangle
//
//	This is a "better" version of the Toolbox TextBox() routine that does
//	not erase before it draws.

void
UTextDrawing::DrawWithJustification(
	Ptr				inText,
	SInt32			inLength,
	const Rect&		inRect,
	SInt16			inJustification,
	bool			inFirstLeading)		// Add leading above first line?
{
	FontInfo	fontInfo;				// Determine vertical line spacing
	::GetFontInfo(&fontInfo);			//   based on font characteristics

	SInt16 lineHeight = (SInt16) (fontInfo.ascent + fontInfo.descent + fontInfo.leading);
	SInt16 lineBase   = (SInt16) (inRect.top + fontInfo.ascent);

	if (inFirstLeading) {
		lineBase += fontInfo.leading;
	}

										// Get pixel width of a line a text
										//   in both int and fixed format
	SInt16		intWidth   = (SInt16) (inRect.right - inRect.left);
	Fixed		fixedWidth = ::Long2Fix(intWidth);

										// Get text justification option
	SInt16	justification = GetActualJustification(inJustification);

	StClipRgnState	saveClip;			// Draw within input rectangle
	saveClip.ClipToIntersection(inRect);

	SInt32	textLeft = inLength;
	Ptr		textEnd  = inText + inLength;

	while ((inText < textEnd) && (lineBase <= inRect.bottom)) {

			// IM:Text 5-50 states that the StyledLineBreak call
			// only handles widths up to the [maximum short] integer
			// range. So we limit things accordingly.

		SInt32	textLen = textLeft;
		if (textLen > max_Int16) {
			textLen = max_Int16;
		}

		SInt32	lineBytes = textLen;
		Fixed	wrapWidth = fixedWidth;

			// StyledLineBreak will pass back the number of bytes
			// of text that we should draw on this line

		::StyledLineBreak(
							inText, textLen,
							0, textLen, 0,
							&wrapWidth, &lineBytes);

										// Determine location to start text
										//   line based on justification

		if ( (justification == teFlushLeft) ||
			 (justification == teJustLeft) ) {
										// Text at left edge
			::MoveTo(inRect.left, lineBase);

		} else {						// Center or right justified

										// Determine length of text without
										//   spaces
			SInt32	blackSpace = ::VisibleLength(inText, lineBytes);
			SInt16	visWidth   = ::TextWidth(inText, 0, (SInt16) blackSpace);

			if (justification == teFlushRight) {
										// Text extends to right edge
				::MoveTo((SInt16) (inRect.right - visWidth), lineBase);

			} else {					// Horizontally centered text
				::MoveTo((SInt16) (inRect.left + (intWidth - visWidth) / 2),
							lineBase);
			}
		}
										// Draw one line of text
		::MacDrawText(inText, 0, (SInt16) lineBytes);

		lineBase += lineHeight;			// Move down for next line
		inText   += lineBytes;			// Move to first char of next line
		textLeft -= lineBytes;			// Decrement bytes of text remaining
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawTruncatedWithJust
// ---------------------------------------------------------------------------
//	Draw justified text within a rectangle that can optionally also be
// 	truncated if the text is longer than the rect width
//
//	This is similar to the function above except that it can only handle
//	drawing text on a single line, there is no support for line wrapping

void
UTextDrawing::DrawTruncatedWithJust(
	ConstStringPtr	inString,
  	const Rect&		inRect,
  	SInt16			inJustification,
	bool			inTruncateString,
  	bool	 		inPreferOutline,
  	TruncCode		inTruncWhere)
{
	LStr255		localString = inString;	// Copy string because truncation
										//  will change the string

										// Set the width of the text box
	Rect 	localBox = inRect;
	SInt16	boxWidth = (SInt16) (localBox.right - localBox.left);

										// TrueType or BitMap preferred?
										// Save original setting
	Boolean		saveOutline = ::GetOutlinePreferred();
	::SetOutlinePreferred(inPreferOutline);

										// Check if the string is too wide
	SInt16	widthOfString = ::StringWidth(localString);
	if (inTruncateString && (widthOfString > boxWidth)) {

										// Truncate string
		::TruncString(boxWidth, localString, inTruncWhere);
		widthOfString = ::StringWidth(localString);
	}

		// If the width of the string is less than the text box,
		// then set the justification. Otherwise it draw the text
		// at starting at the left edge of the box.

	if (widthOfString < boxWidth) {

		switch (GetActualJustification(inJustification)) {

			case teCenter:
				localBox.left += (SInt16) ((boxWidth - widthOfString) / 2);
				break;

			case teFlushRight:
				localBox.left += (SInt16) (boxWidth - widthOfString);
				break;
		}
	}
										// Finally, draw the string
	FontInfo 	theFontInfo;
	::GetFontInfo(&theFontInfo);
	::MoveTo(localBox.left, (SInt16) (localBox.top + theFontInfo.ascent));
	::DrawString(localString);
	
	::SetOutlinePreferred(saveOutline);	// Restore original setting
}


// ---------------------------------------------------------------------------
//	¥ DimTextColor
// ---------------------------------------------------------------------------

void
UTextDrawing::DimTextColor(
	RGBColor&	ioColor)
{
	ioColor.red   = (UInt16) ((ioColor.red   + 65535) >> 1);
	ioColor.green = (UInt16) ((ioColor.green + 65535) >> 1);
	ioColor.blue  = (UInt16) ((ioColor.blue  + 65535) >> 1);
}

#pragma mark -

// ===========================================================================
// ¥ UMouseTracking											  UMouseTracking ¥
// ===========================================================================
//	Routines for mosue down tracking

// ---------------------------------------------------------------------------

namespace UMouseTracking {

	// -----------------------------------------------------------------------
	//	Timer callback function and user data
	//
	//		To have a function called repeatedly while the mouse is down,
	//		we install an event loop timer (on Carbon 1.1 or later). The
	//		timer callback calls the user-specified track action function.
	
	struct	STrackActionData {
		TrackActionFunc		func;
		void*				userData;
	};

	static void TrackActionCallback(
		LTimerTask*	inTask)
	{
		STrackActionData*	data =
				static_cast<STrackActionData*>(inTask->GetUserData());
		
		if (data->func != nil) {
			(*data->func)(data->userData);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ TrackMouseDown
// ---------------------------------------------------------------------------
//	Track mouse while it is down, returning when the mouse moves or the
//	button is released
//
//		inPort - GrafPort used for mouse coordinates. Pass nil to use
//					the current port
//
//		outPoint - Mouse location
//
//		outResult - Kind of event which ended the tracking. May be:
//
//					kMouseTrackingMouseDragged
//					kMouseTrackingMouseReleased
//					kMouseTrackingKeyModifiersChanged (only on Carbon 1.1+)
//
//	On Carbon (v1.1 or later), this is a wrapper for the Toolbox function
//	::TrackMouseLocation(), which yields time to other processes. Otherwise,
//	we implement our own tracking loop.
//
//	Function consumes the mouse up event when returning
//	kMouseTrackingMouseReleased
//
//	Although this function calls through to ::TrackMouseLocation() if
//	present, it is not a complete replacement for that function. Our
//	implemention only works for mouse down tracking and does not check
//	if the modifier keys change. If you need those features on systems
//	where ::TrackMouseLocation() isn't present, you will need to write
//	your own implementation.
//
//	For modifier key changes (when ::TrackMouseLocation() isn't present),
//	you can use TrackMouseDownWithAction() [see below] and check the
//	modifier keys state in your Action function.

void
UMouseTracking::TrackMouseDown(
	GrafPtr					inPort,
	Point&					outPoint,
	MouseTrackingResult&	outResult)
{
#if TARGET_API_MAC_CARBON			// Use Toolbox routine if present

	if (CFM_AddressIsResolved_(TrackMouseLocation)) {

		::TrackMouseLocation(inPort, &outPoint, &outResult);
		return;
	}
	
#endif

	StGrafPortSaver	savePort(inPort);
	
	Point	startPt;
	::GetMouse(&startPt);
	
	Point	currPt;
									// Loop until the mouse moves
	do {							//   or the button is released
		::GetMouse(&currPt);
		
		if (::StillDown()) {
			if (not ::EqualPt(currPt, startPt)) {
										// Mouse is down and has moved
				outResult = kMouseTrackingMouseDragged;
				break;
			}
		
		} else {						// Mouse button is up
			EventRecord		event;		// Consume mouse up event
			if (UEventMgr::GetMouseUp(event)) {
				currPt = event.where;
				::GlobalToLocal(&currPt);
			}
			
			outResult = kMouseTrackingMouseReleased;
			break;
		}
		
	} while (true);
	
	outPoint = currPt;
}


// ---------------------------------------------------------------------------
//	¥ TrackMouseDownWithAction
// ---------------------------------------------------------------------------
//	Track mouse while it is down and continuously call the specified action
//	function, returning when the mouse moves or the button is released
//
//		inPort, outPoint, outResult parameters are the same as for
//		TrackMouseDown()
//
//		inActionFunc - Pointer to function to call while tracking. Signature:
//
//							void ActionFunc( void* userData );
//
//						Function pointer may be nil
//
//		inTickInterval - Ticks between calls to the Action function. If zero,
//							the Action function is called as often
//							as possible.
//
//		inUserData - Passed as a parameter to the Action function

void
UMouseTracking::TrackMouseDownWithAction(
	GrafPtr					inPort,
	Point&					outPoint,
	MouseTrackingResult&	outResult,
	TrackActionFunc			inActionFunc,
	UInt32					inTickInterval,
	void*					inUserData)
{
#if TARGET_API_MAC_CARBON			// Use Toolbox routine if present

	if (CFM_AddressIsResolved_(TrackMouseLocation)) {
	
		EventTimerInterval	interval = ::TicksToEventTime(inTickInterval);
		
		if (interval == 0) {		// For Timers, a zero interval means
									//   call just once. We interpret zero
									//   to mean as often as possbile, so
									//   make the interval very small.
			interval = kEventDurationNanosecond;
		}

		LTimerTaskFunctor	timer( ::GetCurrentEventLoop(),
								   interval, interval,
								   &TrackActionCallback );
								   
		STrackActionData	data;
		data.func = inActionFunc;
		data.userData = inUserData;
	
		timer.SetUserData(&data);
									
		::TrackMouseLocation(inPort, &outPoint, &outResult);
		return;
	}
	
#endif

	StGrafPortSaver	savePort(inPort);
	
	Point	startPt;
	::GetMouse(&startPt);
	
	Point	currPt;
	UInt32	lastCallTick = 0;
									// Loop until the mouse moves
	do {							//   or the button is released
		::GetMouse(&currPt);
		
		if (::StillDown()) {
			if (not ::EqualPt(currPt, startPt)) {
										// Mouse is down and has moved
				outResult = kMouseTrackingMouseDragged;
				break;
			}
		
		} else {						// Mouse button is up
			EventRecord		event;		// Consume mouse up event
			if (UEventMgr::GetMouseUp(event)) {
				currPt = event.where;
				::GlobalToLocal(&currPt);
			}
			
			outResult = kMouseTrackingMouseReleased;
			break;
		}
		
			// If we reach this point in the loop, it means the
			// mouse is still down and has not moved. Execute
			// callback function, taking the interval into account.
		
		if (inActionFunc != nil) {
		
			UInt32	ticks = ::TickCount();
			
			if (ticks >= lastCallTick + inTickInterval) {
				
				lastCallTick = ticks;
				(*inActionFunc)(inUserData);
			}
		}
		
	} while (true);
	
	outPoint = currPt;
}


// ---------------------------------------------------------------------------
//	TrackMouseDownInRegion
// ---------------------------------------------------------------------------
//	Track mouse while it is down, returning when the mouse exits the region,
//	enters the region, or the button is released
//
//		inPort - GrafPort used for mouse coordinates. Pass nil to use
//					the current port
//
//		inRegion - Region to consider while tracking
//
//		ioWasInRgn - Whether or not the mouse is inside the region
//						If calling this function in a loop, you should
//						initialize this value before the first call.
//						On subsequent calls, you can rely on this
//						function to have updated the value according
//						to the current mouse location.
//
//		outResult - Kind of event which ended the tracking. May be:
//
//					kMouseTrackingMouseExited
//					kMouseTrackingMouseEntered
//					kMouseTrackingMouseReleased
//					kMouseTrackingKeyModifiersChanged (only on Carbon 1.1+)
//
//	On Carbon (v1.1 or later), this is a wrapper for the Toolbox function
//	::TrackMouseRegion(), which yields time to other processes. Otherwise,
//	we implement our own tracking loop.
//
//	Function consumes the mouse up event when returning
//	kMouseTrackingMouseReleased
//
//	Although this function calls through to ::TrackMouseRegion() if
//	present, it is not a complete replacement for that function. Our
//	implemention only works for mouse down tracking and does not check
//	if the modifier keys change. If you need those features on systems
//	where ::TrackMouseRegion() isn't present, you will need to write
//	your own implementation.
//
//	For modifier key changes (when ::TrackMouseRegion() isn't present),
//	you can use TrackMouseDownInRegionWithAction() [see below] and check
//	the modifier keys state in your Action function.

void
UMouseTracking::TrackMouseDownInRegion(
	GrafPtr					inPort,
	RgnHandle				inRegion,
	Boolean&				ioWasInRgn,
	MouseTrackingResult&	outResult)
{
#if TARGET_API_MAC_CARBON			// Use Toolbox routine if present

	if (CFM_AddressIsResolved_(TrackMouseRegion)) {
		::TrackMouseRegion(inPort, inRegion, &ioWasInRgn, &outResult);
		return;
	}

#endif

	StGrafPortSaver	savePort(inPort);
	
	Point	startPt;
	::GetMouse(&startPt);
	
	Boolean	isInside;
									// Loop until the mouse crosses
									//   region boundary or the
	do {							//   button is released
		Point	currPt;
		::GetMouse(&currPt);
		
		if (::StillDown()) {
			isInside = ::PtInRgn(currPt, inRegion);
			
			if (isInside != ioWasInRgn) {
										// Mouse has gone in-to-out or
										//   out-to-in
				outResult = kMouseTrackingMouseExited;
				if (isInside) {
					outResult = kMouseTrackingMouseEntered;
				}
				break;
			}
			
		} else {						// Mouse button is up
			EventRecord		event;		// Consume mouse up event
			if (UEventMgr::GetMouseUp(event)) {
				currPt = event.where;
				::GlobalToLocal(&currPt);
			}
			
			isInside = ::PtInRgn(currPt, inRegion);
			outResult = kMouseTrackingMouseReleased;
			break;
		}
		
	} while (true);
	
	ioWasInRgn = isInside;
}


// ---------------------------------------------------------------------------
//	TrackMouseDownInRegionWithAction
// ---------------------------------------------------------------------------
//	Track mouse while it is down and continuously call the specified action
//	function, returning when the mouse exits the region, enters the region,
//	or the button is released
//
//		inPort, inRegion, ioWasInRgn, and outResult parameters are the same
//		as for TrackMouseDownInRegion()
//
//		ioActionFunc, inTickInterval, and inUserData parameters are the
//		same as for TrackMouseLocationWithAction()

void
UMouseTracking::TrackMouseDownInRegionWithAction(
	GrafPtr					inPort,
	RgnHandle				inRegion,
	Boolean&				ioWasInRgn,
	MouseTrackingResult&	outResult,
	TrackActionFunc			inActionFunc,
	UInt32					inTickInterval,
	void*					inUserData)
{
#if TARGET_API_MAC_CARBON			// Use Toolbox routine if present

	if (CFM_AddressIsResolved_(TrackMouseRegion)) {
	
		EventTimerInterval	interval = ::TicksToEventTime(inTickInterval);
		
		if (interval == 0) {		// For Timers, a zero interval means
									//   call just once. We interpret zero
									//   to mean as often as possbile, so
									//   make the interval very small.
			interval = kEventDurationNanosecond;
		}

		LTimerTaskFunctor	timer( ::GetCurrentEventLoop(),
								   interval, interval,
								   &TrackActionCallback );
								   
		STrackActionData	data;
		data.func = inActionFunc;
		data.userData = inUserData;
	
		timer.SetUserData(&data);
									
		::TrackMouseRegion(inPort, inRegion, &ioWasInRgn, &outResult);
		return;
	}

#endif

	StGrafPortSaver	savePort(inPort);
	
	Point	startPt;
	::GetMouse(&startPt);
	
	Boolean	isInside;
	UInt32	lastCallTick = 0;
									// Loop until the mouse crosses
									//   region boundary or the
	do {							//   button is released
		Point	currPt;
		::GetMouse(&currPt);
		
		if (::StillDown()) {
			isInside = ::PtInRgn(currPt, inRegion);
			
			if (isInside != ioWasInRgn) {
										// Mouse has gone in-to-out or
										//   out-to-in
				outResult = kMouseTrackingMouseExited;
				if (isInside) {
					outResult = kMouseTrackingMouseEntered;
				}
				break;
			}
			
		} else {						// Mouse button is up
			EventRecord		event;		// Consume mouse up event
			if (UEventMgr::GetMouseUp(event)) {
				currPt = event.where;
				::GlobalToLocal(&currPt);
			}
			
			isInside = ::PtInRgn(currPt, inRegion);
			outResult = kMouseTrackingMouseReleased;
			break;
		}
		
			// If we reach this point in the loop, it means the
			// mouse is still down and has not entered/exited the
			// region. Execute  callback function, taking the
			// interval into account.
		
		if (inActionFunc != nil) {
		
			UInt32	ticks = ::TickCount();
			
			if (ticks >= lastCallTick + inTickInterval) {
				
				lastCallTick = ticks;
				(*inActionFunc)(inUserData);
			}
		}
		
	} while (true);
	
	ioWasInRgn = isInside;
}


PP_End_Namespace_PowerPlant
