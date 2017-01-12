// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UPrintingUtils.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UPrintingUtils.h>
#include <UMemoryMgr.h>

#include <Quickdraw.h>

PP_Begin_Namespace_PowerPlant


const SInt16 SetLineWidth = 182;			// Set line width picture comment


// ---------------------------------------------------------------------------
//	¥ StFractionalPenWidth					Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor sets the fractional line width, destructor restores previous
//	fraction.

StFractionalPenWidth::StFractionalPenWidth(
	SInt16	inNumerator,
	SInt16	inDenominator)

	: mNumerator(inNumerator),
  	  mDenominator(inDenominator)

{
	StHandleBlock lineWidth(sizeof (Point));
	Point* lineWidthP = (Point*) *((Handle) lineWidth);

	lineWidthP->v = mNumerator;
	lineWidthP->h = mDenominator;

	::PicComment(SetLineWidth, sizeof (Point), lineWidth);
	::PenNormal();
}


// ---------------------------------------------------------------------------
//	¥ ~StFractionalPenWidth					Destructor				  [public]
// ---------------------------------------------------------------------------

StFractionalPenWidth::~StFractionalPenWidth()
{
	StHandleBlock lineWidth(sizeof (Point));
	Point* lineWidthP = (Point*) *((Handle) lineWidth);

	lineWidthP->h = mNumerator;			// We have to invert the fraction
	lineWidthP->v = mDenominator;		// for PS driver (it's wierd, I know)
	::PicComment(SetLineWidth, sizeof (Point), lineWidth);

	lineWidthP = (Point*) *((Handle) lineWidth);	// Handle could have moved

	lineWidthP->h = 1;					// Then send 1/1 for LW SC driver
	lineWidthP->v = 1;
	::PicComment(SetLineWidth, sizeof (Point), lineWidth);

	::PenNormal();
}


PP_End_Namespace_PowerPlant
