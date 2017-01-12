// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSMTPResponse.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSMTPResponse
#define _H_LSMTPResponse
#pragma once

#include <LInternetResponse.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LSMTPResponse : public LInternetResponse {

public:
						LSMTPResponse();
						
						LSMTPResponse( char* inResponse );
						
						LSMTPResponse( const LSMTPResponse& copyResponse );
						
	virtual				~LSMTPResponse();

	virtual void		SetResponse( const char* inResponse );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
