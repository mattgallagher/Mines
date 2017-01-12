// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFingerResponse.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Class for handling Finger style responses.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFingerResponse.h>
#include <cstring>
#include <cstdlib>
#include <UInternet.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LFingerResponse()
// ---------------------------------------------------------------------------
//	Constructor

LFingerResponse::LFingerResponse()
{
}

// ---------------------------------------------------------------------------
//		¥ LFingerResponse()
// ---------------------------------------------------------------------------
//	Alternate Constructor

LFingerResponse::LFingerResponse(char * inResponse)
{
	SetResponse(inResponse);
}


// ---------------------------------------------------------------------------
//		¥ LFingerResponse()
// ---------------------------------------------------------------------------
//	Copy Constructor

LFingerResponse::LFingerResponse(const LFingerResponse& copyResponse)
	:LInternetResponse(copyResponse)
{
}

// ---------------------------------------------------------------------------
//		¥ ~LFingerResponse
// ---------------------------------------------------------------------------
//	Destructor

LFingerResponse::~LFingerResponse()
{
}

// ---------------------------------------------------------------------------
//		¥ SetResponse
// ---------------------------------------------------------------------------
//

void
LFingerResponse::SetResponse(const char * inResponse) {
	ResetResponse();
	mResponseText = inResponse;
}

PP_End_Namespace_PowerPlant
