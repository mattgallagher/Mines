// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPOP3Connection.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// POP3 Connection

#include <LPOP3Connection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LArrayIterator.h>
#include <UThread.h>
#include <LSimpleThread.h>

#include <cstdio>

PP_Begin_Namespace_PowerPlant

#define kDefaultPOP3RcvBufferSize = 1024


// ---------------------------------------------------------------------------
//	¥ LPOP3Connection						Constructor
// ---------------------------------------------------------------------------

LPOP3Connection::LPOP3Connection(LThread& inThread)
	:LInternetProtocol(inThread)
{
	mPOP3State			= POP3Closed;
	mPOP3ServerState	= POP3Offline;
	mNoAPOP				= false;
	*mServerTimeStamp	= '\0';
	SetTickleSeconds(kPOP3DefaultTickleSeconds);
	mCheckContentLength = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LPOP3Connection						Destructor
// ---------------------------------------------------------------------------

LPOP3Connection::~LPOP3Connection()
{
}


// ---------------------------------------------------------------------------
//	¥ GetOneMessage
// ---------------------------------------------------------------------------

void
LPOP3Connection::GetOneMessage(
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inUser,
	ConstStr255Param	inPassword,
	UInt32				inMsgNumber,
	LMailMessage *		outMessage,
	Boolean				inAPOP,
	Boolean				inDelete,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mPOP3State != POP3Connected);
	// Set up the connection if necessary.

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
		Connect(inRemoteHost,
				inUser,
				inPassword,
				inAPOP,
				inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		if (mCheckContentLength) {
			POP3ListElem theListElem;
			DoLIST(inMsgNumber, theListElem);
			progress.totalBytes = theListElem.msgSize;
		}

		progress.totalItems = 1;
		LPOP3Response cmdResponse;

		progress.currentItem = (SInt32)inMsgNumber;
		BroadcastProgress(msg_ReceivingItem, progress, true);

		GetMailMessage(inMsgNumber, cmdResponse, *outMessage, progress);

		if (inDelete) {
			DeleteMessage(inMsgNumber);
			BroadcastProgress(msg_DeleteItemSuccess, progress, true);
		}

		// Tear down the connection if necessary.
		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}

		progress.completedItems++;
		BroadcastProgress(msg_RetrieveItemSuccess, progress, true);
	}
	catch (...) {
		BroadcastProgress(msg_RetrieveItemFailed, progress, true);
		// Tear down the connection if necessary.
		try {
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		} catch(...) {
		}

		throw; //rethrow the error encountered here... might be a LPOP3response object
	}
}


// ---------------------------------------------------------------------------
//	¥ GetMessages
// ---------------------------------------------------------------------------

void
LPOP3Connection::GetMessages(
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inUser,
	ConstStr255Param	inPassword,
	LInternetMessageList *	outMessageList,
	Boolean				inAPOP,
	Boolean				inDelete,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mPOP3State != POP3Connected);

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
		Connect(inRemoteHost,
				inUser,
				inPassword,
				inAPOP,
				inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		CollectAllMessages(outMessageList, inDelete, progress);

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
		// Tear down the connection if necessary.
		try {
			if (ConnectDisconnect) {
				progress.currentItem = inRemoteHost;
				BroadcastProgress(msg_ClosingConnection, progress, true);
				Disconnect();
				BroadcastProgress(msg_Disconnected, progress, true);
			}
		} catch(...) {
		}

		throw; //rethrow the error encountered here... might be a LPOP3response object
	}
}


// ---------------------------------------------------------------------------
//	¥ GetHeaders
// ---------------------------------------------------------------------------

void
LPOP3Connection::GetHeaders(
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inUser,
	ConstStr255Param	inPassword,
	LInternetMessageList *	outMessageList,
	Boolean				inAPOP,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mPOP3State != POP3Connected);

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
		Connect(inRemoteHost,
				inUser,
				inPassword,
				inAPOP,
				inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	CollectAllHeaders(outMessageList, progress);

	// Tear down the connection if necessary.
	if (ConnectDisconnect) {
		Disconnect();
		BroadcastProgress(msg_Disconnected, progress, true);
	}
}


// ---------------------------------------------------------------------------
//	¥ Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LPOP3Connection::Connect(
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inUser,
	ConstStr255Param	inPassword,
	Boolean 			inAPOP,
	SInt16				inRemotePort)
{
	if (mPOP3State == POP3Connected)
		return;

	LInternetProtocol::Connect(inRemoteHost, (UInt16) inRemotePort);

	WaitForServerID();

	DoAuthorization(inUser, inPassword, inAPOP);

	mPOP3ServerState = POP3Transaction;
	mPOP3State = POP3Connected;

	mTickleLastTicks = ::TickCount();
	StartIdling();
}


// ---------------------------------------------------------------------------
//	¥ Disconnect
// ---------------------------------------------------------------------------

void
LPOP3Connection::Disconnect()
{
	StopIdling();
	if (mPOP3State != POP3Closed) {
		SendQUIT();
		LInternetProtocol::Disconnect();
	}
}


// ---------------------------------------------------------------------------
//	¥ WaitForServerID
// ---------------------------------------------------------------------------

void
LPOP3Connection::WaitForServerID()
{
	LPOP3Response	cmdResponse;

	//Check connection state
	WaitResponse(cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);

	//Pull message id out for later APOP use
	const char * tempResponse = cmdResponse.GetResponse();
	const char * rspStart = PP_CSTD::strchr(tempResponse, '<');
	const char * rspEnd = PP_CSTD::strchr(tempResponse, '>');
	if ((!rspStart) || (!rspEnd)) {
		mNoAPOP = true;
	} else {
		PP_CSTD::strncpy(mServerTimeStamp, rspStart, (UInt32) (rspEnd - rspStart + 1));
		mServerTimeStamp[rspEnd - rspStart + 1] = '\0'; //Just in case
	}

	mPOP3ServerState = POP3Authorization;
}


// ---------------------------------------------------------------------------
//	¥ SendQUIT
// ---------------------------------------------------------------------------

void
LPOP3Connection::SendQUIT()
{
	char			commandString[256];
	LPOP3Response	cmdResponse;

	if (!mDisconnectReceived) {
		PP_CSTD::sprintf(commandString, "%s", kPOP3Quit);
		SendCommandWithReply(commandString, cmdResponse);
		if (!cmdResponse.GetStatus())
			throw(cmdResponse);

		mPOP3State = POP3Closed;
		mPOP3ServerState = POP3Update;
	}
}


// ---------------------------------------------------------------------------
//	¥ DoAuthorization
// ---------------------------------------------------------------------------
// Figures out the type of authorization we should use and tries it.
// If APOP fails (it may not be supported) we try the normal negotiation sequence.

void
LPOP3Connection::DoAuthorization(	ConstStr255Param inUser,
									ConstStr255Param inPassword,
									Boolean inAPOP)
{
	char			commandString[256];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Authorization);

	if (inAPOP && (!mNoAPOP)) {
		char APOPDigest[64];

		PP_CSTD::strcpy(commandString, mServerTimeStamp);
		::CopyPascalStringToC(inPassword, commandString + PP_CSTD::strlen(commandString));

		UInternet::MD5String(commandString, APOPDigest);

		PP_CSTD::sprintf(commandString, "%s ", kPOP3APOP);
		::CopyPascalStringToC(inUser, commandString + PP_CSTD::strlen(commandString));
		PP_CSTD::sprintf(commandString + PP_CSTD::strlen(commandString), " %s", APOPDigest);
		SendCommandWithReply(commandString, cmdResponse);

		if (cmdResponse.GetStatus())
			return;
	}

	//Fall through and try normal authorization if APOP returns error...
	// This may fail too, but at least we tried.
	PP_CSTD::sprintf(commandString, "%s ", kPOP3User);
	::CopyPascalStringToC(inUser, commandString + PP_CSTD::strlen(commandString));
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);

	PP_CSTD::sprintf(commandString, "%s ", kPOP3Pass);
	::CopyPascalStringToC(inPassword, commandString + PP_CSTD::strlen(commandString));
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);
}


// ---------------------------------------------------------------------------
//	¥ CollectAllMessages
// ---------------------------------------------------------------------------
// NOTE: A new LMailMessage is "newed" here, but is deleted by the
//			destructor of the LInternetMessageList object.
// Returns the number of messages in the list.

void
LPOP3Connection::CollectAllMessages(
					LInternetMessageList * outList,
					Boolean deleteAsRetrieve,
					SProgressMessage& inProgressMsg)
{
	LPOP3Response cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	//Get message list from server
	LArray	theListList(sizeof(POP3ListElem));
	UInt32 theCount = DoLIST(&theListList);

	inProgressMsg.theProtocol = this;
	inProgressMsg.totalItems = theCount;
	inProgressMsg.completedItems = 0;
	inProgressMsg.totalBytes = 0;
	inProgressMsg.completedBytes = 0;

	//Get the messages and append to the message list
	POP3ListElem	theListElem;
	LArrayIterator	iter(theListList);
	while (iter.Next(&theListElem)) {
		cmdResponse.ResetResponse();
		inProgressMsg.completedItems++;
		inProgressMsg.totalBytes = theListElem.msgSize;

		inProgressMsg.currentItem = (SInt32)theListElem.msgNumber;
		BroadcastProgress(msg_ReceivingItem, inProgressMsg, true);

		try {
			LMailMessage * tempMessage =  new LMailMessage();

			GetMailMessage(theListElem.msgNumber, cmdResponse, *tempMessage, inProgressMsg);

			outList->AppendItem(&tempMessage);

			BroadcastProgress(msg_RetrieveItemSuccess, inProgressMsg, true);

			if (deleteAsRetrieve) {
				DeleteMessage(theListElem.msgNumber);
				BroadcastProgress(msg_DeleteItemSuccess, inProgressMsg, true);
			}
		} catch (...) {
			BroadcastProgress(msg_RetrieveItemFailed, inProgressMsg, true);
			throw;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CollectAllHeaders
// ---------------------------------------------------------------------------
// NOTE: A new LMailMessage is created here, but is deleted by the
//			destructor of the LInternetMessageList object.
// Returns the number of messages in the list.
// Does not try to delete messages since it's only getting headers.

void
LPOP3Connection::CollectAllHeaders(
						LInternetMessageList * outList,
						SProgressMessage& inProgressMsg)
{
	LPOP3Response cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	//Get message list from server
	LArray	theListList(sizeof(POP3ListElem));
	UInt32 theCount = DoLIST(&theListList);

	inProgressMsg.theProtocol = this;
	inProgressMsg.totalItems = theCount;
	inProgressMsg.completedItems = 0;
	inProgressMsg.totalBytes = 0;
	inProgressMsg.completedBytes = 0;

	//Get the messages and append to the message list
	POP3ListElem	theListElem;
	LArrayIterator	iter(theListList);
	while (iter.Next(&theListElem)) {
		inProgressMsg.totalBytes = theListElem.msgSize;
		inProgressMsg.currentItem = (SInt32)theListElem.msgNumber;
		inProgressMsg.completedItems++;

		try {
			LMailMessage * tempMessage =  new LMailMessage();

			GetTop(theListElem.msgNumber, 0, *tempMessage, cmdResponse);

			outList->AppendItem(&tempMessage);
			BroadcastProgress(msg_RetrieveItemSuccess, inProgressMsg, true);
		} catch (...) {
			BroadcastProgress(msg_RetrieveItemFailed, inProgressMsg, true);
			throw;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DeleteMessage
// ---------------------------------------------------------------------------

void
LPOP3Connection::DeleteMessage(UInt32 inMsgNumber)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s %lu", kPOP3Dele, inMsgNumber);
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);
}


// ---------------------------------------------------------------------------
//	¥ ResetServer
// ---------------------------------------------------------------------------

void
LPOP3Connection::ResetServer(LPOP3Response * outCmdResponse)
{
	char			commandString[kMaxPOP3CommandLen];

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s", kPOP3Rset);
	SendCommandWithReply(commandString, *outCmdResponse);

	if (!outCmdResponse->GetStatus()) {
		throw *outCmdResponse;
	}
}


// ---------------------------------------------------------------------------
//	¥ ServerStatus
// ---------------------------------------------------------------------------

void
LPOP3Connection::ServerStatus(UInt32 * outCount, UInt32 * outSize)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s", kPOP3Stat);
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);

	const char * response = cmdResponse.GetResponse();
	PP_CSTD::sscanf(response, "%lu %lu", outCount, outSize);
}


// ---------------------------------------------------------------------------
//	¥ NoopServer
// ---------------------------------------------------------------------------

void
LPOP3Connection::NoopServer()
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s", kPOP3NOOP);
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);
}


// ---------------------------------------------------------------------------
//	¥ GetMailMessage
// ---------------------------------------------------------------------------

void
LPOP3Connection::GetMailMessage(UInt32 				inMsgNumber,
								LPOP3Response& 		outResponse,
								LMailMessage&		outMessage,
								SProgressMessage&	inProgressMsg)
{
	char			commandString[kMaxPOP3CommandLen];

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	if (!inProgressMsg.totalBytes) {
		POP3ListElem theListElem;
		DoLIST(inMsgNumber, theListElem);
		inProgressMsg.totalBytes = theListElem.msgSize;
	}

	PP_CSTD::sprintf(commandString, "%s %lu", kPOP3Retr, inMsgNumber);
	SendCommandWithMailReply(commandString, outResponse, outMessage, &inProgressMsg);
	if (!outResponse.GetStatus())
		throw(outResponse);
}


// ---------------------------------------------------------------------------
//	¥ GetTop
// ---------------------------------------------------------------------------

void
LPOP3Connection::GetTop(UInt32 				inMsgNumber,
						UInt32 				inMsgLines,
						LMailMessage&		outMessage,
						LPOP3Response&		outResponse)
{
	char			commandString[kMaxPOP3CommandLen];

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s %lu %lu", kPOP3Top, inMsgNumber, inMsgLines);
	SendCommandWithMailReply(commandString, outResponse, outMessage, nil);
	if (!outResponse.GetStatus())
		throw(outResponse);
}


// ---------------------------------------------------------------------------
//	¥ DoLIST
// ---------------------------------------------------------------------------
//	Pass nil for outList will return only the count

UInt32
LPOP3Connection::DoLIST(LArray * outList)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;
	UInt32			theCount = 0;
	POP3ListElem	theListElem;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s", kPOP3List);
	SendCommandWithMultiReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		return 0;

	//Parse the list return
	StHandleBlock text(Size_Zero);
	cmdResponse.GetResponseData()->BufferToHandle(text);
	StHandleLocker	lockItems(text);
	char * nextLine = PP_CSTD::strtok(*text, CRLF);
	while (1) {
		if (PP_CSTD::strcmp(nextLine, ".") == 0)
			break;
		theCount++;

		if (outList) {
			PP_CSTD::sscanf(nextLine, "%lu %lu", &theListElem.msgNumber, &theListElem.msgSize);
			outList->InsertItemsAt(1, LArray::index_Last, &theListElem);
		}
		nextLine = PP_CSTD::strtok(nil, CRLF);
	}

	return theCount;
}


// ---------------------------------------------------------------------------
//	¥ DoLIST
// ---------------------------------------------------------------------------

void
LPOP3Connection::DoLIST(UInt32 inMsgNumber, POP3ListElem& outListElem)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s %lu", kPOP3List, inMsgNumber);
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);

	//Parse the list return
	char tempResponse[255];
	PP_CSTD::strcpy(tempResponse, cmdResponse.GetResponse());
	char * nextLine = PP_CSTD::strtok(tempResponse, CRLF);
	PP_CSTD::sscanf(nextLine, "%lu %lu", &outListElem.msgNumber, &outListElem.msgSize);
}


// ---------------------------------------------------------------------------
//	¥ DoUIDL
// ---------------------------------------------------------------------------
//	Pass nil for outList will return only the count

UInt32
LPOP3Connection::DoUIDL(LArray * outList)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;
	UInt32			theCount = 0;
	POP3ListElem	theListElem;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s", kPOP3UIDL);
	SendCommandWithMultiReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		return 0;

	//Parse the list return
	StHandleBlock text(Size_Zero);
	cmdResponse.GetResponseData()->BufferToHandle(text);
	StHandleLocker	lockItems(text);
	char * nextLine = PP_CSTD::strtok(*text, CRLF);
	while (1) {
		if (PP_CSTD::strcmp(nextLine, ".") == 0)
			break;
		theCount++;

		if (outList) {
			PP_CSTD::sscanf(nextLine, "%lu %s", &theListElem.msgNumber, theListElem.msgUIDL);
			outList->InsertItemsAt(1, LArray::index_Last, &theListElem);
		}

		nextLine = PP_CSTD::strtok(nil, CRLF);
	}

	return theCount;
}


// ---------------------------------------------------------------------------
//	¥ DoUIDL
// ---------------------------------------------------------------------------

void
LPOP3Connection::DoUIDL(UInt32 inMsgNumber, POP3ListElem& outListElem)
{
	char			commandString[kMaxPOP3CommandLen];
	LPOP3Response	cmdResponse;

	ThrowIf_(mPOP3ServerState != POP3Transaction);

	PP_CSTD::sprintf(commandString, "%s %lu", kPOP3UIDL, inMsgNumber);
	SendCommandWithReply(commandString, cmdResponse);
	if (!cmdResponse.GetStatus())
		throw(cmdResponse);

	//Parse the list return
	char tempResponse[255];
	PP_CSTD::strcpy(tempResponse, cmdResponse.GetResponse());
	char * nextLine = PP_CSTD::strtok(tempResponse, CRLF);
	PP_CSTD::sscanf(nextLine, "%lu %s", &outListElem.msgNumber, outListElem.msgUIDL);
}


// ---------------------------------------------------------------------------
//	¥ SendCommandWithReply
// ---------------------------------------------------------------------------

void
LPOP3Connection::SendCommandWithReply(
							char * 				inString,
							LPOP3Response&		outResponse)
{
	StMutex lockCommand(mCommandMutex);

	outResponse.ResetResponse();
	SendCmd(inString);
	WaitResponse(outResponse);
}


// ---------------------------------------------------------------------------
//	¥ SendCommandWithMultiReply
// ---------------------------------------------------------------------------

void
LPOP3Connection::SendCommandWithMultiReply(
							char * 				inString,
							LPOP3Response&		outResponse,
							SProgressMessage *	inProgressMsg)
{
	StMutex lockCommand(mCommandMutex);

	outResponse.ResetResponse();
	SendCmd(inString);
	WaitMultiResponse(outResponse, inProgressMsg);
}


// ---------------------------------------------------------------------------
//	¥ SendCommandWithMailReply
// ---------------------------------------------------------------------------

void
LPOP3Connection::SendCommandWithMailReply(
							char * 				inString,
							LPOP3Response&		outResponse,
							LMailMessage&		outMessage,
							SProgressMessage *	inProgressMsg)
{
	StMutex lockCommand(mCommandMutex);

	outResponse.ResetResponse();
	outMessage.ResetMembers();
	SendCmd(inString);
	WaitMailResponse(outResponse, outMessage, inProgressMsg);
}


// ---------------------------------------------------------------------------
//	¥ SendCmd
// ---------------------------------------------------------------------------

void
LPOP3Connection::SendCmd(char * inString)
{
	ThrowIf_((PP_CSTD::strlen(inString) + 2) > kMaxPOP3CommandLen);

	PP_CSTD::strcat(inString, CRLF);

	mEndpoint->Send(inString, PP_CSTD::strlen(inString));

	mTickleLastTicks = ::TickCount();	//Reset last tickle counter
}


// ---------------------------------------------------------------------------
//	¥ WaitResponse
// ---------------------------------------------------------------------------
//ÊNote: No Try/Catch here... we want the run function to catch for us

void
LPOP3Connection::WaitResponse(LPOP3Response&		outResponse)
{
	char theResponse[kMaxPOP3ResponseLen];
	UInt32 dataSize = kMaxPOP3ResponseLen;

	ThrowIfNot_(mEndpoint->ReceiveLine(theResponse,
										dataSize,
										mTimeoutSeconds,
										true));
	theResponse[dataSize] = '\0';

	outResponse.SetResponse(theResponse);
}


// ---------------------------------------------------------------------------
//	¥ WaitMultiResponse
// ---------------------------------------------------------------------------

void
LPOP3Connection::WaitMultiResponse(
	LPOP3Response&		outResponse,
	SProgressMessage*	inProgressMsg)
{
	char theResponse[kMaxPOP3ResponseLen];
	LDynamicBuffer dataBuffer;
	StPointerBlock theData((SInt32) mRcvSize);
	Boolean dataExpedited;
	UInt32 dataSize;

	if (inProgressMsg) {
		inProgressMsg->completedBytes = 0;
	}

	// first grab the response portion
	dataSize = kMaxPOP3ResponseLen;
	ThrowIfNot_(mEndpoint->ReceiveLine(theResponse,
										dataSize,
										mTimeoutSeconds,
										true));
	theResponse[dataSize] = '\0';

	// now loop and get all the data
	do {
		dataSize = mRcvSize;

		mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);

		dataBuffer.ConcatenateBuffer(theData, (SInt32) dataSize);

		if (inProgressMsg) {
			inProgressMsg->completedBytes += dataSize;
			BroadcastProgress(msg_ReceivingData, *inProgressMsg);
		}
	} while (!HaveCompleteMultiBuffer(&dataBuffer));

	// Force the last msg_ReceivingData message just in case
	if (inProgressMsg != nil) {
		BroadcastProgress(msg_ReceivingData, *inProgressMsg, true);
	}

	outResponse.SetResponse(theResponse, &dataBuffer);
}


// ---------------------------------------------------------------------------
//	¥ WaitMailResponse
// ---------------------------------------------------------------------------

void
LPOP3Connection::WaitMailResponse(
	LPOP3Response& 		outResponse,
	LMailMessage&		outMessage,
	SProgressMessage*	inProgressMsg)
{
	char theResponse[kMaxPOP3ResponseLen];
	LDynamicBuffer dataBuffer;

	StPointerBlock theData((SInt32) (mRcvSize + 1));
	StPointerBlock smtpData((SInt32) (mRcvSize + kSMTPMaxLineLen + 1));

	Boolean dataExpedited;
	UInt32 dataSize;

	if (inProgressMsg) {
		inProgressMsg->completedBytes = 0;
	}

	//first grab the response portion
	dataSize = kMaxPOP3ResponseLen;
	ThrowIfNot_(mEndpoint->ReceiveLine(theResponse,
										dataSize,
										mTimeoutSeconds,
										true));
	theResponse[dataSize] = '\0';
	outResponse.SetResponse(theResponse);

	//now loop and get the mail message
	Boolean haveFullMessage = false;
	Boolean lineIncomplete;
	*smtpData = '\0';
	do {
		dataSize = mRcvSize;

		mEndpoint->ReceiveData(theData, dataSize, dataExpedited, mTimeoutSeconds);
		theData[dataSize] = '\0';

		//Remove SMTP decoding
		PP_CSTD::strcat(smtpData, theData);
		SInt32 i;
		for (i = 0; smtpData[i]; i++) {
			//start of line
			if (smtpData[i] == '.') {
				//look to see if next char is a period
				if (smtpData[i + 1] == '.')
					i++;
				else if ((smtpData[i + 1]) && (smtpData[i + 1] == CR)) {
					haveFullMessage = true;
					break;
				}
			}

			//scan to end of line
			UInt32 j;
			lineIncomplete = false;
			char currLine[kSMTPMaxLineLen];
			for (j = 0; smtpData[i] != LF; j++) {
				ThrowIf_(j > kSMTPMaxLineLen);
				currLine[j] = smtpData[i++];
				//break if have incomplete line
				if (!smtpData[i]) {
					i -= (j+1);	//"unread" this line
					lineIncomplete = true;
					break;
				}
			}

			//bailout if we had an incomplete line
			//	we'll pick up the rest next time.
			if (lineIncomplete)
				break;

			//append LF
			currLine[j++] = smtpData[i];

			//write decoded line to buffer
			dataBuffer.ConcatenateBuffer(currLine, (SInt32) j);
		}

		//save what's left for the next receive. it will be
		//	decoded and added to buffer as possible
		PP_CSTD::memmove(smtpData, &smtpData[i], PP_CSTD::strlen(&smtpData[i]) + 1);	//+1 grabs the terminator too

		if (inProgressMsg) {
			inProgressMsg->completedBytes += dataSize;
			BroadcastProgress(msg_ReceivingData, *inProgressMsg);
		}

		outMessage.SetPartialMessage(&dataBuffer);

	} while (!haveFullMessage);

	// Force the last msg_ReceivingData message just in case
	if (inProgressMsg != nil) {
		BroadcastProgress(msg_ReceivingData, *inProgressMsg, true);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HaveCompleteMultiBuffer
// ---------------------------------------------------------------------------
// Utility routine to test for a comlete valid response buffer
//	This version watches for "CRLF.CRLF" sequence at end of buffer.

Boolean
LPOP3Connection::HaveCompleteMultiBuffer(LDynamicBuffer* dataBuffer)
{
	char tempBufferEnd[kMailTermSequenceLength + 1];

	LStream * buffStream = dataBuffer->GetBufferStream();

	buffStream->SetMarker(kMailTermSequenceLength, streamFrom_End);
	SInt32 dataRead = buffStream->PeekData(tempBufferEnd, kMailTermSequenceLength);
	tempBufferEnd[dataRead] = '\0';

	//Special case where we have an empty return buffer
	if ((dataRead == 3) && (PP_CSTD::strcmp(tempBufferEnd, ".\r\n") == 0))
		return true;
	else if (PP_CSTD::strstr(tempBufferEnd, kMailTermSequence))
		return true;

	return false;
}


// ---------------------------------------------------------------------------
//	¥ SpendTime
// ---------------------------------------------------------------------------

void
LPOP3Connection::SpendTime(const EventRecord& /* inMacEvent*/ )
{
	//Tickle the server so it doesn't drop us.
	if (mPOP3ServerState == POP3Transaction) {
		if ((mTickleLastTicks + mTickleQuantum) < ::TickCount()) {
			mTickleLastTicks = ::TickCount();
			//Create seperate thread to avoid calling NOOP from
			//	main thread
			LSimpleThread * NoopThread = new LSimpleThread(
					LPOP3Connection::LPOP3Connection_NOOPThread,
					this);
			NoopThread->Resume();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LPOP3Connection_NOOPThread
// ---------------------------------------------------------------------------
// Utility routine intended to be called from LSimpleThread that allows
//	us to NOOP the server from a thread other than the main thread durring idle time

void
LPOP3Connection::LPOP3Connection_NOOPThread(LThread& thread, void* arg)
{
#pragma unused (thread)
	LPOP3Connection * theConnection = (LPOP3Connection *)arg;
	try {
		theConnection->NoopServer();
	} catch (...) {
	}
}

PP_End_Namespace_PowerPlant
