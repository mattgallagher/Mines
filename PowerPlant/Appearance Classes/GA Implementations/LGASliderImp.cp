// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGASliderImp.cp				PowerPlant 2.2.2	й1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGASliderImp.h>
#include <LView.h>
#include <UDrawingUtils.h>
#include <UGAColorRamp.h>
#include <UGraphicUtils.h>
#include <UMemoryMgr.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Constants for indicators
// ---------------------------------------------------------------------------

		// Color indices
const SInt16 		B	=	colorRamp_Black;	//	Index for black
const SInt16		W	=	colorRamp_White;	//	Index for white
const SInt16		N	=	-1;					//	Index for no color (draw nothing)


		// Indicator Choices
enum {
	indicator_VertRightPointingNormal	=	 0,
	indicator_VertRightPointingPushed	=	 1,
	indicator_VertRightPointingGhost	=	 2,
	indicator_VertRightPointingDimmed	=	 3,
	indicator_VertRightPointingBW		=	 4,
	indicator_VertRightPointingPushedBW	=	 5,
	indicator_VertRightPointingGhostBW	=	 6,

	indicator_HorizDownPointingNormal	=	 7,
	indicator_HorizDownPointingPushed	=	 8,
	indicator_HorizDownPointingGhost	=	 9,
	indicator_HorizDownPointingDimmed	=	10,
	indicator_HorizDownPointingBW		=	11,
	indicator_HorizDownPointingPushedBW	=	12,
	indicator_HorizDownPointingGhostBW	=	13,

	indicator_VertLeftPointingNormal	=	14,
	indicator_VertLeftPointingPushed	=	15,
	indicator_VertLeftPointingGhost		=	16,
	indicator_VertLeftPointingDimmed	=	17,
	indicator_VertLeftPointingBW		=	18,
	indicator_VertLeftPointingPushedBW	=	19,
	indicator_VertLeftPointingGhostBW	=	20,

	indicator_HorizUpPointingNormal		=	21,
	indicator_HorizUpPointingPushed		=	22,
	indicator_HorizUpPointingGhost		=	23,
	indicator_HorizUpPointingDimmed		=	24,
	indicator_HorizUpPointingBW			=	25,
	indicator_HorizUpPointingPushedBW	=	26,
	indicator_HorizUpPointingGhostBW	=	27,

	indicator_VertNonDirectNormal		=	28,
	indicator_VertNonDirectPushed		=	29,
	indicator_VertNonDirectGhost		=	30,
	indicator_VertNonDirectDimmed		=	31,
	indicator_VertNonDirectBW			=	32,
	indicator_VertNonDirectPushedBW		=	33,
	indicator_VertNonDirectGhostBW		=	34,

	indicator_HorizNonDirectNormal		=	35,
	indicator_HorizNonDirectPushed		=	36,
	indicator_HorizNonDirectGhost		=	37,
	indicator_HorizNonDirectDimmed		=	38,
	indicator_HorizNonDirectBW			=	39,
	indicator_HorizNonDirectPushedBW	=	40,
	indicator_HorizNonDirectGhostBW		=	41
};

		// Indicator Masks
enum {
	indicator_VertRightPointingMask		=	0,
	indicator_HorizDownPointingMask		=	1,
	indicator_VertLeftPointingMask		=	2,
	indicator_HorizUpPointingMask		=	3,
	indicator_VertNonDirectMask			=	4,
	indicator_HorizNonDirectMask		=	5
};


	//	Indicator PixMap Array
	//		NOTE:	all pixmaps have been set at 16x16 even though they vary
	//				from one type to the other, this just made it easier to
	//				load them all into a single array

const SInt8 indicator_Choices[42][16][16] =
{
	// ее VERTICAL RIGHT INDICATORS
	//  NORMAL COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ B,  1,  3,  3,  3,  3,  3,  3,  3,  3,  B,  N,  N,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  1,  3,  3,  3,  3,  3,  5,  5,  5,  8,  B,  N,  N },
		{ B,  3,  5,  5, 10, 10, 10, 10, 10, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  1,  3,  3,  3,  3,  3,  5,  5,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5, 10, 10, 10, 10, 10, 10,  5,  5,  5,  5,  8,  B },
		{ B,  3,  5,  1,  3,  3,  3,  3,  3,  5,  5,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5, 10, 10, 10, 10, 10, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  B,  N,  N,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  B,  N,  N,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  3,  5,  5,  5,  5,  5,  8,  8,  8, 10,  B,  N,  N },
		{ B,  5,  8,  8, 12, 12, 12, 12, 12, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  3,  5,  5,  5,  5,  5,  8,  8,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8, 12, 12, 12, 12, 12, 12,  8,  8,  8,  8, 10,  B },
		{ B,  5,  8,  3,  5,  5,  5,  5,  5,  8,  8,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8, 12, 12, 12, 12, 12, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N,  N },
		{ B,  5, 10, 10, 10, 10, 10, 10, 10, 10,  B,  N,  N,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST COLOR
	{
		{ N,  7,  7,  7, 11,  7,  7,  7, 11,  7,  N,  N,  N,  N,  N,  N },
		{ 7,  1,  W,  W,  2,  W,  W,  W,  2,  W,  7,  N,  N,  N,  N,  N },
		{ 7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  4,  7,  N,  N,  N,  N },
		{ 7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  W,  2,  W,  W,  W,  2,  2,  2,  2,  4,  7,  N,  N },
		{ 7,  W,  2,  2,  9,  5,  5,  5,  9,  5,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  W,  2,  W,  W,  W,  2,  2,  2,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  9,  5,  5,  5,  9,  5,  2,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  W,  2,  W,  W,  W,  2,  2,  2,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  9,  5,  5,  5,  9,  5,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  4,  7,  N,  N },
		{ 7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  4,  7,  N,  N,  N,  N },
		{ 7,  2,  4,  4,  7,  4,  4,  4,  7,  4,  7,  N,  N,  N,  N,  N },
		{ N,  7,  7,  7, 11,  7,  7,  7, 11,  7,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  DIMMED COLOR
	{
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N,  N,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N,  N },
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  NORMAL B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ B,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST/DIMMED B&W
	{
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N,  N,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// ее HORIZONTAL DOWN INDICATORS
	//  NORMAL COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5,  B,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  1,  5,  1,  5,  1,  5,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  5, 10,  5, 10,  5, 10,  5,  5,  5,  8,  B,  N },
		{ N,  B,  8,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N },
		{ N,  N,  B,  8,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ N,  N,  N,  B,  8,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  8,  5,  5,  5,  8,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  8,  8,  8,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  3,  8,  3,  8,  3,  8,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  8, 12,  8, 12,  8, 12,  8,  8,  8, 10,  B,  N },
		{ N,  B, 10,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N },
		{ N,  N,  B, 10,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ N,  N,  N,  B, 10,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B, 10,  8,  8,  8, 10,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B, 10, 10, 10,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST COLOR
	{
		{ N,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  N,  N },
		{ 7,  1,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  2,  7,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  1,  2,  1,  2,  1,  2,  2,  2,  2,  4,  7,  N },
		{ 11, 2,  5,  5,  2,  9,  2,  9,  2,  9,  5,  5,  5,  7, 11,  N },
		{ 7,  W,  2,  2,  W,  5,  W,  5,  W,  5,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  W,  5,  W,  5,  W,  5,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  W,  5,  W,  5,  W,  5,  2,  2,  2,  4,  7,  N },
		{ 11, 2,  5,  5,  2,  9,  2,  9,  2,  9,  5,  5,  5,  7, 11,  N },
		{ 7,  W,  2,  2,  2,  5,  2,  5,  2,  5,  2,  2,  2,  4,  7,  N },
		{ N,  7,  4,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N },
		{ N,  N,  7,  4,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ N,  N,  N,  7,  4,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N,  N },
		{ N,  N,  N,  N,  7,  4,  2,  2,  2,  4,  7,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  7,  4,  4,  4,  7,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  7,  7,  7,  N,  N,  N,  N,  N,  N,  N }
	},

	//  DIMMED COLOR
	{
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N },
		{ N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N },
		{ N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  8,  2,  2,  2,  8,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  8,  8,  8,  N,  N,  N,  N,  N,  N,  N }
	},

	//  NORMAL B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST/DIMMED B&W
	{
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ N,  N,  N,  N,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  W,  B,  W,  N,  N,  N,  N,  N,  N,  N }
	},

	// ее VERTICAL LEFT INDICATORS
	//  NORMAL - COLOR
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  B,  1,  3,  3,  3,  3,  3,  3,  3,  5,  B },
		{ N,  N,  N,  N,  B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  N,  B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  B,  3,  5,  5,  1,  3,  3,  3,  3,  3,  5,  5,  8,  B },
		{ N,  B,  3,  5,  5,  5,  5, 10, 10, 10, 10, 10, 10,  5,  8,  B },
		{ B,  1,  5,  5,  5,  5,  1,  3,  3,  3,  3,  3,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5,  5,  5, 10, 10, 10, 10, 10, 10,  5,  8,  B },
		{ B,  5,  5,  5,  5,  5,  1,  3,  3,  3,  3,  3,  5,  5,  8,  B },
		{ N,  B,  8,  5,  5,  5,  5, 10, 10, 10, 10, 10, 10,  5,  8,  B },
		{ N,  N,  B,  8,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  N,  B,  8,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  N,  N,  B,  8,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  N,  N,  N,  B,  8,  8,  8,  8,  8,  8,  8,  8,  8,  B },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED - COLOR
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  B,  3,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ N,  N,  N,  N,  B,  5,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ N,  N,  N,  B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ N,  N,  B,  5,  8,  8,  3,  5,  5,  5,  5,  5,  8,  8, 10,  B },
		{ N,  B,  5,  8,  8,  8,  8, 12, 12, 12, 12, 12, 12,  8, 10,  B },
		{ B,  3,  8,  8,  8,  8,  3,  5,  5,  5,  5,  5,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8,  8,  8, 12, 12, 12, 12, 12, 12,  8, 10,  B },
		{ B,  8,  8,  8,  8,  8,  3,  5,  5,  5,  5,  5,  8,  8, 10,  B },
		{ N,  B, 10,  8,  8,  8,  8, 12, 12, 12, 12, 12, 12,  8, 10,  B },
		{ N,  N,  B, 10,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ N,  N,  N,  B, 10,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ N,  N,  N,  N,  B, 10,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ N,  N,  N,  N,  N,  B, 10, 10, 10, 10, 10, 10, 10, 10, 10,  B },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST - COLOR
	{
		{ N,  N,  N,  N,  N,  N,  7, 11,  7,  7,  7, 11,  7,  7,  7,  N },
		{ N,  N,  N,  N,  N,  7,  W,  2,  W,  W,  W,  2,  W,  W,  2,  7 },
		{ N,  N,  N,  N,  7,  W,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7 },
		{ N,  N,  N,  7,  W,  2,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7 },
		{ N,  N,  7,  W,  2,  2,  W,  2,  W,  W,  W,  2,  2,  2,  4,  7 },
		{ N,  7,  W,  2,  2,  2,  2,  9,  5,  5,  5,  9,  5,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  2,  W,  2,  W,  W,  W,  2,  2,  2,  4,  7 },
		{ 7,  1,  2,  2,  2,  2,  2,  9,  5,  5,  5,  9,  5,  2,  4,  7 },
		{ 7,  2,  2,  2,  2,  2,  W,  2,  W,  W,  W,  2,  2,  2,  4,  7 },
		{ N,  7,  4,  2,  2,  2,  2,  9,  5,  5,  5,  9,  5,  2,  4,  7 },
		{ N,  N,  7,  4,  2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7 },
		{ N,  N,  N,  7,  4,  2,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7 },
		{ N,  N,  N,  N,  7,  4,  2,  5,  2,  2,  2,  5,  2,  2,  4,  7 },
		{ N,  N,  N,  N,  N,  7,  4,  7,  4,  4,  4,  7,  4,  4,  4,  7 },
		{ N,  N,  N,  N,  N,  N,  7, 11,  7,  7,  7, 11,  7,  7,  7,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  DIMMED - COLOR
	{
		{ N,  N,  N,  N,  N,  N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N },
		{ N,  N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  N,  N,  N,  N,  N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  NORMAL B&W
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  W,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  W,  W,  B },
		{ N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  PUSHED B&W
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  B,  B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  B,  B,  B },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	//  GHOST/DIMMED B&W
	{
		{ N,  N,  N,  N,  N,  N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  N,  N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  N,  N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  N,  N,  N,  N,  N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// ее HORIZONTAL UP INDICATORS
	//  NORMAL COLOR
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  1,  3,  3,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  3,  5,  5,  5,  8,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  B,  3,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N,  N },
		{ N,  N,  B,  3,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ N,  B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N },
		{ B,  1,  5,  5,  1,  5,  1,  5,  1,  5,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  3, 10,  3, 10,  3, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  5, 10,  5, 10,  5, 10,  5,  5,  5,  8,  B,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  B,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N }
	},

	//  PUSHED COLOR
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  3,  5,  8,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  5,  8,  8,  8, 10,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  B,  5,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N,  N },
		{ N,  N,  B,  5,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ N,  B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N },
		{ B,  3,  8,  8,  3,  8,  3,  8,  3,  8,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  5, 12,  5, 12,  5, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  8, 12,  8, 12,  8, 12,  8,  8,  8, 10,  B,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N },
		{ B,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  B,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N }
	},

	//  GHOST COLOR
	{
		{ N,  N,  N,  N,  N,  N,  7,  7,  7,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  7,  W,  1,  2,  7,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  7,  W,  2,  2,  2,  4,  7,  N,  N,  N,  N,  N },
		{ N,  N,  N,  7,  W,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N,  N },
		{ N,  N,  7,  W,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ N,  7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N },
		{ 7,  1,  2,  2,  W,  2,  W,  2,  W,  2,  2,  2,  2,  4,  7,  N },
		{11,  2,  5,  5,  2,  9,  2,  9,  2,  9,  5,  5,  5,  7, 11,  N },
		{ 7,  W,  2,  2,  W, 10,  W, 10,  W, 10,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  W, 10,  W, 10,  W, 10,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  W, 10,  W, 10,  W, 10,  2,  2,  2,  4,  7,  N },
		{11,  2,  5,  5,  2,  9,  2,  9,  2,  9,  5,  5,  5,  7, 11,  N },
		{ 7,  W,  2,  2,  2, 10,  2, 10,  2, 10,  2,  2,  2,  4,  7,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N },
		{ 7,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  7,  N },
		{ N,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  N,  N }
	},

	//  DIMMED COLOR
	{
		{ N,  N,  N,  N,  N,  N,  8,  8,  8,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  8,  2,  2,  2,  8,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  8,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N,  N },
		{ N,  N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N,  N },
		{ N,  N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ N,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N },
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N }
	},

	//  NORMAL B&W
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N }
	},

	//  PUSHED B&W
	{
		{ N,  N,  N,  N,  N,  N,  B,  B,  B,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N,  N },
		{ N,  N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ N,  N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N }
	},

	//  GHOST/DIMMED B&W
	{
		{ N,  N,  N,  N,  N,  N,  W,  B,  W,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  B,  W,  W,  W,  B,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N,  N,  N },
		{ N,  N,  N,  B,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N,  N },
		{ N,  N,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ N,  B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N },
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N }
	},

	// ее VERTICAL NON-DIRECTIONAL
	// NORMAL COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5,  B },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5,  1,  3,  3,  3,  3,  3,  3,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5, 10, 10, 10, 10, 10, 10, 10,  5,  5,  8,  B },
		{ B,  3,  5,  5,  1,  3,  3,  3,  3,  3,  3,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5, 10, 10, 10, 10, 10, 10, 10,  5,  5,  8,  B },
		{ B,  3,  5,  5,  1,  3,  3,  3,  3,  3,  3,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5, 10, 10, 10, 10, 10, 10, 10,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  B },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// PUSHED COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  1,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8,  1,  5,  5,  5,  5,  5,  5,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8, 12, 12, 12, 12, 12, 12, 12,  8,  8, 10,  B },
		{ B,  5,  8,  8,  1,  5,  5,  5,  5,  5,  5,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8, 12, 12, 12, 12, 12, 12, 12,  8,  8, 10,  B },
		{ B,  5,  8,  8,  1,  5,  5,  5,  5,  5,  5,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8, 12, 12, 12, 12, 12, 12, 12,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B },
		{ B,  5, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  B },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// GHOST COLOR
	{
		{ N,  7,  7,  7,  7, 11,  7,  7,  7, 11,  7,  7,  7,  7,  7,  N },
		{ 7,  1,  W,  W,  W,  2,  W,  W,  W,  2,  W,  W,  W,  W,  2,  7 },
		{ 7,  W,  2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  W,  2,  W,  W,  W,  2,  W,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  9,  5,  5,  5,  9,  5,  5,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  W,  2,  W,  W,  W,  2,  W,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  9,  5,  5,  5,  9,  5,  5,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  W,  2,  W,  W,  W,  2,  W,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  9,  5,  5,  5,  9,  5,  5,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  2,  4,  7 },
		{ 7,  W,  2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  2,  4,  7 },
		{ 7,  2,  4,  4,  4,  7,  4,  4,  4,  7,  4,  4,  4,  4,  4,  7 },
		{ N,  7,  7,  7,  7, 11,  7,  7,  7, 11,  7,  7,  7,  7,  7,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// DIMMED COLOR
	{
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// NORMAL B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  B,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  B,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  B,  B,  B,  B,  B,  B,  B,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// PUSHED B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  W,  W,  W,  W,  W,  W,  W,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// GHOST/DIMMED B&W
	{
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W },
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N },
		{ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  N }
	},

	// HORIZONTAL NON-DIRECTIONAL
	// NORMAL COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5,  B,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  1,  5,  1,  5,  1,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  3, 10,  3, 10,  3, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  5, 10,  5, 10,  5, 10,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  B,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N }
	},

	// PUSHED COLOR
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  8,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  3,  8,  3,  8,  3,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  5, 12,  5, 12,  5, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  8, 12,  8, 12,  8, 12,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8, 10,  B,  N,  N,  N },
		{ B,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  B,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N }
	},

	// GHOST COLOR
	{
		{ N,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  N,  N,  N,  N },
		{ 7,  1,  W,  W,  W,  W,  W,  W,  W,  W,  W,  2,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  W,  2,  W,  2,  W,  2,  2,  2,  4,  7,  N,  N,  N },
		{11,  2,  5,  2,  9,  2,  9,  2,  9,  5,  5,  7, 11,  N,  N,  N },
		{ 7,  W,  2,  W,  5,  W,  5,  W,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  W,  5,  W,  5,  W,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  W,  5,  W,  5,  W,  5,  2,  2,  4,  7,  N,  N,  N },
		{11,  2,  5,  2,  9,  2,  9,  2,  9,  5,  5,  7, 11,  N,  N,  N },
		{ 7,  W,  2,  W,  5,  W,  5,  W,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  5,  2,  5,  2,  5,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  W,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  7,  N,  N,  N },
		{ 7,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  7,  N,  N,  N },
		{ N,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  N,  N,  N,  N }
	},

	// DIMMED COLOR
	{
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ 8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  8,  N,  N,  N },
		{ N,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  N,  N,  N,  N }
	},

	// NORMAL B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  B,  W,  B,  W,  B,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N }
	},

	// PUSHED B&W
	{
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  W,  B,  W,  B,  W,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N },
		{ N,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  B,  N,  N,  N,  N }
	},

	// GHOST/DIMMED B&W
	{
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  N,  N,  N },
		{ B,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  B,  N,  N,  N },
		{ N,  W,  B,  W,  B,  W,  B,  W,  B,  W,  B,  W,  N,  N,  N,  N }
	},
};


			// Indicator masks. All masks are 16 x 16 pixels

const UInt16	indicator_Masks[6][16] =
{
	// Vertical Right
	{ 0x7FC0, 0xFFE0, 0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF, 0xFFFF,
	  0xFFFF, 0xFFFE, 0xFFFC, 0xFFF8, 0xFFF0, 0xFFE0, 0x7FC0, 0x0000 },

	// Horizontal Down
	{ 0x7FFC, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
	  0xFFFE, 0xFFFE, 0x7FFC, 0x3FF8, 0x1FF0, 0x0FE0, 0x07C0, 0x0380 },

	// Vertical Left
	{ 0x03FE, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF, 0xFFFF,
	  0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF, 0x0FFF, 0x07FF, 0x03FE, 0x0000 },

	// Horizontal Up
	{ 0x0380, 0x07C0, 0x0FE0, 0x1FF0, 0x3FF8, 0x7FFC, 0xFFFE, 0xFFFE,
	  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0x7FFC },

	// Vertical Non-Directional
	{ 0x7FFE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7FFE, 0x0000, 0x0000, 0x0000 },

	// Horizontal Non-Directional
	{ 0x7FF0, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8,
	  0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0xFFF8, 0x7FF0 }
};


// ---------------------------------------------------------------------------
//	е LGASliderImp							Contstructor			  [public]
// ---------------------------------------------------------------------------

LGASliderImp::LGASliderImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
	mThumbWorld			= nil;
	mNumTickMarks		= 0;
	mLastPosition		= 0;
	mHorizontal			= false;
	mHasLiveFeedback	= false;
	mNonDirectional		= false;
	mReverseDirection	= false;
}


// ---------------------------------------------------------------------------
//	е LGASliderImp							Stream Contstructor		  [public]
// ---------------------------------------------------------------------------

LGASliderImp::LGASliderImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mThumbWorld			= nil;
	mNumTickMarks		= 0;
	mLastPosition		= 0;
	mHorizontal			= false;
	mHasLiveFeedback	= false;
	mNonDirectional		= false;
	mReverseDirection	= false;
}


// ---------------------------------------------------------------------------
//	е LGASliderImp							Destructor				  [public]
// ---------------------------------------------------------------------------

LGASliderImp::~LGASliderImp()
{
}


// ---------------------------------------------------------------------------
//	е Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::Init(
	LControlPane*	/* inControlPane */,
	LStream*		/* inStream */)
{										// This function is never called
}


// ---------------------------------------------------------------------------
//	е Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;

	SDimension16	frameSize;
	mControlPane->GetFrameSize(frameSize);

	mHorizontal = frameSize.width > frameSize.height;

	mHasLiveFeedback  = (inControlKind & kControlSliderLiveFeedback) != 0;
	mReverseDirection = (inControlKind & kControlSliderReverseDirection) != 0;
	mNonDirectional   = (inControlKind & kControlSliderNonDirectional) != 0;

	bool hasTickMarks = (inControlKind & kControlSliderHasTickMarks) != 0;

	if (mNonDirectional) {			// NonDirectional Slider can't have
		hasTickMarks = false;		//   tick marks or a reversed direction
		mReverseDirection = false;
	}

	if (hasTickMarks) {
		mNumTickMarks = (SInt16) mControlPane->GetValue();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::DrawSelf()
{
	SInt16			depth = mControlPane->GetBitDepth();

	DrawIndicatorTrack(depth);

	Rect	indicatorRect;
	CalcIndicatorRect(indicatorRect,
						ValueToPosition(mControlPane->GetValue()));
	DrawIndicator(depth, indicatorRect);

	if (mNumTickMarks > 1) {
		DrawTickMarks(depth);
	}
}


// ---------------------------------------------------------------------------
//	е DrawIndicatorTrack											  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::DrawIndicatorTrack(
	SInt16		inDepth)
{
	StColorPenState::Normalize();

	Rect	trackRect;
	CalcTrackRect(trackRect);

	bool	enabledAndActive = IsEnabled() && IsActive();

	if (inDepth < 4) {		// Black and White
		::MacOffsetRect(&trackRect, 1, 1);

		if (!enabledAndActive) {
			StColorPenState::SetGrayPattern();
		}

		::FrameRoundRect(&trackRect, 4, 4);
		::MacInsetRect(&trackRect, 1, 1);
		::EraseRoundRect(&trackRect, 4, 4);

	} else {				// Color
		RGBColor	theColor;
		bool		enabledAndActive = IsEnabled() && IsActive();

		if (enabledAndActive) {
									// Gray top edge
			trackRect.right++;
			trackRect.bottom++;
			theColor = UGAColorRamp::GetColor(5);
			::RGBForeColor(&theColor);
			::FrameRoundRect(&trackRect, 4, 4);
			UGraphicUtils::PaintColorPixel( (SInt16) (trackRect.left + 1),
											(SInt16) (trackRect.top + 1),
											theColor);

									// White bottom edge
			theColor = Color_White;
			::RGBForeColor(&theColor);
			::MacOffsetRect(&trackRect, 1, 1);
			::FrameRoundRect(&trackRect, 4, 4);
			UGraphicUtils::PaintColorPixel( (SInt16) (trackRect.right - 2),
											(SInt16) (trackRect.bottom - 2),
											theColor);

									// Fixup some pixels
			if (mHorizontal) {
				theColor = UGAColorRamp::GetColor(5);
			}
			UGraphicUtils::PaintColorPixel( (SInt16) (trackRect.right - 2),
											trackRect.top,
											theColor);
			trackRect.right--;
			trackRect.bottom--;

		} else {
			::MacOffsetRect(&trackRect, 1, 1);
		}

									// Draw the Track

		SInt8	paintIndex = 4;		// Color of interior and border
		SInt8	frameIndex = 8;		//   depends on whether the slider
		if (enabledAndActive) {		//   is enabled and active
			paintIndex = 5;
			frameIndex = 12;
		}

		theColor = UGAColorRamp::GetColor(paintIndex);	// Interior
		::RGBForeColor(&theColor);
		::PaintRoundRect(&trackRect, 4, 4);

		theColor = UGAColorRamp::GetColor(frameIndex);	// Border
		::RGBForeColor(&theColor);
		::FrameRoundRect(&trackRect, 4, 4);
	}
}


// ---------------------------------------------------------------------------
//	е DrawIndicator													  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::DrawIndicator(
	SInt16			inDepth,
	const Rect&		inIndicatorRect,
	bool			inGhost,
	bool			inPressed)
{
	SInt16		indicatorChoice = -1;

	if (inDepth < 4) {
		bool	ghostOrDimmed = inGhost || !(IsActive() && IsEnabled());

		if ( mNonDirectional) {
			if (mHorizontal) {
				if (ghostOrDimmed) {
					indicatorChoice = indicator_HorizNonDirectGhostBW;

				} else if (inPressed) {
					indicatorChoice = indicator_HorizNonDirectPushedBW;

				} else {
					indicatorChoice =	indicator_HorizNonDirectBW;
				}

			} else {	// Vertical
				if (ghostOrDimmed) {
					indicatorChoice = indicator_VertNonDirectGhostBW;

				} else if (inPressed) {
					indicatorChoice = indicator_VertNonDirectPushedBW;

				} else {
					indicatorChoice =	indicator_VertNonDirectBW;
				}
			}

		} else {
			if (mHorizontal) {
				if (mReverseDirection) {
					if (ghostOrDimmed) {
						indicatorChoice = indicator_HorizUpPointingGhostBW;

					} else if (inPressed) {
						indicatorChoice = indicator_HorizUpPointingPushedBW;

					} else {
						indicatorChoice =	indicator_HorizUpPointingBW;
					}

				} else {
					if (ghostOrDimmed) {
						indicatorChoice = indicator_HorizDownPointingGhostBW;

					} else if (inPressed) {
						indicatorChoice = indicator_HorizDownPointingPushedBW;

					} else {
						indicatorChoice =	indicator_HorizDownPointingBW;
					}
				}

			} else {
				if (mReverseDirection) {
					if (ghostOrDimmed) {
						indicatorChoice = indicator_VertLeftPointingGhostBW;

					} else if (inPressed) {
						indicatorChoice = indicator_VertLeftPointingPushedBW;

					} else {
						indicatorChoice =	indicator_VertLeftPointingBW;
					}

				} else {
					if (ghostOrDimmed) {
						indicatorChoice = indicator_VertRightPointingGhostBW;

					} else if (inPressed) {
						indicatorChoice = indicator_VertRightPointingPushedBW;

					} else {
						indicatorChoice =	indicator_VertRightPointingBW;
					}
				}
			}
		}

	} else {		// Color

		if (IsActive() && IsEnabled()) {
			if (inGhost) {
				if ( mNonDirectional) {
					indicatorChoice =	indicator_VertNonDirectGhost;
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizNonDirectGhost;
					}

				} else {
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizDownPointingGhost;
						if (mReverseDirection) {
							indicatorChoice =	indicator_HorizUpPointingGhost;
						}

					} else {
						indicatorChoice =	indicator_VertRightPointingGhost;
						if (mReverseDirection) {
							indicatorChoice =	indicator_VertLeftPointingGhost;
						}

					}
				}

			} else if (inPressed) {
				if ( mNonDirectional) {
					indicatorChoice =	indicator_VertNonDirectPushed;
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizNonDirectPushed;
					}

				} else {
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizDownPointingPushed;
						if (mReverseDirection) {
							indicatorChoice =	indicator_HorizUpPointingPushed;
						}

					} else {
						indicatorChoice =	indicator_VertRightPointingPushed;
						if (mReverseDirection) {
							indicatorChoice =	indicator_VertLeftPointingPushed;
						}
					}
				}

			} else {
				if ( mNonDirectional) {
					indicatorChoice =	indicator_VertNonDirectNormal;
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizNonDirectNormal;
					}

				} else {
					if (mHorizontal) {
						indicatorChoice =	indicator_HorizDownPointingNormal;
						if (mReverseDirection) {
							indicatorChoice =	indicator_HorizUpPointingNormal;
						}

					} else {
						indicatorChoice =	indicator_VertRightPointingNormal;
						if (mReverseDirection) {
							indicatorChoice =	indicator_VertLeftPointingNormal;
						}
					}
				}
			}

		} else {
			if ( mNonDirectional) {
				indicatorChoice =	indicator_VertNonDirectDimmed;
				if (mHorizontal) {
					indicatorChoice =	indicator_HorizNonDirectDimmed;
				}

			} else {
				if (mHorizontal) {
					indicatorChoice =	indicator_HorizDownPointingDimmed;
					if (mReverseDirection) {
						indicatorChoice =	indicator_HorizUpPointingDimmed;
					}

				} else {
					indicatorChoice =	indicator_VertRightPointingDimmed;
					if (mReverseDirection) {
						indicatorChoice =	indicator_VertLeftPointingDimmed;
					}
				}
			}
		}
	}

			// Plot the choice of indicator
	StColorPenState::Normalize();
	PlotPixMap(indicatorChoice, 16, 16, inIndicatorRect);
}


// ---------------------------------------------------------------------------
//	е EraseIndicator											   [protected]
// ---------------------------------------------------------------------------

void
LGASliderImp::EraseIndicator(
	SInt16		inDepth,
	const Rect	&inIndicatorRect)
{
		// Erase indicator by drawing what's behind it, which
		// is the track and background. To avoid flicker, we draw
		// the track first, then remove the track from the clipping
		// region so that we can draw the background by just erasing.

	DrawIndicatorTrack(inDepth);		// First draw the track

	Rect	trackRect;
	CalcTrackRect(trackRect);

	if (inDepth < 4) {
		::MacOffsetRect(&trackRect, 1, 1);	// BW Track is offset by 1 pixel

	} else {
		trackRect.right += 2;			// Track has 2 pixels of shading
		trackRect.bottom += 2;			//   (1 pixel on each edge)
	}

	StClipRgnState::DiffWithCurrent(trackRect);

	mControlPane->ApplyForeAndBackColors();
	::EraseRect(&inIndicatorRect);		// Erase to background color

}


// ---------------------------------------------------------------------------
//	е MoveIndicator												   [protected]
// ---------------------------------------------------------------------------

void
LGASliderImp::MoveIndicator(
	SInt16		inFromPos,
	SInt16		inToPos,
	bool		inGhost,
	bool		inPressed)
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	StColorDrawLoop	theLoop(frame);
	SInt16			depth;
	while (theLoop.NextDepth(depth)) {

		Rect	toRect;
		CalcIndicatorRect(toRect, inToPos);

		StRegion	toPosRgn(BuildIndicatorRgn(toRect), false);

		if (inFromPos != inToPos) {			// Indicator is moving

			Rect	fromRect;
			CalcIndicatorRect(fromRect, inFromPos);

			StClipRgnState	clipRgn;
											// Exclude new indicator location
			clipRgn.DiffWithCurrent(toPosRgn);

											// Erase indicator at old location
			EraseIndicator(depth, fromRect);
		}
											// Draw indicator at new location

		if (mThumbWorld == nil) {				// No GWorld. Draw manually
			DrawIndicator(depth, toRect, inGhost, inPressed);

		} else {								// There's a GWorld for the
			StColorState	colors;				//   thumb. Use it to draw.
			colors.Normalize();
			mThumbWorld->CopyImage(UQDGlobals::GetCurrentPort(), toRect,
									srcCopy, toPosRgn);
		}
	}
}


// ---------------------------------------------------------------------------
//	е DrawTickMarks													  [public]
// ---------------------------------------------------------------------------
//	Draw all tick marks

void
LGASliderImp::DrawTickMarks(
	SInt16		inDepth)
{
	SInt16	trackLength = (SInt16) CalcTrackLength();
	SInt16	divisor		= (SInt16) (mNumTickMarks - 1);

	Rect	markRect;
	CalcTickMarkRect(markRect);

	::PenNormal();
	if ((inDepth <= 4) && !(IsActive() && IsEnabled())) {
		StColorPenState::SetGrayPattern();
	}

	for (SInt16 i = 0; i < mNumTickMarks; i++) {
		DrawOneTickMark(inDepth, markRect, (SInt16) (i * trackLength / divisor));
	}
}


// ---------------------------------------------------------------------------
//	е DrawOneTickMark												  [public]
// ---------------------------------------------------------------------------
//	Draw a single tick mark at the specified position

void
LGASliderImp::DrawOneTickMark(
	SInt16		inDepth,
	const Rect	&inMarkRect,
	SInt16		inPosition)
{
	RGBColor	theColor;
	Rect		tickRect = inMarkRect;

	if (mHorizontal) {
		tickRect.left += inPosition + 1;
		tickRect.right = (SInt16) (tickRect.left + 3);

	} else {
		tickRect.top += inPosition + 1;
		tickRect.bottom  = (SInt16) (tickRect.top + 3);
	}

	if ((inDepth > 4) && IsActive() && IsEnabled()) {
		theColor = Color_White;			// White hilite on top and left
		::RGBForeColor(&theColor);
		::MoveTo( tickRect.left,		(SInt16) (tickRect.bottom - 2) );
		::MacLineTo( tickRect.left,		tickRect.top );
		::MacLineTo( (SInt16) (tickRect.right - 2),	tickRect.top );

		theColor = UGAColorRamp::GetColor(7);
		::RGBForeColor(&theColor);		// Gray shadow on bottom and right
		::MoveTo( (SInt16) (tickRect.right - 1),	(SInt16) (tickRect.top + 1) );
		::MacLineTo( (SInt16) (tickRect.right - 1),	(SInt16) (tickRect.bottom - 1) );
		::MacLineTo( (SInt16) (tickRect.left + 1),	(SInt16) (tickRect.bottom - 1) );
	}

	theColor = Color_Black;
	if ((inDepth > 4) && !(IsActive() && IsEnabled())) {
		theColor = UGAColorRamp::GetColor(8);
	}

	::RGBForeColor(&theColor);
	::MoveTo( (SInt16) (tickRect.left + 1), (SInt16) (tickRect.bottom - 2) );

	if (mHorizontal) {
		::MacLineTo( (SInt16) (tickRect.left + 1), (SInt16) (tickRect.top + 1) );
	} else {
		::MacLineTo( (SInt16) (tickRect.right - 2), (SInt16) (tickRect.top + 1) );
	}
}


// ---------------------------------------------------------------------------
//	е PlotPixMap													  [public]
// ---------------------------------------------------------------------------
//	Plots a given indicator's pixmap

void
LGASliderImp::PlotPixMap(
	SInt16			inChoice,
	SInt16			inRowCount,
	SInt16			inColumnCount,
	const Rect		&inFrame)
{
	SInt8 colorindex;
	for ( SInt16 i = 0; i < inRowCount; i++ ) {
		for ( SInt16 j = 0; j < inColumnCount; j++ ) {
			RGBColor	theColor;
			colorindex = indicator_Choices[inChoice][i][j];
			if ( colorindex != N ) {
				theColor = UGAColorRamp::GetColor(colorindex);
				UGraphicUtils::PaintColorPixel( (SInt16) (inFrame.left + j),
												(SInt16) (inFrame.top + i),
												theColor );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	е BuildIndicatorRgn												  [public]
// ---------------------------------------------------------------------------
//	Creates a region for the current orientation of the indicator using the
//	mask array

RgnHandle
LGASliderImp::BuildIndicatorRgn(
	const Rect&		inFrame)
{
	SInt16	indicatorChoice;		// Determine type of indicator

	if ( mNonDirectional) {
		indicatorChoice =	indicator_VertNonDirectMask;
		if (mHorizontal) {
			indicatorChoice =	indicator_HorizNonDirectMask;
		}

	} else {
		if (mHorizontal) {
			indicatorChoice =	indicator_HorizDownPointingMask;
			if (mReverseDirection) {
				indicatorChoice =	indicator_HorizUpPointingMask;
			}

		} else {
			indicatorChoice =	indicator_VertRightPointingMask;
			if (mReverseDirection) {
				indicatorChoice =	indicator_VertLeftPointingMask;
			}
		}
	}

									// Fill in BitMap struct for the
	BitMap	maskMap;				//   appropriate mask
	maskMap.baseAddr = (Ptr) indicator_Masks[indicatorChoice];
	maskMap.rowBytes = 2;			// Always 16 bits wide
	maskMap.bounds = inFrame;

	StRegion	indicatorRgn;		// Create a region from the bitmap
	::BitMapToRegion(indicatorRgn, &maskMap);

	return indicatorRgn.Release();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е FindHotSpot													  [public]
// ---------------------------------------------------------------------------
//	Return hot spot containing the specified point

SInt16
LGASliderImp::FindHotSpot(
	Point		inPoint) const
{									// Only Slider hot spot is the indicator
	return PointInHotSpot(inPoint, kControlIndicatorPart)
					? kControlIndicatorPart
					: kControlNoPart;
}


// ---------------------------------------------------------------------------
//	е PointInHotSpot												  [public]
// ---------------------------------------------------------------------------
//	Return whether a point is within the specified hot spot

Boolean
LGASliderImp::PointInHotSpot(
	Point		inPoint,
	SInt16		inHotSpot) const
{
	Boolean	inside = false;

	if (inHotSpot == kControlIndicatorPart) {	// Only Slider hot spot is
		Rect	indicatorRect;					//   the indicator
		CalcIndicatorRect(	indicatorRect,
							ValueToPosition(mControlPane->GetValue()));
		inside = ::MacPtInRect(inPoint, &indicatorRect);
	}

	return inside;
}


// ---------------------------------------------------------------------------
//	е TrackHotSpot													  [public]
// ---------------------------------------------------------------------------
//	Track mouse while it is being pressed within a hot spot

Boolean
LGASliderImp::TrackHotSpot(
	SInt16		/* inHotSpot */,
	Point		inPoint,
	SInt16		/* inModifiers */)
{
	StColorPenState	theColorPenState;

	SInt16	startThumb = ValueToPosition(mControlPane->GetValue());

										// Draw pressed thumb
	MoveIndicator(startThumb, startThumb, false, true);
	mPushed = true;

	Rect	thumbRect;
	CalcIndicatorRect(thumbRect, startThumb);

	if (!mHasLiveFeedback) {			// For non-live thumb, remove
										//   current thumb area from clip
										//   region so that it doesn't get
										//   erased while drawing the ghost
										//   image during tracking
		StRegion	thumbRgn( BuildIndicatorRgn(thumbRect) );
		StClipRgnState::DiffWithCurrent(thumbRgn);
		LView::OutOfFocus(nil);
	}

										// Stack-based object to delete thumb
										//   in case an exception occurs
										//   while tracking
	StDeleter<LGWorld>	thumbWorldDeleter;

	try {								// For smooth drawing, make a GWorld
		mThumbWorld = nil;				//   containing the thumb image
		mThumbWorld = new LGWorld(thumbRect);
		thumbWorldDeleter.Adopt(mThumbWorld);
		mThumbWorld->BeginDrawing();

		DrawIndicator( ::GetPixDepth( ::GetPortPixMap(mThumbWorld->GetMacGWorld() ) ),
						thumbRect, !mHasLiveFeedback, true);

		mThumbWorld->EndDrawing();
	}

	catch (...) {						// GWorld failed. Clean up and
		delete mThumbWorld;				//    continue. Drawing will be done
		mThumbWorld = nil;				//    manually without the GWorld.
		thumbWorldDeleter.Adopt(nil);
	}

	SInt16	thumbMin, thumbMax;
	FindIndicatorLimits(thumbMin, thumbMax);

		// The thumb follows the mouse while the mouse is inside the
		// slop rect. If the mouse leaves the slop rect, the thumb
		// returns to its original position.

	Rect	slopRect;					// Slop is within 20 pixels of the
	CalcTrackRect(slopRect);			//   track rect. This is close to
	::MacInsetRect(&slopRect, -20, -20);	//   what the AM uses.

	SInt16	currPos = startThumb;

	while (::StillDown()) {				// Track mouse while it's down
		Point	currentPt;
		::GetMouse(&currentPt);

		SInt16	prevPos = currPos;
		currPos = startThumb;

		if (::MacPtInRect(currentPt, &slopRect)) {
										// Mouse is inside our slop rect
			if (mHorizontal) {			// See how far it has moved
				currPos += currentPt.h - inPoint.h;
			} else {
				currPos += currentPt.v - inPoint.v;
			}

			if (mHorizontal) {			// Enforce horizontal limits
				if (currPos < thumbMin) {
					currPos = thumbMin;
				} else if (currPos > thumbMax) {
					currPos = thumbMax;
				}

			} else {					// Enforce vertical limits
				if (currPos > thumbMin) {
					currPos = thumbMin;
				} else if (currPos < thumbMax) {
					currPos = thumbMax;
				}
			}
		}

		if (currPos != prevPos) {		// Mouse has moved
										// Move Thumb to follow mouse
			MoveIndicator(prevPos, currPos, !mHasLiveFeedback, true);

			if (mHasLiveFeedback) {		// Change slider value
				mControlPane->SetValue(PositionToValue(currPos));
				mControlPane->FocusDraw();
			}
		}
	}

	thumbWorldDeleter.Adopt(nil);		// Done tracking. Delete GWorld
	mThumbWorld = nil;					//   for pressed thumb

	mPushed = false;
	mControlPane->FocusDraw();

	if (currPos != startThumb) {		// Thumb moved during tracking
		MoveIndicator(currPos, ValueToPosition(PositionToValue(currPos)));
		mControlPane->SetValue(PositionToValue(currPos));

	} else {							// Thumb didn't move, but we need
										//   to redraw thumb as unpressed
		MoveIndicator(startThumb, startThumb, false, false);
	}

	return false;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::SetValue(
	SInt32		inValue)
{
	SInt32	oldValue = mControlPane->GetValue();

	if ( !IsPushed()  &&
		 (inValue != oldValue)  &&
		 mControlPane->FocusExposed() ) {

		MoveIndicator(ValueToPosition(oldValue), ValueToPosition(inValue));
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е CalcIndicatorRect												  [public]
// ---------------------------------------------------------------------------

void
LGASliderImp::CalcIndicatorRect(
	Rect		&outRect,
	SInt16		inPosition) const
{
	Rect	trackRect;
	CalcTrackRect(trackRect);

	if (mNonDirectional) {				// Rectangular indicator
		if (mHorizontal) {
			outRect.left	= inPosition;
			outRect.right	= (SInt16) (inPosition + 13);
			outRect.top		= (SInt16) (trackRect.top - 4);
			outRect.bottom	= (SInt16) (outRect.top + 16);

		} else {
			outRect.bottom	= inPosition;
			outRect.top		= (SInt16) (inPosition - 13);
			outRect.left	= (SInt16) (trackRect.left - 4);
			outRect.right	= (SInt16) (outRect.left + 16);
		}

	} else {							// Indicator points ...
		if (mHorizontal) {
			outRect.left	= inPosition;
			outRect.right	= (SInt16) (inPosition + 15);

			if (mReverseDirection) {		// Up
				outRect.top = (SInt16) (trackRect.top - 6);
			} else {						// Down
				outRect.top = (SInt16) (trackRect.top - 3);
			}

			outRect.bottom	= (SInt16) (outRect.top + 16);

		} else {
			outRect.bottom	= inPosition;
			outRect.top		= (SInt16) (inPosition - 15);

			if (mReverseDirection) {		// Left
				outRect.left = (SInt16) (trackRect.left - 6);
			} else {						// Right
				outRect.left = (SInt16) (trackRect.left - 3);
			}

			outRect.right = (SInt16) (outRect.left + 16);
		}
	}
}


// ---------------------------------------------------------------------------
//	е CalcTrackRect													  [public]
// ---------------------------------------------------------------------------
//	Pass back the bounds of the track in which the indicator moves

void
LGASliderImp::CalcTrackRect(
	Rect		&outRect) const
{
	CalcLocalFrameRect(outRect);

	if (mNonDirectional) {				// Indicator is a rectangle
		if (mHorizontal) {
			outRect.top	   += 4;
			outRect.bottom	= (SInt16) (outRect.top + 5);
			outRect.right  -= 2;

		} else {
			outRect.left   += 4;
			outRect.bottom -= 2;
			outRect.right	= (SInt16) (outRect.left + 5);
		}

	} else {							// Indicator points ...
		if (mHorizontal) {
			if (mReverseDirection) {		// Up
				outRect.bottom -= 5;
				outRect.top		= (SInt16) (outRect.bottom - 5);
				outRect.right  -= 2;

			} else {						// Down
				outRect.top	   += 3;
				outRect.right  -= 2;
				outRect.bottom  = (SInt16) (outRect.top + 5);
			}

		} else {
			if (mReverseDirection) {		// Left
				outRect.right  -= 5;
				outRect.left    = (SInt16) (outRect.right - 5);
				outRect.bottom -= 2;

			} else {						// Right
				outRect.left   += 3;
				outRect.bottom -= 2;
				outRect.right   = (SInt16) (outRect.left + 5);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	е CalcTickMarkRect												  [public]
// ---------------------------------------------------------------------------
//	Pass back the bounds of area in which the tick marks draw

void
LGASliderImp::CalcTickMarkRect(
	Rect		&outRect) const
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	outRect = frame;

	if (mHorizontal) {				// Horizontal with Indicator pointing ...
		SInt16	width  = 15;
		SInt16	height = 16;

		outRect.left  += 3 + width / 2;
		outRect.right -= 6 + width / 2;

		if (mReverseDirection) {		// Up
			height = 17;

			outRect.bottom -= height;
			outRect.top     = (SInt16) (outRect.bottom - 8);

		} else {						// Down
			outRect.top   += height;
			outRect.bottom = (SInt16) (outRect.top + 8);
		}

	} else {						// Vertical with Indicator pointing ...
		SInt16	height = 15;
		SInt16	width  = 16;

		outRect.top    += 3 + height / 2;
		outRect.bottom -= 8 + height / 2;

		if (mReverseDirection) {		// Left
			width = 17;

			outRect.right -= width;
			outRect.left   = (SInt16) (outRect.right - 8);

		} else {						// Right
			outRect.left += width;
			outRect.right = (SInt16) (outRect.left + 8);
		}
	}

	::SectRect(&frame, &outRect, &outRect);	// Don't go outside of frame
}


// ---------------------------------------------------------------------------
//	е CalcTrackLength												  [public]
// ---------------------------------------------------------------------------
//	Calculate the length in pixels of the track in which the indicator moves
//
//	At the max value, the indicator will be this many pixels away from where
//	it is at the min value.

SInt32
LGASliderImp::CalcTrackLength() const
{
	Rect	frame;
	CalcLocalFrameRect(frame);
										// Directional indicator is 15 pixels
										//   thick with 5 pixel border on
										//   each side
	SInt32 length = frame.bottom - frame.top - 25;
	if (mHorizontal) {
		length = frame.right - frame.left - 25;
	}

	if (mNonDirectional) {				// NonDirectional indicator is
		length += 2;					//   2 pixels thinner
	}

	return length;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е FindIndicatorLimits										   [protected]
// ---------------------------------------------------------------------------
//	Determine the pixel location of the edge (left or bottom) of the
//	indicator when it is at its minimum and maximum position

void
LGASliderImp::FindIndicatorLimits(
	SInt16	&outMin,
	SInt16	&outMax) const
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	outMin = (SInt16) (frame.bottom - 5);
	outMax = (SInt16) (frame.top + 20);
	if (mHorizontal) {
		outMin = (SInt16) (frame.left + 5);
		outMax = (SInt16) (frame.right - 20);
	}

	if (mNonDirectional) {
		if (mHorizontal) {
			outMax += 2;
		} else {
			outMax -= 2;
		}
	}
}


// ---------------------------------------------------------------------------
//	е PositionToValue											   [protected]
// ---------------------------------------------------------------------------
//	Return the value of the Slider based on the specified pixel location
//	of the indicator (left or bottom edge)

SInt32
LGASliderImp::PositionToValue(
	SInt16	inPosition) const
{
	SInt16	pMin, pMax;					//   v - vMin       p - pMin
	FindIndicatorLimits(pMin, pMax);	// -----------  =  ----------
										// vMax - vMin     pMax - Min

	SInt32	vMin = mControlPane->GetMinValue();
	SInt32	vMax = mControlPane->GetMaxValue();

	SInt32	value = vMin;				// Special case min and max to
	if (inPosition == pMax) {			//   avoid rounding errors at the
		value = vMax;					//   boundaries

	} else if (inPosition != pMin) {
		double ratio = (double) (vMax - vMin) / (pMax - pMin);
		value = (SInt32) (vMin + (inPosition - pMin) * ratio + 0.5);
	}

	return value;
}


// ---------------------------------------------------------------------------
//	е ValueToPosition											   [protected]
// ---------------------------------------------------------------------------
//	Return the pixel position of the indicator (left or bottom edge) when
//	the Slider has the specified value

SInt16
LGASliderImp::ValueToPosition(
	SInt32	inValue) const
{
	SInt16	pMin, pMax;					//   v - vMin       p - pMin
	FindIndicatorLimits(pMin, pMax);	// -----------  =  ----------
										// vMax - vMin     pMax - Min

	SInt32	vMin = mControlPane->GetMinValue();
	SInt32	vMax = mControlPane->GetMaxValue();

	SInt16	position = pMin;			// Special case min and max to
	if (inValue >= vMax) {				//   avoid rounding errors at the
		position = pMax;				//   boundaries

	} else if (inValue > vMin) {
		double ratio = (double) (pMax - pMin) / (vMax - vMin);
		position = (SInt16) (pMin + (inValue - vMin) * ratio + 0.5);
	}

	return position;
}

PP_End_Namespace_PowerPlant
