// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDisclosureTriangle.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDisclosureTriangle.h>
#include <LControlImp.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LDisclosureTriangle					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LDisclosureTriangle::LDisclosureTriangle(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LDisclosureTriangle					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LDisclosureTriangle::LDisclosureTriangle(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	bool				inFaceLeft,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID,
		   inFaceLeft ? kControlTriangleLeftFacingProc : kControlTriangleProc,
			Str_Empty, 0, inValueMessage, inValue, 0, 1)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LDisclosureTriangle					Destructor				  [public]
// ---------------------------------------------------------------------------

LDisclosureTriangle::~LDisclosureTriangle()
{
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LDisclosureTriangle::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	SetValue(1 - GetValue());			// Toggle between 0 and 1
}


PP_End_Namespace_PowerPlant
