// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMercutioMBAR.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	MenuBar class which supports the Mercutio MDEF (from Ramon M. Felciano).
//
//	Tested with version 1.5.2 of Mercutio MDEF.
//
//	Usage Note: If you have menus with key equivalents that use modifiers
//	besides the command key, you must call SetModifierKeys() to specify
//	the appropriate modifier keys.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LMercutioMBAR.h>
#include <LMenu.h>
#include <PP_Messages.h>

#include <ToolUtils.h>

#include "MercutioAPI.h"				// Header for Mercutio MDEF

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LMercutioMBAR							Constructor				  [public]
// ---------------------------------------------------------------------------

LMercutioMBAR::LMercutioMBAR(
	ResIDT	inMBARid)

	: LMenuBar(inMBARid)
{
	mMercutioMenu = nil;				// Search Menus for one that uses
										// Mercutio MDEF
	LMenu	*theMenu = nil;
	while (FindNextMenu(theMenu)) {
		if (MDEF_IsCustomMenu(theMenu->GetMacMenuH())) {
			mMercutioMenu = theMenu->GetMacMenuH();
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LMercutioMBAR						Destructor				  [public]
// ---------------------------------------------------------------------------

LMercutioMBAR::~LMercutioMBAR()
{
}


// ---------------------------------------------------------------------------
//	¥ FindKeyCommand												  [public]
// ---------------------------------------------------------------------------
//	Return the Command number corresponding to a keystroke
//		Returns cmd_Nothing if the keystroke is not a menu equivalent
//
//	Usage Note: Call this function when CouldBeKeyCommand() is true.

CommandT
LMercutioMBAR::FindKeyCommand(
	const EventRecord&	inKeyEvent,
	SInt32&				outMenuChoice) const
{
	CommandT	menuCmd = cmd_Nothing;
	outMenuChoice = 0;

	if ( (mModifierKeys & keyDownMask)  ||
		 (inKeyEvent.modifiers & mModifierKeys) ) {

		outMenuChoice = MDEF_MenuKey((SInt32) inKeyEvent.message,
								(SInt16) inKeyEvent.modifiers, mMercutioMenu);

		if (HiWord(outMenuChoice) != 0) {
			menuCmd = FindCommand(HiWord(outMenuChoice), LoWord(outMenuChoice));
		}
	}

	return menuCmd;
}


PP_End_Namespace_PowerPlant
