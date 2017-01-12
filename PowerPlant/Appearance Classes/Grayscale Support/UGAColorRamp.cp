// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UGAColorRamp.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair
//
//	Maintains a table of colors corresponding to those used in the
//	Apple Grayscale Appearance (AGA) specification.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UGAColorRamp.h>
#include <UEnvironment.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant

RGBColor	UGAColorRamp::sColorTable [20] = {		// Color Table
	{ 65535, 65535, 65535 },	// White		 0
	{ 61166, 61166, 61166 },	// Gray 1		 1
	{ 56797, 56797, 56797 },	// Gray 2		 2
	{ 52428, 52428, 52428 },	// Gray 3		 3
	{ 48059, 48059, 48059 },	// Gray 4		 4
	{ 43690, 43690, 43690 },	// Gray 5		 5
	{ 39321, 39321, 39321 },	// Gray 6		 6
	{ 34952, 34952, 34952 },	// Gray 7		 7
	{ 30583, 30583, 30583 },	// Gray 8		 8
	{ 26214, 26214, 26214 },	// Gray 9		 9
	{ 21845, 21845, 21845 },	// Gray 10		10
	{ 17476, 17476, 17476 },	// Gray 11		11
	{  8738,  8738,  8738 },	// Gray 12		12
	{     0,     0,     0 },	// Black		13
	{ 13107, 13107, 13107 },	// A1			14
	{  4369,  4369,  4369 },	// A2			15
	{ 52428, 52428, 65535 },	// P1			16
	{ 39321, 39321, 65535 },	// P2			17
	{ 26214, 26214, 52428 },	// P3			18
	{ 13107, 13107, 39321 }		// P4			19
};


// ---------------------------------------------------------------------------
//	¥ GetColor												 [static] [public]
// ---------------------------------------------------------------------------

const RGBColor&
UGAColorRamp::GetColor(
	SInt8	inColorIndex)
{
	return sColorTable[inColorIndex];
}


void
UGAColorRamp::GetColor(
	SInt8		inColorIndex,
	RGBColor&	outColor)
{
	outColor = sColorTable[inColorIndex];
}


// ---------------------------------------------------------------------------
//	¥ GetFocusAccentColor									 [static] [public]
// ---------------------------------------------------------------------------
//	Pass back the color used to draw focus boxes

void
UGAColorRamp::GetFocusAccentColor(
	RGBColor&	outColor)
{
										// Gray 8 is the default focus accent
										//   color
	outColor = UGAColorRamp::GetColor(colorRamp_Gray8);

	if ( UEnvironment::IsAppearanceRunning()  &&
		 UEnvironment::HasFeature(env_HasAppearance101) ) {

										// With Appearance 1.0.1 or later,
										//   we can ask the System for the
										//   accent colors
		CTabHandle accentColors;
		OSStatus status = ::GetThemeAccentColors(&accentColors);

		if ( (status == noErr)  &&
			 (accentColors != nil)  &&
			 ((**accentColors).ctSize >= 3) ) {
										// From inspection, we determined that
										//   Apple uses the fourth color as
										//   the focus accent color
			outColor = (**accentColors).ctTable[3].rgb;
			::DisposeHandle((Handle) accentColors);
		}
	}
}

PP_End_Namespace_PowerPlant
