// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTCPEndpoint.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTCPEndpoint
#define _H_LTCPEndpoint
#pragma once

#include <LEndpoint.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LTCPEndpoint											[abstract]
// ===========================================================================
//	An abstract base class for TCP/IP endpoints. TCP/IP is the session-
//	oriented protocol of the Internet, and is the layer upon which most of
//	the familiar Internet protocols (HTTP, FTP, SMTP, etc.) are built.
//
//	In PowerPlant, there are two subclasses of LTCPEndpoint, named
//	LMacTCPTCPEndpoint and LOpenTptTCPEndpoint. The appropriate class is
//	created automatically when you call UNetworkFactory::CreateTCPEndpoint.

class LTCPEndpoint : public LEndpoint{

public:
								LTCPEndpoint();
	virtual						~LTCPEndpoint();

	// connection establishment (clients)

	virtual void				Connect(
										LInternetAddress&	inRemoteAddress,
										UInt32				inTimeoutSeconds = Timeout_None) = 0;
	virtual void				Disconnect() = 0;
	virtual void				SendDisconnect() = 0;
	virtual void				AbortiveDisconnect() = 0;
	virtual void				AcceptRemoteDisconnect() = 0;

	virtual LInternetAddress*	GetRemoteHostAddress() = 0;

	// passive connections (servers)

	virtual void				Listen() = 0;
	virtual void				AcceptIncoming(LTCPEndpoint* 	inEndpoint) = 0;
	virtual void				RejectIncoming() = 0;

	// sending data

	virtual void				Send(
										void*					inData,
										UInt32					inDataSize)
								{
									SendData(inData, inDataSize, false);
								}

	virtual void				SendData(
										void*					inData,
										UInt32					inDataSize,
										Boolean					inExpedited = false,
										UInt32					inTimeoutSeconds = Timeout_None) = 0;

	virtual void				SendPStr(	ConstStringPtr			inString);
	virtual void				SendCStr(	char*					inString);
	virtual void				SendHandle(	Handle					inHandle);
	virtual void				SendPtr(	Ptr						inPtr);

	// receiving data

	virtual void				Receive(
										void*					outDataBuffer,
										UInt32&					ioDataSize)
								{
									Boolean tempExp;
									ReceiveData(outDataBuffer, ioDataSize, tempExp);
								}

	virtual void				ReceiveData(
										void*					outDataBuffer,
										UInt32&					ioDataSize,
										Boolean&				outExpedited,
										UInt32					inTimeoutSeconds = Timeout_None) = 0;

	virtual Boolean				ReceiveDataUntilMatch(
										void*					outDataBuffer,
										UInt32&					ioDataSize,
										Boolean&				outExpedited,
										UInt32					inTimeoutSeconds = Timeout_None,
										char					inMatchChar = 0x0D);

	virtual Boolean				ReceiveLine(
										char *					outString,
										UInt32&					ioDataSize,
										UInt32					inTimeoutSeconds = Timeout_None,
										Boolean					inUseLF = false);

	virtual Boolean				ReceiveChar(
										char&					outChar,
										UInt32					inTimeoutSeconds = Timeout_None);
		// etc.

	// receive configuration

	virtual UInt32				GetAmountUnread() = 0;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
