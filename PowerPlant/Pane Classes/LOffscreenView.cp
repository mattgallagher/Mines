// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOffscreenView.cp			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A View whose image draws offscreen and then gets copied to the screen

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LOffscreenView.h>
#include <PP_Messages.h>
#include <TArrayIterator.h>
#include <UEnvironment.h>
#include <UGWorld.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LOffscreenView						Constructor				  [public]
// ---------------------------------------------------------------------------

LOffscreenView::LOffscreenView()
{
	mOffscreenWorld = nil;
	mDrawingSelf	= false;
	mMousingSelf	= false;
}


// ---------------------------------------------------------------------------
//	¥ LOffscreenView						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LOffscreenView::LOffscreenView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)

	: LView(inPaneInfo, inViewInfo)
{
	mOffscreenWorld = nil;
	mDrawingSelf	= false;
	mMousingSelf	= false;
}


// ---------------------------------------------------------------------------
//	¥ LOffscreenView						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LOffscreenView::LOffscreenView(
	LStream*	inStream)

	: LView(inStream)
{
	mOffscreenWorld = nil;
	mDrawingSelf	= false;
	mMousingSelf	= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LOffscreenView						Destructor				  [public]
// ---------------------------------------------------------------------------

LOffscreenView::~LOffscreenView()
{
}


// ---------------------------------------------------------------------------
//	¥ EstablishPort													  [public]
// ---------------------------------------------------------------------------
//	Set current port to the OffscreenView
//
//	If this View is being drawn, the current port should be the Offscreen
//	GWorld. Otherwise, some subpane is trying to draw itself directly (not
//	in response to an update event) so we call the inherited function to do
//	the normal thing.

Boolean
LOffscreenView::EstablishPort()
{
	Boolean	portSet = (mOffscreenWorld != nil);

	if (mDrawingSelf) {
		if ( portSet &&
			 (UQDGlobals::GetCurrentPort() != (GrafPtr) mOffscreenWorld) ) {

			::SetGWorld(mOffscreenWorld, nil);
		}

	} else {
		portSet = LView::EstablishPort();
	}

	return portSet;
}


// ---------------------------------------------------------------------------
//	¥ Draw															  [public]
// ---------------------------------------------------------------------------
//	Draw contents of View offscreen and then copy them onscreen.
//
//	First, we try to draw offscreen using temporary memory.
//	If that fails, we try to draw offscreen using application memory.
//	If all else fails, we call the inherited version to draw directly to
//	the screen.

void
LOffscreenView::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	try {
		DrawOffscreen(inSuperDrawRgnH, useTempMem);
	}

	catch (...) {
		try {
			DrawOffscreen(inSuperDrawRgnH);
		}

		catch (...) {
			if (FocusExposed()) {

					// Offscreen drawing erases to the background,
					// so we must do it here when drawing to screen.

				Rect	frame;
				CalcLocalFrameRect(frame);

				ApplyForeAndBackColors();
				::EraseRect(&frame);

				LView::Draw(inSuperDrawRgnH);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawOffscreen												   [protected]
// ---------------------------------------------------------------------------
//	Draw contents of View to an Offscreen World, then copy bits all at
//	once to the normal port.
//
//	Use the given flags when creating the offscreen GWorld.

void
LOffscreenView::DrawOffscreen(
	RgnHandle	inSuperDrawRgnH,
	GWorldFlags	inFlags)
{
	if (FocusExposed()) {			// Don't draw if unable to focus

									// Determine area to draw

		mUpdateRgn = mRevealedRect;	// Start with the revealed area

									// Intersect with draw area of SuperView
		if (inSuperDrawRgnH != nil) {
			mUpdateRgn &= inSuperDrawRgnH;
		}

		GrafPtr	currentPort;		// Intersect with Port's visible region
		::GetPort(&currentPort);

		{
			StRegion	visibleRgn;
			::GetPortVisibleRegion(currentPort, visibleRgn);

									// Convert to Port coordinates
			::MacOffsetRgn(visibleRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);
			mUpdateRgn &= visibleRgn;
		}

		if (!mUpdateRgn.IsEmpty()) {
									// Some portion needs to be drawn

									// Set up Offscreen World for drawing

									// Bounds of Offscreen World are the
									//   bounds of the region of this View
									//   that needs to be redrawn
			Rect	offBounds = mUpdateRgn.Bounds();
			::MacOffsetRect(&offBounds, mPortOrigin.h, mPortOrigin.v);

									// Save portRect of the current Port
			Rect	onscreenPortRect;
			::GetPortBounds(currentPort, &onscreenPortRect);

									// Determine the proper background for
									//   the Offscreen World.
			RGBColor	backColor;
			RGBColor*	backColorPtr = nil;

			if (UEnvironment::GetAppearanceVersion() < 0x0110) {
									// No Appearance Manager. Just get the
									// SuperView's background color
				backColorPtr = &backColor;
				GetForeAndBackColors(nil, backColorPtr);

			} else {				// Let Appearance Manager apply the proper
									//   background, which may be a color
									//   and/or pattern
				ApplyForeAndBackColors();
			}

									// Allocate a stack-based Offscreen World.
									//   Subsequent drawing will take place in
									//   this Offscreen World.

			StOffscreenGWorld	offWorld(offBounds, sBitDepth, inFlags,
											nil, nil, backColorPtr);

									// To keep coord systems in synch, set
									//   the portRect of the Offscreen World
									//   to that of the original port
			mOffscreenWorld = offWorld.GetMacGWorld();
			::SetPortBounds(mOffscreenWorld, &onscreenPortRect);

									// StValueChanger will save/restore the
									// current bit depth. We set the bit
									// depth to that of the Offscreen World.
			StValueChanger<SInt16>	bitDepth(
					sBitDepth,
					::GetPixDepth( ::GetPortPixMap(mOffscreenWorld) ));

									// Prevents subpanes from changing Ports
			StValueChanger<bool>	drawingSelf(mDrawingSelf, true);

			OutOfFocus(nil);		// Offscreen World needs to be focused
			FocusDraw();

									// Draw this view and its subpanes
									// into the Offscreen World. Offscreen
									// image will be copied to original port
									// when offWorld's destructor is called.
			Rect	frame;
			CalcLocalFrameRect(frame);
			if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
				DrawSelf();
			}
			TArrayIterator<LPane*> iterator(mSubPanes);
			LPane	*theSub;
			while (iterator.Next(theSub)) {
				theSub->Draw(mUpdateRgn);
			}

			OutOfFocus(nil);		// Make sure Offscreen World is focused
			FocusDraw();			//   since it will actually draw at the
									//   end of this scope
		}

		mUpdateRgn.Clear();			// Emptying update region frees up memory
									//   if this region wasn't rectangular
	}
}


// ---------------------------------------------------------------------------
//	¥ SubImageChanged												  [public]
// ---------------------------------------------------------------------------
//	Notification that the Image of some SubView changed size, location,
//	or scroll units.
//
//	In this override, we pass the notification up the chain. This is because
//	the LOffscreenView isn't really the logical superview of its subpanes;
//	it's just in the hierarchy to draw offscreen. So, we pass the notification
//	up to the logical superview of its subpanes.

void
LOffscreenView::SubImageChanged(
	LView*	inSubView)
{
	if (mSuperView != nil) {
		mSuperView->SubImageChanged(inSubView);
	}
}


// ---------------------------------------------------------------------------
//	¥ Click
// ---------------------------------------------------------------------------
//	In this override, we treat the offscreen view as transparent. If the click
//	isn't in some subpane, we set a flag and redispatch the click so that the
//	SuperView can handle the click as if the offscreen view weren't present.

void
LOffscreenView::Click(
	SMouseDownEvent& inMouseDown)
{
	LPane* clickedPane = FindSubPaneHitBy(inMouseDown.wherePort.h,
										  inMouseDown.wherePort.v);

	if (clickedPane != nil) {		// Click in a subpane
		clickedPane->Click(inMouseDown);

	}  else if (mSuperView != nil) {
									// Click in this view. We don't handle
									//   clicks, so set flag and
									//   redispatch the click.
		StValueChanger<bool> mousing(mMousingSelf, true);
		mSuperView->Click(inMouseDown);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouse
// ---------------------------------------------------------------------------
//	In this override, we treat the offscreen view as transparent. This
//	mirrors the override to Click(), so that the Pane that handles the
//	click gets to adjust the cursor.

void
LOffscreenView::AdjustMouse(
	Point				inPortPt,
	const EventRecord	&inMacEvent,
	RgnHandle			outMouseRgn)
{
									// Check if a SubPane of this View
									//   contains the point
	LPane	*hitPane = FindSubPaneHitBy(inPortPt.h, inPortPt.v);

	if (hitPane != nil) {			// SubPane is hit, let it adjust the
									//   cursor shape
		hitPane->AdjustMouse(inPortPt, inMacEvent, outMouseRgn);

	} else if (mSuperView != nil) {
									// Cursor in this view. We don't handle
									//   cursors, so set flag and
									//   redispatch the call.
		StValueChanger<bool> mousing(mMousingSelf, true);
		mSuperView->AdjustMouse(inPortPt, inMacEvent, outMouseRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ IsHitBy
// ---------------------------------------------------------------------------
//	Override to return that LOffscreenView isn't hit if the mMousingSelf
//	flag is set. Click() and AdjustMouse() set the flag to treat offscreen
//	views as transparent to the mouse.

Boolean
LOffscreenView::IsHitBy(
	SInt32	inHorizPort,
	SInt32	inVertPort)
{
	return ( not mMousingSelf &&
			 LView::IsHitBy(inHorizPort, inVertPort) );
}


PP_End_Namespace_PowerPlant
