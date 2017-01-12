// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlPane.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Host Pane for a control that is implemented via a helper "imp" class
//
//	To support Appearance Manager controls which may draw outside their
//	control rectangle, a ControlPane stores structure offsets. The Frame of
//	the ControlPane is aligned with the control rectangle, and then the
//	structure offsets (which should be values >= 0) specify how far outside
//	the Frame the structure is on each side.
//
//	When drawing, a ControlPane considers the structure rectangle instead of
//	the Frame. However, hit testing for mouse clicks still use the Frame.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LControlPane.h>
#include <LCommander.h>
#include <LControlImp.h>
#include <LString.h>
#include <LView.h>
#include <UDrawingState.h>
#include <URegistrar.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LControlPane							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LControlPane::LControlPane(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControl(inStream)
{
	mControlImp = nil;
	mStructureOffsets = Rect_0000;

	MakeControlImp(inImpID, inStream);
}


// ---------------------------------------------------------------------------
//	¥ LControlPane							Stream Constructor		  [public]
// ---------------------------------------------------------------------------
//	Does not create an Implementation object. You MUST call MakeControlImp()
//	before using the ControlPane.
//
//	Use this constructor in the initializer list for a subclass where you
//	don't know the control kind or title until later.

LControlPane::LControlPane(
	LStream*	inStream)

	: LControl(inStream)
{
	mControlImp = nil;
	mStructureOffsets = Rect_0000;
}


// ---------------------------------------------------------------------------
//	¥ LControlPane							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LControlPane::LControlPane(
	const SPaneInfo&	inPaneInfo,
	ClassIDT			inImpID,
	SInt16				inControlKind,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	SInt32				inRefCon)

	: LControl(inPaneInfo, inValueMessage, inValue, inMinValue, inMaxValue)
{
	mControlImp = nil;
	mStructureOffsets = Rect_0000;

	MakeControlImp(inImpID, inControlKind, inTitle, inTextTraitsID, inRefCon);
}


// ---------------------------------------------------------------------------
//	¥ LControlPane							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Does not create an Implementation object. You MUST call MakeControlImp()
//	before using the ControlPane.
//
//	Use this constructor in the initializer list for a subclass where you
//	don't know the control kind or title until later.

LControlPane::LControlPane(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue)

	: LControl(inPaneInfo, inValueMessage, inValue, inMinValue, inMaxValue)
{
	mControlImp = nil;
	mStructureOffsets = Rect_0000;
}


// ---------------------------------------------------------------------------
//	¥ ~LControlPane							Destructor				  [public]
// ---------------------------------------------------------------------------

LControlPane::~LControlPane()
{
	delete mControlImp;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::FinishCreateSelf()
{
	mControlImp->FinishCreateSelf();
	mControlImp->GetStructureOffsets(mStructureOffsets);
}


// ---------------------------------------------------------------------------
//	¥ MakeControlImp												  [public]
// ---------------------------------------------------------------------------

void
LControlPane::MakeControlImp(
	ClassIDT	inImpID,
	LStream*	inStream)
{
	LAttachable	*defaultAttachable = LAttachable::GetDefaultAttachable();
	LCommander	*defaultCommander  = LCommander::GetDefaultCommander();
	LView		*defaultView	   = LPane::GetDefaultView();

	mControlImp = static_cast<LControlImp*>(
						URegistrar::CreateObject(inImpID, inStream));

	if (mControlImp == nil) {
		#ifdef Debug_Signal
				LStr255	msg(StringLiteral_("Unregistered Control Imp ClassID: "));
				msg.Append(&inImpID, sizeof(inImpID));
				SignalString_(msg);
		#endif
		Throw_(err_NilPointer);

	} else {
		mControlImp->Init(this, inStream);

		LAttachable::SetDefaultAttachable(defaultAttachable);
		LCommander::SetDefaultCommander(defaultCommander);
		LPane::SetDefaultView(defaultView);
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeControlImp												  [public]
// ---------------------------------------------------------------------------

void
LControlPane::MakeControlImp(
	ClassIDT			inImpID,
	SInt16				inControlKind,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	SInt32				inRefCon)
{
	delete mControlImp;

	LAttachable	*defaultAttachable = LAttachable::GetDefaultAttachable();
	LCommander	*defaultCommander = LCommander::GetDefaultCommander();
	LView		*defaultView = LPane::GetDefaultView();

	mControlImp = static_cast<LControlImp*>(
						URegistrar::CreateObject(inImpID, nil));

	if (mControlImp == nil) {
		#ifdef Debug_Signal
				LStr255	msg(StringLiteral_("Unregistered Control Imp ClassID: "));
				msg.Append(&inImpID, sizeof(inImpID));
				SignalString_(msg);
		#endif
		Throw_(err_NilPointer);

	} else {
		mControlImp->Init(this, inControlKind, inTitle, inTextTraitsID, inRefCon);

		LAttachable::SetDefaultAttachable(defaultAttachable);
		LCommander::SetDefaultCommander(defaultCommander);
		LPane::SetDefaultView(defaultView);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);

	mControlImp->ResizeFrameBy(inWidthDelta, inHeightDelta);
	mControlImp->GetStructureOffsets(mStructureOffsets);
}


// ---------------------------------------------------------------------------
//	¥ MoveBy														  [public]
// ---------------------------------------------------------------------------

void
LControlPane::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LPane::MoveBy(inHorizDelta, inVertDelta, inRefresh);

	mControlImp->MoveBy(inHorizDelta, inVertDelta);
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LControlPane::GetDescriptor(
	Str255		outDescriptor) const
{
	return mControlImp->GetDescriptor(outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	mControlImp->SetDescriptor(inDescriptor);
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------

CFStringRef
LControlPane::CopyCFDescriptor() const
{
	return mControlImp->CopyCFDescriptor();
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetCFDescriptor(
	CFStringRef	inStringRef)
{
	mControlImp->SetCFDescriptor(inStringRef);
}

#endif

// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

SInt32
LControlPane::GetValue() const
{
	SInt32	value = mValue;
	mControlImp->GetValue(value);
	return value;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetValue(
	SInt32		inValue)
{
	if (inValue < mMinValue) {		// Enforce min/max range
		inValue = mMinValue;
	} else if (inValue > mMaxValue) {
		inValue = mMaxValue;
	}

	if (mValue != inValue) {		// If value is not the current value
		mControlImp->SetValue(inValue);
		LControl::SetValue(inValue);
		mControlImp->PostSetValue ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetMinValue(
	SInt32		inMinValue)
{
	mControlImp->SetMinValue(inMinValue);
	LControl::SetMinValue(inMinValue);
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetMaxValue(
	SInt32		inMaxValue)
{
	mControlImp->SetMaxValue(inMaxValue);
	LControl::SetMaxValue(inMaxValue);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusDraw														  [public]
// ---------------------------------------------------------------------------

Boolean
LControlPane::FocusDraw(
	LPane*		/* inSubPane */)
{
	Boolean	revealed = LControl::FocusDraw();

		// To keep the Control Manager happy, we must set the
		// origin to (0, 0). This invalidates PP's focus cache
		// and shifts the clipping region.
		
										// Get port bounds and clipping
										//   region before changing origin
	GrafPtr		port = UQDGlobals::GetCurrentPort();
	Rect		portRect;
	::GetPortBounds(port, &portRect);
	
	StRegion	clipRgn;
	::GetClip(clipRgn);
	
	::SetOrigin(0, 0);
	
										// After origin change, PP focus
	LView::OutOfFocus(nil);				//   is no longer valid

										// We want to clip to the same
										//   screen area as before, so we
										//   must offset the clip region
										//   to account for the origin change
	clipRgn.OffsetBy((SInt16) -portRect.left, (SInt16) -portRect.top);
	::SetClip(clipRgn);

	mControlImp->FocusImp();

	return revealed;
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LControlPane::FindHotSpot(
	Point		inPoint) const
{
	return mControlImp->FindHotSpot(inPoint);
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot												  [public]
// ---------------------------------------------------------------------------

Boolean
LControlPane::PointInHotSpot(
	Point		inPoint,
	SInt16		inHotSpot) const
{
	return mControlImp->PointInHotSpot(inPoint, inHotSpot);
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LControlPane::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{
	return mControlImp->TrackHotSpot(inHotSpot, inPoint, inModifiers);
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::HotSpotAction(
	SInt16		inHotSpot,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
	mControlImp->HotSpotAction(inHotSpot, inCurrInside, inPrevInside);

	SInt16	part = kControlNoPart;
	if (inCurrInside) {
		part = inHotSpot;
	}

	DoTrackAction(part, mValue);
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::DoneTracking(
	SInt16		inHotSpot,
	Boolean		inGoodTrack)
{
	mControlImp->DoneTracking(inHotSpot, inGoodTrack);
}


// ---------------------------------------------------------------------------
//	¥ DoTrackAction													  [public]
// ---------------------------------------------------------------------------
//	Override to do something while the mouse is being held down after click
//	inside the control
//
//	inHotSpot is kControlNoPart if the mouse is not currently inside the
//	part that was originally clicked.
//
//	inValue may be different from the control's stored mValue

void
LControlPane::DoTrackAction(
	SInt16		/* inHotSpot */,
	SInt32		/* inValue */)
{
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::DrawSelf()
{
	mControlImp->DrawSelf();
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf													   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::ShowSelf()
{
	mControlImp->ShowSelf();
}


// ---------------------------------------------------------------------------
//	¥ HideSelf													   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::HideSelf()
{
	mControlImp->HideSelf();
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf												   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::ActivateSelf()
{
	mControlImp->ActivateSelf();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::DeactivateSelf()
{
	mControlImp->DeactivateSelf();
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::EnableSelf()
{
	mControlImp->EnableSelf();
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf												   [protected]
// ---------------------------------------------------------------------------

void
LControlPane::DisableSelf()
{
	mControlImp->DisableSelf();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetAllValues													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::GetAllValues(
	SInt32	&outValue,
	SInt32	&outMinValue,
	SInt32	&outMaxValue)
{
	mControlImp->GetAllValues(outValue, outMinValue, outMaxValue);
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	mControlImp->SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size			*outDataSize) const
{
	mControlImp->GetDataTag(inPartCode, inTag, inBufferSize, inBuffer,
							outDataSize);
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	mControlImp->SetTextTraitsID(inTextTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ GetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

ResIDT
LControlPane::GetTextTraitsID() const
{
	return mControlImp->GetTextTraitsID();
}


// ---------------------------------------------------------------------------
//	¥ SetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::SetFontStyle(
	ControlFontStyleRec	&inStyleRec)
{
	mControlImp->SetFontStyle(inStyleRec);
}


// ---------------------------------------------------------------------------
//	¥ GetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LControlPane::GetFontStyle(
	ControlFontStyleRec	&outStyleRec)
{
	mControlImp->GetFontStyle(outStyleRec);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ImpStructureChanged											  [public]
// ---------------------------------------------------------------------------
//	Notification that control imp's Structure region may have changed

void
LControlPane::ImpStructureChanged()
{
	mControlImp->GetStructureOffsets(mStructureOffsets);
}


// ---------------------------------------------------------------------------
//	¥ CalcPortStructureRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the Pane's Structure Rectangle in Port coordinates
//
//	A ControlPane may draw outside of its Frame. This feature was added to
//	support Appearance Manager controls, which may draw outside their
//	control rectangles.
//
//	Returns true if the Rect is within QuickDraw space (16-bit)
//	Returns false if the Rect is outside QuickDraw space
//		and outRect is unchanged

bool
LControlPane::CalcPortStructureRect(
	Rect&	outRect) const
{
	bool	isInQDSpace = CalcPortFrameRect(outRect);
	
		// Structure offsets should always be >= 0, indicating
		// how far outside the frame the structure is on each side

	if (isInQDSpace) {
		outRect.left   -= mStructureOffsets.left;
		outRect.top    -= mStructureOffsets.top;
		outRect.right  += mStructureOffsets.right;
		outRect.bottom += mStructureOffsets.bottom;
	}
	
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortExposedRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the exposed rectangle of a Pane and return whether that
//	rectangle is empty. The rectangle is in Port coordinates.

Boolean
LControlPane::CalcPortExposedRect(
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
//	¥ SupportsCalcBestRect											  [public]
// ---------------------------------------------------------------------------

Boolean
LControlPane::SupportsCalcBestRect() const
{
	return mControlImp->SupportsCalcBestRect();
}


// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LControlPane::CalcBestControlRect(
	SInt16&		outWidth,
	SInt16&		outHeight,
	SInt16&		outBaselineOffset) const
{
	if (mControlImp->SupportsCalcBestRect()) {
		mControlImp->CalcBestControlRect(outWidth, outHeight, outBaselineOffset);
		
	} else {
		outWidth = mFrameSize.width;
		outHeight = mFrameSize.height;
		outBaselineOffset = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetContainer
// ---------------------------------------------------------------------------
//	Return the Container of a ControlPane
//
//	The Container is normally the SuperView. However, controls that use
//	LControlSubPane have a SuperView that is an LControlView, which is
//	an implementation detail and not the logical container.

LPane*
LControlPane::GetContainer() const
{
	return mSuperView;
}


// ---------------------------------------------------------------------------
//	¥ PortToLocalPoint
// ---------------------------------------------------------------------------
//	Convert point from Port to Local coordinates

void
LControlPane::PortToLocalPoint(
	Point&	/* ioPoint */) const
{
}		// No conversion needed. Local coords are the same as Port coords.


// ---------------------------------------------------------------------------
//	¥ LocalToPortPoint
// ---------------------------------------------------------------------------
//	Convert point from Local to Port coordinates

void
LControlPane::LocalToPortPoint(
	Point&	/* ioPoint */) const
{
}		// No conversion needed. Local coords are the same as Port coords.


PP_End_Namespace_PowerPlant

