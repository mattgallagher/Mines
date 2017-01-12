// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetResponse.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Base class for handling typical Internet style protocol responses which
//	include a response number, and associated text response.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LInternetResponse.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LInternetResponse						Default Constructor
// ---------------------------------------------------------------------------

LInternetResponse::LInternetResponse()
{
	InitResponse();
}


// ---------------------------------------------------------------------------
//	¥ ~LInternetResponse					Destructor
// ---------------------------------------------------------------------------

LInternetResponse::~LInternetResponse()
{
}


// ---------------------------------------------------------------------------
//	¥ InitResponse()
// ---------------------------------------------------------------------------

void
LInternetResponse::InitResponse()
{
	mResponseCode = 0;
	mResponseText = "";
}

PP_End_Namespace_PowerPlant
