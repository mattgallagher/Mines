// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdControl.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for standard Mac Controls

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStdControl.h>
#include <LView.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UTextTraits.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <UMemoryMgr.h>
#include <UTBAccessors.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

LStdControl* LStdControl::sTrackingControl = nil;


// ---------------------------------------------------------------------------
//	¥ CreateFromCNTL										 [static] [public]
// ---------------------------------------------------------------------------
//	Create a StdControl from a CNTL resource

LStdControl*
LStdControl::CreateFromCNTL(
	ResIDT		inCNTLid,
	MessageT	inValueMessage,
	ResIDT		inTextTraitsID,
	LView		*inSuperView)
{
	LStdControl	*theStdControl = nil;

	::HidePen();
	inSuperView->FocusDraw();

	if (inTextTraitsID != 0) {		// Control does not use System font
		UTextTraits::SetPortTextTraits(inTextTraitsID);
	}

	ControlHandle	macControlH = ::GetNewControl(inCNTLid,
												inSuperView->GetMacWindow());
	::ShowPen();
	ThrowIfNil_(macControlH);

	SCNTLResource	*resP = *(SCNTLResourceH) ::GetResource(kControlTemplateResourceType, inCNTLid);

	SPaneInfo	thePaneInfo;
	thePaneInfo.paneID	= inCNTLid;
	thePaneInfo.left	= resP->bounds.left;
	thePaneInfo.top		= resP->bounds.top;
	thePaneInfo.width	= (SInt16) (resP->bounds.right - thePaneInfo.left);
	thePaneInfo.height	= (SInt16) (resP->bounds.bottom - thePaneInfo.top);
	thePaneInfo.visible = (resP->visible != 0);
	thePaneInfo.enabled = true;

	thePaneInfo.bindings.left =
		thePaneInfo.bindings.top =
		thePaneInfo.bindings.right =
		thePaneInfo.bindings.bottom = false;

	thePaneInfo.userCon		= 0;
	thePaneInfo.superView	= inSuperView;

									// Mask off useWFont variation code
	SInt16	controlKind = (SInt16)
			(resP->procID & ~((UInt16) kControlUsesOwningWindowsFontVariant));
	switch (controlKind) {

		case pushButProc:
			theStdControl = new LStdButton(thePaneInfo, inValueMessage,
									inTextTraitsID, macControlH);
			break;

		case checkBoxProc:
			theStdControl = new LStdCheckBox(thePaneInfo, inValueMessage,
									resP->value, inTextTraitsID, macControlH);
			break;

		case radioButProc:
			theStdControl = new LStdRadioButton(thePaneInfo, inValueMessage,
									resP->value, inTextTraitsID, macControlH);
			break;

		case popupMenuProc:
			theStdControl = new LStdPopupMenu(thePaneInfo, inValueMessage,
									::GetControlMaximum(macControlH),
									inTextTraitsID, macControlH);
			break;

		default:
			theStdControl = new LStdControl(thePaneInfo, inValueMessage,
									::GetControlValue(macControlH),
									::GetControlMinimum(macControlH),
									::GetControlMaximum(macControlH),
									controlKind, inTextTraitsID,
									macControlH);
			break;
	}

	return theStdControl;
}


// ---------------------------------------------------------------------------
//	¥ LStdControl								Default Constructor	  [public]
// ---------------------------------------------------------------------------

LStdControl::LStdControl()
{
	mMacControlH = nil;
	mControlKind = -1;				// Illegal control kind
	mThumbFunc = nil;
	mTextTraitsID = 0;
	mUsingBigValues = false;
}


// ---------------------------------------------------------------------------
//	¥ LStdControl								Copy Constructor	  [public]
// ---------------------------------------------------------------------------
//	On entry, the current Port must be Window into which to install
//	the Control.

LStdControl::LStdControl(
	const LStdControl	&inOriginal)

	: LControl(inOriginal)
{
	if (inOriginal.mMacControlH != nil) {
		Str255	title;
		::GetControlTitle(inOriginal.mMacControlH, title);

		InitStdControl(	inOriginal.mControlKind,
						inOriginal.mTextTraitsID,
						title,
						::GetControlReference(inOriginal.mMacControlH) );

		::SetControlAction(mMacControlH,
							::GetControlAction(inOriginal.mMacControlH));

	} else {
		mMacControlH = nil;
		mControlKind  = inOriginal.mControlKind;
		mTextTraitsID = inOriginal.mTextTraitsID;
		mUsingBigValues = inOriginal.mUsingBigValues;
	}

	mThumbFunc = inOriginal.mThumbFunc;
}


// ---------------------------------------------------------------------------
//	¥ LStdControl(SInt16)
// ---------------------------------------------------------------------------
//	Construct a StdControl for a particular kind of Toolbox Control
//
//	NOTE: On entry, the current Port must be Window into which to
//	install the Control.

LStdControl::LStdControl(
	SInt16	inControlKind)
{
	mThumbFunc = nil;
	mUsingBigValues = false;

	InitStdControl(inControlKind, 0, Str_Empty, 0);
}


// ---------------------------------------------------------------------------
//	¥ LStdControl							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LStdControl::LStdControl(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,
	SInt32			inMinValue,
	SInt32			inMaxValue,
	SInt16			inControlKind,
	ResIDT			inTextTraitsID,
	ConstStringPtr	inTitle,
	SInt32			inMacRefCon)
		: LControl(inPaneInfo, inValueMessage, inValue,
						inMinValue, inMaxValue)
{
	mThumbFunc = nil;
	mUsingBigValues = false;

	InitStdControl(inControlKind, inTextTraitsID, inTitle, inMacRefCon);
}


// ---------------------------------------------------------------------------
//	¥ LStdControl
// ---------------------------------------------------------------------------
//	Construct from input parameters and an existing ControlHandle

LStdControl::LStdControl(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,
	SInt32			inMinValue,
	SInt32			inMaxValue,
	SInt16			inControlKind,
	ResIDT			inTextTraitsID,
	ControlHandle	inMacControlH)
		: LControl(inPaneInfo, inValueMessage, inValue,
						inMinValue, inMaxValue)
{
	mMacControlH = inMacControlH;
	mThumbFunc = nil;
	mControlKind = inControlKind;
	mTextTraitsID = inTextTraitsID;
	mUsingBigValues = false;
}


// ---------------------------------------------------------------------------
//	¥ LStdControl							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LStdControl::LStdControl(
	LStream	*inStream)
		: LControl(inStream)
{
	SInt16	controlKind;
	ResIDT	textTraitsID;
	Str255	title;
	SInt32	macRefCon;

	*inStream >> controlKind;
	*inStream >> textTraitsID;

	inStream->ReadPString(title);

	*inStream >> macRefCon;

	mThumbFunc = nil;
	mUsingBigValues = false;
	InitStdControl(controlKind, textTraitsID, title, macRefCon);
}


// ---------------------------------------------------------------------------
//	¥ InitStdControl							Private Initializer	 [private]
// ---------------------------------------------------------------------------
//	Private initializer which creates the Toolbox Control

void
LStdControl::InitStdControl(
	SInt16			inControlKind,
	ResIDT			inTextTraitsID,
	ConstStringPtr	inTitle,
	SInt32			inMacRefCon)
{
	mControlKind = inControlKind;	// Store ControlKind before changing
									//   var code (if using window font)
	mTextTraitsID = inTextTraitsID;

	if (inTextTraitsID != 0) {		// Control does not use System font
		inControlKind += kControlUsesOwningWindowsFontVariant;
	}

	Rect	frame;					// Get Frame in Local coordinates
	if (!CalcLocalFrameRect(frame)) {

			// Frame is outside of QuickDraw space. NewControl requires
			// a rectangle in QuickDraw space, so we have to use an
			// artificial location. The Control won't get drawn until
			// its SuperView scrolls it into view.

		frame.left = 0;
		frame.top = 0;
		frame.right = mFrameSize.width;
		frame.bottom = mFrameSize.height;
	}

			// Create Toolbox Control. During construction Panes are
			// always invisible and disabled, so we pass "false" for
			// the "visible" parameter to NewControl. Since NewControl
			// always creates enabled controls, we then explicitly
			// disable the control.

	StFocusAndClipIfHidden	focus(this);

	mMacControlH = ::NewControl(UQDGlobals::GetCurrentWindowPort(), &frame, inTitle,
								false, (SInt16) mValue, (SInt16) mMinValue, (SInt16) mMaxValue,
								inControlKind, inMacRefCon);
	ThrowIfNil_(mMacControlH);

	::HiliteControl(mMacControlH, 255);

			// Set Control value, minimum, and maximum, performing
			// any necessary scaling or shifting for 32-bit values.
			// Don't do this for PopupMenus, which use the values
			// for different purposes.

	if ((inControlKind < popupMenuProc) ||
		(inControlKind > popupMenuProc + 15)) {

		SetStdMinAndMax();
		SInt32	macValue = mValue;
		if (mUsingBigValues) {
			macValue = CalcSmallValue(macValue);
		}
		::SetControlValue(mMacControlH, (SInt16) macValue);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LStdControl								Destructor			  [public]
// ---------------------------------------------------------------------------

LStdControl::~LStdControl()
{
	if (mMacControlH != nil) {
		if (mSuperView != nil) {		// DisposeControl() autumotically
			FocusDraw();				//   erases. To prevent this, we
		}								//   hide the pen first.
		::HidePen();
		::DisposeControl(mMacControlH);
		::ShowPen();
		mMacControlH = nil;
	}
}

#pragma mark ...

// ---------------------------------------------------------------------------
//	¥ AlignControlRect
// ---------------------------------------------------------------------------
//	Set the rectangle inside the Toolbox ControlRecord to the Frame rectangle
//	of the StdControl.
//
//	At (almost) all times, the contrlRect field of the Toolbox ControlRecord
//	must be the same as the Frame rectangle of the StdControl object. We store
//	directly to the ControlRecord rather than using the SizeControl and
//	MoveControl traps because we want to bypass the automatic drawing
//	(and the need to set the port and coordinate system) performed by
//	those traps.
//
//	The contrlRect is not the same as the Frame rectangle if the latter is
//	outside the 16-bit QuickDraw coordinate space.

void
LStdControl::AlignControlRect()
{
	if (mMacControlH != nil) {
		Rect	frame;
		if (!CalcLocalFrameRect(frame)) {

				// Frame is outside QD Space. Put Mac ControlRect at the
				// upper left limit of QD Space (extreme negative coords).
				// That location is guaranteed to be offscreen (unless
				// you have a control longer than 32K pixels) since PP
				// Image coordinates start at (0,0) and are never negative.

			frame.left	 = min_Int16;
			frame.right  = (SInt16) (frame.left + mFrameSize.width);
			frame.top	 = min_Int16;
			frame.bottom = (SInt16) (frame.top + mFrameSize.height);
		}

		::SetControlBounds(mMacControlH, &frame);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Return the Descriptor, which is the Title, of a StdControl

StringPtr
LStdControl::GetDescriptor(
	Str255	outDescriptor) const
{
	::GetControlTitle(mMacControlH, outDescriptor);
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor
// ---------------------------------------------------------------------------
//	Set the Descriptor, which is the Title, of a StdControl

void
LStdControl::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	FocusDraw();
	::SetControlTitle(mMacControlH, inDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		Override to update the Mac ControlHandle

void
LStdControl::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
	AlignControlRect();
}


// ---------------------------------------------------------------------------
//	¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//		Override to update the Mac ControlHandle

void
LStdControl::MoveBy(
	SInt32		inLeftDelta,
	SInt32		inTopDelta,
	Boolean		inRefresh)
{
	LPane::MoveBy(inLeftDelta, inTopDelta, inRefresh);
	AlignControlRect();
}


// ---------------------------------------------------------------------------
//	¥ ValueIsInStdRange [static]
// ---------------------------------------------------------------------------
//	Return whether a value is within the range supported by the Mac
//	Control Manager

Boolean
LStdControl::ValueIsInStdRange(
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
LStdControl::CalcSmallValue(
	SInt32	inBigValue)
{
									// Assume small range, where Big value
									//   is how far Big value is from its
									//   minimum value
	SInt16	smallValue = (SInt16) (inBigValue - mMinValue);

	SInt32	range = mMaxValue - mMinValue;

	if (range > max_Int16) {		// Big range, need to calculate value

									// Special case min and max to avoid
									//   rounding errors at the boundaries
		if (inBigValue == mMinValue) {
			smallValue = ::GetControlMinimum(mMacControlH);

		} else if (inBigValue == mMaxValue) {
			smallValue = ::GetControlMaximum(mMacControlH);

		} else {					//   Scale factor is the ratio of
									//   their ranges
			double	ratio = (double) max_Int16 / range;
			smallValue = (SInt16) ((inBigValue - mMinValue) * ratio);
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
LStdControl::CalcBigValue(
	SInt16	inSmallValue)
{
									// Assume small range, where value
									//   is how far Big value is from
									//   its minimum value
	SInt32	bigValue = mMinValue + inSmallValue;

	SInt32	range = mMaxValue - mMinValue;

	if (range > max_Int16) {		// Big range, need to calculate value

									// Special case min and max to avoid
									//   rounding errors at boundaries
		if (inSmallValue == ::GetControlMinimum(mMacControlH)) {
			bigValue = mMinValue;

		} else if (inSmallValue == ::GetControlMaximum(mMacControlH)) {
			bigValue = mMaxValue;

		} else {					//   Scale factor is the ratio of
									//   the big and small ranges
			double	ratio = (double) range / max_Int16;
			bigValue = (SInt32) (mMinValue + inSmallValue * ratio);
		}
	}

	return bigValue;
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set the value of a Standard Control
//
//	Overrides inherited function to map from 32 bit numbers to the 16 bit
//	numbers supported by the Mac Control Manager

void
LStdControl::SetValue(
	SInt32	inValue)
{
	if (inValue < mMinValue) {		// Enforce min/max range
		inValue = mMinValue;
	} else if (inValue > mMaxValue) {
		inValue = mMaxValue;
	}

	if (inValue != mValue) {		// Do nothing if not changing value

		SInt16	ctlValue = (SInt16) inValue;
		if (mUsingBigValues) {		// Scale from 32 to 16 bit value
			ctlValue = CalcSmallValue(inValue);
		}

		{
			StFocusAndClipIfHidden	focus(this);
			::SetControlValue(mMacControlH, ctlValue);
		}
									// Let base class adjust value
		LControl::SetValue(inValue);
	}
}


// ---------------------------------------------------------------------------
//	¥ SynchValue
// ---------------------------------------------------------------------------

void
LStdControl::SynchValue()
{
	SInt16	ctlValue = (SInt16) mValue;
	if (mUsingBigValues) {
		ctlValue = CalcSmallValue(mValue);
	}

	{
		StFocusAndClipIfHidden	focus(this);
		if (ctlValue != ::GetControlValue(mMacControlH)) {
			::SetControlValue(mMacControlH, ctlValue);
		} else {
			::Draw1Control(mMacControlH);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetStdMinAndMax
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

void
LStdControl::SetStdMinAndMax()
{
									// First assume 16-bit values
	SInt16	smallValue	= (SInt16) mValue;
	SInt16	smallMin	= (SInt16) mMinValue;
	SInt16	smallMax	= (SInt16) mMaxValue;

									// Value is "big" if either the min or
									//   max is outside the 16-bit range
	mUsingBigValues = not ValueIsInStdRange(mMinValue) or
		 			  not ValueIsInStdRange(mMaxValue);

	if (mUsingBigValues) {			// Values aren't 16-bit, so we need
									//   to scale them

		smallMin = 0;				// Min is always zero for big values

									// Check difference between Max and Min
									//   If difference is in 16 bit range,
									//   set Max to the difference
		SInt32	max = mMaxValue - mMinValue;
		if (max > max_Int16) {		//   If difference is out of 16 bit range,
			max = max_Int16;		//   set Max to max_Int16
		}
		smallMax = (SInt16) max;

		smallValue = CalcSmallValue(mValue);
	}
									// Set Toolbox values

	StFocusAndClipIfHidden	focus(this);	// Control Mgr might draw

	::SetControlMinimum (mMacControlH, smallMin);
	::SetControlMaximum (mMacControlH, smallMax);
	::SetControlValue	(mMacControlH, smallValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LStdControl::SetMinValue(
	SInt32	inMinValue)
{
	if (inMinValue != mMinValue) {
		LControl::SetMinValue(inMinValue);
		SetStdMinAndMax();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LStdControl::SetMaxValue(
	SInt32	inMaxValue)
{
	if (inMaxValue != mMaxValue) {
		LControl::SetMaxValue(inMaxValue);
		SetStdMinAndMax();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LStdControl::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}

#pragma mark ...

// ---------------------------------------------------------------------------
//	¥ FocusDraw
// ---------------------------------------------------------------------------

Boolean
LStdControl::FocusDraw(
	LPane*	/* inSubPane */)
{
	Boolean	focused = LControl::FocusDraw();
	::PenNormal();
	if (mTextTraitsID != 0) {
		UTextTraits::SetPortTextTraits(mTextTraitsID);
	}
	ApplyForeAndBackColors();
	return focused;
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot
// ---------------------------------------------------------------------------

short
LStdControl::FindHotSpot(
	Point	inPoint) const
{
	return ::TestControl(mMacControlH, inPoint);
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot
// ---------------------------------------------------------------------------

Boolean
LStdControl::PointInHotSpot(
	Point 	inPoint,
	SInt16	inHotSpot) const
{
	return (inHotSpot == ::TestControl(mMacControlH, inPoint));
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot
// ---------------------------------------------------------------------------
//	Track the mouse while it is down after clicking in a Control HotSpot
//
//	Returns whether the mouse is released within the HotSpot

Boolean
LStdControl::TrackHotSpot(
	SInt16	inHotSpot,
	Point 	inPoint,
	SInt16	/* inModifiers */)
{
		// For some bizarre reason, the actionProc for a thumb (indicator)
		// has different parameters than that for other parts. This
		// class uses the actionProc in the ControlRecord for non-thumb
		// parts, and the member variable mThumbFunc for the thumb.
		// (ProcPtr)(-1) is a special flag that tells the ControlManager
		// to use the actionProc in the ControlRecord.

	ControlActionUPP	actionProc = (ControlActionUPP) (-1);
	if (inHotSpot >= kControlIndicatorPart) {
		actionProc = (ControlActionUPP) mThumbFunc;
		sTrackingControl = this;
	}

		// TrackControl handles tracking and returns kControlNoPart
		// if the mouse is released outside the HotSpot

	SInt16	origValue = ::GetControlValue(mMacControlH);
	Boolean	releasedInHotSpot =
		::TrackControl(mMacControlH, inPoint, actionProc) != kControlNoPart;

	sTrackingControl = nil;

		// Control Manager can change the value while tracking.
		// If it did, we need to call SetValue() to update the
		// class's copy of the value.

	SInt32	currValue = ::GetControlValue(mMacControlH);
	if (currValue != origValue) {
		SInt32	newValue = currValue;
		if (mUsingBigValues) {		// Scale from 16 to 32 bit value
			newValue = CalcBigValue((SInt16) currValue);
		}

		SetValue(newValue);
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
//	StdControls use Control Manager callbacks for tracking. This function
//	simulates the effect of tracking by hilighting the hot spot.

void
LStdControl::HotSpotAction(
	SInt16		inHotSpot,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
									// If in-out state has changed ...
	if (inCurrInside != inPrevInside) {
		FocusDraw();
									// If inside, hilite the hot spot,
									// If outside, unhilite the entire control
		::HiliteControl(mMacControlH, (SInt16) (inCurrInside ? inHotSpot : 0));
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------
//	Perform result of clicking and releasing mouse inside a HotSpot

void
LStdControl::HotSpotResult(
	SInt16	inHotSpot)
{
	if (inHotSpot >= kControlIndicatorPart) {
		BroadcastMessage(msg_ThumbDragged, (void*) this);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw a StdControl

void
LStdControl::DrawSelf()
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
		ApplyForeAndBackColors();

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


// ---------------------------------------------------------------------------
//	¥ ShowSelf
// ---------------------------------------------------------------------------
//	Show a StdControl

void
LStdControl::ShowSelf()
{
		// PP Panes don't draw immediately when showing; they wait
		// until the following update event. ShowControl() draws, so
		// we suppress drawing by making the clip region empty. We then
		// refresh to force an update event.

	FocusDraw();
	StClipRgnState	emptyClip(nil);
	::ShowControl(mMacControlH);
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ HideSelf
// ---------------------------------------------------------------------------
//	Hide a StdControl

void
LStdControl::HideSelf()
{
		// HideControl() erases the control, which could cause an
		// ugly flash. So we suppress drawing by making the clip region
		// empty and refresh to force an update event where what's
		// underneath the control will draw itself.

	FocusDraw();
	StClipRgnState	emptyClip(nil);
	::HideControl(mMacControlH);
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf
// ---------------------------------------------------------------------------
//	Enable a StdControl

void
LStdControl::EnableSelf()
{
	StFocusAndClipIfHidden	focus(this);		// Control will draw
	::HiliteControl(mMacControlH, 0);
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf
// ---------------------------------------------------------------------------
//	Disable a StdControl

void
LStdControl::DisableSelf()
{
	StFocusAndClipIfHidden	focus(this);		// Control will draw
	::HiliteControl(mMacControlH, 255);
}

#pragma mark -

// ===========================================================================
// ¥ LStdButton													  LStdButton ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LStdButton							Default Constructor
// ---------------------------------------------------------------------------

LStdButton::LStdButton()

	: LStdControl((SInt16)pushButProc)
{
	mIsDefault = false;
}


// ---------------------------------------------------------------------------
//	¥ LStdButton							Copy Constructor
// ---------------------------------------------------------------------------

LStdButton::LStdButton(
	const LStdButton&	inOriginal)
	
	: LStdControl(inOriginal)
{
	mIsDefault = inOriginal.mIsDefault;
}


// ---------------------------------------------------------------------------
//	¥ LStdButton							Parameterized Constructor
// ---------------------------------------------------------------------------

LStdButton::LStdButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	ResIDT			inTextTraitsID,
	Str255			inTitle)
	
	: LStdControl(inPaneInfo, inValueMessage, 0, 0, 0, pushButProc,
						inTextTraitsID, inTitle, 0)
{
	mIsDefault = false;
}


// ---------------------------------------------------------------------------
//	¥ LStdButton							Parameterized Constructor
// ---------------------------------------------------------------------------
//	Construct from input parameters and an existing ControlHandle

LStdButton::LStdButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	ResIDT			inTextTraitsID,
	ControlHandle	inMacControlH)
	
	: LStdControl(inPaneInfo, inValueMessage, 0, 0, 0, pushButProc,
						inTextTraitsID, inMacControlH)
{
	mIsDefault = false;
}


// ---------------------------------------------------------------------------
//	¥ LStdButton							Stream Constructor
// ---------------------------------------------------------------------------

LStdButton::LStdButton(
	LStream*	inStream)
	
	: LStdControl(inStream)
{
	mIsDefault = false;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortStructureRect										   [protected]
// ---------------------------------------------------------------------------
//	Calculate the Button's Structure Rectangle in Port coordinates
//
//	A Button may draw a default outline outside of its Frame
//
//	Returns true if the Rect is within QuickDraw space (16-bit)
//	Returns false if the Rect is outside QuickDraw space
//		and outRect is unchanged

bool
LStdButton::CalcPortStructureRect(
	Rect&	outRect) const
{
	bool	isInQDSpace = CalcPortFrameRect(outRect);
	
	if (isInQDSpace and mIsDefault) {		// Allow 4 pixel outset for
		::MacInsetRect(&outRect, -4, -4);	//   default ring
	}
	
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortExposedRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the exposed rectangle of a Button and return whether that
//	rectangle is empty. The rectangle is in Port coordinates.

Boolean
LStdButton::CalcPortExposedRect(
	Rect&	outRect,						// In Port coords
	bool	inOKIfHidden) const
{
	bool exposed = ((mVisible == triState_On) || inOKIfHidden)  &&
				   CalcPortStructureRect(outRect)  &&
				   (mSuperView != nil);

	if (exposed) {
		Rect	superRevealed;
		mSuperView->GetRevealedRect(superRevealed);

		exposed = ::SectRect(&outRect, &superRevealed, &outRect);
	}

	return exposed;
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LStdButton::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == kControlPushButtonDefaultTag) {
	
		Boolean	becomeDefault = *(Boolean *) inDataPtr;
	
		if (mIsDefault != becomeDefault) {
		
			
			if (UEnvironment::IsAppearanceRunning()) {
											// Appearance Manager will draw
											//   the button properly
				::SetControlData(mMacControlH, inPartCode, inTag,
										inDataSize, inDataPtr);
			}
		
			if (becomeDefault) {			// Immediately draw default ring
			
				mIsDefault = true;
				Draw(nil);
			
			} else {						// Ring will be erased when area
											//   underneath it redraws
				Refresh();
				mIsDefault = false;
			}
		}
		
	} else if (inTag == kControlPushButtonCancelTag) {
	
		// Do nothing. In Standard UI, cancel button does not have
		// a special appearance.
	
	} else {
		LStdControl::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LStdButton::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if (inTag == kControlPushButtonDefaultTag) {
	
		if (inBufferSize >= sizeof(Boolean)) {
			*(Boolean *) inBuffer = mIsDefault;
		}
		
		if (outDataSize != nil) {
			*outDataSize = sizeof(Boolean);
		}
	
	} else {
		LStdControl::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LStdButton::DrawSelf()
{
	LStdControl::DrawSelf();
	
	if (mIsDefault and not UEnvironment::IsAppearanceRunning()) {
	
			// When the Appearance Manager isn't running, the default
			// ring is a 3-pixel thick rounded rectangle
	
		Rect	frame;
		CalcLocalFrameRect(frame);
		::MacInsetRect(&frame, -4, -4);
		
		SInt16	roundCorner = (SInt16) ((frame.bottom - frame.top - 8) / 2);
		if (roundCorner < 16) {
			roundCorner = 16;
		}
		
		ApplyForeAndBackColors();
		::PenNormal();
		::PenSize(3, 3);
		
		if (::GetControlHilite(mMacControlH) != 0) {
										// Inactive button. Draw gray ring.
			Pattern	grayPat;
			UQDGlobals::GetGrayPat(&grayPat);
			::PenPat(&grayPat);
		}
		
		::FrameRoundRect(&frame, roundCorner, roundCorner);
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------
//	Respond to a click in a StdButton

void
LStdButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	BroadcastValueMessage();		// Although value doesn't change,
									//   send message to inform Listeners
									//   that button was clicked
}

#pragma mark -

// ===========================================================================
// ¥ LStdCheckBox												LStdCheckBox ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LStdCheckBox
// ---------------------------------------------------------------------------
//	Default Constructor

LStdCheckBox::LStdCheckBox()
	: LStdControl(checkBoxProc)
{
	SetMaxValue(1);
}


// ---------------------------------------------------------------------------
//	¥ LStdCheckBox(LStdCheckBox&)
// ---------------------------------------------------------------------------
//	Copy Constructor

LStdCheckBox::LStdCheckBox(
	const LStdCheckBox	&inOriginal)
		: LStdControl(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdCheckBox
// ---------------------------------------------------------------------------
//	Construct from input parameters

LStdCheckBox::LStdCheckBox(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,			// Must be 0 or 1
	ResIDT			inTextTraitsID,
	Str255			inTitle)
		: LStdControl(inPaneInfo, inValueMessage, inValue, 0, 1,
				checkBoxProc, inTextTraitsID, inTitle, 0)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdCheckBox
// ---------------------------------------------------------------------------
//	Construct from input parameters and an existing ControlHandle

LStdCheckBox::LStdCheckBox(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,			// Must be 0 or 1
	ResIDT			inTextTraitsID,
	ControlHandle	inMacControlH)
		: LStdControl(inPaneInfo, inValueMessage, inValue, 0, 1,
						checkBoxProc, inTextTraitsID, inMacControlH)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdCheckBox(LStream*)
// ---------------------------------------------------------------------------
//	Construct from data in a Stream

LStdCheckBox::LStdCheckBox(
	LStream	*inStream)
		: LStdControl(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------
//	Respond to a click in a StdCheckBox by toggling it between
//	checked (value = 0) and unchecked (value = 1)

void
LStdCheckBox::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	SetValue(1 - GetValue());
}

#pragma mark -

// ===========================================================================
// ¥ LStdRadioButton										 LStdRadioButton ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LStdRadioButton
// ---------------------------------------------------------------------------
//	Default Constructor

LStdRadioButton::LStdRadioButton()
	: LStdControl(radioButProc)
{
	SetMaxValue(1);
}


// ---------------------------------------------------------------------------
//	¥ LStdRadioButton(LStdRadioButton&)
// ---------------------------------------------------------------------------
//	Copy Constructor

LStdRadioButton::LStdRadioButton(
	const LStdRadioButton	&inOriginal)
		: LStdControl(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdRadioButton
// ---------------------------------------------------------------------------
//	Construct from input parameters

LStdRadioButton::LStdRadioButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,			// Must be 0 or 1
	ResIDT			inTextTraitsID,
	Str255			inTitle)
		: LStdControl(inPaneInfo, inValueMessage, inValue, 0, 1,
				radioButProc, inTextTraitsID, inTitle, 0)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdRadioButton
// ---------------------------------------------------------------------------
//	Construct from input parameters and an existing ControlHandle

LStdRadioButton::LStdRadioButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt32			inValue,			// Must be 0 or 1
	ResIDT			inTextTraitsID,
	ControlHandle	inMacControlH)
		: LStdControl(inPaneInfo, inValueMessage, inValue, 0, 1,
						radioButProc, inTextTraitsID, inMacControlH)
{
}


// ---------------------------------------------------------------------------
//	¥ LStdRadioButton(LStream*)
// ---------------------------------------------------------------------------
//	Construct from data in a Stream

LStdRadioButton::LStdRadioButton(
	LStream*	inStream)

	: LStdControl(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------
//	Set value of a RadioButton

void
LStdRadioButton::SetValue(
	SInt32	inValue)
{
	if (inValue != mValue) {
		LStdControl::SetValue(inValue);

			// If turning RadioButton on, broadcast message so that the
			// RadioGroup (if present) will turn off the other RadioButtons
			// in the group.

		if (mValue == Button_On) {
			BroadcastMessage(msg_ControlClicked, (void*) this);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------
//	Respond to a click in a StdRadioButton

void
LStdRadioButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
		// In the Mac interface, clicking on a RadioButton always
		// turns it on (or leaves it on). The standard way to turn
		// off a RadioButton is to turn on another one in the
		// same Radio Group. A Radio Group will normally be a
		// Listener of a RadioButton.

	SetValue(Button_On);
}

#pragma mark -
// ===========================================================================
// ¥ LStdPopupMenu											   LStdPopupMenu ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LStdPopupMenu							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LStdPopupMenu::LStdPopupMenu(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inTitleOptions,
	ResIDT				inMENUid,
	SInt16				inTitleWidth,
	SInt16				inPopupVariation,
	ResIDT				inTextTraitsID,
	Str255				inTitle,
	OSType				inResTypeMENU,
	SInt16				inInitialMenuItem)

	: LStdControl(inPaneInfo, inValueMessage, inTitleOptions,
				inMENUid, inTitleWidth, inPopupVariation, inTextTraitsID,
				inTitle, (SInt32) inResTypeMENU)
{
	InitStdPopupMenu(inInitialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LStdPopupMenu							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from input parameters and an existing ControlHandle

LStdPopupMenu::LStdPopupMenu(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inMaxValue,
	ResIDT				inTextTraitsID,
	ControlHandle		inMacControlH)

	: LStdControl(inPaneInfo, inValueMessage, 1, 1, inMaxValue,
						popupMenuProc, inTextTraitsID, inMacControlH)
{
	mValue = 1;					// New popup is always on first menu item
}


// ---------------------------------------------------------------------------
//	¥ LStdPopupMenu							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LStdPopupMenu::LStdPopupMenu(
	LStream*	inStream)

	: LStdControl(inStream)
{
	SInt16	initialMenuItem;
	*inStream >> initialMenuItem;

	InitStdPopupMenu(initialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LStdPopupMenu							Destructor				  [public]
// ---------------------------------------------------------------------------

LStdPopupMenu::~LStdPopupMenu()
{
}


// ---------------------------------------------------------------------------
//	¥ InitStdPopupMenu						Private Initializer		 [private]
// ---------------------------------------------------------------------------

void
LStdPopupMenu::InitStdPopupMenu(
	SInt16	inInitialMenuItem)
{
		// Popups use the initial values for other purposes. Control Manager
		// determines min/max from the size of the Menu. So now we have to
		// adjust the value, min, and max stored by LControl.

	mValue    = ::GetControlValue(mMacControlH);
	mMinValue = ::GetControlMinimum(mMacControlH);
	mMaxValue = ::GetControlMaximum(mMacControlH);

	if (inInitialMenuItem != mValue) {
		::SetControlValue(mMacControlH, inInitialMenuItem);
		mValue = ::GetControlValue(mMacControlH);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetMacMenuH													  [public]
// ---------------------------------------------------------------------------
//	Return the MenuHandle associated with a StdPopupMenu

MenuHandle
LStdPopupMenu::GetMacMenuH() const
{
#if ACCESSOR_CALLS_ARE_FUNCTIONS

	return ::GetControlPopupMenuHandle(GetMacControl());

#else

	PopupPrivateDataHandle	dataH =
		(PopupPrivateDataHandle) (**GetMacControl()).contrlData;

	MenuHandle	menuH = nil;
	if (dataH != nil) {
		menuH = (**dataH).mHandle;
	}

	return menuH;

#endif
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------
//	Track the mouse while it is down after clicking in a Control HotSpot
//
//	Returns whether the mouse is released within the HotSpot

Boolean
LStdPopupMenu::TrackHotSpot(
	SInt16	/* inHotSpot */,
	Point 	inPoint,
	SInt16	/* inModifiers */)
{

		// TrackControl handles tracking and returns kControlNoPart
		// if the mouse is released outside the HotSpot

	Boolean	releasedInHotSpot =
		::TrackControl(mMacControlH, inPoint, (ControlActionUPP) (-1))
				!= kControlNoPart;

		// Control Mananager changes value to the selected menu
		// item. We need to set our value accordingly.

	SetValue(::GetControlValue(mMacControlH));

	return releasedInHotSpot;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw popup menu

void
LStdPopupMenu::DrawSelf()
{
		// For some reason, after a Popup Menu moves, the Control Manager
		// erases the old location of the Popup. All this happens within
		// the Draw1Control() call to draw the Popup at its new location.
		// To prevent this erasure, we restrict the clipping region to
		// just the current location of the Popup.

	Rect	frame;
	CalcLocalFrameRect(frame);
	StClipRgnState	clip;
	clip.ClipToIntersection(frame);
	LStdControl::DrawSelf();
}

#pragma mark -

// ===========================================================================
// ¥ LDefaultOutline										 LDefaultOutline ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LDefaultOutline
// ---------------------------------------------------------------------------
//	Construct an Outline for the specified host Pane

LDefaultOutline::LDefaultOutline(
	LPane	*inHostPane)
{
	SDimension16	hostSize;
	SPoint32		hostLocation;
	inHostPane->GetFrameSize(hostSize);
	inHostPane->GetFrameLocation(hostLocation);

									// Size and location of Outline depend
									//    on Host size and location
	PutInside(inHostPane->GetSuperView());
	ResizeFrameTo( (SInt16) (hostSize.width + 8),
				   (SInt16) (hostSize.height + 8), false);

	SPoint32		superLocation;
	mSuperView->GetFrameLocation(superLocation);
	PlaceInSuperFrameAt(hostLocation.h - superLocation.h - 4,
						hostLocation.v - superLocation.v - 4, false);

									// Use same FrameBinding as host
	inHostPane->GetFrameBinding(mFrameBinding);

	mEnabled = triState_Off;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw a LDefaultOutline

void
LDefaultOutline::DrawSelf()
{
		// Outline is a three-pixel thick rounded rectangle
		// The curvature of the corner is 1/2 the height of the host Pane

	Rect	frame;
	if (CalcLocalFrameRect(frame)) {
		SInt16	roundCorner = (SInt16) ((mFrameSize.height - 8) / 2);
		if (roundCorner < 16) {
			roundCorner = 16;
		}
		ApplyForeAndBackColors();
		::PenNormal();
		::PenSize(3, 3);
		::FrameRoundRect(&frame, roundCorner, roundCorner);
	}
}


PP_End_Namespace_PowerPlant
