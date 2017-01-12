// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIRCResponse.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LIRCResponse
#define _H_LIRCResponse
#pragma once

#include <LInternetResponse.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LIRCResponse : public LInternetResponse {
public:
							LIRCResponse();
							LIRCResponse(char * inData);
							LIRCResponse(const LIRCResponse& copyResponse);
	virtual					~LIRCResponse();

	virtual void			SetResponse(const char* inResponse);

	virtual inline const char *	GetResponsePrefix() {return mPrefix.c_str();}
	virtual inline const char *	GetResponseCommandText() {return mCommandText.c_str();}
	virtual inline const char *	GetResponseParams() {return mParams.c_str();}

	virtual void			ResetResponse() {
								LInternetResponse::ResetResponse();
								mPrefix[0] = '\0';
								mCommandText[0] = '\0';
								mParams[0] = '\0';
							}
protected:
	PP_STD::string			mPrefix;
	PP_STD::string			mCommandText;
	PP_STD::string			mParams;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
