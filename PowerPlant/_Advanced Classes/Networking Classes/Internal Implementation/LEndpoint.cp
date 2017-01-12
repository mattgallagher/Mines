// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEndpoint.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Encapsulates the idea of a "network endpoint," or one side of a two-way
//	communication link. The endpoint is the object that is used to send or
//	receive data over the network.
//
//	LEndpoint is an abstract base class (thus its constructor is declared
//	protected). Use one of the predefined subclasses, such as
//	LMacTCPTCPEndpoint or LOpenTptTCPEndpoint, or use the
//	UNetworkFactory::CreateTCPEndpoint function to create the appropriate
//	endpoint for the system software that's installed on the user's machine.

#include <LEndpoint.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LEndpoint ===

// ---------------------------------------------------------------------------
//		¥ LEndpoint						[protected]
// ---------------------------------------------------------------------------
//	Constructor

LEndpoint::LEndpoint()
{
	mQueueSends = false;
}

// ---------------------------------------------------------------------------
//		¥ ~LEndpoint
// ---------------------------------------------------------------------------
//	Destructor

LEndpoint::~LEndpoint()
{
}

PP_End_Namespace_PowerPlant
