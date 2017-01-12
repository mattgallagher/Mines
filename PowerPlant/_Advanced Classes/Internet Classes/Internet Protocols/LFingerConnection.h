// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFingerConnection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFingerConnection
#define _H_LFingerConnection
#pragma once

#include <LInternetProtocol.h>
#include <LFingerResponse.h>
#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

const SInt16	kFingerPort		= 79;			// The TCP port number where
												// Finger occurs by default
const char kFingerVerboseFlag[] = "/W ";

enum FingerConnectionState {
	FingerClosed,
	FingerConnected
};

// ---------------------------------------------------------------------------

class LFingerConnection : public LInternetProtocol {
public:

	static const SInt16	kSingleRequestOverhead = 5;			// = "/W" + SPACE + CRLF

							LFingerConnection(LThread& inThread);
	virtual					~LFingerConnection();

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									SInt16				inRemotePort = kFingerPort);

	virtual void 			Disconnect();

	virtual void 			GetUsers(
									ConstStr255Param	inRemoteHost,
									LFingerResponse&	outResponse,
									Boolean 			inVerbose = false,
									SInt16				inRemotePort = kFingerPort);

	virtual void			GetOneUser(
									ConstStr255Param	inRemoteHost,
									const char *		inUserRequest,
									LFingerResponse&	outResponse,
									Boolean 			inVerbose = false,
									SInt16				inRemotePort = kFingerPort);

	virtual void			InternalRequestAllUsers(
									LFingerResponse&	outResponse,
									SProgressMessage&	inProgressMsg,
									Boolean 			inVerbose = false);

	virtual void			InternalRequestUser(
									const char *		inUserRequest,
									LFingerResponse&	outResponse,
									SProgressMessage&	inProgressMsg,
									Boolean 			inVerbose = false);
protected:
	virtual void			SendCmd(
									char * 				inString);
	virtual void			SendCommandWithReply(
									char * 				inString,
									LFingerResponse&	outResponse,
									SProgressMessage&	inProgressMsg);
	virtual void			WaitResponse(
									LFingerResponse&		outResponse,
									SProgressMessage&	inProgressMsg);
private:
	FingerConnectionState	mFingerState;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
