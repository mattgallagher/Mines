// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPConnection.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// HTTP Connection

#include <LHTTPConnection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LString.h>

#include <cstdio>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//		¥ LHTTPConnection()
// ---------------------------------------------------------------------------
//	Constructor

LHTTPConnection::LHTTPConnection(LThread& inThread)
	:LInternetProtocol(inThread)
{
	mHTTPState = HTTPClosed;
	mCheckContentLength = true;
}


// ---------------------------------------------------------------------------
//		¥ ~LHTTPConnection
// ---------------------------------------------------------------------------
//	Destructor

LHTTPConnection::~LHTTPConnection()
{
}

// ---------------------------------------------------------------------------
//		¥ Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LHTTPConnection::Connect(
	ConstStr255Param	inRemoteHost,
	SInt16				inRemotePort)
{
	if (mHTTPState == HTTPConnected)
		return;

	LInternetProtocol::Connect(inRemoteHost, (UInt16) inRemotePort);

	mHTTPState = HTTPConnected;
}

// ---------------------------------------------------------------------------
//		¥ Disconnect
// ---------------------------------------------------------------------------

void
LHTTPConnection::Disconnect()
{
	if(mHTTPState != HTTPClosed) {
		LInternetProtocol::Disconnect();
		mHTTPState = HTTPClosed;
	}
}

// ---------------------------------------------------------------------------
//		¥ RequestResource
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::RequestResource(
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		EHTTPMethod			inMethod,
		SInt16				inRemotePort)
{
	DoHTTPMethod(
		inMethod,
		inRemoteHost,
		inRequest,
		inMessage,
		outResponse,
		inRemotePort);
}


// ---------------------------------------------------------------------------
//		¥ Get
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::Get(
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		SInt16				inRemotePort)
{
	DoHTTPMethod(
		HTTPGet,
		inRemoteHost,
		inRequest,
		inMessage,
		outResponse,
		inRemotePort);
}

// ---------------------------------------------------------------------------
//		¥ Head
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::Head(
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		SInt16				inRemotePort)
{
	DoHTTPMethod(
		HTTPHead,
		inRemoteHost,
		inRequest,
		inMessage,
		outResponse,
		inRemotePort);
}

// ---------------------------------------------------------------------------
//		¥ Post
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::Post(
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		SInt16				inRemotePort)
{
	DoHTTPMethod(
		HTTPPost,
		inRemoteHost,
		inRequest,
		inMessage,
		outResponse,
		inRemotePort);
}

// ---------------------------------------------------------------------------
//		¥ Put
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::Put(
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		SInt16				inRemotePort)
{
	DoHTTPMethod(
		HTTPPut,
		inRemoteHost,
		inRequest,
		inMessage,
		outResponse,
		inRemotePort);
}

// ---------------------------------------------------------------------------
//		¥ DoHTTPMethod
// ---------------------------------------------------------------------------
//

void
LHTTPConnection::DoHTTPMethod(
		EHTTPMethod			inMethod,
		ConstStr255Param	inRemoteHost,
		ConstStr255Param	inRequest,
		LHTTPMessage&		inMessage,
		LHTTPResponse&		outResponse,
		SInt16				inRemotePort)
{
	char theMethod[16];
	switch (inMethod) {
		case HTTPGet:
			PP_CSTD::strcpy(theMethod, kHTTPGet);
			break;
		case HTTPHead:
			PP_CSTD::strcpy(theMethod, kHTTPHead);
			break;
		case HTTPPost:
			PP_CSTD::strcpy(theMethod, kHTTPPost);
			break;
		case HTTPPut:
			PP_CSTD::strcpy(theMethod, kHTTPPut);
			break;
		default:
			Throw_(err_AssertFailed);
	}


	ThrowIfNot_(*inRequest);

	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.currentItem = inRemoteHost;
	progress.totalItems = 1;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	BroadcastProgress(msg_OpeningConnection, progress, true);
	Connect(inRemoteHost, inRemotePort);
	BroadcastProgress(msg_Connected, progress, true);

	StPointerBlock commandString(inRequest[0] + 32);	//+32 is for overhead of cmd string
	PP_CSTD::sprintf(commandString, "%s ", theMethod);
	::CopyPascalStringToC(inRequest, commandString + PP_CSTD::strlen(commandString));
	PP_CSTD::sprintf(commandString + PP_CSTD::strlen(commandString), " %s\r\n", kHTTPVersion);

	try {
		progress.currentItem = inRequest;
		SendCommandWithReply(commandString,
							inMessage,
							outResponse,
							progress);

		if (outResponse.GetStatus())
			BroadcastProgress(msg_RetrieveItemSuccess, progress, true);
		else
			BroadcastProgress(msg_RetrieveItemFailed, progress, true);

		Disconnect();
		BroadcastProgress(msg_Disconnected, progress, true);
	} catch (...) {
		try {
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		} catch(...) {
		}
		throw;
	}
}

// ---------------------------------------------------------------------------
//		¥ SendCommandWithReply
// ---------------------------------------------------------------------------

void
LHTTPConnection::SendCommandWithReply(const char * inString,
										LHTTPMessage& inMessage,
										LHTTPResponse& outResponse,
										SProgressMessage&	inProgressMsg)
{
	outResponse.ResetResponse();

	//Build HTTP request
	LDynamicBuffer messageBuffer;
	inMessage.GetMessage(&messageBuffer);
	messageBuffer.PrependBuffer(inString);
	messageBuffer += CRLF;

	SendData(&messageBuffer, inProgressMsg);

	WaitResponse(outResponse, inProgressMsg);
}

// ---------------------------------------------------------------------------
//		¥ WaitResponse
// ---------------------------------------------------------------------------

void
LHTTPConnection::WaitResponse(
							LHTTPResponse& outResponse,
							SProgressMessage&	inProgressMsg)
{
	char			theResponse[kMaxHTTPResponseLen + 1];
	LDynamicBuffer	dataBuffer;
	StPointerBlock	theData((SInt32) mRcvSize);
	Boolean			dataExpedited;
	UInt32			dataSize;

	inProgressMsg.completedBytes = 0;
	inProgressMsg.totalBytes = 0;

	if (mCheckContentLength)
		mCheckedHeader = false;

	try {
		//first grab the response portion
		dataSize = kMaxHTTPResponseLen;
		ThrowIfNot_(mEndpoint->ReceiveLine(theResponse,
											dataSize,
											mTimeoutSeconds,
											true));
		theResponse[dataSize] = '\0';
		outResponse.SetResponse(theResponse);

		LHTTPMessage * returnMessage = outResponse.GetReturnMessage();
		returnMessage->ResetMembers();
		//now loop and get all the data
		while (1) {
			dataSize = mRcvSize;

			mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);

			dataBuffer.ConcatenateBuffer(theData, (SInt32) dataSize);

			inProgressMsg.completedBytes += dataSize;

			returnMessage->SetPartialMessage(&dataBuffer);

			if (mCheckContentLength) {
				if (!mCheckedHeader) {
					//Check only when we have a header
					if (returnMessage->HasCustomHeader()) {
						mCheckedHeader = true;

						LHeaderField contentLengthField;
						if (returnMessage->GetArbitraryField(kFieldContentLength, &contentLengthField))
							inProgressMsg.totalBytes = (UInt32) PP_CSTD::atol(contentLengthField.GetBody());
					}
				}
			}

			BroadcastProgress(msg_ReceivingData, inProgressMsg);
		}
	}

	catch (const LException& err) {
		if (not IsDisconnectError(err.GetErrorCode())) {
			throw;
		}
	}

	catch (ExceptionCode err) {
		if (not IsDisconnectError(err)) {
			throw;
		}
	}

	//Force the last msg_ReceivingData message just in case
	BroadcastProgress(msg_ReceivingData, inProgressMsg, true);
}

PP_End_Namespace_PowerPlant
