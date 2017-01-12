// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabsControl.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTabsControl.h>
#include <LControlImp.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UAppearance.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTabsControl							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTabsControl::LTabsControl(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream, inImpID)
{
	SInt16	initialTab;
	*inStream >> initialTab;

	InitTabsControl(initialTab);
}


// ---------------------------------------------------------------------------
//	¥ LTabsControl							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LTabsControl::LTabsControl(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	MessageT			inMessage,
	SInt16				inControlKind,
	ResIDT				inTabxID,
	SInt16				inInitialValue,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
						inControlKind, Str_Empty, 0, inMessage, inTabxID)
{
	InitTabsControl(inInitialValue);
}


// ---------------------------------------------------------------------------
//	¥ ~LTabsControl							Destructor				  [public]
// ---------------------------------------------------------------------------

LTabsControl::~LTabsControl()
{
}


// ---------------------------------------------------------------------------
//	¥ InitTabsControl												 [private]
// ---------------------------------------------------------------------------

void
LTabsControl::InitTabsControl(
	SInt16	inInitialValue)
{
	SInt32	value, min, max;

	(mControlSubPane->GetControlImp())->GetAllValues(value, min, max);

	SetMinValue(min);
	SetMaxValue(max);

	mControlSubPane->PokeValue(min - 1);
	SetValue(inInitialValue);
}


// ---------------------------------------------------------------------------
//	¥ EnableTab														  [public]
// ---------------------------------------------------------------------------

void
LTabsControl::EnableTab(
	SInt16		inTabIndex,
	Boolean		inEnabled)
{
	SetDataTag(inTabIndex, kControlTabEnabledFlagTag,
				sizeof(Boolean), &inEnabled);
}


// ---------------------------------------------------------------------------
//	¥ IsTabEnabled													  [public]
// ---------------------------------------------------------------------------

Boolean
LTabsControl::IsTabEnabled(
	SInt16		inTabIndex) const
{
	Boolean	enabled;
	GetDataTag(inTabIndex, kControlTabEnabledFlagTag,
				sizeof(Boolean), &enabled);
	return enabled;
}


// ---------------------------------------------------------------------------
//	¥ GetEmbeddedTextColor
// ---------------------------------------------------------------------------
//	Pass back the color for drawing text within the TabsControl

void
LTabsControl::GetEmbeddedTextColor(
	SInt16		inDepth,
	bool		inHasColor,
	bool		inIsActive,
	RGBColor&	outColor) const
{
	ThemeTextColor	color = kThemeTextColorTabFrontInactive;
	if (inIsActive) {
		color = kThemeTextColorTabFrontActive;
	}

	UAppearance::GetThemeTextColor(color, inDepth, inHasColor,
									inIsActive, &outColor);
}


PP_End_Namespace_PowerPlant
