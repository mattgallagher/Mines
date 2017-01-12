// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptUDPEndpoint.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOpenTptUDPEndpoint
#define _H_LOpenTptUDPEndpoint
#pragma once

#include <LUDPEndpoint.h>
#include <OpenTransport.h>
#include <UOpenTptSupport.h>
#include <LNetMessageQueue.h>
#include <LSharedMemoryPool.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LOpenTptUDPEndpoint
// ===========================================================================
//	An implementation of LTCPEndpoint that communicates with MacTCP.
//	We recommend that you do not make a subclass of this class.

class LOpenTptUDPEndpoint :		public LUDPEndpoint,
								public LOpenTptNotifHandler{

public:
								LOpenTptUDPEndpoint();
								~LOpenTptUDPEndpoint();

	virtual void				Bind(
										LInternetAddress&		inLocalAddress,
										UInt32					inListenQueueSize = 0,
										Boolean					inReusePort = true);

	virtual void				Unbind();
	virtual LInternetAddress*	GetLocalAddress();

	virtual void				SendPacketData(
										LInternetAddress&		inRemoteHost,
										void*					inData,
										UInt32					inDataSize);
	virtual bool				ReceiveFrom(
										LInternetAddress&		outRemoteAddress,
										void*					outDataBuffer,
										UInt32&					ioDataSize,
										UInt32					inTimeoutSeconds = Timeout_None);
	virtual void				ReceiveError(
										UInt32&					outRemoteAddress,
										UInt16&					outRemotePort,
										SInt32&					outError);

	// connection status
	virtual EEndpointState		GetState();

	// acknowledgement of sent data

	virtual void				AckSends();
	virtual void				DontAckSends();
	virtual Boolean				IsAckingSends();

	void						AbortThreadOperation(LThread * inThread);

protected:
	void					Int_HandleAsyncEventSelf(
									OTEventCode		inEventCode,
									OTResult		inResult,
									void*			inCookie);

	inline const TEndpointInfo&	GetEndpointInfo()
									{
										return mEndpointInfo;
									}
	inline const EndpointRef	GetEndpointRef()
									{
										return mEndpointRef;
									}

	EndpointRef				mEndpointRef;				// Open Transport endpoint object
	TEndpointInfo			mEndpointInfo;				// info obtained when endpoint was opened

	LInternetAddress *		mLocalAddress;

	LNetMessageQueue *		mMessageQueue;
	LSharedMemoryPool * 	mSharedPool;

	StOpenTptOperation *	mWaitingDataOperation;
private:

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
