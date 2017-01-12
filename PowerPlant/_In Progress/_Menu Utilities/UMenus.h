// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMenus.h					PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_UMenus
#define _H_UMenus
#pragma once

	// You should use the alias PP_MenuUtilities to qualify
	// calls to the menu utility functions. This alias will map
	// to an implementation namespace based upon the setting of
	// the PP_MenuUtils_Option preprocess symbol.
	//
	// The file UMenus.i contains the function declarations


#if PP_MenuUtils_Option == PP_MenuUtils_AppearanceOnly

	// Always use the Menu Manager 2.0 (Appearance 1.1) Menu
	// Manager routines.

	#include <UAppearanceMenus.h>
	namespace PP_MenuUtilities = PP_PowerPlant::UAppearanceMenus;

#elif PP_MenuUtils_Option == PP_MenuUtils_Appearance10Only

	// Alwaus use the Menu Manager 1.0 (Appearance 1.0.x)
	// Menu Manager routines

	#include <UAppearance10Menus.h>
	namespace PP_MenuUtilities = PP_PowerPlant::UAppearance10Menus;

#elif PP_MenuUtils_Option == PP_MenuUtils_Conditional

	// Use the Appearance Menu Manager routines if available,
	// else use Classic

	#include <UConditionalMenus.h>
	namespace PP_MenuUtilities = PP_PowerPlant::UConditionalMenus;

#else
	// use the Classic implementation

	#include <UClassicMenus.h>
	namespace PP_MenuUtilities = PP_PowerPlant::UClassicMenus;

#endif // PP_MenuUtils_Option


#endif	// _H_UMenus
