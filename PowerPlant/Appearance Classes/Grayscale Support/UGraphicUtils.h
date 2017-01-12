// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGraphicUtils.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_UGraphicUtils
#define _H_UGraphicUtils
#pragma once

#include <UEnvironment.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

enum ERectCorners {
	rectCorners_None,
	rectCorners_TopLeft,
	rectCorners_TopRight,
	rectCorners_BottomRight,
	rectCorners_BottomLeft
};

enum ERectEdges {
	rectEdges_None,
	rectEdges_TopLeft,
	rectEdges_BottomRight
};


	//	Inlines for finding the min or max of two longs
inline long GAUtils_LMax ( long a, long b ) { return a > b ? a : b; }
inline long GAUtils_LMin ( long a, long b ) { return a < b ? a : b; }

	// Our very own abs function so we don't need to
	// depend on the standard library for it

inline SInt16 GAUtils_Absolute(SInt16 n)
{
	if (n < 0) {
		n = (SInt16) -n;
	}

	return n;
}


// ---------------------------------------------------------------------------

class	UGraphicUtils {
public:
	static	RGBColor 		Darken( const RGBColor& inColor );

	static	RGBColor 		Lighten( const RGBColor& inColor );

	static	RGBColor 		Lightness(
									const RGBColor&		inColor,
									SInt16				inLightnessPerCent);


								//  Color Transformations

	static	void			BlendGraphic(
									const Rect&			inRect,
									const RGBColor&		inOpColor,
									const RGBColor&		inForeColor);

	static	void			BlendCIconWithMask(
									CIconHandle			inIconHandle,
									const Rect&			inIconRect,
									const RGBColor&		inOpColor,
									const RGBColor&		inForeColor);

	static	void			InvertCIconMask(
									CIconHandle			inIconHandle,
									const Rect&			inIconRect);


								//  Graphical Functions

	static	void 			DrawLine(
									Point				inStartLoc,
            						Point				inEndLoc);

	static	void 			ContinueLine( Point inEndLoc );

	static	void 			PaintColorPixel(
									SInt16				inHorizLoc,
									SInt16				inVertLoc,
									const RGBColor&		inColor);

	static void				DrawCIconGraphic(
									CIconHandle			inCIconH,
									const Rect&			inRect,
									SInt16				inTransform,
									SInt16				inBitDepth);

	static void				DrawPictGraphic(
									PicHandle			inPictH,
									const Rect&			inRect,
									SInt16				inTransform,
									SInt16				inBitDepth);


								//  Specialized Graphical Functions

	static	void			BottomRightSide(
									const Rect&			inRect,
		          					short				inTopOffset,
		           					short				inLeftOffset,
		           					short				inBottomOffset,
		           					short				inRightOffset);

	static	void			BottomRightSideWithNotch(
									const Rect&			inRect,
		          					short				inTopOffset,
		           					short				inLeftOffset,
		           					short				inBottomOffset,
		           					short				inRightOffset,
				           			short				inNotch);

	static	void			TopLeftSide(
									const Rect&			inRect,
		          					short				inTopOffset,
		           					short				inLeftOffset,
		           					short				inBottomOffset,
		           					short				inRightOffset);

	static	void			AngledCornerFrame(
									const Rect&			inRect,
									const RGBColor*		inColorArray,
									SInt16				inCornerOffset);

	static	void			NoCornerPixelFrame( const Rect& inRect );

	static	void			BevelledEdge (
									const Rect&			inRect,
									ERectEdges 			inRectEdge,
									const RGBColor*		inColorArray,
									SInt16				inEdgeWidth = 2);

	static	void			PaintCornerPixels	(
									const Rect&			inRect,
									ERectCorners		inCornerToPaint,
									const RGBColor*		inCornerColorArray,
									SInt16				inEdgeWidth = 2);

	static	void			RoundCornerEdges (
									const Rect&			inRect,
									ERectEdges			inRectEdge,
									const RGBColor&		inEdgeColor);


								//  Miscellaneous

	static	bool			HasAppearance()
								{
									return UEnvironment::HasFeature ( env_HasAppearance );
								}

	static	bool			HasAppearanceCompatMode()
								{
									return UEnvironment::HasFeature ( env_HasAppearanceCompat );
								}

	static	bool			IsAppearanceRunning()
								{
									return UEnvironment::IsAppearanceRunning ();
								}

	static	bool			HasMacOS8()
								{
									return (UEnvironment::GetOSVersion () >= 0x0800);
								}

	static	Rect			LocateDeviceRect( Point inMouseLoc );

	static	SInt16			RectWidth( const Rect& inRect )
								{
									return (SInt16) (inRect.right - inRect.left);
								}

	static	SInt16			RectHeight( const Rect& inRect )
								{
									return (SInt16) (inRect.bottom - inRect.top);
								}

	static	UInt16			RangeRandom(
									UInt16 inMin,
									UInt16 inMax);

	static	void			SlowDown( UInt32 inDelayTicks );

	static	GrafPtr			CreateGrafPort( const Rect& inBounds );

	static	void			DisposeGrafPort( GrafPtr inDoomedPort );
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
