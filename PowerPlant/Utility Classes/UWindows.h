// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UWindows.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UWindows
#define _H_UWindows
#pragma once

#include <PP_Prefix.h>
#include <MacWindows.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace	UWindows {

	Rect			GetWindowContentRect( WindowPtr inWindowP );

	Rect			GetWindowStructureRect( WindowPtr inWindowP );

	GDHandle		FindDominantDevice( const Rect &inGlobalRect );

	WindowPtr		FindNthWindow( SInt16 inN );

	SInt16			FindWindowIndex( WindowPtr inWindowP );

	WindowPtr		FindNamedWindow( Str255	 inName );

	bool			GetColorTableEntry(
							CTabHandle	inColorTableH,
							SInt16		inEntryValue,
							RGBColor&	outColor);
							
	void			ExcludeStructuresAbove(
							WindowPtr	inWindowP,
							RgnHandle	ioRegion);
							
	void			ExcludeActiveStructures( RgnHandle ioRegion );						
							
	void			CenterBoundsOnRect(
							Rect&		ioBounds,
							const Rect&	inRect);
							
	void			AlertBoundsOnRect(
							Rect&		ioBounds,
							const Rect&	inRect);
							
	void			PositionBoundsOnScreen(
							Rect&		ioBounds,
							GDHandle	inScreen);
							
	bool			BoundsAreOutsideRect(
							SInt16		inLeft,
							SInt16		inTop,
							SInt16		inWidth,
							SInt16		inHeight,
							const Rect&	inRect);
							
	bool			BoundsAreTooFarOffscreen(
							SInt16		inLeft,
							SInt16		inTop,
							SInt16		inWidth,
							SInt16		inHeight,
							const Rect&	inRect);

#if PP_Target_Carbon
							
	OSStatus		RepositionWindow(
							WindowRef				inWindow,
							WindowRef				inParentWindow,
							WindowPositionMethod	inPosMethod);
							
	void			CascadeBoundsOnScreen(
							Rect&		ioBounds,
							const Rect&	inScreenRect,
							Point		inOffset,
							WindowClass	inWindClass);
							
	void			CascadeBoundsAtLocation(
							Rect&		ioBounds,
							Point		inLocation,
							const Rect&	inScreenRect,
							Point		inOffset,
							WindowClass	inWindClass);
							
	bool			TooManyWindowsAtLocation(
							SInt16		inLeft,
							SInt16		inTop,
							WindowClass	inWindClass,
							SInt16		inMaxWindows);
							
#endif // PP_Target_Carbon
}

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
