// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UQDOperators.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Operator functions for common QuickDraw data types
//
//	NOTE: These operators are intentionally NOT in the PowerPlant namespace

#ifndef _H_UQDOperators
#define _H_UQDOperators
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

// ---------------------------------------------------------------------------
//	¥ RGBColor comparison

inline bool	operator == (
		const RGBColor&		inColorOne,
		const RGBColor&		inColorTwo)
	{
		return (inColorOne.red   == inColorTwo.red)    &&
			   (inColorOne.green == inColorTwo.green)  &&
			   (inColorOne.blue  == inColorTwo.blue);
	}


inline bool operator != (
		const RGBColor&		inColorOne,
		const RGBColor&		inColorTwo)
	{
		return !(inColorOne == inColorTwo);
	}


// ---------------------------------------------------------------------------
//	¥ Point comparison

inline bool operator == (
		Point	inPointOne,
		Point	inPointTwo)
	{								// Point is two 16-bit integers. With some
									//   creative typecasting, we can do a
									//   single 32-bit comparison
		return ( *(SInt32*)&inPointOne == *(SInt32*)&inPointTwo );
	}


inline bool operator != (
		Point	inPointOne,
		Point	inPointTwo)
	{
		return !(inPointOne == inPointTwo);
	}


// ---------------------------------------------------------------------------
//	¥ Rect comparison

inline bool operator == (
		const Rect&		inRectOne,
		const Rect&		inRectTwo)
	{								// Rect is four 16-bit integers. With some
									//   creative typecasting, we can do a
									//   two 32-bit comparisons
		return (((SInt32 *) &inRectOne)[0] == ((SInt32 *) &inRectTwo)[0]) &&
			   (((SInt32 *) &inRectOne)[1] == ((SInt32 *) &inRectTwo)[1]);
	}



inline bool operator != (
		const Rect&		inRectOne,
		const Rect&		inRectTwo)
	{
		return !(inRectOne == inRectTwo);
	}


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
