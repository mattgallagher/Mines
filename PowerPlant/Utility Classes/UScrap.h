// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UScrap.h				PowerPlant 2.2.2		©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UScrap
#define _H_UScrap
#pragma once

#include <PP_Prefix.h>
#include <Scrap.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UScrap {

		// On Carbon, there is a ScrapRef which changes every time the
		// scrap is cleared. On Classic, the scrapCount field returned
		// by InfoScrap() does the same thing. However, the two values
		// are different types. We conditionally define RefType so that
		// we can use the same API for Carbon and Classic.

	#if PP_Target_Carbon
		typedef ScrapRef	RefType;
	#else
		typedef SInt16		RefType;
	#endif
	
	RefType		GetReference();

	SInt32		GetData(
						ScrapFlavorType		inDataType,
						Handle				ioData);

	bool		HasData( ScrapFlavorType inDataType );

	void		SetData(
						ScrapFlavorType		inDataType,
						const void*			inDataPtr,
						SInt32				inDataLength,
						bool				inClear = true);

	void		SetData(
						ScrapFlavorType		inDataType,
						Handle				inData,
						bool				inClear = true);

	void		ClearData();
}

PP_End_Namespace_PowerPlant

#endif
