// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSMTPResponse.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling SMTP style responses.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSMTPResponse.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSMTPResponse							Default Constructor
// ---------------------------------------------------------------------------

LSMTPResponse::LSMTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ LSMTPResponse							Constructor
// ---------------------------------------------------------------------------

LSMTPResponse::LSMTPResponse(
	char*	inResponse)
{
	SetResponse(inResponse);
}


// ---------------------------------------------------------------------------
//	¥ LSMTPResponse							Copy Constructor
// ---------------------------------------------------------------------------

LSMTPResponse::LSMTPResponse(
	const LSMTPResponse&	copyResponse)

	:LInternetResponse(copyResponse)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LSMTPResponse						Destructor
// ---------------------------------------------------------------------------

LSMTPResponse::~LSMTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LSMTPResponse::SetResponse(
	const char*	inResponse)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);

	char * p = PP_CSTD::strtok(tempResponse, " ");
	if (p) {
		mResponseCode = PP_CSTD::atol(p);

		p = PP_CSTD::strtok(nil, "");
		mResponseText = p;
	}
}


PP_End_Namespace_PowerPlant
