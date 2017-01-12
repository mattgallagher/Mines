// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdScrollBarImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implementation for the standard ScrollBar control (CDEF 1) which
//	supports live tracking and 32-bit values

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStdScrollBarImp.h>
#include <LScrollBar.h>
#include <LStream.h>
#include <LView.h>
#include <UControlMgr.h>
#include <UDrawingState.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LStdScrollBarImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LStdScrollBarImp::LStdScrollBarImp(
	LStream*	inStream)

	: LStdControlImp(inStream)
{
	mUsingBigValues = false;
	mLiveFeedback   = false;
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LStdScrollBarImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	LStdControlImp::Init(inControlPane, inControlKind, inTitle, inTextTraitsID,
						inRefCon);

									// Store pointer to this object in refCon
									//   so callback functions can access it
	::SetControlReference(mMacControlH, (long) this);

									// May have to scale from 32 to 16 bits
	mUsingBigValues = ApplySmallSettings( mControlPane->PeekValue(),
										  mControlPane->GetMinValue(),
										  mControlPane->GetMaxValue() );

									// Check for live feedback variation
	mLiveFeedback = (inControlKind == kControlScrollBarLiveProc);
}


// ---------------------------------------------------------------------------
//	¥ ~LStdScrollBarImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LStdScrollBarImp::~LStdScrollBarImp()
{
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LStdScrollBarImp::SetValue(
	SInt32		inValue)
{
	SInt16	ctlValue = (SInt16) inValue; // Scale to 32-bit value if necessary
	if (mUsingBigValues) {
		ctlValue = CalcSmallValue( inValue,
								   mControlPane->GetMinValue(),
								   mControlPane->GetMaxValue() );
	}

	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlValue(mMacControlH, ctlValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LStdScrollBarImp::SetMinValue(
	SInt32		inMinValue)
{
									// Scale to 16-bit values if necessary
	mUsingBigValues =
			ApplySmallSettings( mControlPane->PeekValue(),
								inMinValue,
								mControlPane->GetMaxValue() );
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LStdScrollBarImp::SetMaxValue(
	SInt32		inMaxValue)
{
									// Scale to 16-bit values if necessary
	mUsingBigValues =
			ApplySmallSettings( mControlPane->PeekValue(),
							    mControlPane->GetMinValue(),
							    inMaxValue );
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LStdScrollBarImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		/* inModifiers */)
{
	if ((inHotSpot >= kControlIndicatorPart) && mLiveFeedback) {
		TrackLiveThumb(inPoint);		// Special case for live tracking
		return false;
	}
	
		// Allocate UPP for TrackAction callback as a local static variable.
		// Carbon, CFM, and 68K have different ways of creating UPPs.
		
#if TARGET_API_MAC_CARBON

	static StControlActionUPP	sTrackActionUPP(LStdScrollBarImp::TrackAction);

#else

	#if TARGET_RT_MAC_CFM		// For CFM, a UPP is a pointer to a
								//   RoutineDescriptor struct. We use
								//   statics for storage.

		static RoutineDescriptor	sTrackActionDesc =
				BUILD_ROUTINE_DESCRIPTOR( uppControlActionProcInfo,
										  LStdScrollBarImp::TrackAction );

		static ControlActionUPP		sTrackActionUPP = &sTrackActionDesc;

	#else						// For non-CFM, a UPP is just a function
								//   pointer
		static ControlActionUPP		sTrackActionUPP = LStdScrollBarImp::TrackAction;
	#endif

#endif
										// ActionProc gets called while
										//   holding down the mouse
	ControlActionUPP	actionProc = sTrackActionUPP;
	if (inHotSpot >= kControlIndicatorPart) {
		actionProc = nil;				// No action for non-live thumb
	}
										// Save current value so we can
										//   see if it changed while tracking
	SInt16	origValue = ::GetControlValue(mMacControlH);

		// TrackControl handles tracking and returns 0 if the mouse is
		// released outside the HotSpot

	Boolean	releasedInHotSpot =
		::TrackControl(mMacControlH, inPoint, actionProc) != kControlNoPart;

	mControlPane->FocusDraw();

	if (actionProc == nil) {		// End of no feedback thumb tracking

			// After dragging the thumb without live feedback, the
			// Control Manager sets the ScrollBar value based on the
			// final thumb position. We need to synch the ControlPane's
			// value with the ScrollBar's value (if it changed).

		SInt32	currValue = ::GetControlValue(mMacControlH);
		if (currValue != origValue) {
			SInt32	newValue = currValue;
			if (mUsingBigValues) {
				newValue = CalcBigValue((SInt16) currValue);
			}
			mControlPane->SetValue(newValue);
		}
	}

	return releasedInHotSpot;
}


// ---------------------------------------------------------------------------
//	¥ TrackLiveThumb												  [public]
// ---------------------------------------------------------------------------

void
LStdScrollBarImp::TrackLiveThumb(
	Point	inPoint)
{
	SDimension16	frameSize;				// ScrollBar is horizontal if it
	mControlPane->GetFrameSize(frameSize);	//   is more wide than tall
	bool	isHorizontal = frameSize.width > frameSize.height;

	SInt16	startClick = inPoint.v;			// Save starting location of click
	if (isHorizontal) {						// Only need the coord that's in
		startClick = inPoint.h;				//   the direction of the scroll
	}

		// To implement smooth dragging, we must ensure that the
		// control's value is different for each pixel location of the
		// thumb. We do this by determining the pixel location of the
		// edge of the thumb when the control is at its min and max
		// values. The difference represents the entire range of movement
		// of the thumb. By setting the min and max to these pixel values,
		// we set up a one-to-one correspondence between the control's value
		// and the pixel location of the thumb.

	SInt16	min   = ::GetControlMinimum(mMacControlH);	// Save current values
	SInt16	max   = ::GetControlMaximum(mMacControlH);

										// Determine thumb pixel location
										//   at its current, min, and max
										//   values
	SInt16	startThumb, minThumb, maxThumb;

	{
		StClipRgnState	emptyClip(nil);	// Prevent CDEF from drawing while
										//   we fiddle with the value

										// Current location of thumb
		startThumb = CalcThumbEdge(isHorizontal);

										// Thumb location when value = min
		::SetControlValue(mMacControlH, min);
		minThumb = CalcThumbEdge(isHorizontal);
										// Thumb location when value = max
		::SetControlValue(mMacControlH, max);
		maxThumb = CalcThumbEdge(isHorizontal);

										// Set artificial values
		::SetControlMinimum(mMacControlH, minThumb);
		::SetControlMaximum(mMacControlH, maxThumb);
		::SetControlValue(mMacControlH, startThumb);
	}

		// Ask CDEF for the constraints for dragging the thumb.
		// We need the slopRect, which specifies the area in which
		// to track the mouse. If the mouse leaves the slopRect, the
		// thumb stops following the mouse and snaps back to its
		// original location.

	IndicatorDragConstraint	dragParams;
	topLeft(dragParams.limitRect) = inPoint;

	UControlMgr::SendControlMessage(mMacControlH, thumbCntl, &dragParams);

	SInt32	vValue;
	SInt32	vMin = mControlPane->GetMinValue();
	SInt32	vMax = mControlPane->GetMaxValue();

	while (::StillDown()) {				// Start our mouse tracking loop

			// Determine the thumb value based on how far the current
			// mouse location is from where the click occurred

		Point currentPoint;
		::GetMouse(&currentPoint);

		SInt16	thumbValue = startThumb;

		if (::MacPtInRect(currentPoint, &dragParams.slopRect)) {
			if (isHorizontal) {
				thumbValue += currentPoint.h - startClick;
			} else {
				thumbValue += currentPoint.v - startClick;
			}

			if (thumbValue < minThumb) {	// Enfore min and max limits
				thumbValue = minThumb;
			} else if (thumbValue > maxThumb) {
				thumbValue = maxThumb;
			}
		}								// Settting control value will position
										//   the thumb
		::SetControlValue(mMacControlH, thumbValue);

			// Use the thumb values to calculate the "real" value
			//
			//      v - vMax        t - tMin
			//    -----------  =  -----------
			//    vMax - vMin     tMax - tMin

		vValue = vMin + (vMax - vMin) * (thumbValue - minThumb) /
								(maxThumb - minThumb);

										// Inform ScollBar Pane
		mControlPane->DoTrackAction(kControlIndicatorPart, vValue);

		mControlPane->FocusDraw();		// DoTrackAction() may change the focus
										//   so we must reset it
	}

	// Now we must undo all the funky stuff we did to get
	// the scroll bar to perform live tracking

	{
		StClipRgnState	emptyClip(nil);	// Suppress drawing while resetting
										//   the control's min and max
		ApplySmallSettings(vValue, vMin, vMax);
	}

	SetValue( mControlPane->GetValue() );
}


// ---------------------------------------------------------------------------
//	¥ CalcThumbEdge
// ---------------------------------------------------------------------------
//	Pass back the pixel location of the edge (left or top depending on
//	orientation) of the thumb

SInt16
LStdScrollBarImp::CalcThumbEdge(
	bool	inHorizontal) const
{
	StRegion	thumbRgn;
									// Get current thumb location
	UControlMgr::SendControlMessage(mMacControlH, calcThumbRgn,
										RgnHandle(thumbRgn));

	Rect	thumbRect;
	thumbRgn.GetBounds(thumbRect);

	SInt16	edge = thumbRect.top;
	if (inHorizontal) {
		edge = thumbRect.left;
	}

	return edge;
}


// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------
//	Member function for handling callbacks while tracking non-thumb parts

void
LStdScrollBarImp::DoTrackAction(
	SInt16		inHotSpot)
{
									// Scale to 32-bit value if necessary
	SInt32	value = ::GetControlValue(mMacControlH);
	if (mUsingBigValues) {
		value = CalcBigValue((SInt16) value);
	}
									// ControlPane handles tracking action
	mControlPane->DoTrackAction(inHotSpot, value);

	mControlPane->FocusDraw();		// Action could have changed focus
}


// ---------------------------------------------------------------------------
//	¥ TrackAction										  [static] [protected]
// ---------------------------------------------------------------------------
//	ControlManager ActionProc callback function for non-thumb parts

pascal void
LStdScrollBarImp::TrackAction(
	ControlHandle	inMacControlH,
	SInt16			inPart)
{
	try {
		LStdScrollBarImp	*imp =
					(LStdScrollBarImp*) ::GetControlReference(inMacControlH);
		imp->DoTrackAction(inPart);
	}

	catch (...) { }			// Don't propagate exception into Toolbox!
}


PP_End_Namespace_PowerPlant
