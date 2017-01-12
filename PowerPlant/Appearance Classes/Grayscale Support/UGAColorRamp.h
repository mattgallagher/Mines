// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGAColorRamp.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair
//
//	Maintains a table of colors corresponding to those used in the
//	Apple Grayscale Appearance (AGA) specification.

#ifndef _H_UGAColorRamp
#define _H_UGAColorRamp
#pragma once

#include <PP_Prefix.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


enum {		// Enumeration of constants for the colors as specified by AGA
	colorRamp_White		 	= 	0,
	colorRamp_Gray1			=	1,
	colorRamp_Gray2			=	2,
	colorRamp_Gray3			=	3,
	colorRamp_Gray4			=	4,
	colorRamp_Gray5			=	5,
	colorRamp_Gray6			=	6,
	colorRamp_Gray7			=	7,
	colorRamp_Gray8			=	8,
	colorRamp_Gray9			=	9,
	colorRamp_Gray10		=	10,
	colorRamp_Gray11		=	11,
	colorRamp_Gray12		=	12,
	colorRamp_Black		 	= 	13,
	colorRamp_GrayA1		=	14,
	colorRamp_GrayA2		=	15,
	colorRamp_Purple1		=	16,
	colorRamp_Purple2		=	17,
	colorRamp_Purple3		=	18,
	colorRamp_Purple4		=	19
};


// ---------------------------------------------------------------------------

class UGAColorRamp {
public:
	static	const RGBColor&	GetColor( SInt8 inColorIndex );

	static	void		GetColor(
								SInt8		inColorIndex,
								RGBColor&	outColor);

	static	const RGBColor&	GetWhiteColor()	{ return GetColor(colorRamp_White); }

	static	const RGBColor&	GetBlackColor() { return GetColor(colorRamp_Black); }

	static	void		GetFocusAccentColor( RGBColor& outColor );

protected:
	static	RGBColor	sColorTable[20];
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
