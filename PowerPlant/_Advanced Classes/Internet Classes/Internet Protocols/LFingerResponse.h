// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFingerResponse.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFingerResponse
#define _H_LFingerResponse
#pragma once

#include <LInternetResponse.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LFingerResponse
// ===========================================================================

class LFingerResponse : public LInternetResponse {

public:
							LFingerResponse();
							LFingerResponse(char * inResponse);
							LFingerResponse(const LFingerResponse& copyResponse);
	virtual					~LFingerResponse();

	virtual void			SetResponse(const char* inResponse);

};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
