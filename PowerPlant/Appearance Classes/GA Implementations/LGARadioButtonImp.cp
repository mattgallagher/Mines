// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGARadioButtonImp.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGARadioButtonImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LControl.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <LString.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant

											// Constants for the various state choices for the
											// radio button
enum {
	radioRendering_NormalColorOff			=	0,
	radioRendering_NormalColorOn			=	1,
	radioRendering_NormalColorMixed			=	2,
	radioRendering_PushedColorOff			=	3,
	radioRendering_PushedColorOn			=	4,
	radioRendering_PushedColorMixed			=	5,
	radioRendering_DimmedColorOff			=	6,
	radioRendering_DimmedColorOn			=	7,
	radioRendering_DimmedColorMixed			=	8,
	radioRendering_NormalBWOff				=	9,
	radioRendering_NormalBWOn				=	10,
	radioRendering_NormalBWMixed			=	11,
	radioRendering_PushedBWOff				=	12,
	radioRendering_PushedBWOn				=	13,
	radioRendering_PushedBWMixed			=	14
};

											// Constants for the number of choices for the drawing
											// arrays and constants for the number of rows and
											// columns in the array
enum {
	radio_Choice			=	15,
	radio_Rows				=	12,
	radio_Columns			=	12
};


enum {
	radioButton_RadioWidth			=	12,
	radioButton_TextOffset			=	2
};


											// Color Constants

const SInt8 B	=	colorRamp_Black;		//	Index for black
const SInt8	W	=	colorRamp_White;		//	Index for white
const SInt8	N	=	-1;						//	Index for no color which means draw nothing


											// ¥ PIXEL MAP ARRAY

											// Definition of the array that specifies the pixel
											// map for the different choices for the radio button

const SInt8 radio_RenderingChoices[radio_Choice][radio_Rows][radio_Columns] =
{
											// OFF Normal - color radioRendering_NormalColorOff
	{
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N },		//	0
		{ N,  N,  B, 10,  2,  2,  2,  4, 11,  B,  N,  N },		//	1
		{ N,  B,  4,  2,  1,  W,  W,  W,  2,  7,  B,  N },		//	2
		{ 5, 10,  2,  1,  W,  W,  1,  1,  2,  4, 11,  5 },		//	3
		{11,  2,  1,  W,  W,  1,  1,  2,  2,  4,  7, 11 },		//	4
		{ B,  2,  W,  W,  1,  1,  2,  2,  4,  4,  7,  B },		//	5
		{ B,  2,  W,  1,  1,  2,  2,  4,  4,  5,  7,  B },		//	6
		{11,  4,  W,  1,  2,  2,  4,  4,  5,  5,  7, 11 },		//	7
		{ 5, 10,  2,  2,  2,  4,  4,  5,  5,  7, 11,  5 },		//	8
		{ N,  B,  7,  4,  4,  4,  5,  5,  7,  7,  B,  N },		//	9
		{ N,  N,  B, 10,  7,  7,  7,  7, 11,  B,  N,  N },		//	10
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }		//	11
	},
											//	ON Normal - color	radioRendering_NormalColorOn
	{
		{ N,  N,  N,  5, 12,  B,  B, 12,  5,  N,  N,  N },
		{ N,  N, 12,  B, 11, 10, 10, 10, 11,  B,  N,  N },
		{ N, 12, 11, 10,  8,  8,  8,  7,  7,  6,  B,  N },
		{ 5,  B, 10,  8,  B,  B,  B,  B,  6,  6, 11,  5 },
		{12, 11,  8,  B,  B,  B,  B,  B,  B,  6,  4, 11 },
		{ B, 10,  8,  B,  B,  B,  B,  B,  B,  4,  4,  B },
		{ B, 10,  8,  B,  B,  B,  B,  B,  B,  4,  2,  B },
		{12, 10,  7,  B,  B,  B,  B,  B,  B,  2,  W, 11 },
		{ 5, 12,  7,  6,  B,  B,  B,  B,  2,  W, 11,  5 },
		{ N, 12,  6,  6,  6,  4,  4,  2,  W,  W,  B,  N },
		{ N,  N, 12, 11,  4,  4,  2,  W, 11,  B,  N,  N },
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }
	},
											//	MIXED Normal - color	radioRendering_NormalColorMixed
	{
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N },
		{ N,  N,  B, 10,  2,  2,  2,  4, 11,  B,  N,  N },
		{ N,  B,  4,  2,  1,  W,  W,  W,  2,  7,  B,  N },
		{ 5, 10,  2,  1,  W,  W,  1,  1,  2,  4, 11,  5 },
		{11,  2,  1,  W,  W,  1,  1,  2,  2,  4,  7, 11 },
		{ B,  2,  W,  B,  B,  B,  B,  B,  B,  4,  7,  B },
		{ B,  2,  W,  B,  B,  B,  B,  B,  B,  5,  7,  B },
		{11,  4,  W,  1,  2,  2,  4,  4,  5,  5,  7, 11 },
		{ 5, 10,  2,  2,  2,  4,  4,  5,  5,  7, 11,  5 },
		{ N,  B,  7,  4,  4,  4,  5,  5,  7,  7,  B,  N },
		{ N,  N,  B, 10,  7,  7,  7,  7, 11,  B,  N,  N },
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }
	},
											//	OFF Pushed - color	radioRendering_PushedColorOff
	{
		{ N,  N,  N,  5, 12,  B,  B, 12,  5,  N,  N,  N },
		{ N,  N, 12,  B, 11, 11, 11, 11, 11,  B,  N,  N },
		{ N, 12, 11, 11, 10, 10,  9,  9,  9,  8,  B,  N },
		{ 5,  B, 11, 10, 10,  9,  9,  8,  8,  8, 11,  5 },
		{12, 11, 10, 10,  9,  9,  8,  8,  8,  7,  6, 11 },
		{ B, 11, 10,  9,  9,  8,  8,  8,  7,  7,  6,  B },
		{ B, 11,  9,  9,  8,  8,  8,  7,  7,  6,  6,  B },
		{12, 11,  9,  8,  8,  8,  7,  7,  6,  6,  4, 11 },
		{ 5, 12,  9,  8,  8,  7,  7,  6,  6,  4, 10,  5 },
		{ N, 11,  8,  8,  7,  7,  6,  6,  4,  4,  B,  N },
		{ N,  N, 12, 11,  6,  6,  6,  4, 10,  B,  N,  N },
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }
	},
											//	ON Pushed - color	radioRendering_PushedColorOn
	{
		{ N,  N,  N,  5, 12,  B,  B, 12,  5,  N,  N,  N },
		{ N,  N, 12,  B, 11, 11, 11, 11, 11,  B,  N,  N },
		{ N, 12, 11, 11, 10, 10,  9,  9,  9,  8,  B,  N },
		{ 5,  B, 11, 10,  B,  B,  B,  B,  8,  8, 11,  5 },
		{12, 11, 10,  B,  B,  B,  B,  B,  B,  7,  6, 11 },
		{ B, 11, 10,  B,  B,  B,  B,  B,  B,  7,  6,  B },
		{ B, 11,  9,  B,  B,  B,  B,  B,  B,  6,  6,  B },
		{12, 11,  9,  B,  B,  B,  B,  B,  B,  6,  4, 11 },
		{ 5, 12,  9,  8,  B,  B,  B,  B,  6,  4, 10,  5 },
		{ N, 11,  8,  8,  7,  7,  6,  6,  4,  4,  B,  N },
		{ N,  N, 12, 11,  6,  6,  6,  4, 10,  B,  N,  N },
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }
	},
											//	MIXED Pushed - color	radioRendering_PushedColorMixed
	{
		{ N,  N,  N,  5, 12,  B,  B, 12,  5,  N,  N,  N },
		{ N,  N, 12,  B, 11, 11, 11, 11, 11,  B,  N,  N },
		{ N,  2, 11, 11, 10, 10,  9,  9,  9,  8,  B,  N },
		{ 5,  B, 11, 10, 10,  9,  9,  8,  8,  8, 11,  5 },
		{12, 11, 10, 10,  9,  9,  8,  8,  8,  7,  6,  B },
		{ B, 11, 10,  B,  B,  B,  B,  B,  B,  7,  6,  B },
		{ B, 11,  9,  B,  B,  B,  B,  B,  B,  6,  6,  B },
		{12, 11,  9,  8,  8,  8,  7,  7,  6,  6,  4, 11 },
		{ 5, 12,  9,  8,  8,  7,  7,  6,  6,  4, 10,  5 },
		{ N, 11,  8,  8,  7,  7,  6,  6,  4,  4,  B,  N },
		{ N,  N, 12, 11,  6,  6,  6,  4, 10,  B,  N,  N },
		{ N,  N,  N,  5, 11,  B,  B, 11,  5,  N,  N,  N }
	},
											//	OFF Dimmed - color	radioRendering_DimmedColorOff
	{
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N }
	},
											//	ON Dimmed - color	radioRendering_DimmedColorOn
	{
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  7,  2,  2,  8,  8,  8,  8,  2,  2,  7,  N },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ N,  7,  2,  2,  8,  8,  8,  8,  2,  2,  7,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N }
	},
											//	MIXED Dimmed - color	radioRendering_DimmedColorMixed
	{
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ 7,  2,  2,  8,  8,  8,  8,  8,  8,  2,  2,  7 },
		{ 7,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  7 },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  7,  2,  2,  2,  2,  2,  2,  2,  2,  7,  N },
		{ N,  N,  7,  7,  2,  2,  2,  2,  7,  7,  N,  N },
		{ N,  N,  N,  N,  7,  7,  7,  7,  N,  N,  N,  N }
	},
											// OFF Normal - B&W	radioRendering_NormalBWOff
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	},
											//	ON Normal - B&W	radioRendering_NormalBWOn
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  B,  B,  B,  B,  W,  W,  B,  N },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ N,  B,  W,  W,  B,  B,  B,  B,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	},
											//	MIXED Normal - B&W	radioRendering_NormalBWMixed
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  N,  B,  B,  W,  W,  W,  W,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	},
											//	OFF Pushed - B&W	radioRendering_PushedBWOff
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  N },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ N,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	},
											//	ON Pushed - B&W radioRendering_PushedBWOn
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  B,  B,  W,  B,  B,  B,  B,  W,  B,  B,  N },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ N,  B,  B,  W,  B,  B,  B,  B,  W,  B,  B,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	},
											//	MIXED Pushed - B&W	radioRendering_PushedBWMixed
	{
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  N },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ B,  B,  W,  B,  B,  B,  B,  B,  B,  W,  B,  B },
		{ B,  B,  W,  W,  W,  W,  W,  W,  W,  W,  B,  B },
		{ N,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  N },
		{ N,  B,  B,  B,  W,  W,  W,  W,  B,  B,  B,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  N,  N,  N,  N }
	}
};


// ---------------------------------------------------------------------------
//	¥ LGARadioButtonImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGARadioButtonImp::LGARadioButtonImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGARadioButtonImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGARadioButtonImp::LGARadioButtonImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGARadioButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGARadioButtonImp::~LGARadioButtonImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::Init (
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane	= inControlPane;
	mTitle			= inTitle;
	mTextTraitsID	= inTextTraitsID;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGARadioButtonImp::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mTitle, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::SetTextTraitsID(
	ResIDT	 inTextTraitsID)
{

	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::PostSetValue()
{
	if (IsPushed()) {
		SetPushedState(false);

	} else {
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::CalcTitleRect(
	Rect&	outRect)
{
	ResIDT	textTID = GetTextTraitsID ();

	UTextTraits::SetPortTextTraits ( textTID );

												// Figure out the height of the text for the
												// selected font
	FontInfo fi;
	::GetFontInfo ( &fi );
	SInt16		textHeight = (SInt16) (fi.ascent + fi.descent);

												// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );
	SInt16		frameHeight = UGraphicUtils::RectHeight ( outRect );
	::MacInsetRect ( &outRect, 4, 2 );

												// If the text height is greater than the frame
												// height the constrain it to the frame height
	if ( frameHeight < textHeight ) {
		textHeight = frameHeight;
	}
												// Now we modify the title rect so that it is to
												// the right of the radio control itself
	outRect.top += ((frameHeight - textHeight) / 2) - radioButton_TextOffset;
	outRect.bottom = (SInt16) (outRect.top + textHeight);
	outRect.left += radioButton_RadioWidth + radioButton_TextOffset;

}


// ---------------------------------------------------------------------------
//	¥ CalcLocalControlRect															[public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::CalcLocalControlRect (
		Rect &outRect )
{
												// Figure out the frame rect for the radio button
	CalcLocalFrameRect ( outRect );
	SInt16		frameHeight = UGraphicUtils::RectHeight ( outRect );

												// Adjust the rect such that it is always centered
												// vertically within the radio buttons frame rect
	outRect.left += 1;
	outRect.right = (SInt16) (outRect.left + radioButton_RadioWidth);
	outRect.top += ((frameHeight - radioButton_RadioWidth) / 2) + 1;
	outRect.bottom = (SInt16) (outRect.top + radioButton_RadioWidth);

}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawSelf()
{
	StColorPenState::Normalize();

	if ( IsEnabled() && IsActive()) {		// Draw button in proper state
		if (IsPushed()) {
			DrawRadioHilited();

		} else {
			DrawRadioNormal();
		}

	} else {
		DrawRadioDimmed();
	}

	if (HasTitle()  &&  !IsPushed()) {
		DrawRadioTitle();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawRadioTitle												  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawRadioTitle()
{
	ResIDT	textTID = GetTextTraitsID();
	Rect	titleRect;

	UTextTraits::SetPortTextTraits(textTID);

	CalcTitleRect(titleRect);

	RGBColor	textColor;
	::GetForeColor(&textColor);

	mControlPane->ApplyForeAndBackColors();

	if ( (mControlPane->GetBitDepth() >= 4)  &&		// Dim color of inactive
		 (!IsEnabled() || !IsActive()) ) {			//   or disabled text

			UTextDrawing::DimTextColor(textColor);
	}

	::RGBForeColor(&textColor);

	UTextDrawing::DrawTruncatedWithJust(	mTitle,
											titleRect,
											teFlushDefault,
											true);
}


// ---------------------------------------------------------------------------
//	¥ DrawRadioNormal												  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawRadioNormal ()
{
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalControlRect ( localFrame );

												// BLACK & WHITE
	if ( mControlPane->GetBitDepth() < 4 ) {
		if ( IsMixedState ()) {
			DrawRadioChoice ( radioRendering_NormalBWMixed, localFrame );

		} else if ( IsSelected ()) {
			DrawRadioChoice ( radioRendering_NormalBWOn, localFrame );

		} else {
			DrawRadioChoice ( radioRendering_NormalBWOff, localFrame );
		}

	} else {								// COLOR

		if ( IsMixedState ()) {
			DrawRadioChoice ( radioRendering_NormalColorMixed, localFrame );

		} else if ( IsSelected ()) {
			DrawRadioChoice ( radioRendering_NormalColorOn, localFrame );

		} else {
			DrawRadioChoice ( radioRendering_NormalColorOff, localFrame );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawRadioHilited												  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawRadioHilited	()
{
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalControlRect ( localFrame );

												// BLACK & WHITE
	if ( mControlPane->GetBitDepth() < 4 ) {
		if ( IsPushed ()) {
			if ( IsMixedState ()) {
				DrawRadioChoice ( radioRendering_PushedBWMixed, localFrame );

			} else if ( IsSelected ()) {
				DrawRadioChoice ( radioRendering_PushedBWOn, localFrame );

			} else {
				DrawRadioChoice ( radioRendering_PushedBWOff, localFrame );
			}
		}

	} else {								// COLOR
		if ( IsPushed ()) {
			if ( IsMixedState ()) {
				DrawRadioChoice ( radioRendering_PushedColorMixed, localFrame );

			} else if ( IsSelected ()) {
				DrawRadioChoice ( radioRendering_PushedColorOn, localFrame );

			} else {
				DrawRadioChoice ( radioRendering_PushedColorOff, localFrame );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawRadioDimmed												  [public]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawRadioDimmed	()
{

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalControlRect ( localFrame );
												// BLACK & WHITE
	if ( mControlPane->GetBitDepth() < 4 ) {

		if ( IsMixedState ()) {
			DrawRadioChoice ( radioRendering_NormalBWMixed, localFrame );

		} else if ( IsSelected ()) {
			DrawRadioChoice ( radioRendering_NormalBWOn, localFrame );

		} else {
			DrawRadioChoice ( radioRendering_NormalBWOff, localFrame );
		}

	} else {								// COLOR
		if ( IsMixedState ()) {
			DrawRadioChoice ( radioRendering_DimmedColorMixed, localFrame );

		} else if ( IsSelected ()) {
			DrawRadioChoice ( radioRendering_DimmedColorOn, localFrame );

		} else {
			DrawRadioChoice ( radioRendering_DimmedColorOff, localFrame );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawRadioChoice											   [protected]
// ---------------------------------------------------------------------------

void
LGARadioButtonImp::DrawRadioChoice(
	SInt16			inChoice,
	const Rect&		inFrame)
{
												// Plot the radio button by iterating over the
												// array of color indices and drawing each of the
												// pixels making up the button
	SInt8 colorindex;
	for ( SInt16 i = 0; i < radio_Rows; i++ ) {
		for ( SInt16 j = 0; j < radio_Columns; j++ ) {
			RGBColor	tempColor;
			colorindex = radio_RenderingChoices[inChoice][i][j];
			if ( colorindex != N ) {
				tempColor = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel ( (SInt16) (inFrame.left + j),
												 (SInt16) (inFrame.top + i),
												 tempColor );
			}
		}
	}
}


PP_End_Namespace_PowerPlant
