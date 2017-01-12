// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFingerConnection.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Finger Connection

#include <LFingerConnection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LArrayIterator.h>

#include <cstdio>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LFingerConnection						Constructor
// ---------------------------------------------------------------------------

LFingerConnection::LFingerConnection(
	LThread&	inThread)
	
	: LInternetProtocol(inThread)
{
	mFingerState = FingerClosed;
}


// ---------------------------------------------------------------------------
//	¥ ~LFingerConnection					Destructor
// ---------------------------------------------------------------------------

LFingerConnection::~LFingerConnection()
{
}


// ---------------------------------------------------------------------------
//	¥ GetOneUser
// ---------------------------------------------------------------------------

void
LFingerConnection::GetOneUser(
	ConstStr255Param	inRemoteHost,
	const char*			inUserRequest,
	LFingerResponse&	outResponse,
	Boolean 			inVerbose,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mFingerState != FingerConnected);

	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	if (ConnectDisconnect) {
		progress.currentItem = inRemoteHost;
		BroadcastProgress(msg_OpeningConnection, progress, true);
		Connect(inRemoteHost, inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		progress.currentItem = inUserRequest;
		BroadcastProgress(msg_ReceivingItem, progress, true);
		InternalRequestUser(inUserRequest, outResponse, progress, inVerbose);
		BroadcastProgress(msg_RetrieveItemSuccess, progress, true);

		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}
	}
	
	catch (...) {
		BroadcastProgress(msg_RetrieveItemFailed, progress, true);

		try {
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		}
		
		catch(...) { }

		throw; // rethrow the error encountered here
	}

}


// ---------------------------------------------------------------------------
//	¥ GetUsers
// ---------------------------------------------------------------------------

void
LFingerConnection::GetUsers(
	ConstStr255Param	inRemoteHost,
	LFingerResponse&	outResponse,
	Boolean 			inVerbose,
	SInt16				inRemotePort)
{
	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	Boolean ConnectDisconnect = (mFingerState != FingerConnected);

	if (ConnectDisconnect) {
		progress.currentItem = inRemoteHost;
		BroadcastProgress(msg_OpeningConnection, progress, true);
		Connect(inRemoteHost, inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		progress.currentItem = inRemoteHost;
		BroadcastProgress(msg_ReceivingItem, progress, true);
		InternalRequestAllUsers(outResponse,  progress, inVerbose);
		BroadcastProgress(msg_RetrieveItemSuccess, progress, true);

		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}
	} catch (...) {
		BroadcastProgress(msg_RetrieveItemFailed, progress, true);

		try {
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		} catch(...) {
		}

		throw; //rethrow the error encountered here
	}
}


// ---------------------------------------------------------------------------
//	¥ Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LFingerConnection::Connect(
	ConstStr255Param	inRemoteHost,
	SInt16				inRemotePort)
{
	if (mFingerState == FingerConnected)
		return;

	LInternetProtocol::Connect(inRemoteHost, (UInt16) inRemotePort);

	mFingerState = FingerConnected;
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LFingerConnection::Disconnect()
{
	if (mFingerState == FingerConnected) {
		LInternetProtocol::Disconnect();
	}
}


// ---------------------------------------------------------------------------
//	¥ InternalRequestAllUsers
// ---------------------------------------------------------------------------

void
LFingerConnection::InternalRequestAllUsers(
	LFingerResponse&	outResponse,
	SProgressMessage&	inProgressMsg,
	Boolean 			inVerbose)
{
	char request[kSingleRequestOverhead + 1];
	*request = '\0';

	if (inVerbose) {
		PP_CSTD::strcat(request, kFingerVerboseFlag);
	}

	PP_CSTD::strcat(request, CRLF);

	SendCommandWithReply(request, outResponse, inProgressMsg);
}


// ---------------------------------------------------------------------------
//	¥ InternalRequestUser
// ---------------------------------------------------------------------------

void
LFingerConnection::InternalRequestUser(
	const char*		inUserRequest,
	LFingerResponse&	outResponse,
	SProgressMessage&	inProgressMsg,
	Boolean 			inVerbose)
{
	StPointerBlock theData((SInt32) (PP_CSTD::strlen(inUserRequest) + kSingleRequestOverhead + 1));
	char * request = theData;
	*request = '\0';

	if (inVerbose) {
		PP_CSTD::strcat(request, kFingerVerboseFlag);
	}

	PP_CSTD::strcat(request, inUserRequest);
	PP_CSTD::strcat(request, CRLF);

	SendCommandWithReply(request, outResponse, inProgressMsg);
}


// ---------------------------------------------------------------------------
//	¥ SendCommandWithReply
// ---------------------------------------------------------------------------

void
LFingerConnection::SendCommandWithReply(
	char* 				inString,
	LFingerResponse&	outResponse,
	SProgressMessage&	inProgressMsg)
{
	outResponse.ResetResponse();
	SendCmd(inString);
	WaitResponse(outResponse, inProgressMsg);
}


// ---------------------------------------------------------------------------
//	¥ SendCmd
// ---------------------------------------------------------------------------

void
LFingerConnection::SendCmd(
	char*	inString)
{
	StPointerBlock tempBuffer((SInt32) (PP_CSTD::strlen(inString) + 3), true); // +3 is space for CRLF and NULL term
	PP_CSTD::strcpy(tempBuffer, inString);
	PP_CSTD::strcat(tempBuffer, CRLF);

	// Clear the receive buffer (just in case) before sending
	// the next command so we don't get confused about the responses.
	UInt32 unreadDataSize = mEndpoint->GetAmountUnread();
	if (unreadDataSize) {
		StPointerBlock theData((SInt32) unreadDataSize);
		mEndpoint->Receive(tempBuffer, unreadDataSize);
	}

	mEndpoint->Send(tempBuffer, PP_CSTD::strlen(tempBuffer));
}


// ---------------------------------------------------------------------------
//	¥ WaitResponse
// ---------------------------------------------------------------------------

void
LFingerConnection::WaitResponse(
	LFingerResponse&	outResponse,
	SProgressMessage&	inProgressMsg)
{
	PP_STD::string dataBuffer;
	UInt32 dataSize;
	StPointerBlock theData((SInt32) mRcvSize);
	Boolean dataExpedited;

	while(1) {
		try {
			dataSize = mRcvSize;
			mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);
			dataBuffer.append(theData, dataSize);
		}

		catch (const LException& err) {
			if (IsDisconnectError(err.GetErrorCode())) {
				break;
			} else {
				throw;
			}
		}

		catch (ExceptionCode err) {
			if (IsDisconnectError(err)) {
				break;
			} else {
				throw;
			}
		}
	}

	// Force the last msg_ReceivingData message just in case
	BroadcastProgress(msg_ReceivingData, inProgressMsg, true);
	outResponse.SetResponse(dataBuffer.c_str());
}


PP_End_Namespace_PowerPlant
