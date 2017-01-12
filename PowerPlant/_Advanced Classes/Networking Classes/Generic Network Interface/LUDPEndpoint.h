// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LUDPEndpoint.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LUDPEndpoint
#define _H_LUDPEndpoint
#pragma once

#include <LEndpoint.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LUDPEndpoint												[abstract]
// ===========================================================================
//	An abstract base class for UDP endpoints. UDP is the sessionless
//	protocol of the Internet, and is used by a handful of protocols such
//	as NTP (network time protocol).
//
//	In PowerPlant, there are two subclasses of LUDPEndpoint, named
//	LMacUDPEndpoint and LOpenTptUDPEndpoint. The appropriate class is
//	created automatically when you call UNetworkFactory::CreateUDPEndpoint.

class LUDPEndpoint : public LEndpoint {

public:
								LUDPEndpoint();
	virtual						~LUDPEndpoint();

	// UDP datagram messaging

	virtual void				SendPacketData(
										LInternetAddress&		inRemoteHost,
										void*					inData,
										UInt32					inDataSize) = 0;
	virtual bool				ReceiveFrom(
										LInternetAddress&		outRemoteAddress,
										void*					outDataBuffer,
										UInt32&					ioDataSize,
										UInt32					inTimeoutTicks = Timeout_None) = 0;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
