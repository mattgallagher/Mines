// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UControlRegistry.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UControlRegistry.h>
									// Control Panes and Views
#include <LBevelButton.h>
#include <LCmdBevelButton.h>
#include <LChasingArrows.h>
#include <LCheckBox.h>
#include <LCheckBoxGroupBox.h>
#include <LClock.h>
#include <LDisclosureTriangle.h>
#include <LEditText.h>
#include <LIconControl.h>
#include <LImageWell.h>
#include <LLittleArrows.h>
#include <LPictureControl.h>
#include <LPlacard.h>
#include <LPopupButton.h>
#include <LPopupGroupBox.h>
#include <LProgressBar.h>
#include <LPushButton.h>
#include <LRadioButton.h>
#include <LRadioGroupView.h>
#include <LScrollBar.h>
#include <LSeparatorLine.h>
#include <LSlider.h>
#include <LStaticText.h>
#include <LTabsControl.h>
#include <LTextGroupBox.h>
#include <LWindowHeader.h>
									// Standard Implementations
#include <LStdPopupButtonImp.h>
#include <LStdPushButtonImp.h>
#include <LStdScrollBarImp.h>
									// Appearance Manager Implementation
#include <LAMBevelButtonImp.h>
#include <LAMCheckBoxGroupBoxImp.h>
#include <LAMClockImp.h>
#include <LAMControlImp.h>
#include <LAMEditTextImp.h>
#include <LAMImageWellImp.h>
#include <LAMPopupButtonImp.h>
#include <LAMPopupGroupBoxImp.h>
#include <LAMPushButtonImp.h>
#include <LAMStaticTextImp.h>
#include <LAMTextGroupBoxImp.h>
#include <LAMTrackActionImp.h>
#include <LAMWindowHeaderImp.h>
									// Grayscale Appearance Implementations
#include <LGABevelButtonImp.h>
#include <LGALittleArrowsImp.h>
#include <LGAPushButtonImp.h>
#include <LGARadioButtonImp.h>
#include <LGACheckBoxImp.h>
#include <LGADisclosureTriangleImp.h>
#include <LGAPopupButtonImp.h>
#include <LGASliderImp.h>
#include <LGAIconControlImp.h>
#include <LGASeparatorImp.h>
#include <LGAPlacardImp.h>
#include <LGAWindowHeaderImp.h>
#include <LGAImageWellImp.h>
#include <LGAStaticTextImp.h>
#include <LGAGroupBoxImp.h>
#include <LGATextGroupBoxImp.h>
#include <LGACheckBoxGroupBoxImp.h>
#include <LGAPopupGroupBoxImp.h>
#include <LGAProgressBarImp.h>
#include <LGAChasingArrowsImp.h>
#include <LGAPictureControlImp.h>
#include <LGATabsControlImp.h>
#include <LGAEditTextImp.h>
									// Other PP Classes
#include <UEnvironment.h>
#include <URegistrar.h>
#include <LStdControl.h>
#include <LDialogBox.h>
#include <LMultiPanelView.h>
#include <LScrollerView.h>
									// Other GA Classes
#include <LGAColorSwatchControl.h>
#include <LGAFocusBorder.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ RegisterClasses										 [static] [public]
// ---------------------------------------------------------------------------
//	Registers all of the Appearance Manager classes as well as the proper
//	implementation classes depending upon whether or not the App Mgr
//	is present.
//
//	This is the routine you will normally call. On systems with the App Mgr,
//	you get the native controls. Without the App Mgr, you get the grayscale
//	controls.

void
UControlRegistry::RegisterClasses()
{
	#if TARGET_API_MAC_CARBON			// Carbon always has Appearance Mgr
	
		RegisterAMClasses();
		
	#else

		if (UEnvironment::HasFeature (env_HasAppearance)) {
			RegisterAMClasses();

			if (not UEnvironment::HasFeature (env_HasAppearance101)) {

					// AM Popup menus require Appearance version 1.0.1
					// or later. This affects LPopupButton, LPopupGroupBox,
					// and LBevelButton. So if we are running an earlier
					// version (AM 1.0 shipped with Mac OS 8.0), we have
					// to use the GA imps for those classes.

				RegisterClassID_( LGAPopupButtonImp,	LPopupButton::imp_class_ID );
				RegisterClassID_( LGAPopupGroupBoxImp,	LPopupGroupBox::imp_class_ID );
				RegisterClassID_( LGABevelButtonImp,	LBevelButton::imp_class_ID );
			}

		} else {
			RegisterGAClasses();
		}
	
	#endif // TARGET_API_MAC_CARBON
}


// ---------------------------------------------------------------------------
//	¥ RegisterAMClasses										 [static] [public]
// ---------------------------------------------------------------------------
//	Registers Appearance Manager classes and AM implementations. Be sure to
//	check that the App Mgr is present before calling this function, and be
//	aware that LPopupButton and LPopupGroupBox require AM 1.0.1 or later.
//
//	Call this routine if you wish to use the AM imps all the time.

void
UControlRegistry::RegisterAMClasses()
{
	::RegisterAppearanceClient();		// Tell OS we are Appearance-aware

	RegisterAMAbstractionClasses();
	RegisterAMImpClasses();
	RegisterOther();
}


// ---------------------------------------------------------------------------
//	¥ RegisterGAClasses										 [static] [public]
// ---------------------------------------------------------------------------
//	Registers Appearance Manager classes and GA implementations. The GA
//	imps do not require that the App Mgr be present.
//
//	Call this routine if you want to use the GA imps all the time.

void
UControlRegistry::RegisterGAClasses()
{
	RegisterAMAbstractionClasses();
	RegisterGAImpClasses();
	RegisterOther();
}


// ---------------------------------------------------------------------------
//	¥ RegisterClassicControls								 [static] [public]
// ---------------------------------------------------------------------------
//	Registers the 5 controls provided in System 7: Check Box, Radio Button,
//	Push Button, Scroll Bar, and Popup Menu
//
//	If inAlwaysGA is true, registers GA Imps. If false, registers AM Imps
//	if App Mgr is present and Std Imps if App Mgr is not present.
//
//	Call this routine if you only use the 5 classic controls to avoid
//	registering all the other controls (that you don't use).

void
UControlRegistry::RegisterClassicControls(
	bool	inAlwaysGA)
{
	RegisterClass_(LCheckBox);
	RegisterClass_(LPopupButton);
	RegisterClass_(LPushButton);
	RegisterClass_(LRadioButton);
	RegisterClass_(LScrollBar);

	if (inAlwaysGA) {						// GA Imps
		RegisterClassID_( LGACheckBoxImp,		LCheckBox::imp_class_ID);
		RegisterClassID_( LGAPopupButtonImp,	LPopupButton::imp_class_ID);
		RegisterClassID_( LGAPushButtonImp,		LPushButton::imp_class_ID);
		RegisterClassID_( LGARadioButtonImp,	LRadioButton::imp_class_ID);

			// +++ No GA ScrollBar Imp. Use native Imp.

//		RegisterClassID_( LGAScrollBarImp,		LScrollBar::imp_classID);

		if (UEnvironment::HasFeature (env_HasAppearance)) {
			RegisterClassID_(LAMTrackActionImp, LScrollBar::imp_class_ID);
		} else {
			RegisterClassID_(LStdScrollBarImp, LScrollBar::imp_class_ID);
		}

	} else if (UEnvironment::HasFeature (env_HasAppearance)) {
											// AM Imps
		::RegisterAppearanceClient();
		RegisterClassID_( LAMControlImp,		LCheckBox::imp_class_ID);
		RegisterClassID_( LAMPopupButtonImp,	LPopupButton::imp_class_ID);
		RegisterClassID_( LAMPushButtonImp,		LPushButton::imp_class_ID);
		RegisterClassID_( LAMControlImp,		LRadioButton::imp_class_ID);
		RegisterClassID_( LAMTrackActionImp,	LScrollBar::imp_class_ID);

	} else {								// Std Imps
		RegisterClassID_( LStdControlImp,		LCheckBox::imp_class_ID);
		RegisterClassID_( LStdPopupButtonImp,	LPopupButton::imp_class_ID);
		RegisterClassID_( LStdPushButtonImp,	LPushButton::imp_class_ID);
		RegisterClassID_( LStdControlImp,		LRadioButton::imp_class_ID);
		RegisterClassID_( LStdScrollBarImp,		LScrollBar::imp_class_ID);
	}

											// Companion classes
	RegisterClass_(LRadioGroupView);
	RegisterClass_(LScrollerView);
}

#pragma mark -

// ===========================================================================
//	"PRIVATE" ROUTINES
// ===========================================================================
//	Since the above 4 register routines reuse a lot of the same code, it cuts
//	down on code size (as well as maintenance) to have the following 4 routines.
//	You will typically only use the above 4 routines.

// ---------------------------------------------------------------------------
//	¥ RegisterAMAbstractionClasses							[static] [private]
// ---------------------------------------------------------------------------
//	Registers all of the Appearance Manager abstraction classes.

void
UControlRegistry::RegisterAMAbstractionClasses()
{
	RegisterClass_(LBevelButton);
	RegisterClass_(LChasingArrows);
	RegisterClass_(LCheckBox);
	RegisterClass_(LCheckBoxGroupBox);
	RegisterClass_(LClock);
	RegisterClass_(LCmdBevelButton);
	RegisterClass_(LDisclosureTriangle);
	RegisterClass_(LEditText);
	RegisterClass_(LIconControl);
	RegisterClass_(LImageWell);
	RegisterClass_(LLittleArrows);
	RegisterClass_(LPictureControl);
	RegisterClass_(LPlacard);
	RegisterClass_(LPopupButton);
	RegisterClass_(LPopupGroupBox);
	RegisterClass_(LProgressBar);
	RegisterClass_(LPushButton);
	RegisterClass_(LRadioButton);
	RegisterClass_(LScrollBar);
	RegisterClass_(LSeparatorLine);
	RegisterClass_(LSlider);
	RegisterClass_(LStaticText);
	RegisterClass_(LTabsControl);
	RegisterClass_(LTextGroupBox);
	RegisterClass_(LWindowHeader);
}


// ---------------------------------------------------------------------------
//	¥ RegisterAMImpClasses									[static] [private]
// ---------------------------------------------------------------------------
//	Registers all of the Appearance Manager implementation classes.

void
UControlRegistry::RegisterAMImpClasses()
{
	RegisterClassID_(LAMControlImp, LChasingArrows::imp_class_ID);
	RegisterClassID_(LAMControlImp, LCheckBox::imp_class_ID);
	RegisterClassID_(LAMControlImp, LDisclosureTriangle::imp_class_ID);
	RegisterClassID_(LAMControlImp, LIconControl::imp_class_ID);
	RegisterClassID_(LAMControlImp, LPictureControl::imp_class_ID);
	RegisterClassID_(LAMControlImp,	LPlacard::imp_class_ID);
	RegisterClassID_(LAMControlImp, LRadioButton::imp_class_ID);
	RegisterClassID_(LAMControlImp, LSeparatorLine::imp_class_ID);
	RegisterClassID_(LAMControlImp,	LTabsControl::imp_class_ID);

	RegisterClassID_(LAMTrackActionImp, LLittleArrows::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp, LProgressBar::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp, LScrollBar::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp, LSlider::imp_class_ID);

	RegisterClassID_(LAMBevelButtonImp,		 LBevelButton::imp_class_ID);
	RegisterClassID_(LAMCheckBoxGroupBoxImp, LCheckBoxGroupBox::imp_class_ID);
	RegisterClassID_(LAMClockImp,			 LClock::imp_class_ID);
	RegisterClassID_(LAMEditTextImp,		 LEditText::imp_class_ID);
	RegisterClassID_(LAMImageWellImp,		 LImageWell::imp_class_ID);
	RegisterClassID_(LAMPopupButtonImp,		 LPopupButton::imp_class_ID);
	RegisterClassID_(LAMPopupGroupBoxImp,	 LPopupGroupBox::imp_class_ID);
	RegisterClassID_(LAMPushButtonImp,		 LPushButton::imp_class_ID);
	RegisterClassID_(LAMStaticTextImp,		 LStaticText::imp_class_ID);
	RegisterClassID_(LAMTextGroupBoxImp,	 LTextGroupBox::imp_class_ID);
	RegisterClassID_(LAMWindowHeaderImp,	 LWindowHeader::imp_class_ID);
}


// ---------------------------------------------------------------------------
//	¥ RegisterGAImpClasses									[static] [private]
// ---------------------------------------------------------------------------
//	Registers all of the GA implementation classes.

void
UControlRegistry::RegisterGAImpClasses()
{
	RegisterClassID_( LGABevelButtonImp,		LBevelButton::imp_class_ID );
	RegisterClassID_( LGAChasingArrowsImp,		LChasingArrows::imp_class_ID );
	RegisterClassID_( LGACheckBoxImp,			LCheckBox::imp_class_ID );
	RegisterClassID_( LGACheckBoxGroupBoxImp,	LCheckBoxGroupBox::imp_class_ID );
	RegisterClassID_( LGADisclosureTriangleImp,	LDisclosureTriangle::imp_class_ID );
	RegisterClassID_( LGAEditTextImp,			LEditText::imp_class_ID );
	RegisterClassID_( LGAIconControlImp,		LIconControl::imp_class_ID );
	RegisterClassID_( LGAImageWellImp,			LImageWell::imp_class_ID );
	RegisterClassID_( LGALittleArrowsImp,		LLittleArrows::imp_class_ID );
	RegisterClassID_( LGAPictureControlImp,		LPictureControl::imp_class_ID );
	RegisterClassID_( LGAPlacardImp,			LPlacard::imp_class_ID );
	RegisterClassID_( LGAPopupButtonImp,		LPopupButton::imp_class_ID );
	RegisterClassID_( LGAPopupGroupBoxImp,		LPopupGroupBox::imp_class_ID );
	RegisterClassID_( LGAProgressBarImp,		LProgressBar::imp_class_ID );
	RegisterClassID_( LGAPushButtonImp,			LPushButton::imp_class_ID );
	RegisterClassID_( LGARadioButtonImp,		LRadioButton::imp_class_ID );
	RegisterClassID_( LGASeparatorImp,			LSeparatorLine::imp_class_ID );
	RegisterClassID_( LGASliderImp,				LSlider::imp_class_ID );
	RegisterClassID_( LGAStaticTextImp,			LStaticText::imp_class_ID );
	RegisterClassID_( LGATabsControlImp,		LTabsControl::imp_class_ID );
	RegisterClassID_( LGATextGroupBoxImp,		LTextGroupBox::imp_class_ID );
	RegisterClassID_( LGAWindowHeaderImp,		LWindowHeader::imp_class_ID );

			// +++ No GA Imp for ScrollBar and Clock. Use native Imp.

//	RegisterClassID_( LGAScrollBarImp, LScrollBar::imp_classID );
//	RegisterClassID_( LGAClockImp, LClock::imp_class_ID );

	if (UEnvironment::HasFeature (env_HasAppearance)) {
		RegisterClassID_( LAMTrackActionImp,	LScrollBar::imp_class_ID);
		RegisterClassID_( LAMControlImp,		LClock::imp_class_ID);
	} else {
		RegisterClassID_( LStdScrollBarImp,		LScrollBar::imp_class_ID);
		// There is no std Clock.
	}
}


// ---------------------------------------------------------------------------
//	¥ RegisterOther											[static] [private]
// ---------------------------------------------------------------------------
//	Registers all of the sundry other classes.

void
UControlRegistry::RegisterOther()
{
	RegisterClass_(LRadioGroupView);
	RegisterClass_(LMultiPanelView);
	RegisterClass_(LScrollerView);
	RegisterClass_(LWindowThemeAttachment);
	RegisterClass_(LDialogBox);

	RegisterClass_(LGAColorSwatchControl);
	RegisterClass_(LGAFocusBorder);
}


PP_End_Namespace_PowerPlant
