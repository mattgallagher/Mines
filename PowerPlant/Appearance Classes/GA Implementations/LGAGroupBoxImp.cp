// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAGroupBoxImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAGroupBoxImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LControlPane.h>
#include <UDrawingState.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


enum {
	groupBox_HeaderLeftOffset		=	9,
	groupBox_HeaderLineOffset		=	3,
	groupBox_DefaultHeaderPaneID	=	FOUR_CHAR_CODE('grph')
};


// ---------------------------------------------------------------------------
//	¥ LGAGroupBoxImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAGroupBoxImp::LGAGroupBoxImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
	mPrimaryBorder = true;
	mHeaderObject  = nil;
	mBaseline	   = 0;
}


// ---------------------------------------------------------------------------
//	¥ LGAGroupBoxImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAGroupBoxImp::LGAGroupBoxImp(
	LStream	*inStream)

	: LGAControlImp(inStream)
{
	mPrimaryBorder = true;
	mHeaderObject  = nil;
	mBaseline	   = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAGroupBoxImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAGroupBoxImp::~LGAGroupBoxImp ()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGAGroupBoxImp::GetDescriptor (
		Str255	outTitle ) const
{
	if ( HasHeader ()) {
		return mHeaderObject->GetDescriptor ( outTitle );

	} else {
		outTitle[0] = 0;
		return nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetBoxBorderRegion											  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGAGroupBoxImp::GetBoxBorderRegion	(	const Rect	&inRevealed )
{
												// Build region for the border, which is the border
												// rect plus and offsets
	StRegion		borderRgn;
	{
		StRegionBuilder	builder(borderRgn);

		Rect	borderRect;
		CalcBorderRect ( borderRect );
		::MacFrameRect ( &borderRect );	// Outer edge of border
		borderRect.left += 3;
		borderRect.top += 3;
		borderRect.bottom -= 3;
		borderRect.right -= 3;
		::MacFrameRect ( &borderRect );	// Inner edge of border
	}

												// Removed revealed rectangle from border region
	mControlPane->PortToLocalPoint ( topLeft ( inRevealed ));
	mControlPane->PortToLocalPoint ( botRight ( inRevealed ));
	borderRgn -= inRevealed;

												// Return the region forming the border portion of
												// the group box
	return borderRgn.Release();

}


// ---------------------------------------------------------------------------
//	¥ GetBoxContentRegion											  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGAGroupBoxImp::GetBoxContentRegion	(
		const Rect	&inRevealed )
{
												// Create a new region
	Rect	contentRect;
	CalcContentRect ( contentRect );
	StRegion		contentRgn ( contentRect );

												// Clip the region using the revealed rect
	mControlPane->PortToLocalPoint ( topLeft ( inRevealed ));
	mControlPane->PortToLocalPoint ( botRight ( inRevealed ));

	contentRgn &= inRevealed;

												// Return the region forming the border portion of
												// the group box
	return contentRgn.Release();

}


// ---------------------------------------------------------------------------
//	¥ CalcHeaderRect												  [public]
// ---------------------------------------------------------------------------
//

void
LGAGroupBoxImp::CalcHeaderRect (
		Rect &outRect )
{
	if ( HasHeader ()) {
		mHeaderObject->CalcLocalFrameRect ( outRect );
		::MacInsetRect ( &outRect, -groupBox_HeaderLineOffset, 0 );

	} else {
		outRect = Rect_0000;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcContentRect												  [public]
// ---------------------------------------------------------------------------
//

void
LGAGroupBoxImp::CalcContentRect (
		Rect	&outRect )
{
												// Start by setting the rect to the border rect
	CalcBorderRect ( outRect );
 	::MacInsetRect ( &outRect, 3, 3 );
  												// Adjust the top of the rectangle when there is
  												// a header
	if ( HasHeader ()) {
		outRect.top += 2;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcBorderRect												  [public]
// ---------------------------------------------------------------------------
//

void
LGAGroupBoxImp::CalcBorderRect (
		Rect	&outRect )
{
												// Get the frame for the box
	CalcLocalFrameRect ( outRect );
												// Calculate the border rect by offsetting its top
												// to accomodate the header, if needed
	if ( mBaseline > 0 ) {
		outRect.top += mBaseline;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
												// When the descriptor is set we simply pass it on
												// to the header to handle, if there is one, we
												// should also refresh the border of the box in case
												// the title gets longer or shorter. This also
												// involves making sure that the header object is
												// resized to fit its best rect
	if (mHeaderObject != nil) {
		mHeaderObject->SetDescriptor(inDescriptor);

												// Resize the header to its best rect
		ResizeToBestRect(mBaseline, true);
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::SetTextTraitsID(
	ResIDT	inTextTraitsID)
{
	if (mHeaderObject != nil) {
		mHeaderObject->SetTextTraitsID(inTextTraitsID);
		ResizeToBestRect(mBaseline, true);
		Refresh ();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::DrawSelf()
{
	if (HasPrimaryBorder()) {
		DrawPrimaryBoxBorder();
	} else {
		DrawSecondaryBoxBorder();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPrimaryBoxBorder										   [protected]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::DrawPrimaryBoxBorder ()
{
	StColorPenState::Normalize ();

	Rect	borderRect;
	CalcBorderRect ( borderRect );

	StClipRgnState		theClipState;

											// If the box has a header we need to set up the
											// clipping such that no border will be drawn where
											// the header is to be drawn
	if ( HasHeader ()) {
		Rect 	headerRect;
		CalcHeaderRect ( headerRect );

		theClipState.ClipToDifference(headerRect);
	}

	if ( mControlPane->GetBitDepth() < 4 ) {				// BLACK & WHITE

		StColorPenState::SetGrayPattern();					// Draw border with a dotted line
		::MacFrameRect ( &borderRect );

	} else {								// COLOR
											// Setup the colors for the border
		RGBColor	borderDarkColor  = UGAColorRamp::GetColor(4);
		RGBColor	borderLightColor = UGAColorRamp::GetColor(2);

		if (IsEnabled() && IsActive()) {
			borderDarkColor  = UGAColorRamp::GetColor(7);
			borderLightColor = Color_White;
		}

											// Frame the light portion of the border
		if ( IsEnabled ()) {
			::RGBForeColor ( &borderLightColor );
			borderRect.right -= 1;
			borderRect.bottom -= 1;
			::MacOffsetRect ( &borderRect, 1, 1 );
			::MacFrameRect ( &borderRect );
			::MacOffsetRect ( &borderRect, -1, -1 );
		}
											// Frame the dark portion of the border
		::RGBForeColor ( &borderDarkColor );
		::MacFrameRect ( &borderRect );
											// Fix up some pixels
		if ( IsEnabled ()) {
			UGraphicUtils::PaintColorPixel (
									borderRect.left,
									borderRect.bottom,
									borderLightColor );
			UGraphicUtils::PaintColorPixel (
									borderRect.right,
									borderRect.top,
									borderLightColor );
		}

											// Restore the border rect to its original size
		CalcBorderRect ( borderRect );
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSecondaryBoxBorder									   [protected]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::DrawSecondaryBoxBorder()
{
	StColorPenState::Normalize ();

	Rect	borderRect;
	CalcBorderRect ( borderRect );

	StClipRgnState		theClipState;

	RGBColor	tempColor;

											// If the box has a header we need to set up the
											// clipping such that no border will be drawn where
											// the header is to be drawn
	if ( HasHeader ()) {
		Rect 	headerRect;
		CalcHeaderRect ( headerRect );

		theClipState.ClipToDifference(headerRect);
	}

	if ( mControlPane->GetBitDepth() < 4 ) {		// BLACK & WHITE

		StColorPenState::SetGrayPattern();			// Draw border with a dotted line
		::MacFrameRect ( &borderRect );

	} else {								// COLOR
											// Setup the colors for the border
		RGBColor	borderDarkColor  = UGAColorRamp::GetColor(4);
		RGBColor	borderLightColor = UGAColorRamp::GetColor(1);

		if (IsEnabled() && IsActive()) {
			borderDarkColor  = UGAColorRamp::GetColor(7);
			borderLightColor = Color_White;
		}

											// Frame the dark portion of the border
		::RGBForeColor ( &borderDarkColor );
		UGraphicUtils::TopLeftSide ( borderRect, 0, 0, 1, 1 );

											// Frame the light portion of the border
		tempColor = IsEnabled () && IsActive () ? borderLightColor : borderDarkColor;
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( borderRect, 1, 1, 0, 0 );
	}
}


#pragma mark -
#pragma mark === HEADER CONSTRUCTION

// ---------------------------------------------------------------------------
//	¥ ResizeToBestRect											   [protected]
// ---------------------------------------------------------------------------

void
LGAGroupBoxImp::ResizeToBestRect(
	SInt16&		outBaselineOffset,
	Boolean		inRefresh)
{
									// Modify the size of the header object
									// so that it fits it's best rectangle
	if (HasHeader() && mHeaderObject->SupportsCalcBestRect()) {
		SInt16	width = 0;
		SInt16	height = 0;

		mHeaderObject->CalcBestControlRect(width, height, outBaselineOffset);
		mHeaderObject->ResizeFrameTo(width, height, inRefresh);

	} else {
		outBaselineOffset = 0;
	}
}


PP_End_Namespace_PowerPlant
