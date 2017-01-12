// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAMModalDialogs.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UAMModalDialogs
#define _H_UAMModalDialogs
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LCommander;

// ---------------------------------------------------------------------------

class	UAMModalDialogs {
public:
	static Boolean		AskForOneNumber(
								LCommander*		inSuper,
								ResIDT			inDialogID,
								PaneIDT			inEditTextID,
								SInt32&			ioNumber);

	static Boolean		AskForOneString(
								LCommander*		inSuper,
								ResIDT			inDialogID,
								PaneIDT			inEditTextID,
								Str255			ioString);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
