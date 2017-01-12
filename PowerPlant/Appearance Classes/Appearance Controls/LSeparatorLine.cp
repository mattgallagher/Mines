// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSeparatorLine.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSeparatorLine.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSeparatorLine						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LSeparatorLine::LSeparatorLine(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LSeparatorLine						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LSeparatorLine::LSeparatorLine(
	const SPaneInfo&	inPaneInfo,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, kControlSeparatorLineProc)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LSeparatorLine						Destructor				  [public]
// ---------------------------------------------------------------------------

LSeparatorLine::~LSeparatorLine()
{
}


PP_End_Namespace_PowerPlant
