// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEndpoint.h					PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LEndpoint
#define _H_LEndpoint
#pragma once

#include <LBroadcaster.h>
#include <UNetworking.h>
#include <LCleanupTask.h>
#include <LInternetAddress.h>
#include <PP_Networking.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LEndpoint												[abstract]
// ===========================================================================
//	Encapsulates the idea of a "network endpoint," or one side of a two-way
//	communication link. The endpoint is the object that is used to send or
//	receive data over the network.

class LEndpoint :	public LBroadcaster {

public:
								LEndpoint();
	virtual						~LEndpoint();

	// local address configuration

	virtual void				Bind(
										LInternetAddress&		inLocalAddress,
										UInt32					inListenQueueSize = 0,
										Boolean					inReusePort = true) = 0;
	virtual void				Unbind() = 0;
	virtual LInternetAddress*	GetLocalAddress() = 0;

	// connection status

	virtual EEndpointState		GetState() = 0;

	// acknowledgement of sent data

	virtual void				AckSends() = 0;
	virtual void				DontAckSends() = 0;
	virtual Boolean				IsAckingSends() = 0;

	virtual void				QueueSends()
									{ mQueueSends = true; }

	virtual void				DontQueueSends()
									{ mQueueSends = false; }

	virtual Boolean				IsQueuingSends()
									{ return mQueueSends; }

	virtual void				AbortThreadOperation(LThread * inThread) = 0;

protected:
	Boolean						mQueueSends;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
