// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LView.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Pane subclass which supports scrolling and SubPanes

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LView.h>
#include <TArrayIterator.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingUtils.h>
#include <UMemoryMgr.h>
#include <UReanimator.h>
#include <UTBAccessors.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Constants

const SInt16		max_PortOrigin	= 16384;
const SInt16		min_PortOrigin	= -32768;
const SInt32		mask_Lo14Bits	= 0x00003FFF;


// ---------------------------------------------------------------------------
//	Class Variables

LView*	LView::sInFocusView = nil;


// ---------------------------------------------------------------------------
//	¥ LView									Default Constructor		  [public]
// ---------------------------------------------------------------------------

LView::LView()
{
	SViewInfo	viewInfo;

	viewInfo.imageSize.width = viewInfo.imageSize.height = 0;
	viewInfo.scrollPos.h = viewInfo.scrollPos.v = 0;
	viewInfo.scrollUnit.h = viewInfo.scrollUnit.v = 1;
	viewInfo.reconcileOverhang = false;

	InitView(viewInfo);
}


// ---------------------------------------------------------------------------
//	¥ LView									Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Does shallow copy; SubPanes are not copied.

LView::LView(
	const LView&	inOriginal)

	: LPane(inOriginal)
{
	SViewInfo	viewInfo;

	viewInfo.imageSize		= inOriginal.mImageSize;
	viewInfo.scrollPos.h	= inOriginal.mFrameLocation.h -
									inOriginal.mImageLocation.h;
	viewInfo.scrollPos.v	= inOriginal.mFrameLocation.v -
									inOriginal.mImageLocation.v;
	viewInfo.scrollUnit		= inOriginal.mScrollUnit;
	viewInfo.reconcileOverhang = inOriginal.mReconcileOverhang;

	InitView(viewInfo);
}


// ---------------------------------------------------------------------------
//	¥ LView									Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LView::LView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo)
	
	: LPane(inPaneInfo)
{
	InitView(inViewInfo);
}


// ---------------------------------------------------------------------------
//	¥ LView									Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LView::LView(
	LStream*	inStream)
	
	: LPane(inStream)
{
	SViewInfo	viewInfo;
	*inStream >> viewInfo.imageSize.width;
	*inStream >> viewInfo.imageSize.height;
	*inStream >> viewInfo.scrollPos.h;
	*inStream >> viewInfo.scrollPos.v;
	*inStream >> viewInfo.scrollUnit.h;
	*inStream >> viewInfo.scrollUnit.v;
	*inStream >> viewInfo.reconcileOverhang;

	InitView(viewInfo);
}


// ---------------------------------------------------------------------------
//	¥ LView									Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LView::LView(
	LStream*	inStream,
	bool		/* inNoViewData */)
	
	: LPane(inStream)
{
	SViewInfo	viewInfo;

	viewInfo.imageSize.width = viewInfo.imageSize.height = 0;
	viewInfo.scrollPos.h = viewInfo.scrollPos.v = 0;
	viewInfo.scrollUnit.h = viewInfo.scrollUnit.v = 1;
	viewInfo.reconcileOverhang = false;
	
	InitView(viewInfo);
}


// ---------------------------------------------------------------------------
//	¥ ~LView								Destructor				  [public]
// ---------------------------------------------------------------------------
//

LView::~LView()
{
	DeleteAllSubPanes();			// Delete Panes contained by this View

	OutOfFocus(this);				// Don't leave a dangling Focus

	if (sDefaultView == this) {
		SetDefaultView(nil);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InitView								Initializer			   [protected]
// ---------------------------------------------------------------------------

void
LView::InitView(
	const SViewInfo&	inViewInfo)
{
	mImageSize = inViewInfo.imageSize;

	mScrollUnit = inViewInfo.scrollUnit;
	if (mScrollUnit.h < 1) {		// ScrollUnit must be a positive number
		mScrollUnit.h = 1;
	}
	if (mScrollUnit.v < 1) {
		mScrollUnit.v = 1;
	}

	mReconcileOverhang = (inViewInfo.reconcileOverhang == 1);
	mImageLocation.h = mImageLocation.v = 0;
	mPortOrigin = Point_00;
	mRevealedRect = Rect_0000;

	ScrollImageTo(inViewInfo.scrollPos.h, inViewInfo.scrollPos.v, Refresh_No);
	CalcRevealedRect();
	
	SetDefaultView(this);
}


// ---------------------------------------------------------------------------
//	¥ FinishCreate													  [public]
// ---------------------------------------------------------------------------
//	Wrapper function for FinishCreateSelf
//	You will rarely want to override this function
//
//	SubPanes are told to FinishCreateSelf *before* their SuperView.
//	Therefore, a SupverView is assured that all its SubPanes have
//	finished creating when its FinishCreateSelf function gets called.

void
LView::FinishCreate()
{
	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->FinishCreate();
	}

	if (mSuperView != nil) {
		mSuperView->OrientSubPane(this);
	}

	if (ExecuteAttachments(msg_FinishCreate, this)) {
		FinishCreateSelf();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AddSubPane
// ---------------------------------------------------------------------------
//	You should not normally call this function. Call LPane::PutInside()
//	to associate a Pane with a SuperView.

void
LView::AddSubPane(
	LPane	*inSub)
{
	mSubPanes.AddItem(inSub);
}


// ---------------------------------------------------------------------------
//	¥ RemoveSubPane
// ---------------------------------------------------------------------------
//	You should not normally call this function. Call LPane::PutInside(nil)
//	to remove a SubPane from its SuperView

void
LView::RemoveSubPane(
	LPane	*inSub)
{
	mSubPanes.Remove(inSub);
}


// ---------------------------------------------------------------------------
//	¥ DeleteAllSubPanes
// ---------------------------------------------------------------------------

void
LView::DeleteAllSubPanes()
{
	TArrayIterator<LPane*> iterator(mSubPanes, LArrayIterator::from_End);
	LPane	*theSub;
	while (iterator.Previous(theSub)) {
		mSubPanes.RemoveItemsAt(1, iterator.GetCurrentIndex());
		delete theSub;
	}
}


// ---------------------------------------------------------------------------
//	¥ OrientSubPane
// ---------------------------------------------------------------------------
//	Adjust the Enabled, Active, and Visible properties of a SubPane based
//	on the properties of this View (which must be the SubPane's SuperView).

void
LView::OrientSubPane(
	LPane	*inSub)
{
	if (mEnabled == triState_On) {
		inSub->SuperEnable();
	} else {
		inSub->SuperDisable();
	}

	if (mActive == triState_On) {
		inSub->SuperActivate();
	} else {
		inSub->SuperDeactivate();
	}

	if (mVisible == triState_On) {
		inSub->SuperShow();
	} else {
		inSub->SuperHide();
	}
}


// ---------------------------------------------------------------------------
//	¥ OrientAllSubPanes
// ---------------------------------------------------------------------------
//	Adjust the Enabled, Active, and Visible properties of all SubPanes
//
//	This function is for the convenience of people who have code from
//	CW5 (PowerPlant 1.0.2) and earlier which created Panes directly in the
//	code (not from 'PPob' resources). You really should call FinishCreate()
//	for each Pane, but to quickly get code running, you can call this
//	function after creating several Panes inside one View.
//
//	Do NOT use this function for new code.

void
LView::OrientAllSubPanes()
{
	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		OrientSubPane(theSub);
	}
}


// ---------------------------------------------------------------------------
//	¥ ExpandSubPane
// ---------------------------------------------------------------------------
//	Adjust the size and location of a SubPane to fit within the interior
//	of a View in the horizontal and/or vertical directions

void
LView::ExpandSubPane(
	LPane	*inSub,
	Boolean	inExpandHoriz,
	Boolean	inExpandVert)
{
	SDimension16	subSize;
	inSub->GetFrameSize(subSize);
	SPoint32		subLocation;
	inSub->GetFrameLocation(subLocation);

	if (inExpandHoriz) {
		subSize.width = mFrameSize.width;
		subLocation.h = 0;
	} else {
		subLocation.h -= mFrameLocation.h;
	}

	if (inExpandVert) {
		subSize.height = mFrameSize.height;
		subLocation.v = 0;
	} else {
		subLocation.v -= mFrameLocation.v;
	}

	inSub->PlaceInSuperFrameAt(subLocation.h, subLocation.v, false);
	inSub->ResizeFrameTo(subSize.width, subSize.height, false);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetImageSize
// ---------------------------------------------------------------------------
//	Pass back the dimensions of a View's Image

void
LView::GetImageSize(
	SDimension32	&outSize) const
{
	outSize = mImageSize;
}


// ---------------------------------------------------------------------------
//	¥ GetScrollPosition
// ---------------------------------------------------------------------------
//	Pass back the location of a View's Frame within its Image

void
LView::GetScrollPosition(
	SPoint32	&outScrollPosition) const
{
	outScrollPosition.h = mFrameLocation.h - mImageLocation.h;
	outScrollPosition.v = mFrameLocation.v - mImageLocation.v;
}


// ---------------------------------------------------------------------------
//	¥ CalcRevealedRect
// ---------------------------------------------------------------------------
//	Calculate the portion of the Frame which is revealed through the
//	Frames of all SuperViews. RevealedRect is in Port coordinates.

void
LView::CalcRevealedRect()
{
									// Start with the Frame
	if (CalcPortFrameRect(mRevealedRect)) {
									// Frame is in QD Space
		if (mSuperView != nil) {	// Intersect Frame with RevealedRect
									//   of SuperView
			Rect	superRevealed;
			mSuperView->GetRevealedRect(superRevealed);
			::SectRect(&superRevealed, &mRevealedRect, &mRevealedRect);
		}

	} else {						// Frame not in QD Space
		mRevealedRect.left =		//   so RevealedRect is empty
			mRevealedRect.top =
			mRevealedRect.right =
			mRevealedRect.bottom = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcPortOrigin
// ---------------------------------------------------------------------------
//	Calculate the coordinate origin for the Port needed to set up the
//	Local coordinates of a View
//
//	The Port origin must be in 16-bit space. In fact, the limitation is more
//	restrictive because the entire Port must be in 16-bit space. The origin
//	is the top left corner. To make sure that the bottom right corner is
//	in 16-bit space, we force the origin to be less than 2^14 (16,384),
//	which allows Port dimensions of a maximum of 16,384 pixels. At 72 dpi,
//	this is about 227 inches or 19 feet (much more screen or printer area
//	than you can get with current technology).
//
//	This means that Local and Image coordinates will be the same when the
//	Image size is less than 16,384 pixels. For Images greater than this,
//	you can't use absolute coordinates for drawing. You need to offset
//	the coordinates using ImageToLocalPoint() and LocalToImagePoint().
//
//	The true coordinates offset is the distance between the top left corners
//	of the Image and the Port. If this offset is greater than 2^14, we
//	use an effective offset that is the true offset modulo 2^14:
//		effective_offset = true_offset modulo 2^14
//	Using this effective offset maintains the bit-wise alignment of the Port
//	with respect to base-2 byte boundaries. This is important for drawing
//	Toolbox Patterns and PixPats, as well as for CopyBits calls.

void
LView::CalcPortOrigin()
{
	SInt32	coord = -mImageLocation.h;
	if (coord > max_PortOrigin) {	// coord is too big
		coord &= mask_Lo14Bits;		// use coord modulo 2^14
	} else if (coord < min_PortOrigin) {
		coord = min_PortOrigin;		// coord is too small, limit to minimum
	}
	mPortOrigin.h = (SInt16) coord;

	coord = -mImageLocation.v;
	if (coord > max_PortOrigin) {
		coord &= mask_Lo14Bits;
	} else if (coord < min_PortOrigin) {
		coord = min_PortOrigin;
	}
	mPortOrigin.v = (SInt16) coord;
}


// ---------------------------------------------------------------------------
//	¥ GetPortOrigin
// ---------------------------------------------------------------------------
//	Pass back the coordinate origin for the Port needed to set up the
//	Local coordinates of a View

void
LView::GetPortOrigin(
	Point	&outOrigin) const
{
	outOrigin = mPortOrigin;
}


// ---------------------------------------------------------------------------
//	¥ GetLocalUpdateRgn
// ---------------------------------------------------------------------------
//	Return the region, in local coordinates, to which update drawing is
//	clipped.
//
//	This region is non-empty only within the scope of a Draw() operation.
//	Call this function from a DrawSelf() function if you want to know
//	the actual area that requires redrawing. Most of the time, you will
//	just draw everything and let the clipping region mask out unnecessary
//	drawing. However, if your drawing is complex, your program might run
//	faster if you only draw the portion which is inside the update region.

RgnHandle
LView::GetLocalUpdateRgn()
{
									// Copy update region, which is in port
									//   coordinates
	StRegion	localUpdateRgn(mUpdateRgn);

									// Offset copy into local coordinates
	Point	localOffset = Point_00;
	PortToLocalPoint(localOffset);
	::MacOffsetRgn(localUpdateRgn, localOffset.h, localOffset.v);

									// Return copy, caller must dispose it
	return localUpdateRgn.Release();
}


// ---------------------------------------------------------------------------
//	¥ SetReconcileOverhang
// ---------------------------------------------------------------------------
//	Specify whether to reconcile the Frame and Image when there is overhang
//
//	See ReconcileFrameAndImage() for comments

void
LView::SetReconcileOverhang(
	Boolean	inSetting)
{
	mReconcileOverhang = inSetting;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		inWidthDelta and inHeightDelta specify, in pixels, how much larger
//		to make the Frame. Positive deltas increase the size, negative deltas
//		reduce the size.

void
LView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);

	CalcRevealedRect();
	OutOfFocus(this);

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->AdaptToSuperFrameSize(inWidthDelta, inHeightDelta, inRefresh);
	}

	ReconcileFrameAndImage(inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Image and Frame by the specified amounts
//
//		inHorizDelta and inVertDelta specify, in pixels, how far to move the
//		Frame (within its surrounding Image). Positive horiz deltas move to
//		the right, negative to the left. Positive vert deltas move down,
//		negative up.

void
LView::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	if ((inHorizDelta == 0) && (inVertDelta == 0)) {
		return;						// Do nothing if not moving
	}

	if (inRefresh) {
		Refresh();
	}

	mImageLocation.h += inHorizDelta;
	mImageLocation.v += inVertDelta;

	mFrameLocation.h += inHorizDelta;
	mFrameLocation.v += inVertDelta;

	CalcPortOrigin();
	CalcRevealedRect();
	OutOfFocus(this);

	if (inRefresh) {
		Refresh();
	}

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->MoveBy(inHorizDelta, inVertDelta, false);
	}
}


// ---------------------------------------------------------------------------
//	¥ SavePlace
// ---------------------------------------------------------------------------
//	Write size and location information to a Stream for later retrieval
//	by the RestorePlace() function

void
LView::SavePlace(
	LStream		*outPlace)
{
	LPane::SavePlace(outPlace);			// Save info for this View
	
	*outPlace << mImageLocation.h;
	*outPlace << mImageLocation.v;
	*outPlace << mImageSize.width;
	*outPlace << mImageSize.height;

										// Save info for SubPanes
	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->SavePlace(outPlace);
	}
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace
// ---------------------------------------------------------------------------
//	Read size and location information stored in a Stream by the
//	SavePlace() function

void
LView::RestorePlace(
	LStream		*inPlace)
{
	LPane::RestorePlace(inPlace);		// Restore info for this View
	
	*inPlace >> mImageLocation.h;
	*inPlace >> mImageLocation.v;
	*inPlace >> mImageSize.width;
	*inPlace >> mImageSize.height;

	CalcPortOrigin();					// Recalibrate internal settings
	CalcRevealedRect();
										// Restore info for SubPanes
	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->RestorePlace(inPlace);
	}
	
	if (mSuperView != nil) {
		mSuperView->SubImageChanged(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdaptToNewSurroundings
// ---------------------------------------------------------------------------
//	Adjust view when its SuperView changes identity or size

void
LView::AdaptToNewSurroundings()
{
	CalcRevealedRect();

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->AdaptToNewSurroundings();
	}
}


// ---------------------------------------------------------------------------
//	¥ AdaptToSuperFrameSize
// ---------------------------------------------------------------------------
//	Adjust state of View when size of SuperView's Frame changes by the
//	specified amounts

void
LView::AdaptToSuperFrameSize(
	SInt32	inSurrWidthDelta,			// Change in width of SuperView
	SInt32	inSurrHeightDelta,			// Change in height of SuperView
	Boolean	inRefresh)
{
	LPane::AdaptToSuperFrameSize(inSurrWidthDelta, inSurrHeightDelta,
									inRefresh);
	AdaptToNewSurroundings();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Show
// ---------------------------------------------------------------------------
//	Make a View visible

void
LView::Show()
{
	LPane::Show();					// Show this View

	if (mVisible == triState_On) {	// If this View has become visible,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperShow();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperShow
// ---------------------------------------------------------------------------
//	The SuperView of a View has become visible

void
LView::SuperShow()
{
	LPane::SuperShow();				// Notify this View

	if (mVisible == triState_On) {	// If this View has become visible,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperShow();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Hide
// ---------------------------------------------------------------------------
//	Make a View invisible

void
LView::Hide()
{
	if (mVisible == triState_On) {
									// If this View is currently Visible,
									//   notify its SubPanes that it will
									//   be hidden
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperHide();
		}
	}

	LPane::Hide();					// Hide this View
}


// ---------------------------------------------------------------------------
//	¥ SuperHide
// ---------------------------------------------------------------------------
//	The SuperView of a View has been hidden

void
LView::SuperHide()
{
		// If this View is visible, hiding its SuperView will cause it
		// to become latently visible (effectively invisible)

	if (mVisible == triState_On) {
									// Notify SubPanes that their SuperView
									//   will be hidden
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperHide();
		}

		LPane::SuperHide();			// Notify this View
	}
}


// ---------------------------------------------------------------------------
//	¥ Activate
// ---------------------------------------------------------------------------
//	Activate a View

void
LView::Activate()
{
	LPane::Activate();				// Activate this View

	if (mActive == triState_On) {	// If this View has become active,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperActivate();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperActivate
// ---------------------------------------------------------------------------
//	The SuperView of a View has been Activated

void
LView::SuperActivate()
{
	LPane::SuperActivate();			// Notify this View

	if (mActive == triState_On) {	// If this View has become active,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperActivate();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Deactivate
// ---------------------------------------------------------------------------
//	Deactivate a View

void
LView::Deactivate()
{
	if (mActive == triState_On) {
									// If this View is currently Active,
									//   notify its SubPanes that it will
									//   be deactivated
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperDeactivate();
		}
	}

	LPane::Deactivate();			// Deactivate this View
}


// ---------------------------------------------------------------------------
//	¥ SuperDeactivate
// ---------------------------------------------------------------------------
//	The SuperView of a View has been Deactivated

void
LView::SuperDeactivate()
{
		// If this View is Active, Deactivating its SuperView will cause it
		// to become latently active (effectively inactive)

	if (mActive == triState_On) {
									// Notify SubPanes that their SuperView
									//   is being deactivate
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperDeactivate();
		}

		LPane::SuperDeactivate();	// Notify this View
	}
}


// ---------------------------------------------------------------------------
//	¥ Enable
// ---------------------------------------------------------------------------
//	Enable a View

void
LView::Enable()
{
	LPane::Enable();				// Enable this View

	if (mEnabled == triState_On) {	// If this View has become enabled,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperEnable();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperEnable
// ---------------------------------------------------------------------------
//	The SuperView of a View has been Enabled

void
LView::SuperEnable()
{
	LPane::SuperEnable();			// Notify this View

	if (mEnabled == triState_On) {	// If this View has become enabled,
									//   notify its SubPanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperEnable();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Disable
// ---------------------------------------------------------------------------
//	Disable a View

void
LView::Disable()
{
	if (mEnabled == triState_On) {
									// If this View is currently Enabled,
									//   notify its SubPanes that it will
									//   be disabled
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperDisable();
		}
	}

	LPane::Disable();				// Disable this View
}


// ---------------------------------------------------------------------------
//	¥ SuperDisable
// ---------------------------------------------------------------------------
//	The SuperView of a View has been Disabled

void
LView::SuperDisable()
{
		// If this View is Enabled, disabling its SuperView will cause it
		// to become latently enabled (effectively disabled)

	if (mEnabled == triState_On) {
									// Notify SubPanes that their SuperView
									//   is being disabled
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			theSub->SuperDisable();
		}

		LPane::SuperDisable();		// Notify this View
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ EstablishPort
// ---------------------------------------------------------------------------
//	Set the GrafPort for a View

Boolean
LView::EstablishPort()
{
	Boolean	portSet = false;		// A View does not store a GrafPort
	if (mSuperView != nil) {		//   Some SuperView must do it
		portSet = mSuperView->EstablishPort();
	}
	return portSet;
}


// ---------------------------------------------------------------------------
//	¥ Refresh
// ---------------------------------------------------------------------------
//	Invalidate the area occupied by a View.
//
//	This forces an Update event that, when processed, will redraw the View.
//	Refresh does nothing if the View is not visible.

void
LView::Refresh()
{
	if (mVisible == triState_On) {
		InvalPortRect(&mRevealedRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DontRefresh
// ---------------------------------------------------------------------------
//	Validate the area occupied by a View.
//
//	This removes the View area from the update region so that the View
//	won't be redrawn during the next Update event.
//
//	The inOKIfHidden parameter specifies whether to validate even if
//	the View is not visible. You will need to pass true for this
//	parameter in order to suppress the automatic refresh which occurs
//	after hiding a View.

void
LView::DontRefresh(
	Boolean	inOKIfHidden)
{
	if ((mVisible == triState_On) || inOKIfHidden) {
		ValidPortRect(&mRevealedRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------
//	Prepare for drawing in the View by setting the Port and clipping area
//
//	The inSubPane parameter specifies which SubPane of this View is
//	requesting the focus. SubPanes that aren't descended from LView can't
//	focus themselves and must ask their SuperView to focus. If the parameter
//	is nil, then this View is requesting focus for itself.
//
//	Returns whether any portion of the View is revealed, which means
//	it returns true if the clip region is not empty and false if it is.
//
//	If FocusDraw returns true, it also means that the View's Frame is
//	in QuickDraw space. The Revealed area would be empty if the Frame
//	did not intersect the Port rectangle (which is always in QuickDraw
//	space) of the Mac Port containing the View.

Boolean
LView::FocusDraw(
	LPane*	/* inSubPane */)
{
									// Check if revealed rect is empty
	Boolean	revealed = (mRevealedRect.left < mRevealedRect.right);

		// Debugging code. Define PP_Debug_VerifyFocusCache as 1
		// in your prefix file to enable focus verification. This
		// helps diagnose problems where the focus is not being
		// invalidated after changing the port state.

#if PP_Debug_VerifyFocusCache
	if (this == sInFocusView)  {

		GrafPtr		beforePort;		// Verify Port setting
		::GetPort(&beforePort);

		EstablishPort();

		GrafPtr		afterPort;
		::GetPort(&afterPort);

		SignalIf_( beforePort != afterPort );

									// Verify Port origin
		Rect	afterBounds;
		::GetPortBounds(afterPort, &afterBounds);
		SignalIf_( mPortOrigin.h != afterBounds.left );
		SignalIf_( mPortOrigin.v != afterBounds.top );

									// Verify Clipping region
		Rect	clipR = mRevealedRect;
		PortToLocalPoint(topLeft(clipR));
		PortToLocalPoint(botRight(clipR));
		StRegion	testClip = clipR;

			// Current clipping region can be different from
			// the one set by focusing, but if it is it must
			// not contain any area that's not in the focus one.
			// Therefore, the intersection of the focus region
			// with the current clip region must equal the
			// current clip region.

		StRegion	afterClip;
		::GetClip(afterClip);
		testClip &= afterClip;

		SignalIf_ ( !::MacEqualRgn(testClip, afterClip) );
	}
#endif

	if (this != sInFocusView) {		// Skip if already in focus
		if (EstablishPort()) {		// Set current Mac Port

										// Set up local coordinate system
			::SetOrigin(mPortOrigin.h, mPortOrigin.v);

										// Clip to revealed area of View
			Rect	clippingRect = mRevealedRect;
			PortToLocalPoint(topLeft(clippingRect));
			PortToLocalPoint(botRight(clippingRect));
			::ClipRect(&clippingRect);

			sInFocusView = this;		// Cache current Focus

		} else {
			SignalStringLiteral_("Focus View with no GrafPort");
			revealed = false;
		}
	}

	return revealed;
}


// ---------------------------------------------------------------------------
//	¥ FocusExposed
// ---------------------------------------------------------------------------
//	Prepare for drawing in the View by setting the Port and clipping area
//
//	Returns whether a View is exposed, meaning that it is visible and
//	that it is revealed through the Frames of all its SuperViews.
//
//	inAlwaysFocus specifies whether to focus a View even if it is not visible

Boolean
LView::FocusExposed(
	Boolean	inAlwaysFocus)
{
	Boolean	exposed = (mVisible == triState_On);

	if (exposed || inAlwaysFocus) {
		exposed &= FocusDraw();
	}

	return exposed;
}


// ---------------------------------------------------------------------------
//	¥ OutOfFocus [static]
// ---------------------------------------------------------------------------
//	Notify View system that a View is no longer in focus
//
//	Call when the coordinate system or clipping region of a View changes,
//	passing a pointer to that View. This clears the saved focus if that
//	View was the one in focus.
//
//	Use nil for inView if you manually change (and don't restore) the
//	current port or clipping region

void
LView::OutOfFocus(
	LView	*inView)
{
	if ( (inView == nil) || (sInFocusView == inView) ) {
		sInFocusView = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ Draw
// ---------------------------------------------------------------------------
//	Draw a View and all its SubPanes
//
//	inSuperDrawRgnH specifies, in Port coordinates, the portion of the
//	View's SuperView that needs to be drawn. Specify nil to draw the
//	entire View.

void
LView::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	try {
		if (FocusExposed()) {		// Don't draw if unable to focus

									// Determine area of this View to draw

									// Start with revealed area
			mUpdateRgn = mRevealedRect;

									// Intersect with super draw area
			if (inSuperDrawRgnH != nil) {
				mUpdateRgn &= inSuperDrawRgnH;
			}

			{						// Intersect with Port's visible region
	 			GrafPtr	currentPort;
				::GetPort(&currentPort);

				StRegion	visibleRgn;
				::GetPortVisibleRegion(currentPort, visibleRgn);
									// Convert to Port coordinates
				::MacOffsetRgn(visibleRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);

				mUpdateRgn &= visibleRgn;
			}

			if (not mUpdateRgn.IsEmpty()) {
									// Some portion needs to be drawn

									// Clip to the portion to redraw
									//   Must convert to Local coords and
									//   then back to Port coords
				::MacOffsetRgn(mUpdateRgn, mPortOrigin.h, mPortOrigin.v);

									// Get bounds of update region in local
									//   coords so we can use it in the
									//   device loop
				Rect	localUpdateFrame;
				mUpdateRgn.GetBounds(localUpdateFrame);

				::SetClip(mUpdateRgn);

				::MacOffsetRgn(mUpdateRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);

				Rect	frame;
				CalcLocalFrameRect(frame);

				if ( (sBitDepth > 0)  ||
				     UDrawingUtils::IsPortBuffered(GetMacPort()) ) {

					if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
										// A View is visually behind its
										//   SubPanes so it draws itself first,
										//   then its SubPanes
						DrawSelf();
					}
										// Draw SubPanes
					TArrayIterator<LPane*> iterator(mSubPanes);
					LPane	*theSub;
					while (iterator.Next(theSub)) {
						theSub->Draw(mUpdateRgn);
					}

				} else {

					StValueChanger<SInt16>	saveDepth(sBitDepth);
					StColorDrawLoop	deviceLoop(localUpdateFrame);

					while (deviceLoop.NextDepth(sBitDepth)) {
						sDeviceHasColor = UDrawingUtils::DeviceSupportsColor(
											deviceLoop.GetCurrentDevice());

							// deviceLoop sets the clipping region to account for
							// multiple monitors. Set the View's update region to the
							// clipping region expressed in Port coords.

						::GetClip(mUpdateRgn);
						::MacOffsetRgn(mUpdateRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);

						if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
										// A View is visually behind its
										//   SubPanes so it draws itself first,
										//   then its SubPanes
							DrawSelf();
						}
										// Draw SubPanes
						TArrayIterator<LPane*> iterator(mSubPanes);
						LPane	*theSub;
						while (iterator.Next(theSub)) {
							theSub->Draw(mUpdateRgn);
						}

						FocusDraw();	// SubPanes can change focus, so we
										//   must refocus this View before
										//   drawing at the next bit depth
					}
				}

				mUpdateRgn.Clear();	// Emptying update region frees up memory
			}
		}
	}

	catch (...) {
		SignalStringLiteral_("Exception caught in LView::Draw()");
	}

	OutOfFocus(nil);					// Device loop changes clip region
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CountPanels
// ---------------------------------------------------------------------------
//	Return the number of horizontal and vertical Panels. A Panel is a
//	"frameful" of a View's Image.

void
LView::CountPanels(
	UInt32	&outHorizPanels,
	UInt32	&outVertPanels)
{
	SDimension32	imageSize;
	GetImageSize(imageSize);

	SDimension16	frameSize;
	GetFrameSize(frameSize);

	outHorizPanels = 1;
	if (frameSize.width > 0  &&  imageSize.width > 0) {
		outHorizPanels = (UInt32) ((imageSize.width - 1) / frameSize.width) + 1;
	}

	outVertPanels = 1;
	if (frameSize.height > 0  &&  imageSize.height > 0) {
		outVertPanels = (UInt32) ((imageSize.height - 1) / frameSize.height) + 1;
	}
}


// ---------------------------------------------------------------------------
//	¥ PrintPanel
// ---------------------------------------------------------------------------
//	Try to Print a Panel of a View
//
//	The View is at the top level of the Printout, meaning that it controls
//	pagination. This functions scrolls the View to the specified panel.

void
LView::PrintPanel(
	const PanelSpec	&inPanel,
	RgnHandle		inSuperPrintRgnH)
{
									// Don't print if invisible
	if (IsVisible()) {
									// Area of this View to print is the
									//   intersection of inSuperPrintRgnH
									//   with the Revealed area of this View
		mUpdateRgn = mRevealedRect;
		if (inSuperPrintRgnH != nil) {
			mUpdateRgn &= inSuperPrintRgnH;
		}

		try {
			if ( !mUpdateRgn.IsEmpty()  &&
				 ScrollToPanel(inPanel) &&
				 FocusDraw()) {

									// Clip to the portion to redraw
									//   Must convert to Local coords and
									//   then back to Port coords
				::MacOffsetRgn(mUpdateRgn, mPortOrigin.h, mPortOrigin.v);
				::SetClip(mUpdateRgn);
				::MacOffsetRgn(mUpdateRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);

					Rect	frame;
					CalcLocalFrameRect(frame);
					if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
									// Some portion needs to be printed
						PrintPanelSelf(inPanel);
					}

									// Let SubPanes print within this
									//   Panel of its SuperView
				TArrayIterator<LPane*> iterator(mSubPanes);
				LPane	*theSub;
				while (iterator.Next(theSub)) {
					theSub->SuperPrintPanel(inPanel, mUpdateRgn);
				}
			}
		}

		catch (...) {
			SignalStringLiteral_("Exception caught in LView::PrintPanel");
		}

		mUpdateRgn.Clear();			// Emptying update region frees up memory
									//   if this region wasn't rectangular
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperPrintPanel
// ---------------------------------------------------------------------------
//	SuperView is printing a panel
//
//	The View is not in control of pagination. In general, it is not clear
//	how to print nested scrolling views. This function just prints the
//	View at its current location, without scrolling to a particular panel.

void
LView::SuperPrintPanel(
	const PanelSpec	&inSuperPanel,
	RgnHandle		inSuperPrintRgnH)
{
									// Don't print if invisible
	if (IsVisible()) {
		try {
									// Area of this View to print is the
									//   intersection of inSuperPrintRgnH
									//   with the Revealed area of this View
			mUpdateRgn = mRevealedRect;
			if (inSuperPrintRgnH != nil) {
				mUpdateRgn &= inSuperPrintRgnH;
			}

			if ( !mUpdateRgn.IsEmpty() && FocusDraw() ) {
									// Some portion needs to be printed

									// Clip to the portion to redraw
									//   Must convert to Local coords and
									//   then back to Port coords
				::MacOffsetRgn(mUpdateRgn, mPortOrigin.h, mPortOrigin.v);
				::SetClip(mUpdateRgn);
				::MacOffsetRgn(mUpdateRgn, (SInt16) -mPortOrigin.h, (SInt16) -mPortOrigin.v);

				Rect	frame;		//   Print Panel of this View
				CalcLocalFrameRect(frame);
				if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
					PrintPanelSelf(inSuperPanel);
				}
									//   Print Panels of SubPanes
				TArrayIterator<LPane*> iterator(mSubPanes);
				LPane	*theSub;
				while (iterator.Next(theSub)) {
					theSub->SuperPrintPanel(inSuperPanel, mUpdateRgn);
				}
			}
		}

		catch (...) {
			SignalStringLiteral_("Exception caught in LView::SuperPrintPanel");
		}

		mUpdateRgn.Clear();			// Emptying update region frees up memory
									//   if this region wasn't rectangular
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollToPanel
// ---------------------------------------------------------------------------
//	Scroll View Image to the specified Panel
//
//	Return whether the specified Panel exists. If it doesn't, View is
//	not scrolled.

Boolean
LView::ScrollToPanel(
	const PanelSpec	&inPanel)
{
	Boolean	panelInImage = false;

	SDimension16	frameSize;
	GetFrameSize(frameSize);

	UInt32	horizPanelCount;
	UInt32	vertPanelCount;
	CountPanels(horizPanelCount, vertPanelCount);

	if ((inPanel.horizIndex <= horizPanelCount) &&
		(inPanel.vertIndex <= vertPanelCount)) {
		SInt32 horizPos = (SInt32) (frameSize.width * (inPanel.horizIndex - 1));
		SInt32 vertPos = (SInt32) (frameSize.height * (inPanel.vertIndex - 1));
		ScrollImageTo(horizPos, vertPos, false);
		panelInImage = true;
	}

	return panelInImage;
}


// ---------------------------------------------------------------------------
//	¥ FindPageBreak
// ---------------------------------------------------------------------------
//	Adjust the input height and width to the closest "natural" page break
//
//		inTopLeft - Top left corner of the page in Image coords
//
//		ioHeight - On entry, a proposed page height in pixels
//				   On exit, the natural page height based on the View contents
//
//		ioWidth - On entry, a proposed page width in pixels
//				  On exit, the natural page width based on the View contents
//
//	For both ioHeight and ioWidth, you should find the closest natural
//	page break that is less than the input value. If there is no natural
//	page break less than the input value, pass back the smallest page break.
//
//	An input value <= 0 means not to calculate that value.
//
//	Subclasses whose content has natural page breaks should override this
//	function. For example, a view that displays lines of text would set
//	the height at the space between two lines of text rather than have the
//	page break chop off the bottom of the characters.

void
LView::FindPageBreak(
	const SPoint32&		/* inTopLeft */,
	SInt16&				/* ioHeight */,
	SInt16&				/* ioWidth */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ScrollImageTo
// ---------------------------------------------------------------------------
//	Scroll Image to the specified horizontal and vertical locations
//
//	Scrolling moves the Image relative to the Frame and Port, so that a
//	different portion of the Image is visible thru the Frame.
//
//	When scrolled to (0, 0), the top left of the Image coincides with
//	the top left of the Frame (home position).
//
//	If inRefresh is true, the Port containing the View is updated
//	immediately, rather than refreshed at the next update event.
//	Scrolling usually happens during mouse down tracking, so we want
//	immediate visual feedback.

void
LView::ScrollImageTo(
	SInt32		inLeftLocation,
	SInt32		inTopLocation,
	Boolean		inRefresh)
{
	ScrollImageBy(mImageLocation.h - mFrameLocation.h + inLeftLocation,
				  mImageLocation.v - mFrameLocation.v + inTopLocation,
				  inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ ScrollImageBy
// ---------------------------------------------------------------------------
//	Scroll Image by specified horizontal and vertical increments
//
//	Scrolling moves the Image relative to the Frame and Port, so that a
//	different portion of the Image is visible thru the Frame.
//
//	Positive deltas scroll right and down.
//	Negative deltas scroll left and up.
//
//	If inRefresh is true, the Port containing the View is updated
//	immediately, rather than refreshed at the next update event.
//	Scrolling usually happens during mouse down tracking, so we want
//	immediate visual feedback.

void
LView::ScrollImageBy(
	SInt32		inLeftDelta,			// Pixels to scroll horizontally
	SInt32		inTopDelta,				// Pixels to scroll vertically
	Boolean		inRefresh)
{
	if ((inLeftDelta == 0)  &&  (inTopDelta == 0)) {
		return;							// Do nothing
	}

	if (inRefresh && IsVisible()) {

			// Check if any portion of what is visible now will be
			// visible after the scroll. If so, it should be faster
			// to move the bits rather than redrawing them.

		SInt32	absLeftDelta = inLeftDelta;
		if (absLeftDelta < 0) {
			absLeftDelta = -absLeftDelta;
		}
		SInt32	absTopDelta = inTopDelta;
		if (absTopDelta < 0) {
			absTopDelta = -absTopDelta;
		}

		if ( (absLeftDelta < (mRevealedRect.right - mRevealedRect.left)) &&
			 (absTopDelta < (mRevealedRect.bottom - mRevealedRect.top)) ) {
			UpdatePort();
			ScrollBits(inLeftDelta, inTopDelta);
		} else {
			Refresh();
		}
	}

	mImageLocation.h -= inLeftDelta;	// Move Image relative to the Port
	mImageLocation.v -= inTopDelta;
	CalcPortOrigin();
	OutOfFocus(this);

	TArrayIterator<LPane*> iterator(mSubPanes);
	LPane	*theSub;
	while (iterator.Next(theSub)) {
		theSub->AdaptToSuperScroll(inLeftDelta, inTopDelta);
	}

	if (mSuperView != nil) {
		mSuperView->SubImageChanged(this);
	}

	if (inRefresh && IsVisible()) {
		UpdatePort();
	}
}


// ---------------------------------------------------------------------------
//	¥ ScrollPinnedImageTo
// ---------------------------------------------------------------------------
//	Scroll Image to the specified horizontal and vertical locations but
//	not beyond the edge of the Frame.
//
//	Return true if the View actually scrolls

Boolean
LView::ScrollPinnedImageTo(
	SInt32		inLeftLocation,
	SInt32		inTopLocation,
	Boolean		inRefresh)
{
	return ScrollPinnedImageBy(
				mImageLocation.h - mFrameLocation.h + inLeftLocation,
				mImageLocation.v - mFrameLocation.v + inTopLocation,
				inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ ScrollPinnedImageBy
// ---------------------------------------------------------------------------
//	Scroll Image by specified horizontal and vertical increments, but
//	don't scroll beyond an edge of the Frame
//
//	Return true if the View actually scrolls

Boolean
LView::ScrollPinnedImageBy(
	SInt32		inLeftDelta,			// Pixels to scroll horizontally
	SInt32		inTopDelta,				// Pixels to scroll vertically
	Boolean		inRefresh)
{
	if (inLeftDelta != 0) {				// Pin horizontal
		inLeftDelta = CalcPinnedScrollDelta(
								inLeftDelta,
								mFrameLocation.h,
								mFrameLocation.h + mFrameSize.width,
								mImageLocation.h,
								mImageLocation.h + mImageSize.width);
	}

	if (inTopDelta != 0) {				// Pin vertical
		inTopDelta = CalcPinnedScrollDelta(
								inTopDelta,
								mFrameLocation.v,
								mFrameLocation.v + mFrameSize.height,
								mImageLocation.v,
								mImageLocation.v + mImageSize.height);
	}

										// No scroll if both deltas are 0
	Boolean	scrolled = (inLeftDelta != 0) || (inTopDelta != 0);

	if (scrolled) {
		ScrollImageBy(inLeftDelta, inTopDelta, inRefresh);
	}

	return scrolled;
}


// ---------------------------------------------------------------------------
//	¥ CalcPinnedScrollDelta
// ---------------------------------------------------------------------------

SInt32
LView::CalcPinnedScrollDelta(
	SInt32		inDelta,
	SInt32		inFrameMin,
	SInt32		inFrameMax,
	SInt32		inImageMin,
	SInt32		inImageMax) const
{
	SInt32	pinnedDelta = inDelta;		// In case there's no pinning

	SInt32	tryMin = inImageMin - inDelta;	// Potential new min/max for
	SInt32	tryMax = inImageMax - inDelta;	//   Image after scroll

		// Check for pin conditions. We may need to pin if
		//
		//		(tryMin > FrameMin)  or  (tryMax < FrameMax)
		//
		// because we always want the Frame to be entirely within
		// the Image.
		//
		// However, the Image can be smaller than the Frame. In that
		// case, we prefer to align the min edges rather than the
		// max edges.

	if ( ((inDelta < 0) && (inImageMin >= inFrameMin))  ||
		 ((inDelta > 0) && (inImageMin <= inFrameMin) &&
		 	(inImageMax <= inFrameMax)) ) {

		pinnedDelta = 0;						// Pin to current location

	} else
	if ( ((inDelta < 0) && (tryMin > inFrameMin))  ||
		 ((inDelta > 0) && (tryMin < inFrameMin) && (tryMax < inFrameMax) &&
		 	((inImageMax - inImageMin) < (inFrameMax - inFrameMin))) ) {

		pinnedDelta = inImageMin - inFrameMin;	// Pin to align min edges

	} else
	if ( ((inDelta > 0) && (tryMin < inFrameMin) && (tryMax < inFrameMax)) ) {

		pinnedDelta = inImageMax - inFrameMax;	// Pin to align max edges
	}

	return pinnedDelta;
}

// ---------------------------------------------------------------------------
//	¥ ScrollBits
// ---------------------------------------------------------------------------
//	Scroll the pixels of a View
//
//	Called internally by ScrollImageBy to shift the pixels

void
LView::ScrollBits(
	SInt32		inLeftDelta,			// Pixels to scroll horizontally
	SInt32		inTopDelta)				// Pixels to scroll vertically
{
	if (FocusExposed()) {
										// Get Frame in Port coords
		Rect	frame = mRevealedRect;
		PortToLocalPoint(topLeft(frame));
		PortToLocalPoint(botRight(frame));

										// Scroll Frame, clipping to the
										//   update region
		StRegion	updateRgn;
		::ScrollRect(&frame, (SInt16) -inLeftDelta, (SInt16) -inTopDelta,
								updateRgn);

										// Force redraw of update region
										// Use our InvalPortRgn rather than
										// Toolbox InvalRgn
		::MacOffsetRgn(updateRgn, (SInt16) -mPortOrigin.h,
								  (SInt16) -mPortOrigin.v);
		InvalPortRgn(updateRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ AutoScrollImage
// ---------------------------------------------------------------------------
//	Scroll the Image if the specified point is outside the View's Frame.
//
//	Returns true if the View actually scrolled
//
//	Call this function while tracking the mouse to scroll a View in the
//	direction of the mouse location.

Boolean
LView::AutoScrollImage(
	Point	inLocalPt)
{
	Boolean	scrolled = false;
	Rect	frame;
	CalcLocalFrameRect(frame);

	SInt32	horizScroll = 0;
	if (inLocalPt.h < frame.left) {				// AutoScroll left
		horizScroll = -mScrollUnit.h;
	} else if (inLocalPt.h > frame.right) {		// AutoScroll right
		horizScroll = mScrollUnit.h;
	}

	SInt32	vertScroll = 0;
	if (inLocalPt.v < frame.top) {				// AutoScroll up
		vertScroll = -mScrollUnit.v;
	} else if (inLocalPt.v > frame.bottom) {	// AutoScroll down
		vertScroll = mScrollUnit.v;
	}

	if ((horizScroll != 0) || (vertScroll != 0)) {
		scrolled = ScrollPinnedImageBy(horizScroll, vertScroll, true);
	}

	return scrolled;
}


// ---------------------------------------------------------------------------
//	¥ SetScrollUnit
// ---------------------------------------------------------------------------

void
LView::SetScrollUnit(
	const SPoint32	&inScrollUnit)
{
	mScrollUnit = inScrollUnit;

	if (mScrollUnit.h < 1) {		// ScrollUnit must be a positive number
		mScrollUnit.h = 1;
	}

	if (mScrollUnit.v < 1) {
		mScrollUnit.v = 1;
	}

	if (mSuperView != nil) {
		mSuperView->SubImageChanged(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetScrollUnit
// ---------------------------------------------------------------------------

void
LView::GetScrollUnit(
	SPoint32	&outScrollUnit) const
{
	outScrollUnit = mScrollUnit;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeImageTo
// ---------------------------------------------------------------------------
//	Set the Image size to the specified pixel dimensions

void
LView::ResizeImageTo(
	SInt32		inWidth,
	SInt32		inHeight,
	Boolean		inRefresh)
{
	ResizeImageBy(inWidth - mImageSize.width,
				  inHeight - mImageSize.height,
				  inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ ResizeImageBy
// ---------------------------------------------------------------------------
//	Change the Image size by the specified pixel increments

void
LView::ResizeImageBy(
	SInt32		inWidthDelta,
	SInt32		inHeightDelta,
	Boolean		inRefresh)
{
	mImageSize.width += inWidthDelta;
	mImageSize.height += inHeightDelta;

	ReconcileFrameAndImage(inRefresh);

	if (mSuperView != nil) {
		mSuperView->SubImageChanged(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ReconcileFrameAndImage
// ---------------------------------------------------------------------------
//	Adjusts the Image so that it fits within the Frame
//
//	This function addresses the problem of what to do when you scroll a
//	View to at or near the bottom or right, then make the View's Frame
//	larger. This would normally expose some "undefined" area below or
//	to the right of the Image.
//
//	If mReconcileOverhang is true, this function scrolls the Image so that
//	the bottom right corner is at the bottom right of the Frame. However,
//	it never moves the top left corner of the Image beyond the top left
//	of the Frame. Therefore, the only time "undefined" area is exposed is
//	when the Frame is larger than the Image.
//
//	For Views with fixed Image sizes, such as drawings where the Image size
//	is the size of a printed page, set mReconcileOverhang to true. The user
//	does not normally want to see past the bottom or right of such Views.
//
//	For Views with variable Image sizes, such as text blocks where the size
//	of the Image depends on the number of lines of text, set
//	mReconcileOverhang to false. The user may want to see the undefined
//	area in anticipation of the Image growing.

void
LView::ReconcileFrameAndImage(
	Boolean	inRefresh)
{
	if (mReconcileOverhang) {
		SPoint32	currScrollPos;
		GetScrollPosition(currScrollPos);
		SPoint32	newScrollPos = currScrollPos;

									// Reconcile Vertical position
		if ( (mFrameLocation.v + mFrameSize.height) >
			 (mImageLocation.v + mImageSize.height) ) {
									// Frame extends below Image
			 newScrollPos.v = mImageSize.height - mFrameSize.height;
			 if (newScrollPos.v < 0) {
			 	newScrollPos.v = 0;
			 }
		}

									// Reconcile horizontal position
		if ( (mFrameLocation.h + mFrameSize.width) >
			 (mImageLocation.h + mImageSize.width) ) {
									// Frame extends right Image
			 newScrollPos.h = mImageSize.width - mFrameSize.width;
			 if (newScrollPos.h < 0) {
			 	newScrollPos.h = 0;
			 }
		}

		if ( (newScrollPos.v != currScrollPos.v) ||
			 (newScrollPos.h != currScrollPos.h) ) {
			 						// Scroll to keep bottom right in
			 						//   an appropriate position
			ScrollImageTo(newScrollPos.h, newScrollPos.v, Refresh_No);
			if (inRefresh) {
				Refresh();
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SubImageChanged
// ---------------------------------------------------------------------------
//	Notification that the Image of some SubView changed size, location,
//	or scroll units.
//
//	Override to respond to such changes.

void
LView::SubImageChanged(
	LView*	/* inSubView */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FindSubPaneHitBy
// ---------------------------------------------------------------------------
//	Find the SubPane of this View that is hit by the specified point.
//	Return nil if no SubPane is hit
//
//	inHorizPort and inVertPort are in Port coordinates

LPane*
LView::FindSubPaneHitBy(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
		// Test SubPanes from last to first. Since SubPanes draw from
		// first to last, the visually topmost is last.

	LPane	*hitSubPane = nil;
	TArrayIterator<LPane*> iterator(mSubPanes, LArrayIterator::from_End);
	LPane	*theSub;
	while (iterator.Previous(theSub)) {
		if (theSub->IsHitBy(inHorizPort, inVertPort)) {
			hitSubPane = theSub;
			break;
		}
	}

	return hitSubPane;
}


// ---------------------------------------------------------------------------
//	¥ FindDeepSubPaneContaining
// ---------------------------------------------------------------------------
//	Return the most deeply nested visible SubPane which contains the specified
//	point, which is in Port coordinates
//
//	Return nil if no SubPane contains the point

LPane*
LView::FindDeepSubPaneContaining(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
	LPane	*hitSubPane = nil;
	TArrayIterator<LPane*> iterator(mSubPanes, LArrayIterator::from_End);
	LPane	*theSub;

	while (iterator.Previous(theSub)) {
		if (theSub->Contains(inHorizPort, inVertPort) && theSub->IsVisible()) {
			hitSubPane =
				theSub->FindDeepSubPaneContaining(inHorizPort, inVertPort);
			if (hitSubPane == nil) {
				hitSubPane = theSub;
			}
			break;
		}
	}

	return hitSubPane;
}


// ---------------------------------------------------------------------------
//	¥ FindShallowSubPaneContaining
// ---------------------------------------------------------------------------
//	Return the immediate visible SubPane which contains the specified point,
//	which is in Port coordinates
//
//	Return nil if no SubPane contains the point
//
//	Same as FindSubPaneHitBy except that it tests using Contains()
//	rather than IsHitBy(). Contains() does not check if the Pane is enabled.

LPane*
LView::FindShallowSubPaneContaining(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
	LPane	*hitSubPane = nil;
	TArrayIterator<LPane*> iterator(mSubPanes, LArrayIterator::from_End);
	LPane	*theSub;

	while (iterator.Previous(theSub)) {
		if (theSub->Contains(inHorizPort, inVertPort) && theSub->IsVisible()) {
			hitSubPane = theSub;
			break;
		}
	}

	return hitSubPane;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Click
// ---------------------------------------------------------------------------
//	Handle a click inside a View

void
LView::Click(
	SMouseDownEvent	&inMouseDown)
{
									// Check if a SubPane of this View
									//   is hit
	LPane	*clickedPane = FindSubPaneHitBy(inMouseDown.wherePort.h,
											inMouseDown.wherePort.v);

	if (clickedPane != nil) {		// SubPane is hit, let it respond to
									//   the Click
		clickedPane->Click(inMouseDown);

	} else {						// No SubPane hit. Inherited function
		LPane::Click(inMouseDown);	//   will process click on this View
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouse
// ---------------------------------------------------------------------------

void
LView::AdjustMouse(
	Point				inPortPt,
	const EventRecord&	inMacEvent,
	RgnHandle			outMouseRgn)	// In Port coords
{
									// Find subpane containing the mouse
	LPane	*hitPane = FindSubPaneHitBy(inPortPt.h, inPortPt.v);
	
	if (hitPane != nil) {			// Let subpane adjust the mouse
		hitPane->AdjustMouse(inPortPt, inMacEvent, outMouseRgn);
		
	} else {						// This View adjusts the mouse
		LPane::AdjustMouse(inPortPt, inMacEvent, outMouseRgn);
		
			// Exclude from the mouse region the area covered by this
			// View's enabled subpanes
			
		StRegion	subRgn;
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane*	theSub;
		while (iterator.Next(theSub)) {
		
			if (theSub->IsEnabled()) {
				Rect	subFrame;
				theSub->CalcPortFrameRect(subFrame);
				subRgn = subFrame;
				
				::DiffRgn(outMouseRgn, subRgn, outMouseRgn);
			}
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FindPaneByID
// ---------------------------------------------------------------------------
//	Find the Pane of a View which has the specified ID
//
//		Searches all Panes contained within this View, not just direct
//		subpanes. Returns nil if Pane with the target ID is not found.

LPane*
LView::FindPaneByID(
	PaneIDT	inPaneID)
{
		// We call the const function to get the Pane pointer, but cast
		// away the constness. This is safe since this function is not
		// const and it's whole purpose is to return a pointer to a Pane
		// that can be changed.

	return const_cast<LPane*>(FindConstPaneByID(inPaneID));
}


// ---------------------------------------------------------------------------
//	¥ FindConstPaneByID
// ---------------------------------------------------------------------------
//	Find the Pane of a View which has the specified ID
//
//		Searches all Panes contained within this View, not just direct
//		subpanes. Returns nil if Pane with the target ID is not found.
//
//	Use when you only want to inspect (and not change) the Pane.

const LPane*
LView::FindConstPaneByID(
	PaneIDT	inPaneID) const
{
	const LPane	*thePane = nil;

	if (inPaneID == mPaneID) {			// Check first if this is the one
		thePane = this;
	} else {
										// Search all subpanes
		TArrayIterator<LPane*> iterator(mSubPanes);
		LPane	*theSub;
		while (iterator.Next(theSub)) {
			thePane = theSub->FindConstPaneByID(inPaneID);
			if (thePane != nil) break;
		}
	}

	return thePane;
}


// ---------------------------------------------------------------------------
//	¥ GetValueForPaneID
// ---------------------------------------------------------------------------
//	Get the Value of the Pane of a View which has the specified ID
//
//	The Pane may be the View itself, or a SubPane of the View

SInt32
LView::GetValueForPaneID(
	PaneIDT	inPaneID) const
{
	SInt32	value = 0;
	const LPane	*thePane = FindConstPaneByID(inPaneID);
	if (thePane != nil) {
		value = thePane->GetValue();
	} else {
		SignalStringLiteral_("GetValue For Unknown Pane ID");
	}
	return value;
}


// ---------------------------------------------------------------------------
//	¥ SetValueForPaneID
// ---------------------------------------------------------------------------
//	Set the Value of the Pane of a View which has the specified ID
//
//	The Pane may be the View itself, or a SubPane of the View

void
LView::SetValueForPaneID(
	PaneIDT	inPaneID,
	SInt32	inValue)
{
	LPane	*thePane = FindPaneByID(inPaneID);
	if (thePane != nil) {
		thePane->SetValue(inValue);
	} else {
		SignalStringLiteral_("SetValue for Unknown Pane ID");
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptorForPaneID
// ---------------------------------------------------------------------------
//	Get the Descriptor of the Pane of a View which has the specified ID
//
//	The Pane may be the View itself, or a SubPane of the View

StringPtr
LView::GetDescriptorForPaneID(
	PaneIDT	inPaneID,
	Str255	outDescriptor) const
{
	const LPane	*thePane = FindConstPaneByID(inPaneID);
	if (thePane != nil) {
		thePane->GetDescriptor(outDescriptor);
	} else {
		SignalStringLiteral_("GetDescriptor for Unknown Pane ID");
	}
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptorForPaneID
// ---------------------------------------------------------------------------
//	Set the Descriptor of the Pane of a View which has the specified ID
//
//	The Pane may be the View itself, or a SubPane of the View

void
LView::SetDescriptorForPaneID(
	PaneIDT				inPaneID,
	ConstStringPtr		inDescriptor)
{
	LPane	*thePane = FindPaneByID(inPaneID);
	if (thePane != nil) {
		thePane->SetDescriptor(inDescriptor);
	} else {
		SignalStringLiteral_("SetDescriptor for Unknown Pane ID");
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PortToLocalPoint
// ---------------------------------------------------------------------------
//	Convert point from Port to Local coordinates

void
LView::PortToLocalPoint(
	Point	&ioPoint) const
{
	ioPoint.h += mPortOrigin.h;
	ioPoint.v += mPortOrigin.v;
}


// ---------------------------------------------------------------------------
//	¥ LocalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Local to Port coordinates

void
LView::LocalToPortPoint(
	Point	&ioPoint) const
{
	ioPoint.h -= mPortOrigin.h;
	ioPoint.v -= mPortOrigin.v;
}


// ---------------------------------------------------------------------------
//	¥ ImageToLocalPoint
// ---------------------------------------------------------------------------
//	Convert point from Image (32-bit) to Local (16-bit) coordinates
//
//	Image and Local coordinates are different only when the Image size
//	is greater than 16K (15-bit)

void
LView::ImageToLocalPoint(
	const SPoint32	&inImagePt,
	Point			&outLocalPt) const
{
	outLocalPt.h = (SInt16) (inImagePt.h + mPortOrigin.h + mImageLocation.h);
	outLocalPt.v = (SInt16) (inImagePt.v + mPortOrigin.v + mImageLocation.v);
}


// ---------------------------------------------------------------------------
//	¥ LocalToImagePoint
// ---------------------------------------------------------------------------
//	Convert point from Local (16-bit) to Image (32-bit) coordinates
//
//	Image and Local coordinates are different only when the Image size
//	is greater than 16K (15-bit)

void
LView::LocalToImagePoint(
	const Point		&inLocalPt,
	SPoint32		&outImagePt) const
{
	outImagePt.h = (SInt32) inLocalPt.h - mPortOrigin.h - mImageLocation.h;
	outImagePt.v = (SInt32) inLocalPt.v - mPortOrigin.v - mImageLocation.v;
}


// ---------------------------------------------------------------------------
//	¥ ImagePointIsInFrame
// ---------------------------------------------------------------------------
//	Return whether a Point specified in Image Coords is within the
//	Frame of a View

Boolean
LView::ImagePointIsInFrame(
	SInt32	inHorizImage,
	SInt32	inVertImage) const
{
	return PointIsInFrame(inHorizImage + mImageLocation.h,
							inVertImage + mImageLocation.v);
}


// ---------------------------------------------------------------------------
//	¥ ImageRectIntersectsFrame
// ---------------------------------------------------------------------------
//	Return whether a Rectangle specified in Image Coords intersects
//	the Frame of a View

Boolean
LView::ImageRectIntersectsFrame(
	SInt32	inLeftImage,
	SInt32	inTopImage,
	SInt32	inRightImage,
	SInt32	inBottomImage) const
{
			// Covert input to Port Coords

	SInt32	inLeft = inLeftImage + mImageLocation.h;
	SInt32	inRight = inRightImage + mImageLocation.h;
	SInt32	inTop = inTopImage + mImageLocation.v;
	SInt32	inBottom = inBottomImage + mImageLocation.v;

			// Get individual coordinates of Frame in Port Coords

	SInt32	frLeft = mFrameLocation.h;
	SInt32	frRight = frLeft + mFrameSize.width;
	SInt32	frTop  = mFrameLocation.v;
	SInt32	frBottom = frTop + mFrameSize.height;

			// Determine if rectangles intersect by comparing
			// each edge of the input rectangle to the opposing
			// edge of the frame rectangle

	return ( (inLeft < frRight)  &&
			 (inTop < frBottom)  &&
			 (inRight > frLeft)  &&
			 (inBottom > frTop) );
}

PP_End_Namespace_PowerPlant
