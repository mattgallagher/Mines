// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdControlImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStdControlImp.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <UTextTraits.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LStdControlImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LStdControlImp::LStdControlImp(
	LStream*	inStream)

	: LControlImp(inStream)
{
	mMacControlH  = nil;
	mControlKind  = -1;
	mTextTraitsID = 0;
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	ResIDT	textTraitsID;
	Str255	title;

	*inStream >> controlKind;			// Read parameters from Stream
	*inStream >> textTraitsID;
	inStream->ReadPString(title);

										// Call parameterized Init()
	Init(inControlPane, controlKind, title, textTraitsID, 0);
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	mControlPane  = inControlPane;
	mControlKind  = inControlKind;
	mTextTraitsID = inTextTraitsID;

	MakeMacControl(inTitle, inRefCon);
}


// ---------------------------------------------------------------------------
//	¥ MakeMacControl											   [protected]
// ---------------------------------------------------------------------------

void
LStdControlImp::MakeMacControl(
	ConstStringPtr	inTitle,
	SInt32			inRefCon)
{
									// Mapping from AM CDEF to old CDEF IDs
	if ((mControlKind >> 4) == (kControlPopupButtonProc >> 4)) {
									// Popup Menu
		mControlKind += (popupMenuProc - kControlPopupButtonProc);

	} else if ((mControlKind >> 4) == (kControlPushButtonProc >> 4)) {
									// PushButton, CheckBox, RadioButton
		mControlKind -= kControlPushButtonProc;

	} else if ((mControlKind >> 4) == (kControlScrollBarProc >> 4)) {
									// Scroll Bar
		mControlKind = scrollBarProc;
	}

									// Set variant for using custom font
									//   if we have Text Traits
	SInt16	controlKind = mControlKind;
	if (mTextTraitsID != 0) {
		controlKind += kControlUsesOwningWindowsFontVariant;
	}

	Rect	frame;					// Get Frame in Local coordinates
	if (!CalcLocalFrameRect(frame)) {

			// Frame is outside of QuickDraw space. NewControl requires
			// a rectangle in QuickDraw space, so we have to use an
			// artificial location. The Control won't get drawn until
			// its SuperView scrolls it into view.

		SDimension16	frameSize;
		mControlPane->GetFrameSize(frameSize);

		::MacSetRect(&frame, 0, 0, frameSize.width, frameSize.height);
	}

			// Create Toolbox Control. During construction Panes are
			// always invisible and disabled, so we pass "false" for
			// the "visible" parameter to NewControl. Since NewControl
			// always creates enabled controls, we then explicitly
			// disable the control.

	StFocusAndClipIfHidden	focus(mControlPane);

	mMacControlH = ::NewControl(UQDGlobals::GetCurrentWindowPort(),
								&frame, inTitle, false,
								(SInt16) mControlPane->PeekValue(),
								(SInt16) mControlPane->GetMinValue(),
								(SInt16) mControlPane->GetMaxValue(),
								controlKind, inRefCon);

	ThrowIfNil_(mMacControlH);

	::HiliteControl(mMacControlH, 255);		// Deactivate Control
}


// ---------------------------------------------------------------------------
//	¥ ~LStdControlImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LStdControlImp::~LStdControlImp()
{
	if (mMacControlH != nil) {

			// DisposeControl() autumotically erases. To prevent this,
			// we hide the pen first.

		mControlPane->FocusDraw();
		::HidePen();
		::DisposeControl(mMacControlH);
		::ShowPen();
		mMacControlH = nil;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LStdControlImp::GetDescriptor(
	Str255		outDescriptor) const
{
	::GetControlTitle(mMacControlH, outDescriptor);
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlTitle(mMacControlH, inDescriptor);
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------

CFStringRef
LStdControlImp::CopyCFDescriptor() const
{
	CFStringRef	stringRef = nil;
	
	if (CFM_AddressIsResolved_(CopyControlTitleAsCFString)) {
		::CopyControlTitleAsCFString(mMacControlH, &stringRef);
		
	} else {
		Str255	descriptor;
		GetDescriptor(descriptor);
		
		stringRef = ::CFStringCreateWithPascalString(
										nil,
										descriptor,
										::CFStringGetSystemEncoding() );
	}
	
	return stringRef;
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetCFDescriptor(
	CFStringRef	inStringRef)
{
	if (CFM_AddressIsResolved_(SetControlTitleWithCFString)) {
		StFocusAndClipIfHidden	focus(mControlPane);
		::SetControlTitleWithCFString(mMacControlH, inStringRef);
		
	} else {
		Str255	descriptor;
		::CFStringGetPascalString( inStringRef,
								   descriptor,
								   sizeof(descriptor),
								   ::CFStringGetSystemEncoding() );
		SetDescriptor(descriptor);
	}
}

#endif

// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetValue(
	SInt32		inValue)
{
	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlValue(mMacControlH, (SInt16) inValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetMinValue(
	SInt32		inMinValue)
{
	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlMinimum(mMacControlH, (SInt16) inMinValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetMaxValue(
	SInt32		inMaxValue)
{
	StFocusAndClipIfHidden	focus(mControlPane);
	::SetControlMaximum(mMacControlH, (SInt16) inMaxValue);
}


// ---------------------------------------------------------------------------
//	¥ GetAllValues													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::GetAllValues(
	SInt32&		outValue,
	SInt32&		outMinValue,
	SInt32&		outMaxValue)
{
	outValue    = ::GetControlValue(mMacControlH);
	outMinValue = ::GetControlMinimum(mMacControlH);
	outMaxValue = ::GetControlMaximum(mMacControlH);
}


// ---------------------------------------------------------------------------
//	¥ GetMacControl													  [public]
// ---------------------------------------------------------------------------

ControlHandle
LStdControlImp::GetMacControl() const
{
	return mMacControlH;
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

ResIDT
LStdControlImp::GetTextTraitsID() const
{
	return mTextTraitsID;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LStdControlImp::FindHotSpot(
	Point		inPoint) const
{
	mControlPane->FocusDraw();
	return ::TestControl(mMacControlH, inPoint);
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot												  [public]
// ---------------------------------------------------------------------------

Boolean
LStdControlImp::PointInHotSpot(
	Point		inPoint,
	SInt16		inHotSpot) const
{
	return (inHotSpot == ::TestControl(mMacControlH, inPoint));
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LStdControlImp::TrackHotSpot(
	SInt16		/* inHotSpot */,
	Point		inPoint,
	SInt16		/* inModifiers */)
{
		// TrackControl handles tracking and returns kControlNoPart
		// if the mouse is released outside the HotSpot

	SInt16	origValue = ::GetControlValue(mMacControlH);

	Boolean	releasedInHotSpot =
		::TrackControl(mMacControlH, inPoint, (ControlActionUPP) (-1))
				!= kControlNoPart;

		// Control Manager can change the value while tracking.
		// If it did, we need to call SetValue() to update the
		// class's copy of the value.

	SInt16	currValue = ::GetControlValue(mMacControlH);
	if (currValue != origValue) {
		mControlPane->SetValue(currValue);
	}

	return releasedInHotSpot;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction
// ---------------------------------------------------------------------------
//	Take action during mouse down tracking
//
//	inCurrInside tells whether the mouse is currently inside the HotSpot
//	inPrevInside tells whether the mouse was inside the HotSpot on the
//		previous call to this function
//
//	This function simulates the effect of tracking by hilighting the hot spot.

void
LStdControlImp::HotSpotAction(
	SInt16		inHotSpot,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
									// If in-out state has changed ...
	if (inCurrInside != inPrevInside) {
		mControlPane->FocusDraw();
									// If inside, hilite the hot spot,
									// If outside, unhilite the entire control
		::HiliteControl(mMacControlH, (SInt16) (inCurrInside ? inHotSpot : 0));
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusImp														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::FocusImp()
{
	::PenNormal();
	ApplyTextTraits();
	mControlPane->GetContainer()->ApplyForeAndBackColors();
}


// ---------------------------------------------------------------------------
//	¥ ApplyTextTraits											   [protected]
// ---------------------------------------------------------------------------

void
LStdControlImp::ApplyTextTraits()
{
	UTextTraits::SetPortTextTraits(mTextTraitsID);

		// If control's owner port is not the current port, we must
		// also set the text traits of the owner port. The control manager
		// retrieves font information from the owner port.

	if ( (mMacControlH != nil) &&
		 ::GetControlOwner(mMacControlH) != UQDGlobals::GetCurrentWindowPort()) {

		StGrafPortSaver		saveAndSetPort((GrafPtr) ::GetWindowPort(::GetControlOwner(mMacControlH)));
		UTextTraits::SetPortTextTraits(mTextTraitsID);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::DrawSelf()
{
		// The Control Manager does not properly draw a Control into
		// anything other than a Window. If the current port is not
		// the same port as that stored in the ControlRecord, then
		// we most likely are drawing the Control in a printer or
		// offscreen port.

	GrafPtr	currentPort = UQDGlobals::GetCurrentPort();

	if (currentPort == (GrafPtr) ::GetWindowPort( ::GetControlOwner(mMacControlH) )) {
		::Draw1Control(mMacControlH);	// Draw in the Window

	} else {							// Port is NOT the Control's Window

			// We must first switch the active port to the Control's
			// Window, set the clip region, then draw the Control
			// into a Picture. OpenPicture automatically suppresses
			// drawing to the screen.

		Rect	frame;
		CalcLocalFrameRect(frame);
		::SetPortWindowPort(::GetControlOwner(mMacControlH));
		::PenNormal();
		if (mTextTraitsID != 0) {
			UTextTraits::SetPortTextTraits(mTextTraitsID);
		}

		PicHandle	controlPict;
		{
			StClipRgnState	clip(frame);
			controlPict = ::OpenPicture(&frame);
			::Draw1Control(mMacControlH);
			::ClosePicture();
		}

			// Now we can switch back to the original port and
			// draw the Picture containing the Control.

		::MacSetPort(currentPort);
		::DrawPicture(controlPict, &frame);
		::KillPicture(controlPict);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ShowSelf														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::ShowSelf()
{
		// PP Panes don't draw immediately when showing; they wait
		// until the following update event. ShowControl() draws, so
		// we suppress drawing by making the clip region empty. We then
		// refresh to force an update event.

	mControlPane->FocusDraw();
	StClipRgnState	emptyClip(nil);
	::ShowControl(mMacControlH);
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ HideSelf														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::HideSelf()
{
		// HideControl() erases the control, which could cause an
		// ugly flash. So we suppress drawing by making the clip region
		// empty and refresh to force an update event where what's
		// underneath the control will draw itself.

	mControlPane->FocusDraw();
	StClipRgnState	emptyClip(nil);
	::HideControl(mMacControlH);
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::ActivateSelf()
{
										// Control must be Active and Enabled
										//   to be visually highlighted
	if (mControlPane->GetEnabledState() == triState_On) {
		mControlPane->FocusDraw();
		StClipRgnState	emptyClip(nil);
		::HiliteControl(mMacControlH, 0);
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::DeactivateSelf()
{
	if (::IsControlHilited(mMacControlH)) {
		mControlPane->FocusDraw();
		StClipRgnState	emptyClip(nil);
		::HiliteControl(mMacControlH, 255);
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::EnableSelf()
{
										// Control must be Active and Enabled
										//   to be visually highlighted
	if (mControlPane->GetActiveState() == triState_On) {
		mControlPane->FocusDraw();
		StClipRgnState	emptyClip(nil);
		::HiliteControl(mMacControlH, 0);
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::DisableSelf()
{
	DeactivateSelf();
}


// ---------------------------------------------------------------------------
//	¥ Hilite														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::Hilite(
	SInt16		inPartCode)
{
	StFocusAndClipIfHidden	focus(mControlPane);	// Control will draw
	::HiliteControl(mMacControlH, inPartCode);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::ResizeFrameBy(
	SInt16		/* inWidthDelta */,
	SInt16		/* inHeightDelta */)
{
	AlignControlRect();
}


// ---------------------------------------------------------------------------
//	¥ MoveBy														  [public]
// ---------------------------------------------------------------------------

void
LStdControlImp::MoveBy(
	SInt32		/* inHorizDelta */,
	SInt32		/* inVertDelta */)
{
	AlignControlRect();
}


// ---------------------------------------------------------------------------
//	¥ AlignControlRect											   [protected]
// ---------------------------------------------------------------------------
//	Align bounds of Toolbox control to Pane's Frame

void
LStdControlImp::AlignControlRect()
{
	if (mMacControlH != nil) {
		Rect	frame;
		if (!CalcLocalFrameRect(frame)) {

				// Frame is outside QD Space. Put Mac ControlRect at the
				// upper left limit of QD Space (extreme negative coords).
				// That location is guaranteed to be offscreen (unless
				// you have a control longer than 32K pixels) since PP
				// Image coordinates start at (0,0) and are never negative.

			SDimension16	frameSize;
			mControlPane->GetFrameSize(frameSize);

			frame.left   = min_Int16;
			frame.right  = (SInt16) (frame.left + frameSize.width);
			frame.top    = min_Int16;
			frame.bottom = (SInt16) (frame.top + frameSize.height);
		}

		AdjustControlBounds(frame);

		::SetControlBounds(mMacControlH, &frame);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustControlBounds										   [protected]
// ---------------------------------------------------------------------------
//	Change bounds if control rect is not the same size as the Frame
//	of the ControlPane
//
//	On input ioBounds is the same as the Frame of the ControlPane

void
LStdControlImp::AdjustControlBounds(
	Rect&	/* ioBounds */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ValueIsInStdRange										 [static] [public]
// ---------------------------------------------------------------------------
//	Return whether a value is within the range supported by the Mac
//	Control Manager

Boolean
LStdControlImp::ValueIsInStdRange(
	SInt32	inValue)
{
	return ((inValue >= min_Int16) && (inValue <= max_Int16));
}


// ---------------------------------------------------------------------------
//	¥ CalcSmallValue
// ---------------------------------------------------------------------------
//	Map from Big to Small value when using 32 bit values
//
//	Call this function only when mUsingBigValues is true. Control class
//	uses Big (32 bit) values, but the Mac Control Manager only supports
//	Small (16 bit) values. See the discussion for the SetStdMinAndMax
//	function.

SInt16
LStdControlImp::CalcSmallValue(
	SInt32	inBigValue,
	SInt32	inBigMinimum,
	SInt32	inBigMaximum)
{
									// Assume small range, where Big value
									//   is how far Big value is from its
									//   minimum value
	SInt16	smallValue = (SInt16) (inBigValue - inBigMinimum);

	SInt32	range = inBigMaximum - inBigMinimum;

	if (range > max_Int16) {		// Big range, need to calculate value

									// Special case min and max to avoid
									//   rounding errors at the boundaries
		if (inBigValue <= inBigMinimum) {
			smallValue = ::GetControlMinimum(mMacControlH);

		} else if (inBigValue >= inBigMaximum) {
			smallValue = ::GetControlMaximum(mMacControlH);

		} else {					//   Scale factor is the ratio of
									//   their ranges
			double	ratio = (double) max_Int16 / range;
			smallValue = (SInt16) ((inBigValue - inBigMinimum) * ratio);
		}
	}

	return smallValue;
}


// ---------------------------------------------------------------------------
//	¥ CalcBigValue
// ---------------------------------------------------------------------------
//	Map from Small to Big value when using 32 bit values
//
//	Call this function only when mUsingBigValues is true. Control class
//	uses Big (32 bit) values, but the Mac Control Manager only supports
//	Small (16 bit) values. See the discussion for the SetStdMinAndMax
//	function.

SInt32
LStdControlImp::CalcBigValue(
	SInt16	inSmallValue)
{
	SInt32	minValue = mControlPane->GetMinValue();
	SInt32	maxValue = mControlPane->GetMaxValue();

									// Assume small range, where value
									//   is how far Big value is from
									//   its minimum value
	SInt32	bigValue = minValue + inSmallValue;

	SInt32	range = maxValue - minValue;

	if (range > max_Int16) {		// Big range, need to calculate value

									// Special case min and max to avoid
									//   rounding errors at boundaries
		if (inSmallValue <= ::GetControlMinimum(mMacControlH)) {
			bigValue = minValue;

		} else if (inSmallValue >= ::GetControlMaximum(mMacControlH)) {
			bigValue = maxValue;

		} else {					//   Scale factor is the ratio of
									//   the big and small ranges
			double	ratio = (double) range / max_Int16;
			bigValue = (SInt32) (minValue + inSmallValue * ratio);
		}
	}

	return bigValue;
}


// ---------------------------------------------------------------------------
//	¥ SetSmallMinAndMax
// ---------------------------------------------------------------------------
//	Sets the minimum and maximum values for the standard Mac control
//	associated with a StdControl object
//
//	The Control class uses Big (32 bit) values, but the Mac Control Manager
//	only supports Small (16 bit) values. So this StdControl class has
//	to map between Big and Small values.
//
//	Using Big Values:
//	When either the minimum or maximum value is outside 16 bit range
//	(below -32,768 or above 32,767), we set mUsingBigValues to true.
//	In that case, you must call CalcSmallValue and CalcBigValue to
//	map between the Small values used by the Mac Control Manager and the
//	Big values used by this class.
//
//	Mapping Strategy:
//	When using big values, we always set the minimum value for the
//	Mac Control to zero. There are two cases for the maximum value,
//	depending on whether the difference between the Big Max and Min
//	is greater than or less than 32,767. If greater, we set the Mac
//	Control maximum to 32,767. If less, we set the Mac Control maximum
//	to that difference. These choices simplify the math for converting
//	between Big and Small values.
//
//	Here are the mapping equations for the two cases:
//
//	(1) BigMax - BigMin > 32,767
//
//		SmallValue      BigValue - BigMin
//		-----------  =  ------------------
//		   32,767       (BigMax - BigMin)
//
//	(2) BigMax - BigMin <= 32,767
//
//		SmallValue = BigValue - BigMin
//
//	The functions CalcSmallValue and CalcBigValue use the above equations
//	to convert between Big and Small values.

bool
LStdControlImp::ApplySmallSettings(
	SInt32	inValue,
	SInt32	inMinValue,
	SInt32	inMaxValue)
{
									// First assume 16-bit values
	SInt16	smallValue	= (SInt16) inValue;
	SInt16	smallMin	= (SInt16) inMinValue;
	SInt16	smallMax	= (SInt16) inMaxValue;

	bool	usingBigValues = !ValueIsInStdRange(inMinValue) ||
		 					 !ValueIsInStdRange(inMaxValue);

	if (usingBigValues) {			// Values aren't 16-bit, so we need
									//   to scale them

		smallMin = 0;				// Min is always zero for big values

									// Check difference between Max and Min
									//   If difference is in 16 bit range,
									//   set Max to the difference
		SInt32	max = inMaxValue - inMinValue;
		if (max > max_Int16) {		//   If difference is out of 16 bit range,
			max = max_Int16;		//   set Max to max_Int16
		}
		smallMax = (SInt16) max;

		smallValue = CalcSmallValue(inValue, inMinValue, inMaxValue);
	}
									// Set Toolbox values

	StFocusAndClipIfHidden	focus(mControlPane);	// Control Mgr might draw

	::SetControlMinimum (mMacControlH, smallMin);
	::SetControlMaximum (mMacControlH, smallMax);
	::SetControlValue	(mMacControlH, smallValue);

	return usingBigValues;
}


PP_End_Namespace_PowerPlant
