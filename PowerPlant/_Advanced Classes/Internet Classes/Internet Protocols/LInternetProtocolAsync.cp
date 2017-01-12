// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetProtocolAsync.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Base class for handling asynchronous style Internet protocols using TCP/IP.

#include <LInternetProtocolAsync.h>
#include <LInternetAddress.h>
#include <UNetworkMessages.h>
#include <UNetworkFactory.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LInternetProtocolAsync ===

// ---------------------------------------------------------------------------
//		¥ LInternetProtocolAsync()
// ---------------------------------------------------------------------------
//	Constructor

LInternetProtocolAsync::LInternetProtocolAsync()
{
	mEndpointThread = nil;
	mSendSize = kDefaultAysncSendSize;
	mRcvSize = kDefaultAysncRcvSize;
	mTimeoutSeconds = Timeout_None;
	*mRemoteHost = '\0';
	mRemotePort = 0;
	mLocalAddress = nil;
}


// ---------------------------------------------------------------------------
//		¥ ~LInternetProtocolAsync
// ---------------------------------------------------------------------------
//	Destructor

LInternetProtocolAsync::~LInternetProtocolAsync()
{
	if (mEndpointThread) {
		Disconnect();

		while (mEndpointThread)
			LThread::Yield();
	}

	delete mLocalAddress;
}

LInternetAddress *
LInternetProtocolAsync::GetLocalAddress()
{
	if (!mLocalAddress) {
		LInternetMapper* mapper = UNetworkFactory::CreateInternetMapper();
		mLocalAddress = mapper->GetLocalAddress();
		delete mapper;
	}

	return new LInternetAddress(*mLocalAddress);
}


// ---------------------------------------------------------------------------
//		¥ ThreadDied
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::ThreadDied()
{
	mEndpointThread = nil;
}


// ---------------------------------------------------------------------------
//		¥ Connect
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::Connect(
	ConstStringPtr		inRemoteHost,
	UInt16				inRemotePort)
{
	LString::CopyPStr(inRemoteHost, (unsigned char *)mRemoteHost);
	LString::PToCStr((unsigned char *)mRemoteHost);

	mRemotePort = inRemotePort;

	mEndpointThread = new LAsyncProtocolThread(this, inRemoteHost, inRemotePort);
	mEndpointThread->Resume();
}


// ---------------------------------------------------------------------------
//		¥ Disconnect
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::Disconnect()
{
	if (mEndpointThread)
		mEndpointThread->KillThread();
}


// ---------------------------------------------------------------------------
//		¥ SendData
// ---------------------------------------------------------------------------
// The data you pass here is copied and placed into a temporary queue waiting
//	to be sent via the mEndpointThread. You will want to watch your available
//	memory when attempting to send large amounts of data.
//
//	IMPORTANT: When sending large amounts of data you may want to break it
//	up into smaller segments and use the HandleDataSent() method to chain
//	your sends.
//
// Returns a reference number to data block passed...
//	this will later be returned in HandleDataSent as an indication that this data
//	was sent. This is mostly for situations where you may want to track progress
//	of data being sent on this type of connection.


UInt32
LInternetProtocolAsync::SendData(
	char*				inData,
	UInt32				totalSize)
{
	if (mEndpointThread)
		return mEndpointThread->QueueOutgoingData(inData, totalSize);
	else
		return 0;
}


// ---------------------------------------------------------------------------
//		¥ Abort
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::Abort()
{
	if (mEndpointThread)
		mEndpointThread->Abort();
}


// ---------------------------------------------------------------------------
//		¥ IsIdle
// ---------------------------------------------------------------------------

Boolean
LInternetProtocolAsync::IsIdle()
{
	if (mEndpointThread == nil)
		return true;

	return mEndpointThread->IsIdle();
}


// ---------------------------------------------------------------------------
//		¥ HandleConnect
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::HandleConnect()
{
}


// ---------------------------------------------------------------------------
//		¥ HandleDisconnect
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::HandleDisconnect()
{
}


// ---------------------------------------------------------------------------
//		¥ HandleDataSent
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::HandleDataSent(UInt32 /* inReference */)
{
}


// ---------------------------------------------------------------------------
//		¥ HandleIncomingData
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::HandleIncomingData(const char * /* theData */, UInt32 /* theLength */)
{
}


// ---------------------------------------------------------------------------
//		¥ HandleListen
// ---------------------------------------------------------------------------

void
LInternetProtocolAsync::HandleListen()
{
}

// ===========================================================================

#pragma mark -
#pragma mark === LAsyncProtocolThread ===

LAsyncProtocolThread::LAsyncProtocolThread (
						LInternetProtocolAsync * inSuperConnection,
						ConstStringPtr		inRemoteHost,
						UInt16				inRemotePort)
		:LThread(false, thread_DefaultStack, threadOption_Default, nil),
			mSuperConnection(inSuperConnection)
{
	mRemoteAddress = new LInternetAddress(inRemoteHost, inRemotePort, false);
	mEndpoint = nil;
	mDataWaiting = false;
}

LAsyncProtocolThread::~LAsyncProtocolThread()
{
	if (mEndpoint) {
		mEndpoint->RemoveListener(this);
		delete mEndpoint;
		mEndpoint = nil;
	}

	delete mRemoteAddress;

	mSuperConnection->ThreadDied();
}


// ---------------------------------------------------------------------------
//		¥ IsIdle
// ---------------------------------------------------------------------------

Boolean
LAsyncProtocolThread::IsIdle()
{
	if (mEndpoint == nil)
		return true;

	EEndpointState netState = mEndpoint->GetState();
	return ((netState == endpoint_Idle) || (netState == endpoint_Unbound));
}


// ---------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------
// These will be events from the endpoint

void
LAsyncProtocolThread::ListenToMessage(MessageT inMessage, void* /* ioParam */)
{
	switch (inMessage) {
		case T_DISCONNECT:
		case T_ORDREL:
			KillThread();
			break;

		case T_DATA:
			DataWaiting();
			break;

		case T_LISTEN:
			HandleListen();
			break;
	}
}


// ---------------------------------------------------------------------------
//		¥ DataWaiting
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::DataWaiting()
{
	mDataWaiting = true;
	if (mState == threadState_Suspended)
		Resume();
}


// ---------------------------------------------------------------------------
//		¥ Abort
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::Abort()
{
	if (mEndpoint)
		mEndpoint->AbortThreadOperation(this);
}


// ---------------------------------------------------------------------------
//		¥ KillThread
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::KillThread()
{
	mContinue = false;
	if (mState == threadState_Suspended) {
		Resume();
	} else {
		Abort();
	}
}

// ---------------------------------------------------------------------------
//		¥ QueueOutgoingData
// ---------------------------------------------------------------------------

UInt32
LAsyncProtocolThread::QueueOutgoingData(char * inData, UInt32 inLength)
{
	const LDynamicBuffer * newBuff = new LDynamicBuffer(inData, (SInt32) inLength);
	mDataQueue.Enqueue(&newBuff);
	if (mState == threadState_Suspended) {
		Resume();
	}
	return (UInt32)newBuff;
}


// ---------------------------------------------------------------------------
//		¥ SendData
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::SendData(LDynamicBuffer * dataBuff)
{
	LStream * dataStream = dataBuff->GetBufferStream();
	dataStream->SetMarker(0, streamFrom_Start);

	SInt32 amountRead;
	SInt32 amountSent = 0;
	SInt32 sendBufferSize = (SInt32)mSuperConnection->GetSendSize();

	do {
		{
			StPointerBlock tempSendBuffer(sendBufferSize, true);
			amountRead = dataStream->ReadData(tempSendBuffer, sendBufferSize);
			if (amountRead > 0) {
				mEndpoint->Send(tempSendBuffer, (UInt32) amountRead);
			}
		}
		amountSent += amountRead;
	} while (amountRead == sendBufferSize);
}


// ---------------------------------------------------------------------------
//		¥ Run
// ---------------------------------------------------------------------------

void *
LAsyncProtocolThread::Run()
{
	StPointerBlock theData((SInt32) (mSuperConnection->GetRcvSize() + 1));
	UInt32 theBufferSize;

	try {
		//Establish the connection
		InternalConnect();
		HandleConnect();

		//Loop sending/receiving data
		LDynamicBuffer * dataBuff;
		mContinue = true;
		while (mContinue) {
			try {
				// Receive first to keep data from building up on the endpoint
				if (mDataWaiting) {
					theBufferSize = mSuperConnection->GetRcvSize();
					mEndpoint->Receive(theData, theBufferSize);
					HandleIncomingData(theData, theBufferSize);
					mDataWaiting = (mEndpoint->GetAmountUnread() > 0);
				}

				else if (mDataQueue.Dequeue(&dataBuff)) {
					SendData(dataBuff);
					delete dataBuff;
					HandleDataSent((UInt32)dataBuff);
				}

				else {
					Suspend();	// Suspend thread until there is something to do
					continue;
				}

				LThread::Yield();
			}

			catch (const LException& inErr) {
				if (inErr.GetErrorCode() == Disconnect_Error) {
					break;
				}
				else if (inErr.GetErrorCode() == OrderlyDisconnect_Error) {
					mEndpoint->AcceptRemoteDisconnect();
					break;
				}
				else if (inErr.GetErrorCode() == Timeout_Error) {
					continue;
				}
				else if (inErr.GetErrorCode() == Abort_Error) {
					// continue on Abort... If mContinue is false then we will
					//	bailout of the loop.
					continue;
				}
				else {
					break;
				}
			}

			catch(ExceptionCode inErr) {
				if (inErr == Disconnect_Error) {
					break;
				}
				else if (inErr == OrderlyDisconnect_Error) {
					mEndpoint->AcceptRemoteDisconnect();
					break;
				}
				else if (inErr == Timeout_Error) {
					continue;
				}
				else if (inErr == Abort_Error) {
					//continue on Abort... If mContinue is false then we will
					//	bailout of the loop.
					continue;
				}
				else {
					break;
				}
			}
		}
	}

	catch (const LException& inErr) {
		if (inErr.GetErrorCode() == OrderlyDisconnect_Error) {
			mEndpoint->AcceptRemoteDisconnect();
		}
	}

	catch(ExceptionCode inErr) {
		if (inErr == OrderlyDisconnect_Error) {
			mEndpoint->AcceptRemoteDisconnect();
		}
	}

	try {
		InternalDisconnect();
	} catch(...) {
	}

	HandleDisconnect();

	return nil;
}

// ---------------------------------------------------------------------------
//		¥ InternalConnect
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::InternalConnect()
{
	mEndpoint = UNetworkFactory::CreateTCPEndpoint();
	mEndpoint->AddListener(this);

	LInternetAddress localAddress(0, 0);
	mEndpoint->Bind(localAddress);

	mEndpoint->Connect(*mRemoteAddress, mSuperConnection->GetTimeoutSeconds());
}

// ---------------------------------------------------------------------------
//		¥ Disconnect
// ---------------------------------------------------------------------------

void
LAsyncProtocolThread::InternalDisconnect()
{
	if (mEndpoint) {
		mEndpoint->Disconnect();
	}
}


// ---------------------------------------------------------------------------
//		¥ Utility Wrappers
// ---------------------------------------------------------------------------
//	These routines just wrap try/catch to prevent exceptions from throwing
//	back into our thread

void
LAsyncProtocolThread::HandleConnect()
{
	try {
		mSuperConnection->HandleConnect();
	} catch(...) {
	}
}

void
LAsyncProtocolThread::HandleDisconnect()
{
	try {
		mSuperConnection->HandleDisconnect();
	} catch(...) {
	}
}

void
LAsyncProtocolThread::HandleDataSent(UInt32 inReference)
{
	try {
		mSuperConnection->HandleDataSent(inReference);
	} catch(...) {
	}
}

void
LAsyncProtocolThread::HandleIncomingData(const char * theData, UInt32 theLength)
{
	try {
		mSuperConnection->HandleIncomingData(theData, theLength);
	} catch(...) {
	}
}

void
LAsyncProtocolThread::HandleListen()
{
	try {
		mSuperConnection->HandleListen();
	} catch(...) {
	}
}

PP_End_Namespace_PowerPlant
