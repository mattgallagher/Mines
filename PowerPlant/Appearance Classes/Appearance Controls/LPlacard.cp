// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPlacard.cp					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPlacard.h>
#include <UAppearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPlacard								Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPlacard::LPlacard(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
}


// ---------------------------------------------------------------------------
//	¥ LPlacard								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPlacard::LPlacard(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID, kControlPlacardProc)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LPlacard								Destructor				  [public]
// ---------------------------------------------------------------------------

LPlacard::~LPlacard()
{
}


// ---------------------------------------------------------------------------
//	¥ GetEmbeddedTextColor
// ---------------------------------------------------------------------------
//	Pass back the color for drawing text within the Placard

void
LPlacard::GetEmbeddedTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive,
	RGBColor&	outColor) const
{
	ThemeTextColor	color = kThemeTextColorPlacardInactive;
	if (inIsActive) {
		color = kThemeTextColorPlacardActive;
	}

	UAppearance::GetThemeTextColor(color, inDepth, inHasColor,
									inIsActive, &outColor);
}


PP_End_Namespace_PowerPlant
