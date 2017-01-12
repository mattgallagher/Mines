// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UScreenPort.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Creates a GrafPort that is the same size as the Gray Region
//	(all screen area excluding the menu bar).
//
//	All class member functions are static, so you never actually
//	make a UScreenPort object.
//
//	Routines:
//		void	UScreenPort::Initialize()
//			Creates a new GrafPort.
//
//		void	UScreenPort::Dispose();
//			Disposes of the GrafPort
//
//		GrafPtr	UScreenPort::GetScreenPort();
//			Returns a pointer to the GrafPort
//
//	Typical Use:
//		- Call MacSetPort(UScreenPort::GetScreenPort()) whenever you want
//		to draw into the Screen Port. This is just a normal QuickDraw
//		port, so it's up to you to adjust the port characteristics
//		(font, pen, color settings, etc.) as needed.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UScreenPort.h>
#include <UDrawingState.h>
#include <UTBAccessors.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

GrafPtr	UScreenPort::sScreenGrafPtr = nil;


// ---------------------------------------------------------------------------
//	¥ Initialize
// ---------------------------------------------------------------------------
//	Create a GrafPort covering the entire gray region (all screen area)

void
UScreenPort::Initialize()
{
	if (sScreenGrafPtr != nil) {		// Do nothing if already initialized
		return;
	}

	StGrafPortSaver	savePort;			// Save/restore current GrafPort

										// Allocate and Initialize GrafPort
#if PP_Target_Carbon

	sScreenGrafPtr = ::CreateNewPort();

#else

	sScreenGrafPtr = (GrafPtr) ::NewPtr(sizeof(CGrafPort));
	if (sScreenGrafPtr != nil) {
		::OpenCPort((CGrafPtr) sScreenGrafPtr);
	}

#endif

		// To make the GrafPort the same as the screen (excluding the
		// menu bar), we adjust its origin so that its local coordinates
		// are the same as global coordinates and set its port rectangle,
		// clipping region, and visible region to the size of the Gray Region.

	if (sScreenGrafPtr != nil) {
		RgnHandle	grayRgnH = ::GetGrayRgn();
		Rect		grayBounds;
		::GetRegionBounds(grayRgnH, &grayBounds);

		::MovePortTo(grayBounds.left, grayBounds.top);
		::SetOrigin(grayBounds.left, grayBounds.top);

		::PortSize((SInt16) (grayBounds.right - grayBounds.left),
				   (SInt16) (grayBounds.bottom - grayBounds.top));
		::SetClip(grayRgnH);

		::SetPortVisibleRegion(sScreenGrafPtr, grayRgnH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Dispose
// ---------------------------------------------------------------------------

void
UScreenPort::Dispose()
{
	if (sScreenGrafPtr != nil) {

	#if PP_Target_Carbon

		::DisposePort(sScreenGrafPtr);

	#else

		::CloseCPort((CGrafPtr) sScreenGrafPtr);
		::DisposePtr((Ptr) sScreenGrafPtr);

	#endif

		sScreenGrafPtr = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetScreenPort
// ---------------------------------------------------------------------------

GrafPtr
UScreenPort::GetScreenPort()
{
	if (sScreenGrafPtr == nil) {		// Initialize if necessary
		Initialize();
	}

	return sScreenGrafPtr;
}


PP_End_Namespace_PowerPlant
