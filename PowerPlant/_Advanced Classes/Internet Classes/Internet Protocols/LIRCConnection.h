// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIRCConnection.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LIRCConnection
#define _H_LIRCConnection
#pragma once

#include <LBroadcaster.h>
#include <LInternetProtocolAsync.h>
#include <LIRCResponse.h>
#include <IRC_Constants.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

const SInt16	kIRCPort		= 6667;			// the TCP port number where
												// IRC occurs by default

enum IRCConnectionState {
	IRCClosed,
	IRCConnected
};

const SInt32	kIRCTimeOffset		= 2082844800;
const char		kIRCMsgCommandChar	= 0x01;

#define	IRC_ThrowIfTooLong_(length)									\
	do {															\
		if (length > kMaxIRCCommandLen) Throw_(err_AssertFailed);	\
	} while (false)

// ===========================================================================

#pragma mark === LIRCConnection ===

// ===========================================================================
//		¥ LIRCConnection
// ===========================================================================

class LIRCReceiveThread;
class LIRCThread;

class LIRCConnection : public LInternetProtocolAsync,
						public LBroadcaster {

public:
							LIRCConnection();
	virtual					~LIRCConnection();

	virtual void 			Connect(
									ConstStr255Param	inRemoteHost,
									ConstStr255Param	inNickname,
									ConstStr255Param	inRealname,
									SInt16				inRemotePort = kIRCPort,
									ConstStr255Param	inPassword = "\p",
									ConstStr255Param	inUsername = "\p");

	virtual void 			Disconnect();

	//Accessors
	virtual const char * 	GetNickname() { return mNickname.c_str(); }
	virtual const char * 	GetRealname() { return mRealname.c_str(); }
	virtual const char * 	GetPassword() { return mPassword.c_str(); }
	virtual const char * 	GetUsername() { return mUsername.c_str(); }

	//Outgoing Commands
	//Connection regsitration
	virtual void 			SendPASS(const char * inPassword);
	virtual void 			SendNICK(const char * inNickname);
	virtual void 			SendUSER(const char * inUserName,
										const char * inServerName,
										const char * inRealName);
	virtual void 			SendOPER(const char * inUserName, const char * inPassword);
	virtual void 			SendQUIT(const char * inQuitMessage = nil);

	//Channel operations
	virtual void 			SendJOIN(const char * inChannelName, const char * inKey = "");
	virtual void 			SendPART(const char * inChannelName);
	virtual void 			SendMODE(const char * inChannelNick,
										IRCModeFlag inMode,
										Boolean inStateIsOn,
										const char * inParam = "");
	virtual void			SendMODERequest(const char * inChannelNick);
	virtual void 			SendTOPIC(const char * inChannelName,
										const char * inTopic);
	virtual void 			SendNAMES(const char * inChannelName);
	virtual void 			SendLIST(const char * inChannelName = "");
	virtual void 			SendINVITE(const char * inChannelName,
										const char * inNickName);
	virtual void 			SendKICK(const char * inChannelName,
										const char * inNickName,
										const char * inComment);

	//Server queries and commands
	virtual void 			SendVERSION(const char * inServerName = "");
	virtual void 			SendSTATS(IRCSTATQuery inQuery,
										const char * inServerName = "");
	virtual void 			SendLINKS(const char * inServerName = "",
										const char * inServerMask = "");
	virtual void 			SendTIME(const char * inServerName = "");
	virtual void 			SendCONNECT(const char * inServerName,
										const char * inPort,
										const char * inRemoteServer);
	virtual void 			SendTRACE(const char * inServerName);
	virtual void 			SendADMIN(const char * inServerName = "");
	virtual void 			SendINFO(const char * inServerName = "");

	//Sending messages
	virtual void 			SendPRIVMSG(const char * inRecipient,
										const char * inMessage);
	virtual void 			SendNOTICE(const char * inRecipient,
										const char * inMessage);

	//User based queries
	virtual void 			SendWHO(const char * inNickName = "", Boolean OperatorsOnly = false);
	virtual void 			SendWHOIS(const char * inNickMask,
										const char * inServerName = nil);
	virtual void 			SendWHOWAS(const char * inNickname,
										const char * inServerName = nil,
										UInt16 inCount = 0);

	//Miscellaneous messages
	virtual void 			SendKILL(const char * inNickname,
										const char * inComment);
	virtual void			SendPONG(const char * inPingFrom);

	//Optionals
	virtual void 			SendAWAY(const char * inComment = nil);
	virtual void 			SendREHASH();
	virtual void 			SendRESTART();
	virtual void 			SendSUMMON(const char * inUser,
										const char * inServer = nil);
	virtual void 			SendUSERS(const char * inServer = nil);
	virtual void 			SendUSERHOST(const char * inNicknameList);
	virtual void 			SendISON(const char * inNicknameList);

protected:
	//LInternetProtocolAsync Overrides
	virtual void 			HandleConnect();
	virtual void 			HandleDisconnect();
	virtual void 			HandleIncomingData(const char * theData, UInt32 theLength);

	//IRC Command Handlers
	virtual void 			HandlePrivmsgCommand(LIRCResponse * inMessage);
	virtual void 			HandleJoinCommand(LIRCResponse * inMessage);
	virtual void 			HandlePartCommand(LIRCResponse * inMessage);
	virtual void 			HandleQuitCommand(LIRCResponse * inMessage);
	virtual void 			HandleNoticeCommand(LIRCResponse * inMessage);
	virtual void 			HandleTopicCommand(LIRCResponse * inMessage);
	virtual void 			HandleInviteCommand(LIRCResponse * inMessage);
	virtual void 			HandleKickCommand(LIRCResponse * inMessage);
	virtual void 			HandleNickCommand(LIRCResponse * inMessage);
	virtual void 			HandleSQuitCommand(LIRCResponse * inMessage);
	virtual void 			HandleObjectCommand(LIRCResponse * inMessage);
	virtual void 			HandleModeCommand(LIRCResponse * inMessage);
	virtual void			HandlePingCommand(LIRCResponse * inMessage);

	//IRC Error Handlers
	virtual void 			HandleNoParamError(LIRCResponse * inMessage);
	virtual void 			HandleOneParamError(LIRCResponse * inMessage);
	virtual void 			HandleTwoParamError(LIRCResponse * inMessage);

	//IRC Response Handlers
	virtual void 			HandleUserHostReply(LIRCResponse * inMessage);
	virtual void 			HandleIsonReply(LIRCResponse * inMessage);
	virtual void 			HandleAwayReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisUserReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisServerReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisOperatorReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisIdleReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisEndReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoisChannelsReply(LIRCResponse * inMessage);
	virtual void 			HandleWhowasUserReply(LIRCResponse * inMessage);
	virtual void 			HandleWhowasEndReply(LIRCResponse * inMessage);
	virtual void 			HandleListReply(LIRCResponse * inMessage);
	virtual void 			HandleChannelmodeisReply(LIRCResponse * inMessage);
	virtual void 			HandleUmodeisReply(LIRCResponse * inMessage);
	virtual void 			HandleTopicReply(LIRCResponse * inMessage);
	virtual void 			HandleInviteReply(LIRCResponse * inMessage);
	virtual void 			HandleSummonReply(LIRCResponse * inMessage);
	virtual void 			HandleVersionReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoReply(LIRCResponse * inMessage);
	virtual void 			HandleWhoEndReply(LIRCResponse * inMessage);
	virtual void 			HandleNamesReply(LIRCResponse * inMessage);
	virtual void 			HandleNamesEndReply(LIRCResponse * inMessage);
	virtual void 			HandleLinksReply(LIRCResponse * inMessage);
	virtual void 			HandleLinksEndReply(LIRCResponse * inMessage);
	virtual void 			HandleBanlistReply(LIRCResponse * inMessage);
	virtual void 			HandleBanlistEndReply(LIRCResponse * inMessage);
	virtual void 			HandleRehashReply(LIRCResponse * inMessage);
	virtual void 			HandleTimeReply(LIRCResponse * inMessage);
	virtual void 			HandleUsersReply(LIRCResponse * inMessage);
	virtual void 			HandleAdminMeReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceLinkReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceConnectingReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceHandshakeReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceUnknownReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceOperatorReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceUserReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceServerReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceNewTypeReply(LIRCResponse * inMessage);
	virtual void 			HandleTraceLogReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsLinkInfoReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsCommandsReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsCLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsNLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsILineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsKLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsYLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsEndReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsLLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsOLineReply(LIRCResponse * inMessage);
	virtual void 			HandleStatsHLineReply(LIRCResponse * inMessage);
	virtual void 			HandleLUserOPReply(LIRCResponse * inMessage);
	virtual void 			HandleLUserUnknownReply(LIRCResponse * inMessage);
	virtual void 			HandleLUserChannelsReply(LIRCResponse * inMessage);

	//Undocumented
	virtual void 			HandleCreationTimeReply(LIRCResponse * inMessage);
	virtual void 			HandleTopicInformationReply(LIRCResponse * inMessage);

	//Misc. Internals
	virtual void			HandleMessage(LIRCResponse * inMessage);
	virtual void			HandleCommand(LIRCResponse * inMessage);

	virtual void			SendCmd(char * inString);
	virtual inline Boolean	IRCMessageIs(LIRCResponse * msg, const char * command);
	virtual Boolean			ParseNick(const char * inPrefix,
										char * outNick,
										char * outExtension);
	virtual void			IRCTimeToDateTime(const char * inIRCTime, DateTimeRec * outDateTime);
	virtual void			BroadcastResponseParams(MessageT inBroadcastMessage,
													LIRCResponse * inMessage);

	IRCConnectionState		mIRCState;
	Str255					mLocalAddress;

	LDynamicBuffer			mLocalDataBuffer;

	PP_STD::string			mNickname;
	PP_STD::string			mRealname;
	PP_STD::string			mPassword;
	PP_STD::string			mUsername;

friend class LIRCReceiveThread;
friend class LIRCThread;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
