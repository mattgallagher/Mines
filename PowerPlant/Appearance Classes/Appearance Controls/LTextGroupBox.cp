// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextGroupBox.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTextGroupBox.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTextGroupBox							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTextGroupBox::LTextGroupBox(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LTextGroupBox							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTextGroupBox::LTextGroupBox(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	Boolean				inPrimary,
	ResIDT				inTextTraitsID,
	ConstStringPtr		inTitle,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
						inPrimary ? kControlGroupBoxTextTitleProc
								  : kControlGroupBoxSecondaryTextTitleProc,
						inTitle, inTextTraitsID)
{
}


// ---------------------------------------------------------------------------
//	¥ LTextGroupBox						Destructor				  [public]
// ---------------------------------------------------------------------------

LTextGroupBox::~LTextGroupBox()
{
}


PP_End_Namespace_PowerPlant
