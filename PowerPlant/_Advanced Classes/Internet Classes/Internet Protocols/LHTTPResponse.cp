// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPResponse.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling HTTP style responses.
//
// NOTE: Since HTTP responses always include the requested data or an error
//		description (usually in HTML) this class also includes the actual
//		LHTTPMessage return from the server.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LHTTPResponse.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LHTTPResponse							Default Constructor
// ---------------------------------------------------------------------------

LHTTPResponse::LHTTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ LHTTPResponse							Constructor
// ---------------------------------------------------------------------------

LHTTPResponse::LHTTPResponse(
	char*	inResponse)
{
	SetResponse(inResponse);
}


// ---------------------------------------------------------------------------
//	¥ ~LHTTPResponse						Destructor
// ---------------------------------------------------------------------------

LHTTPResponse::~LHTTPResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------
//
void
LHTTPResponse::SetResponse(
	const char*	inResponse)
{
	SetResponse(inResponse, PP_CSTD::strlen(inResponse));
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LHTTPResponse::SetResponse(
	const char*	inResponse,
	UInt32		inLength)
{
	const char * p, *q;

	p = PP_CSTD::strchr(inResponse, LF);
	if (p) {
		mResponseText.assign(inResponse, (UInt32) (p - inResponse)); 	// -1 removes the CR

		// Pull code out of response
		q = PP_CSTD::strchr(inResponse, ' ');
		if (q)
			mResponseCode = PP_CSTD::atol(++q);

		p++;		// +1 walks past the LF
		if (*p)
			mMessage.SetMessage(p, inLength - (p - inResponse));
	}
}


// ---------------------------------------------------------------------------
//	¥ SetResponse
// ---------------------------------------------------------------------------

void
LHTTPResponse::SetResponse(
	const char*		inResponse,
	LDynamicBuffer*	inResponseData)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);
	char * p;

	// strip CRLF
	p = PP_CSTD::strchr(tempResponse, LF);
	if (p)
		*p = '\0';
	p = PP_CSTD::strchr(tempResponse, CR);
	if (p)
		*p = '\0';
	mResponseText = tempResponse;

	// Pull code out of response
	p = const_cast<char*>(PP_CSTD::strchr(inResponse, ' '));
	if (p)
		mResponseCode = PP_CSTD::atol(++p);

	// Set the data, we just grab the stream to prevent having to
	// duplicate a potentially large amount of data
	mMessage.SetMessage(inResponseData);
}


PP_End_Namespace_PowerPlant
