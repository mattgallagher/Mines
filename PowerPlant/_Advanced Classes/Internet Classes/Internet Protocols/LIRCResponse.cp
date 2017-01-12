// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIRCResponse.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling IRC style responses.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LIRCResponse.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>
#include <IRC_Constants.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LIRCResponse							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LIRCResponse::LIRCResponse()
{
	mResponseCode = IRC_UNDEFINED;
}


// ---------------------------------------------------------------------------
//	¥ LIRCResponse							Constructor				  [public]
// ---------------------------------------------------------------------------

LIRCResponse::LIRCResponse(
	char*	inData)
{
	SetResponse(inData);
}


// ---------------------------------------------------------------------------
//	¥ LIRCResponse							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LIRCResponse::LIRCResponse(
	const LIRCResponse&	copyResponse)
	
	: LInternetResponse(copyResponse)
{
	mPrefix = copyResponse.mPrefix;
	mCommandText = copyResponse.mCommandText;
	mParams = copyResponse.mParams;
}


// ---------------------------------------------------------------------------
//	¥ ~LIRCResponse							Destructor				  [public]
// ---------------------------------------------------------------------------

LIRCResponse::~LIRCResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ SetResponse													  [public]
// ---------------------------------------------------------------------------

void
LIRCResponse::SetResponse(
	const char*	inResponse)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inResponse) + 1), true);
	PP_CSTD::strcpy(tempResponse, inResponse);
	char * p;

	// Since IRC is so loosely defined, this contains the full
	// text of the response so that others can parse the message
	// themselves via a "unknown" broadcast
	mResponseText = inResponse;

	p = PP_CSTD::strtok(tempResponse, " ");
	// Get the prefix
	if (*p == ':') {
		mPrefix = ++p;
		p = PP_CSTD::strtok(nil, " ");
	} else {
		mPrefix = "";
	}

	// Determine if command is textual or numeric
	if (PP_CSTD::isalpha(*p)) {
		mCommandText = p;
		mResponseCode = IRC_UNDEFINED;
	} else {
		mResponseCode = PP_CSTD::atol(p);
		// Skip over the users nick following the numeric command...
		p = PP_CSTD::strtok(nil, " ");
	}

	// Grab the rest as params...
	p = PP_CSTD::strtok(nil, "");
	// step over the leading colon if there is one
	if (*p == ':')
		p++;

	mParams = p;
}


PP_End_Namespace_PowerPlant
