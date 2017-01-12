// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_DebugConstants.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_PP_DebugConstants
#define _H_PP_DebugConstants
#pragma once

#include <PP_Types.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

//	CommandT's in the range of 600-799 (inclusive) are reserved for use
//	by the Debugging Classes. Other ID's (such as Resource ID's) are not
//	reserved, but typically will be chosen from this same range as well.

// ¥ Sundry constants

const	UInt32		kDefault_Threshold				=	60;
const	Size		kDefault_EatMemorySize			=	10240;	// 10K

// ¥ Resources (and embedded PaneIDT's)

const	ResIDT		MENU_DebugMenu					=	600;
const	ResIDT		MENU_SubmenuCommandChain		=	225;
const	ResIDT		MENU_SubmenuPaneTree			=	226;
const	ResIDT		MENU_SubmenuCompactHeap			=	227;
const	ResIDT		MENU_SubmenuPurgeHeap			=	228;
const	ResIDT		MENU_SubmenuCompactAndPurgeHeap	=	229;
const	ResIDT		MENU_SubmenuDebugNewFlags		=	230;
const	ResIDT		MENU_SubmenuQCTests				=	231;
const	ResIDT		MENU_SubmenuQCActions			=	232;
const	ResIDT		MENU_SubmenugDebugThrow			=	233;
const	ResIDT		MENU_SubmenugDebugSignal		=	234;

const	ResIDT		icsX_DebugMenuTitle				=	600;

const	ResIDT		PPob_EatMemoryDialog			=	601;
const	ResIDT		PPob_AMEatMemoryDialog			=	605;
const	PaneIDT		EatMemoryDialog_RadioHandle		=	1;	// Cooresponds to EMemoryType
const	PaneIDT		EatMemoryDialog_RadioPtr		=	2;	//	values for ease of access.
const	PaneIDT		EatMemoryDialog_EditAmount		=	FOUR_CHAR_CODE('AMNT');
const	PaneIDT		EatMemoryDialog_RadioGroup		=	FOUR_CHAR_CODE('RGPV');

const	ResIDT		PPob_LCommanderTreeWindow		=	602;
const	ResIDT		PPob_AMLCommanderTreeWindow		=	606;
const	ResIDT		PPob_LPaneTreeWindow			=	603;
const	ResIDT		PPob_AMLPaneTreeWindow			=	607;
const	PaneIDT		TreeWindow_Tree					=	FOUR_CHAR_CODE('LTRE');
const	PaneIDT		TreeWindow_ButtonRefresh		=	FOUR_CHAR_CODE('RFrh');
const	PaneIDT		TreeWindow_CheckAutoRefresh		=	FOUR_CHAR_CODE('ATrh');
const	PaneIDT		TreeWindow_ButtonDumpTree		=	FOUR_CHAR_CODE('DmpT');
const	PaneIDT		TreeWindow_CheckExtraInfo		=	FOUR_CHAR_CODE('xtrI');

const	ResIDT		PPob_DialogValidatePPob			=	604;
const	ResIDT		PPob_AMDialogValidatePPob		=	608;
const	PaneIDT		ValidatePPob_EditResIDT			=	FOUR_CHAR_CODE('EDIT');


// ¥ Commands

// Sundry

const	CommandT	cmd_Debugger					=	600;
const	CommandT	cmd_ScrambleHeap				=	601;
const	CommandT	cmd_LaunchMemoryApp				=	602;
const	CommandT	cmd_EatMemorySpecify			=	603;
const	CommandT	cmd_ReleaseEatPools				=	604;
const	CommandT	cmd_ValidatePPob				=	608;
const	CommandT	cmd_ValidateAllPPobs			=	609;

// DebugNew

const	CommandT	cmd_DebugNewValidate			=	605;
const	CommandT	cmd_DebugNewReport				=	606;
const	CommandT	cmd_DebugNewForget				=	607;
const	CommandT	cmd_DebugNewFlags				=	626;
const	CommandT	cmd_DebugNewFlagsDontFree		=	627;
const	CommandT	cmd_DebugNewFlagsApplZone		=	628;

// QC

const	CommandT	cmd_QCAction					=	610;
const	CommandT	cmd_QCAction1Sec				=	611;
const	CommandT	cmd_QCAction2Sec				=	612;
const	CommandT	cmd_QCAction3Sec				=	613;
const	CommandT	cmd_QCAction4Sec				=	614;
const	CommandT	cmd_QCAction5Sec				=	615;
const	CommandT	cmd_QCActionNow					=	616;
const	CommandT	cmd_QCActionStop				=	617;
const	CommandT	cmd_QCActionRepeat				=	618;
const	CommandT	cmd_QCActionIdle				=	619;

const	CommandT	cmd_QCActivate					=	620;
const	CommandT	cmd_QCSaveState					=	621;
const	CommandT	cmd_QCTests						=	622;

// Pane hierarchy view

const	CommandT	cmd_PaneTree					=	630;
const	CommandT	cmd_PaneTree1Sec				=	631;
const	CommandT	cmd_PaneTree2Sec				=	632;
const	CommandT	cmd_PaneTree3Sec				=	633;
const	CommandT	cmd_PaneTree4Sec				=	634;
const	CommandT	cmd_PaneTree5Sec				=	635;
const	CommandT	cmd_PaneTreeNow					=	636;
const	CommandT	cmd_PaneTreeStop				=	637;
const	CommandT	cmd_PaneTreeShowHide			=	638;

// Commander tree

const	CommandT	cmd_CommandChain				=	640;
const	CommandT	cmd_CommandChain1Sec			=	641;
const	CommandT	cmd_CommandChain2Sec			=	642;
const	CommandT	cmd_CommandChain3Sec			=	643;
const	CommandT	cmd_CommandChain4Sec			=	644;
const	CommandT	cmd_CommandChain5Sec			=	645;
const	CommandT	cmd_CommandChainNow				=	646;
const	CommandT	cmd_CommandChainStop			=	647;
const	CommandT	cmd_CommandChainShowHide		=	648;

// gDebugThrow

const	CommandT	cmd_gDebugThrow					=	650;
const	CommandT	cmd_gDebugThrowNothing			=	651;
const	CommandT	cmd_gDebugThrowAlert			=	652;
const	CommandT	cmd_gDebugThrowDebugger			=	653;

// gDebugSignal

const	CommandT	cmd_gDebugSignal				=	660;
const	CommandT	cmd_gDebugSignalNothing			=	661;
const	CommandT	cmd_gDebugSignalAlert			=	662;
const	CommandT	cmd_gDebugSignalDebugger		=	663;

// Compact heap action

const	CommandT	cmd_CompactHeap					=	670;
const	CommandT	cmd_CompactHeap1Sec				=	671;
const	CommandT	cmd_CompactHeap2Sec				=	672;
const	CommandT	cmd_CompactHeap3Sec				=	673;
const	CommandT	cmd_CompactHeap4Sec				=	674;
const	CommandT	cmd_CompactHeap5Sec				=	675;
const	CommandT	cmd_CompactHeapNow				=	676;
const	CommandT	cmd_CompactHeapStop				=	677;
const	CommandT	cmd_CompactHeapRepeat			=	678;
const	CommandT	cmd_CompactHeapIdle				=	679;

// Purge heap action

const	CommandT	cmd_PurgeHeap					=	680;
const	CommandT	cmd_PurgeHeap1Sec				=	681;
const	CommandT	cmd_PurgeHeap2Sec				=	682;
const	CommandT	cmd_PurgeHeap3Sec				=	683;
const	CommandT	cmd_PurgeHeap4Sec				=	684;
const	CommandT	cmd_PurgeHeap5Sec				=	685;
const	CommandT	cmd_PurgeHeapNow				=	686;
const	CommandT	cmd_PurgeHeapStop				=	687;
const	CommandT	cmd_PurgeHeapRepeat				=	688;
const	CommandT	cmd_PurgeHeapIdle				=	689;

// Compact and purge heap action

const	CommandT	cmd_CompactAndPurge				=	690;
const	CommandT	cmd_CompactAndPurge1Sec			=	691;
const	CommandT	cmd_CompactAndPurge2Sec			=	692;
const	CommandT	cmd_CompactAndPurge3Sec			=	693;
const	CommandT	cmd_CompactAndPurge4Sec			=	694;
const	CommandT	cmd_CompactAndPurge5Sec			=	695;
const	CommandT	cmd_CompactAndPurgeNow			=	696;
const	CommandT	cmd_CompactAndPurgeStop			=	697;
const	CommandT	cmd_CompactAndPurgeRepeat		=	698;
const	CommandT	cmd_CompactAndPurgeIdle			=	699;

// HeapManager

const	CommandT	cmd_LaunchHeapBoss				=	700;

// Endcaps

const	CommandT	cmd_DebugFirstCommand			=	600;	// cmd_Debugger
const	CommandT	cmd_DebugLastCommand			=	799;	// currently unused.


// ¥ Messages

const	MessageT	msg_RefreshTree					=	cmd_CommandChainNow;
const	MessageT	msg_AutoRefreshTree				=	cmd_CommandChainStop;
const	MessageT	msg_DumpTree					=	669;
const	MessageT	msg_DisplayExtraTreeInfo		=	659;

const	MessageT	msg_StartRepeating				=	639;
const	MessageT	msg_StopRepeating				=	649;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_PP_DebugConstants
