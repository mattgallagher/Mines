// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPConnection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFTPConnection
#define _H_LFTPConnection
#pragma once

#include <LInternetProtocol.h>
#include <LFile.h>
#include <LPeriodical.h>
#include <LFTPResponse.h>
#include <LThread.h>
#include <LMutexSemaphore.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const SInt16	kFTPPort		= 21;		// The TCP port number where
											// FTP occurs by default
												
const char kFTPUser[]			= "USER";
const char kFTPPass[]			= "PASS";
const char kFTPAccount[]		= "ACCT";
const char kFTPChangeDir[]		= "CWD";
const char kFTPChangeDirUp[]	= "CDUP";
const char kFTPStructMount[]	= "SMNT";
const char kFTPReinit[]			= "REIN";
const char kFTPQuit[]			= "QUIT";

const char kFTPPortCmd[]		= "PORT";
const char kFTPPassive[]		= "PASV";
const char kFTPType[]			= "TYPE";
const char kFTPFileStruct[]		= "STRU";
const char kFTPXferMode[]		= "MODE";

const char kFTPRetrieve[]		= "RETR";
const char kFTPStore[]			= "STOR";
const char kFTPStoreUnique[]	= "STOU";
const char kFTPAppend[]			= "APPE";
const char kFTPAllocate[]		= "ALLO";
const char kFTPRestart[]		= "REST";
const char kFTPRenameFrom[]		= "RNFR";
const char kFTPRenameTo[]		= "RNTO";
const char kFTPAbort[]			= "ABOR";
const char kFTPDelete[]			= "DELE";
const char kFTPRemoveDir[]		= "RMD";
const char kFTPMakeDir[]		= "MKD";
const char kFTPPrintDir[]		= "PWD";
const char kFTPList[]			= "LIST";
const char kFTPNameList[]		= "NLST";
const char kFTPSite[]			= "SITE";
const char kFTPSystem[]			= "SYST";
const char kFTPStatus[]			= "STAT";
const char kFTPHelp[]			= "HELP";
const char kFTPNOOP[]			= "NOOP";
const char kFTPMACB[]			= "MACB";

const char kFTPAnonymous[]		= "anonymous";

enum EFTPTransferType {
	FTPUndefinedXfer = 0,
	FTPASCIIXfer = 'A',
	FTPImageXfer = 'I'
};

const SInt32 kMaxFTPCommandLen			= 513;
const SInt16 kFTPDefaultTickleSeconds	= 60;	// Number of seconds of no activity
												// to wait before sending NOOP to server
												// so we don't get disconnected.

enum FTPConnectionState {
	FTPClosed,
	FTPConnected
};

class LFTPResponse;

// ---------------------------------------------------------------------------

class LFTPConnection :	public LInternetProtocol,
						public LPeriodical {

public:
							LFTPConnection(LThread& inThread);
	virtual					~LFTPConnection();

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									const char * 		inUser = kFTPAnonymous,
									const char * 		inPassword = nil,
									const char * 		inAccount = nil,
									SInt16				inRemotePort = kFTPPort);

	virtual void 			Disconnect();

	virtual void			PutFile(
									ConstStr255Param	inRemoteHost,
									const char * 		inUser,
									const char * 		inPassword,
									const char * 		inAccount,
									EFTPTransferType	inType,
									const char * 		inFileName,
									LFile * 			inFile,
									char *				outFileName = nil,
									Boolean 			createUnique = false,
									Boolean				usePassive = false,
									SInt16				inRemotePort = kFTPPort);

	virtual void			PutFile(
									EFTPTransferType	inType,
									const char * 		inFileName,
									LFile * 			inFile,
									char *				outFileName = nil,
									Boolean 			createUnique = false,
									Boolean				usePassive = false);

	virtual void			GetFile(
									ConstStr255Param	inRemoteHost,
									const char * 		inUser,
									const char * 		inPassword,
									const char * 		inAccount,
									EFTPTransferType	inType,
									const char * 		inFileName,
									LFile * 			inFile,
									Boolean				usePassive = false,
									SInt16				inRemotePort = kFTPPort);

	virtual void			GetFile(
									EFTPTransferType	inType,
									const char * 		inFileName,
									LFile * 			inFile,
									Boolean				usePassive = false);

	virtual void			RenameRemoteFile(const char * inOldFileName, const char * inNewFileName);

	virtual void			ListFolder(
									LDynamicBuffer* outBuffer,
									const char * inDirectory = nil,
									Boolean NameList = false,
									Boolean usePassive = false);

	virtual void			NegotiateMacBinaryTransfers();

	// FTP protocol
	virtual Boolean			SendMACB();
	virtual SInt32			SendRETR(const char * inFileName);
	virtual void			SendPASV(
									LStr255&	outHostStr,
									SInt16 *	outPort);
	virtual void			SendSTOR(
									const char * inFileName,
									LFTPResponse& outResponse,
									Boolean	inUnique);
	virtual void			SendPORT(
									UInt32	HostNum,
									SInt16	Port);
	virtual void			SendTYPE(EFTPTransferType inType);
	virtual	void			SendChangeDir(const char * inDirectory);
	virtual	void			SendChangeDirUp();
	virtual void			SendGetWorkingDir(LFTPResponse&	outResponse);
	virtual	void			SendDeleteRemoteDir(const char * inDirName);
	virtual void			SendCreateRemoteDir(const char * inDirName);
	virtual	void 			SendSystemRequest(LFTPResponse&	outResponse);
	virtual	void			SendDelete(const char * inFile);
	virtual	void			SendRenameFileFrom(const char * inFile);
	virtual	void			SendRenameFileTo(const char * inFile);
	virtual void			NoopServer();
	virtual void			SendQUIT();
	virtual void			SendLIST(const char * inPath = nil, Boolean NameList = false);

	virtual	void			SpendTime(
									const EventRecord		&inMacEvent);

	virtual LFTPResponse	GetLastResponse()
							{ return mLastResponse; }

	virtual EFTPTransferType	GetTransferType() { return mTransferType; }
	virtual Boolean			GetMacBinaryNegotiated() { return mMacBinaryNegotiated; }

protected:
	virtual void			InternalPutFile(
									const char * inFileName,
									LFile * inFile,
									EFTPTransferType inType,
									char * outFileName,
									Boolean createUnique,
									SProgressMessage&	inProgressMsg,
									Boolean usePassive = false);
	virtual void			InternalGetFile(
									const char * inFileName,
									LFile * inFile,
									EFTPTransferType inType,
									SProgressMessage&	inProgressMsg,
									Boolean usePassive = false);

	virtual void			HandleIncomingData();

	virtual void			DoAuthorization(
									const char * inUser = kFTPAnonymous,
									const char * inPassword = nil,
									const char * inAccount = nil);

	virtual void			SendCmd(
									char * 				inString);
	virtual void			SendCommandWithReply(
									char * 				inString,
									LFTPResponse&		outResponse);
	virtual void			WaitResponse(
									LFTPResponse&		outResponse);

	virtual void			WaitForServerID();

	virtual void			SetTickleSeconds(UInt32 inSeconds)
									{
										mTickleQuantum = inSeconds * 60; //Quantum is in ticks
									}

	virtual UInt32			GetTickleSeconds()
									{
										return mTickleQuantum/60; //Quantum is in ticks
									}

	virtual Boolean			HaveCompleteBuffer(const char * bufferString);

	virtual Boolean			HaveLastLine(const char * bufferString);

	static void				LFTPConnection_NOOPThread(LThread& thread, void* arg);

	FTPConnectionState		mFTPState;
	LStr255					mRemoteSystem;
	UInt32					mTickleQuantum;
	UInt32					mTickleLastTicks;
	LFTPResponse			mLastResponse;				//Last FTP response from server
														// may include important info from server
	LMutexSemaphore			mCommandMutex;
	EFTPTransferType		mTransferType;
	Boolean					mMacBinaryNegotiated;
	Boolean					mSuppressNOOP;

	friend class StSupressNOOP;
};

// ---------------------------------------------------------------------------

class StSupressNOOP
{
	public:
			StSupressNOOP(LFTPConnection * inConnection)
			{ mConnection = inConnection;
			  mConnection->mSuppressNOOP = true; }
			~StSupressNOOP()
			{ mConnection->mSuppressNOOP = false; }

	protected:
		LFTPConnection * mConnection;

	friend class LFTPConnection;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
