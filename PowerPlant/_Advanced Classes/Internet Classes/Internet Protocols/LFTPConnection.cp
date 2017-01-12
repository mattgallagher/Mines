// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPConnection.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// FTP Connection - This object represnts the "Command Connection" portion of
//	the FTP protocol.
//
// To Do:
//		Add "MACB D" support
//		Add support for sending resource fork (MacBinary Encoding)

#include <LFTPConnection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LFTPDataConnection.h>
#include <LSimpleThread.h>
#include <UThread.h>

#include <cstdio>
#include <cstdlib>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LFTPConnection ===

// ---------------------------------------------------------------------------
//		¥ LFTPConnection()
// ---------------------------------------------------------------------------
//	Constructor

LFTPConnection::LFTPConnection(LThread& inThread)
	:LInternetProtocol(inThread)
{
	mFTPState = FTPClosed;
	mRemoteSystem = "";
	SetTickleSeconds(kFTPDefaultTickleSeconds);
	mMacBinaryNegotiated = false;
}


// ---------------------------------------------------------------------------
//		¥ ~LFTPConnection
// ---------------------------------------------------------------------------
//	Destructor

LFTPConnection::~LFTPConnection()
{
}

// ---------------------------------------------------------------------------
//		¥ PutFile
// ---------------------------------------------------------------------------

void
LFTPConnection::PutFile(
	ConstStr255Param	inRemoteHost,
	const char * 		inUser,
	const char *		inPassword,
	const char *		inAccount,
	EFTPTransferType	inType,
	const char *		inFileName,
	LFile * 			inFile,
	char *				outFileName,
	Boolean 			createUnique,
	Boolean				usePassive,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mFTPState != FTPConnected);

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
		Connect(inRemoteHost,
				inUser,
				inPassword,
				inAccount,
				inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		// Send the message.
		InternalPutFile(inFileName,
						inFile,
						inType,
						outFileName,
						createUnique,
						progress,
						usePassive);

		// Tear down the connection if necessary.
		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}
	} catch (...) {
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

		throw;	//rethrow the error encountered here... might be a LFTPResponse object
	}
}

// ---------------------------------------------------------------------------
//		¥ PutFile
// ---------------------------------------------------------------------------

void
LFTPConnection::PutFile(
	EFTPTransferType	inType,
	const char *		inFileName,
	LFile * 			inFile,
	char *				outFileName,
	Boolean 			createUnique,
	Boolean				usePassive)
{
	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	// Send the message.
	InternalPutFile(inFileName,
					inFile,
					inType,
					outFileName,
					createUnique,
					progress,
					usePassive);
}

// ---------------------------------------------------------------------------
//		¥ GetFile
// ---------------------------------------------------------------------------

void
LFTPConnection::GetFile(
	ConstStr255Param	inRemoteHost,
	const char *		inUser,
	const char *		inPassword,
	const char *		inAccount,
	EFTPTransferType	inType,
	const char *		inFileName,
	LFile * 			inFile,
	Boolean				usePassive,
	SInt16				inRemotePort)
{
	Boolean ConnectDisconnect = (mFTPState != FTPConnected);

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
		Connect(inRemoteHost,
				inUser,
				inPassword,
				inAccount,
				inRemotePort);
		BroadcastProgress(msg_Connected, progress, true);
	}

	try {
		InternalGetFile(inFileName,
						inFile,
						inType,
						progress,
						usePassive);

		// Tear down the connection if necessary.
		if (ConnectDisconnect) {
			ConnectDisconnect = false;
			progress.currentItem = inRemoteHost;
			BroadcastProgress(msg_ClosingConnection, progress, true);
			Disconnect();
			BroadcastProgress(msg_Disconnected, progress, true);
		}
	} catch (...) {
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

		throw;	//rethrow the error encountered here... might be a LFTPResponse object
	}

}

// ---------------------------------------------------------------------------
//		¥ GetFile
// ---------------------------------------------------------------------------

void
LFTPConnection::GetFile(
	EFTPTransferType	inType,
	const char *		inFileName,
	LFile * 			inFile,
	Boolean				usePassive)
{
	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;

	InternalGetFile(inFileName,
					inFile,
					inType,
					progress,
					usePassive);
}

// ---------------------------------------------------------------------------
//		¥ RenameRemoteFile
// ---------------------------------------------------------------------------

void
LFTPConnection::RenameRemoteFile(const char * inOldFileName, const char * inNewFileName)
{
	SendRenameFileFrom(inOldFileName);
	SendRenameFileTo(inNewFileName);
}

// ---------------------------------------------------------------------------
//		¥ ListFolder
// ---------------------------------------------------------------------------
// Pass nil or use default for inDirectory to get current folder.

void
LFTPConnection::ListFolder(
							LDynamicBuffer* outBuffer,
							const char * inDirectory,
							Boolean NameList,
							Boolean usePassive)
{
	LStr255	HostStr;
	UInt32	HostNum;
	SInt16	Port;

	//Open data connection
	if (usePassive)
		SendPASV(HostStr, &Port);

	// Set up the progress message.
	SProgressMessage progress;
	progress.theProtocol = this;
	progress.totalItems = 0;
	progress.completedItems = 0;
	progress.totalBytes = 0;
	progress.completedBytes = 0;
	if (inDirectory != nil) {
		progress.currentItem = inDirectory;
	}

	LFTPDataConnection theDataConnection(this);
	BroadcastProgress(msg_OpeningDataConnection, progress, true);
	theDataConnection.MakeDataConnection(HostStr, &HostNum, &Port, usePassive);

	if (!usePassive)
		SendPORT(HostNum, Port);

	BroadcastProgress(msg_DataConnected, progress, true);

	try {
		SendLIST(inDirectory, NameList);
		theDataConnection.RetrieveData(outBuffer, progress);
	} catch(...) {
	}

	BroadcastProgress(msg_ClosingDataConnection, progress, true);
	theDataConnection.Disconnect();
	BroadcastProgress(msg_DataDisconnected, progress, true);

	//Should get a response indicating transfer successful
	WaitResponse(mLastResponse);
	if (! (mLastResponse.GetResponseCode() == kFTPClosingDataCon ||
	 		mLastResponse.GetResponseCode() == kFTPConnectionClosed))
	{
		BroadcastProgress(msg_RetrieveItemFailed, progress, true);
		throw (mLastResponse);
	}

	BroadcastProgress(msg_RetrieveItemSuccess, progress, true);
}

// ---------------------------------------------------------------------------
//		¥ Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LFTPConnection::Connect(
	ConstStr255Param	inRemoteHost,
	const char *		inUser,
	const char *		inPassword,
	const char *		inAccount,
	SInt16				inRemotePort)
{
	if (mFTPState == FTPConnected)
		return;

	LInternetProtocol::Connect(inRemoteHost, (UInt16) inRemotePort);

	WaitForServerID();

	DoAuthorization(inUser, inPassword, inAccount);

	mFTPState = FTPConnected;
	mTickleLastTicks = ::TickCount();
}

// ---------------------------------------------------------------------------
//		¥ DoAuthorization
// ---------------------------------------------------------------------------
//

void
LFTPConnection::DoAuthorization(const char * inUser, const char * inPassword, const char * inAccount)
{
	char			commandString[256];

	if (mFTPState != FTPClosed)
		return;

	if ((!inUser) || (!*inUser))
		PP_CSTD::sprintf(commandString, "%s %s", kFTPUser, kFTPAnonymous);
	else
		PP_CSTD::sprintf(commandString, "%s %s", kFTPUser, inUser);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPUserNameOK)
		throw(mLastResponse);

	char tempPass[256];
	if ((!inPassword) || (!*inPassword)) {
		LInternetAddress* myAddress = mEndpoint->GetLocalAddress();
		Str255 myAddressString;
		myAddress->GetIPDescriptor(myAddressString);

		PP_CSTD::strcpy(tempPass, "user@");
		::CopyPascalStringToC(myAddressString, tempPass + PP_CSTD::strlen(tempPass));

		inPassword = tempPass;
		delete myAddress;
	}

	PP_CSTD::sprintf(commandString, "%s %s", kFTPPass, inPassword);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPUserLoggedIn)
		throw(mLastResponse);

	//The response to password is likely to have server info in it
	//	so we don't use mLastResponse here
	if (inAccount && *inAccount) {
		LFTPResponse	cmdResponse;
		PP_CSTD::sprintf(commandString, "%s %s", kFTPAccount, inAccount);
		SendCommandWithReply(commandString, cmdResponse);
		if (cmdResponse.GetResponseCode() != kFTPUserLoggedIn ||
			cmdResponse.GetResponseCode() != kFTPCmdNotImp) {
			throw(cmdResponse);
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ Disconnect
// ---------------------------------------------------------------------------

void
LFTPConnection::Disconnect()
{
	if (mFTPState != FTPClosed) {
		SendQUIT();
		LInternetProtocol::Disconnect();
	}
}

// ---------------------------------------------------------------------------
//		¥ InternalPutFile
// ---------------------------------------------------------------------------

void
LFTPConnection::InternalPutFile(
								const char * inFileName,
								LFile * inFile,
								EFTPTransferType inType,
								char * outFileName,
								Boolean createUnique,
								SProgressMessage&	inProgressMsg,
								Boolean usePassive)
{
	StSupressNOOP noNOOP(this);

	LStr255	HostStr;
	UInt32	HostNum;
	SInt16	Port;

	ThrowIfNot_(mFTPState == FTPConnected);

	SendTYPE(inType);

	if (usePassive)
		SendPASV(HostStr, &Port);

	inProgressMsg.currentItem = inFileName;

	LFTPDataConnection theDataConnection(this);
	BroadcastProgress(msg_OpeningDataConnection, inProgressMsg, true);
	theDataConnection.MakeDataConnection(HostStr, &HostNum, &Port, usePassive);

	if (!usePassive)
		SendPORT(HostNum, Port);

	BroadcastProgress(msg_DataConnected, inProgressMsg, true);

	SendSTOR(inFileName, mLastResponse, createUnique);	//cmdResponse will contain new name if unique
	if (createUnique && outFileName) {
		PP_CSTD::strcpy(outFileName, mLastResponse.GetResponse());
	}

	try {
		BroadcastProgress(msg_SendingItem, inProgressMsg, true);
		theDataConnection.SendFile(inFile, inProgressMsg);
	} catch(...) {
	}

	BroadcastProgress(msg_ClosingDataConnection, inProgressMsg, true);
	theDataConnection.Disconnect();
	BroadcastProgress(msg_DataDisconnected, inProgressMsg, true);

	//Should get a response indicating transfer successful
	//check for any/all 2xx codes (these are all positive responses)
	WaitResponse(mLastResponse);
	if (! (mLastResponse.GetResponseCode()/100 == 2)) {
		BroadcastProgress(msg_SendItemFailed, inProgressMsg, true);
		throw (mLastResponse);
	}

	inProgressMsg.completedItems++;
	BroadcastProgress(msg_SendItemSuccess, inProgressMsg, true);
}

// ---------------------------------------------------------------------------
//		¥ InternalGetFile
// ---------------------------------------------------------------------------

void
LFTPConnection::InternalGetFile(
								const char * inFileName,
								LFile * inFile,
								EFTPTransferType inType,
								SProgressMessage&	inProgressMsg,
								Boolean usePassive)
{
	StSupressNOOP noNOOP(this);

	LStr255	HostStr;
	UInt32	HostNum;
	SInt16	Port;

	ThrowIfNot_(mFTPState == FTPConnected);

	SendTYPE(inType);

	if (usePassive)
		SendPASV(HostStr, &Port);

	inProgressMsg.currentItem = inFileName;

	LFTPDataConnection theDataConnection(this);
	BroadcastProgress(msg_OpeningDataConnection, inProgressMsg, true);
	theDataConnection.MakeDataConnection(HostStr, &HostNum, &Port, usePassive);

	if (!usePassive)
		SendPORT(HostNum, Port);

	BroadcastProgress(msg_DataConnected, inProgressMsg, true);

	SInt32 theSize = SendRETR(inFileName);
	if (inProgressMsg.totalBytes == 0)
		inProgressMsg.totalBytes = (UInt32) theSize;
	inProgressMsg.currentItem = inFileName;

	try {
		BroadcastProgress(msg_ReceivingItem, inProgressMsg, true);
		theDataConnection.RetrieveFile(inFile, inProgressMsg);
	} catch(...) {
	}

	BroadcastProgress(msg_ClosingDataConnection, inProgressMsg, true);
	theDataConnection.Disconnect();
	BroadcastProgress(msg_DataDisconnected, inProgressMsg, true);

	//Should get a response indicating transfer successful
	WaitResponse(mLastResponse);
	if (! (mLastResponse.GetResponseCode()/100 == 2)) {
		BroadcastProgress(msg_RetrieveItemFailed, inProgressMsg, true);
		throw (mLastResponse);
	}

	BroadcastProgress(msg_RetrieveItemSuccess, inProgressMsg, true);
}

// ---------------------------------------------------------------------------
//		¥ NegotiateMacBinaryTransfers
// ---------------------------------------------------------------------------

void
LFTPConnection::NegotiateMacBinaryTransfers()
{
	mMacBinaryNegotiated = SendMACB();
}

// ---------------------------------------------------------------------------
//		¥ SendMACB
// ---------------------------------------------------------------------------
//	NOTE: 	This routine returns a Boolean representing the success of the
//			command and does not throw an exception. This is due to the fact
//			that a MACB negotiation is quite likely to fail but does not
//			represent a failure condition in your connection.

Boolean
LFTPConnection::SendMACB()
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %c", kFTPMACB, 'E');	//¥ÊNeed type stuff here
	SendCommandWithReply(commandString, mLastResponse);

	return (mLastResponse.GetResponseCode() == kFTPCommandOK);
}

// ---------------------------------------------------------------------------
//		¥ SendRETR
// ---------------------------------------------------------------------------

SInt32
LFTPConnection::SendRETR(const char * inFileName)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPRetrieve, inFileName);
	SendCommandWithReply(commandString, mLastResponse);
	if (!	((mLastResponse.GetResponseCode() == kFTPDataConOpen) ||
			(mLastResponse.GetResponseCode() == kFTPAboutToOpen))) {
		throw(mLastResponse);
	}

	//Try to get the length of the file from the response
	//	 not guaranteed to work with all servers
	SInt32 theNumber = 0;
	const char * offset = PP_CSTD::strchr(mLastResponse.GetResponse(), '(');
	if (offset) {
		LStr255 theNumString = offset + 1;
		//truncate the string at the first space character
		UInt8 firstSpace = theNumString.Find(' ');
		if (firstSpace)
			theNumString[0] = (UInt8) (firstSpace - 1);

		::StringToNum(theNumString, &theNumber);
	}
	return theNumber;
}

// ---------------------------------------------------------------------------
//		¥ SendLIST
// ---------------------------------------------------------------------------

void
LFTPConnection::SendLIST(const char * inPath, Boolean NameList)
{
	char			commandString[256];
	const char * command = NameList ? kFTPNameList : kFTPList;

	ThrowIfNot_(mFTPState == FTPConnected);

	if (inPath && *inPath)
		PP_CSTD::sprintf(commandString, "%s %s", command, inPath);
	else
		PP_CSTD::sprintf(commandString, "%s", command);

	SendCommandWithReply(commandString, mLastResponse);
	if (!	((mLastResponse.GetResponseCode() == kFTPDataConOpen) ||
			(mLastResponse.GetResponseCode() == kFTPAboutToOpen))) {
		throw(mLastResponse);
	}
}


// ---------------------------------------------------------------------------
//		¥ SendPASV
// ---------------------------------------------------------------------------

void
LFTPConnection::SendPASV(
						LStr255&	outHostStr,
						SInt16 *		outPort)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s", kFTPPassive);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPEnteringPsv)
		throw(mLastResponse);

	char response[kMaxFTPCommandLen];
	PP_CSTD::strcpy(response, mLastResponse.GetResponse());

	//lower string for easy compares later on
	UInt32 i = 0;
	while (response[i] != '\0') {
		response[i] = (char) (PP_CSTD::tolower(response[i]));
		i++;
	}

	char * address, dottedDecAddress[255];
	char * currTok;
	char * p;

	//Reply string "should" be: Entering Passive Mode A1,A2,A3,A4,a1,a2
	address = PP_CSTD::strstr(response, "mode");
	ThrowIfNot_(address);

	address += 4;	//past "mode"

	*dottedDecAddress = '\0';
	currTok = PP_CSTD::strtok(address, "(), .");	//A1...Host address
	PP_CSTD::strcat (dottedDecAddress, currTok);

	currTok = PP_CSTD::strtok(nil, "(), .");		//A2
	PP_CSTD::strcat (dottedDecAddress, ".");
	PP_CSTD::strcat (dottedDecAddress, currTok);

	currTok = PP_CSTD::strtok(nil, "(), .");		//A3
	PP_CSTD::strcat (dottedDecAddress, ".");
	PP_CSTD::strcat (dottedDecAddress, currTok);

	currTok = PP_CSTD::strtok(nil, "(), .");		//A4
	PP_CSTD::strcat (dottedDecAddress, ".");
	PP_CSTD::strcat (dottedDecAddress, currTok);
	outHostStr = dottedDecAddress;

	p = (char*)(outPort);
	currTok = PP_CSTD::strtok(nil, "(), .");		//a1...Host port
	p[0] = (char) (PP_CSTD::strtol(currTok, nil, 10) & 0xff);

	currTok = PP_CSTD::strtok(nil, "(), .");		//a1
	p[1] = (char) (PP_CSTD::strtol(currTok, nil, 10) & 0xff);
}

// ---------------------------------------------------------------------------
//		¥ SendSTOR
// ---------------------------------------------------------------------------

void
LFTPConnection::SendSTOR(
						const char * inFileName,
						LFTPResponse& outResponse,
						Boolean	inUnique)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	if (inUnique)
		PP_CSTD::sprintf(commandString, "%s %s", kFTPStoreUnique, inFileName);
	else
		PP_CSTD::sprintf(commandString, "%s %s", kFTPStore, inFileName);

	SendCommandWithReply(commandString, outResponse);
	if (!	((outResponse.GetResponseCode() == kFTPDataConOpen) ||
			(outResponse.GetResponseCode() == kFTPAboutToOpen))) {
		throw(outResponse);
	}
}

// ---------------------------------------------------------------------------
//		¥ SendPORT
// ---------------------------------------------------------------------------

void
LFTPConnection::SendPORT(
						UInt32	HostNum,
						SInt16	Port)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %hu,%hu,%hu,%hu,%hu,%hu", kFTPPortCmd,
		(unsigned short)((HostNum>>24)&0xff),
		(unsigned short)((HostNum>>16)&0xff),
		(unsigned short)((HostNum>>8)&0xff),
		(unsigned short)(HostNum&0xff),
		(unsigned short)((Port>>8)&0xff),
		(unsigned short)(Port&0xff));

	SendCommandWithReply(commandString, mLastResponse);
	if (!mLastResponse.CommandOK())
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ WaitForServerID
// ---------------------------------------------------------------------------

void
LFTPConnection::WaitForServerID()
{
	//Check connection state
	WaitResponse(mLastResponse);
	if (mLastResponse.GetResponseCode() !=  kFTPServiceReady)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendTYPE
// ---------------------------------------------------------------------------

void
LFTPConnection::SendTYPE(EFTPTransferType inType)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %c", kFTPType, inType);
	SendCommandWithReply(commandString, mLastResponse);
	if (!mLastResponse.CommandOK())
		throw(mLastResponse);

	mTransferType = inType;
}

// ---------------------------------------------------------------------------
//		¥ SendChangeDir
// ---------------------------------------------------------------------------

void
LFTPConnection::SendChangeDir(const char * inDirectory)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPChangeDir, inDirectory);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPFileActionOK) {
		throw(mLastResponse);
	}
}

// ---------------------------------------------------------------------------
//		¥ SendChangeDirUp
// ---------------------------------------------------------------------------

void
LFTPConnection::SendChangeDirUp()
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s", kFTPChangeDirUp);
	SendCommandWithReply(commandString, mLastResponse);
	if (!mLastResponse.CommandOK())
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendGetWorkingDir
// ---------------------------------------------------------------------------

void
LFTPConnection::SendGetWorkingDir(LFTPResponse&	outResponse)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s", kFTPPrintDir);
	SendCommandWithReply(commandString, outResponse);
	if (outResponse.GetResponseCode() != kFTPPathCreated)
		throw(outResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendDeleteRemoteDir
// ---------------------------------------------------------------------------

void
LFTPConnection::SendDeleteRemoteDir(const char * inDirName)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPRemoveDir, inDirName);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPFileActionOK)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendCreateRemoteDir
// ---------------------------------------------------------------------------

void
LFTPConnection::SendCreateRemoteDir(const char * inDirName)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPMakeDir, inDirName);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPPathCreated)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendSystemRequest
// ---------------------------------------------------------------------------

void
LFTPConnection::SendSystemRequest(LFTPResponse&	outResponse)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s", kFTPSystem);
	SendCommandWithReply(commandString, outResponse);
	if (outResponse.GetResponseCode() != kFTPNameSystem)
		throw(outResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendDelete
// ---------------------------------------------------------------------------

void
LFTPConnection::SendDelete(const char * inFile)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPDelete, inFile);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPFileActionOK)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendRenameFileFrom
// ---------------------------------------------------------------------------

void
LFTPConnection::SendRenameFileFrom(const char * inFile)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPRenameFrom, inFile);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPReqOKPending)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendRenameFileTo
// ---------------------------------------------------------------------------

void
LFTPConnection::SendRenameFileTo(const char * inFile)
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s %s", kFTPRenameTo, inFile);
	SendCommandWithReply(commandString, mLastResponse);
	if (mLastResponse.GetResponseCode() != kFTPFileActionOK)
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ NoopServer
// ---------------------------------------------------------------------------

void
LFTPConnection::NoopServer()
{
	if (mSuppressNOOP) {
		return;
	}

	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	PP_CSTD::sprintf(commandString, "%s", kFTPNOOP);
	SendCommandWithReply(commandString, mLastResponse);
	if (!mLastResponse.CommandOK())
		throw(mLastResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendQUIT
// ---------------------------------------------------------------------------

void
LFTPConnection::SendQUIT()
{
	char			commandString[256];

	ThrowIfNot_(mFTPState == FTPConnected);

	mFTPState = FTPClosed;

	if (!mDisconnectReceived) {
		PP_CSTD::sprintf(commandString, "%s", kFTPQuit);
		SendCommandWithReply(commandString, mLastResponse);
		if (mLastResponse.GetResponseCode() != kFTPServiceClosing)
			throw(mLastResponse);
	}
}

// ---------------------------------------------------------------------------
//		¥ SendCommandWithReply
// ---------------------------------------------------------------------------

void
LFTPConnection::SendCommandWithReply(
							char *				inString,
							LFTPResponse&		outResponse)
{
	StMutex lockCommand(mCommandMutex);

	outResponse.ResetResponse();
	SendCmd(inString);
	WaitResponse(outResponse);
}

// ---------------------------------------------------------------------------
//		¥ SendCmd
// ---------------------------------------------------------------------------

void
LFTPConnection::SendCmd(char * inString)
{
	//Clear the receive buffer before sending the next command
	//	FTP servers send lots of junk information and we don't want to be confused
	//	about what we are reading after the command is issued.
	UInt32 unreadDataSize = mEndpoint->GetAmountUnread();
	if (unreadDataSize) {
		StPointerBlock tempBuffer((SInt32) unreadDataSize);
		mEndpoint->Receive(tempBuffer, unreadDataSize);
	}

	ThrowIf_((PP_CSTD::strlen(inString) + 2) > kMaxFTPCommandLen);

	PP_CSTD::strcat(inString, CRLF);

	mEndpoint->Send(inString, PP_CSTD::strlen(inString));
}

// ---------------------------------------------------------------------------
//		¥ HandleIncomingData
// ---------------------------------------------------------------------------

void
LFTPConnection::HandleIncomingData()
{
	LThread::Yield(mThread);
}

// ---------------------------------------------------------------------------
//		¥ WaitResponse
// ---------------------------------------------------------------------------
//ÊNote: No Try/Catch here... we want the run function to catch for us
void
LFTPConnection::WaitResponse(LFTPResponse&		outResponse)
{
	PP_STD::string dataBuffer;

	do {
		char theData[256];
		UInt32 dataSize = 255;

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
// Utility routine to test for a comlete valid response buffer
// FTP is kinda tricky because of the multi-line responses

Boolean
LFTPConnection::HaveCompleteBuffer(const char * bufferString)
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
// Utility routine to find out if we have the last line of a FTP response

Boolean
LFTPConnection::HaveLastLine(const char * bufferString) {
	Boolean haveLastLine = true;
	SInt16 i;
	char responseCode[4] = "";

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

		//Check the last line for the result code
		if (lastTok) {
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
			return false;
		}
	}

	return haveLastLine;
}

// ---------------------------------------------------------------------------
//		¥ SpendTime
// ---------------------------------------------------------------------------

void
LFTPConnection::SpendTime(const EventRecord& /* inMacEvent*/ )
{
	//Tickle the server so it doesn't drop us.
	if (mFTPState == FTPConnected) {
		if ((mTickleLastTicks + mTickleQuantum) < ::TickCount()) {
			mTickleLastTicks = ::TickCount(); //Reset last tickle counter
			//Create seperate thread to avoid calling NOOP from
			//	main thread
			LSimpleThread * NoopThread = new LSimpleThread(
					LFTPConnection::LFTPConnection_NOOPThread,
					this);
			NoopThread->Resume();
		}
	}
}

// ---------------------------------------------------------------------------
//		¥ LFTPConnection_NOOPThread
// ---------------------------------------------------------------------------
// Utility routine intended to be called from LSimpleThread that allows
//	us to NOOP the server from a thread other than the main thread durring idle time

void
LFTPConnection::LFTPConnection_NOOPThread(LThread& thread, void* arg)
{
#pragma unused (thread)
	LFTPConnection * theConnection = (LFTPConnection *)arg;
	try {
		theConnection->NoopServer();
	} catch (...) {
	}
}

PP_End_Namespace_PowerPlant
