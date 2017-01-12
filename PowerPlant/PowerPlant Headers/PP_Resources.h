// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Resources.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_PP_Resources
#define _H_PP_Resources
#pragma once

#include <PP_Types.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

									// ¥ MBAR Resources
const ResIDT	MBAR_Standard	= 128;
const ResIDT	MBAR_Aqua		= 129;

const ResIDT	MBAR_Initial	= MBAR_Standard;	// Deprecated

									// ¥ MENU Resources
const ResIDT	MENU_Apple		= 128;
const ResIDT	MENU_Font		= 250;
const ResIDT	MENU_Size		= 251;
const ResIDT	MENU_Style		= 252;
const ResIDT	MENU_Empty		= 12345;	// Menu with no items
const ResIDT	MENU_Contextual	= 12346;	// Reserved for contextual menu

const ResIDT	ALRT_About					= 128;

									// ¥ In "PP Document Alerts.rsrc"
const ResIDT	ALRT_SaveChangesClosing		= 201;
const ResIDT	ALRT_SaveChangesQuitting	= 202;
const SInt16	answer_Save					= 1;
const SInt16	answer_Cancel				= 2;
const SInt16	answer_DontSave				= 3;

const ResIDT	ALRT_ConfirmRevert			= 203;
									// End "PP Document Alerts.rsrc"

									// ¥ In "PP Copy & Customize.rsrc"
const ResIDT	ALRT_LowMemory				= 204;

const ResIDT	STRx_Standards				= 200;
const SInt16	str_ProgramName				= 1;
const SInt16	str_SaveAs					= 2;
const SInt16	str_OpenDialogTitle			= 3;
const SInt16	str_SaveDialogTitle			= 4;
const SInt16	str_HelpMenuTitle			= 5;
									// End "PP Copy & Customize.rsrc"

									// ¥ In "PP Action Strings.rsrc"
const ResIDT	STRx_RedoEdit				= 150;
const ResIDT	STRx_UndoEdit				= 151;
const SInt16	str_CantRedoUndo			= 1;
const SInt16	str_RedoUndo				= 2;
const SInt16	str_Cut						= 3;
const SInt16	str_Copy					= 4;
const SInt16	str_Paste					= 5;
const SInt16	str_Clear					= 6;
const SInt16	str_Typing					= 7;

const ResIDT	STRx_RedoDrag				= 152;
const ResIDT	STRx_UndoDrag				= 153;
const SInt16	str_DragCopy				= 1;
const SInt16	str_DragMove				= 2;
const SInt16	str_DragDrop				= 3;

const ResIDT	STRx_RedoStyle				= 156; 	// Note: 155 is used by dtF Database
const ResIDT	STRx_UndoStyle				= 157;
const SInt16	str_Font					= 1;
const SInt16	str_Size					= 2;
const SInt16	str_Style					= 3;
const SInt16	str_Face					= str_Style;
const SInt16	str_Justification			= 4;
const SInt16	str_Alignment				= str_Justification;
const SInt16	str_Color					= 5;

const ResIDT	STRx_DefaultEditStrings		= 220;
const SInt16	str_CutDefault				= 1;
const SInt16	str_CopyDefault				= 2;
const SInt16	str_PasteDefault			= 3;
const SInt16	str_ClearDefault			= 4;
									// End "PP Action Strings.rsrc"

									// ¥ In "PP Cursors.rsrc"
const ResIDT	CURS_ThemeBase				=	400;	// Add to ThemeCursor constants
														//   to get CURS resource ID
									// End "PP Cursors.rsrc"

const ResIDT	Txtr_SystemFont				= 0;

									// ¥ In "GetDirectory.rsrc"
const ResIDT	DLOG_GetDirectory			= 600;
									// End "GetDirectory.rsrc"

									// ¥ In "NumberParts.rsrc"
const ResIDT	NoPt_USNumberParts			= 200;
									// End "NumberParts.rsrc"

									// PP Resource Types
const OSType	ResType_PPob				= FOUR_CHAR_CODE('PPob');
const OSType	ResType_IDList				= FOUR_CHAR_CODE('RidL');
const ResType	ResType_MenuCommands		= FOUR_CHAR_CODE('Mcmd');
const ResType	ResType_AETable				= FOUR_CHAR_CODE('aedt');
const ResType	ResType_TextTraits			= FOUR_CHAR_CODE('Txtr');
const ResType	ResType_NumberParts			= FOUR_CHAR_CODE('NoPt');

									// Toolbox Resource Types
const OSType	ResType_MacWindow			= FOUR_CHAR_CODE('WIND');
const OSType	ResType_WindowColorTable	= FOUR_CHAR_CODE('wctb');
const OSType	ResType_MenuBar				= FOUR_CHAR_CODE('MBAR');
const OSType	ResType_MenuDefProc			= FOUR_CHAR_CODE('MDEF');
const OSType	ResType_Driver				= FOUR_CHAR_CODE('DRVR');
const OSType	ResType_Open				= FOUR_CHAR_CODE('open');
const OSType	ResType_Icon				= FOUR_CHAR_CODE('ICON');
const OSType	ResType_IconList			= FOUR_CHAR_CODE('ICN#');
const OSType	ResType_Picture				= FOUR_CHAR_CODE('PICT');
const OSType	ResType_Text				= FOUR_CHAR_CODE('TEXT');
const OSType	ResType_TextStyle			= FOUR_CHAR_CODE('styl');
const OSType	ResType_AnimatedCursor		= FOUR_CHAR_CODE('acur');


typedef struct {
	SInt16	numResources;
	SInt16	resIDs[1];
} SResIDListRec, *SResIDListP, **SResIDListH;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
