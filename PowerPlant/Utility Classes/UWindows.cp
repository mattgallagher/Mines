// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UWindows.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Window utility functions

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <UWindows.h>
#include <UDrawingUtils.h>
#include <UTBAccessors.h>

#include <LowMem.h>
#include <MacWindows.h>
#include <TextUtils.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt16	Screen_Margin = 6;		// Margin to leave around screen
										//   when positioning windows
										
const SInt16	Offset_CascadeLeft = 3;	// Pixels to offset horizontally
										//   when cascading windows
										
const SInt16	Offset_CascadeTop = 3;	// Pixels to offset vertically from
										//   title bar when cascading windows
										
const SInt16	Delta_CloseEnough = 3;	// Locations are considered the same
										//   if within this many pixels

// ---------------------------------------------------------------------------
//	¥ GetWindowContentRect
// ---------------------------------------------------------------------------
//	Return the bounding rectangle of the content region of the specified
//	Toolbox window

Rect
UWindows::GetWindowContentRect(
	WindowPtr	inWindowP)
{
	Rect	contentRect = Rect_0000;

	if (inWindowP != nil) {
	
		#if TARGET_API_MAC_CARBON
		
			::GetWindowBounds(inWindowP, kWindowContentRgn, &contentRect);
			
		#else
	
			if (MacIsWindowVisible(inWindowP)) {
									// For visible windows, the bounding
									//   box the content region is correct
				contentRect = ((**((WindowPeek) inWindowP)->contRgn)).rgnBBox;

			} else {					// For invisible windows, the content
										//   region is not valid. Instead, get
				GrafPtr	savePort;		//   the port rectangle and convert
				::GetPort(&savePort);	//   to global coordinates
				::SetPortWindowPort(inWindowP);
				::GetWindowPortBounds(inWindowP, &contentRect);
				::LocalToGlobal(&topLeft(contentRect));
				::LocalToGlobal(&botRight(contentRect));
				::MacSetPort(savePort);
			}
			
		#endif
	}

	return contentRect;
}


// ---------------------------------------------------------------------------
//	¥ GetWindowStructureRect
// ---------------------------------------------------------------------------
//	Return the bounding rectangle of the structure region of the specified
//	Toolbox window

Rect
UWindows::GetWindowStructureRect(
	WindowPtr	inWindowP)
{
	Rect			structureRect = Rect_0000;

	if (inWindowP != nil) {

		#if TARGET_API_MAC_CARBON

			::GetWindowBounds(inWindowP, kWindowStructureRgn, &structureRect);

		#else
			const SInt16	coord_Offscreen = 0x4000;

			if (MacIsWindowVisible(inWindowP)) {
										// For visible windows, the bounding
										//   box the structure region is correct
				structureRect = ((**((WindowPeek) inWindowP)->strucRgn)).rgnBBox;

			} else {					// For invisible windows, the structure
										//   region is not valid. We have to
										//   move the window offscreen, show it,
										//   copy bounds of structure region,
										//   hide it, and then move it back
				GrafPtr	savePort;
				::GetPort(&savePort);
				::MacSetPort(inWindowP);
										// Get current position in global coords
				Point	savePosition = topLeft(inWindowP->portRect);
				::LocalToGlobal(&savePosition);
				::MacSetPort(savePort);

				::MacMoveWindow(inWindowP, coord_Offscreen, coord_Offscreen, false);
				::ShowHide(inWindowP, true);

				structureRect = ((**((WindowPeek) inWindowP)->strucRgn)).rgnBBox;
				::MacOffsetRect(&structureRect, (SInt16) (savePosition.h - coord_Offscreen),
											 (SInt16) (savePosition.v - coord_Offscreen));

				::ShowHide(inWindowP, false);
				::MacMoveWindow(inWindowP, savePosition.h, savePosition.v, false);
			}

		#endif
	}

	return structureRect;
}


// ---------------------------------------------------------------------------
//	¥ FindDominantDevice
// ---------------------------------------------------------------------------
//	Return the GDevice of the active screen which contains the largest portion
//	of the specified rectangle.
//
//	Returns nil if the rectangle is completely offscreen

GDHandle
UWindows::FindDominantDevice(
	const Rect	&inGlobalRect)
{
	GDHandle	dominantDevice = nil;

									// Loop thru all GDevices to find the
									//   one which the greatest intersection
									//   area with inGlobalRect

	GDHandle	theDevice = ::GetDeviceList();
	SInt32		greatestArea = 0;

	while (theDevice != nil) {
									// Only check GDevices for Screens
		if (UDrawingUtils::IsActiveScreenDevice(theDevice)) {
			Rect	intersection;
									// Find intersection of Window with
									//   this active screen Device
			if (::SectRect(&inGlobalRect, &(**theDevice).gdRect,
						 &intersection)) {
									// Window intersects this Device
									// Calculate area of intersection
				SInt32	sectArea = (intersection.right - intersection.left) *
						   (SInt32) (intersection.bottom - intersection.top);

				if (sectArea > greatestArea) {
									// Intersection is greater than previous
									//    maximum
					greatestArea = sectArea;
					dominantDevice = theDevice;
				}
			}
		}
		theDevice = ::GetNextDevice(theDevice);
	}

	return dominantDevice;
}


// ---------------------------------------------------------------------------
//	¥ FindNthWindow
// ---------------------------------------------------------------------------
//	Return a WindowPtr to the Nth Window
//
//	Windows are ordered from front to back
//		For positive indexes, 1 is the front window, 2 is the second, etc.
//		For negative indexes, -1 is the last window, -2 next to last, etc.
//	If Abs(N) > number of windows or N = 0, return nil

WindowPtr
UWindows::FindNthWindow(
	SInt16	inN)
{
	WindowPtr	theWindowP = nil;
	SInt16		wIndex = inN;

	if (wIndex < 0) {				// Negative index counts from end
		SInt16	windowCount = 0;	// Count how many windows there are
		theWindowP = ::GetWindowList();
		while (theWindowP) {
			windowCount++;
			theWindowP = MacGetNextWindow(theWindowP);
		}							// Compute positive index
		wIndex = (SInt16) (windowCount + wIndex + 1);
	}

	if (wIndex > 0) {				// Count down into linked list
		theWindowP = ::GetWindowList();
		while (--wIndex  && theWindowP) {
			theWindowP = MacGetNextWindow(theWindowP);
		}
	}

	return theWindowP;
}

// ---------------------------------------------------------------------------
//	¥ FindWindowIndex
// ---------------------------------------------------------------------------
//	Return index position of a Window
//
//	Windows are ordered from front to back, with #1 being the front window.
//	If inWindowP is not found, returns 0;

SInt16
UWindows::FindWindowIndex(
	WindowPtr	inWindowP)
{
	SInt16		index = 1;
	WindowPtr	currWindowP = ::GetWindowList();

	while ( (currWindowP != nil) && (currWindowP != inWindowP) ) {
		index++;
		currWindowP = MacGetNextWindow(currWindowP);
	}

	if (currWindowP == nil) {
		index = 0;
	}

	return index;
}


// ---------------------------------------------------------------------------
//	¥ FindNamedWindow
// ---------------------------------------------------------------------------
//	Return a WindowPtr to the Window with the specified name
//
//	Searches from front to back for the first window with the specified
//	name. Returns nil if no match found.

WindowPtr
UWindows::FindNamedWindow(
	Str255	inName)
{
	WindowPtr	theWindowP = ::GetWindowList();

	while (theWindowP != nil) {
		Str255	windowName;
		::GetWTitle(theWindowP, windowName);
		if (::IdenticalString(inName, windowName, nil) == 0) {
			break;
		}
		theWindowP = MacGetNextWindow(theWindowP);
	}

	return theWindowP;
}


// ---------------------------------------------------------------------------
//	¥ GetColorTableEntry
// ---------------------------------------------------------------------------
//	Retrieve an entry from a Window Color Table ('wctb' resource).
//
//	If the entry exists, function returns true and sets outColor. Returns
//	false if the entry does not exist.

bool
UWindows::GetColorTableEntry(
	CTabHandle	inColorTableH,
	SInt16		inEntryValue,
	RGBColor&	outColor)
{
	bool	entryFound = false;

	if ( (inColorTableH != nil) && (*inColorTableH != nil) ) {

			// Search from the back, since that's what the Window Mgr does

		SInt16	size = (**inColorTableH).ctSize;

		for (SInt16 index = size; index >= 0; index--) {

			if ((**inColorTableH).ctTable[index].value == inEntryValue) {
				outColor = (**inColorTableH).ctTable[index].rgb;
				entryFound = true;
				break;
			}
		}
	}

	return entryFound;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ExcludeStructuresAbove
// ---------------------------------------------------------------------------
//	Subtract the structure regions of all visible windows above the
//	specified window from the specified region
//
//		inWindowP	- Target window
//		ioRegion	- Caller must pass a valid RgnHandle, which may be
//						non-empty. Structure regions of windows above the
//						target window are excluded from this region.

void
UWindows::ExcludeStructuresAbove(
	WindowPtr	inWindowP,
	RgnHandle	ioRegion)
{
		// The window list stores windows in front-to-back order.
		// Traverse the list from the start until we reach the
		// input window. For safety, check for nil, too.

	WindowPtr	currWindowP = ::GetWindowList();

	while ( (currWindowP != inWindowP) && (currWindowP != nil) ) {
	
			// Exclude structure regions of visible windows
	
		if (MacIsWindowVisible(currWindowP)) {
		
			#if TARGET_API_MAC_CARBON
			
				StRegion	structRgn;
				::GetWindowRegion(currWindowP, kWindowStructureRgn,
										structRgn);
				
				::DiffRgn(ioRegion, structRgn, ioRegion);
				
			#else
			
				::DiffRgn( ioRegion,
						   ((WindowPeek) currWindowP)->strucRgn,
						   ioRegion );
								
			#endif
		}
	
		currWindowP = MacGetNextWindow(currWindowP);
	}
}


// ---------------------------------------------------------------------------
//	¥ ExcludeActiveStructures
// ---------------------------------------------------------------------------
//	Subtract the structure regions of all active windows from the
//	specified region
//
//		ioRegion	- Caller must pass a valid RgnHandle, which may be
//						non-empty. Structure regions of active windows
//						are excluded from this region.

void
UWindows::ExcludeActiveStructures(
	RgnHandle	ioRegion)
{
		// The window list stores windows in front-to-back order.
		// Traverse the list from the start until we reach the
		// first visible, inactive window (or the end of the list).

	WindowPtr	currWindowP = ::GetWindowList();
	
	while (currWindowP != nil) {
	
		if (MacIsWindowVisible(currWindowP)) {

			if (IsWindowHilited(currWindowP)) {
			
				#if TARGET_API_MAC_CARBON
				
					StRegion	structRgn;
					::GetWindowRegion(currWindowP, kWindowStructureRgn,
										structRgn);
					
					::DiffRgn(ioRegion, structRgn, ioRegion);
					
				#else
				
					::DiffRgn( ioRegion,
							   ((WindowPeek) currWindowP)->strucRgn,
							   ioRegion );
									
				#endif
				
			} else {			// Window is visible and not hilited.
				break;			//   Terminate loop.
			}
		}
	
		currWindowP = MacGetNextWindow(currWindowP);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CenterBoundsOnRect
// ---------------------------------------------------------------------------
//	Position the bounds so that it is centered on the input rectangle

void
UWindows::CenterBoundsOnRect(
	Rect&		ioBounds,
	const Rect&	inRect)
{
										// Center horizontally
	SInt16	rectWidth	= (SInt16) (inRect.right - inRect.left);
	SInt16	boundsWidth	= (SInt16) (ioBounds.right - ioBounds.left);
	
	ioBounds.left  = (SInt16) (inRect.left + (rectWidth - boundsWidth) / 2);
	ioBounds.right = (SInt16) (ioBounds.left + boundsWidth);
	
										// Vertical
	SInt16	rectHeight	 = (SInt16) (inRect.bottom - inRect.top);
	SInt16	boundsHeight = (SInt16) (ioBounds.bottom - ioBounds.top);
	
	if (rectHeight < boundsHeight) {	// Rect is shorter than Bounds
		ioBounds.top = inRect.top;		// Align top edges
		
	} else {							// Center vertically
		ioBounds.top = (SInt16) (inRect.top + (rectHeight - boundsHeight) / 2);
	}
	
	ioBounds.bottom = (SInt16) (ioBounds.top + boundsHeight);
}


// ---------------------------------------------------------------------------
//	¥ AlertBoundsOnRect
// ---------------------------------------------------------------------------
//	Position the bounds so that it is at the alert position on the input rect
//
//	Inside Mac says that alert position is centered horizontally and
//	positioned vertically such that about one-fifth of the rectangle is above
//	it. Tests show that this means that the distance between the top of
//	the rectangle and the top of the bounds is one-fifth the difference
//	in heights. This assumes that the rectangle is taller than the bounds.
//
//	If the rectangle isn't taller than the bounds, then the top of the bounds
//	is the same as the top of the rectangle.

void
UWindows::AlertBoundsOnRect(
	Rect&		ioBounds,
	const Rect&	inRect)
{
										// Center horizontally
	SInt16	rectWidth	= (SInt16) (inRect.right - inRect.left);
	SInt16	boundsWidth	= (SInt16) (ioBounds.right - ioBounds.left);
	
	ioBounds.left  = (SInt16) (inRect.left + (rectWidth - boundsWidth) / 2);
	ioBounds.right = (SInt16) (ioBounds.left + boundsWidth);
	
										// Position vertically
	SInt16	rectHeight	 = (SInt16) (inRect.bottom - inRect.top);
	SInt16	boundsHeight = (SInt16) (ioBounds.bottom - ioBounds.top);
	
	if (rectHeight <= boundsHeight) {	// Rect is not taller than Bounds
		ioBounds.top = inRect.top;		// Align top sides
		
	} else {							// 1/5 of height difference is
										//   above the Bounds
		ioBounds.top = (SInt16) (inRect.top + (rectHeight - boundsHeight) / 5);
	}
	
	ioBounds.bottom = (SInt16) (ioBounds.top + boundsHeight);
}


// ---------------------------------------------------------------------------
//	¥ PositionBoundsOnScreen
// ---------------------------------------------------------------------------
//	Position the bounds so that it fits entirely on the screen

void
UWindows::PositionBoundsOnScreen(
	Rect&		ioBounds,
	GDHandle	inScreen)
{
	Rect	screenRect = (**inScreen).gdRect;
	
										// Adjust for MenuBar on main screen
	if (inScreen == ::GetMainDevice()) {
		screenRect.top += ::GetMBarHeight();
	}
	
										// Store bounds width and height
	SInt16	boundsWidth	 = (SInt16) (ioBounds.right - ioBounds.left);
	SInt16	boundsHeight = (SInt16) (ioBounds.bottom - ioBounds.top);
	
										// Adjust left of bounds
	if (ioBounds.left < (screenRect.left + Screen_Margin)) {
		ioBounds.left = (SInt16) (screenRect.left + Screen_Margin);
		
	} else if (ioBounds.right > (screenRect.right - Screen_Margin)) {
		ioBounds.left = (SInt16) (screenRect.right - Screen_Margin - boundsWidth);
	}
	
										// Adjust top of bounds
	if (ioBounds.top < (screenRect.top + Screen_Margin)) {
		ioBounds.top = (SInt16) (screenRect.top + Screen_Margin);
		
	} else if (ioBounds.bottom > (screenRect.bottom - Screen_Margin)) {
		ioBounds.top = (SInt16) (screenRect.bottom - Screen_Margin - boundsHeight);
	}
	
										// Set right and bottom based on
										//   saved width and height
	ioBounds.right  = (SInt16) (ioBounds.left + boundsWidth);
	ioBounds.bottom = (SInt16) (ioBounds.top + boundsHeight);
}


// ---------------------------------------------------------------------------
//	¥ BoundsAreOutsideRect
// ---------------------------------------------------------------------------
//	Return whether any portion of the bounds is outside a specified rectangle

bool
UWindows::BoundsAreOutsideRect(
	SInt16		inLeft,
	SInt16		inTop,
	SInt16		inWidth,
	SInt16		inHeight,
	const Rect&	inRect)
{
	return (inLeft < inRect.left)  ||
		   (inTop < inRect.top)  ||
		   ((inLeft + inWidth) > inRect.right)  ||
		   ((inTop + inHeight) > inRect.bottom);
}


// ---------------------------------------------------------------------------
//	¥ BoundsAreTooFarOffscreen
// ---------------------------------------------------------------------------
//	Return whether the bounds are too far offscreen to be considered a
//	valid cascade position
//
//	We consider the bounds too far offscreen if more than 1/2 of the height
//	or the width is outside the screen rectangle
//
//	We assume that the top left is inside the screen rectangle

bool
UWindows::BoundsAreTooFarOffscreen(
	SInt16		inLeft,
	SInt16		inTop,
	SInt16		inWidth,
	SInt16		inHeight,
	const Rect&	inRect)
{
	bool	tooFar = false;
	
	if ((inTop + inHeight) > inRect.bottom) {
		tooFar = (2 * (inRect.bottom - inTop) < inHeight);
	}
	
	if (not tooFar && ((inLeft + inWidth) > inRect.left)) {
		tooFar = (2 * (inRect.right - inLeft) < inWidth);
	}
	
	return tooFar;
}

#pragma mark -
#pragma mark === Carbon-Only Functions ===

#if PP_Target_Carbon

// ---------------------------------------------------------------------------
//	¥ RepositionWindow											 [Carbon Only]
// ---------------------------------------------------------------------------
//	Position a window relative to another window or display screen
//
//	This routine is an implementation of the Toolbox routine with the same
//	name. We do this because the Toolbox RepositionWindow() does not
//	exist on Systems prior to Mac OS 8.5. However, CarbonLib 1.0.x runs
//	on Mac OS 8.1 and we need this routine to support floating windows
//	on that configuration
//
//	Behavior Difference from Toolbox
//		When positioning relative to a parent window screen, this function
//	behaves differently from the Toolbox implementation when the parent
//	WindowRef is nil. This function assumes the screen is the main screen,
//	and positions the window accordingly.
//
//		The Toolbox implementation considers this a paramErr (-50) and
//	does not change the window position.
//
//		The behavior of this function matches that of the WIND resource
//	positioning constants introduced with System 7. The advantage is
//	that callers don't have to make a special case for when there is no
//	parent window. For example, you usually want to place alerts on the
//	screen containing the frontmost window, because that's presumably
//	where the user's attention is focused. But if there are no windows,
//	you want the alert to be on the main screen.
//
//		Also, the Toolbox RepositionWindow has different behavior on
//	Mac OS 8 and 9, Carbon or Classic. This function tries to mimic the
//	Mac OS 9 Toolbox behavior.

OSStatus
UWindows::RepositionWindow(
	WindowRef				inWindow,
	WindowRef				inParentWindow,
	WindowPositionMethod	inPosMethod)
{
	if (not ::IsValidWindowPtr(inWindow)) {
		return errInvalidWindowRef;
	}

	OSStatus	status;
									// Get bounds of window's structure
									//   region in global coords
	Rect	wBounds;
	status = ::GetWindowBounds(inWindow, kWindowStructureRgn, &wBounds);
	
									// Get window's content rect and
									//   determine its offset from the
									//   structure rect. We'll need this
									//   info later when we move the window.
	Rect	contentRect;
	status = ::GetWindowBounds(inWindow, kWindowContentRgn, &contentRect);
	
	Point	contentOffset;
	contentOffset.h = (SInt16) (contentRect.left - wBounds.left);
	contentOffset.v = (SInt16) (contentRect.top - wBounds.top);
	
		// We will position the window relative to a reference rectangle.
		// There are 3 possible reference rectangles:
		//		- Bounds of main screen
		//		- Bounds of parent window
		//		- Bounds of screen containing parent window
	
	Rect		referenceRect;		// Position bounds relative to this rect
	GDHandle	screen = nil;		// Screen on which to place bounds
	
	switch (inPosMethod) {
	
		case kWindowCenterOnMainScreen:
		case kWindowCascadeOnMainScreen:
		case kWindowAlertPositionOnMainScreen: {
									// Reference is bounds of main screen
			screen = ::GetMainDevice();
			referenceRect = (**screen).gdRect;
			referenceRect.top += ::GetMBarHeight();
			break;
		}
			
		case kWindowCenterOnParentWindow:
		case kWindowCascadeOnParentWindow:
		case kWindowAlertPositionOnParentWindow: {
									// Reference is bounds of parent window
			if (not ::IsValidWindowPtr(inParentWindow)) {
				return errInvalidWindowRef;
			}
		
			::GetWindowBounds(inParentWindow, kWindowStructureRgn, &referenceRect);
			
									// Screen is the one that contains the
									//   largest portion of the parent window
			screen = FindDominantDevice(referenceRect);
			break;
		}
			
		case kWindowCenterOnParentWindowScreen:
		case kWindowCascadeOnParentWindowScreen:
		case kWindowAlertPositionOnParentWindowScreen: {
									// Reference is bounds of screen containing
									//   largest portion of parent window
			if (::IsValidWindowPtr(inParentWindow)) {
				::GetWindowBounds(inParentWindow, kWindowStructureRgn, &referenceRect);
				screen = FindDominantDevice(referenceRect);
			}
			
			if (screen == nil) {	// Parent window does not exist or is
									//   offscreen, so use main screen
				screen = ::GetMainDevice();
			}
			
			referenceRect = (**screen).gdRect;
			
			if (screen == ::GetMainDevice()) {
				referenceRect.top += ::GetMBarHeight();
			}
			break;
		}
	}
	
		// Now that we have the reference rectangle, there are
		// 3 possible positionings:
		//		- Center
		//		- Cascade
		//		- Alert
		
	bool	forceAllOnScreen = true;
	
	switch (inPosMethod) {
	
		case kWindowCenterOnMainScreen:
		case kWindowCenterOnParentWindow:
		case kWindowCenterOnParentWindowScreen:
										// Center
			CenterBoundsOnRect(wBounds, referenceRect);
			break;
			
		case kWindowCascadeOnMainScreen:
		case kWindowCascadeOnParentWindowScreen:
		case kWindowCascadeOnParentWindow: {
										// Cascade
										
				// Amount to offset each cascade position depends on the
				// thickness of the window's structure region around the
				// content region
			
			Point	offset;
			offset.h = (SInt16) (Offset_CascadeLeft + (contentRect.left - wBounds.left));
			offset.v = (SInt16) (Offset_CascadeTop + (contentRect.top - wBounds.top));
			
			WindowClass		windClass;
			::GetWindowClass(inWindow, &windClass);
			
			if (inPosMethod == kWindowCascadeOnParentWindow) {
			
					// The start location for cascading is offset from
					// the parent window's content region
				
				Rect	parentContent;
				::GetWindowBounds(inParentWindow, kWindowContentRgn, &parentContent);
				
				Point	startLocation;
				startLocation.h = (SInt16) (parentContent.left + Offset_CascadeLeft);
				startLocation.v = (SInt16) (parentContent.top + Offset_CascadeTop);
			
					// Need screen rect to make sure bounds isn't offscreen
					
				Rect	screenRect = (**screen).gdRect;
				if (screen == ::GetMainDevice()) {
					screenRect.top += ::GetMBarHeight();
				}
				
				CascadeBoundsAtLocation(
					wBounds,
					startLocation,
					screenRect,
					offset,
					windClass );
					
			} else {					// Cascade on a screen
				CascadeBoundsOnScreen(
					wBounds,
					referenceRect,
					offset,
					windClass );
										// This is the only situation where
										//   it's OK for the window to be
										//   partially offscreen
				forceAllOnScreen = false;
			}
			break;
		}
			
		case kWindowAlertPositionOnMainScreen:
		case kWindowAlertPositionOnParentWindow:
		case kWindowAlertPositionOnParentWindowScreen:
										// Alert Position
			AlertBoundsOnRect(wBounds, referenceRect);
			break;
	}
	
		// Next, we adjust the bounds, if necessary, so that
		// it fits entirely on the screen
	
	if (forceAllOnScreen) {
		PositionBoundsOnScreen(wBounds, screen);
	}
	
		// Finally, we move the window to its new location
		// Note: Although Carbon has a routine called SetWindowBounds(),
		// that routine is not reliable because it behaves differently
		// on different OS versions.

	::MacMoveWindow( inWindow,
					 (SInt16) (wBounds.left + contentOffset.h),
					 (SInt16) (wBounds.top + contentOffset.v),
					 false );
	
	return status;
}


// ---------------------------------------------------------------------------
//	¥ CascadeBoundsOnScreen										 [Carbon Only]
// ---------------------------------------------------------------------------
//	Position the bounds so that it is cascaded on the screen
//
//	The cascade position is one where there aren't already too many windows.
//	Most of the time, this means that no other window is at (or near) the
//	position. If there is a window at a position, we offset the position
//	and retest.
//
//	However, it's possible that there are so many windows that every position
//	is occupied. If so, we wrap around back to the top left and try again.
//	With each wrap, we allow one more window to be at the position.
//
//	For example, if there are already 2 or more windows at each location, the
//	cascade position will be the first one where there are exactly 2 windows,
//	and not 3 or more.

void
UWindows::CascadeBoundsOnScreen(
	Rect&		ioBounds,
	const Rect&	inScreenRect,
	Point		inOffset,
	WindowClass	inWindClass)
{	
	SInt16	boundsWidth	 = (SInt16) (ioBounds.right - ioBounds.left);
	SInt16	boundsHeight = (SInt16) (ioBounds.bottom - ioBounds.top);
	
		// Start location is at the top left of the screen
	
	SInt16	left	= inScreenRect.left;
	SInt16	top		= inScreenRect.top;
	
	if (BoundsAreTooFarOffscreen( left, top, boundsWidth, boundsHeight, inScreenRect)) {
	
			// If first cascade position is too far offscreen, there's no
			// need to keep looking. We will just have to use it. This
			// will only happen if the bounds is bigger than the screen.
		
		ioBounds.left	= left;
		ioBounds.right	= (SInt16) (left + boundsWidth);
		ioBounds.top	= top;
		ioBounds.bottom	= (SInt16) (top + boundsHeight);
		
		return;
	}
	
			// Loop over cascade positions until we find one where
			// there aren't too many windows there already.
	
	SInt16	wraps = 0;
	
	while (TooManyWindowsAtLocation(left, top, inWindClass, wraps)) {
	
			// Present cascade position is occupied, move down and right
			// and try again
		
		left += inOffset.h;
		top  += inOffset.v;
		
		if (BoundsAreTooFarOffscreen(left, top, boundsWidth, boundsHeight, inScreenRect)) {
		
				// Proposed location is too far offscreen, so we wrap
				// around back to the top left of the screen
			
			left   = inScreenRect.left;
			top    = inScreenRect.top;
			wraps += 1;
		}
	}

	ioBounds.left	= left;				// Put bounds at the computed location
	ioBounds.right	= (SInt16) (left + boundsWidth);
	ioBounds.top	= top;
	ioBounds.bottom	= (SInt16) (top + boundsHeight);
}


// ---------------------------------------------------------------------------
//	¥ CascadeBoundsAtLocation									 [Carbon Only]
// ---------------------------------------------------------------------------
//	Position bounds so that it cascaded on screen starting at a specified
//	location
//
//	This routine is similar to CascadeBoundsOnScreen, with the following
//	differences:
//
//		- Starting location is an input parameter rather than always
//			the top left of the screen
//		- Cascading does not loop back to the starting location if all
//			positions are occupied
//		- Last cascade position is the first one where any portion of
//			the bounds is offscreen

void
UWindows::CascadeBoundsAtLocation(
	Rect&		ioBounds,
	Point		inLocation,
	const Rect&	inScreenRect,
	Point		inOffset,
	WindowClass	inWindClass)
{
	SInt16	boundsWidth	 = (SInt16) (ioBounds.right - ioBounds.left);
	SInt16	boundsHeight = (SInt16) (ioBounds.bottom - ioBounds.top);
	
	SInt16	left	= inLocation.h;		// Start at specified location
	SInt16	top		= inLocation.v;		
	
	if (BoundsAreOutsideRect(left, top, boundsWidth, boundsHeight, inScreenRect)) {
	
			// If first cascade position is offscreen, there's no
			// need to keep looking. We will just have to use it.
		
		ioBounds.left	= left;
		ioBounds.right	= (SInt16) (left + boundsWidth);
		ioBounds.top	= top;
		ioBounds.bottom	= (SInt16) (top + boundsHeight);
		
		return;
	}
	
	while (TooManyWindowsAtLocation(left, top, inWindClass, 0)) {
	
			// Present cascade position is occupied, move down and right
			// and try again
		
		left += inOffset.h;
		top  += inOffset.v;
		
		if (BoundsAreOutsideRect( left, top, boundsWidth, boundsHeight, inScreenRect)) {
		
			// Proposed location is offscreen. We don't wrap, though.
			// We just use this position.
		
			break;
		}
	}
	
	ioBounds.left	= left;				// Put bounds at the computed location
	ioBounds.right	= (SInt16) (left + boundsWidth);
	ioBounds.top	= top;
	ioBounds.bottom	= (SInt16) (top + boundsHeight);
}


// ---------------------------------------------------------------------------
//	¥ TooManyWindowsAtLocation									 [Carbon Only]
// ---------------------------------------------------------------------------
//	Return whether there are at more than a specified number of windows at
//	a particular location
//
//	A window is considered to be at the location if its top left corner is
//	within Delta_CloseEnough pixels both horizontally and vertically

bool
UWindows::TooManyWindowsAtLocation(
	SInt16		inLeft,						// Global coords
	SInt16		inTop,
	WindowClass	inWindClass,				// Only count windows of this class
	SInt16		inMaxWindows)
{
	bool		tooMany = false;
	SInt16		windowCount = 0;
	
		// Loop through all visible windows of the specified class
	
	WindowRef	window = ::GetFrontWindowOfClass(inWindClass, true);
	
	while (window != nil) {
	
			// Get location of this window and see if its location
			// is close to the specified location
	
		Rect	bounds;
		::GetWindowBounds(window, kWindowStructureRgn, &bounds);
		
		SInt16	horizDelta = (SInt16) (bounds.left - inLeft);
		SInt16	vertDelta  = (SInt16) (bounds.top - inTop);
		
		if (horizDelta < 0) {
			horizDelta = (SInt16) (-horizDelta);
		}
		
		if (vertDelta < 0) {
			vertDelta = (SInt16) (-vertDelta);
		}
		
		if ( (horizDelta <= Delta_CloseEnough)  &&
			 (vertDelta  <= Delta_CloseEnough) ) {
		
			windowCount += 1;			// Another window at the location
		}
		
		if (windowCount > inMaxWindows) {
			tooMany = true;				// Maximum window count reached
			break;						//   so we can stop looping
		}
		
		window = ::GetNextWindowOfClass(window, inWindClass, true);
	}
	
	return tooMany;
}

#endif // PP_Target_Carbon


PP_End_Namespace_PowerPlant
