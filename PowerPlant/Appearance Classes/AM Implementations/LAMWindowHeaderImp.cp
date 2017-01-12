// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMWindowHeaderImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMWindowHeaderImp.h>
#include <LView.h>
#include <UAppearance.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMWindowHeaderImp				Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMWindowHeaderImp::LAMWindowHeaderImp(
	LStream*		inStream)

	: LAMControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LAMWindowHeaderImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMWindowHeaderImp::~LAMWindowHeaderImp()
{
}


// ---------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors										  [public]
// ---------------------------------------------------------------------------

bool
LAMWindowHeaderImp::ApplyForeAndBackColors() const
{
		// There is a bug in Appearance Manager 1.1 for the Platinum
		// Theme. ApplyThemeBackground() for a WindowHeader always sets
		// the background to a medium gray. However, an inactive WindowHeader
		// actually draws in a light gray.
		//
		// An inactive Placard draws the same as a WindowHeader under
		// Platinum Theme. Therefore, under AM 1.1, we use the background for
		// a Placard instead of a WindowHeader when inactive.
		//
		// This workaround will draw incorrectly for Themes where Placard
		// and WindowHeader draw differently when inactive. But checking
		// for Platinum Theme is a potentially time-consuming process
		// requiring File Manager calls.
		//
		// We feel that this potential glitch with non-Platinum Themes
		// is tolerable. It only affects Mac OS 8.5/8.6 (Mac OS 9.0.4 has
		// AM 1.1.1 which fixes the problem), and Apple ships only
		// Platinum Theme on that OS.
		
	if (UEnvironment::GetAppearanceVersion() == 0x0110) {
	
		ThemeBackgroundKind		backgroundKind = kThemeBackgroundPlacard;

		if (IsActive() and IsEnabled()) {
			 
			 	// Not drawing inactively. We use the background for
				// a WindowHeader or ListViewWindowHeader, depending
				// on the variation code of the control.

			ControlVariant	variant = ::GetControlVariant(mMacControlH);

			backgroundKind = kThemeBackgroundWindowHeader;
			if (variant == 1) {
				backgroundKind = kThemeBackgroundListViewWindowHeader;
			}
		}

		Rect	frame;
		mControlPane->CalcLocalFrameRect(frame);

		SInt16	bitDepth;
		bool	hasColor;
		mControlPane->GetDeviceInfo(bitDepth, hasColor);

		UAppearance::ApplyThemeBackground(
								backgroundKind,
								&frame,
								(IsActive() and IsEnabled()) ? kThemeStateActive
										   					 : kThemeStateInactive,
								bitDepth,
								hasColor);
								
		RGBColor	foreColor;			// Get forecolor from container
		mControlPane->GetContainer()->GetForeAndBackColors(&foreColor, nil);
		::RGBForeColor(&foreColor);

		return true;
	
	} else {							// Not AM 1.1. The inherited function
										//   works fine.
		return LAMControlImp::ApplyForeAndBackColors();
	}
}


PP_End_Namespace_PowerPlant
