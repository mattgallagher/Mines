// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAppearance.cp				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper functions for Appearance Manager calls
//
//	PP needs to call some AM 1.1 routines in order to draw properly
//	with Themes. However, we want PP to run on machines without AM 1.1.
//	Therefore, we have wrapper functions with the same name but within
//	a UAppearance namespace.
//
//	The wrapper function calls the Toolbox routine if running under AM 1.1
//	or later. Under AM 1.0.x, the wrapper function implements the standard
//	Platinum Theme.
//
//	To prevent Linker errors, we use conditional compilation to exclude the
//	call for a 68K target. AM 1.1 doesn't exist on 68K. There is no stub
//	library to weak link, so we can't have the function call in the
//	compiled code.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UAppearance.h>
#include <UDrawingState.h>
#include <UEnvironment.h>


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Colors used in Platinum Appearance

const RGBColor	Color_PlatinumDimText	 = { 29555, 29555, 29555 };
const RGBColor	Color_PlatinumMediumGray = { 57054, 57054, 57054 };
const RGBColor	Color_PlatinumLightGray  = { 61423, 61423, 61423 };


// ---------------------------------------------------------------------------
//	¥ GetThemeDrawingState
// ---------------------------------------------------------------------------

OSStatus
UAppearance::GetThemeDrawingState(
	ThemeDrawingState*	outState)
{
	OSStatus	status = errMessageNotSupported;
	*outState = nil;
	
	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::GetThemeDrawingState(outState);
	}

	#endif										// Do nothing if no AM 1.1

	return status;
}


// ---------------------------------------------------------------------------
//	¥ SetThemeDrawingState
// ---------------------------------------------------------------------------

OSStatus
UAppearance::SetThemeDrawingState(
	ThemeDrawingState	inState,
	Boolean				inDisposeNow)
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::SetThemeDrawingState(inState, inDisposeNow);
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inState, inDisposeNow)
	#endif

	return status;
}


// ---------------------------------------------------------------------------
//	¥ NormalizeThemeDrawingState
// ---------------------------------------------------------------------------

OSStatus
UAppearance::NormalizeThemeDrawingState()
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::NormalizeThemeDrawingState();
	}

	#endif										// Do nothing if no AM 1.1

	return status;
}


// ---------------------------------------------------------------------------
//	¥ DisposeThemeDrawingState
// ---------------------------------------------------------------------------

OSStatus
UAppearance::DisposeThemeDrawingState(
	ThemeDrawingState	inState)
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::DisposeThemeDrawingState(inState);
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inState)
	#endif

	return status;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetThemeTextColor
// ---------------------------------------------------------------------------

OSStatus
UAppearance::GetThemeTextColor(
	ThemeTextColor		inThemeColor,
	SInt16				inBitDepth,
	Boolean				inHasColor,
	Boolean				inIsActive,
	RGBColor*			outColor)
{
	OSStatus	status = noErr;

	#if PP_Uses_Appearance11_Routines
	
	if (UEnvironment::HasFeature(env_HasAppearance11)) {

		status = ::GetThemeTextColor(inThemeColor, inBitDepth,
									inHasColor, outColor);

	} else

	#else
		#pragma unused(inThemeColor)
	#endif
								// No AM 1.1. Use colors for Platinum Theme.
	{
		if (inIsActive || (inBitDepth < 4) || !inHasColor) {
			*outColor = Color_Black;

		} else {					// Inactive text on a color device
									//   is gray
			*outColor = Color_PlatinumDimText;
		}
	}

	return status;
}


// ---------------------------------------------------------------------------
//	¥ SetThemeBackground
// ---------------------------------------------------------------------------
//	The Toolbox version of SetThemeBackground, despite what some documentation
//	says, does not align the pattern to the port origin. It uses the current
//	pattern alignment, which is a private QuickDraw setting.
//
//	PowerPlant programs typically set all relevant QuickDraw state data
//	before drawing. To avoid alignment problems, this version first calls
//	ApplyThemeBackground to align the pattern to (0,0), then calls
//	SetThemeBackground.

OSStatus
UAppearance::SetThemeBackground(
	ThemeBrush	inBrush,
	SInt16		inBitDepth,
	Boolean		inHasColor)
{
	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
	
			// ApplyThemeBackground changes the pattern origin to
			// align with the input bounding box. The values for
			// the ThemeBrush and ThemeState don't matter, as the
			// subsequent call to SetThemeBackground will set
			// the background.
	
		Rect	originBox = { 0, 0, 1, 1 };

		::ApplyThemeBackground(kThemeBackgroundTabPane, &originBox,
							   kThemeStateInactive, inBitDepth, inHasColor);
	}
	
	#endif
	
	return ::SetThemeBackground(inBrush, inBitDepth, inHasColor);
}


// ---------------------------------------------------------------------------
//	¥ ApplyThemeBackground
// ---------------------------------------------------------------------------

OSStatus
UAppearance::ApplyThemeBackground(
	ThemeBackgroundKind 	inThemeKind,
	const Rect*				inBounds,
	ThemeDrawState 			inState,
	SInt16 					inBitDepth,
	Boolean 				inHasColor)
{
	OSStatus	status = noErr;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {

		status = ::ApplyThemeBackground(inThemeKind, inBounds, inState,
										inBitDepth, inHasColor);

	} else

	#else
		#pragma unused(inBounds)
	#endif
								// No AM 1.1. Use colors for Platinum Theme.
	{
			// There is a Black & White variation for Platinum Theme.
			// Under AM 1.0.1, we can check for this by calling
			// IsThemeInColor(). Unfortunately, there isn't a simple
			// way to check this under AM 1.0 (which shipped with
			// Mac OS 8.0) so the background color might not be correct
			// under AM 1.0.

		if (UEnvironment::HasFeature(env_HasAppearance101)) {

			inHasColor = ::IsThemeInColor(inBitDepth, inHasColor);
		}

		if ((inBitDepth < 4)  ||  !inHasColor) {	// Black & White

			::RGBBackColor(&Color_White);

		} else {									// Color

			switch (inThemeKind) {

				case kThemeBackgroundTabPane:
					::RGBBackColor(&Color_PlatinumLightGray);
					break;

				case kThemeBackgroundPlacard:
					if (inState == kThemeStateInactive) {
						::RGBBackColor(&Color_PlatinumLightGray);
					} else {
						::RGBBackColor(&Color_PlatinumMediumGray);
					}
					break;

				case kThemeBackgroundWindowHeader:
				case kThemeBackgroundListViewWindowHeader:

						// WindowHeader color depends on AM version. Under
						// AM 1.1 (or later), inactive WindowHeaders draw
						// in light gray. Prior to AM 1.1, they draw in
						// medium gray.
						//
						// AM 1.1 might exist on the System even if the
						// AM 1.1 routines aren't compiled into the code
						// of this program. That happens if this is a 68K
						// target running under emulation on a PPC.

					#if ! PP_Uses_Appearance11_Routines
						if ( UEnvironment::HasFeature(env_HasAppearance11)  &&
							 (inState == kThemeStateInactive) ) {

							::RGBBackColor(&Color_PlatinumLightGray);
						} else
					#endif

					{
						::RGBBackColor(&Color_PlatinumMediumGray);
					}
					break;

				default:
					status = paramErr;
					break;
			}
		}
	}

	return status;
}


// ---------------------------------------------------------------------------
//	¥ SetControlViewSize
// ---------------------------------------------------------------------------

void
UAppearance::SetControlViewSize(
	ControlHandle	inControlH,
	SInt32			inViewSize)
{
	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		::SetControlViewSize(inControlH, inViewSize);
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inControlH, inViewSize)
	#endif
}


// ---------------------------------------------------------------------------
//	¥ SetWindowContentBrush
// ---------------------------------------------------------------------------

OSStatus
UAppearance::SetWindowContentBrush(
	WindowPtr	inMacWindowP,
	ThemeBrush	inBrush,
	SInt16		inBitDepth,
	Boolean		inHasColor)
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		RGBColor	winColor;
		OSStatus	status = ::GetThemeBrushAsColor(inBrush, inBitDepth,
													inHasColor, &winColor);

		if (status == noErr) {
			::SetWindowContentColor(inMacWindowP, &winColor);
		}
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inMacWindowP, inBrush, inBitDepth, inHasColor)
	#endif
	
	return status;
}


// ---------------------------------------------------------------------------
//	¥ GetControlRegion
// ---------------------------------------------------------------------------

OSStatus
UAppearance::GetControlRegion(
	ControlHandle		inMacControlH,
	ControlPartCode		inPart,
	RgnHandle			outRegionH)
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::GetControlRegion(inMacControlH, inPart, outRegionH);
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inMacControlH, inPart, outRegionH)
	#endif

	return status;
}


// ---------------------------------------------------------------------------
//	¥ GetThemeButtonBackgroundBounds
// ---------------------------------------------------------------------------

OSStatus
UAppearance::GetThemeButtonBackgroundBounds(
	const Rect*					inControlRect,
	ThemeButtonKind				inKind,
	const ThemeButtonDrawInfo*	inDrawInfo,
	Rect*						outBounds)
{
	OSStatus	status = errMessageNotSupported;

	#if PP_Uses_Appearance11_Routines

	if (UEnvironment::HasFeature(env_HasAppearance11)) {
		status = ::GetThemeButtonBackgroundBounds(inControlRect, inKind,
													inDrawInfo, outBounds);
	}

	#else										// Do nothing if no AM 1.1
		#pragma unused(inControlRect, inKind, inDrawInfo, outBounds)
	#endif

	return status;
}


// ---------------------------------------------------------------------------
//	¥ DrawThemeGroupBorder
// ---------------------------------------------------------------------------
//	Draw a primary or secondary theme group box border
//
//	Caller must set up proper background for erasing

void
UAppearance::DrawThemeGroupBorder(
	const Rect*		inRect,
	ThemeDrawState	inState,
	bool			inIsPrimary)
{
		// On Aqua, the drawing of the group box uses an additive
		// mode, where repeated calls to draw the same frame create
		// successively darker borders. So we need to erase the border
		// area before drawing. We assume the border can draw one pixel
		// on either side of the frame.

	if (UEnvironment::HasFeature(env_HasAquaTheme)) {
		Rect	border = *inRect;
		::MacInsetRect(&border, -1, -1);
		
		StRegion	borderRgn = border;
		
		::MacInsetRect(&border, 2, 2);
		borderRgn -= border;
		
		::EraseRgn(borderRgn);
	}
	
	if (inIsPrimary) {
		::DrawThemePrimaryGroup(inRect, inState);
	} else {
		::DrawThemeSecondaryGroup(inRect, inState);
	}
}


PP_End_Namespace_PowerPlant
