// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSMTPConnection.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// SMTP Connection

#include <LSMTPConnection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LArrayIterator.h>

#include <cstdio>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LSMTPConnection ===

// ---------------------------------------------------------------------------
//		¥ LSMTPConnection()
// ---------------------------------------------------------------------------
//	Constructor

LSMTPConnection::LSMTPConnection(LThread& inThread)
	:LInternetProtocol(inThread)
{
	mSMTPState = SMTPClosed;
}


// ---------------------------------------------------------------------------
//		¥ ~LSMTPConnection
// ---------------------------------------------------------------------------
//	Destructor

LSMTPConnection::~LSMTPConnection()
{
}


// ---------------------------------------------------------------------------
//		¥ SendOneMessage
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendOneMessage(
	ConstStr255Param	inRemoteHost,
	LMailMessage&		theMessage,
	SInt16				inRemotePort )
{
	Boolean ConnectDisconnect = (mSMTPState != SMTPConnected);

	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	// Set up the connection if necessary.
	if (ConnectDisconnect) {
		progress.currentItem = inRemoteHost;
		BroadcastProgress(msg_OpeningConnection, progress, true);
		Connect(inRemoteHost, inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	// Send the message.
	try {
		progress.totalItems = 1;
		progress.currentItem = theMessage.GetSubject();
		BroadcastProgress(msg_SendingItem, progress, true);
		InternalSendMessage(theMessage, progress);
		progress.completedItems++;
		BroadcastProgress(msg_SendItemSuccess, progress, true);

		// Tear down the connection if necessary.
		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}
	}
	catch (...) {
		BroadcastProgress(msg_SendItemFailed, progress, true);

		try {
			// Tear down the connection if necessary.
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		} catch(...) {
		}

		throw; //rethrow the error encountered here... might be a LSMTPResponse object
	}
}

// ---------------------------------------------------------------------------
//		¥ SendMessages
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendMessages(
	ConstStr255Param	inRemoteHost,
	LInternetMessageList *	inMessageList,
	SInt16				inRemotePort )
{
	ThrowIfNil_(inMessageList);

	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	Boolean ConnectDisconnect = (mSMTPState != SMTPConnected);

	// Set up the connection if necessary.
	if (ConnectDisconnect) {
		progress.currentItem = inRemoteHost;
		BroadcastProgress(msg_OpeningConnection, progress, true);
		Connect(inRemoteHost, inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	progress.totalItems = inMessageList->GetCount();

	try {
		// Send the messages.
		LMailMessage *	currMsg;
		LArrayIterator	iter(*inMessageList);
		while(iter.Next(&currMsg)) {
			progress.currentItem = currMsg->GetSubject();
			BroadcastProgress(msg_SendingItem, progress, true);
			InternalSendMessage(*currMsg, progress);
			progress.completedItems++;
			BroadcastProgress(msg_SendItemSuccess, progress, true);
		}

		// Tear down the connection if necessary.
		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}

	}
	catch (...) {
		try {
			// Tear down the connection if necessary.
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		} catch(...) {
		}

		throw;	//rethrow the error encountered here... might be a LSMTPResponse object
	}
}


// ---------------------------------------------------------------------------
//		¥ Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LSMTPConnection::Connect(
	ConstStr255Param	inRemoteHost,
	SInt16				inRemotePort)
{
	if (mSMTPState == SMTPConnected)
		return;

	LInternetProtocol::Connect(inRemoteHost, (UInt16) inRemotePort);

	WaitForServerID();

	SendHELO();

	mSMTPState = SMTPConnected;
}

// ---------------------------------------------------------------------------
//		¥ Disconnect
// ---------------------------------------------------------------------------

void
LSMTPConnection::Disconnect()
{
	if (mSMTPState != SMTPClosed) {
		SendQUIT();
		LInternetProtocol::Disconnect();
	}
}

// ---------------------------------------------------------------------------
//		¥ InternalSendMessage
// ---------------------------------------------------------------------------

void
LSMTPConnection::InternalSendMessage(LMailMessage& inMessage, SProgressMessage&	inProgressMsg)
{
	ThrowIfNot_(mSMTPState == SMTPConnected);

	SendRSET();

	SendMAIL(inMessage);

	SendRCPT(inMessage.GetTo());
	SendRCPT(inMessage.GetCC());
	SendRCPT(inMessage.GetBCC());

	SendDATACmd();

	SendMessageData(inMessage, inProgressMsg);
}

// ---------------------------------------------------------------------------
//		¥ WaitForServerID
// ---------------------------------------------------------------------------

void
LSMTPConnection::WaitForServerID()
{
	LSMTPResponse	cmdResponse;

	//Check connection state
	WaitResponse(cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPServReady)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendHELO
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendHELO()
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	LInternetAddress* myAddress = mEndpoint->GetLocalAddress();
	Str255 myAddressString;
	myAddress->GetIPAddress(myAddressString);
	delete myAddress;

	PP_CSTD::sprintf(commandString, "%s ", kSMTPHelo);
	::CopyPascalStringToC(myAddressString, commandString + PP_CSTD::strlen(commandString));
	SendCommandWithReply(commandString, cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPRequestOK)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendRSET
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendRSET()
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	//RSET... generally a good idea before each transfer
	PP_CSTD::sprintf(commandString, "%s", kSMTPReset);
	SendCommandWithReply(commandString, cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPRequestOK)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendMAIL
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendMAIL(LMailMessage& inMessage)
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	//MAIL... initiate mail sequence
	PP_CSTD::sprintf(commandString, "%s:<%s>", kSMTPMail, inMessage.GetFrom());
	SendCommandWithReply(commandString, cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPRequestOK)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendRCPT
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendRCPT(LArray * theList)
{
	if (!theList)
		return;

	LArrayIterator iter(*theList);
	char currListItem[255];
	LStr255 cleanAddress;
	while(iter.Next(currListItem)) {
		cleanAddress = currListItem;
		UInternet::GetCleanAddress(cleanAddress);
		LString::CopyPStr(cleanAddress, (unsigned char *)currListItem);
		LString::PToCStr((unsigned char *)currListItem);
		SendRCPT(currListItem);
	}
}

// ---------------------------------------------------------------------------
//		¥ SendRCPT
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendRCPT(const char * theRecipient)
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	if ((!theRecipient) || (!*theRecipient))
		return;

	PP_CSTD::sprintf(commandString, "%s:<%s>", kSMTPRecipient, theRecipient);
	SendCommandWithReply(commandString, cmdResponse);
	switch (cmdResponse.GetResponseCode()) {
		case kSMTPRequestOK:
		case kSMTPNotLocal:
			break;

		default:
			throw(cmdResponse);
	}
}

// ---------------------------------------------------------------------------
//		¥ SendDATACmd
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendDATACmd()
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	//DATA
	PP_CSTD::sprintf(commandString, "%s", kSMTPData);
	SendCommandWithReply(commandString, cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPStartMail)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendMessageData
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendMessageData(LMailMessage& inMessage, SProgressMessage& inProgressMsg)
{
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	try {
		Boolean messageComplete = true;	//defaults to true as indication of start in GetPartialMessage
		do {
			LDynamicBuffer messageBuffer;
			messageComplete = inMessage.GetPartialMessage(&messageBuffer, messageComplete);
			SendSMTPEncoded(&messageBuffer, inProgressMsg, messageComplete);
		} while (!messageComplete);
	} catch(...) {
		LInternetProtocol::Disconnect();	//If we fail after DATA command is sent, we should close w/o quit
		throw;
	}

	WaitResponse(cmdResponse);
	if (cmdResponse.GetResponseCode() != kSMTPRequestOK)
		throw(cmdResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendSMTPEncoded
// ---------------------------------------------------------------------------
//	SMTP encodes message data as it sends to server. Call this function as many
//	times as necessary to send your mail message. If calling multiple times, on
//	the last call, you must set isEndOfData to true.
//
//	NOTE: Asumes you have handed it all outgoing data, or data that can or will break
//	at a end of line. This is necessary to handle SMTP encoding cleanly.

void
LSMTPConnection::SendSMTPEncoded(LDynamicBuffer * inData,
								SProgressMessage& inProgressMsg,
								Boolean isEndOfData)
{
	PP_STD::string outEncoded;

	LStream * origStream = inData->GetBufferStream();

	//If necessary, terminate the buffer with CRLF
	char termination;
	origStream->SetMarker(1, streamFrom_End);
	origStream->PeekData(&termination, 1);
	if ((termination != CR) && (termination != LF))
		inData->ConcatenateBuffer(CRLF);

	// Walk data, SMTP encode and send in chunks roughly the size of LInternetProtocol::GetSendSize()
	char currChar, nextChar;
	origStream->SetMarker(0, streamFrom_Start);
	while (origStream->ReadData(&currChar, 1)) {
		//stuff periods at the head of lines
		if (currChar == '.')
			outEncoded += '.';

		UInt32 i;
		char currLine[kSMTPMaxLineLen];
		for (i = 0; !((currChar == CR) || (currChar == LF)); i++) {
			ThrowIf_(i > kSMTPMaxLineLen - 2); //Line length > 1000 (after CRLF added)

			ThrowIf_((unsigned char)currChar >= 128); //High ASCII

			// a little bit faster to assign to char array here and
			//		append the whole line at once to outEncoded
			currLine[i] = currChar;
			origStream->ReadData(&currChar, 1);
		}

		//Append current line and write CRLF to terminate line
		outEncoded.append(currLine, i);
		outEncoded += CRLF;

		if (outEncoded.length() > GetSendSize()) {
			SendData(outEncoded.c_str(), outEncoded.size(), inProgressMsg);
			outEncoded = "";
		}

		//Walk up to begining of next line if necessary
		//	we know currChar is either a CR *or* LF so check to see if we have a CRLF
		//Peek at the next char to see if its a LF
		if (origStream->PeekData(&nextChar, 1)) {
			if (currChar == CR && nextChar == LF)
				origStream->ReadData(&nextChar, 1);	//read the LF to step over it
		}
	}

	//Write the SMTP termination sequence if this is the end of the data
	if (isEndOfData) {
		outEncoded += kMailTermSequence;
	}

	//Send the last segment of data
	SendData(outEncoded.c_str(), outEncoded.size(), inProgressMsg);
}

// ---------------------------------------------------------------------------
//		¥ SendQUIT
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendQUIT()
{
	char			commandString[256];
	LSMTPResponse	cmdResponse;

	ThrowIfNot_(mSMTPState == SMTPConnected);

	mSMTPState = SMTPClosed;

	if (!mDisconnectReceived) {
		PP_CSTD::sprintf(commandString, "%s", kSMTPQuit);
		SendCommandWithReply(commandString, cmdResponse);
		// We just ignore errors here since there's rarely anything you can do
	}
}

// ---------------------------------------------------------------------------
//		¥ SendCommandWithReply
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendCommandWithReply(
							char * 				inString,
							LSMTPResponse&		outResponse)
{
	outResponse.ResetResponse();
	SendCmd(inString);
	WaitResponse(outResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendCmd
// ---------------------------------------------------------------------------

void
LSMTPConnection::SendCmd(char * inString)
{
	ThrowIf_((PP_CSTD::strlen(inString) + 2) > kMaxSMTPCommandLen);

	//Clear the receive buffer (just in case) before sending
	//	the next command so we don't get confused about the responses.
	UInt32 unreadDataSize = mEndpoint->GetAmountUnread();
	if (unreadDataSize) {
		StPointerBlock tempBuffer((SInt32) unreadDataSize);
		mEndpoint->Receive(tempBuffer, unreadDataSize);
	}

	PP_CSTD::strcat(inString, CRLF);
	mEndpoint->Send(inString, PP_CSTD::strlen(inString));
}

// ---------------------------------------------------------------------------
//		¥ WaitResponse
// ---------------------------------------------------------------------------
// ÊNote: No Try/Catch here... we want to let the calling function catch for us

void
LSMTPConnection::WaitResponse(LSMTPResponse& outResponse)
{
	UInt32 dataSize;
	PP_STD::string dataBuffer;
	StPointerBlock theData((SInt32) mRcvSize);

	do {
		dataSize = mRcvSize;
		mEndpoint->ReceiveLine(theData, dataSize, mTimeoutSeconds, true);
		dataBuffer.append(theData, dataSize);
	} while (!HaveCompleteBuffer(dataBuffer.c_str()));

	outResponse.SetResponse(dataBuffer.c_str());
}

// ===========================================================================

#pragma mark -
#pragma mark === Utilities ===

// ---------------------------------------------------------------------------
//		¥ HaveCompleteBuffer
// ---------------------------------------------------------------------------
// Utility routine to test for a complete valid response buffer

Boolean
LSMTPConnection::HaveCompleteBuffer(const char * bufferString)
{
	Boolean haveLastLine = HaveLastLine(bufferString);

	if (haveLastLine) {
		SInt32 bufferLen = (SInt32) PP_CSTD::strlen(bufferString);
		//Just because we have the last line doesn't mean we have
		//	the whole last line.
		haveLastLine = (bufferString[bufferLen - 2] == CR)
							&& (bufferString[bufferLen - 1] == LF);
	}

	return haveLastLine;
}

// ---------------------------------------------------------------------------
//		¥ HaveLastLine
// ---------------------------------------------------------------------------
// Utility routine to find out if we have the last line of a SMTP response

Boolean
LSMTPConnection::HaveLastLine(const char * bufferString) {
	SInt16 i;
	char responseCode[4] = "";

	Boolean haveLastLine = true;
	if (bufferString[3] == '-') {
		//Copy the result code for reference against the last line
		for (i = 0; i < 3; i++) {
			responseCode[i] = bufferString[i];
		}

		//Find the last line
		char * lastTok = UInternet::PointToNextLine(bufferString);
		while (*lastTok) {
			char * nextTok = UInternet::PointToNextLine(lastTok);
			if (*nextTok)
				lastTok = nextTok;
			else
				break;
		}

		//Check the last line for a space after the result code
		if (*lastTok) {
			if (lastTok[3] == ' ') {
				for (i = 0; i < 3; i++) {
					if (responseCode[i] != lastTok[i]) {
						 haveLastLine = false;
						 break;
					}
				}
			 } else {
				 haveLastLine = false;
			}
		} else {
			 haveLastLine = false;
		}
	}

	return haveLastLine;
}

PP_End_Namespace_PowerPlant
