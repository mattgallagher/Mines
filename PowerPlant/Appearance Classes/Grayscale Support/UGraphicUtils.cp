// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGraphicUtils.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UGraphicUtils.h>

#include <LString.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UGAColorRamp.h>

#include <ColorPicker.h>

PP_Begin_Namespace_PowerPlant


#pragma mark === COLOR MANIPULATION

// ---------------------------------------------------------------------------
//	¥ Darken														  [static]
// ---------------------------------------------------------------------------

RGBColor
UGraphicUtils::Darken(
	const RGBColor&	inColor)
{
	RGBColor newColor;

	newColor.red   = (UInt16) (inColor.red   >> 1);
	newColor.green = (UInt16) (inColor.green >> 1);
	newColor.blue  = (UInt16) (inColor.blue  >> 1);

	return newColor;
}



// ---------------------------------------------------------------------------
// 	¥ Lighten														  [static]
// ---------------------------------------------------------------------------

RGBColor
UGraphicUtils::Lighten(
	const RGBColor&	inColor)
{
	RGBColor newColor;

	newColor.red   = (UInt16) (inColor.red   + 65535 >> 1);
	newColor.green = (UInt16) (inColor.green + 65535 >> 1);
	newColor.blue  = (UInt16) (inColor.blue  + 65535 >> 1);

	return newColor;
}


// ---------------------------------------------------------------------------
// 	¥ Lightness														  [static]
// ---------------------------------------------------------------------------
//
//	This function allows the lightness of a color to be modified by setting the
//	lightness percentage.  The percentage must be passed in as a value between
//	0 and 100, where the larger the value the lighter the color will become.
//	For a color with 100% saturation a lightness of 50% will yield the true
//	color, less than that will be darker and more will be lighter.

RGBColor
UGraphicUtils::Lightness(
	const RGBColor&	inColor,
	SInt16			inLightnessPerCent)
{
									// Convert the RGBColor to an HSLColor and then set the
									// lightnessfor the color to the percentage passed in
									// NOTE: the higher the percentage the lighter the color
	RGBColor	tempColor = inColor;
	HSLColor hslColor;
	::RGB2HSL ( &tempColor, &hslColor );
	hslColor.lightness = (UInt16) ((65535 * inLightnessPerCent) / 100);
	::HSL2RGB ( &hslColor, &tempColor );

	return tempColor;
}


#pragma mark -
#pragma mark === COLOR TRANSFORMATIONS

// ---------------------------------------------------------------------------
// 	¥ BlendGraphic													  [static]
// ---------------------------------------------------------------------------
//
// This method blends the color being painted using the OpColor and the destination
// colors, this can be used to create transforms for lightening and darkening images.
// To use it simply draw the image first then pass in a rect to this function that
// represents the area to be blended along with the color to use for the OpColor
// as well as the color that will be used to paint the rect over the image resulting
// in the blend.  Varying the two colors can change the results. Typically if you
// have an image that has the background color showing through where you don't
// want that color to be transformed then the fore color should be set to that
// color.

void
UGraphicUtils::BlendGraphic(
	const Rect&		inRect,
	const RGBColor&	inOpColor,
	const RGBColor&	inForeColor)
{
	::OpColor(&inOpColor);
	::RGBForeColor(&inForeColor);
	::PenMode(blend);
	::PaintRect(&inRect);
}


// ---------------------------------------------------------------------------
// 	¥ BlendCIconWithMask											  [static]
// ---------------------------------------------------------------------------
//
// This method blends the color being painted using the OpColor and the destination
// colors, this can be used to create transforms for lightening and darkening CIcons.
// This function uses the mask for from the cicn to handle the poainting for  the
// blend, a region is built from this mask for use when painting. The region is
// offset to the location specified by the icon rect that is passed intot this
// function, as the mask data is typically setup at 0, 0.

void
UGraphicUtils::BlendCIconWithMask(
	CIconHandle			inIconHandle,
	const Rect			&inIconRect,
	const RGBColor		&inOpColor,
	const RGBColor		&inForeColor )
{
												// Build a region that will be used as the mask
												// when drawing
	RgnHandle	maskRgn = ::NewRgn ();
	if ( maskRgn != nil && inIconHandle != nil ) {
												// First we need to setup the base address for the
												// icon's mask
		::HLock ( (Handle)inIconHandle );
		(**inIconHandle).iconMask.baseAddr = (Ptr)&(**inIconHandle).iconMaskData;

												// Now we convert the mask bitmap to a region
		::BitMapToRegion ( maskRgn, &((**inIconHandle).iconMask) );

												// Offset the region to the correct location based
												// on the rect passed in to us
		::MacOffsetRgn ( maskRgn, inIconRect.left, inIconRect.top );

												// Now handle the blending
		::OpColor ( &inOpColor );
		::RGBForeColor ( &inForeColor );
		::PenMode ( blend );
		::MacPaintRgn ( maskRgn );

												// Cleanup
		::DisposeRgn ( maskRgn );
		::HUnlock((Handle) inIconHandle);
	}
}


// ---------------------------------------------------------------------------
//	¥ InvertCIconMask
// ---------------------------------------------------------------------------

void
UGraphicUtils::InvertCIconMask(
	CIconHandle		inIconHandle,
	const Rect&		inIconRect)
{
	RgnHandle	maskRgn = ::NewRgn ();
	if ( maskRgn != nil && inIconHandle != nil ) {
												// First we need to setup the base address for the
												// icon's mask
		::HLock ( (Handle)inIconHandle );
		(**inIconHandle).iconMask.baseAddr = (Ptr)&(**inIconHandle).iconMaskData;

												// Now we convert the mask bitmap to a region
		::BitMapToRegion ( maskRgn, &((**inIconHandle).iconMask) );

												// Offset the region to the correct location based
												// on the rect passed in to us
		::MacOffsetRgn ( maskRgn, inIconRect.left, inIconRect.top );

		::MacInvertRgn(maskRgn);
												// Cleanup
		::DisposeRgn(maskRgn);
		::HUnlock((Handle) inIconHandle);
	}
}


#pragma mark -
#pragma mark === RENDERING EDGES OF RECTANGLES

// ---------------------------------------------------------------------------
// 	¥ BottomRightSide												  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::BottomRightSide (
		const Rect 	&inRect,
		SInt16 inTopOffset,
		SInt16 inLeftOffset,
		SInt16 inBottomOffset,
		SInt16 inRightOffset )
{
	Point startPoint = { (SInt16) (inRect.bottom - 1 - inBottomOffset),
						 (SInt16) (inRect.left + inLeftOffset) };

	Point endPoint = { 	(SInt16) (inRect.bottom - 1 - inBottomOffset),
						(SInt16) (inRect.right - 1 - inRightOffset) };

	Point continuePoint = { (SInt16) (inRect.top + inTopOffset),
							(SInt16) (inRect.right - 1 - inRightOffset) };

												//  Draw a line along the bottom of the rectangle
	DrawLine ( startPoint, endPoint);

												//  Then continue up the right edge of the
												// rectangle
	ContinueLine ( continuePoint );
}


// ---------------------------------------------------------------------------
// 	¥ BottomRightSideWithNotch										  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::BottomRightSideWithNotch (
		const Rect 	&inRect,
		SInt16 inTopOffset,
		SInt16 inLeftOffset,
		SInt16 inBottomOffset,
		SInt16 inRightOffset,
		SInt16 inNotch )
{
	Point startPoint = { (SInt16) (inRect.bottom - 1 - inBottomOffset),
						 (SInt16) (inRect.left + inLeftOffset) };

	Point endPoint = {	(SInt16) (inRect.bottom - 1 - inBottomOffset),
						(SInt16) (inRect.right - 1 - inRightOffset - inNotch) };

	Point continuePointUpNotch = { 	(SInt16) (inRect.bottom - 1 - inBottomOffset - inNotch),
									(SInt16) (inRect.right - 1 - inRightOffset - inNotch) };

	Point continuePointAlongNotch = { 	(SInt16) (inRect.bottom - 1 - inBottomOffset - inNotch),
										(SInt16) (inRect.right - 1 - inRightOffset) };

	Point continueToEndPoint = {	(SInt16) (inRect.top + inTopOffset),
									(SInt16) (inRect.right - 1 - inRightOffset) };

												// Draw a line along the bottom of the rectangle
												// till we reach the notch
	DrawLine ( startPoint, endPoint);

												// Then continue up the right edge of the notch
	ContinueLine ( continuePointUpNotch );

												// Then continue up the top edge of the notch
	ContinueLine ( continuePointAlongNotch );

												// Then continue up the right edge of the
												// rectangle
	ContinueLine ( continueToEndPoint );
}


// ---------------------------------------------------------------------------
// 	¥ TopLeftSide													  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::TopLeftSide (
	const Rect 	&inRect,
	SInt16 inTopOffset,
	SInt16 inLeftOffset,
	SInt16 inBottomOffset,
	SInt16 inRightOffset )
{
	Point startPoint = {	(SInt16) (inRect.bottom - 1 - inBottomOffset),
							(SInt16) (inRect.left + inLeftOffset) };

	Point endPoint = {	(SInt16) (inRect.top + inTopOffset),
						(SInt16) (inRect.left + inLeftOffset) };

	Point continuePoint = { (SInt16) (inRect.top + inTopOffset),
							(SInt16) (inRect.right - 1 - inRightOffset) };

												// Draw a line along the left edge of the rectangle
	DrawLine ( startPoint, endPoint);

												// Then continue along the top edge of the rectangle
	ContinueLine ( continuePoint );
}


// ---------------------------------------------------------------------------
// 	¥ NoCornerPixelFrame											  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::NoCornerPixelFrame (
		const Rect &inRect )
{
												// The same thing can be done by calling
												// FrameRoundRect which is probably a lot more
												// efficient
	::FrameRoundRect ( &inRect, 4, 4 );
}


// ---------------------------------------------------------------------------
// 	¥ AngledCornerFrame												  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::AngledCornerFrame(
	const Rect&		inRect,
	const RGBColor*	inColorArray,
	SInt16			inCornerOffset)
{
												// Setup some edge values
	SInt16		left = inRect.left;
	SInt16		top = inRect.top;
	SInt16		right = inRect.right;
	SInt16		bottom = inRect.bottom;

												// Setup some points for drawing routine
	Point 	startPoint;
	Point 	endPoint;

												// DRAW LINES
	::RGBForeColor ( &inColorArray[0] );
												// Draw a line along the left edge of the
												// rectangle
	startPoint.h = left;
	startPoint.v = (SInt16) (bottom - (inCornerOffset + 1));
	endPoint.h = left;
	endPoint.v = (SInt16) (top + inCornerOffset);
	DrawLine ( startPoint, endPoint);

												// Draw a line along the top edge
	startPoint.h = (SInt16) (left + inCornerOffset);
	startPoint.v = top;
	endPoint.h = (SInt16) (right - (inCornerOffset + 1));
	endPoint.v = top;
	DrawLine ( startPoint, endPoint );

												// Draw a line along the right edge
	startPoint.h = (SInt16) (right - 1);
	startPoint.v = (SInt16) (top + inCornerOffset);
	endPoint.h = (SInt16) (right - 1);
	endPoint.v = (SInt16) (bottom - (inCornerOffset + 1));
	DrawLine ( startPoint, endPoint );

												// Draw a line along the bottom edge
	startPoint.h = (SInt16) (left + inCornerOffset);
	startPoint.v = (SInt16) (bottom - 1);
	endPoint.h = (SInt16) (right - (inCornerOffset + 1));
	endPoint.v = (SInt16) (bottom - 1);
	DrawLine ( startPoint, endPoint );

												// CORNER PIXELS
	SInt16		pixelCount = (SInt16) (inCornerOffset + 1);
	for ( SInt16 index = 0; index < pixelCount; index++ ) {
												// TOP LEFT CORNER
		PaintColorPixel (	(SInt16) (left + index),
							(SInt16) (top + (inCornerOffset - index)),
							inColorArray[index + 1] );

												// TOP RIGHT CORNER
		PaintColorPixel (	(SInt16) (right - (1 + index)),
							(SInt16) (top + (inCornerOffset - index)),
							inColorArray[index + 1] );

												// BOTTOM RIGHT CORNER
		PaintColorPixel (	(SInt16) (right - (1 + index)),
							(SInt16) (bottom - (1 + (inCornerOffset - index))),
							inColorArray[index + 1] );

												// BOTTOM LEFT CORNER
		PaintColorPixel (	(SInt16) (left + index),
							(SInt16) (bottom - (1 + (inCornerOffset - index))),
							inColorArray[index + 1] );
	}
}


// ---------------------------------------------------------------------------
// 	¥ BevelledEdge													  [static]
// ---------------------------------------------------------------------------
//
//	This function is used to draw bevelled edges on either the top left or bottom right
//	edges of a rectangle.  These edges can be any width with a color being specified
//	for each edge.

void
UGraphicUtils::BevelledEdge	(
	const Rect&		inRect,
	ERectEdges		inRectEdge,
	const RGBColor*	inColorArray,
	SInt16			inEdgeWidth )
{
	Rect		r = inRect;

	switch ( inRectEdge ) {
		case rectEdges_TopLeft: {
			for ( SInt16 i = 0; i < inEdgeWidth; i++ ) {
													// Frame the bottom right edge of the rectangle
				::RGBForeColor ( &inColorArray [i] );
				TopLeftSide ( r, 0, 0, 0, 0 );

													// If we are an edge that is wider than one
													// pixel then we need to inset the rect
				if ( inEdgeWidth > 1 ) {
					::MacInsetRect ( &r, 1, 1 );
				}
			}
		}
		break;

		case rectEdges_BottomRight: {
			for ( SInt16 i = 0; i < inEdgeWidth; i++ ) {
													// Frame the bottom right edge of the rectangle
				::RGBForeColor ( &inColorArray [i] );
				BottomRightSide ( r, 1, 1, 0, 0 );

													// If we are an edge that is wider than one
													// pixel then we need to inset the rect
				if ( inEdgeWidth > 1 ) {
					::MacInsetRect ( &r, 1, 1 );
				}
			}
		}
		break;
		
		default:
		break;
	}
}


// ---------------------------------------------------------------------------
// 	¥ PaintCornerPixels												  [static]
// ---------------------------------------------------------------------------
//
//	This function is used to paint in the corner pixels of a rectangle, the corner to be
//	painted is specified by an enum for each of the corners.  The function supports
//	painting up to two pixels and allows spearate colors to be specifed for each of
//	these pixels.

void
UGraphicUtils::PaintCornerPixels	(
	const Rect&		inRect,
	ERectCorners	inCornerToPaint,
	const RGBColor*	inCornerColorArray,
	SInt16			inEdgeWidth )
{
	Rect		r = inRect;

	switch (inCornerToPaint) {
											// TOP LEFT CORNER
		case rectCorners_TopLeft: {
			for (SInt16 i = 0; i < inEdgeWidth; i++) {
												// Paint the top left pixel
				PaintColorPixel(r.left, r.top, inCornerColorArray[i]);

												// If we are a corner that is wider than one pixel
												// then we need to inset the rect
				if (inEdgeWidth > 1) {
					::MacInsetRect(&r, 1, 1);
				}
			}
		}
		break;

											// TOP RIGHT CORNER
		case rectCorners_TopRight: {
			for (SInt16 i = 0; i < inEdgeWidth; i++) {
												// Paint the top right pixel
				PaintColorPixel((SInt16) (r.right - 1), r.top, inCornerColorArray[i]);

												// If we are a corner that is wider than one pixel
												// then we need to inset the rect
				if (inEdgeWidth > 1) {
					::MacInsetRect(&r, 1, 1);
				}
			}
		}
		break;

											// BOTTOM RIGHT CORNER
		case rectCorners_BottomRight: {
			for (SInt16 i = 0; i < inEdgeWidth; i++) {
												// Paint the bottom right pixel
				PaintColorPixel((SInt16) (r.right - 1), (SInt16) (r.bottom - 1), inCornerColorArray[i]);

												// If we are a corner that is wider than one pixel
												// then we need to inset the rect
				if (inEdgeWidth > 1) {
					::MacInsetRect(&r, 1, 1);
				}
			}
		}
		break;

											// BOTTOM LEFT CORNER
		case rectCorners_BottomLeft: {
			for (SInt16 i = 0; i < inEdgeWidth; i++) {
												// Paint the bottom left pixel
				PaintColorPixel( r.left, (SInt16) (r.bottom - 1), inCornerColorArray[i]);

												// If we are a corner that is wider than one pixel
												// then we need to inset the rect
				if (inEdgeWidth > 1) {
					::MacInsetRect(&r, 1, 1);
				}
			}
		}
		break;
		
		default:
		break;
	}
}


// ---------------------------------------------------------------------------
// 	¥ RoundCornerEdges												  [static]
// ---------------------------------------------------------------------------
//

void
UGraphicUtils::RoundCornerEdges	(
	const Rect&			inRect,
	ERectEdges			inRectEdge,
	const RGBColor&		inEdgeColor )
{
	Point startPoint, endPoint;
												// Get the foreground color setup
	::RGBForeColor ( &inEdgeColor );
												//	Draw the approrpiate edge of the rectangle
	switch ( inRectEdge ) {
		case rectEdges_TopLeft: {
			startPoint.h = (SInt16) (inRect.left + 1);
			startPoint.v = (SInt16) (inRect.bottom - 5);
			endPoint.h = (SInt16) (inRect.left + 1);
			endPoint.v = (SInt16) (inRect.top + 3);
       	DrawLine ( startPoint, endPoint );

 			endPoint.h = (SInt16) (inRect.left + 2);
			endPoint.v = (SInt16) (inRect.top + 3);
       	ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.left + 2);
			endPoint.v = (SInt16) (inRect.top + 2);
	      ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.left + 3);
			endPoint.v = (SInt16) (inRect.top + 2);
	      ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.left + 3);
			endPoint.v = (SInt16) (inRect.top + 1);
       	ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.right - 4);
			endPoint.v = (SInt16) (inRect.top + 1);
       	ContinueLine ( endPoint );
      }
		break;

		case rectEdges_BottomRight: {
			startPoint.h = (SInt16) (inRect.left + 3);
			startPoint.v = (SInt16) (inRect.bottom - 2);
			endPoint.h = (SInt16) (inRect.right - 4);
			endPoint.v = (SInt16) (inRect.bottom - 2);
       	DrawLine ( startPoint, endPoint );

 			endPoint.h = (SInt16) (inRect.right - 4);
			endPoint.v = (SInt16) (inRect.bottom - 3);
       	ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.right - 3);
			endPoint.v = (SInt16) (inRect.bottom - 3);
	      ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.right - 3);
			endPoint.v = (SInt16) (inRect.bottom - 4);
	      ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.right - 2);
			endPoint.v = (SInt16) (inRect.bottom - 4);
       	ContinueLine ( endPoint );

 			endPoint.h = (SInt16) (inRect.right - 2);
			endPoint.v = (SInt16) (inRect.top + 3);
       	ContinueLine ( endPoint );
		}
		break;
		
		default:
		break;
	}
}


#pragma mark -
#pragma mark === GRAPHICS FUNCTIONS

// ---------------------------------------------------------------------------
// 	¥ DrawLine														  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::DrawLine(
	Point inStartLoc,
	Point inEndLoc)
{
											  // Move to the starting location then draw the line
											  // to the end location
  ::MoveTo ( inStartLoc.h, inStartLoc.v );
  ::MacLineTo ( inEndLoc.h, inEndLoc.v );
}


// ---------------------------------------------------------------------------
// 	¥ ContinueLine													  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::ContinueLine(
	Point	inEndLoc)
{
							  	// Continue drawing the line from the current
							  	// location of the pen to the end location
  ::MacLineTo(inEndLoc.h, inEndLoc.v);
}


// ---------------------------------------------------------------------------
// 	¥ PaintColorPixel												  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::PaintColorPixel(
	SInt16				inHorizLoc,
	SInt16				inVertLoc,
	const RGBColor&		inColor)
{
  								// We will simply paint the pixel by calling MacLineTo
	::RGBForeColor(&inColor);
	::MoveTo(inHorizLoc, inVertLoc);
	::MacLineTo(inHorizLoc, inVertLoc);
}


// ---------------------------------------------------------------------------
//	¥ DrawCIconGraphic
// ---------------------------------------------------------------------------

void
UGraphicUtils::DrawCIconGraphic(
	CIconHandle		inCIconH,
	const Rect&		inRect,
	SInt16			inTransform,
	SInt16			inBitDepth)
{
	if (inCIconH != nil) {
		StColorPenState	savePenState;
		StColorPenState::Normalize ();

		::PlotCIcon(&inRect, inCIconH);		// Draw color icon

		RGBColor	opColor;
		RGBColor	foreColor;

		if (inTransform == kTransformSelected) {

			if (inBitDepth > 4) {			// Color. Darken icon
				opColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::BlendCIconWithMask(inCIconH, inRect,
													opColor, Color_Black);

			} else {						// Black & White
				UGraphicUtils::InvertCIconMask(inCIconH, inRect);
			}

		} else if (inTransform == kTransformDisabled) {

			if (inBitDepth > 4) {			// Color. Lighten icon
				opColor   = UGAColorRamp::GetColor(colorRamp_Gray7);
				foreColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				UGraphicUtils::BlendCIconWithMask(inCIconH, inRect,
													opColor, foreColor);

			} else {						// Black & White
				::PenMode(patBic);			// Mask with a gray pattern
				StColorPenState::SetGrayPattern();
				::PaintRect(&inRect);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPictGraphic
// ---------------------------------------------------------------------------

void
UGraphicUtils::DrawPictGraphic(
	PicHandle		inPictH,
	const Rect&		inRect,
	SInt16			inTransform,
	SInt16			inBitDepth)
{
	if (inPictH != nil) {
		StColorPenState	savePenState;
		StColorPenState::Normalize ();

		::DrawPicture(inPictH, &inRect);

		RGBColor	opColor;
		RGBColor	foreColor;

		if (inTransform == kTransformSelected) {

			if (inBitDepth > 4) {			// Color. Darken picture
				opColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::BlendGraphic(inRect, opColor, Color_Black);

			} else {						// Black & White
				::MacInvertRect(&inRect);
			}

		} else if (inTransform == kTransformDisabled) {

			if (inBitDepth > 4) {			// Color. Lighten picture
				opColor   = UGAColorRamp::GetColor(colorRamp_Gray7);
				foreColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				UGraphicUtils::BlendGraphic(inRect, opColor, foreColor);

			} else {						// Black & White
				::PenMode(patBic);			// Mask with a gray pattern
				StColorPenState::SetGrayPattern();
				::PaintRect(&inRect);
			}
		}
	}
}

#pragma mark -
#pragma mark === MISCELLANEOUS FUNCTIONS

// ---------------------------------------------------------------------------
// 	¥ LocateDeviceRect												  [static]
// ---------------------------------------------------------------------------

Rect
UGraphicUtils::LocateDeviceRect(
	Point	inMouseLoc)
{
	Rect		deviceRect = Rect_0000;
	GDHandle	theDevice  = ::GetDeviceList();

	while (theDevice != nil) {

		if (UDrawingUtils::IsActiveScreenDevice(theDevice)) {

			Rect 	r = (**theDevice).gdRect;

	  		if (::MacPtInRect(inMouseLoc, &r)) {
	  			deviceRect = r;
	  			break;
	  		}
	  	}

  		theDevice = ::GetNextDevice(theDevice);
	}

	return deviceRect;
}


// ---------------------------------------------------------------------------
// 	¥ RangeRandom													  [static]
// ---------------------------------------------------------------------------

UInt16
UGraphicUtils::RangeRandom(
	UInt16		inMin,
	UInt16		inMax)
{
	UInt16		rand;				// Treat the return value as 0 - 65536
	SInt32		range, temp;

	rand = (UInt16) ::Random ();
	range = inMax - inMin;
	temp = (rand * range ) / 65536;

	return (UInt16) (temp + inMin);
}


// ---------------------------------------------------------------------------
// 	¥ SlowDown														  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::SlowDown(
	UInt32		inDelayTicks)
{
	unsigned long dummy;
	::Delay(inDelayTicks, &dummy);
}


// ---------------------------------------------------------------------------
// 	¥ CreateGrafPort 												  [static]
// ---------------------------------------------------------------------------

GrafPtr
UGraphicUtils::CreateGrafPort(
	const Rect	&inBounds)
{
#if PP_Target_Carbon
													// Save current port and restore it
													// at the end
	StGrafPortSaver	savePort;

	Rect	localBounds = inBounds;					// Local copy of bounds

													// Set the top-left corner of bounds to (0,0)
	::MacOffsetRect(&localBounds, (SInt16) -inBounds.left, (SInt16) -inBounds.top);

													// Allocate a new GrafPort. CreateNewPort
													// should make the newPort the current port.
	CGrafPtr	newPort = CreateNewPort();
	if (newPort != nil) {
													// Initialize and allocate the bitmap
		::SetPortBounds(newPort, &localBounds);
		// no idea how to set the rowBytes nor baseAddr correctly

		::ClipRect(&localBounds);
		StRegion	visRgn;
		::GetPortVisibleRegion(newPort, visRgn);

		Rect		visBounds;
		visRgn.GetBounds(visBounds);
		::EraseRect(&visBounds);
	}

	return newPort;

#else

	GrafPtr	savedPort;						// Saved GrafPtr for later restore
	GrafPtr	newPort;							// New GrafPort
	Rect	localBounds;						// Local copy of bounds

	::GetPort ( &savedPort );

													// Set the top-left corner of bounds to (0,0)
	localBounds = inBounds;
	::MacOffsetRect ( &localBounds, (SInt16) (-inBounds.left), (SInt16) (-inBounds.top) );

													// Allocate a new GrafPort
	newPort = (GrafPtr)::NewPtrClear ( sizeof( GrafPort ) );

	if ( newPort != nil ) {
													// Initialize the new port and make the current
													// port
		::OpenPort( newPort );
													// Initialize and allocate the bitmap
		newPort->portBits.bounds = localBounds;
  		newPort->portBits.rowBytes = (SInt16) (((localBounds.right + 15) >> 4) << 1);
		newPort->portBits.baseAddr =  ::NewPtrClear( newPort->portBits.rowBytes *
													(long)localBounds.bottom );
		if ( newPort->portBits.baseAddr != nil ) {
													// Clean up the new port
			newPort->portRect = localBounds;
			::ClipRect ( &localBounds );
			::RectRgn ( newPort->visRgn, &localBounds );
			::EraseRect ( &localBounds );

		} else {
													// Allocation failed; deallocate the port
			::ClosePort ( newPort );
			::DisposePtr ( (Ptr)newPort );
			newPort = nil;
		}
	}

	::MacSetPort(savedPort);
	return newPort;

#endif
}



// ---------------------------------------------------------------------------
// 	¥ DisposeGrafPort 												  [static]
// ---------------------------------------------------------------------------

void
UGraphicUtils::DisposeGrafPort(
	GrafPtr		inDoomedPort)
{
#if PP_Target_Carbon

	::DisposePort(inDoomedPort);

#else

	::ClosePort(inDoomedPort);
	::DisposePtr(inDoomedPort->portBits.baseAddr);
	::DisposePtr((Ptr)inDoomedPort);

#endif
}


PP_End_Namespace_PowerPlant
