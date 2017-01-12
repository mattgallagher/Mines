// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAFocusBorder.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAFocusBorder.h>

#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LStream.h>
#include <LString.h>
#include <URegions.h>
#include <UDrawingState.h>
#include <PP_Messages.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LGAFocusBorder* LGAFocusBorder::sFocusedBorder = nil;


// ---------------------------------------------------------------------------
//	¥ LGAFocusBorder						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGAFocusBorder::LGAFocusBorder()
{
	mPaintBorderFace	= true;
	mInsetSubPaneID		= PaneIDT_Undefined;
	mCommanderSubPaneID = PaneIDT_Unspecified;
	mFrameInsetSubPane	= true;
	mNotchInsetBorder	= false;
	mNotchBorderFace	= false;
	mNotchWidth			= 15;
	mCanFocus			= false;
	mInsetSubPane		= nil;
}


// ---------------------------------------------------------------------------
//	¥ LGAFocusBorder						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAFocusBorder::LGAFocusBorder(
	LStream*	inStream)

	: LView(inStream)
{
	mInsetSubPane = nil;

	*inStream >> mInsetSubPaneID;
	*inStream >> mCommanderSubPaneID;
	*inStream >> mPaintBorderFace;
	*inStream >> mFrameInsetSubPane;
	*inStream >> mNotchInsetBorder;
	*inStream >> mNotchBorderFace;
	*inStream >> mNotchWidth;
	*inStream >> mCanFocus;

	Boolean		hasFocus;
	*inStream >> hasFocus;

										// Debug check for consistent settings:
										// CanFocus requires a Commander SubPane
	SignalIf_( mCanFocus && (mCommanderSubPaneID == PaneIDT_Unspecified) );

	if (mCanFocus && hasFocus) {		// Make this the focused border

		if (sFocusedBorder) {			// Clear current focused border
			sFocusedBorder->BecomeFocused(false);
		}
										// Then set us up as the current focus
		sFocusedBorder = this;
	}

	if (mCommanderSubPaneID == PaneIDT_Undefined) {
										// No Commander SubPane, so take this
										// object out of the chain of command
		SetDefaultCommander(GetSuperCommander());
		SetSuperCommander(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LGAFocusBorder						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAFocusBorder::~LGAFocusBorder ()
{
	if (sFocusedBorder == this) {	// Clear the sFocusedBorder field if
		sFocusedBorder = nil;		// it is currently pointing to us
	}
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::FinishCreateSelf()
{
												// Setup our reference to the commander sub pane,
												// if we are that view then we do nothing
	if (mCommanderSubPaneID != PaneIDT_Undefined) {

		LCommander *commanderSubPane = dynamic_cast<LCommander *>(FindPaneByID(mCommanderSubPaneID));
		ThrowIfNil_(commanderSubPane);
		if (commanderSubPane == this) {
			commanderSubPane = nil;
		}
												// Make sure that the associated commander is set
												// to latent
		SetLatentSub ( commanderSubPane );
	}
												// Setup our reference to the inset sub pane we will
												// be framing if requested by the user, if we are
												// that view then we do nothing or if there is no
												// pane ID specified we simply set the sub pane to
												// nil
	if ( mInsetSubPaneID != PaneIDT_Undefined ) {

		mInsetSubPane = FindPaneByID ( mInsetSubPaneID );
		if ( mInsetSubPane == this )
			mInsetSubPane = nil;

	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetBorderRegion												  [public]
// ---------------------------------------------------------------------------
//	Return region covered by the border. Caller is responsible for disposing
//	the returned RgnHandle.

RgnHandle
LGAFocusBorder::GetBorderRegion()
{
												// Calculate the border frame rect
	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
	StRegion faceRgn ( localFrame );
												// If requested remove the portion of the border
												// that would be occupied by the growbox from the
												// face region
	if ( mNotchBorderFace ) {
		Rect growRect = localFrame;
		growRect.left = (SInt16) (growRect.right - mNotchWidth);
		growRect.top  = (SInt16) (growRect.bottom - mNotchWidth);
		StRegion	growRgn ( growRect );
		faceRgn -= growRgn;
	}
												// Now we need to remove the portion of the face
												// occupied by the inset sub pane, if this has been
												// requested and we have a reference to a sub pane
	if ( mInsetSubPane ) {
		Rect insetRect;
		mInsetSubPane->CalcPortFrameRect ( insetRect );
		PortToLocalPoint ( topLeft ( insetRect ));
		PortToLocalPoint ( botRight( insetRect ));
		StRegion insetRgn ( insetRect );

												// Now if we also need to notch out the inset frame
												// then handle it now
		if ( NotchInsetFrame ()) {
			Rect	notchRect = insetRect;
			notchRect.left = (SInt16) (notchRect.right - mNotchWidth);
			notchRect.top  = (SInt16) (notchRect.bottom - mNotchWidth);
			StRegion notchRgn ( notchRect );
			insetRgn -= notchRgn;
		}
												// Now we can get the inset region removed from the
												// face region
		faceRgn -= insetRgn;
	}
												// Now that is all done we can return the region to
												// the caller who will be responsible for disposing
												// of it when done
	return faceRgn.Release ();

}


// ---------------------------------------------------------------------------
//	¥ GetInsetFrameRegion											  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGAFocusBorder::GetInsetFrameRegion ()
{
												// Now we need to remove the portion of the face
												// occupied by the inset sub pane, if this has been
												// requested and we have a reference to a sub pane
	if ( mInsetSubPane ) {
		Rect insetRect;
		mInsetSubPane->CalcPortFrameRect ( insetRect );
		PortToLocalPoint ( topLeft ( insetRect ));
		PortToLocalPoint ( botRight( insetRect ));
		StRegion insetRgn ( insetRect );

												// Now if we also need to notch out the inset frame
												// then handle it now
		if ( NotchInsetFrame ()) {
			Rect	notchRect = insetRect;
			notchRect.left = (SInt16) (notchRect.right - mNotchWidth);
			notchRect.top  = (SInt16) (notchRect.bottom - mNotchWidth);
			StRegion notchRgn ( notchRect );
			insetRgn -= notchRgn;
		}
												// Now we need to build the outer portion of the
												// region
		::MacInsetRect ( &insetRect, -2, -2 );
		StRegion outerRgn ( insetRect );

												// Now if we also need to notch out the inset frame
												// then handle it now
		if ( NotchInsetFrame ()) {
			Rect	outerNotchRect = insetRect;
			outerNotchRect.left = (SInt16) (outerNotchRect.right - mNotchWidth);
			outerNotchRect.top  = (SInt16) (outerNotchRect.bottom - mNotchWidth);
			StRegion outerNotchRgn ( outerNotchRect );
			outerRgn -= outerNotchRgn;
		}
													// Now we can get the inset region removed from
													// the face region
		outerRgn -= insetRgn;
													//	Now that is all done we can return the region
													// to the caller who will be responsible for
													// disposing of it when done
		return outerRgn.Release ();
	}
													//	We don't have an inset pane to frame so we
													// just return nil
	return nil;

}


// ---------------------------------------------------------------------------
//	¥ GetFocusRegion												  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGAFocusBorder::GetFocusRegion ()
{

	StRegion focusRgn;
												// Figure out the inset pane's bounding rect
	if ( mInsetSubPane ) {
		Rect	localFrame;
		mInsetSubPane->CalcPortFrameRect ( localFrame );
		PortToLocalPoint ( topLeft ( localFrame ));
		PortToLocalPoint ( botRight ( localFrame ));

												// Make the rect larger
		SInt16	inset = -2;
		if (FrameInsetSubPane()) {
			inset = -3;
		}
		::MacInsetRect(&localFrame, inset, inset);

												// Build the region
		focusRgn = localFrame;

												// Now we need to removed the portion of the face
												// occupied by the inset sub pane, if this has
												// been requested and we have a reference to a sub
												// pane
		::MacInsetRect ( &localFrame, 2, 2 );
		StRegion insetRgn ( localFrame );

												// Now if we also need to notch out the inset frame
												// then handle it now
		if ( NotchInsetFrame ()) {
			Rect	notchRect = localFrame;
			notchRect.left = (SInt16) (notchRect.right - mNotchWidth);
			notchRect.top  = (SInt16) (notchRect.bottom - mNotchWidth);
			StRegion notchRgn ( notchRect );
			insetRgn -= notchRgn;
												// We also need to take the notch out of the focus
												// region after we have offset it by the thickness
												// of the focus ring
			notchRgn.OffsetBy(2, 2);
			focusRgn -= notchRgn;
		}

												// Now we can get the inset region removed from the
												// face region
		focusRgn -= insetRgn;
	}
												// Now that is all done we can return the region to
												// the caller who will be responsible for disposing
												// of it when done
	return focusRgn.Release ();

}


// ---------------------------------------------------------------------------
//	¥ GetThemeFocusRegion											  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGAFocusBorder::GetThemeFocusRegion ()
{

	StRegion focusRgn;
												// Figure out the inset pane's bounding rect which
												// will be used to create the region needed for
												// rendering of the focus ring
	if ( mInsetSubPane ) {
		Rect	portFrame;
		mInsetSubPane->CalcPortFrameRect ( portFrame );
		PortToLocalPoint ( topLeft ( portFrame ));
		PortToLocalPoint ( botRight ( portFrame ));

												// Build the initial region

		if (not FrameInsetSubPane()) {			// If we're not drawing a frame,
			::MacInsetRect(&portFrame, 1, 1);	// the focus region is a bit smaller.
		}

		focusRgn = portFrame;

												// Now we are going to build a region that consists
												// of the varoius pixels that are missing from the
												// corners of the focus ring
		StRegion		cornerRgn;
		{
			StRegionBuilder builder ( cornerRgn );

			Rect pixelRect = portFrame;

			pixelRect.right  = (SInt16) (pixelRect.left + 1);
			pixelRect.bottom = (SInt16) (pixelRect.top + 1);
			::MacFrameRect ( &pixelRect );

			pixelRect = portFrame;
			pixelRect.left   = (SInt16) (pixelRect.right - 1);
			pixelRect.bottom = (SInt16) (pixelRect.top + 1);
			::MacFrameRect ( &pixelRect );

			pixelRect = portFrame;
			pixelRect.right = (SInt16) (pixelRect.left + 1);
			pixelRect.top   = (SInt16) (pixelRect.bottom - 1);
			::MacFrameRect ( &pixelRect );

												// If there is a notch we need to handle things a
												// little differently in the bottom right corner
			if ( NotchInsetFrame ()) {
				Rect	notchRect = portFrame;
				notchRect.left = (SInt16) (notchRect.right - mNotchWidth);
				notchRect.top  = (SInt16) (notchRect.bottom - mNotchWidth);

				pixelRect = notchRect;
				pixelRect.left   = (SInt16) (pixelRect.right - 1);
				pixelRect.bottom = pixelRect.top;
				pixelRect.top--;
				::MacFrameRect ( &pixelRect );

				pixelRect = notchRect;
				pixelRect.right = pixelRect.left;
				pixelRect.left--;
				pixelRect.top = (SInt16) (pixelRect.bottom - 1);
				::MacFrameRect ( &pixelRect );

			} else {

				pixelRect = portFrame;
				pixelRect.left = (SInt16) (pixelRect.right - 1);
				pixelRect.top  = (SInt16) (pixelRect.bottom - 1);
				::MacFrameRect ( &pixelRect );
			}
		}
												// Now if we need to notch out the frame if there
												// is a notch
		if ( NotchInsetFrame ()) {
			Rect	notchRect = portFrame;
			notchRect.left = (SInt16) (notchRect.right - mNotchWidth);
			notchRect.top  = (SInt16) (notchRect.bottom - mNotchWidth);
												//	Create a region fromn the notch
			StRegion notchRgn ( notchRect );
												// Remove the no0tch from the main region
			focusRgn -= notchRgn;
		}
												//	Remove the corner pixels from the main region
		focusRgn -= cornerRgn;

	}
												// Now that is all done we can return the region to
												// the caller who will be responsible for disposing
												// of it when done
	return focusRgn.Release ();

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetPaintBorderFace											  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetPaintBorderFace(
	Boolean	inPaintFace,
	Boolean	inRedraw)
{
	if (mPaintBorderFace != inPaintFace) {

		mPaintBorderFace = inPaintFace;

		if (inRedraw) {
			RefreshBorder ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetInsetSubPane												  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetInsetSubPane(
	LPane*	inInsetSubPane)
{
												// We only change the sub pane if we have too, in
												// addition to setting the reference we also need
												// to make sure that we get things invalidated so
												// that the new sub pane shows up and gets framed
												// if requested
	if (mInsetSubPane != inInsetSubPane) {

												// Before we switch to the new inset subpane make
												// sure we refresh the old so that it is erased,
												// this only happens if there actually is one
		RefreshInsetSubPane();

												// Now we can setup the new reference
		mInsetSubPane = inInsetSubPane;

												// Now we get the new subpane refreshed so that it
												// will show up, NOTE: this will only happen if
												// there is one as there are many occasions where
												// the subpane could be set to nil, this check is
												// done in the refresh method
		RefreshInsetSubPane();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetInsetSubPaneByID											  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetInsetSubPaneByID(
	PaneIDT	inInsetSubPaneID)
{
												// Setup our reference to the inset sub pane we
												// will be framing if requested by the user, if we
												// are that view then we do nothing or if there is
												// no pane ID specified we simply set the sub pane
												// to nil
	if ( inInsetSubPaneID != PaneIDT_Undefined ) {

		mInsetSubPaneID = inInsetSubPaneID;

												// Locate the subpane and set our reference to it,
												// if we are the subpane set the reference to nil
		mInsetSubPane = FindPaneByID ( inInsetSubPaneID );
		if ( mInsetSubPane == this )
			mInsetSubPane = nil;

	} else {

		mInsetSubPane = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCommanderSubPaneByID										  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetCommanderSubPaneByID(
	PaneIDT	inCommanderSubPaneID)
{

	if ( mCommanderSubPaneID != inCommanderSubPaneID ) {

		mCommanderSubPaneID = inCommanderSubPaneID;

										// Debug check for consistent settings:
										// CanFocus requires a Commander SubPane
		SignalIf_( mCanFocus && (mCommanderSubPaneID == PaneIDT_Unspecified) );

												// We need to make sure if the user has specified
												// that we are the focused border that our static
												// member is setup
		if ( mCanFocus && HasFocus() ) {
												// Make sure that we get rid of any current focus
			if ( sFocusedBorder ) {
				sFocusedBorder->BecomeFocused ( false );
			}
												// Then set us up as the current focus
			sFocusedBorder = this;

		}

												// Setup a reference to the commander
		LCommander *commanderSubPane = dynamic_cast<LCommander *>(FindPaneByID ( mCommanderSubPaneID ));
		ThrowIfNil_ ( commanderSubPane );
		if ( commanderSubPane == this ) {
			commanderSubPane = nil;
		}
												// Make sure that the associated commander is set
												// to latent
		SetLatentSub ( commanderSubPane );
	}
}


// ---------------------------------------------------------------------------
//	¥ BecomeFocused													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::BecomeFocused(
	Boolean	inBecomeFocused,
	Boolean	inRefresh)
{
	if (HasFocus() != inBecomeFocused) {	// State is changing

		if (inBecomeFocused) {

			if (sFocusedBorder != nil) {	// Unfocus current border
				sFocusedBorder->BecomeFocused(false, Refresh_Yes);
			}

			sFocusedBorder = this;			// This border is focused

		} else {

			sFocusedBorder = nil;			// No current focus border

		}

		if (inRefresh) {
			RefreshFocusBorder();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCanFocus													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetCanFocus(
	Boolean	inCanFocused)
{
	mCanFocus = inCanFocused;
}


// ---------------------------------------------------------------------------
//	¥ SetNotchWidth													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SetNotchWidth(
	SInt16		inNotchWidth,
	Boolean		inRedraw)
{
	if (GetNotchWidth() != inNotchWidth) {

		mNotchWidth = inNotchWidth;

		if (inRedraw) {
			RefreshBorder ();
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::ActivateSelf()
{
												// We are only going to force a redraw of the frame
												// around the inset frame if there is an inset pane,
												// this should minimise the amount of flashing
	if ( FrameInsetSubPane () && IsVisible ()) {
		RefreshInsetFrameBorder ();
	}
												// Because we have tweaked the refreshing such that
												// we don't always do a full refresh of the border
												// we need to make sure that we get the focus
												// border refreshed if needed
	if ( CanFocus () && HasFocus ()) {
		RefreshFocusBorder ();
	}
}



// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::DeactivateSelf()
{
												// We are only going to force a redraw of the frame
												// around the inset frame if there is an inset pane,
												// this should minimise the amount of flashing, we
												// will also only do this when we are visible
	if ( FrameInsetSubPane () && IsVisible ()) {
		RefreshInsetFrameBorder ();
	}
												// Because we have tweaked the refreshing such that
												// we don't always do a full refresh of the border
												// we need to make sure that we get the focus
												// border refreshed if needed
	if ( CanFocus () && HasFocus ()) {
		RefreshFocusBorder ();
	}
}


// ---------------------------------------------------------------------------
//	¥ Activate														  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::Activate()
{
	LView::Activate();
	UpdatePort();
}


// ---------------------------------------------------------------------------
//	¥ Deactivate													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::Deactivate()
{
	LView::Deactivate();
	UpdatePort();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------

Boolean
LGAFocusBorder::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean cmdHandled;

	switch ( inCommand ) {

		case msg_TabSelect:
			cmdHandled = false;
			break;

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ RefreshBorder													  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::RefreshBorder()
{
	StRegion	faceRgn(GetBorderRegion(), false);
	RefreshRgn(faceRgn);
}


// ---------------------------------------------------------------------------
//	¥ RefreshInsetFrameBorder										  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::RefreshInsetFrameBorder()
{
	StRegion	insetRgn(GetInsetFrameRegion(), false);
	RefreshRgn(insetRgn);
}


// ---------------------------------------------------------------------------
//	¥ RefreshFocusBorder											  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::RefreshFocusBorder()
{
#if PP_Target_Carbon

		// On Carbon, the Appearance Manager draws the focus
		// border and there is a metric to determine its thickness.
		
		// Region to refresh is just the border, and not its interior.
		// We start with the region around which the focus border draws.
								
	StRegion	focusRgn(GetThemeFocusRegion(), false);
	
	StRegion	borderRgn(focusRgn);	// Make copy of focus region
	
	focusRgn.InsetBy(1, 1);				// Border overlaps focus region
										//   by one pixel
										
		// Find out how far the border is outset from the focused region.
		// We assume that a focus region has the same thickness as
		// a focus rectangle.
	
	SInt32		outset;
	::GetThemeMetric(kThemeMetricFocusRectOutset, &outset);
	
		// Grow border region to account for the outset, then subtract
		// the interior region.
	
	borderRgn.InsetBy((SInt16)(-outset), (SInt16)(-outset));
	borderRgn -= focusRgn;
	
	RefreshRgn(borderRgn);

#else

	StRegion	focusRgn(GetFocusRegion(), false);
	RefreshRgn(focusRgn);

#endif
}


// ---------------------------------------------------------------------------
//	¥ RefreshInsetSubPane											  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::RefreshInsetSubPane()
{
	if (mInsetSubPane != nil) {

		mInsetSubPane->Refresh ();		// First refresh the pane itself

		if (FrameInsetSubPane()) {		// Then, refresh it's border
			RefreshInsetFrameBorder();
		}

		if (CanFocus () and HasFocus()) {
			RefreshFocusBorder();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Draw															  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	LView::Draw(inSuperDrawRgnH);
	
		// Draw FocusBorder AFTER subpanes, because border may overlap
		// the frame of the inset subpane
	
	if (FocusExposed()) {
	
		if ( CanFocus() && HasFocus() && mInsetSubPane &&
			 mInsetSubPane->IsEnabled() && mInsetSubPane->IsActive()) {

			SInt16	depth;
			bool	hasColor;
			GetDeviceInfo(depth, hasColor);

			DrawFocusBorder( depth );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::DrawSelf()
{
	StColorPenState::Normalize();

	if (WantsFacePainted()) {			// Paint face with the background color

		StRegion faceRgn( GetBorderRegion (), false );

		ApplyForeAndBackColors();
		::EraseRgn(faceRgn);
	}

	SInt16	depth;
	bool	hasColor;
	GetDeviceInfo(depth, hasColor);

	DrawBorder( depth );				// Draw border around inset subpane
}


// ---------------------------------------------------------------------------
//	¥ DrawBorder												   [protected]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::DrawBorder(
	SInt16		inBitDepth)
{
	RGBColor	tempColor;

	if (mInsetSubPane) {
		tempColor = Color_Black;
		Rect	insetRect;

		bool	activeAndEnabled = mInsetSubPane->IsActive() and
								   mInsetSubPane->IsEnabled();

												// Figure out the inset pane's bounding rect
		mInsetSubPane->CalcPortFrameRect ( insetRect );
		PortToLocalPoint ( topLeft ( insetRect ));
		PortToLocalPoint ( botRight ( insetRect ));

		if ( inBitDepth >= 4 ) {			// COLOR
												// If requested then we go ahead and frame the
												// inset sub pane if there is one
			if (FrameInsetSubPane()) {
												// Now we can proceed with the drawing of the frame
												// around the inset sub pane
												// BLACK INNER FRAME
												// TOPLEFT EDGES
				if (not activeAndEnabled) {
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				}

				::RGBForeColor ( &tempColor );
				UGraphicUtils::TopLeftSide ( insetRect, -1, -1, -1, -1 );

											// BOTTOM RIGHT EDGES
												// If a notch has been requested then we draw the
												// frame with a notch otherwise we simply frame it
				if ( NotchInsetFrame ()) {
					UGraphicUtils::BottomRightSideWithNotch ( insetRect, -1, -1, -1, -1, mNotchWidth );
				} else {
					UGraphicUtils::BottomRightSide  ( insetRect, -1, -1, -1, -1 );
				}
				::MacInsetRect(&insetRect, -1, -1);
			}

			if (WantsFacePainted() &&			// if we want to draw shadows
				activeAndEnabled &&				// and shadows are called for
				(!HasFocus() || !CanFocus())) {	// and we're not drawing the focus ring
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				::RGBForeColor ( &tempColor );
				UGraphicUtils::TopLeftSide ( insetRect, -1, -1, 0, 0 );

											// BOTTOM RIGHT LIGHT EDGES
				::RGBForeColor ( &Color_White );

												// If a notch has been requested then handle it,
												// otherwise just do the edge drawing
				if ( NotchInsetFrame ()) {
					UGraphicUtils::BottomRightSideWithNotch ( insetRect, 0, 0, -1, -1, mNotchWidth );
				} else {
					UGraphicUtils::BottomRightSide ( insetRect, 0, 0, -1, -1 );
				}
			}

		} else {							// BLACK & WHITE
												// Now we can proceed with the drawing of the frame
												// around the inset sub pane
												// BLACK INNER FRAME
												// TOPLEFT EDGES
			if (FrameInsetSubPane()) {
				::RGBForeColor ( &Color_Black );
				UGraphicUtils::TopLeftSide ( insetRect, -1, -1, -1, -1 );

												// BOTTOM RIGHT EDGES
													//	If a notch has been requested then we draw the
													// frame with a notch otherwise we simply frame it
				if ( NotchInsetFrame ()) {
					UGraphicUtils::BottomRightSideWithNotch ( insetRect, -1, -1, -1, -1, mNotchWidth );
				} else {
					UGraphicUtils::BottomRightSide  ( insetRect, -1, -1, -1, -1 );
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawFocusBorder											   [protected]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::DrawFocusBorder(
	SInt16		inBitDepth)
{
	StColorPenState::Normalize();

	RGBColor	tempColor;
											// Don't even bother if there isn't an inset subpane
	if ( mInsetSubPane ) {

		if (UEnvironment::HasFeature(env_HasAppearance101)) {
											// Appearance Mgr draws the border

			StRegion focusRgn ( GetThemeFocusRegion (), false );
			::DrawThemeFocusRegion ( focusRgn, true );

		} else {

			RGBColor	accentColor;
			UGAColorRamp::GetFocusAccentColor(accentColor);

												// Figure out the inset pane's bounding rect
			Rect	insetRect;
			mInsetSubPane->CalcPortFrameRect ( insetRect );
			PortToLocalPoint ( topLeft ( insetRect ));
			PortToLocalPoint ( botRight ( insetRect ));

												// Setup the focus color which is the user
												// accent color for color and black for B&W
			if (inBitDepth < 4) {
				accentColor = Color_Black;
			}
			::RGBForeColor ( &accentColor );

			SInt16	inset = -1;					// Inset amount depends on whether we are
			if (FrameInsetSubPane()) {			// drawing a frame around the sub pane
				inset = -2;
			}
			::MacInsetRect(&insetRect, inset, inset);

												// INNER RING
			if ( NotchInsetFrame ()) {
												// TOP LEFT EDGES
				UGraphicUtils::TopLeftSide ( insetRect, 0, 0, 0, 0 );

												// BOTTOM RIGHT EDGES
				UGraphicUtils::BottomRightSideWithNotch (
											insetRect,
											0,
											0,
											0,
											0,
											mNotchWidth );

			} else {

				::MacFrameRect ( &insetRect );
			}

												// Make the rect larger by one pixel
			::MacInsetRect ( &insetRect, -1, -1 );

												// OUTER RING
			if ( NotchInsetFrame ()) {
												// TOP LEFT EDGES
				::MoveTo ( insetRect.left, (SInt16) (insetRect.bottom - 2) );
				::MacLineTo ( insetRect.left, (SInt16) (insetRect.top + 1) );
				::MoveTo ( (SInt16) (insetRect.left + 1), insetRect.top );
				::MacLineTo ( (SInt16) (insetRect.right - 2), insetRect.top );

												// BOTTOM RIGHT EDGES
				UGraphicUtils::BottomRightSideWithNotch (
											insetRect,
											1,
											1,
											0,
											0,
											mNotchWidth );

												// Fixup a couple of corner pixels
				tempColor = UGAColorRamp::GetColor(2);
				if (inBitDepth < 4) {
					tempColor = Color_White;
				}

				UGraphicUtils::PaintColorPixel (
									(SInt16) (insetRect.right - (mNotchWidth + 1)),
									(SInt16) (insetRect.bottom - 1),
									tempColor );
				UGraphicUtils::PaintColorPixel (
									(SInt16) (insetRect.right - 1),
									(SInt16) (insetRect.bottom - (mNotchWidth + 1)),
									tempColor );

			} else {

				UGraphicUtils::NoCornerPixelFrame ( insetRect );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ PutOnDuty
// ---------------------------------------------------------------------------

void
LGAFocusBorder::PutOnDuty(
	LCommander*	/* inNewTarget */)
{
	BecomeFocused(true, Refresh_Yes);
}


// ---------------------------------------------------------------------------
//	¥ TakeOffDuty
// ---------------------------------------------------------------------------

void
LGAFocusBorder::TakeOffDuty()
{
	BecomeFocused(false, Refresh_Yes);
}


// ---------------------------------------------------------------------------
//	¥ SubTargetChanged											   [protected]
// ---------------------------------------------------------------------------

void
LGAFocusBorder::SubTargetChanged()
{
	if (sFocusedBorder == nil) {

		if ( CanFocus()  and
			 (mInsetSubPane != nil)  and
			 mInsetSubPane->IsEnabled()  and
			 mInsetSubPane->IsActive() ) {

			BecomeFocused(true);
		}
	}
}


PP_End_Namespace_PowerPlant
