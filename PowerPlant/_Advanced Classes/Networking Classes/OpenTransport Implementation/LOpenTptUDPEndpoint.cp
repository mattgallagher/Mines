// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptUDPEndpoint.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	OpenTransport version of a UDP endpoint. An instance of this class will
//	normally be created for you by use of UNetworkFactory.

#include <LOpenTptUDPEndpoint.h>
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

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//		¥ LOpenTptUDPEndpoint()
// ---------------------------------------------------------------------------
//	Constructor

LOpenTptUDPEndpoint::LOpenTptUDPEndpoint()
{
	mEndpointRef = nil;
	UOpenTptSupport::StartOpenTransport();

	OSStatus result;

#ifdef DEBUG_OTSESSIONWATCHER

	mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kUDPDebugName),
										0, &mEndpointInfo, &result);
	if (result != 0) {
		mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kUDPName),
											0, &mEndpointInfo, &result);
	}

#else

	mEndpointRef = ::OTOpenEndpoint( ::OTCreateConfiguration(kUDPName),
											0, &mEndpointInfo, &result);

#endif

	ThrowIfError_(result);

	InstallNotifierProc(mEndpointRef);
	::OTSetAsynchronous(mEndpointRef);
	DontAckSends();

	mSharedPool = LSharedMemoryPool::GetSharedPool();
	mSharedPool->AddPoolUser(this);

	StDeleter<LInternetAddress>	localAddress( new LInternetAddress(0,0) );
	StDeleter<LNetMessageQueue>	messageQueue( new LNetMessageQueue(*this) );

	mLocalAddress = localAddress.Release();
	mMessageQueue = messageQueue.Release();

	mWaitingDataOperation = nil;
}

// ---------------------------------------------------------------------------
//		¥ ~LOpenTptUDPEndpoint()
// ---------------------------------------------------------------------------
//	Destructor

LOpenTptUDPEndpoint::~LOpenTptUDPEndpoint()
{
	RemoveNotifierProc();	//do this before closing the provider

	if (mEndpointRef)
		::OTCloseProvider(mEndpointRef);		// ignore errors

	delete mLocalAddress;

	//Need to delete mMessageQueue before removing memory pool!
	delete mMessageQueue;

	//Probably always want to RemovePoolUser last since other objects may
	//	have dependance on the memory pool and the memory pool might
	//	delete itself when we do this.
	mSharedPool->RemovePoolUser(this);
}

// ---------------------------------------------------------------------------
//		¥ AbortThreadOperation
// ---------------------------------------------------------------------------

void
LOpenTptUDPEndpoint::AbortThreadOperation(LThread * inThread)
{
	StOpenTptOperation * theOperation = (StOpenTptOperation*)StAsyncOperation::GetThreadOperation(inThread);
	if (theOperation) {
		theOperation->AbortOperation();
	}
}

// ---------------------------------------------------------------------------
//		¥ GetState
// ---------------------------------------------------------------------------
//	Returns state of endpoint

EEndpointState
LOpenTptUDPEndpoint::GetState()
{
	ResultT result = ::OTGetEndpointState(mEndpointRef);
	if (result == kOTStateChangeErr)
		result = endpoint_UnknownState;

	return (EEndpointState) result;
}

// ===========================================================================

#pragma mark -
#pragma mark === local address configuration

void
LOpenTptUDPEndpoint::Bind(
					LInternetAddress&		inLocalAddress,
					UInt32					/* inListenQueueSize */,
					Boolean					inReusePort)

{
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
	Req.qlen = Ret.qlen = 1;	// UDP ignores listen queue size

	Ret.addr.len = Ret.addr.maxlen = (ByteCount) GetEndpointInfo().addr;
	Ret.addr.buf = (unsigned char*)::NewPtrClear((SInt32) Ret.addr.maxlen);
	ThrowIfMemFail_(Ret.addr.buf);

	StOpenTptOperation	operation(this, T_BINDCOMPLETE, &Ret, true);
	ThrowIfError_(::OTBind(GetEndpointRef(), &Req, &Ret));
	operation.WaitForCompletion();

	delete mLocalAddress;
	mLocalAddress =
			UOpenTptSupport::OTAddressToPPAddress((OTAddress*)Ret.addr.buf);

	::DisposePtr((char*)Req.addr.buf);
	::DisposePtr((char*)Ret.addr.buf);
}

// ---------------------------------------------------------------------------
//		¥ Unbind
// ---------------------------------------------------------------------------

void
LOpenTptUDPEndpoint::Unbind()
{
	StOpenTptOperation	operation(this, T_UNBINDCOMPLETE);
	ThrowIfError_(::OTUnbind(GetEndpointRef()));
	operation.WaitForCompletion();

	mLocalAddress->SetHostPort(0);
}

// ---------------------------------------------------------------------------
//		¥ GetLocalAddress
// ---------------------------------------------------------------------------

LInternetAddress*
LOpenTptUDPEndpoint::GetLocalAddress()
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
#pragma mark === datagram messaging

void
LOpenTptUDPEndpoint::SendPacketData(
					LInternetAddress&		inRemoteHost,
					void*					inData,
					UInt32					inDataSize)
{
	TUnitData data;
	inRemoteHost.MakeOTIPAddress(data.addr);
	data.opt.maxlen = 0;
	data.opt.len = 0;
	data.opt.buf = nil;
	data.udata.maxlen = data.udata.len = inDataSize;
	data.udata.buf = (unsigned char*)inData;

	StOpenTptOperation	operation(this, T_GODATA);

	OSStatus result = ::OTSndUData(mEndpointRef, &data);

	while (result == kOTLookErr) {
		//Clear Error
		::OTRcvUDErr(mEndpointRef, NULL);

		//Try again
		result = ::OTSndUData(mEndpointRef, &data);
	}

	::DisposePtr((Ptr)data.addr.buf);
	if (result == kOTFlowErr) {
		//Wait for T_GODATA message and send data
		operation.WaitForCompletion();
		SendPacketData(inRemoteHost, inData, inDataSize);
	} else {
		ThrowIfError_(result);
	}
}

bool
LOpenTptUDPEndpoint::ReceiveFrom(
					LInternetAddress&		outRemoteAddress,
					void*					outDataBuffer,
					UInt32&					ioDataSize,
					UInt32					inTimeoutSeconds)
{
	if (mWaitingDataOperation)	//Don't setup multiples
		return false;

	if (inTimeoutSeconds == 0)
		inTimeoutSeconds = Timeout_None;

	StOpenTptOperation	operation(this, T_DATA);

	TUnitData data;

	data.opt.maxlen = 0;
	data.opt.len = 0;
	data.opt.buf = nil;

	data.addr.maxlen = sizeof(struct InetAddress);
	data.addr.len = sizeof(struct InetAddress);
	data.addr.buf = (unsigned char*)::NewPtrClear((SInt32) data.addr.maxlen);
	ThrowIfMemFail_(data.addr.buf);

	data.udata.maxlen = data.udata.len = ioDataSize;
	data.udata.buf = (unsigned char*)outDataBuffer;

	OTFlags flags;
	ResultT result = ::OTRcvUData(mEndpointRef, &data, &flags);

	while (result == kOTLookErr) {
		//Clear Error
		::OTRcvUDErr(mEndpointRef, NULL);

		//Try again
		result = ::OTRcvUData(mEndpointRef, &data, &flags);
	}

	if (result) {
		::DisposePtr((char*)data.addr.buf);

		switch (result) {
			case kOTNoDataErr:		//swallow kOTNoDataErr and block thread
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
				return ReceiveFrom(outRemoteAddress, outDataBuffer, ioDataSize, inTimeoutSeconds);
				break;

			default:
				ThrowIfError_(result);
				break;
		}

		Throw_(result);
	}

	//if the call returned an address (it won't if the last one had T_MORE)
	if (data.addr.len) {
		outRemoteAddress.SetIPAddress(((InetAddress*)data.addr.buf)->fHost);
		outRemoteAddress.SetHostPort(((InetAddress*)data.addr.buf)->fPort);
	}

	::DisposePtr((char*)data.addr.buf);
	ioDataSize = data.udata.len;

	return flags == T_MORE;
}

void
LOpenTptUDPEndpoint::ReceiveError(
					UInt32&					outRemoteAddress,
					UInt16&					outRemotePort,
					SInt32&					outError)
{
	TUDErr error;
	InetAddress theResult;

	error.addr.maxlen = sizeof(theResult);
	error.addr.len = sizeof(theResult);
	error.addr.buf = (unsigned char*)&theResult;

	error.opt.maxlen = 0;
	error.opt.len = 0;
	error.opt.buf = nil;

	::OTRcvUDErr(mEndpointRef,&error);

	outRemoteAddress = theResult.fHost;
	outRemotePort = theResult.fPort;
	outError = error.error;
}

// ===========================================================================

#pragma mark -
#pragma mark === acknowledgement of sent data

// ---------------------------------------------------------------------------
//		¥ AckSends
// ---------------------------------------------------------------------------

void
LOpenTptUDPEndpoint::AckSends()
{
	ThrowIfError_(::OTAckSends(GetEndpointRef()));
}

// ---------------------------------------------------------------------------
//		¥ DontAckSends
// ---------------------------------------------------------------------------

void
LOpenTptUDPEndpoint::DontAckSends()
{
	ThrowIfError_(::OTDontAckSends(GetEndpointRef()));
}

// ---------------------------------------------------------------------------
//		¥ IsAckingSends
// ---------------------------------------------------------------------------

Boolean
LOpenTptUDPEndpoint::IsAckingSends()
{
	return ::OTIsAckingSends(GetEndpointRef());
}

// ===========================================================================

#pragma mark -
#pragma mark === implementation details

// ---------------------------------------------------------------------------
//		¥ HandleAsyncEventSelf
// ---------------------------------------------------------------------------
//	Called by NotifyProc to handle notifications for this endpoint.
//	You will be notified via a Broadcast at primary task time of the event if necessary.
//	Warning: This routine will probably be called at interrupt time.

void
LOpenTptUDPEndpoint::Int_HandleAsyncEventSelf(
				OTEventCode		inEventCode,
				OTResult		inResult,
				void*			/* inCookie */)
{
	switch (inEventCode) {
		case T_DATA:
			mMessageQueue->Append(new(*mSharedPool) LDataArrived((MessageT) inEventCode, noErr, nil, 0, nil, false, this));
			if (mWaitingDataOperation)
				mWaitingDataOperation->Int_AsyncResume(kOTNoError);
			break;

		case T_UDERR: {
			UInt32		remoteAddress;
			UInt16		remotePort;
			SInt32		error;

			ReceiveError(remoteAddress, remotePort, error);

			mMessageQueue->Append(new(*mSharedPool) LICMPMessage(
															(MessageT) inEventCode,
															inResult,
															error,
															remoteAddress,
															remotePort,
															this));
			}
			break;


		default:
			mMessageQueue->Append(new(*(mSharedPool)) LNetMessage((MessageT) inEventCode, inResult, this));
			break;
	}
}

PP_End_Namespace_PowerPlant
