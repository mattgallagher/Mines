// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetResponse.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LNetResponse
#define _H_LNetResponse
#pragma once

#include <LDynamicBuffer.h>
#include <string>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LInternetResponse {
public:
							LInternetResponse();

	virtual					~LInternetResponse();

	virtual SInt32			GetResponseCode() const;
	
	virtual const char*		GetResponse() const;

	virtual void			SetResponse(const char* inResponse) = 0;

	virtual void			ResetResponse() {InitResponse();}

protected:
	SInt32					mResponseCode;
	PP_STD::string			mResponseText;

private:
	void					InitResponse();
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	¥ GetResponseCode										 [inline] [public]
// ---------------------------------------------------------------------------

inline SInt32
LInternetResponse::GetResponseCode() const
{
	return mResponseCode;
}


// ---------------------------------------------------------------------------
//	¥ GetResponse											 [inline] [public]
// ---------------------------------------------------------------------------

inline const char*
LInternetResponse::GetResponse() const
{
	return mResponseText.c_str();;
}


// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
