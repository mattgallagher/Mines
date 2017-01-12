// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPOP3Response.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPOP3Response
#define _H_LPOP3Response
#pragma once

#include <LInternetResponse.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LPOP3Response : public LInternetResponse {

public:
							LPOP3Response();
							LPOP3Response(char * inResponse);
							LPOP3Response(const LPOP3Response& copyResponse);
	virtual					~LPOP3Response();

	virtual void			SetResponse(const char* inResponse);
	virtual void			SetResponse(const char * inResponse, LDynamicBuffer * inResponseData);

	inline Boolean			GetStatus() {return (Boolean) mResponseCode;}

	virtual LDynamicBuffer*	GetResponseData();

	virtual void			ResetResponse();

protected:
	LDynamicBuffer			mResponseData;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
