// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPResponse.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling POP3 style responses.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFTPResponse.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFTPResponse							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LFTPResponse::LFTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ LFTPResponse							Constructor				  [public]
// ---------------------------------------------------------------------------

LFTPResponse::LFTPResponse(char * inResponse)
{
	SetResponse(inResponse);
}


// ---------------------------------------------------------------------------
//	¥ LFTPResponse							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LFTPResponse::LFTPResponse(const LFTPResponse& copyResponse)
	:LInternetResponse(copyResponse)
{
	mResponseData = copyResponse.mResponseData;
}


// ---------------------------------------------------------------------------
//	¥ ~LFTPResponse							Destructor				  [public]
// ---------------------------------------------------------------------------

LFTPResponse::~LFTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LFTPResponse::SetResponse(const char * inResponse)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);

	//Since FTP servers send mostly human readable text,
	//	mResponseData contains the full text of the response
	//	in case you want to display it to the user.
	//We do our best in LFTPConnection::WaitResponse() to get
	//	all the data for you.
	mResponseData = tempResponse;

	char * p = PP_CSTD::strtok(tempResponse, " ");
	if (p) {
		mResponseCode = PP_CSTD::atol(p);

		p = PP_CSTD::strtok(nil, CRLF);
		mResponseText = p;
	}
}


PP_End_Namespace_PowerPlant
