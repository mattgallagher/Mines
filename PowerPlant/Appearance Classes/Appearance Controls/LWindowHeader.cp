// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindowHeader.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LWindowHeader.h>
#include <UAppearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LWindowHeader						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LWindowHeader::LWindowHeader(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LWindowHeader						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LWindowHeader::LWindowHeader(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	bool				inListView,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
					inListView ? kControlWindowListViewHeaderProc
							   : kControlWindowHeaderProc)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LWindowHeader						Destructor				  [public]
// ---------------------------------------------------------------------------

LWindowHeader::~LWindowHeader()
{
}


// ---------------------------------------------------------------------------
//	¥ GetEmbeddedTextColor
// ---------------------------------------------------------------------------
//	Pass back the color for drawing text within the WindowHeader

void
LWindowHeader::GetEmbeddedTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive,
	RGBColor&	outColor) const
{
	ThemeTextColor	color = kThemeTextColorWindowHeaderInactive;
	if (inIsActive) {
		color = kThemeTextColorWindowHeaderActive;
	}

	UAppearance::GetThemeTextColor(color, inDepth, inHasColor,
									inIsActive, &outColor);
}


PP_End_Namespace_PowerPlant
