// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetProtocol.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Base class for handling a typical Internet style protocols using TCP/IP.

#include <LInternetProtocol.h>
#include <LInternetAddress.h>
#include <UNetworkMessages.h>
#include <UNetworkFactory.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LInternetProtocol						Constructor
// ---------------------------------------------------------------------------

LInternetProtocol::LInternetProtocol(LThread& inThread)
	: mThread(&inThread)
{
	mEndpoint			= nil;
	mDisconnectReceived = false;
	mStartDisconnect	= false;
	mSendSize			= kDefaultSendSize;
	mRcvSize			= kDefaultRcvSize;
	mProgressLastTicks	= 0;
	mTimeoutSeconds		= Timeout_None;
	mMinBroadcastTicks	= kDefaultMinBroadcastTicks;
}


// ---------------------------------------------------------------------------
//	¥ ~LInternetProtocol					Destructor
// ---------------------------------------------------------------------------

LInternetProtocol::~LInternetProtocol()
{
	ResetMembers();
}


// ---------------------------------------------------------------------------
//	¥ ResetMembers
// ---------------------------------------------------------------------------

void
LInternetProtocol::ResetMembers()
{
	if (mEndpoint) {
		mEndpoint->RemoveListener(this);
		delete mEndpoint;
		mEndpoint = nil;
	}

	mDisconnectReceived = false;
	mStartDisconnect	= false;
	mProgressLastTicks	= 0;
}


// ---------------------------------------------------------------------------
//	¥ Connect
// ---------------------------------------------------------------------------

void
LInternetProtocol::Connect(
	ConstStringPtr		inRemoteHost,
	UInt16				inRemotePort)
{
	ResetMembers();

	mEndpoint = UNetworkFactory::CreateTCPEndpoint();
	mEndpoint->AddListener(this);

	LInternetAddress localAddress(0, 0);
	mEndpoint->Bind(localAddress);

	LInternetAddress remoteAddress(inRemoteHost, inRemotePort, false);
	mDisconnectReceived = false;
	mEndpoint->Connect(remoteAddress, mTimeoutSeconds);
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LInternetProtocol::Disconnect()
{
	if (mEndpoint) {
		mEndpoint->AbortThreadOperation(mThread);
		mEndpoint->Disconnect();
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage
// ---------------------------------------------------------------------------

void
LInternetProtocol::ListenToMessage(MessageT inMessage, void* /* ioParam */)
{
	switch (inMessage) {
		case T_DISCONNECT:
		case T_ORDREL:
			HandleDisconnect();
			break;

		case T_DATA:
		case T_EXDATA:
			HandleIncomingData();
			break;

		case T_LISTEN:
			HandleListen();
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ Abort
// ---------------------------------------------------------------------------

void
LInternetProtocol::Abort()
{
	if (mEndpoint) {
		mEndpoint->AbortThreadOperation(mThread);
	}
}


// ---------------------------------------------------------------------------
//	¥ IsIdle
// ---------------------------------------------------------------------------

Boolean
LInternetProtocol::IsIdle()
{
	if (mEndpoint == nil)
		return true;
	EEndpointState netState = mEndpoint->GetState();
	return ((netState == endpoint_Idle) || (netState == endpoint_Unbound));
}


// ---------------------------------------------------------------------------
//	¥ HandleDisconnect
// ---------------------------------------------------------------------------

void
LInternetProtocol::HandleDisconnect()
{
	mDisconnectReceived = true;
	BroadcastMessage(msg_UnexpectedDisconnect, nil);
}


// ---------------------------------------------------------------------------
//	¥ StartDisconnect
// ---------------------------------------------------------------------------

void
LInternetProtocol::StartDisconnect()
{
	mStartDisconnect = true;
}


// ---------------------------------------------------------------------------
//	¥ HandleIncomingData
// ---------------------------------------------------------------------------

void
LInternetProtocol::HandleIncomingData()
{
}


// ---------------------------------------------------------------------------
//	¥ HandleListen
// ---------------------------------------------------------------------------

void
LInternetProtocol::HandleListen()
{
}


// ---------------------------------------------------------------------------
//	¥ SendData
// ---------------------------------------------------------------------------

void
LInternetProtocol::SendData(
	const char*			inData,
	UInt32				inTotalSize,
	SProgressMessage&	inProgressMsg)
{
	Assert_(mEndpoint != nil);

	if (!inProgressMsg.totalBytes)
		inProgressMsg.totalBytes = inTotalSize;

	UInt32 amountSent = 0;
	const char * p = inData;
	UInt32 sendBufferSize = GetSendSize();

	while (amountSent < inTotalSize) {
		if (sendBufferSize > (inTotalSize - amountSent)) {
			sendBufferSize = inTotalSize - amountSent;
		}

		mEndpoint->Send((void*)(p), sendBufferSize);
		p += sendBufferSize;
		amountSent += sendBufferSize;
		inProgressMsg.completedBytes += sendBufferSize;
		BroadcastProgress(msg_SendingData, inProgressMsg);
	}
}


// ---------------------------------------------------------------------------
//	¥ SendData
// ---------------------------------------------------------------------------

void
LInternetProtocol::SendData(
	LDynamicBuffer *	inBuffer,
	SProgressMessage&	inProgressMsg)
{
	Assert_(mEndpoint != nil);

	if (!inProgressMsg.totalBytes)
		inProgressMsg.totalBytes = (UInt32) inBuffer->GetBufferLength();

	LStream * dataStream = inBuffer->GetBufferStream();
	dataStream->SetMarker(0, streamFrom_Start);

	SInt32 amountRead;
	SInt32 amountSent = 0;
	SInt32 sendBufferSize = (SInt32)GetSendSize();

	do {
		{
			StPointerBlock tempSendBuffer(sendBufferSize, true);
			amountRead = dataStream->ReadData(tempSendBuffer, sendBufferSize);
			if (amountRead > 0) {
				mEndpoint->Send(tempSendBuffer, (UInt32) amountRead);
			}
		}
		amountSent += amountRead;
		inProgressMsg.completedBytes += amountRead;
		BroadcastProgress(msg_SendingData, inProgressMsg);
	} while (amountRead == sendBufferSize);
}


// ---------------------------------------------------------------------------
//	¥ BroadcastProgress
// ---------------------------------------------------------------------------

void
LInternetProtocol::BroadcastProgress(
	MessageT				inMessage,
	SProgressMessage& 		inProgressMsg,
	Boolean					inForceBroadcast)
{
	if ((inForceBroadcast) || ((mProgressLastTicks + mMinBroadcastTicks) < ::TickCount())) {
		BroadcastMessage(inMessage, &inProgressMsg);
		mProgressLastTicks = ::TickCount();
	}
}


PP_End_Namespace_PowerPlant
