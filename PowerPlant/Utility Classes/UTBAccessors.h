// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTBAccessors.h				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	Accessor functions for Toolbox data structures that are opaque in Carbon
//
//	In Apple's headers, accessor functions are define when the symbol
//	ACCESSOR_CALLS_ARE_FUNCTIONS is 1 (true). By default, this is true
//	for Carbon targets. For simplicity in maintaining a single source base for
//	Carbon and Classic targets, we define equivalent inline functions
//	when the Toolbox doesn't define them.
//
//	Also, when OPAQUE_TOOLBOX_STRUCTS is 1 (true), CGrafPtr and GrafPtr are
//	the same type. They are different types when that symbol is false.
//	Apple's headers use CGrafPtr parameter types, so we define overloaded
//	functions that take GrafPtr paraemters when types are not opaque.
//
//	Below is a chart which explains how these flags may be set:
//
//									  Carbon	Transition	Illegal	Classic
//		ACCESSOR_CALLS_ARE_FUNCTIONS	1			1			0		0
//		OPAQUE_TOOLBOX_STRUCTS			1			0			1		0
//
//	For Carbon targets, both MUST be true. Do not set them yourself. For
//	Classic targets, both are false by default.
//
//	As a transition strategy for updating your Classic code you can define
//	ACCESSOR_CALLS_ARE_FUNCTIONS to true so that you can use the accessor
//	functions defined in Apple's header. If you do this, you must add
//	CarbonAccessors.o to your Classic targets.
//
//	This file defines the accessor functions used by PowerPlant code. This
//	allows the same code to compile for the Carbon, Trasition, and Classic
//	cases.
//
//	Note: This file does not depend on PowerPlant (it includes no PP headers).

#ifndef _H_UTBAccessors
#define _H_UTBAccessors
#pragma once

#include <ConditionalMacros.h>

	// Define accessor function for the "Transition" and "Classic"
	// situations as described in the above table

#if !ACCESSOR_CALLS_ARE_FUNCTIONS || !OPAQUE_TOOLBOX_STRUCTS

#include <Controls.h>
#include <Lists.h>
#include <LowMem.h>
#include <Menus.h>
#include <QuickDraw.h>
#include <MacWindows.h>

// ===========================================================================
//	Accessor Functions
// ===========================================================================

#if !ACCESSOR_CALLS_ARE_FUNCTIONS

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Control Manager

inline
Rect*			GetControlBounds( ControlHandle control, Rect* bounds )
					{
						*bounds = (**control).contrlRect;
						return bounds;
					}

inline
void			SetControlBounds( ControlHandle control, const Rect* bounds )
					{
						(**control).contrlRect = *bounds;
					}

inline
WindowPtr		GetControlOwner( ControlHandle control )
					{
						return (**control).contrlOwner;
					}

inline
Boolean			IsControlHilited( ControlHandle control )
					{
						return (**control).contrlHilite != 255;
					}
					
inline
UInt16			GetControlHilite( ControlHandle control )
					{
						return (**control).contrlHilite;
					}

#pragma mark -
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	List Manager

inline
ControlHandle	GetListVerticalScrollBar( ListRef list )
					{
						return (**list).vScroll;
					}

inline
ControlHandle	GetListHorizontalScrollBar( ListRef list )
					{
						return (**list).hScroll;
					}

inline
Rect*			GetListViewBounds( ListRef list, Rect* view )
					{
						*view = (**list).rView;
						return view;
					}

#pragma mark -
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Menu Manager

typedef SInt16		MenuID;		// Define for UH 3.2

inline
MenuID			GetMenuID( MenuRef menu )
					{
						return (**menu).menuID;
					}

inline
SInt16			GetMenuWidth( MenuRef menu )
					{
						return (**menu).menuWidth;
					}

#pragma mark -
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	QuickDraw

inline
Rect*			GetPortBounds( CGrafPtr port, Rect* rect )
					{
						*rect = port->portRect;
						return rect;
					}

inline
void			SetPortBounds( CGrafPtr port, const Rect* rect )
					{
						port->portRect = *rect;
					}

inline
PixMapHandle	GetPortPixMap( CGrafPtr port )
					{
						return port->portPixMap;
					}

inline
short			GetPixDepth( PixMapHandle pixMap )
					{
						return (**pixMap).pixelSize;
					}

inline
RgnHandle		GetPortVisibleRegion( CGrafPtr port, RgnHandle visRgn )
					{
						::MacCopyRgn(port->visRgn, visRgn);
						return visRgn;
					}

inline
void			SetPortVisibleRegion( CGrafPtr port, RgnHandle visRgn )
					{
						::MacCopyRgn(visRgn, port->visRgn);
					}

inline
short			GetPortTextFont( CGrafPtr port )
					{
						return port->txFont;
					}

inline
Style			GetPortTextFace( CGrafPtr port )
					{
						return port->txFace;
					}

inline
short			GetPortTextMode( CGrafPtr port )
					{
						return port->txMode;
					}

inline
short			GetPortTextSize( CGrafPtr port )
					{
						return port->txSize;
					}

inline
Rect*			GetRegionBounds( RgnHandle region, Rect* bounds )
					{
						*bounds = (**region).rgnBBox;
						return bounds;
					}

#pragma mark -
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Window Manager

inline DEFINE_API( WindowRef )
				GetWindowList()
					{
						return LMGetWindowList();
					}

#endif // !ACCESSOR_CALLS_ARE_FUNCTIONS

#pragma mark -
// ===========================================================================
//	Non-Opaque Accessors
// ===========================================================================
//	When Toolbox structs are opaque, a CGrafPtr and GrafPtr are the same type.
//	However, when they are not opaque, they are different types. The Toolbox
//	headers define accessor functions using the CGrafPtr parameter type. To
//	allow code to call the same accessors using a GrafPtr, we overload those
//	functions here.

#if !OPAQUE_TOOLBOX_STRUCTS

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	QuickDraw

inline
Rect*			GetPortBounds( GrafPtr port, Rect* rect )
					{
						*rect = port->portRect;
						return rect;
					}

inline
void			SetPortBounds( GrafPtr port, const Rect* rect )
					{
						port->portRect = *rect;
					}
					
inline
Boolean			IsPortColor( GrafPtr port )
					{
						return ((port->portBits.rowBytes & 0xC000) == 0xC000);
					}

inline
RgnHandle		GetPortVisibleRegion( GrafPtr port, RgnHandle visRgn )
					{
						::MacCopyRgn(port->visRgn, visRgn);
						return visRgn;
					}

inline
void			SetPortVisibleRegion( GrafPtr port, RgnHandle visRgn )
					{
						::MacCopyRgn(visRgn, port->visRgn);
					}

inline
short			GetPortTextFont( GrafPtr port )
					{
						return port->txFont;
					}

inline
Style			GetPortTextFace( GrafPtr port )
					{
						return port->txFace;
					}

inline
short			GetPortTextMode( GrafPtr port )
					{
						return port->txMode;
					}

inline
short			GetPortTextSize( GrafPtr port )
					{
						return port->txSize;
					}
					
#pragma mark -
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Window Manager
					
inline
WindowRef		GetWindowFromPort( GrafPtr port )
					{
						return (WindowRef) port;
					}

#endif // !OPAQUE_TOOLBOX_STRUCTS

#endif // !ACCESSOR_CALLS_ARE_FUNCTIONS || !OPAQUE_TOOLBOX_STRUCTS

#endif
