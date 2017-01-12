// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGADisclosureTriangleImp.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGADisclosureTriangleImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LGAControlImp.h>

#include <LControl.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant

													// Constants for the various choices that are
													// used in the rendering of the triangle
enum {
	disclosure_EnabledRight				=	0,
	disclosure_PressedRight				=	1,
	disclosure_IntermediateRight		=	2,
	disclosure_PressedDown				=	3,
	disclosure_EnabledDown				=	4,
	disclosure_EnabledLeft				=	5,
	disclosure_PressedLeft				=	6,
	disclosure_IntermediateLeft			=	7,
	disclosure_DisabledRight			=	8,
	disclosure_DisabledDown				=	9,
	disclosure_DisabledLeft				=	10,
	disclosure_EnabledRightBW			=	11,
	disclosure_PressedRightBW			=	12,
	disclosure_IntermediateRightBW		=	13,
	disclosure_PressedDownBW			=	14,
	disclosure_EnabledDownBW			=	15,
	disclosure_EnabledLeftBW			=	16,
	disclosure_PressedLeftBW			=	17,
	disclosure_IntermediateLeftBW		=	18
};

enum {
	disclosure_Width					=	12
};

enum {
	disclosure_Choice					=	19,
	disclosure_Rows						=	12,
	disclosure_Columns					=	12
};


												// COLOR CONSTANTS

const SInt16 	B	=	colorRamp_Black;		//	Index for black
const SInt16	W	=	colorRamp_White;		//	Index for white
const SInt16	N	=	-1;						//	Index for no color which means draw nothing
const SInt16	A1	=	colorRamp_GrayA1;		//	Additional shade of gray
const SInt16	A2	=	colorRamp_GrayA2;		//	Additional shade of gray
const SInt16	P1	=	colorRamp_Purple1;		//	Shade of purple for the disclosure triangle
const SInt16	P2	=	colorRamp_Purple2;		//	Shade of purple for the disclosure triangle
const SInt16	P3	=	colorRamp_Purple3;		//	Shade of purple for the disclosure triangle
const SInt16	P4	=	colorRamp_Purple4;		//	Shade of purple for the disclosure triangle


const SInt8 disclosure_PixMapArray[disclosure_Choice][disclosure_Rows][disclosure_Columns] =
{
													// ENABLED RIGHT - color disclosure_EnabledRight
	{
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	1
		{ N, N, N, N, B,P1, B, N, N, N, N, N },		//	2
		{ N, N, N, N, B,P1,P2, B, N, N, N, N },		//	3
		{ N, N, N, N, B,P1,P2,P2, B, N, N, N },		//	4
		{ N, N, N, N, B,P1,P2,P2,P3, B, N, N },		//	5
		{ N, N, N, N, B,P1,P2,P3, B, 7, 4, N },		//	6
		{ N, N, N, N, B,P1,P3, B, 7, 4, N, N },		//	7
		{ N, N, N, N, B,P2, B, 7, 4, N, N, N },		//	8
		{ N, N, N, N, B, B, 7, 4, N, N, N, N },		//	9
		{ N, N, N, N, B, 7, 4, N, N, N, N, N },		//	10
		{ N, N, N, N, N, 4, N, N, N, N, N, N }		//	11
	},
													//	PRESSED RIGHT - color disclosure_PressedRight
	{
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	1
		{ N, N, N, N, B,P4, B, N, N, N, N, N },		//	2
		{ N, N, N, N, B,P4,P4, B, N, N, N, N },		//	3
		{ N, N, N, N, B,P4,P4,P4, B, N, N, N },		//	4
		{ N, N, N, N, B,P4,P4,P4,P4, B, N, N },		//	5
		{ N, N, N, N, B,P4,P4,P4, B, N, N, N },		//	6
		{ N, N, N, N, B,P4,P4, B, N, N, N, N },		//	7
		{ N, N, N, N, B,P4, B, N, N, N, N, N },		//	8
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	9
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	INTERMEDIATE RIGHT - color	disclosure_IntermediateRight
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, B, N, N },		//	1
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	2
		{ N, N, N, N, N, N, N, B,P4, B, N, N },		//	3
		{ N, N, N, N, N, N, B,P4,P4, B, N, N },		//	4
		{ N, N, N, N, N, B,P4,P4,P4, B, N, N },		//	5
		{ N, N, N, N, B,P4,P4,P4,P4, B, N, N },		//	6
		{ N, N, N, B,P4,P4,P4,P4,P4, B, N, N },		//	7
		{ N, N, B,P4,P4,P4,P4,P4,P4, B, N, N },		//	8
		{ N, B, B, B, B, B, B, B, B, B, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	PRESSED DOWN - color	disclosure_PressedDown
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	2
		{ B, B, B, B, B, B, B, B, B, B, B, N },		//	3
		{ N, B,P4,P4,P4,P4,P4,P4,P4, B, N, N },		//	4
		{ N, N, B,P4,P4,P4,P4,P4, B, N, N, N },		//	5
		{ N, N, N, B,P4,P4,P4, B, N, N, N, N },		//	6
		{ N, N, N, N, B,P4, B, N, N, N, N, N },		//	7
		{ N, N, N, N, N, B, N, N, N, N, N, N },		//	8
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	ENABLED DOWN - color	disclosure_EnabledDown
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	2
		{ B, B, B, B, B, B, B, B, B, B, B, N },		//	3
		{ N, B,P1,P1,P1,P1,P1,P1,P2, B, 7, 4 },		//	4
		{ N, N, B,P2,P2,P2,P2,P3, B, 7, 4, N },		//	5
		{ N, N, N, B,P2,P2,P3, B, 7, 4, N, N },		//	6
		{ N, N, N, N, B,P3, B, 7, 4, N, N, N },		//	7
		{ N, N, N, N, N, B, 7, 4, N, N, N, N },		//	8
		{ N, N, N, N, N, N, 4, N, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	ENABLED LEFT - color	disclosure_EnabledLeft
	{
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	0
		{ N, N, N, N, N, N, B, B, 7, N, N, N },		//	1
		{ N, N, N, N, N, B,P1, B, 7, N, N, N },		//	2
		{ N, N, N, N, B,P2,P1, B, 7, N, N, N },		//	3
		{ N, N, N, B,P2,P2,P1, B, 7, N, N, N },		//	4
		{ N, N, B,P3,P2,P2,P1, B, 7, N, N, N },		//	5
		{ N, N, N, B,P3,P2,P1, B, 7, N, N, N },		//	6
		{ N, N, N, N, B,P3,P1, B, 7, N, N, N },		//	7
		{ N, N, N, N, N, B,P2, B, 7, N, N, N },		//	8
		{ N, N, N, N, N, N, B, B, 7, N, N, N },		//	9
		{ N, N, N, N, N, N, N, B, 7, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, 7, N, N, N }		//	11
	},
													//	PRESSES LEFT - color	disclosure_PressedLeft
	{
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	0
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	1
		{ N, N, N, N, N, B,P4, B, N, N, N, N },		//	2
		{ N, N, N, N, B,P4,P4, B, N, N, N, N },		//	3
		{ N, N, N, B,P4,P4,P4, B, N, N, N, N },		//	4
		{ N, N, B,P4,P4,P4,P4, B, N, N, N, N },		//	5
		{ N, N, N, B,P4,P4,P4, B, N, N, N, N },		//	6
		{ N, N, N, N, B,P4,P4, B, N, N, N, N },		//	7
		{ N, N, N, N, N, B,P4, B, N, N, N, N },		//	8
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	INTERMEDIATE LEFT - color	disclosure_IntermediateLeft
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, B, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, B, B, N, N, N, N, N, N, N, N },		//	2
		{ N, N, B,P4, B, N, N, N, N, N, N, N },		//	3
		{ N, N, B,P4,P4, B, N, N, N, N, N, N },		//	4
		{ N, N, B,P4,P4,P4, B, N, N, N, N, N },		//	5
		{ N, N, B,P4,P4,P4,P4, B, N, N, N, N },		//	6
		{ N, N, B,P4,P4,P4,P4,P4, B, N, N, N },		//	7
		{ N, N, B,P4,P4,P4,P4,P4,P4, B, N, N },		//	8
		{ N, N, B, B, B, B, B, B, B, B, B, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	DISABLED RIGHT - color	disclosure_DisabledRight
	{
		{ N, N, N, N, 5, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, 5, 5, N, N, N, N, N, N },		//	1
		{ N, N, N, N, 5, 5, 5, N, N, N, N, N },		//	2
		{ N, N, N, N, 5, 5, 5, 5, N, N, N, N },		//	3
		{ N, N, N, N, 5, 5, 5, 5, 5, N, N, N },		//	4
		{ N, N, N, N, 5, 5, 5, 5, 5, 5, N, N },		//	5
		{ N, N, N, N, 5, 5, 5, 5, 5, N, N, N },		//	6
		{ N, N, N, N, 5, 5, 5, 5, N, N, N, N },		//	7
		{ N, N, N, N, 5, 5, 5, N, N, N, N, N },		//	8
		{ N, N, N, N, 5, 5, N, N, N, N, N, N },		//	9
		{ N, N, N, N, 5, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	DISABLED DOWN - color	disclosure_DisabledDown
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	2
		{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, N },		//	3
		{ N, 5, 5, 5, 5, 5, 5, 5, 5, 5, N, N },		//	4
		{ N, N, 5, 5, 5, 5, 5, 5, 5, N, N, N },		//	5
		{ N, N, N, 5, 5, 5, 5, 5, N, N, N, N },		//	6
		{ N, N, N, N, 5, 5, 5, N, N, N, N, N },		//	7
		{ N, N, N, N, N, 5, N, N, N, N, N, N },		//	8
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	DISABLED LEFT - color	disclosure_DisabledLeft
	{
		{ N, N, N, N, N, N, N, 5, N, N, N, N },		//	0
		{ N, N, N, N, N, N, 5, 5, N, N, N, N },		//	1
		{ N, N, N, N, N, 5, 5, 5, N, N, N, N },		//	2
		{ N, N, N, N, 5, 5, 5, 5, N, N, N, N },		//	3
		{ N, N, N, 5, 5, 5, 5, 5, N, N, N, N },		//	4
		{ N, N, 5, 5, 5, 5, 5, 5, N, N, N, N },		//	5
		{ N, N, N, 5, 5, 5, 5, 5, N, N, N, N },		//	6
		{ N, N, N, N, 5, 5, 5, 5, N, N, N, N },		//	7
		{ N, N, N, N, N, 5, 5, 5, N, N, N, N },		//	8
		{ N, N, N, N, N, N, 5, 5, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, 5, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													// ENABLED RIGHT - B&W	disclosure_EnabledRightBW
	{
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	1
		{ N, N, N, N, B, W, B, N, N, N, N, N },		//	2
		{ N, N, N, N, B, W, W, B, N, N, N, N },		//	3
		{ N, N, N, N, B, W, W, W, B, N, N, N },		//	4
		{ N, N, N, N, B, W, W, W, W, B, N, N },		//	5
		{ N, N, N, N, B, W, W, W, B, N, N, N },		//	6
		{ N, N, N, N, B, W, W, B, N, N, N, N },		//	7
		{ N, N, N, N, B, W, B, N, N, N, N, N },		//	8
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	9
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	PRESSED RIGHT - B&W	disclosure_PressedRightBW
	{
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	1
		{ N, N, N, N, B, B, B, N, N, N, N, N },		//	2
		{ N, N, N, N, B, B, B, B, N, N, N, N },		//	3
		{ N, N, N, N, B, B, B, B, B, N, N, N },		//	4
		{ N, N, N, N, B, B, B, B, B, B, N, N },		//	5
		{ N, N, N, N, B, B, B, B, B, N, N, N },		//	6
		{ N, N, N, N, B, B, B, B, N, N, N, N },		//	7
		{ N, N, N, N, B, B, B, N, N, N, N, N },		//	8
		{ N, N, N, N, B, B, N, N, N, N, N, N },		//	9
		{ N, N, N, N, B, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	INTERMEDIATE RIGHT - B&W	disclosure_IntermediateRightBW
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, B, N, N },		//	1
		{ N, N, N, N, N, N, N, N, B, B, N, N },		//	2
		{ N, N, N, N, N, N, N, B, B, B, N, N },		//	3
		{ N, N, N, N, N, N, B, B, B, B, N, N },		//	4
		{ N, N, N, N, N, B, B, B, B, B, N, N },		//	5
		{ N, N, N, N, B, B, B, B, B, B, N, N },		//	6
		{ N, N, N, B, B, B, B, B, B, B, N, N },		//	7
		{ N, N, B, B, B, B, B, B, B, B, N, N },		//	8
		{ N, B, B, B, B, B, B, B, B, B, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	PRESSED DOWN - B&W	disclosure_PressedDownBW
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	2
		{ B, B, B, B, B, B, B, B, B, B, B, N },		//	3
		{ N, B, B, B, B, B, B, B, B, B, N, N },		//	4
		{ N, N, B, B, B, B, B, B, B, N, N, N },		//	5
		{ N, N, N, B, B, B, B, B, N, N, N, N },		//	6
		{ N, N, N, N, B, B, B, N, N, N, N, N },		//	7
		{ N, N, N, N, N, B, N, N, N, N, N, N },		//	8
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	ENABLED DOWN - B&W	disclosure_EnabledDownBW
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	2
		{ B, B, B, B, B, B, B, B, B, B, B, N },		//	3
		{ N, B, W, W, W, W, W, W, W, B, N, N },		//	4
		{ N, N, B, W, W, W, W, W, B, N, N, N },		//	5
		{ N, N, N, B, W, W, W, B, N, N, N, N },		//	6
		{ N, N, N, N, B, W, B, N, N, N, N, N },		//	7
		{ N, N, N, N, N, B, N, N, N, N, N, N },		//	8
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	ENABLED LEFT - B&W	disclosure_EnabledLeftBW
	{
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	0
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	1
		{ N, N, N, N, N, B, W, B, N, N, N, N },		//	2
		{ N, N, N, N, B, W, W, B, N, N, N, N },		//	3
		{ N, N, N, B, W, W, W, B, N, N, N, N },		//	4
		{ N, N, B, W, W, W, W, B, N, N, N, N },		//	5
		{ N, N, N, B, W, W, W, B, N, N, N, N },		//	6
		{ N, N, N, N, B, W, W, B, N, N, N, N },		//	7
		{ N, N, N, N, N, B, W, B, N, N, N, N },		//	8
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	PRESSES LEFT - B&W disclosure_PressedLeftBW
	{
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	0
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	1
		{ N, N, N, N, N, B, B, B, N, N, N, N },		//	2
		{ N, N, N, N, B, B, B, B, N, N, N, N },		//	3
		{ N, N, N, B, B, B, B, B, N, N, N, N },		//	4
		{ N, N, B, B, B, B, B, B, N, N, N, N },		//	5
		{ N, N, N, B, B, B, B, B, N, N, N, N },		//	6
		{ N, N, N, N, B, B, B, B, N, N, N, N },		//	7
		{ N, N, N, N, N, B, B, B, N, N, N, N },		//	8
		{ N, N, N, N, N, N, B, B, N, N, N, N },		//	9
		{ N, N, N, N, N, N, N, B, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	},
													//	INTERMEDIATE LEFT - B&W	disclosure_IntermediateLeftBW
	{
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	0
		{ N, N, B, N, N, N, N, N, N, N, N, N },		//	1
		{ N, N, B, B, N, N, N, N, N, N, N, N },		//	2
		{ N, N, B, B, B, N, N, N, N, N, N, N },		//	3
		{ N, N, B, B, B, B, N, N, N, N, N, N },		//	4
		{ N, N, B, B, B, B, B, N, N, N, N, N },		//	5
		{ N, N, B, B, B, B, B, B, N, N, N, N },		//	6
		{ N, N, B, B, B, B, B, B, B, N, N, N },		//	7
		{ N, N, B, B, B, B, B, B, B, B, N, N },		//	8
		{ N, N, B, B, B, B, B, B, B, B, B, N },		//	9
		{ N, N, N, N, N, N, N, N, N, N, N, N },		//	10
		{ N, N, N, N, N, N, N, N, N, N, N, N }		//	11
	}
};


#pragma mark === INITIALIZATION

// ---------------------------------------------------------------------------
//	¥ LGADisclosureTriangleImp				Constructor				  [public]
// ---------------------------------------------------------------------------

LGADisclosureTriangleImp::LGADisclosureTriangleImp (
	LControlPane* inControlPane )
	
	: LGAControlImp ( inControlPane )
{
}


// ---------------------------------------------------------------------------
//	¥ LGADisclosureTriangleImp				Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGADisclosureTriangleImp::LGADisclosureTriangleImp (
	LStream	*inStream )
	
	: LGAControlImp ( inStream )
{
	mLeftDisclosure = false;
	mAutoToggle = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LGADisclosureTriangleImp				Destructor				  [public]
// ---------------------------------------------------------------------------

LGADisclosureTriangleImp::~LGADisclosureTriangleImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init ( inControlPane, controlKind, title, textTraitID );
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::Init (
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
														// Setup our fields
	mControlPane = inControlPane;

														// Get our flags setup from the control kind
	mLeftDisclosure = (inControlKind == kControlTriangleLeftFacingProc ||
								inControlKind == kControlTriangleLeftFacingAutoToggleProc);
	mAutoToggle = true;
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::SetPushedState	(
	Boolean	inPushedState )
{
	if ( mPushed != inPushedState ) {
		mPushed = inPushedState;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::SetValue	(
		SInt32	inValue )
{
	if ( inValue != mControlPane->GetValue ()) {
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::PostSetValue	()
{
														// And make sure we get the hilite state
														// changed
	if ( IsPushed ()) {
		SetPushedState ( false );
	}
														// We always need get things redrawn
	DrawIntermediateState ();
	mControlPane->Draw ( nil );

}


// ---------------------------------------------------------------------------
//	¥ CalcLocalControlRect											  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::CalcLocalControlRect(
	Rect&	outRect)
{
														// Figure out the frame rect for the triangle button
	CalcLocalFrameRect ( outRect );

														// Adjust the rect such that it is always
														// centered vertically  and horizontally
														// within the frame rect
	outRect.left += ((UGraphicUtils::RectWidth ( outRect ) - disclosure_Width) / 2);
	outRect.right = (SInt16) (outRect.left + disclosure_Width);
	outRect.top += ((UGraphicUtils::RectHeight ( outRect ) - disclosure_Width) / 2);
	outRect.bottom = (SInt16) (outRect.top + disclosure_Width);
}


#pragma mark -
#pragma mark === DATA HANDLING

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::SetDataTag (
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr )
{
	switch ( inTag ) {
		case kControlTriangleLastValueTag: {
			SInt16	value = *(SInt16*) inDataPtr;
									// Get the value setup, if this is turned
									// on it will result in the triangle being
									// toggled
			mControlPane->SetValue(value);
			break;
		}
		
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::GetDataTag (
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	switch (inTag) {
	
		case kControlTriangleLastValueTag: {
			if (outDataSize != nil) {
				*outDataSize = sizeof(SInt16);
			}

			*(SInt16*) inBuffer = (SInt16) mControlPane->GetValue();
			break;
		}
		
		default:
			LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			break;
	}
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
// 			¥	DrawSelf											  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::DrawSelf	()
{
	StColorPenState::Normalize ();

	SInt16		depth = mControlPane->GetBitDepth();

														// Get the disclosure triangle drawn in its
														// various states
	if ( IsPushed ()) {
		StColorPenState::Normalize ();

		if ( IsSelected ()) {
			PlotPixelMapChoice ( depth < 4 ? disclosure_PressedDownBW :
															disclosure_PressedDown );
		} else {
			PlotPixelMapChoice ( IsLeftDisclosure() ?
				(depth < 4 ? disclosure_PressedLeftBW : disclosure_PressedLeft) :
					(depth < 4 ? disclosure_PressedRightBW : disclosure_PressedRight));
		}

	} else if ( !IsEnabled () || !IsActive ()) {

		if ( IsSelected ()) {
			if ( depth < 4	) {
				StColorPenState::SetGrayPattern();

				PlotPixelMapChoice ( disclosure_PressedDownBW );
			} else {
				StColorPenState::Normalize ();

				PlotPixelMapChoice ( disclosure_DisabledDown );
			}
		} else {
			if ( depth < 4	) {
				StColorPenState::SetGrayPattern();

				PlotPixelMapChoice ( IsLeftDisclosure() ? disclosure_PressedLeftBW :
													disclosure_PressedRightBW );
			} else {
				StColorPenState::Normalize ();

				PlotPixelMapChoice ( IsLeftDisclosure() ? disclosure_DisabledLeft :
												disclosure_DisabledRight );
			}
		}

	} else {

		if ( IsSelected ()) {
			PlotPixelMapChoice ( depth < 4 ? disclosure_EnabledDownBW :
																disclosure_EnabledDown );
		} else {
			PlotPixelMapChoice ( IsLeftDisclosure() ?
				(depth < 4 ? disclosure_EnabledLeftBW : disclosure_EnabledLeft) :
					(depth < 4 ? disclosure_EnabledRightBW : disclosure_EnabledRight));
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawIntermediateState											  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::DrawIntermediateState	()
{
	if ( IsVisible () && mControlPane->FocusDraw ()) {
		StColorPenState	theColorPenState;
		theColorPenState.Normalize ();

															// Setup a device loop so that we can
															// handle drawing at the correct bit depth
		Rect	localRect;
		CalcLocalControlRect ( localRect );
		StColorDrawLoop	theLoop ( localRect );
		SInt16				depth;
		while ( theLoop.NextDepth ( depth )) {
															// Now we wipe the background before we
															// draw the intermediate state
			WipeBackground ();
															// Now plot the intermediate state for
															// either the left or the right triangle
			if ( depth >= 4 ) {
				PlotPixelMapChoice ( IsLeftDisclosure () ?  disclosure_IntermediateLeft :
																	disclosure_IntermediateRight );
			} else {
				PlotPixelMapChoice ( IsLeftDisclosure () ?  disclosure_IntermediateLeftBW :
																	disclosure_IntermediateRightBW );
			}
															// Now we wipe the background between
															// states
			WipeBackground ();
															// Now plot the pressed state for the new
															// state which will provide a transition
															// into the final state
			if ( depth >= 4 ) {
				PlotPixelMapChoice ( IsLeftDisclosure () ?
												( IsSelected () ? disclosure_PressedDown :
																		disclosure_PressedLeft) :
												( IsSelected () ? disclosure_PressedDown :
																		disclosure_PressedRight ));
			} else {
				PlotPixelMapChoice ( IsLeftDisclosure () ?
												( IsSelected () ? disclosure_PressedDownBW :
																		disclosure_PressedLeftBW) :
												( IsSelected () ? disclosure_PressedDownBW :
																		disclosure_PressedRightBW ));
			}
															// Now we wipe the background again before
															// the final state is drawn
			WipeBackground ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ WipeBackground												  [public]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::WipeBackground()
{
	if ( IsVisible () && mControlPane->FocusDraw ()) {
		StColorPenState	theColorPenState;
		theColorPenState.Normalize ();
															// In order to make the transitions
															// between the various states smoother we
															// are going to attempt to figure out what
															// the background color is behind the
															// triangle, by using the current clipping
															// region we will subsequently use this
															// color to wipe the background
		Rect	localFrame;
		CalcLocalFrameRect ( localFrame );
		StRegion		currClip;
		::GetClip ( currClip );
		Rect clipRect = currClip.Bounds();
		::SectRect ( &localFrame, &clipRect, &localFrame );

															// Get the fore and back colors applied
		mControlPane->ApplyForeAndBackColors ();

															// Now we wipe the background with this
															// color by performing an erase rect
		::EraseRect ( &localFrame );
	}
}


// ---------------------------------------------------------------------------
//	¥ PlotPixelMapChoice										   [protected]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::PlotPixelMapChoice	(
		SInt16	inChoice )
{
														// Get the frame for the control which we
														// will use to plot the triangle
	Rect	localFrame;
	CalcLocalControlRect ( localFrame );
														// Plot the triangle by iterating over the
														// array of color choices that specify the
														// control
	SInt8 colorindex;
	for ( SInt16 i = 0; i < disclosure_Rows; i++ ) {
		for ( SInt16 j = 0; j < disclosure_Columns; j++ ) {
			RGBColor	tempColor;
			colorindex = disclosure_PixMapArray[inChoice][i][j];
			if ( colorindex != N ) {
				tempColor = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.left + j),
										(SInt16) (localFrame.top + i),
										tempColor );
			}
		}
	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGADisclosureTriangleImp::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
														// If the mouse moved in or out of the hot
														// spot handle the hiliting of the control
	if ( HasAutoToggle () && inCurrInside != inPrevInside ) {
 														// Because a disclosure triangle is basically
 														// just a switch we simply toggle the state
		SetPushedState ( inCurrInside );
														// Get the control redrawn so that any
														// hiliting changes can take effect, we also
														// will do a wipe before the state change is
														// drawn
		mControlPane->Draw ( nil );
	}
}


PP_End_Namespace_PowerPlant
