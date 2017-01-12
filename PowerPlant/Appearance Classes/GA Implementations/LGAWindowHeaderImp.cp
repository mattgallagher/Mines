// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAWindowHeaderImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAWindowHeaderImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LStream.h>
#include <UDrawingState.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAWindowHeaderImp					Constructor				  [public]
// ---------------------------------------------------------------------------

LGAWindowHeaderImp::LGAWindowHeaderImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
	mHasBottomDivider = false;
}


// ---------------------------------------------------------------------------
//	¥ LGAWindowHeaderImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAWindowHeaderImp::LGAWindowHeaderImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mHasBottomDivider = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAWindowHeaderImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAWindowHeaderImp::~LGAWindowHeaderImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString (title);

	Init(inControlPane, controlKind);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;

	mHasBottomDivider = (inControlKind == kControlWindowHeaderProc);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetHasBottomDivider											  [public]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::SetHasBottomDivider(
	bool	 inHasBottomDivider,
	bool	 inRedraw)
{
	if (mHasBottomDivider != inHasBottomDivider) {
		mHasBottomDivider = inHasBottomDivider;

		if (inRedraw) {
			mControlPane->Refresh ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LGAWindowHeaderImp::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = Color_Black;
	}

	if (outBackColor != nil) {
		SInt8 index = colorRamp_Gray1;

		if (IsEnabled() && IsActive()) {
			index = colorRamp_Gray2;
		}

		UGAColorRamp::GetColor(index, *outBackColor);
	}

	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::DrawSelf()
{
	DrawHeaderFace();
	DrawHeaderBorder();
	DrawHeaderShading();
}


// ---------------------------------------------------------------------------
//	¥ DrawHeaderFace											   [protected]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::DrawHeaderFace()
{
	Rect		faceRect;
	CalcLocalFrameRect(faceRect);

		// If less than 16 colors (bit depth 4), draw a white background.
		// Otherwise, draw a gray background.

	RGBColor	faceColor = Color_White;

	if (mControlPane->GetBitDepth() >= 4) {

		SInt8	colorIndex = colorRamp_Gray1;

		if (IsActive() && IsEnabled()) {
			colorIndex = colorRamp_Gray2;
		}

		UGAColorRamp::GetColor(colorIndex, faceColor);
	}

	StColorPenState::Normalize();
	::RGBForeColor(&faceColor);
	::PaintRect(&faceRect);
}


// ---------------------------------------------------------------------------
//	¥ DrawHeaderBorder											   [protected]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::DrawHeaderBorder ()
{
	Rect		borderRect;
	CalcLocalFrameRect(borderRect);

		// Border is always black, except when there are more than 16
		// colors (bit depth 4). Then we draw a gray border if the
		// header is disabled or inactive.

	RGBColor	borderColor = Color_Black;

	if ( (mControlPane->GetBitDepth() >= 4) &&
		 (!IsEnabled() || !IsActive()) ) {

		borderColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	StColorPenState::Normalize();
	::RGBForeColor(&borderColor);

	if (HasBottomDivider()) {
		::MacFrameRect(&borderRect);

	} else {				// No bottom divider, draw top and side lines

		::MoveTo( 	 borderRect.left,		borderRect.bottom);
		::MacLineTo( borderRect.left,		borderRect.top);
		::MacLineTo( (SInt16) (borderRect.right - 1),	borderRect.top);
		::MacLineTo( (SInt16) (borderRect.right - 1),	borderRect.bottom);
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawHeaderShading											   [protected]
// ---------------------------------------------------------------------------

void
LGAWindowHeaderImp::DrawHeaderShading ()
{
		// Draw shading for enabled and active headers with 16 colors
		// (bit depth 4) or more.

	if ( (mControlPane->GetBitDepth() >= 4) &&
		 IsEnabled () && IsActive ()) {

		Rect		shadingRect;
		CalcLocalFrameRect(shadingRect);
		::MacInsetRect(&shadingRect, 1, 1);

		if (!HasBottomDivider()) {		// Without bottom divider, draw
			shadingRect.bottom++;		//   shading one pixel lower
		}

		StColorPenState::Normalize();
										// Lighten top left sides
		::RGBForeColor(&Color_White);
		UGraphicUtils::TopLeftSide(shadingRect, 0, 0, 1, 0);

										// Darken bottom right sides
		RGBColor	shadingColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		::RGBForeColor(&shadingColor);
		UGraphicUtils::BottomRightSide(shadingRect, 1, 1, 0, 0);
	}
}


PP_End_Namespace_PowerPlant
