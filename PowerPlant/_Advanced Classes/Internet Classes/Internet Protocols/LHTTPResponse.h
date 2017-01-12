// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPResponse.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LHTTPResponse
#define _H_LHTTPResponse
#pragma once

#include <LInternetResponse.h>
#include <LHTTPMessage.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LHTTPResponse : public LInternetResponse {

public:
							LHTTPResponse();

							LHTTPResponse(char * inResponse);

	virtual					~LHTTPResponse();

	virtual void			SetResponse(const char* inResponse);
	virtual void			SetResponse(const char * inResponse, UInt32 inLength);
	virtual void			SetResponse(const char * inResponse, LDynamicBuffer * inResponseData);
							// Accept all 2xx responses as valid
	inline Boolean			GetStatus() {return (GetResponseCode()/100 == 2);}

	virtual inline LHTTPMessage*	GetReturnMessage() {return &mMessage;}

	virtual void			ResetResponse() {
								LInternetResponse::ResetResponse();
								mMessage.ResetMembers();
							}
protected:
	LHTTPMessage			mMessage;

};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
