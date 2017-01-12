// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPOP3Connection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPOP3Connection
#define _H_LPOP3Connection
#pragma once

#include <LInternetProtocol.h>
#include <LMailMessage.h>
#include <LPOP3Response.h>
#include <LThread.h>
#include <LMutexSemaphore.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const SInt16		kPOP3Port		= 110;		// the TCP port number where
												// POP3 occurs by default
const char kPOP3User[]	= "USER";
const char kPOP3Pass[]	= "PASS";
const char kPOP3Stat[]	= "STAT";
const char kPOP3List[]	= "LIST";
const char kPOP3Retr[]	= "RETR";
const char kPOP3Dele[]	= "DELE";
const char kPOP3NOOP[]	= "NOOP";
const char kPOP3Rset[]	= "RSET";
const char kPOP3UIDL[]	= "UIDL";
const char kPOP3Top[]	= "TOP";
const char kPOP3APOP[]	= "APOP";
const char kPOP3Quit[]	= "QUIT";

const SInt16 kMaxPOP3CommandLen = 256;
const SInt16 kMaxPOP3ResponseLen = 513;
const SInt16 kPOP3DefaultTickleSeconds = 60;	// Number of seconds of no activity
												// to wait before sending NOOP to server
												// so we don't get disconnected.

typedef struct POP3ListElem {
	UInt32		msgNumber;
	UInt32		msgSize;
	char		msgUIDL[71];
} POP3ListElem;

enum POP3ConnectionState {
	POP3Closed,
	POP3Connected
};

	// These track the state of the server...
	// some commands only allowed in certain server states.
enum POP3ServerState {
	POP3Offline,
	POP3Authorization,
	POP3Transaction,
	POP3Update
};

// ---------------------------------------------------------------------------

class LPOP3Connection : public LInternetProtocol,
						public LPeriodical {

public:
							LPOP3Connection(LThread& inThread);
	virtual					~LPOP3Connection();

	POP3ConnectionState		GetConnectionState()
							{
								return mPOP3State;
							}

	POP3ServerState			GetServerState()
							{
								return mPOP3ServerState;
							}

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inUser,
									ConstStr255Param	inPassword,
									Boolean 			inAPOP = false,
									SInt16				inRemotePort = kPOP3Port);

	virtual void 			GetOneMessage(
								ConstStr255Param	inRemoteHost,
								ConstStr255Param	inUser,
								ConstStr255Param	inPassword,
								UInt32				inMsgNumber,
								LMailMessage *		outMessage,
								Boolean				inAPOP = false,
								Boolean				inDelete = false,
								SInt16				inRemotePort = kPOP3Port);

	virtual void 			GetMessages(
								ConstStr255Param	inRemoteHost,
								ConstStr255Param	inUser,
								ConstStr255Param	inPassword,
								LInternetMessageList *	outMessageList,
								Boolean				inAPOP = false,
								Boolean				inDelete = false,
								SInt16				inRemotePort = kPOP3Port);

	virtual void 			GetHeaders(
								ConstStr255Param	inRemoteHost,
								ConstStr255Param	inUser,
								ConstStr255Param	inPassword,
								LInternetMessageList *	outMessageList,
								Boolean				inAPOP = false,
								SInt16				inRemotePort = kPOP3Port);

	virtual void 			Disconnect();

	virtual void			SetCheckContentLength(Boolean inCheck)
								{ mCheckContentLength = inCheck; }

	virtual Boolean			GetCheckContentLength()
								{ return mCheckContentLength; }

	virtual void			SendQUIT();
	virtual void			DeleteMessage(UInt32 inMsgNumber);
	virtual void			ResetServer(LPOP3Response * outCmdResponse);
	virtual void			ServerStatus(UInt32 * outCount, UInt32 * outSize);
	virtual void			NoopServer();
	virtual void			GetMailMessage(
									UInt32 inMsgNumber,
									LPOP3Response& outResponse,
									LMailMessage&		outMessage,
									SProgressMessage&	inProgressMsg);
	virtual void			GetTop(
									UInt32 inMsgNumber,
									UInt32 inMsgLines,
									LMailMessage&	outMessage,
									LPOP3Response&	outResponse);
	virtual UInt32			DoLIST(LArray * outList);
	virtual void			DoLIST(UInt32 inMsgNumber, POP3ListElem& outListElem);
	virtual UInt32			DoUIDL(LArray * outList);
	virtual void			DoUIDL(UInt32 inMsgNumber, POP3ListElem& outListElem);
	virtual void			CollectAllMessages(
									LInternetMessageList * outList,
									Boolean deleteAsRetrieve,
									SProgressMessage& inProgressMsg);
	virtual void			CollectAllHeaders(
									LInternetMessageList * outList,
									SProgressMessage& inProgressMsg);

	virtual	void			SpendTime(
									const EventRecord		&inMacEvent);

protected:
	virtual void			SendCmd(
									char * 				inString);
	virtual void			SendCommandWithReply(
									char * 				inString,
									LPOP3Response&		outResponse);
	virtual void			SendCommandWithMultiReply(
									char * 				inString,
									LPOP3Response&		outResponse,
									SProgressMessage *	inProgressMsg = nil);
	virtual void			SendCommandWithMailReply(
									char * 				inString,
									LPOP3Response&		outResponse,
									LMailMessage&		outMessage,
									SProgressMessage *	inProgressMsg);

	virtual void			WaitResponse(
									LPOP3Response&		outResponse);
	virtual void			WaitMultiResponse(
									LPOP3Response& outResponse,
									SProgressMessage *	inProgressMsg);

	virtual void			WaitMailResponse(LPOP3Response& 		outResponse,
									LMailMessage&		outMessage,
									SProgressMessage *	inProgressMsg);

	virtual void			WaitForServerID();

	virtual void			DoAuthorization(
									ConstStr255Param inUser,
									ConstStr255Param inPassword,
									Boolean inAPOP);

	virtual void			SetTickleSeconds(UInt32 inSeconds)
									{
										mTickleQuantum = inSeconds * 60; //Quantum is in ticks
									}

	virtual UInt32			GetTickleSeconds()
									{
										return mTickleQuantum/60; //Quantum is in ticks
									}

	virtual Boolean 		HaveCompleteMultiBuffer(LDynamicBuffer* dataBuffer);

	static void				LPOP3Connection_NOOPThread(LThread& thread, void* arg);

	POP3ConnectionState		mPOP3State;
	POP3ServerState			mPOP3ServerState;
	Boolean					mNoAPOP;					//Used if we hit a problem with APOP
	char					mServerTimeStamp[128];
	UInt32					mTickleQuantum;
	UInt32					mTickleLastTicks;
	Boolean					mCheckContentLength;
	LMutexSemaphore			mCommandMutex;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
