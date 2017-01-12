// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPOP3Response.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling POP3 style responses.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPOP3Response.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPOP3Response							Default Constructor
// ---------------------------------------------------------------------------

LPOP3Response::LPOP3Response()
{
}


// ---------------------------------------------------------------------------
//	¥ LPOP3Response()						Constructor
// ---------------------------------------------------------------------------

LPOP3Response::LPOP3Response(
	char*	inResponse)
{
	SetResponse(inResponse);
}


// ---------------------------------------------------------------------------
//	¥ LPOP3Response							Copy Constructor
// ---------------------------------------------------------------------------

LPOP3Response::LPOP3Response(
	const LPOP3Response&	copyResponse)
	
	: LInternetResponse(copyResponse)
{
	mResponseData = copyResponse.mResponseData;
}


// ---------------------------------------------------------------------------
//	¥ ~LPOP3Response						Destructor
// ---------------------------------------------------------------------------

LPOP3Response::~LPOP3Response()
{
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LPOP3Response::SetResponse(
	const char* inResponse)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);
	char * p, * q;

	p = PP_CSTD::strtok(tempResponse, CRLF);
	mResponseCode = (p[0] == '+');

	q = PP_CSTD::strchr(p, ' ');
	if (q)
		mResponseText = ++q;

	p = PP_CSTD::strtok(nil, "");
	if (p)
		mResponseData.SetBuffer(++p);	// +1 walks past linefeed we'll always have
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LPOP3Response::SetResponse(
	const char*		inResponse,
	LDynamicBuffer*	inResponseData)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);
	char * p;

	// Set the response
	mResponseCode = (*tempResponse == '+');
	p = PP_CSTD::strchr(tempResponse, ' ');
	if (p)
		mResponseText = ++p;

	// Set the data, we just grab the stream to prevent having to
	// duplicate a potentially large amount of data

	mResponseData.SetStream(inResponseData->DetachStream());
}


// ---------------------------------------------------------------------------
//	¥ GetResponseData
// ---------------------------------------------------------------------------

LDynamicBuffer*
LPOP3Response::GetResponseData()
{
	return &mResponseData;
}


// ---------------------------------------------------------------------------
//	¥ ResetResponse
// ---------------------------------------------------------------------------

void
LPOP3Response::ResetResponse()
{
	LInternetResponse::ResetResponse();
	mResponseData.ResetBuffer();
}


PP_End_Namespace_PowerPlant
