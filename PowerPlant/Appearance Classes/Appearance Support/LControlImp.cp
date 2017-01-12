// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlImp.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Base class for control implementations

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LControlImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LControlImp							Constructor				  [public]
// ---------------------------------------------------------------------------

LControlImp::LControlImp(
	LControlPane*	inControlPane)
{
	mControlPane = inControlPane;
}


// ---------------------------------------------------------------------------
//	¥ LControlImp							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LControlImp::LControlImp(
	LStream*	/* inStream */)
{
	mControlPane = nil;
}


// ---------------------------------------------------------------------------
//	¥ LControlImp							Destructor				  [public]
// ---------------------------------------------------------------------------

LControlImp::~LControlImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LControlImp::Init(
	LControlPane*	inControlPane,
	LStream*		/* inStream */)
{
	mControlPane = inControlPane;
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LControlImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf												  [public]
// ---------------------------------------------------------------------------

void
LControlImp::FinishCreateSelf()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::ResizeFrameBy(
	SInt16		/* inWidthDelta */,
	SInt16		/* inHeightDelta */)
{
}


// ---------------------------------------------------------------------------
//	¥ MoveBy														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::MoveBy(
	SInt32		/* inHorizDelta */,
	SInt32		/* inVertDelta */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LControlImp::GetDescriptor(
	Str255		outDescriptor) const
{
	return outDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetDescriptor(
	ConstStringPtr	/* inDescriptor */)
{
}

#if PP_Uses_CFDescriptor

// ---------------------------------------------------------------------------
//	¥ CopyCFDescriptor												  [public]
// ---------------------------------------------------------------------------

CFStringRef
LControlImp::CopyCFDescriptor() const
{
	return nil;
}


// ---------------------------------------------------------------------------
//	¥ SetCFDescriptor												  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetCFDescriptor(
	CFStringRef	/* inStringRef */)
{
}

#endif

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

bool
LControlImp::GetValue(
	SInt32&		/* outValue */) const
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetValue(
	SInt32		/* inValue */)
{
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::PostSetValue()
{
}


// ---------------------------------------------------------------------------
//	¥ SetMinValue													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetMinValue(
	SInt32		/* inMinValue */)
{
}


// ---------------------------------------------------------------------------
//	¥ SetMaxValue													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetMaxValue(
	SInt32		/* inMaxValue */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetAllValues													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::GetAllValues(
	SInt32&		outValue,
	SInt32&		outMinValue,
	SInt32&		outMaxValue)
{
	outValue    = mControlPane->GetValue();
	outMinValue = mControlPane->GetMinValue();
	outMaxValue = mControlPane->GetMaxValue();
}


// ---------------------------------------------------------------------------
//	¥ GetMacControl													  [public]
// ---------------------------------------------------------------------------
//	Return the associated Toolbox Control
//
//	Subclasses which use a Toolbox Control should override this function

ControlHandle
LControlImp::GetMacControl() const
{
	return nil;
}


// ---------------------------------------------------------------------------
//	¥ SetScrollViewSize												  [public]
// ---------------------------------------------------------------------------
//	Specify the size of the view being scrolled by a control

void
LControlImp::SetScrollViewSize(
	SInt32		/* inViewSize */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------
//	Controls may draw outside the Frame of its ControlPane. The bounds within
//	which all drawing occurs is the Structure. The Structure offsets are
//	values >= 0 that specify how far outside the Frame the Structure is
//	on each side.
//
//	Returns error code if unable to calculate offsets

OSStatus
LControlImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
	outOffsets = Rect_0000;
	
	return noErr;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ FocusImp														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::FocusImp()
{
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LControlImp::FindHotSpot(
	Point		/* inPoint */) const
{
	return 1;
}


// ---------------------------------------------------------------------------
//	¥ PointInHotSpot												  [public]
// ---------------------------------------------------------------------------

Boolean
LControlImp::PointInHotSpot(
	Point		/* inPoint */,
	SInt16		/* inHotSpot */) const
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LControlImp::TrackHotSpot(
	SInt16		/* inHotSpot */,
	Point		/* inPoint */,
	SInt16		/* inModifiers */)
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		/* inCurrInside */,
	Boolean		/* inPrevInside */)
{
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::DoneTracking(
	SInt16		inHotSpot,
	Boolean		/* inGoodTrack */)
{
	HotSpotAction(inHotSpot, false, true);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::DrawSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ ShowSelf														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::ShowSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ HideSelf														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::HideSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::ActivateSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LControlImp::DeactivateSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::EnableSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::DisableSelf()
{
}


// ---------------------------------------------------------------------------
//	¥ Hilite														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::Hilite(
	SInt16		/* inPartCode */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetDataTag(
	SInt16			/* inPartCode */,
	FourCharCode	/* inTag */,
	Size			/* inDataSize */,
	void*			/* inDataPtr */)
{
	SignalStringLiteral_("Unsupported data tag");
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::GetDataTag(
	SInt16			/* inPartCode */,
	FourCharCode	/* inTag */,
	Size			/* inBufferSize */,
	void*			/* inBuffer */,
	Size*			/* outDataSize */) const
{
	ThrowOSErr_(errDataNotSupported);
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetTextTraitsID(
	ResIDT		/* inTextTraitsID */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

ResIDT
LControlImp::GetTextTraitsID() const
{
	return 0;
}


// ---------------------------------------------------------------------------
//	¥ SetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::SetFontStyle(
	const ControlFontStyleRec&	/* inStyleRec */)
{
}


// ---------------------------------------------------------------------------
//	¥ GetFontStyle													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::GetFontStyle(
	ControlFontStyleRec&	/* outStyleRec */) const
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors										  [public]
// ---------------------------------------------------------------------------

bool
LControlImp::ApplyForeAndBackColors() const
{
	RGBColor backColor, foreColor;
	bool	hasColors = GetForeAndBackColors(&foreColor, &backColor);

	if (hasColors) {
		::RGBForeColor(&foreColor);
		::RGBBackColor(&backColor);
	}

	return hasColors;
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LControlImp::GetForeAndBackColors(
	RGBColor*	/* outForeColor */,
	RGBColor*	/* outBackColor */) const
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ SupportsCalcBestRect											  [public]
// ---------------------------------------------------------------------------

Boolean
LControlImp::SupportsCalcBestRect() const
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LControlImp::CalcBestControlRect(
	SInt16&		/* outWidth */,
	SInt16&		/* outHeight */,
	SInt16&		/* outBaselineOffset */) const
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Idle															  [public]
// ---------------------------------------------------------------------------

void
LControlImp::Idle()
{
}


// ---------------------------------------------------------------------------
//	¥ IdlesAutomatically											  [public]
// ---------------------------------------------------------------------------

bool
LControlImp::IdlesAutomatically() const
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ DoKeyPress													  [public]
// ---------------------------------------------------------------------------

SInt16
LControlImp::DoKeyPress(
	const EventRecord&	/* inKeyEvent */)
{
	return kControlNoPart;
}


// ---------------------------------------------------------------------------
//	¥ BeTarget														  [public]
// ---------------------------------------------------------------------------

void
LControlImp::BeTarget(
	ControlFocusPart	/* inFocusPart */)
{
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget													  [public]
// ---------------------------------------------------------------------------

void
LControlImp::DontBeTarget()
{
}


// ---------------------------------------------------------------------------
//	¥ SetKeyboardFocusPart											  [public]
// ---------------------------------------------------------------------------

bool
LControlImp::SetKeyboardFocusPart(
	ControlFocusPart	/* inFocusPart */)
{
	return false;
}


PP_End_Namespace_PowerPlant
