// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCMMUtils.h					PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UCMMUtils
#define _H_UCMMUtils
#pragma once

#include <PP_Prefix.h>

#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace	UCMMUtils {

	// Although there is nothing preventing you from accessing these
	// globals directly, you should go through the Get/Set accessors
	// for compatibility reasons (in case something changes in the
	// future).

	extern	bool			gCMMPresent;
	extern	bool			gCMMInitialized;

	inline	void			SetCMMPresent( bool inPresent )
								{
									gCMMPresent = inPresent;
								}

	inline	void			SetCMMInitialized( bool inInit )
								{
									gCMMInitialized = inInit;
								}

	inline	bool			IsCMMPresent()		{ return gCMMPresent; }
	
	inline	bool			IsCMMInitialized()	{ return gCMMInitialized; }

			void			Initialize();

			bool			StripFirstDivider( MenuHandle inMenuH );
			
			bool			StripLastDivider( MenuHandle inMenuH );
			
			bool			StripDoubleDividers( MenuHandle	inMenuH );
}

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
