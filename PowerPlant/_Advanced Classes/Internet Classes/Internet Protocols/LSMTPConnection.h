// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSMTPConnection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSMTPConnection
#define _H_LSMTPConnection
#pragma once

#include <LInternetProtocol.h>
#include <LMailMessage.h>
#include <LSMTPResponse.h>
#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const SInt16		kSMTPPort		= 25;		// the TCP port number where
												// SMTP occurs by default
const char kSMTPHelo[]		= "HELO";
const char kSMTPMail[]		= "MAIL FROM";
const char kSMTPRecipient[]	= "RCPT TO";
const char kSMTPData[]		= "DATA";
const char kSMTPReset[]		= "RSET";
const char kSMTPQuit[]		= "QUIT";

const SInt32 kSMTPServReady	= 220;
const SInt32 kSMTPServClose	= 221;
const SInt32 kSMTPRequestOK	= 250;
const SInt32 kSMTPNotLocal	= 251;
const SInt32 kSMTPStartMail	= 354;

const SInt16 kMaxSMTPCommandLen = 513;
const SInt32 kDefaultEncodeBufferLength = 10000;

enum SMTPConnectionState {
	SMTPClosed,
	SMTPConnected
};

// ---------------------------------------------------------------------------

class LSMTPConnection : public LInternetProtocol {

public:
							LSMTPConnection(LThread& inThread);
	virtual					~LSMTPConnection();

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									SInt16				inRemotePort = kSMTPPort);

	virtual void 			Disconnect();

	// mail sending
	virtual void			SendMessages(
									ConstStr255Param	inRemoteHost,
									LInternetMessageList *	inMessages,
									SInt16				inRemotePort = kSMTPPort);
	virtual void			SendOneMessage(
									ConstStr255Param	inRemoteHost,
									LMailMessage&		theMessage,
									SInt16				inRemotePort = kSMTPPort);

	SMTPConnectionState		GetServerState()
							{
								return mSMTPState;
							}
	// SMTP protocol

protected:
	virtual void			SendCmd(
									char *		inString);
	virtual void			SendCommandWithReply(
									char *		inString,
									LSMTPResponse&		outResponse);
	virtual void			WaitResponse(
									LSMTPResponse&		outResponse);

	virtual void			WaitForServerID();
	virtual void			SendHELO();
	virtual void			SendQUIT();

	virtual void			InternalSendMessage(
									LMailMessage&		inMessage,
									SProgressMessage&	inProgressMsg);

	virtual void			SendRSET();
	virtual void			SendMAIL(
									LMailMessage&		inMessage);
	virtual void			SendRCPT(LArray * theList);
	virtual void			SendRCPT(const char * theRecipient);
	virtual void			SendDATACmd();
	virtual void			SendMessageData(
									LMailMessage&		inMessage,
									SProgressMessage&	inProgressMsg);
	virtual void			SendSMTPEncoded(LDynamicBuffer * inData,
											SProgressMessage& inProgressMsg,
											Boolean isEndOfData = true);

	virtual Boolean 		HaveCompleteBuffer(const char * bufferString);
	virtual Boolean			HaveLastLine(const char * bufferString);

	SMTPConnectionState		mSMTPState;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
