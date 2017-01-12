// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UValidPPob.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: MW IDE Team. Heavy modifications by John C. Daub

#ifndef _H_UValidPPob
#define _H_UValidPPob
#pragma once

#include <TArray.h>
#include <PP_DebugConstants.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LStream;

namespace UValidPPob {

	void				ValidatePPob(
								ResIDT				inDialogID = PPob_DialogValidatePPob,
								PaneIDT				inEditFieldID = ValidatePPob_EditResIDT);
	void				ValidateAllPPobs();
	bool				ValidateOnePPob(
								ResIDT				inPPobID,
								TArray<ClassIDT>&	outBadClassIDArray);

	bool				ValidateObjectStream(
								LStream&			ioStream,
								ClassIDT&			outBadClasID);
}

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UValidPPob
