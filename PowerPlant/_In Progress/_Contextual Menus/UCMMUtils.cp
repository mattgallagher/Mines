// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCMMUtils.cp				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utilities for Contextual Menu Manager support

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UCMMUtils.h>
#include <PP_KeyCodes.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant


bool	UCMMUtils::gCMMPresent		=	false;
bool	UCMMUtils::gCMMInitialized	=	false;


// ---------------------------------------------------------------------------
//	¥ÊInitialize
// ---------------------------------------------------------------------------
//	Determines if the Contextual Menu Manager is present, and if so,
//	initializes the CMM.
//
//	This routine needs to be called at least once by your application,
//	typically during startup (e.g. after toolbox initialization)

void
UCMMUtils::Initialize()
{
	SetCMMPresent(	UEnvironment::HasGestaltAttribute(gestaltContextualMenuAttr,
										  gestaltContextualMenuTrapAvailable)
					&& CFM_AddressIsResolved_(::InitContextualMenus));

	SetCMMInitialized(false);

	if (IsCMMPresent()) {
		OSStatus err = ::InitContextualMenus();
		Assert_(err == noErr);
		if (err == noErr) {
			SetCMMInitialized(true);
		}
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ StripFirstDivider
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	If in the given MenuHandle the first item is a divider, remove it.

bool
UCMMUtils::StripFirstDivider(
	MenuHandle	inMenuH)
{
	Assert_(inMenuH != nil);

	bool stripped = false;

	if (::CountMenuItems(inMenuH) > 0) {
		Str255 itemText;
		::GetMenuItemText(inMenuH, 1, itemText);
		if ((itemText[0] > 0) && (itemText[1] == char_Dash)) {
			::DeleteMenuItem(inMenuH, 1);
			stripped = true;
		}
	}

	return stripped;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ StripLastDivider
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	If in the given MenuHandle the last item is a divider, remove it

bool
UCMMUtils::StripLastDivider(
	MenuHandle	inMenuH)
{
	Assert_(inMenuH != nil);

	bool stripped = false;

	SInt16 itemCount = (SInt16) ::CountMenuItems(inMenuH);

	if ( itemCount > 0 ) {
		Str255 itemText;
		::GetMenuItemText(inMenuH, itemCount, itemText);
		if ((itemText[0] > 0) && (itemText[1] == char_Dash)) {
			::DeleteMenuItem(inMenuH, itemCount);
			stripped = true;
		}
	}

	return stripped;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ StripDoubleDividers
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If in the given MenuHandle there are 2 divider lines next to each other,
// strip them. Thanx to David Catmull for the logic

bool
UCMMUtils::StripDoubleDividers(
	MenuHandle	inMenuH)
{
	Assert_(inMenuH != nil);

	bool stripped = false;

	Str255 itemString;
	Str255 prevItemString;

	for (SInt16 ii = (SInt16) ::CountMenuItems(inMenuH); ii > 1; ii--) {
		::GetMenuItemText(inMenuH, ii, itemString);
		::GetMenuItemText(inMenuH, static_cast<SInt16>(ii - 1), prevItemString);

		if (((itemString[0] > 0) && (itemString[1] == char_Dash))
			&& ((prevItemString[0] > 0) && (prevItemString[1] == char_Dash))) {

			::DeleteMenuItem(inMenuH, ii);
			stripped = true;
		}
	}

	return stripped;
}


PP_End_Namespace_PowerPlant
