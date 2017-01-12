// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptTCPEndpoint.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOpenTptTCPEndpoint
#define _H_LOpenTptTCPEndpoint
#pragma once

#include <LTCPEndpoint.h>
#include <OpenTransport.h>
#include <UOpenTptSupport.h>
#include <LNetMessageQueue.h>
#include <LSharedMemoryPool.h>

#if !TARGET_RT_MAC_MACHO
	#include <MacTCP.h>
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LOpenTptTCPEndpoint ===

class LOpenTptNotifHandler;
class LListenCall;

// ===========================================================================
//		¥ LOpenTptTCPEndpoint
// ===========================================================================
//	An implementation of LTCPEndpoint that communicates via OT.
//	We recommend that you do not make a subclass of this class.

class LOpenTptTCPEndpoint :		public LTCPEndpoint,
								public LOpenTptNotifHandler
{

public:
							LOpenTptTCPEndpoint();
	virtual					~LOpenTptTCPEndpoint();

	//Basic Endpoint Operations
	virtual void				Bind(
										LInternetAddress&		inLocalAddress,
										UInt32					inListenQueueSize = 0,
										Boolean					inReusePort = true);
	virtual void				Unbind();
	virtual LInternetAddress*	GetLocalAddress();

	//Basic TCP Endpoint Operations
	virtual void				Connect(
										LInternetAddress&		inRemoteAddress,
										UInt32					inTimeoutSeconds = Timeout_None);
	virtual void				Disconnect();
	virtual void				SendDisconnect();

	virtual void				AbortiveDisconnect();
	virtual void				AcceptRemoteDisconnect();

	virtual LInternetAddress*	GetRemoteHostAddress();

		// passive connections (servers)

	virtual void				Listen();
	virtual void				AcceptIncoming(LTCPEndpoint* 	inEndpoint);
	virtual void				RejectIncoming();

		// sending data

	virtual void				SendData(
										void*					inData,
										UInt32					inDataSize,
										Boolean					inExpedited = false,
										UInt32					inTimeoutSeconds = Timeout_None);

		// receiving data

	virtual void				ReceiveData(
										void*					outDataBuffer,
										UInt32&					ioDataSize,
										Boolean&				outExpedited,
										UInt32					inTimeoutSeconds = Timeout_None);

		// receive configuration

	virtual UInt32				GetAmountUnread();

		// accessors

	virtual EEndpointState		GetState();

	virtual void				AckSends();
	virtual void				DontAckSends();
	virtual Boolean				IsAckingSends();

	inline const TEndpointInfo&	GetEndpointInfo() const
									{
										return mEndpointInfo;
									}
	inline EndpointRef			GetEndpointRef() const
									{
										return mEndpointRef;
									}

	virtual void				Int_HandleAsyncEventSelf(
									OTEventCode		inEventCode,
									OTResult		inResult,
									void*			inCookie);

	virtual void				AbortThreadOperation(LThread * inThread);
protected:

	void					ClearListenEvents();
	void					InternalSetRemoteHostAddress();
	LListenCall *			FindListenCall(OTSequence inSequence);

	EndpointRef				mEndpointRef;				// Open Transport endpoint object
	TEndpointInfo			mEndpointInfo;				// info obtained when endpoint was opened

	LInternetAddress *		mLocalAddress;
	LInternetAddress *		mRemoteAddress;

	LNetMessageQueue *		mMessageQueue;

	LSharedMemoryPool * 	mSharedPool;

	Boolean					mOpenPending;
	OTEventCode				mReceivedDisconnect;

	TInterruptSafeList<LListenCall*>	mListenCalls;
	StOpenTptOperation *	mWaitingDataOperation;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
