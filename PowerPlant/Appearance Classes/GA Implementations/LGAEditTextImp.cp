// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAEditTextImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAEditTextImp.h>
#include <LEditText.h>
#include <UDrawingState.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAEditTextImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAEditTextImp::LGAEditTextImp(
	LControlPane*	inControlPane)

	: LControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAEditTextImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------

LGAEditTextImp::LGAEditTextImp(
	LStream*	inStream)

	: LControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGAEditTextImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAEditTextImp::~LGAEditTextImp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf
// ---------------------------------------------------------------------------

void
LGAEditTextImp::DrawSelf()
{
	bool	hasRing = (dynamic_cast<LEditText*>(mControlPane))->HasFocusRing();

			// The focus ring draws over the outer edge of the border.
			// So, if the ring exists, we draw the border first, then
			// the ring. But if the ring does not exist, we "draw"
			// the ring first (really erases the area which would be
			// covered by the ring), then the border.

	SInt16	depth = mControlPane->GetBitDepth();

	if (hasRing) {
		DrawBorder ( depth );					// Draw border, then ring
		DrawFocusRing ( hasRing, depth );

	} else {
		DrawFocusRing ( hasRing, depth );		// This erases ring area
		DrawBorder ( depth );
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawBorder
// ---------------------------------------------------------------------------

void
LGAEditTextImp::DrawBorder(
	SInt16		inBitDepth)
{
	StColorPenState::Normalize();

	// Figure out the inset pane's bounding rect
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	// Draw the inner frame around the edit text
	RGBColor	tempColor = Color_Black;
	if ( (!IsEnabled() || !IsActive()) && inBitDepth >= 4 ) {
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}
	::RGBForeColor ( & tempColor );
	::MacInsetRect ( &localFrame, 2, 2 );	// Draw black border. 2 pixel inset leaves
	::MacFrameRect ( &localFrame );			//   room for the focus ring.

	// Draw the shadows around the outside of the inner frame,
	// we only do this if we are drawing in color and when we are active and
	// enabled

	if ( inBitDepth >= 4 && IsEnabled() && IsActive()) {
		// TOP LEFT SHADOW
		::MacInsetRect ( &localFrame, -1, -1 );
		tempColor = UGAColorRamp::GetColor(5);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

		// BOTTOM RIGHT SHADOW
		::RGBForeColor ( &Color_White );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );
	}

}


// ---------------------------------------------------------------------------
//	¥ DrawFocusRing
// ---------------------------------------------------------------------------

void
LGAEditTextImp::DrawFocusRing(
	bool		inHasRing,
	SInt16		/* inBitDepth */)
{
	Rect	frame;
	CalcLocalFrameRect ( frame );

	StColorPenState::Normalize();

	if (inHasRing) {					// Draw ring in accent color
		RGBColor	ringColor;
		UGAColorRamp::GetFocusAccentColor(ringColor);

		::RGBForeColor(&ringColor);
		::PenSize(2, 2);
		::FrameRoundRect(&frame, 4, 4);

	} else {							// Erase area covered by ring
		mControlPane->ApplyForeAndBackColors();
		StRegion	ringRgn = frame;
		::MacInsetRect(&frame, 2, 2);
		ringRgn -= frame;
		::EraseRgn(ringRgn);
	}
}


PP_End_Namespace_PowerPlant
