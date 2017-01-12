// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UControlRegistry.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UControlRegistry
#define _H_UControlRegistry
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class UControlRegistry {
public:
			static	void		RegisterClasses 	();
			static 	void		RegisterAMClasses	();
			static 	void		RegisterGAClasses	();

			static	void		RegisterClassicControls(
										bool		inAlwaysGA = false);

private:
			static	void		RegisterAMAbstractionClasses	();
			static	void		RegisterAMImpClasses			();
			static	void		RegisterGAImpClasses			();
			static	void		RegisterOther					();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
