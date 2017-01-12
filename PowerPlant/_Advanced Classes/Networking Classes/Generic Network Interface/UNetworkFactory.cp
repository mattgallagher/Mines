// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworkFactory.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Use this class to create network endpoints and mappers. It will choose
//	the best configuration (i.e. MacTCP or OpenTransport) given the system
//	software that is installed on the host machine.

#include <UNetworkFactory.h>

#if USE_MACTCP
#include <LMacTCPTCPEndpoint.h>
#include <LMacTCPUDPEndpoint.h>
#include <LMacTCPInetMapper.h>
#include <UMacTCPSupport.h>
#endif

#if USE_OPENTPT
#include <LOpenTptTCPEndpoint.h>
#include <LOpenTptUDPEndpoint.h>
#include <LOpenTptInetMapper.h>
#include <UOpenTptSupport.h>
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ CreateTCPEndpoint
// ---------------------------------------------------------------------------
//	Create the best endpoint object for TCP/IP connections given
//	the available system software.

LTCPEndpoint*
UNetworkFactory::CreateTCPEndpoint(UInt32		inReceiveBufferSize)
{
#if !USE_MACTCP
	#pragma unused (inReceiveBufferSize)
#endif

	// ensure TCP is available
	ThrowIf_(!HasTCP());

#if USE_OPENTPT
	if (HasOpenTransport()) {
		UOpenTptSupport::StartOpenTransport();
							// Put this here because we must call
							// InitOpenTransport() before OTCreateConfiguration.

		return new LOpenTptTCPEndpoint();
	}
	else
#endif
	{
#if USE_MACTCP
		return new LMacTCPTCPEndpoint(inReceiveBufferSize);
#else
		return nil;		//This just prevents compile time warning
#endif
	}
}

// ---------------------------------------------------------------------------
//		* CreateUDPEndpoint
// ---------------------------------------------------------------------------
//	Create the best endpoint object for TCP/IP connections given
//	the available system software.

LUDPEndpoint*
UNetworkFactory::CreateUDPEndpoint()
{
	// ensure TCP is available
	ThrowIf_(!HasTCP());

#if USE_OPENTPT
	if (HasOpenTransport()) {
		UOpenTptSupport::StartOpenTransport();
							// Put this here because we must call
							// InitOpenTransport() before OTCreateConfiguration.
		return new LOpenTptUDPEndpoint();
	}
	else
#endif
	{
#if USE_MACTCP
		return new LMacTCPUDPEndpoint();
#else
		return nil;		//This just prevents compile time warning
#endif
	}
}

// ---------------------------------------------------------------------------
//		* CreateInternetMapper
// ---------------------------------------------------------------------------
//	Create the best name mapper object for Internet address
//	resolution given the available system software.

LInternetMapper*
UNetworkFactory::CreateInternetMapper()
{
	// ensure TCP is available
	ThrowIf_(!HasTCP());

#if USE_OPENTPT
	if (HasOpenTransport()) {
		UOpenTptSupport::StartOpenTransport();
	// must call InitOpenTransport() before OTCreateConfiguration
		return new LOpenTptInetMapper();
	}
	else
#endif
	{
#if USE_MACTCP
		return new LMacTCPInetMapper();
#else
		return nil;		//This just prevents compile time warning
#endif
	}
}

// ---------------------------------------------------------------------------
//		* HasTCP
// ---------------------------------------------------------------------------

Boolean
UNetworkFactory::HasTCP()
{
	// at compile time, include _all_ possible tests
#if USE_OPENTPT && USE_MACTCP
	return HasOpenTransport() || HasMacTCP();
#elif USE_OPENTPT
	return HasOpenTransport();
#elif USE_MACTCP
	return HasMacTCP();
#else
	return false;
#endif
}

// ---------------------------------------------------------------------------
//		* HasOpenTransport
// ---------------------------------------------------------------------------

#if USE_OPENTPT
Boolean
UNetworkFactory::HasOpenTransport()
{
	return UOpenTptSupport::HasOpenTransport();
}
#endif

// ---------------------------------------------------------------------------
//		* HasMacTCP
// ---------------------------------------------------------------------------

#if USE_MACTCP
Boolean
UNetworkFactory::HasMacTCP()
{
	return UMacTCPSupport::HasMacTCP();
}
#endif

PP_End_Namespace_PowerPlant
