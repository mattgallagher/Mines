// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFocusBox.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A FocusBox outlines a Pane to indicate that Pane is the current focus
//	for keystrokes.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFocusBox.h>
#include <LView.h>
#include <UDrawingState.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFocusBox								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LFocusBox::LFocusBox()
{
	Disable();
}


// ---------------------------------------------------------------------------
//	¥ LFocusBox								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LFocusBox::LFocusBox(
	const LFocusBox&	inOriginal)

	: LPane(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LFocusBox								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LFocusBox::LFocusBox(
	LStream*	inStream)

	: LPane(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ AttachPane
// ---------------------------------------------------------------------------
//	Associate a host Pane with a FocusBox, sizing and positioning the
//	FocusBox so that it fits around the Pane
//
//	If inSameBindings is true, FocusBox uses same frame bindings as the
//	host Pane. This makes the FocusBox move and resize along with the host
//	Pane if their mutual SuperView moves or resizes.
//
//	However, this will not move/resize the FocusBox if you specifically
//	move/resize the host Pane. To handle this situation, the host Pane must
//	explicitly move/resize the FocusBox, and pass false for inSameBindings
//	to suppress the automatic move/resize.

void
LFocusBox::AttachPane(
	LPane*		inPane,
	Boolean		inSameBindings)
{
									// FocusBox has the same SuperView
									//   as its host Pane
	LView	*superView = inPane->GetSuperView();
	if (superView != mSuperView) {
		PutInside(superView);
	}

	SDimension16	hostSize;		// Size FocusBox to surround its host
	inPane->GetFrameSize(hostSize);
	ResizeFrameTo( (SInt16) (hostSize.width + 6),
				   (SInt16) (hostSize.height + 6), false);

									// Locate FocusBox relative to its host
	SPoint32	hostLocation;		//   location in Port coords
	inPane->GetFrameLocation(hostLocation);

	SPoint32	imageLoc;			// Get Super Image location
	imageLoc.h = imageLoc.v = 0;
	if (superView != nil) {
		superView->GetImageLocation(imageLoc);
	}

									// Specify location of FocusBox
									//   in Image coords of its SuperView
	PlaceInSuperImageAt(hostLocation.h - imageLoc.h - 3,
						hostLocation.v - imageLoc.v - 3, false);

	if (inSameBindings) {			// Use same FrameBinding as host
		inPane->GetFrameBinding(mFrameBinding);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw a FocusBox

void
LFocusBox::DrawSelf()
{
	Rect	frame;
	if (CalcLocalFrameRect(frame)) {
		ApplyForeAndBackColors();
		::PenNormal();
		::PenSize(2, 2);
		::MacFrameRect(&frame);
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------
//	A FocusBox is being shown

void
LFocusBox::ShowSelf()
{
	Draw(nil);							// Draw immediately
	DontRefresh();						// No need to update
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	A FocusBox is being hidden

void
LFocusBox::HideSelf()
{
	if (FocusDraw()) {					// Immediately erase the FocusBox
		Rect	frame;
		CalcLocalFrameRect(frame);

		StRegion	boxRgn( GetBoxRegion(frame), false );
		ApplyForeAndBackColors();
		::EraseRgn(boxRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ Refresh
// ---------------------------------------------------------------------------
//	Invalidate the area occupied by a FocusBox so it will redraw during
//	the next Update event

void
LFocusBox::Refresh()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	StRegion		boxRgn( GetBoxRegion(frame), false );
	RefreshRgn(boxRgn);
}


// ---------------------------------------------------------------------------
//	¥ DontRefresh
// ---------------------------------------------------------------------------
//	Validate the area occupied by a FocusBox so it won't redraw during
//	the next Update event

void
LFocusBox::DontRefresh(
	Boolean	inOKIfHidden)
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	StRegion		boxRgn( GetBoxRegion(frame), false );
	DontRefreshRgn(boxRgn, inOKIfHidden);
}


// ---------------------------------------------------------------------------
//	¥ GetBoxRegion
// ---------------------------------------------------------------------------
//	Pass back a region defining a FocusBox. A FocusBox is a hollow
//	rectangular region.
//
//	Creates a new region which the caller must dispose.

RgnHandle
LFocusBox::GetBoxRegion(
	const Rect&		inFrame) const
{
	StRegion	boxRgn;

	{
		GrafPtr	port = GetMacPort();

		if (port != nil) {
			StGrafPortSaver	savePort(port);
			StRegionBuilder	builder(boxRgn);
			Rect	r = inFrame;
			::MacFrameRect(&r);				// Outer edge of box
			::MacInsetRect(&r, 2, 2);
			::MacFrameRect(&r);				// Inner edge of box
		}
	}

	return boxRgn.Release();
}


PP_End_Namespace_PowerPlant
