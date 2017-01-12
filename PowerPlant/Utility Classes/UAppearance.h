// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAppearance.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper functions for Appearance Manager 1.1 calls

#ifndef _H_UAppearance
#define _H_UAppearance
#pragma once

#include <PP_Prefix.h>
#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Option for using Appearanace Manager 1.1 Routines
//
//		MacOS 68K machines don't support AM 1.1

#if (TARGET_OS_MAC && TARGET_CPU_68K)
	#define PP_Uses_Appearance11_Routines	0
#else
	#define PP_Uses_Appearance11_Routines	1
#endif


// ---------------------------------------------------------------------------

namespace UAppearance {

	OSStatus	GetThemeDrawingState(
						ThemeDrawingState*		outState);

	OSStatus	SetThemeDrawingState(
						ThemeDrawingState		inState,
						Boolean					inDisposeNow);
						
	OSStatus	NormalizeThemeDrawingState();
	
	OSStatus	DisposeThemeDrawingState( ThemeDrawingState inState );

	OSStatus	GetThemeTextColor(
						ThemeTextColor			inThemeColor,
						SInt16					inBitDepth,
						Boolean					inHasColor,
						Boolean					inIsActive,
						RGBColor*				outColor);
						
	OSStatus	SetThemeBackground(
						ThemeBrush				inBrush,
						SInt16 					inBitDepth,
						Boolean 				inHasColor);

	OSStatus	ApplyThemeBackground(
						ThemeBackgroundKind 	inThemeKind,
						const Rect*				inBounds,
						ThemeDrawState 			inState,
						SInt16 					inBitDepth,
						Boolean 				inHasColor);

	void		SetControlViewSize(
						ControlHandle			inControlH,
						SInt32					inViewSize);

	OSStatus	SetWindowContentBrush(
						WindowPtr				inMacWindowP,
						ThemeBrush				inBrush,
						SInt16					inBitDepth,
						Boolean					inHasColor);

	OSStatus	GetControlRegion(
						ControlHandle			inMacControlH,
						ControlPartCode			inPart,
						RgnHandle				outRegionH);
						
	OSStatus	GetThemeButtonBackgroundBounds(
						const Rect*					inControlRect,
						ThemeButtonKind				inKind,
						const ThemeButtonDrawInfo*	inDrawInfo,
						Rect*						outBounds);
						
	void		DrawThemeGroupBorder(
						const Rect*				inRect,
						ThemeDrawState			inState,
						bool					inIsPrimary);
}


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
