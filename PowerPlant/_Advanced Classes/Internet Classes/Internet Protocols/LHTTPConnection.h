// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPConnection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LHTTPConnection
#define _H_LHTTPConnection
#pragma once

#include <LInternetProtocol.h>
#include <LHTTPMessage.h>
#include <LHTTPResponse.h>
#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const SInt16		kHTTPPort		= 80;		// The TCP port number where
												// HTTP occurs by default
const SInt16 kMaxHTTPResponseLen	= 513;
const SInt16 kHTTPRequestOK			= 200;

const char kHTTPVersion[]	= "HTTP/1.0";
const char kHTTPGet[]		= "GET";
const char kHTTPHead[]		= "HEAD";
const char kHTTPPost[]		= "POST";
const char kHTTPPut[]		= "PUT";


enum EHTTPMethod{
	HTTPGet,
	HTTPHead,
	HTTPPost,
	HTTPPut
};

enum HTTPConnectionState {
	HTTPClosed,
	HTTPConnected
};

// ---------------------------------------------------------------------------

class LHTTPConnection : public LInternetProtocol {

public:
							LHTTPConnection(LThread& inThread);
	virtual					~LHTTPConnection();

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									SInt16				inRemotePort = kHTTPPort);

	virtual void 			Disconnect();

	// HTTP protocol
	virtual void			RequestResource(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									EHTTPMethod			inMethod = HTTPGet,
									SInt16				inRemotePort = kHTTPPort);

	virtual void			Get(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SInt16				inRemotePort = kHTTPPort);

	virtual void			Head(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SInt16				inRemotePort = kHTTPPort);
	virtual void			Post(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SInt16				inRemotePort = kHTTPPort);

	virtual void			Put(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SInt16				inRemotePort = kHTTPPort);

	virtual void			SetCheckContentLength(Boolean inCheck)
								{ mCheckContentLength = inCheck; }

	virtual Boolean			GetCheckContentLength()
								{ return mCheckContentLength; }

protected:
	virtual void			DoHTTPMethod(
									EHTTPMethod			inMethod,
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inRequest,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SInt16				inRemotePort);

	virtual void			SendCommandWithReply(
									const char * 		inString,
									LHTTPMessage&		inMessage,
									LHTTPResponse&		outResponse,
									SProgressMessage&	inProgressMsg);
	virtual void			WaitResponse(
									LHTTPResponse& outResponse,
									SProgressMessage&	inProgressMsg);


	HTTPConnectionState		mHTTPState;

	Boolean					mCheckContentLength;
	Boolean					mCheckedHeader;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
