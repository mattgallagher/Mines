// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPResponse.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFTPResponse
#define _H_LFTPResponse
#pragma once

#include <LInternetResponse.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#define kFTPDataConOpen			125
#define kFTPAboutToOpen			150
#define kFTPCommandOK			200
#define kFTPCmdNotImp			202
#define kFTPNameSystem			215
#define kFTPServiceReady		220
#define kFTPServiceClosing		221
#define kFTPClosingDataCon		226
#define kFTPEnteringPsv			227
#define kFTPUserLoggedIn		230
#define kFTPFileActionOK		250
#define kFTPPathCreated			257
#define kFTPUserNameOK			331
#define kFTPReqOKPending		350
#define kFTPConnectionClosed	426
#define kFTPSyntaxErr			500

// ---------------------------------------------------------------------------

class LFTPResponse : public LInternetResponse {

public:
							LFTPResponse();
							LFTPResponse(char * inResponse);
							LFTPResponse(const LFTPResponse& copyResponse);
	virtual					~LFTPResponse();

	inline Boolean			CommandOK() {return mResponseCode == kFTPCommandOK;}

	virtual void			SetResponse(const char* inResponse);

	virtual inline const char *	GetResponseData() {return mResponseData.c_str();}

	virtual void			ResetResponse() {
								LInternetResponse::ResetResponse();
								mResponseData = "";
							}
protected:
	PP_STD::string			mResponseData;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
