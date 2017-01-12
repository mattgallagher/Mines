// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Messages.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_PP_Messages
#define _H_PP_Messages
#pragma once

#include <PP_Types.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

	// Messages are 32-bit numbers used as parameters to a few PowerPlant
	// functions that you typically override:
	//		LCommander::ObeyCommand
	//		LListener::ListenToMessage
	//		LAttachment::ExecuteSelf

	// These function each take a Message and a void* parameter called
	// "ioParam". For each Message defined below, the adjacent comment
	// specifies the data passed via ioParam.

	// If a message is sent as a result of a menu selection (either with
	// the mouse or keyboard equivalent), the ioParam is an SInt32*, where
	// the hi SInt16 is the MENU ID, and the lo SInt16 is the menu item
	// number (value returned by MenuSelect/MenuKey).

	// Note: PowerPlant reserves command/message numbers 0 to 999 and all
	// negative numbers.


											// ioParam Data

const MessageT	cmd_Nothing			= 0;	// nil
const MessageT	msg_Nothing			= 0;	// nil

const MessageT	cmd_About			= 1;	// nil

					// File Menu
const MessageT	cmd_New				= 2;	// nil
const MessageT	cmd_Open			= 3;	// nil
const MessageT	cmd_Close			= 4;	// nil
const MessageT	cmd_Save			= 5;	// nil
const MessageT	cmd_SaveAs			= 6;	// nil
const MessageT	cmd_Revert			= 7;	// nil
const MessageT	cmd_PageSetup		= 8;	// nil
const MessageT	cmd_Print			= 9;	// nil
const MessageT	cmd_PrintOne		= 17;	// nil
const MessageT	cmd_Quit			= 10;	// nil

					// Edit Menu
const MessageT	cmd_Undo			= 11;	// nil
const MessageT	cmd_Cut				= 12;	// nil
const MessageT	cmd_Copy			= 13;	// nil
const MessageT	cmd_Paste			= 14;	// nil
const MessageT	cmd_Clear			= 15;	// nil
const MessageT	cmd_SelectAll		= 16;	// nil
const MessageT	cmd_Redo			= 28;	// nil

const MessageT	cmd_SaveCopyAs		= 18;	// nil
const MessageT	cmd_ShowClipboard	= 19;	// nil
const MessageT	cmd_Preferences		= 27;	// nil

const MessageT	cmd_Help			= 28;	// nil

					// Undo/Redo Editing Actions
const CommandT	cmd_ActionDeleted	= 20;	// LAction*
const CommandT	cmd_ActionDone		= 21;	// LAction*
const CommandT	cmd_ActionCut		= 22;	// LTECutAction*
const CommandT	cmd_ActionCopy		= 23;	// nil [not used]
const CommandT	cmd_ActionPaste		= 24;	// LTEPasteAction*
const CommandT	cmd_ActionClear		= 25;	// LTEClearAction*
const CommandT	cmd_ActionTyping	= 26;	// LTETypingAction*

const MessageT	msg_TabSelect		= 201;	// Boolean* (backward = true)
const MessageT	msg_BroadcasterDied	= 202;	// LBroadcaster*
const MessageT	msg_ControlClicked	= 203;	// LControl*
const MessageT	msg_ThumbDragged	= 204;	// LControl*
const MessageT	msg_ScrollAction	= 205;	// LScrollBar::SScrollMessage*

					// Use these three command numbers to disable the menu
					// item when you use the font-related menus as
					// hierarchical menus.
const MessageT	cmd_FontMenu		= 250;	// nil
const MessageT	cmd_SizeMenu		= 251;	// nil
const MessageT	cmd_StyleMenu		= 252;	// nil

					// Size menu commands
const MessageT	cmd_FontLarger		= 301;	// nil
const MessageT	cmd_FontSmaller		= 302;	// nil
const MessageT	cmd_FontOther		= 303;	// nil

					// Style menu commands
const MessageT	cmd_Plain			= 401;	// nil
const MessageT	cmd_Bold			= 402;	// nil
const MessageT	cmd_Italic			= 403;	// nil
const MessageT	cmd_Underline		= 404;	// nil
const MessageT	cmd_Outline			= 405;	// nil
const MessageT	cmd_Shadow			= 406;	// nil
const MessageT	cmd_Condense		= 407;	// nil
const MessageT	cmd_Extend			= 408;	// nil

					// Text justification
const MessageT	cmd_JustifyDefault	= 411;	// nil
const MessageT	cmd_JustifyLeft		= 412;	// nil
const MessageT	cmd_JustifyCenter	= 413;	// nil
const MessageT	cmd_JustifyRight	= 414;	// nil
const MessageT	cmd_JustifyFull		= 415;	// nil

											// Note: Command numbers 500 to 599 are
											// reserved for use by the Net/Internet classes

											// Note: Command numbers 600 to 799 are
											// reserved for use by the Debugging classes

					// Miscellaneous Messages
const MessageT	msg_GrowZone		= 801;	// SInt32* (in: bytes needed, out: bytes freed)
const MessageT	msg_EventHandlerNote= 802;	// SEventHandlerNote*

					// Attachment Messages
const MessageT	msg_Event			= 810;	// EventRecord*
const MessageT	msg_DrawOrPrint		= 811;	// Rect* (frame of Pane)
const MessageT	msg_Click			= 812;	// SMouseDownEvent*
const MessageT	msg_AdjustCursor	= 813;	// SAdjustMouseEvent*
const MessageT	msg_KeyPress		= 814;	// EventRecord* (KeyDown or AutoKey event)
const MessageT	msg_CommandStatus	= 815;	// SCommandStatus*
const MessageT	msg_PostAction		= 816;	// LAction*
const MessageT	msg_FinishCreate	= 817;	// LPane*
const MessageT	msg_ContextClick	= 818;	// Point* (global mouse point)

const MessageT	msg_OK				= 900;	// nil
const MessageT	msg_Cancel			= 901;	// nil

const CommandT	cmd_UseMenuItem		= -1;	// --- (special flag, message is never sent)
const MessageT	msg_AnyMessage		= -2;	// --- (special flag, message is never sent)

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
