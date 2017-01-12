// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptTCPEndpoint.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	OpenTransport version of a TCP endpoint. An instance of this class will
//	normally be created for you by use of UNetworkFactory.

#include <LOpenTptTCPEndpoint.h>
#include <LInternetMapper.h>
#include <UNetworkFactory.h>
#include <UMemoryMgr.h>

#if PP_Target_Carbon && !OTCARBONAPPLICATION

	// You must put
	//		#define OTCARBONAPPLICATION 1
	// in your prefix file or precompiled header
	//
	// Under Carbon, OTOpenEndpoint() takes a context parameter.
	// The context can be NULL for an application, but must be set
	// for a non-application.
	//
	// $$$ CARBON $$$ This class needs to be revised to support contexts.

	#error "Temporary Wart"

#endif


// ---------------------------------------------------------------------------
//	The constant invalidLength is defined in MacTCP.h, which is a header
//	that does not exist for Mach-O builds

#if TARGET_RT_MAC_MACHO
	enum { invalidLength = -23006 };
#endif


PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LOpenTptTCPEndpoint					Constructor
// ---------------------------------------------------------------------------

LOpenTptTCPEndpoint::LOpenTptTCPEndpoint()
{
	mEndpointRef = nil;
	UOpenTptSupport::StartOpenTransport();

	OSStatus result;

#ifdef DEBUG_OTSESSIONWATCHER

	mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kTCPDebugName),
										0, &mEndpointInfo, &result );
	if (result != 0) {
		mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kTCPName),
										0, &mEndpointInfo, &result );
	}

#else

	mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kTCPName),
										0, &mEndpointInfo, &result );

#endif

	ThrowIfError_(result);

	InstallNotifierProc(mEndpointRef);
	::OTSetAsynchronous(mEndpointRef);
	DontAckSends();

	mSharedPool = LSharedMemoryPool::GetSharedPool();
	mSharedPool->AddPoolUser(this);

	StDeleter<LNetMessageQueue>		messageQueue ( new LNetMessageQueue(*this) );
	StDeleter<LInternetAddress>		localAddress ( new LInternetAddress(0,0) );
	StDeleter<LInternetAddress>		removeAddress( new LInternetAddress(0,0) );

	mMessageQueue  = messageQueue.Release();
	mLocalAddress  = localAddress.Release();
	mRemoteAddress = removeAddress.Release();

	mOpenPending = false;

	mReceivedDisconnect = 0;
	mWaitingDataOperation = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LOpenTptTCPEndpoint					Destructor
// ---------------------------------------------------------------------------

LOpenTptTCPEndpoint::~LOpenTptTCPEndpoint()
{
	TInterruptSafeListIterator<LListenCall*> iter(mListenCalls);
	LListenCall* call;

	while (iter.Next(call)) {
		::OTSndDisconnect (mEndpointRef, call->mTCall);
		delete call;
	}

	RemoveNotifierProc();	//do this before closing the provider

	if ( mEndpointRef && UOpenTptSupport::IsOpenTransportInitialized() ) {
		::OTCloseProvider(mEndpointRef);		// ignore errors
	}

	if (mWaitingDataOperation) {
		mWaitingDataOperation->Int_AsyncResume(kOTLookErr);
	}

	delete mLocalAddress;
	delete mRemoteAddress;

	//Need to delete mMessageQueue before removing memory pool!
	delete mMessageQueue;

	//Probably always want to RemovePoolUser last since other objects may
	//	have dependance on the memory pool and the memory pool might
	//	delete itself when we do this.
	mSharedPool->RemovePoolUser(this);
}


// ---------------------------------------------------------------------------
//	¥ AbortThreadOperation
// ---------------------------------------------------------------------------
// Abort the pending operation on the thread indicated

void
LOpenTptTCPEndpoint::AbortThreadOperation(LThread * inThread)
{
	StOpenTptOperation * theOperation = (StOpenTptOperation*)StAsyncOperation::GetThreadOperation(inThread);
	if (theOperation) {
		theOperation->AbortOperation();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetState
// ---------------------------------------------------------------------------
//	Returns state of endpoint

EEndpointState
LOpenTptTCPEndpoint::GetState()
{
	ResultT result = ::OTGetEndpointState(mEndpointRef);
	if (result == kOTStateChangeErr)
		result = endpoint_UnknownState;

	return (EEndpointState) result;
}

// ===========================================================================

#pragma mark -
#pragma mark === local address configuration

// ---------------------------------------------------------------------------
//	¥ Bind
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::Bind(
	LInternetAddress&		inLocalAddress,
	UInt32					inListenQueueSize,
	Boolean					inReusePort)
{
	mReceivedDisconnect = 0;

	if (inReusePort) {
		TOptMgmt    req;
		TOption*	Opt;
		UInt8 		OptionBuf[kOTFourByteOptionSize];
		Opt = (TOption*)OptionBuf;
		req.opt.buf = OptionBuf;
		req.opt.len = sizeof(OptionBuf);
		req.opt.maxlen = sizeof(OptionBuf);
		req.flags   = T_NEGOTIATE;

		Opt->level  = INET_IP;
		Opt->name   = kIP_REUSEADDR;
		Opt->len    = kOTFourByteOptionSize;
		*(UInt32*)Opt->value = true;

		StOpenTptOperation	operation(this, T_OPTMGMTCOMPLETE);
		ThrowIfError_(::OTOptionManagement(GetEndpointRef(), &req, &req));
		operation.WaitForCompletion();
	}

	TBind				Req;
	TBind				Ret;

	inLocalAddress.MakeOTIPAddress(Req.addr);
	Req.qlen = Ret.qlen = inListenQueueSize;

	Ret.addr.len = Ret.addr.maxlen = (ByteCount) GetEndpointInfo().addr;
	Ret.addr.buf = (unsigned char*)::NewPtrClear((SInt32) Ret.addr.maxlen);
	ThrowIfMemFail_(Ret.addr.buf);

	try {
		StOpenTptOperation	operation(this, T_BINDCOMPLETE, &Ret, true);
		ThrowIfError_(::OTBind(GetEndpointRef(), &Req, &Ret));
		operation.WaitForCompletion();

		delete mLocalAddress;
		mLocalAddress =
				UOpenTptSupport::OTAddressToPPAddress((OTAddress*)Ret.addr.buf);
	} catch (...) {
		::DisposePtr((char*)Req.addr.buf);
		::DisposePtr((char*)Ret.addr.buf);
		throw;
	}

	::DisposePtr((char*)Req.addr.buf);
	::DisposePtr((char*)Ret.addr.buf);

}


// ---------------------------------------------------------------------------
//	¥ Unbind
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::Unbind()
{
	StOpenTptOperation	operation(this, T_UNBINDCOMPLETE);
	ThrowIfError_(::OTUnbind(GetEndpointRef()));
	operation.WaitForCompletion();

	mLocalAddress->SetHostPort(0);
}


// ---------------------------------------------------------------------------
//	¥ GetLocalAddress
// ---------------------------------------------------------------------------

LInternetAddress*
LOpenTptTCPEndpoint::GetLocalAddress()
{
	if (mLocalAddress->GetIPAddress() == 0) {
		LInternetMapper * theMapper = UNetworkFactory::CreateInternetMapper();
		LInternetAddress* tempAddress = theMapper->GetLocalAddress();
		mLocalAddress->SetIPAddress(tempAddress->GetIPAddress());
		delete theMapper;
		delete tempAddress;
	}

	return new LInternetAddress(*mLocalAddress);
}

// ===========================================================================

#pragma mark -
#pragma mark === connection establishment

// ---------------------------------------------------------------------------
//	¥ Connect
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::Connect(
		LInternetAddress&		inRemoteAddress,
		UInt32					inTimeoutSeconds)
{
	TCall					SndCall;

	if (inTimeoutSeconds == 0)
		inTimeoutSeconds = Timeout_None;

	inRemoteAddress.MakeOTIPAddress(SndCall.addr);
	SndCall.udata.buf = SndCall.opt.buf = nil;
	SndCall.udata.len = SndCall.opt.len = 0;
	SndCall.udata.maxlen = SndCall.opt.maxlen = 0;
	SndCall.sequence = 0;

	mOpenPending = true;
	StOpenTptOperation	operation(this, T_CONNECT);
	OSStatus result = ::OTConnect(GetEndpointRef(), &SndCall, nil);
	if (result != kOTNoDataErr)
		Throw_(result);
	try {
		operation.WaitForCompletion(inTimeoutSeconds);
	} catch(...) {
	}
	mOpenPending = false;

	switch (operation.GetResultCode()) {
		case kOTCanceledErr:
			::OTRcvDisconnect(GetEndpointRef(), nil);
			::DisposePtr((char*)SndCall.addr.buf);
			Throw_(operation.GetResultCode());
			break;

		case kOTNoError:
			OSStatus result;

			do {
				result = ::OTRcvConnect(GetEndpointRef(), &SndCall);
				if (result == kOTLookErr) {
					::OTLook(GetEndpointRef());
					::OTRcvDisconnect(GetEndpointRef(), nil);
				}
			} while (result == kOTLookErr);

			if (result == kOTNoError) {
				delete mRemoteAddress;
				mRemoteAddress = UOpenTptSupport::OTAddressToPPAddress(
								(OTAddress*)SndCall.addr.buf);
			}

			::DisposePtr((char*)SndCall.addr.buf);
			ThrowIfError_(result);
			break;

		default:
			::DisposePtr((char*)SndCall.addr.buf);
			Throw_(operation.GetResultCode());
	}
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::Disconnect()
{
	if (mReceivedDisconnect) {
		AcceptRemoteDisconnect();
		if (GetState() == static_cast<EEndpointState>(T_INREL))
			ThrowIfError_(::OTSndOrderlyDisconnect(GetEndpointRef()));
	} else {
		AbortiveDisconnect();
		//NOTE: If we didn't do AbortiveDisconnect we'd have to read
		//		any pending data to clear the endpoint first.
	}
}


// ---------------------------------------------------------------------------
//	¥ SendDisconnect
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::SendDisconnect()
{
	if (mReceivedDisconnect) {
		AcceptRemoteDisconnect();
		if (GetState() != static_cast<EEndpointState>(T_INREL))
			return;
	}

	ThrowIfError_(::OTSndOrderlyDisconnect(GetEndpointRef()));
}


// ---------------------------------------------------------------------------
//	¥ AbortiveDisconnect
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::AbortiveDisconnect()
{
	TCall Call;
	
	Call.addr.maxlen	= 0;
	Call.addr.len		= 0;
	Call.addr.buf		= nil;
	
	Call.opt.maxlen		= 0;
	Call.opt.len		= 0;
	Call.opt.buf		= nil;

	Call.udata.maxlen	= 0;
	Call.udata.len		= 0;
	Call.udata.buf		= nil;
	
	Call.sequence		= 0;

	StOpenTptOperation	operation(this, T_DISCONNECTCOMPLETE);
	OSStatus result = ::OTSndDisconnect(GetEndpointRef(), &Call);

	if (result == kOTNoError) {
		operation.WaitForCompletion();

	} else if (result == kOTLookErr) {
		switch (::OTLook(mEndpointRef)) {
			case T_DISCONNECT:
			case T_DISCONNECTCOMPLETE:
				result = ::OTRcvDisconnect(GetEndpointRef(), nil);
			default:
				break;
		}
		ThrowIfError_(result);

	} else {
		ThrowIfError_(result);
	}
}


// ---------------------------------------------------------------------------
//	¥ AcceptRemoteDisconnect
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::AcceptRemoteDisconnect()
{
	//If we are already in T_INREL then just return
	if (GetState() == static_cast<EEndpointState>(T_INREL))
		return;

	switch (mReceivedDisconnect) {
		case T_ORDREL:
			ThrowIfError_(::OTRcvOrderlyDisconnect(GetEndpointRef()));
			break;

		case T_DISCONNECT:
			ThrowIfError_(::OTRcvDisconnect(GetEndpointRef(), nil));
			break;

		default:
			Throw_(kOTOutStateErr);
	}
}

// ---------------------------------------------------------------------------
//	¥ GetRemoteHostAddress
// ---------------------------------------------------------------------------
// PFV 5/2/99 - Change suggested by Richard Atwell...
//		In the case of a "Listening" (a.k.a Server) Endpoint, a call to
//		GetRemoteHostAddress will now return the address of the next inbound
//		connection in the recieve list. This allows you to do a acceptance
//		test based on the address before accepting the connection.

LInternetAddress*
LOpenTptTCPEndpoint::GetRemoteHostAddress()
{
	TInterruptSafeListIterator<LListenCall*> iter(mListenCalls);
	LListenCall* call;

	if (iter.Next(call)) {
		return new LInternetAddress(call->mRemoteHostAddress);
	} else {
		return new LInternetAddress(*mRemoteAddress);
	}
}


// ---------------------------------------------------------------------------
//	¥ Listen
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::Listen()
{
	ClearListenEvents();
}


// ---------------------------------------------------------------------------
//	¥ AcceptIncoming
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::AcceptIncoming(LTCPEndpoint* 	inEndpoint)
{
	LOpenTptTCPEndpoint * inOTEndpoint = (LOpenTptTCPEndpoint*) inEndpoint;

	ClearListenEvents();

	TInterruptSafeListIterator<LListenCall*> iter(mListenCalls);
	LListenCall* call;
	ThrowIfNot_(iter.Next(call));

	StOpenTptOperation	operation(this, T_ACCEPTCOMPLETE);

	OSStatus result;
	Boolean deleteCall = false;
	do {
		result = ::OTAccept(GetEndpointRef(),
							inOTEndpoint->GetEndpointRef(),
							call->mTCall);

		switch (result) {
			case kOTNoError:
				mListenCalls.Remove(call);
				deleteCall = true;
				break;

			case kOTLookErr:
				OSStatus lookValue = ::OTLook(GetEndpointRef());
				if ((lookValue == T_LISTEN) || (lookValue == T_DISCONNECT))
					ClearListenEvents();
				else
					Throw_(lookValue);
				break;

			case kOTIndOutErr:
				ClearListenEvents();
				result = kOTLookErr;	//just forces us to try again
				break;

			default:
				Throw_(result);
		}
	} while (result == kOTLookErr);

	operation.WaitForCompletion();

	if (deleteCall) {
		delete call;
	}

	inOTEndpoint->InternalSetRemoteHostAddress();
}


// ---------------------------------------------------------------------------
//	¥ RejectIncoming
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::RejectIncoming()
{
	TInterruptSafeListIterator<LListenCall*> iter(mListenCalls);
	LListenCall* call;

	if (iter.Next(call)) {
		::OTSndDisconnect (mEndpointRef, call->mTCall);
		delete call;
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === data transfer

// ---------------------------------------------------------------------------
//	¥ SendData
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::SendData(
		void*					inData,
		UInt32					inDataSize,
		Boolean					inExpedited,
		UInt32					inTimeoutSeconds)
{
	//For consistency with MacTCP side
	if (inDataSize > 65535) {
		Throw_(invalidLength);
		return;
	}

	if (inTimeoutSeconds == 0)
		inTimeoutSeconds = Timeout_None;

	void* 	localDataPtr = inData;
	UInt32	localDataSize = inDataSize;

	OTFlags flags = (UInt32) (inExpedited ? T_EXPEDITED : 0);
	OTEventCode theEvent = inExpedited ? T_GOEXDATA : T_GODATA;

	StOpenTptOperation	operation(this, theEvent);

	OSStatus result = ::OTSnd(GetEndpointRef(), inData, inDataSize, flags);

	if (result == inDataSize) {
		return;
	} else if (result < 0) {
		switch (result)
		{
			case kOTLookErr:
				result = ::OTLook(mEndpointRef);
				switch (result) {
					case T_DISCONNECT:
					case T_DISCONNECTCOMPLETE:
						Throw_(Disconnect_Error);
						break;

					case T_ORDREL:
						Throw_(OrderlyDisconnect_Error);
						break;

					default:
						break;
				}

			case kOTFlowErr:
				//Wait for T_GODATA message and send data
				operation.WaitForCompletion(inTimeoutSeconds);
				break;

			default:
				Throw_(result);
				break;
		}
	} else {
		// Check for async disconnect event.
		if (mReceivedDisconnect) {
			result = ::OTLook(mEndpointRef);

			switch (mReceivedDisconnect) {
				case T_DISCONNECT:
				case T_DISCONNECTCOMPLETE:
					Throw_(Disconnect_Error);
					break;

				case T_ORDREL:
					Throw_(OrderlyDisconnect_Error);
					break;

				default:
					Throw_((ExceptionCode) mReceivedDisconnect);
					break;
			}
		}

		//Wait for T_GODATA message and send rest of data
		localDataPtr = (char*)inData + result;
		localDataSize -= (UInt32) result;
		operation.WaitForCompletion(inTimeoutSeconds);
	}

	SendData(localDataPtr, localDataSize, inExpedited, inTimeoutSeconds);
}


// ---------------------------------------------------------------------------
//	¥ ReceiveData
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::ReceiveData(
		void*					outDataBuffer,
		UInt32&					ioDataSize,
		Boolean&				outExpedited,
		UInt32					inTimeoutSeconds)
{
	if (mWaitingDataOperation)	//Don't setup multiples
	{
		LThread::Yield();
		outExpedited = false;
		ioDataSize = 0;
		return;
	}

	if (inTimeoutSeconds == 0)
		inTimeoutSeconds = Timeout_None;

	StOpenTptOperation	operation(this, T_DATA);

	OTFlags flags;
	OTResult result = ::OTRcv(GetEndpointRef(), outDataBuffer, ioDataSize, &flags);

	if (result >= 0) {
		ioDataSize = (UInt32) result;
		outExpedited = (flags & T_EXPEDITED) != 0;
	} else {
		switch (result) {
			case kOTNoDataErr:		//swallow kOTNoDataErr and block thread
				//ÊBlock waiting for T_DATA
				mWaitingDataOperation = &operation;
				try{
					operation.WaitForCompletion(inTimeoutSeconds);
				} catch(...) {
					mWaitingDataOperation = nil;
					throw;
				}

				//if we are here, we didn't timeout so we must have a
				//		T_DATA event, so we go get the data now
				mWaitingDataOperation = nil;
				ReceiveData(outDataBuffer, ioDataSize, outExpedited, inTimeoutSeconds);
				return;
				break;

			case kOTLookErr:
				result = ::OTLook(mEndpointRef);
				switch (result) {
					case T_DISCONNECT:
					case T_DISCONNECTCOMPLETE:
						Throw_(Disconnect_Error);
						break;

					case T_ORDREL:
						Throw_(OrderlyDisconnect_Error);
						break;

					default:
						break;
				}

			default:
				break;
		}

		ioDataSize = 0;
		outExpedited = false;
		ThrowIfError_(result);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetAmountUnread
// ---------------------------------------------------------------------------

UInt32
LOpenTptTCPEndpoint::GetAmountUnread()
{
	OTByteCount	unreadData;

	OSStatus result = ::OTCountDataBytes(GetEndpointRef(), &unreadData);

	if (result == kOTNoDataErr)
		return 0;

	ThrowIfError_(result);
	return unreadData;
}

// ===========================================================================

#pragma mark -
#pragma mark === acknowledgement of sent data

// ---------------------------------------------------------------------------
//	¥ AckSends
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::AckSends()
{
	ThrowIfError_(::OTAckSends(GetEndpointRef()));
}


// ---------------------------------------------------------------------------
//	¥ DontAckSends
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::DontAckSends()
{
	ThrowIfError_(::OTDontAckSends(GetEndpointRef()));
}


// ---------------------------------------------------------------------------
//	¥ IsAckingSends
// ---------------------------------------------------------------------------

Boolean
LOpenTptTCPEndpoint::IsAckingSends()
{
	return ::OTIsAckingSends(GetEndpointRef());
}

// ===========================================================================

#pragma mark -
#pragma mark === implementation details

// ---------------------------------------------------------------------------
//	¥ InternalSetRemoteHostAddress
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::InternalSetRemoteHostAddress()
{
	TBind remoteAddress;

	remoteAddress.addr.maxlen = (ByteCount) GetEndpointInfo().addr;
	remoteAddress.addr.buf = (unsigned char*)::NewPtrClear((SInt32) remoteAddress.addr.maxlen);
	ThrowIfMemFail_(remoteAddress.addr.buf);

	try {
		StOpenTptOperation	operation(this, T_GETPROTADDRCOMPLETE);
		ThrowIfError_(::OTGetProtAddress(GetEndpointRef(), nil, &remoteAddress));
		operation.WaitForCompletion();

		delete mRemoteAddress;
		mRemoteAddress =
				UOpenTptSupport::OTAddressToPPAddress((OTAddress*)remoteAddress.addr.buf);
	} catch(...) {
	}

	::DisposePtr((char*)remoteAddress.addr.buf);
}


// ---------------------------------------------------------------------------
//	¥ ClearListenEvents
// ---------------------------------------------------------------------------

void
LOpenTptTCPEndpoint::ClearListenEvents()
{
	OSStatus result;
	LListenCall *call = nil;

	try {
		while (true) {
			if (!call) {
				call = new LListenCall();
			}

			result = ::OTListen(mEndpointRef, call->mTCall);

			if (result == kOTNoDataErr)
				break;
			else if (result == kOTLookErr) {
				OSStatus lookEvent = ::OTLook(mEndpointRef);
				if (lookEvent == T_DISCONNECT) {
					TDiscon discon;
					discon.udata.buf = nil;
					discon.udata.len = 0;
					discon.reason = 0;
					discon.sequence = 0;
					ThrowIfError_(::OTRcvDisconnect(mEndpointRef, &discon));
					LListenCall *deadCall = FindListenCall(discon.sequence);
					if (deadCall) {
						delete deadCall;
					}
				} else
					Throw_(lookEvent);
			}
			else if (result == noErr) {
				InetAddress* tempHost = (InetAddress*) call->mTCall->addr.buf;
				call->mRemoteHostAddress = tempHost->fHost;

				mListenCalls.Append(call);
				call = nil;
			}
			else {
				ThrowIfError_(result);
			}
		}
	}

	catch (...) {
		delete call;
		throw;
	}

	delete call;
}


// ---------------------------------------------------------------------------
//	¥ HandleAsyncEventSelf
// ---------------------------------------------------------------------------
//	Called by NotifyProc to handle notifications for this endpoint.
//	You will be notified via a Broadcast at primary task time of the event if necessary.
//	Warning: This routine will probably be called at interrupt time.

void
LOpenTptTCPEndpoint::Int_HandleAsyncEventSelf(
				OTEventCode		inEventCode,
				OTResult		inResult,
				void*			inCookie)
{
	switch (inEventCode) {
		case T_DISCONNECT:
			if (mOpenPending) {
				mOpenPending = false;
				Int_HandleAsyncEvent(T_CONNECT, kOTCanceledErr, inCookie);
			} else {
				mReceivedDisconnect = inEventCode;
				mMessageQueue->Append(new(*mSharedPool) LNetMessage((MessageT) inEventCode, inResult, this));
				if (mWaitingDataOperation)
					mWaitingDataOperation->Int_AsyncResume(Disconnect_Error);
			}
			break;

		case T_DISCONNECTCOMPLETE:
			mMessageQueue->Append(new(*mSharedPool) LNetMessage((MessageT) inEventCode, inResult, this));
			if (mWaitingDataOperation)
				mWaitingDataOperation->Int_AsyncResume(Disconnect_Error);
			break;

		case T_ORDREL:
			mReceivedDisconnect = inEventCode;
			mMessageQueue->Append(new(*mSharedPool) LNetMessage((MessageT) inEventCode, inResult, this));
			if (mWaitingDataOperation)
				mWaitingDataOperation->Int_AsyncResume(OrderlyDisconnect_Error);
			break;

		case T_DATA:
			mMessageQueue->Append(new(*mSharedPool) LDataArrived((MessageT) inEventCode, noErr, nil, 0, nil, false, this));
			if (mWaitingDataOperation)
				mWaitingDataOperation->Int_AsyncResume(kOTNoError);
			break;

		case T_MEMORYRELEASED:
			mMessageQueue->Append(new(*mSharedPool) LMemoryReleasedMsg((MessageT) inEventCode, kOTNoError, inCookie, (UInt32) inResult, this));

		default:
			mMessageQueue->Append(new(*(mSharedPool)) LNetMessage((MessageT) inEventCode, inResult, this));
	}
}


// ---------------------------------------------------------------------------
//	¥ FindListenCall
// ---------------------------------------------------------------------------
//	Utility routine to map a sequence number to it's LListenCall object.

LListenCall *
LOpenTptTCPEndpoint::FindListenCall(OTSequence inSequence)
{
	LListenCall* currCall;
	TInterruptSafeListIterator<LListenCall*> iter(mListenCalls);
	while (iter.Next(currCall)) {
		if (currCall->mTCall->sequence == inSequence) {
			return currCall;
		}
	}

	return nil;
}


PP_End_Namespace_PowerPlant
