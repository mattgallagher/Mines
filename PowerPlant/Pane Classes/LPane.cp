// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPane.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for an object that can draw itself and respond to mouse clicks

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPane.h>
#include <LView.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UCursor.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UMemoryMgr.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

LView*	LPane::sDefaultView			= nil;	// Default SuperView for new Panes
LPane*	LPane::sLastPaneClicked		= nil;	// Pane hit by last mouse click
LPane*	LPane::sLastPaneMoused		= nil;
UInt32	LPane::sWhenLastMouseUp		= 0;

UInt32			LPane::sWhenLastMouseDown	= 0;	// Info from last
Point			LPane::sWhereLastMouseDown;			//   Mouse Down Event
EventModifiers	LPane::sModifiersLastMouseDown = 0;

SInt16	LPane::sClickCount			= 0;

SInt16	LPane::sBitDepth			= 0;
bool	LPane::sDeviceHasColor		= false;


// ---------------------------------------------------------------------------
//	Constants

const SInt16		multiClick_Distance = 4;


// ---------------------------------------------------------------------------
//	¥ LPane									Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPane::LPane()
{
	mPaneID = PaneIDT_Unspecified;
	mFrameSize.width = mFrameSize.height = 0;
	mFrameLocation.h = mFrameLocation.v  = 0;
	mUserCon = 0;

	mFrameBinding.left =
		mFrameBinding.top =
		mFrameBinding.right =
		mFrameBinding.bottom = false;

	mVisible = mActive = mEnabled = triState_Latent;
	mRefreshAllWhenResized = true;

	mSuperView = nil;
}


// ---------------------------------------------------------------------------
//	¥ LPane									Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LPane::LPane(
	const LPane&	inOriginal)
{
									// Copy members of Original
	mPaneID					= inOriginal.mPaneID;
	mFrameSize				= inOriginal.mFrameSize;
	mFrameLocation			= inOriginal.mFrameLocation;
	mFrameBinding			= inOriginal.mFrameBinding;
	mUserCon				= inOriginal.mUserCon;
	mRefreshAllWhenResized	= inOriginal.mRefreshAllWhenResized;

	mSuperView = nil;				// Copy is not inside any View

									// Pane properties. If Original has
									//   property ON, Copy is Latent.
	mVisible = inOriginal.mVisible;
	if (mVisible == triState_On) {
		mVisible = triState_Latent;
	}

	mActive = inOriginal.mActive;
	if (mActive == triState_On) {
		mActive = triState_Latent;
	}

	mEnabled = inOriginal.mEnabled;
	if (mEnabled == triState_On) {
		mEnabled = triState_Latent;
	}
}


// ---------------------------------------------------------------------------
//	¥ LPane									Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPane::LPane(
	const SPaneInfo	&inPaneInfo)
{
	InitPane(inPaneInfo);
}


// ---------------------------------------------------------------------------
//	¥ LPane									Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPane::LPane(
	LStream*	inStream)
{
	SPaneInfo	thePaneInfo;
	SInt32 viewPtr;
	*inStream >> thePaneInfo.paneID;
	*inStream >> thePaneInfo.width;
	*inStream >> thePaneInfo.height;
	*inStream >> thePaneInfo.visible;
	*inStream >> thePaneInfo.enabled;
	*inStream >> thePaneInfo.bindings.left;
	*inStream >> thePaneInfo.bindings.top;
	*inStream >> thePaneInfo.bindings.right;
	*inStream >> thePaneInfo.bindings.bottom;
	*inStream >> thePaneInfo.left;
	*inStream >> thePaneInfo.top;
	*inStream >> thePaneInfo.userCon;
	*inStream >> viewPtr;
	thePaneInfo.superView = reinterpret_cast<LView *>(viewPtr);
	InitPane(thePaneInfo);
}


// ---------------------------------------------------------------------------
//	¥ InitPane								Initializer			   [protected]
// ---------------------------------------------------------------------------

void
LPane::InitPane(
	const SPaneInfo	&inPaneInfo)
{
	mPaneID					= inPaneInfo.paneID;
	mFrameSize.width		= inPaneInfo.width;
	mFrameSize.height		= inPaneInfo.height;
	mFrameLocation.h		= mFrameLocation.v = 0;
	mUserCon				= inPaneInfo.userCon;
	mRefreshAllWhenResized	= true;

	mVisible = triState_Off;
	if (inPaneInfo.visible) {
		mVisible = triState_Latent;
	}

	mActive = triState_Latent;

	mEnabled = triState_Off;
	if (inPaneInfo.enabled) {
		mEnabled = triState_Latent;
	}

	mFrameBinding = inPaneInfo.bindings;

		// Associate Pane with its SuperView

	mSuperView = nil;

	LView*	theSuperView = inPaneInfo.superView;
	if (theSuperView == Default_SuperView) {
		theSuperView = GetDefaultView();
	}

	PutInside(theSuperView, false);
	PlaceInSuperImageAt(inPaneInfo.left, inPaneInfo.top, false);

		// Negative width and/or height means to expand the
		// Pane to the size of the SuperView

	if (theSuperView != nil) {
		Boolean	expandHoriz = (inPaneInfo.width < 0);
		Boolean	expandVert = (inPaneInfo.height < 0);
		if (expandHoriz || expandVert) {
			theSuperView->ExpandSubPane(this, expandHoriz, expandVert);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LPane								Destructor				  [public]
// ---------------------------------------------------------------------------

LPane::~LPane()
{
	PutInside(nil);

	if (sLastPaneClicked == this) {
		sLastPaneClicked = nil;
	}

	if (sLastPaneMoused == this) {
		sLastPaneMoused = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ FinishCreate
// ---------------------------------------------------------------------------
//	Wrapper function for FinishCreateSelf
//	You will rarely want to override this function

void
LPane::FinishCreate()
{
	if (mSuperView != nil) {
		mSuperView->OrientSubPane(this);
	}

	if (ExecuteAttachments(msg_FinishCreate, this)) {
		FinishCreateSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf
// ---------------------------------------------------------------------------
//	Finish Creating a Pane
//
//	This function gets called after creating a Pane from a data stream.
//	Override to perform finishing touches that depend on the entire
//	Pane hierarchy being constructed.
//
//	For example, if a View wants to store a pointer to a SubPane,
//	it should override this function to call FindPaneByID for that
//	Pane. This saves the overhead of repeatedly calling FindPaneByID
//	when the View wants to access that SubPane. You can't do this from
//	a Constructor because SubPanes are created after their SuperView.

void
LPane::FinishCreateSelf()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsAreaInQDSpace										 [static] [public]
// ---------------------------------------------------------------------------

Boolean
LPane::IsAreaInQDSpace(
	SInt32	inLeft,
	SInt32	inTop,
	SInt16	inWidth,
	SInt16	inHeight)
{
	return ( (inLeft > min_QDCoord)  &&
			 (inTop > min_QDCoord)  &&
			 (inLeft + inWidth < max_QDCoord)  &&
			 (inTop + inHeight < max_QDCoord) );
}


// ---------------------------------------------------------------------------
//	¥ GetDeviceInfo													  [public]
// ---------------------------------------------------------------------------
//	Passes back the bit depth and color capability of the device being used
//	for drawing

void
LPane::GetDeviceInfo(
	SInt16&		outBitDepth,
	bool&		outHasColor) const
{
	GrafPtr	port = GetMacPort();
	
	if (UDrawingUtils::IsPortBuffered(port)) {
										// Get info from buffered port
		outBitDepth = UDrawingUtils::GetPortPixelDepth(port);
		outHasColor = ::IsPortColor(port);

	} else if (sBitDepth == 0) {		// Not in a drawing loop

		outBitDepth = 1;
		outHasColor = false;

		Rect	frame;					// Get frame in global coordinates
		if (CalcPortFrameRect(frame)) {
			PortToGlobalPoint(topLeft(frame));
			PortToGlobalPoint(botRight(frame));

										// Find the device with the greatest
										//   depth that intersects our frame
			GDHandle	maxDevice = ::GetMaxDevice(&frame);

			if (maxDevice != nil) {
				outBitDepth	= (**((**maxDevice).gdPMap)).pixelSize;
				outHasColor = UDrawingUtils::DeviceSupportsColor(maxDevice);

			}
		}

	} else {							// Inside a drawing loop
		outBitDepth	= sBitDepth;		// Pass back stored values
		outHasColor = sDeviceHasColor;
	}
}


// ---------------------------------------------------------------------------
//	¥ FindPaneByID													  [public]
// ---------------------------------------------------------------------------

LPane*
LPane::FindPaneByID(
	PaneIDT	inPaneID)
{
	return (inPaneID == mPaneID) ? this : nil;
}


// ---------------------------------------------------------------------------
//	¥ FindConstPaneByID												  [public]
// ---------------------------------------------------------------------------

const LPane*
LPane::FindConstPaneByID(
	PaneIDT	inPaneID) const
{
	return (inPaneID == mPaneID) ? this : nil;
}


// ---------------------------------------------------------------------------
//	¥ GetMacPort													  [public]
// ---------------------------------------------------------------------------

GrafPtr
LPane::GetMacPort() const
{
	return (mSuperView == nil) ? nil : mSuperView->GetMacPort();
}


// ---------------------------------------------------------------------------
//	¥ GetMacWindow													  [public]
// ---------------------------------------------------------------------------

WindowPtr
LPane::GetMacWindow() const
{
	return (mSuperView == nil) ? nil : mSuperView->GetMacWindow();
}


// ---------------------------------------------------------------------------
//	¥ GetCGContext													  [public]
// ---------------------------------------------------------------------------
//	Return Core Graphics context

CGContextRef
LPane::GetCGContext() const
{
	#if TARGET_API_MAC_CARBON
		return (mSuperView == nil) ? nil : mSuperView->GetCGContext();
		
	#else
		return nil;
	#endif
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

SInt32
LPane::GetValue() const
{
	return 0;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LPane::SetValue(
	SInt32	/* inValue */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LPane::GetDescriptor(
	Str255	outDescriptor) const
{
	outDescriptor[0] = 0;
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LPane::SetDescriptor(
	ConstStringPtr	/* inDescriptor */)
{
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------

CFStringRef
LPane::CopyCFDescriptor() const
{
	Str255	descriptor;
	GetDescriptor(descriptor);
	
	return ::CFStringCreateWithPascalString( nil,
											 descriptor,
											 ::CFStringGetSystemEncoding() );
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LPane::SetCFDescriptor(
	CFStringRef	inStringRef)
{
	Str255	descriptor;
	::CFStringGetPascalString( inStringRef,
							   descriptor,
							   sizeof(descriptor),
							   ::CFStringGetSystemEncoding() );
	SetDescriptor(descriptor);
}

#endif

// ---------------------------------------------------------------------------
//	¥ GetUserCon													  [public]
// ---------------------------------------------------------------------------

SInt32
LPane::GetUserCon() const
{
	return mUserCon;
}


// ---------------------------------------------------------------------------
//	¥ SetUserCon													  [public]
// ---------------------------------------------------------------------------

void
LPane::SetUserCon(
	SInt32	inUserCon)
{
	mUserCon = inUserCon;
}


// ---------------------------------------------------------------------------
//	¥ GetSelection
// ---------------------------------------------------------------------------
//	Obtains the current selection within the object.
//
//	Subclasses should override if they contain a selection. Pass back
//	an AEDesc created by ::AECreateDesc(). Caller is responsible for
//	deleting the AEDesc. For example,
//
//		StAEDescriptor	theDesc;		// Destructor will dispose theDesc
//		theEditField->GetSelection(theDesc);
//
//		DoSomethingWithTheDesc( theDesc );

void
LPane::GetSelection(
	AEDesc&		outDesc) const
{
	outDesc.descriptorType = typeNull;	// Default is no selection
	outDesc.dataHandle     = nil;
}


// ---------------------------------------------------------------------------
//	¥ GetFrameSize
// ---------------------------------------------------------------------------
//	Get the width and height, in pixels, of the Frame of a Pane

void
LPane::GetFrameSize(
	SDimension16	&outSize) const
{
	outSize = mFrameSize;
}


// ---------------------------------------------------------------------------
//	¥ Perform
// ---------------------------------------------------------------------------
//	Virtual function that you can override to add program-specific behavior
//	to your Pane subclasses. Useful for situations where you are dealing
//	with generic LPane objects and don't want to use RTTI.

SInt32
LPane::Perform(
	MessageT	/* inMessage */,
	void*		/* ioParam */)
{
	return 0;
}


// ---------------------------------------------------------------------------
//	¥ GetFrameLocation
// ---------------------------------------------------------------------------
//	Get the location of a Pane's Frame
//
//	The location is in 32-bit Port coordinates

void
LPane::GetFrameLocation(
	SPoint32	&outLocation) const
{
	outLocation = mFrameLocation;
}


void
LPane::SetFrameBinding(
	const SBooleanRect	&inFrameBinding)
{
	mFrameBinding = inFrameBinding;
}


void
LPane::GetFrameBinding(
	SBooleanRect	&outFrameBinding) const
{
	outFrameBinding = mFrameBinding;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortFrameRect
// ---------------------------------------------------------------------------
//	Calculate the Pane's Frame Rectangle in Port coordinates
//
//	Returns true if the Frame is within QuickDraw space (16-bit)
//	Returns false if the Frame is outside QuickDraw space
//		and outRect is unchanged

Boolean
LPane::CalcPortFrameRect(
	Rect	&outRect) const
{
	Boolean		isInQDSpace =
				IsAreaInQDSpace(mFrameLocation.h, mFrameLocation.v,
								mFrameSize.width, mFrameSize.height);
	if (isInQDSpace) {
		outRect.left	= (SInt16) mFrameLocation.h;
		outRect.top		= (SInt16) mFrameLocation.v;
		outRect.right	= (SInt16) (outRect.left + mFrameSize.width);
		outRect.bottom	= (SInt16) (outRect.top + mFrameSize.height);
	}
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalFrameRect
// ---------------------------------------------------------------------------
//	Calculate the Pane's Frame Rectangle in local (Image) coordinates
//
//	Returns true if the Frame is within QuickDraw space (16-bit)
//	Returns false if the Frame is outside QuickDraw space
//		and outRect is unchanged

Boolean
LPane::CalcLocalFrameRect(
	Rect	&outRect) const
{
	Boolean		isInQDSpace = CalcPortFrameRect(outRect);
	if (isInQDSpace) {
		PortToLocalPoint(topLeft(outRect));
		PortToLocalPoint(botRight(outRect));
	}
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortExposedRect
// ---------------------------------------------------------------------------
//	Calculate the exposed rectangle of a Pane and return whether that
//	rectangle is empty. The rectangle is in Port coordinates.
//
//	A Pane is exposed if these four conditions are true:
//		It is visbile or inOKIfHidden is true
//		Frame is in QuickDraw space
//		Has a SuperView
//		Frame intersects SuperView's revealed rect
//
//	Pass inOKIfHidden of true to calculate the area of the Pane that
//	would be exposed if the Pane were visible.

Boolean
LPane::CalcPortExposedRect(
	Rect&	outRect,						// In Port coords
	bool	inOKIfHidden) const
{
	bool exposed = ((mVisible == triState_On) || inOKIfHidden)  &&
				   CalcPortFrameRect(outRect)  &&
				   (mSuperView != nil);

	if (exposed) {
		Rect	superRevealed;
		mSuperView->GetRevealedRect(superRevealed);

		exposed = ::SectRect(&outRect, &superRevealed, &outRect);
	}

	return exposed;
}


// ---------------------------------------------------------------------------
//	¥ GetLocalUpdateRgn
// ---------------------------------------------------------------------------
//	Return the region being updated
//
//	The region will be empty if called outside of a Draw() operation

RgnHandle
LPane::GetLocalUpdateRgn()
{
	RgnHandle	localUpdateRgnH = nil;

	if (mSuperView != nil) {
		localUpdateRgnH = mSuperView->GetLocalUpdateRgn();
	}

	return localUpdateRgnH;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameTo
// ---------------------------------------------------------------------------
//	Set the dimensions of the Frame
//
//		inWidth and inHeight specify the new size in pixels

void
LPane::ResizeFrameTo(
	SInt16		inWidth,
	SInt16		inHeight,
	Boolean		inRefresh)
{
	ResizeFrameBy((SInt16) (inWidth - mFrameSize.width),
				  (SInt16) (inHeight - mFrameSize.height),
				  inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		inWidthDelta and inHeightDelta specify, in pixels, how much larger
//		to make the Frame. Positive deltas increase the size, negative deltas
//		reduce the size.

void
LPane::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
									// Save area of Pane before resizing in
									//   case we want to refresh
	Rect	beforeRect = Rect_0000;
	Boolean	exposedBefore = CalcPortExposedRect(beforeRect);

									// Adjust width and height of Frame
	mFrameSize.width += inWidthDelta;
	mFrameSize.height += inHeightDelta;

									// Refresh if desired and Pane was
									//   exposed after or before resizing
	Rect	afterRect = Rect_0000;
	
	if ( inRefresh  &&
		 (CalcPortExposedRect(afterRect) || exposedBefore) ) {
		 
		 	// Make region covering the area including both the
		 	// before and after rectangles
		 
		 StRegion	refreshRgn(beforeRect);
		 refreshRgn += afterRect;
		 
		 if (not mRefreshAllWhenResized) {
		 
				// Area of Pane that is the same between the old
				// and new sizes does not need to be refreshed.
				// Remove the common area from the region to be
				// invalidated.
		 	
		 	Rect	commonRect;
		 	::SectRect(&beforeRect, &afterRect, &commonRect);
		 	
		 	refreshRgn -= commonRect;
		 }

		InvalPortRgn(refreshRgn);		// Refresh "changed" area
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//		inHorizDelta and inVertDelta specify, in pixels, how far to move the
//		Frame (within its surrounding Image). Positive horiz deltas move to
//		the right, negative to the left. Positive vert deltas move down,
//		negative up.

void
LPane::MoveBy(
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

	mFrameLocation.h += inHorizDelta;
	mFrameLocation.v += inVertDelta;

	if (inRefresh) {
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ PlaceInSuperFrameAt
// ---------------------------------------------------------------------------
//	Place the Pane at a location relative to the Frame of its SuperView
//
//		inHorizOffset specifies, in pixels, how far the left edge of the
//		Frame is from the left edge of its Parent Frame. Positive offsets
//		are to the left, negative to the right.
//
//		inVertOffset specifies, in pixels, how far the top edge of the
//		Frame is from the top edge of its Parent Frame. Positive offsets
//		are below, negative above.

void
LPane::PlaceInSuperFrameAt(
	SInt32		inHorizOffset,
	SInt32		inVertOffset,
	Boolean		inRefresh)
{
	SPoint32	superFrameLoc;
	superFrameLoc.h = superFrameLoc.v = 0;

	if (mSuperView != nil) {
		mSuperView->GetFrameLocation(superFrameLoc);
	}

									// Move by difference between new
									//   and current location
	MoveBy(superFrameLoc.h + inHorizOffset - mFrameLocation.h,
		   superFrameLoc.v + inVertOffset - mFrameLocation.v,
		   inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ PlaceInSuperImageAt
// ---------------------------------------------------------------------------
//	Place the Pane within the Image of its SuperView
//
//	inHorizOffset and inVertOffset specify the distance of the top left of
//	the Frame from the top left of the Image of its SuperView

void
LPane::PlaceInSuperImageAt(
	SInt32		inHorizOffset,
	SInt32		inVertOffset,
	Boolean		inRefresh)
{
	SPoint32	superImageLoc;
	superImageLoc.h = superImageLoc.v = 0;

	if (mSuperView != nil) {
		mSuperView->GetImageLocation(superImageLoc);
	}

									// Move by difference between new
									//   and current location
	MoveBy(superImageLoc.h + inHorizOffset - mFrameLocation.h,
		   superImageLoc.v + inVertOffset - mFrameLocation.v,
		   inRefresh);
}


// ---------------------------------------------------------------------------
//	¥ SavePlace
// ---------------------------------------------------------------------------
//	Write size and location information to a Stream for later retrieval
//	by the RestorePlace() function

void
LPane::SavePlace(
	LStream		*outPlace)
{
	*outPlace << mFrameSize.width;
	*outPlace << mFrameSize.height;

	*outPlace << mFrameLocation.h;
	*outPlace << mFrameLocation.v;
}


// ---------------------------------------------------------------------------
//	¥ RestorePlace
// ---------------------------------------------------------------------------
//	Read size and location information stored in a Stream by the
//	SavePlace() function

void
LPane::RestorePlace(
	LStream		*inPlace)
{
	SDimension16	frameSize;
	*inPlace >> frameSize.width;
	*inPlace >> frameSize.height;

	SPoint32		frameLocation;
	*inPlace >> frameLocation.h;
	*inPlace >> frameLocation.v;

		// Restore the Pane to the saved location and size

	MoveBy(frameLocation.h - mFrameLocation.h,
		   frameLocation.v - mFrameLocation.v, false);
	ResizeFrameTo(frameSize.width, frameSize.height, false);
}


// ---------------------------------------------------------------------------
//	¥ PutInside
// ---------------------------------------------------------------------------
//	Put Pane inside the specified View. Location is unspecified.
//
//	The inOrient parameter specifies whether or not to orient the
//	Pane within its new SuperView. The default value is true, which
//	is what you'll normally want.
//
//	InitPane() calls PutInside() with inOrient set to false since
//	the Pane is not fully constructed at that point, meaning that
//	any functions called by OrientSubPane() will be the ones for
//	LPane, and not any overrides. This may result in the Pane not
//	being properly oriented. After creating a Pane, you should call
//	FinishCreate(), which will call OrientSubPane() for the fully
//	constructed Pane object.

void
LPane::PutInside(
	LView	*inView,
	Boolean	inOrient)
{
	if (mSuperView != nil) {			// Sever ties with old Parent
		mSuperView->RemoveSubPane(this);
	}

	mSuperView = inView;

	if (inView != nil) {				// Attach to new Parent
		inView->AddSubPane(this);
		if (inOrient) {
			inView->OrientSubPane(this);
		}
		AdaptToNewSurroundings();		// Adjust state to reflect new Parent
	}
}


// ---------------------------------------------------------------------------
//	¥ AdaptToNewSurroundings
// ---------------------------------------------------------------------------
//	Adjust state of Pane when installed in a new Parent Pane

void
LPane::AdaptToNewSurroundings()
{
}


// ---------------------------------------------------------------------------
//	¥ AdaptToSuperFrameSize
// ---------------------------------------------------------------------------
//	Adjust state of Pane when size of SuperView's Frame changes by the
//	specified amounts

void
LPane::AdaptToSuperFrameSize(
	SInt32	inSurrWidthDelta,			// Change in width of SuperView
	SInt32	inSurrHeightDelta,			// Change in height of SuperView
	Boolean	inRefresh)
{
		// When SuperView changes size, a Pane may move or change
		// size, depending on how each of its sides is "bound" to
		// that of its SuperView. A side that is bound always remains
		// the same distance from the corresponding side of its SuperView.

	SInt32		widthDelta  = 0;
	SInt32		heightDelta = 0;
	SInt32		horizDelta  = 0;
	SInt32		vertDelta   = 0;

	if (mFrameBinding.right) {
		if (mFrameBinding.left) {
										// Both right and left are bound
										// Pane resizes horizontally
			widthDelta = inSurrWidthDelta;
		} else {
										// Right bound, left free
										// Pane moves horizontally
			horizDelta = inSurrWidthDelta;
		}
	}

	if (mFrameBinding.bottom) {
		if (mFrameBinding.top) {
										// Both bottom and top are bound
										// Pane resizes vertically
			heightDelta = inSurrHeightDelta;
		} else {
										// Bottom bound, left free
										// Pane moves vertically
			vertDelta = inSurrHeightDelta;
		}
	}

		// Perform the actual move and/or resize. Do the move first
		// because the resize can cause an immediate redraw.

	if ( (horizDelta != 0) || (vertDelta != 0) ) {
		MoveBy(horizDelta, vertDelta, inRefresh);
	}

	if ( (widthDelta != 0) || (heightDelta != 0) ) {
		ResizeFrameBy((SInt16) widthDelta, (SInt16) heightDelta, inRefresh);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdaptToSuperScroll
// ---------------------------------------------------------------------------
//	Adjust state of Pane when its SuperView scrolls by the specified amounts

void
LPane::AdaptToSuperScroll(
	SInt32	inHorizScroll,
	SInt32	inVertScroll)
{
	SInt32	horizDelta = 0;
	SInt32	vertDelta  = 0;

	if (!mFrameBinding.left && !mFrameBinding.right) {
										// Pane is not bound horizontally
		horizDelta = -inHorizScroll;	// It moves along with the SuperImage
	}

	if (!mFrameBinding.top && !mFrameBinding.bottom) {
										// Pane is not bound vertically
		vertDelta = -inVertScroll;		// It moves along with the SuperImage
	}

	if ( (horizDelta != 0) || (vertDelta != 0) ) {
		MoveBy(horizDelta, vertDelta, false);
	}
}

#pragma mark -

// ===========================================================================
// ¥ Mouse Clicks												Mouse Clicks ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ PointIsInFrame
// ---------------------------------------------------------------------------
//	Return whether a Point is inside the Frame of a Pane
//
//	inHorizPort and inVertPort are in Port coordinates

Boolean
LPane::PointIsInFrame(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
	return ( (inHorizPort >= mFrameLocation.h)  &&
			 (inHorizPort < mFrameLocation.h + mFrameSize.width)  &&
			 (inVertPort >= mFrameLocation.v)  &&
			 (inVertPort < mFrameLocation.v + mFrameSize.height) );
}


// ---------------------------------------------------------------------------
//	¥ IsHitBy
// ---------------------------------------------------------------------------
//	Return whether a Pane is hit by the specified point, which is in
//	Port coordinates
//
//	A Pane is hit if the point is inside its Frame and the Pane is enabled

Boolean
LPane::IsHitBy(
	SInt32	inHorizPort,
	SInt32	inVertPort)
{
	return (PointIsInFrame(inHorizPort, inVertPort) && IsEnabled());
}


// ---------------------------------------------------------------------------
//	¥ FindSubPaneHitBy
// ---------------------------------------------------------------------------
//	Find the SubPane of this Pane that is hit by the specified point.
//	Return nil if no SubPane is hit
//
//	inHorizPort and inVertPort are in Port coordinates

LPane*
LPane::FindSubPaneHitBy(
	SInt32	/* inHorizPort */,
	SInt32	/* inVertPort */) const
{
	return nil;					// A Pane has no SubPanes
}


// ---------------------------------------------------------------------------
//	¥ Contains
// ---------------------------------------------------------------------------
//	Return whether a Pane is contains the specified point, which is in
//	Port coordinates
//
//	Note: This function just calls PointIsInFrame(), but it exists
//	so that you can override it to change how FindDeepSubPaneContaining
//	and FindShallowSubPaneContaining search for Panes.

Boolean
LPane::Contains(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
	return PointIsInFrame(inHorizPort, inVertPort);
}


// ---------------------------------------------------------------------------
//	¥ FindDeepSubPaneContaining
// ---------------------------------------------------------------------------
//	Return the most deeply nested SubPane which contains the specified
//	point, which is in Port coordinates

LPane*
LPane::FindDeepSubPaneContaining(
	SInt32	/* inHorizPort */,
	SInt32	/* inVertPort */) const
{
	return nil;					// A Pane has no SubPanes
}


// ---------------------------------------------------------------------------
//	¥ FindShallowSubPaneContaining
// ---------------------------------------------------------------------------
//	Return the immediate SubPane which contains the specified point,
//	which is in Port coordinates

LPane*
LPane::FindShallowSubPaneContaining(
	SInt32	/* inHorizPort */,
	SInt32	/* inVertPort */) const
{
	return nil;					// A Pane has no SubPanes
}


// ---------------------------------------------------------------------------
//	¥ Click
// ---------------------------------------------------------------------------
//	Wrapper function for handling a click inside a Pane
//
//	This function does some bookkeeping, executes Attachments, then
//	calls ClickSelf() to actually respond to the click.

void
LPane::Click(
	SMouseDownEvent	&inMouseDown)
{
	if (!inMouseDown.delaySelect) {

			// Until now, whereLocal is actually in port coords. Now
			// that we know what Pane is handling the click, we can
			// convert it to the proper local coords.

		PortToLocalPoint(inMouseDown.whereLocal);

		UpdateClickCount(inMouseDown);

		if (ExecuteAttachments(msg_Click, &inMouseDown)) {
			ClickSelf(inMouseDown);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside this Pane
//
//	Override to do something when the user clicks inside a Pane

void
LPane::ClickSelf(
	const SMouseDownEvent&	/* inMouseDown */)
{
}


// ---------------------------------------------------------------------------
//	¥ EventMouseUp
// ---------------------------------------------------------------------------
//	Respond to a mouse up event following a click (mouse down) inside a Pane

void
LPane::EventMouseUp(
	const EventRecord	&inMouseUp)
{
	sWhenLastMouseUp = inMouseUp.when;	// Needed for multi-click detection
}


// ---------------------------------------------------------------------------
//	¥ UpdateClickCount
// ---------------------------------------------------------------------------
//	Determine if the mouse down is part of a multi-click and set internal
//	counters

void
LPane::UpdateClickCount(
	const SMouseDownEvent	&inMouseDown)
{
		// Three conditions necessary for a multi-click:
		//   1) This is the same Pane as was hit with the last mouse down
		//   2) Times of previous and current clicks are close enough
		//   3) Locations of previous and current clicks are close enough

	if ( (sLastPaneClicked == this) &&
		 ClickTimesAreClose(inMouseDown.macEvent.when) &&
		 PointsAreClose(sWhereLastMouseDown, inMouseDown.whereLocal) ) {

			 sClickCount++;

	} else {
		sClickCount = 1;
	}

									// Save info about this click
	sLastPaneClicked = this;
	
	sWhereLastMouseDown		= inMouseDown.whereLocal;
	sWhenLastMouseDown		= inMouseDown.macEvent.when;
	sModifiersLastMouseDown	= inMouseDown.macEvent.modifiers;
}


// ---------------------------------------------------------------------------
//	¥ PointsAreClose
// ---------------------------------------------------------------------------
//	Return whether the two points are close enough to be part of a
//	multi-click. Points are in Local coordinates.

Boolean
LPane::PointsAreClose(
	Point	inFirstPt,
	Point	inSecondPt) const
{
	SInt16	hDelta = (SInt16) (inFirstPt.h - inSecondPt.h);
	if (hDelta < 0) {
		hDelta = (SInt16) (-hDelta);
	}

	SInt16	vDelta = (SInt16) (inFirstPt.v - inSecondPt.v);
	if (vDelta < 0) {
		vDelta = (SInt16) (-vDelta);
	}

	return ( (hDelta <= multiClick_Distance) &&
			 (vDelta <= multiClick_Distance) );
}


// ---------------------------------------------------------------------------
//	¥ ClickTimesAreClose
// ---------------------------------------------------------------------------
//	Return whether the time between the specified time and the time of the
//	last mouse down are close enough to be considered a double-click

Boolean
LPane::ClickTimesAreClose(
	UInt32	inLastClickTime) const
{
	return ((inLastClickTime - sWhenLastMouseDown) <= GetDblTime());
}


#if PP_Uses_ContextMenus
// ---------------------------------------------------------------------------
//	¥ ContextClick
// ---------------------------------------------------------------------------
//	Handle a contextual menu click by passing the click up the visual
//	hierarchy.

OSStatus
LPane::ContextClick (
	Point				inGlobalPt)
{
	OSStatus			result = noErr;

	if (ExecuteAttachments(msg_ContextClick, (void*) &inGlobalPt)) {
		if (not ContextClickSelf(inGlobalPt)) {
			LView *		superView = GetSuperView();
			if (superView) {
				result = superView->ContextClick(inGlobalPt);
			} else {
				result = eventNotHandledErr;
			}
		}
	}
	return result;
}


// ---------------------------------------------------------------------------
//	¥ ContextClickSelf
// ---------------------------------------------------------------------------
//	Handle a contextual menu click by passing the click up the visual
//	hierarchy. Override if this object has an associated contextual menu.

bool
LPane::ContextClickSelf (
	Point				/* inGlobalPt */)
{
	return false;
}
#endif


// ---------------------------------------------------------------------------
//	¥ AdjustMouse
// ---------------------------------------------------------------------------

void
LPane::AdjustMouse(
	Point				inPortPt,
	const EventRecord&	inMacEvent,
	RgnHandle			outMouseRgn)	// In Port coords
{
	Rect	frame;						// Set mouse region to our Frame
	CalcPortFrameRect(frame);
	::RectRgn(outMouseRgn, &frame);
	
	SAdjustMouseEvent	adjuster;		// Load event and mouse region
	adjuster.event	  = inMacEvent;		//   for Attachments
	adjuster.mouseRgn = outMouseRgn;

	if (ExecuteAttachments(msg_AdjustCursor, (void*) &adjuster)) {
										// Let this Pane adjust the mouse
		AdjustMouseSelf(inPortPt, inMacEvent, outMouseRgn);
	}
	
	if (mSuperView != nil) {			// Clip mouse region to revealed
		Rect	revealed;				//    area of our SuperView
		mSuperView->GetRevealedRect(revealed);
	
		::SectRgn(outMouseRgn, StRegion(revealed), outMouseRgn);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustMouseSelf
// ---------------------------------------------------------------------------
//	Set cursor and mouse region
//
//	On entry, ioMouseRgn is our whole Frame in Port coords. This mouse
//	region (converted to global coordinates by LWindow) gets passed
//	to WaitNextEvent.
//
//	Subclasses should override to set the cursor shape. If the cursor
//	shape is different for different areas within the Pane, set
//	ioMouseRgn to the region of constant cursor shape containing
//	inPortPt. The region must be in Port coordinates.

void
LPane::AdjustMouseSelf(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */,
	RgnHandle			/* ioMouseRgn */)
{
	UCursor::SetArrow();				// Arrow is the default cursor
}



#pragma mark -

// ---------------------------------------------------------------------------
//	¥ MouseEnter
// ---------------------------------------------------------------------------
//	Hook function to call when the mouse enters a Pane
//
//	The standard PP event dispatcher does NOT call this function, or
//	MouseLeave or MouseWithin. You must call these functions from your
//	own code, deciding yourself the appropriate conditions. For an example,
//	look at the LMouseTracker class

void
LPane::MouseEnter(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */)
{
}


// ---------------------------------------------------------------------------
//	¥ MouseLeave
// ---------------------------------------------------------------------------
//	Hook function to call when the mouse leaves a Pane
//
//	See comments for MouseEnter()

void
LPane::MouseLeave()
{
}


// ---------------------------------------------------------------------------
//	¥ MouseWithin
// ---------------------------------------------------------------------------
//	Hook function to call while the mouse is within a Pane
//
//	See comments for MouseEnter()

void
LPane::MouseWithin(
	Point				/* inPortPt */,
	const EventRecord&	/* inMacEvent */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsVisible
// ---------------------------------------------------------------------------
//	Return whether a Pane is visible
//
//	A Pane is "visible" if its visible flag is on

#pragma mark LPane::IsVisible

//	Boolean
//	LPane::IsVisible() const			// Define inline


// ---------------------------------------------------------------------------
//	¥ Show
// ---------------------------------------------------------------------------
//	Make a Pane visible

void
LPane::Show()
{
	if (mVisible == triState_Off) {		// Pane is not current visible

		if ((mSuperView != nil) && (mSuperView->mVisible == triState_On)) {
										// SuperView is visible, so this
			mVisible = triState_On;		//   Pane becomes visible
			Refresh();
			ShowSelf();

		} else {						// SuperView is not visible, so
			mVisible = triState_Latent;	//    this Pane is latently visible
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperShow
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has become visible

void
LPane::SuperShow()
{
		// A latently visible Pane becomes "really" visible when its
		// SuperView becomes visible. We don't call Refresh() here,
		// since the SuperView, completely encloses this Pane, will have
		// have been refreshed already.

	if (mVisible == triState_Latent) {
		mVisible = triState_On;
		ShowSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------
//	Pane is being made visible
//
//	Override for Pane classes that need to be informed when they are
//	made visible

void
LPane::ShowSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ Hide
// ---------------------------------------------------------------------------
//	Make a Pane invisible

void
LPane::Hide()
{
	if (mVisible != triState_Off) {	// Pane is Visible On or Latent

		Refresh();					// Refresh area currently covered by Pane
									//   so that what's underneath will
									//   be redrawn

									// Turn Off Visible flag and call
									//   HideSelf() if Pane was Visible On
									//   (but not if Latent)
		Boolean		wasVisible = (mVisible == triState_On);
		mVisible = triState_Off;
		if (wasVisible) {
			HideSelf();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperHide
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has been hidden

void
LPane::SuperHide()
{
		// If a Pane is Visible, hiding its SuperView makes the Pane
		// latently Visible. The Pane will not be visible on the screen,
		// so we call HideSelf() to inform it. However, we don't call
		// Refresh() since the SuperView, which completely encloses
		// this Pane, will have been refreshed already.

	if (mVisible == triState_On) {
		mVisible = triState_Latent;
		HideSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	Pane is being made invisible
//
//	Override for Pane classes that need to be informed when they are
//	made invisible

void
LPane::HideSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ SetVisible
// ---------------------------------------------------------------------------
//	Convenience function to show or hide a Pane based on a bool parameter

void
LPane::SetVisible(
	bool	inMakeVisible)
{
	if (inMakeVisible) {
		Show();
	} else {
		Hide();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsActive
// ---------------------------------------------------------------------------
//	Return whether a Pane is active
//
//	A Pane is "active" if its active flag is on and it is visible

Boolean
LPane::IsActive() const
{
	return ((mActive == triState_On) && IsVisible());
}


// ---------------------------------------------------------------------------
//	¥ Activate
// ---------------------------------------------------------------------------
//	Activate a Pane

void
LPane::Activate()
{
	if (mActive == triState_Off) {		// Active flag is off

			// Active is a hierarchical property, meaning that a Pane
			// can't be active unless its SuperView is active

		if ((mSuperView != nil) && (mSuperView->mActive == triState_On)) {
										// SuperView Active flag is on
			mActive = triState_On;		//   so we activate this Pane
			ActivateSelf();

		} else {						// SuperView Active flag is off or
										//   latent, so this Pane is
			mActive = triState_Latent;	//   latently Active
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperActivate
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has been Activated

void
LPane::SuperActivate()
{
		// A latently Active Pane is truly "active" when its SuperView
		// becomes Active

	if (mActive == triState_Latent) {
		mActive = triState_On;
		ActivateSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf
// ---------------------------------------------------------------------------
//	Pane is being Activated
//
//	Override for Pane classes that change appearance when activated.

void
LPane::ActivateSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ Deactivate
// ---------------------------------------------------------------------------
//	Deactivate a Pane

void
LPane::Deactivate()
{
		// Check state of Active flag, change it to off, then call
		// DeactivateSelf() if the flag was on originally. We change
		// the flag before calling DeactivateSelf() in case any code called
		// by that function checks the state of the flag.

	Boolean		wasActive = (mActive == triState_On);
	mActive = triState_Off;
	if (wasActive) {
		DeactivateSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperDeactivate
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has been Deactivated

void
LPane::SuperDeactivate()
{
		// If a Pane is Active, deactivating its SuperView makes the Pane
		// latently Active. We call DeactivateSelf() since a latently
		// active Pane is visually inactive.

	if (mActive == triState_On) {
		mActive = triState_Latent;
		DeactivateSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf
// ---------------------------------------------------------------------------
//	Pane is being Deactivated
//
//	Override for Pane classes that change appearance when deactivated.

void
LPane::DeactivateSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ SetActive
// ---------------------------------------------------------------------------
//	Convenience function to activate or deactivate a Pane based on a bool
//	parameter

void
LPane::SetActive(
	bool	inMakeActive)
{
	if (inMakeActive) {
		Activate();
	} else {
		Deactivate();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsEnabled
// ---------------------------------------------------------------------------
//	Return whether a Pane is enabled
//
//	A Pane is "enabled" if its enabled flag is on and it is visible

Boolean
LPane::IsEnabled() const
{
	return ((mEnabled == triState_On) && IsVisible());
}


// ---------------------------------------------------------------------------
//	¥ Enable
// ---------------------------------------------------------------------------
//	Enable a Pane

void
LPane::Enable()
{
	if (mEnabled == triState_Off) {		// Enabled flag is off

			// Enabled is a hierarchical property, meaning that a Pane
			// can't be enabled unless its SuperView is enabled

		if ((mSuperView != nil) && (mSuperView->mEnabled == triState_On)) {
										// SuperView Enabled flag is on
			mEnabled = triState_On;		//   so we enable this Pane
			EnableSelf();

		} else {						// SuperView Enabled flag is
										//   latent or off, so this Pane
			mEnabled = triState_Latent;	//   is latently Enabled
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperEnable
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has been Enabled

void
LPane::SuperEnable()
{
		// A latently Enabled Pane is truly "enabled" when its SuperView
		// becomes Enabled

	if (mEnabled == triState_Latent) {
		mEnabled = triState_On;
		EnableSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf
// ---------------------------------------------------------------------------
//	Pane is being Enabled
//
//	Override for Pane classes that change appearance when enabled.
//	To avoid flicker, a Pane can draw immediately from this function
//	rather than refreshing and drawing at the next update event.

void
LPane::EnableSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ Disable
// ---------------------------------------------------------------------------
//	Disable a Pane

void
LPane::Disable()
{
		// Check state of Enabled flag, change it to off, then call
		// DisableSelf() if the flag was on originally. We change
		// the flag before calling DisableSelf() in case any code called
		// by that function checks the state of the flag.

	Boolean		wasEnabled = (mEnabled == triState_On);
	mEnabled = triState_Off;
	if (wasEnabled) {
		DisableSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperDisable
// ---------------------------------------------------------------------------
//	The SuperView of a Pane has been Disabled

void
LPane::SuperDisable()
{
		// If a Pane is enabled, disabling its SuperView makes the Pane
		// latently enabled. We call DisableSelf() since a latently
		// enabled Pane is visually disabled

	if (mEnabled == triState_On) {
		mEnabled = triState_Latent;
		DisableSelf();
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf
// ---------------------------------------------------------------------------
//	Pane is being Disabled
//
//	Override for Pane classes that change appearance when disabled.
//	To avoid flicker, a Pane can draw immediately from this function
//	rather than refreshing and drawing at the next update event.

void
LPane::DisableSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ SetEnabled
// ---------------------------------------------------------------------------
//	Convenience function to enable or disable a Pane based on a bool parameter

void
LPane::SetEnabled(
	bool	inMakeEnabled)
{
	if (inMakeEnabled) {
		Enable();
	} else {
		Disable();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Refresh
// ---------------------------------------------------------------------------
//	Invalidate the area occupied by a Pane.
//
//	This forces an Update event that, when processed, will redraw the Pane.
//	Refresh does nothing if the Pane is not exposed.

void
LPane::Refresh()
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect)) {
		InvalPortRect(&refreshRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ RefreshRect
// ---------------------------------------------------------------------------
//	Invalidate a rectanglar portion of a Pane

void
LPane::RefreshRect(
	const Rect&		inRect)				// In Local coordinates
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect)) {

		Rect	portRect = inRect;		// Convert input rect to Port coords
		LocalToPortPoint(topLeft(portRect));
		LocalToPortPoint(botRight(portRect));

		if (::SectRect(&refreshRect, &portRect, &refreshRect)) {
			InvalPortRect(&refreshRect);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ RefreshRgn
// ---------------------------------------------------------------------------
//	Invalidate a region of a Pane

void
LPane::RefreshRgn(
	RgnHandle	inRegionH)				// In Local coordinates
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect)) {

										// Make copy of input region and
										//   convert to Port coords
		StRegion	portRgn = inRegionH;
		Point		offset  = Point_00;
		LocalToPortPoint(offset);
		::MacOffsetRgn(portRgn, offset.h, offset.v);

		portRgn &= refreshRect;

		if (!portRgn.IsEmpty()) {
			InvalPortRgn(portRgn);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DontRefresh
// ---------------------------------------------------------------------------
//	Validate the area occupied by a Pane.
//
//	This removes the Pane area from the update region so that the Pane
//	won't be redrawn during the next Update event.
//
//	The inOKIfHidden parameter specifies whether to validate even if
//	the Pane is not visible. You will need to pass true for this
//	parameter in order to suppress the automatic refresh which occurs
//	after hiding a Pane.

void
LPane::DontRefresh(
	Boolean	inOKIfHidden)
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect, inOKIfHidden)) {
		ValidPortRect(&refreshRect);
	}
}


// ---------------------------------------------------------------------------
//	¥ DontRefreshRect
// ---------------------------------------------------------------------------
//	Validate a rectangular portion of a Pane.

void
LPane::DontRefreshRect(
	const Rect&		inRect,				// In Local coords
	Boolean			inOKIfHidden)
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect, inOKIfHidden)) {

		Rect	portRect = inRect;		// Convert input rect to Port coords
		LocalToPortPoint(topLeft(portRect));
		LocalToPortPoint(botRight(portRect));

		if (::SectRect(&refreshRect, &portRect, &refreshRect)) {
			ValidPortRect(&refreshRect);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DontRefreshRgn
// ---------------------------------------------------------------------------
//	Validate a region of a Pane.

void
LPane::DontRefreshRgn(
	RgnHandle		inRegionH,			// In Local coords
	Boolean			inOKIfHidden)
{
	Rect	refreshRect;

	if (CalcPortExposedRect(refreshRect, inOKIfHidden)) {

										// Make copy of input region and
										//   convert to Port coords
		StRegion	portRgn = inRegionH;
		Point		offset  = Point_00;
		LocalToPortPoint(offset);
		::MacOffsetRgn(portRgn, offset.h, offset.v);

		portRgn &= refreshRect;

		if (!portRgn.IsEmpty()) {
			ValidPortRgn(portRgn);
		}
	}
}

#pragma mark -

// ===========================================================================
// ¥ Invalidate/Validate								 Invalidate/Validate ¥
// ===========================================================================
//	All rectangles and regions must be in Port coordinates
//
//	You should use these routines rather than the Toolbox traps InvalRect,
//	InvalRgn, ValidRect, and ValidRgn. Those traps require that the current
//	GrafPort be a Window. However, a Pane could be in another kind of
//	GrafPort, such as a Printer Port or GWorld, where calling one of those
//	traps would cause a horrible crash (when the Toolbox tries to access
//	a nonexistant update region).

void
LPane::InvalPortRect(
	const Rect	*inRect)
{
	if (mSuperView != nil) {
		mSuperView->InvalPortRect(inRect);
	}
}


void
LPane::InvalPortRgn(
	RgnHandle	inRgnH)
{
	if (mSuperView != nil) {
		mSuperView->InvalPortRgn(inRgnH);
	}
}


void
LPane::ValidPortRect(
	const Rect	*inRect)
{
	if (mSuperView != nil) {
		mSuperView->ValidPortRect(inRect);
	}
}


void
LPane::ValidPortRgn(
	RgnHandle	inRgnH)
{
	if (mSuperView != nil) {
		mSuperView->ValidPortRgn(inRgnH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FlushPortBuffer												  [public]
// ---------------------------------------------------------------------------
//	Flush Frame of Pane from buffer to screen
//
//	On some systems, Ports may be buffered. To get drawing to appear
//	immediately, you need to flush the buffer.

void
LPane::FlushPortBuffer() const
{
	GrafPtr	port = GetMacPort();
	
	if ( (port != nil)  &&
		 UDrawingUtils::IsPortBuffered(port) ) {
	
		Rect	frame;
		CalcPortFrameRect(frame);
		
		UDrawingUtils::FlushPortBuffer(port, frame);
	}
}


// ---------------------------------------------------------------------------
//	¥ UpdatePort
// ---------------------------------------------------------------------------
//	Redraw invalidated area of the Port containing the Pane
//
//		For Panes that are in a Window Port (i.e., the ultimate super view
//		is a Window), this forces an immediate redraw of the update region
//		of the Window. Since this message is really directed at the Port
//		containing the Pane (rather than the Pane itself), the update occurs
//		even if the Pane is not visible.
//
//		Panes that maintain a Mac GrafPort must override this function if
//		they support updating.

void
LPane::UpdatePort()
{
	if (mSuperView != nil) {
		mSuperView->UpdatePort();
	}
}


// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------
//	Prepare for drawing in the Pane
//
//	Signals if Pane has no SuperView.
//
//	Returns true if SuperView is focused
//
//	A Pane does not have its own coordinate system and clipping region.
//	It relies on its SuperView to set the focus. Even if FocusDraw
//	returns false, the Pane's GrafPort and clipping region of its
//	SuperView will be set (unless Pane has no SuperView)

Boolean
LPane::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean	focused = false;

	if (mSuperView != nil) {
		focused = mSuperView->FocusDraw(this);
	}

	return focused;
}


// ---------------------------------------------------------------------------
//	¥ FocusExposed
// ---------------------------------------------------------------------------
//	Prepare for drawing in the Pane
//
//	Returns whether any portion of the Pane is exposed.
//
//	A Pane is exposed when all the following conditions are true:
//		> Pane's "visible" flag is on
//		> Pane has a SuperView
//		> SuperView is exposed
//		> Pane's Frame intersects exposed area of its SuperView
//
//	Since a Pane does not have its own coordinate system, focusing a
//	Pane sets up the port, coordinate system, and clipping region of
//	its SuperView.
//
//	inAlwaysFocus specifies whether to focus the Pane even if it
//	is not exposed.
//
//	Pane is not focused if it is not exposed AND inAlwaysFocus is false.

Boolean
LPane::FocusExposed(
	Boolean	inAlwaysFocus)
{
	Rect	r;
	Boolean	exposed = CalcPortExposedRect(r);

	if (exposed || inAlwaysFocus) {
		exposed &= FocusDraw();		// Focus must succeed for Pane to
	}								//   be truly exposed

	return exposed;
}


// ---------------------------------------------------------------------------
//	¥ Draw
// ---------------------------------------------------------------------------
//	Try to draw contents of a Pane
//
//	inSuperDrawRgnH specifies, in Port coordinates, the portion of the
//	Pane's SuperView that needs to be drawn. Specify nil to bypass
//	the intersection test.
//
//	This is a wrapper function which calls DrawSelf if it is proper for
//	the Pane to draw. This means that:
//		> Pane is visible
//		> Pane's Frame is in QuickDraw space
//		> Pane's Frame intersects inSuperDrawRgnH
//		> Pane can be focused

void
LPane::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	try {
		Rect	exposedRect;
		
		if ( CalcPortExposedRect(exposedRect) &&
			 ((inSuperDrawRgnH == nil) || ::RectInRgn(&exposedRect, inSuperDrawRgnH)) &&
			 FocusDraw() ) {
			
			Rect	frame;
			CalcLocalFrameRect(frame);

			if ( (sBitDepth > 0)  ||
				 UDrawingUtils::IsPortBuffered(GetMacPort()) ) {

				if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
					DrawSelf();
				}

			} else {
				PortToLocalPoint(topLeft(exposedRect));
				PortToLocalPoint(botRight(exposedRect));
				
				StValueChanger<SInt16>	saveDepth(sBitDepth);
				StColorDrawLoop	deviceLoop(exposedRect);

				while (deviceLoop.NextDepth(sBitDepth)) {
					sDeviceHasColor = UDrawingUtils::DeviceSupportsColor(
												deviceLoop.GetCurrentDevice());

					if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
						DrawSelf();
					}
				}

			}
		}
	}

	catch (...) {
		SignalStringLiteral_("Exception caught in LPane::Draw");
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw contents of Pane
//
//	Derived classes must override this function in order to draw. Normally,
//	you will call CalcLocalFrameRect to get a QuickDraw rectangle defining
//	the size and location of the Pane. Perform all drawing operations
//	relative to the frame rectangle. For example:
//
//		::PenNormal();
//		ApplyForeAndBackColors();
//		Rect	frame;
//		CalcLocalFrameRect(frame);
//		::MacFrameRect(&frame);
//		::MoveTo(frame.left, frame.top);
//		::MacLineTo(frame.right, frame.bottom);
//
//			This draws a box around the Pane and a diagonal line from
//			the top left to the bottom right corner using the default
//			foreground and background colors for the Pane.
//
//	Clipping:
//	On entry, the clipping region is the revealed area of the Pane's
//	SuperView. Therefore, it is possible for a Pane to draw outside of
//	its Frame. You will not normally do this.
//
//	Usage Note: The port, coordinate system, and clipping region are
//		set on entry. They must be the same upon exit. You are responsible
//		for setting the Pen state and text characteristics to the proper
//		values for your Pane.

void
LPane::DrawSelf()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------

void
LPane::ApplyForeAndBackColors() const
{
	if (mSuperView != nil) {
		mSuperView->ApplyForeAndBackColors();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetForeAndBackColors
// ---------------------------------------------------------------------------

void
LPane::SetForeAndBackColors(
	const RGBColor*		inForeColor,
	const RGBColor*		inBackColor)
{
	if (mSuperView != nil) {
		mSuperView->SetForeAndBackColors(inForeColor, inBackColor);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors
// ---------------------------------------------------------------------------
//	Pass back the foreground and/or background colors of a Pane
//
//	Pass nil for outForeColor and/or outBackColor to not retrieve that color
//
//	Override for classes that set the colors.

void
LPane::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if (mSuperView != nil) {
		mSuperView->GetForeAndBackColors(outForeColor, outBackColor);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetEmbeddedTextColor
// ---------------------------------------------------------------------------
//	Pass back the color for drawing text within the Pane

void
LPane::GetEmbeddedTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive,
	RGBColor&	outColor) const
{
		// Let SuperView set text color. If no SuperView, we use
		// default colors.

	if (mSuperView != nil) {
		mSuperView->GetEmbeddedTextColor(inDepth, inHasColor,
											inIsActive, outColor);

	} else {					// No SuperView
		outColor = Color_Black;
	
		if (not inIsActive and (inDepth >= 4) and inHasColor) {
								// Inactive on color device
			UTextDrawing::DimTextColor(outColor);
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CountPanels
// ---------------------------------------------------------------------------
//	Return the number of horizontal and vertical Panels. A Panel is a
//	"frameful" of a Pane.

void
LPane::CountPanels(
	UInt32	&outHorizPanels,
	UInt32	&outVertPanels)
{
	outHorizPanels = 1;					// A Pane is the same size as its
	outVertPanels  = 1;					//   Frame, so there's only one Panel
}


// ---------------------------------------------------------------------------
//	¥ ScrollToPanel
// ---------------------------------------------------------------------------
//	Scroll Pane to the specified Panel
//
//	Panes do not scroll, so just return true indicating that the Panel
//	is always valid

Boolean
LPane::ScrollToPanel(
	const PanelSpec&	/* inPanel */)
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ PrintPanel
// ---------------------------------------------------------------------------
//	Try to Print a Panel of a Pane
//
//	Since a Pane does not scroll, it just prints itself for every panel
//
//	inSuperPrintRgnH specifies, in Port coordinates, the portion of the
//	Pane's SuperView that needs to be printed. Specify nil to bypass
//	the intersection test.
//
//	This is a wrapper function which calls PrintPanelSelf if it is proper for
//	the Pane to print. This means that:
//		> Pane is visible
//		> Pane's Frame is in QuickDraw space
//		> Pane's Frame intersects inSuperDrawRgnH
//		> Pane can be focused

void
LPane::PrintPanel(
	const PanelSpec	&inPanel,
	RgnHandle		inSuperPrintRgnH)
{
	try {
		Rect	frame;
		if ( (mVisible == triState_On)  &&
			 CalcPortFrameRect(frame)  &&
			 ((inSuperPrintRgnH == nil) ||
			 	::RectInRgn(&frame, inSuperPrintRgnH)) &&
			 FocusDraw() ) {

			PortToLocalPoint(topLeft(frame));	// Get Frame in Local coords
			PortToLocalPoint(botRight(frame));

			if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
				PrintPanelSelf(inPanel);
			}
		}
	}

	catch (...) {
		SignalStringLiteral_("Exception caught in LPane::PrintPanel");
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperPrintPanel
// ---------------------------------------------------------------------------
//	SuperView is printing a panel

void
LPane::SuperPrintPanel(
	const PanelSpec	&inSuperPanel,
	RgnHandle	inSuperPrintRgnH)
{
	PrintPanel(inSuperPanel, inSuperPrintRgnH);
}


// ---------------------------------------------------------------------------
//	¥ PrintPanelSelf
// ---------------------------------------------------------------------------
//	Print a Panel of a Pane

void
LPane::PrintPanelSelf(
	const PanelSpec&	/* inPanel */)
{
	DrawSelf();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PortToLocalPoint
// ---------------------------------------------------------------------------
//	Convert point from Port to Local coordinates

void
LPane::PortToLocalPoint(
	Point	&ioPoint) const
{
	if (mSuperView != nil) {
		mSuperView->PortToLocalPoint(ioPoint);
	}
}


// ---------------------------------------------------------------------------
//	¥ LocalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Local to Port coordinates

void
LPane::LocalToPortPoint(
	Point	&ioPoint) const
{
	if (mSuperView != nil) {
		mSuperView->LocalToPortPoint(ioPoint);
	}
}


// ---------------------------------------------------------------------------
//	¥ GlobalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Global to Port coordinates

void
LPane::GlobalToPortPoint(
	Point	&ioPoint) const
{
	if (mSuperView != nil) {
		mSuperView->GlobalToPortPoint(ioPoint);
	}
}


// ---------------------------------------------------------------------------
//	¥ GlobalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Port to Global coordinates

void
LPane::PortToGlobalPoint(
	Point	&ioPoint) const
{
	if (mSuperView != nil) {
		mSuperView->PortToGlobalPoint(ioPoint);
	}
}

#pragma mark -

// ===========================================================================
//	¥ StFocusAndClipIfHidden
// ===========================================================================
//	Stack-based class for focusing a Pane that may be hidden
//
//	Constructor tries to Focus the Pane. If the Pane is hidden (because it
//	has no SuperView, is invisible, or is scrolled out of view), the
//	clip region is set to an empty region to prevent drawing.
//
//	If Pane has no SuperView, caller is responsible for appropriately
//	setting the current Port beforehand.
//
//	The Destructor restores the original clip region if necessary.

StFocusAndClipIfHidden::StFocusAndClipIfHidden(
	LPane*	inPane)
{
	mClipRgn = nil;

	Boolean	exposed = false;
	if ( (inPane->GetSuperView() != nil) &&
		 inPane->GetSuperView()->EstablishPort() ) {
		 
		exposed = inPane->FocusExposed(true);
	}

	if (not exposed) {						// Make an empty clip region
		mClipRgn = ::NewRgn();
		if (mClipRgn != nil) {
			::GetClip(mClipRgn);
			::ClipRect(&Rect_0000);
		}
	}
}


StFocusAndClipIfHidden::~StFocusAndClipIfHidden()
{
	if (mClipRgn != nil) {					// Restore original clip region
		::SetClip(mClipRgn);
		::DisposeRgn(mClipRgn);
	}
}

#pragma mark -

// ===========================================================================
//	¥ StPaneVisibleState
// ===========================================================================
//
//	Stack-based class for temporarily changing a Pane's visible state
//
//	Warning: Changing the visible state directly will not actually show
//	or hide the Pane and may put the Pane in an inconsistent state.

StPaneVisibleState::StPaneVisibleState(
	LPane*		inPane,
	ETriState	inState)
{
	mPane		= inPane;
	mSaveState	= triState_Off;

	if (inPane != nil) {
		mSaveState  = inPane->GetVisibleState();
		inPane->SetVisibleState(inState);
	}
}


StPaneVisibleState::~StPaneVisibleState()
{
	if (mPane != nil) {
		mPane->SetVisibleState(mSaveState);
	}
}

PP_End_Namespace_PowerPlant
