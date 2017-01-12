// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_KeyCodes.h				PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_PP_KeyCodes
#define _H_PP_KeyCodes
#pragma once

#include <PP_Types.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ее Character Codes ее

	// е Navigation Keys е

const UInt8		char_LeftArrow		 = 0x1C;
const UInt8		char_RightArrow		 = 0x1D;
const UInt8		char_UpArrow		 = 0x1E;
const UInt8		char_DownArrow		 = 0x1F;

const UInt8		char_Home			 = 0x01;
const UInt8		char_End			 = 0x04;
const UInt8		char_PageUp			 = 0x0B;
const UInt8		char_PageDown		 = 0x0C;

	// е Deletion Keys е

const UInt8		char_Backspace		 = 0x08;
const UInt8		char_FwdDelete		 = 0x7F;	// Only nonprinting above $20
const UInt8		char_Clear			 = 0x1B;	// Same as Escape
const UInt32	vkey_Clear			 = 0x00004700;

	// е Action Keys е

const UInt8		char_Enter			 = 0x03;
const UInt8		char_Tab			 = 0x09;
const UInt8		char_Return			 = 0x0D;
const UInt8		char_Escape			 = 0x1B;	// Same as Clear
const UInt32	vkey_Escape			 = 0x00003500;

	// е Special Keys е

const UInt8		char_Help			 = 0x05;
const UInt8		char_Function		 = 0x10;	// All function keys F1 to F15
const UInt8		vkey_F1				 = 0x7A;	// Undo
const UInt8		vkey_F2				 = 0x78;	// Cut
const UInt8		vkey_F3				 = 0x63;	// Copy
const UInt8		vkey_F4				 = 0x76;	// Paste
const UInt8		vkey_F5				 = 0x60;
const UInt8		vkey_F6				 = 0x61;
const UInt8		vkey_F7				 = 0x62;
const UInt8		vkey_F8				 = 0x64;
const UInt8		vkey_F9				 = 0x65;
const UInt8		vkey_F10			 = 0x6D;
const UInt8		vkey_F11			 = 0x67;
const UInt8		vkey_F12			 = 0x6F;
const UInt8		vkey_F13			 = 0x69;	// Print Screen
const UInt8		vkey_F14			 = 0x6B;	// Scroll Lock
const UInt8		vkey_F15			 = 0x71;	// Pause

	// е Special Characters е

const UInt8		char_Propeller		 = 0x11;	// Symbol for Command key
const UInt8		char_Radical		 = 0x12;
const UInt8		char_Lozenge		 = 0x13;
const UInt8		char_AppleLogo		 = 0x14;
const UInt8		char_Bullet			 = 0xA5;
const UInt8		char_Apostrophe		 = 0x27;
const UInt8		char_Dash			 = 0x2D;
const UInt8		char_MinusSign		 = 0x2D;
const UInt8		char_Colon			 = 0x3A;
const UInt8		char_Caret			 = 0x5E;

const UInt8		char_FirstPrinting	 = 0x20;	// Nonprinting if less than
const UInt8		char_Space			 = 0x20;
const UInt8		char_Period			 = 0x2E;	// For detecting Command-Period
const UInt8		char_LineFeed		 = 0x0A;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
