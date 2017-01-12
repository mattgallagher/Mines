// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIRCConnection.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// IRC Connection

#include <LIRCConnection.h>
#include <LInternetAddress.h>
#include <UInternet.h>
#include <LArrayIterator.h>
#include <cstdlib>
#include <cstdio>

#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LIRCConnection ===

// ---------------------------------------------------------------------------
//		• LIRCConnection()
// ---------------------------------------------------------------------------
//	Constructor

LIRCConnection::LIRCConnection()
{
	*mLocalAddress = 0;
	mIRCState = IRCClosed;
}

// ---------------------------------------------------------------------------
//		• ~LIRCConnection
// ---------------------------------------------------------------------------
//	Destructor

LIRCConnection::~LIRCConnection()
{
}

// ---------------------------------------------------------------------------
//		• Connect
// ---------------------------------------------------------------------------
// Opens TCP/IP connection and negotiates login with server

void
LIRCConnection::Connect(
	ConstStr255Param	inRemoteHost,
	ConstStr255Param	inNickname,
	ConstStr255Param	inRealname,
	SInt16				inRemotePort,
	ConstStr255Param	inPassword,
	ConstStr255Param	inUsername)
{
	if (mIRCState == IRCConnected)
		return;

	mNickname.assign((const char*)&inNickname[1], inNickname[0]);
	mRealname.assign((const char*)&inRealname[1], inRealname[0]);
	mPassword.assign((const char*)&inPassword[1], inPassword[0]);
	mUsername.assign((const char*)&inUsername[1], inUsername[0]);

	LInternetProtocolAsync::Connect(inRemoteHost, (UInt16) inRemotePort);
}

// ---------------------------------------------------------------------------
//		• Disconnect
// ---------------------------------------------------------------------------

void
LIRCConnection::Disconnect()
{
	if (mIRCState != IRCClosed) {
		SendQUIT();
		LInternetProtocolAsync::Disconnect();
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === LInternetProtocolAsync - Overrides ===


// ---------------------------------------------------------------------------
//		• HandleConnect
// ---------------------------------------------------------------------------
// Do login here

void
LIRCConnection::HandleConnect()
{
	//Get the local address (used primarily by USER)
	LInternetAddress * myAddress = GetLocalAddress();
	myAddress->GetIPAddress(mLocalAddress);
	delete myAddress;

	// Negotiate login
	if (mPassword.length() > 0) {
		SendPASS(mPassword.c_str());
	}

	SendNICK(mNickname.c_str());

	if (mUsername.length() > 0)	{
		SendUSER(mUsername.c_str(), GetRemoteHost(), mRealname.c_str());

	} else {
		//Create a username from the real name
		PP_STD::string tempUserName(mRealname);
		SInt16 realnamelen = (SInt16) mRealname.length();
		for (SInt16 i = 0; i < realnamelen; i++) {
			if (PP_CSTD::isspace(tempUserName[i])) {
				tempUserName[i] = '_';
			} else {
				tempUserName[i] = (char) PP_CSTD::tolower(tempUserName[i]);
			}
		}

		SendUSER(tempUserName.c_str(), GetRemoteHost(), mRealname.c_str());
	}

	mIRCState = IRCConnected;
}

// ---------------------------------------------------------------------------
//		• HandleDisconnect
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleDisconnect()
{
	mIRCState = IRCClosed;
	BroadcastMessage(msg_IRC_Disconnected, this);
}

// ---------------------------------------------------------------------------
//		• HandleIncomingData
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleIncomingData(const char * theData, UInt32 theLength)
{
	LIRCResponse theResponse;

	mLocalDataBuffer.ConcatenateBuffer(theData, (SInt32) theLength);

	StHandleBlock dataH(Size_Zero);
	ThrowIfNot_(mLocalDataBuffer.BufferToHandle(dataH, 0, true));
	StHandleLocker locked(dataH);
	char * data = *dataH;

	char * nextLineEnds;
	nextLineEnds = PP_CSTD::strstr(data, CRLF);
	while (nextLineEnds) {
		char tempCommmand[kMaxIRCCommandLen + 1];
		PP_CSTD::strncpy(tempCommmand, data, (UInt32) (nextLineEnds - data));
		tempCommmand[nextLineEnds - data] = '\0';

		theResponse.SetResponse(tempCommmand);
		HandleMessage(&theResponse);

		nextLineEnds += 2;	//walk past the CRLF
		data = nextLineEnds;
		nextLineEnds = PP_CSTD::strstr(data, CRLF);
	}

	//Write what's left back to the buffer for the next go arround
	mLocalDataBuffer.SetBuffer(data);
}

// ===========================================================================

#pragma mark -
#pragma mark === Message Handling ===

// ---------------------------------------------------------------------------
//		• HandleMessage
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleMessage(LIRCResponse * inMessage)
{
	switch (inMessage->GetResponseCode()) {
		case IRC_UNDEFINED:
			HandleCommand(inMessage);
			break;

		case ERR_NOORIGIN:
		case ERR_NORECIPIENT:
		case ERR_NOTEXTTOSEND:
		case ERR_NOMOTD:
		case ERR_FILEERROR:
		case ERR_NONICKNAMEGIVEN:
		case ERR_SUMMONDISABLED:
		case ERR_USERSDISABLED:
		case ERR_NOTREGISTERED:
		case ERR_ALREADYREGISTRED:
		case ERR_NOPERMFORHOST:
		case ERR_PASSWDMISMATCH:
		case ERR_YOUREBANNEDCREEP:
		case ERR_NOPRIVILEGES:
		case ERR_CANTKILLSERVER:
		case ERR_NOOPERHOST:
		case ERR_UMODEUNKNOWNFLAG:
		case ERR_USERSDONTMATCH:
			HandleNoParamError(inMessage);
			break;

		case ERR_NOSUCHNICK:
		case ERR_NOSUCHSERVER:
		case ERR_NOSUCHCHANNEL:
		case ERR_CANNOTSENDTOCHAN:
		case ERR_TOOMANYCHANNELS:
		case ERR_WASNOSUCHNICK:
		case ERR_TOOMANYTARGETS:
		case ERR_NOTOPLEVEL:
		case ERR_WILDTOPLEVEL:
		case ERR_UNKNOWNCOMMAND:
		case ERR_NOADMININFO:
		case ERR_ERRONEUSNICKNAME:
		case ERR_NICKNAMEINUSE:
		case ERR_NICKCOLLISION:
		case ERR_NOTONCHANNEL:
		case ERR_USERONCHANNEL:
		case ERR_NOLOGIN:
		case ERR_NEEDMOREPARAMS:
		case ERR_KEYSET:
		case ERR_CHANNELISFULL:
		case ERR_UNKNOWNMODE:
		case ERR_INVITEONLYCHAN:
		case ERR_BANNEDFROMCHAN:
		case ERR_BADCHANNELKEY:
		case ERR_CHANOPRIVSNEEDED:
			HandleOneParamError(inMessage);
			break;

		case ERR_USERNOTINCHANNEL:
			HandleTwoParamError(inMessage);
			break;

		case RPL_NONE:
			BroadcastResponseParams(msg_IRC_ReplyNone, inMessage);
			break;

		case RPL_USERHOST:
			HandleUserHostReply(inMessage);
			break;

		case RPL_ISON:
			HandleIsonReply(inMessage);
			break;

		//Away responses
		case RPL_AWAY:
			HandleAwayReply(inMessage);
			break;
		case RPL_UNAWAY:
			BroadcastResponseParams(msg_IRC_UnawayReply, inMessage);
			break;
		case RPL_NOWAWAY:
			BroadcastResponseParams(msg_IRC_NowawayReply, inMessage);
			break;

		//Whois responses
		case RPL_WHOISUSER:
			HandleWhoisUserReply(inMessage);
			break;
		case RPL_WHOISSERVER:
			HandleWhoisServerReply(inMessage);
			break;
		case RPL_WHOISOPERATOR:
			HandleWhoisOperatorReply(inMessage);
			break;
		case RPL_WHOISIDLE:
			HandleWhoisIdleReply(inMessage);
			break;
		case RPL_ENDOFWHOIS:
			HandleWhoisEndReply(inMessage);
			break;
		case RPL_WHOISCHANNELS:
			HandleWhoisChannelsReply(inMessage);
			break;

		//Whowas responses
		case RPL_WHOWASUSER:
			HandleWhowasUserReply(inMessage);
			break;
		case RPL_ENDOFWHOWAS:
			HandleWhowasEndReply(inMessage);
			break;

		//List responses
		case RPL_LISTSTART:
			BroadcastResponseParams(msg_IRC_ListStartReply, inMessage);
			break;
		case RPL_LIST:
			HandleListReply(inMessage);
			break;
		case RPL_LISTEND:
			BroadcastResponseParams(msg_IRC_ListEndReply, inMessage);
			break;

		//Channelmodeis response
		case RPL_CHANNELMODEIS:
			HandleChannelmodeisReply(inMessage);
			break;
		//Usermodeis response
		case RPL_UMODEIS:
			HandleUmodeisReply(inMessage);
			break;

		//Topic responses
		case RPL_NOTOPIC:
			BroadcastResponseParams(msg_IRC_NoTopicReply, inMessage);
			break;
		case RPL_TOPIC:
			HandleTopicReply(inMessage);
			break;

		//Invite
		case RPL_INVITING:
			HandleInviteReply(inMessage);
			break;

		//Summon
		case RPL_SUMMONING:
			HandleSummonReply(inMessage);
			break;

		//Version
		case RPL_VERSION:
			HandleVersionReply(inMessage);
			break;

		//Who responses
		case RPL_WHOREPLY:
			HandleWhoReply(inMessage);
			break;
		case RPL_ENDOFWHO:
			HandleWhoEndReply(inMessage);
			break;

		//Names responses
		case RPL_NAMREPLY:
			HandleNamesReply(inMessage);
			break;
		case RPL_ENDOFNAMES:
			HandleNamesEndReply(inMessage);
			break;

		//Links responses
		case RPL_LINKS:
			HandleLinksReply(inMessage);
			break;
		case RPL_ENDOFLINKS:
			HandleLinksEndReply(inMessage);
			break;

		//Banlist responses
		case RPL_BANLIST:
			HandleBanlistReply(inMessage);
			break;
		case RPL_ENDOFBANLIST:
			HandleBanlistEndReply(inMessage);
			break;

		//Info responses
		case RPL_INFO:
			BroadcastResponseParams(msg_IRC_InfoReply, inMessage);
			break;
		case RPL_ENDOFINFO:
			BroadcastResponseParams(msg_IRC_InfoEndReply, inMessage);
			break;

		//Message of the Day
		case RPL_MOTDSTART:
			BroadcastResponseParams(msg_IRC_MOTDStartReply, inMessage);
			break;
		case RPL_MOTD:
			BroadcastResponseParams(msg_IRC_MOTDReceiveReply, inMessage);
			break;
		case RPL_ENDOFMOTD:
			BroadcastResponseParams(msg_IRC_MOTDCompleteReply, inMessage);
			break;

		//Oper response
		case RPL_YOUREOPER:
			BroadcastResponseParams(msg_IRC_OperReply, inMessage);
			break;

		//Rehash response
		case RPL_REHASHING:
			HandleRehashReply(inMessage);
			break;

		//Time response
		case RPL_TIME:
			HandleTimeReply(inMessage);
			break;

		//Users responses
		case RPL_USERSSTART:
			BroadcastResponseParams(msg_IRC_UsersStartReply, inMessage);
			break;
		case RPL_USERS:
			HandleUsersReply(inMessage);
			break;
		case RPL_ENDOFUSERS:
			BroadcastResponseParams(msg_IRC_UsersEndReply, inMessage);
			break;
		case RPL_NOUSERS:
			BroadcastResponseParams(msg_IRC_UsersNoReply, inMessage);
			break;

		//Trace responses
		case RPL_TRACELINK:
			HandleTraceLinkReply(inMessage);
			break;
		case RPL_TRACECONNECTING:
			HandleTraceConnectingReply(inMessage);
			break;
		case RPL_TRACEHANDSHAKE:
			HandleTraceHandshakeReply(inMessage);
			break;
		case RPL_TRACEUNKNOWN:
			HandleTraceUnknownReply(inMessage);
			break;
		case RPL_TRACEOPERATOR:
			HandleTraceOperatorReply(inMessage);
			break;
		case RPL_TRACEUSER:
			HandleTraceUserReply(inMessage);
			break;
		case RPL_TRACESERVER:
			HandleTraceServerReply(inMessage);
			break;
		case RPL_TRACENEWTYPE:
			HandleTraceNewTypeReply(inMessage);
			break;
		case RPL_TRACELOG:
			HandleTraceLogReply(inMessage);
			break;

		//Stats responses
		case RPL_STATSLINKINFO:
			HandleStatsLinkInfoReply(inMessage);
			break;
		case RPL_STATSCOMMANDS:
			HandleStatsCommandsReply(inMessage);
			break;
		case RPL_STATSCLINE:
			HandleStatsCLineReply(inMessage);
			break;
		case RPL_STATSNLINE:
			HandleStatsNLineReply(inMessage);
			break;
		case RPL_STATSILINE:
			HandleStatsILineReply(inMessage);
			break;
		case RPL_STATSKLINE:
			HandleStatsKLineReply(inMessage);
			break;
		case RPL_STATSYLINE:
			HandleStatsYLineReply(inMessage);
			break;
		case RPL_ENDOFSTATS:
			HandleStatsEndReply(inMessage);
			break;
		case RPL_STATSLLINE:
			HandleStatsLLineReply(inMessage);
			break;
		case RPL_STATSUPTIME:
			BroadcastResponseParams(msg_IRC_StatsUpTimeReply, inMessage);
			break;
		case RPL_STATSOLINE:
			HandleStatsOLineReply(inMessage);
			break;
		case RPL_STATSHLINE:
			HandleStatsHLineReply(inMessage);
			break;

		//LUser responses
		case RPL_LUSERCLIENT:
			BroadcastResponseParams(msg_IRC_LUserClientReply, inMessage);
			break;
		case RPL_LUSEROP:
			HandleLUserOPReply(inMessage);
			break;
		case RPL_LUSERUNKNOWN:
			HandleLUserUnknownReply(inMessage);
			break;
		case RPL_LUSERCHANNELS:
			HandleLUserChannelsReply(inMessage);
			break;
		case RPL_LUSERME:
			BroadcastResponseParams(msg_IRC_LUserMeReply, inMessage);
			break;

		//Admin responses
		case RPL_ADMINME:
			HandleAdminMeReply(inMessage);
			break;
		case RPL_ADMINLOC1:
			BroadcastResponseParams(msg_IRC_AdminLoc1Reply, inMessage);
			break;
		case RPL_ADMINLOC2:
			BroadcastResponseParams(msg_IRC_AdminLoc2Reply, inMessage);
			break;
		case RPL_ADMINEMAIL:
			BroadcastResponseParams(msg_IRC_AdminEmailReply, inMessage);
			break;

		//These are undocumented replies seen from various servers
		case RPL_CREATIONTIME:
			HandleCreationTimeReply(inMessage);
			break;
		case RPL_TOPICINFORMATION:
			HandleTopicInformationReply(inMessage);
			break;

		//These are unused/undefined according to RFC
		case RPL_TRACECLASS:
		case RPL_SERVICEINFO:
		case RPL_SERVICE:
		case RPL_SERVLISTEND:
		case RPL_WHOISCHANOP:
		case RPL_CLOSING:
		case RPL_INFOSTART:
		case ERR_YOUWILLBEBANNED:
		case ERR_NOSERVICEHOST:
		case RPL_STATSQLINE:
		case RPL_ENDOFSERVICES:
		case RPL_SERVLIST:
		case RPL_KILLDONE:
		case RPL_CLOSEEND:
		case RPL_MYPORTIS:
		case ERR_BADCHANMASK:
		default:
			BroadcastMessage(msg_IRC_UnknownReply, inMessage);
			break;
	}
}

// ---------------------------------------------------------------------------
//		• HandleCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleCommand(LIRCResponse * inMessage)
{
	if (IRCMessageIs(inMessage, kIRCPrivmsg)) {
		HandlePrivmsgCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCJoin)) {
		HandleJoinCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCPart)) {
		HandlePartCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCQuit)) {
		HandleQuitCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCTopic)) {
		HandleTopicCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCNotice)) {
		HandleNoticeCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCInvite)) {
		HandleInviteCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCKick)) {
		HandleKickCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCNick)) {
		HandleNickCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCSQuit)) {
		HandleSQuitCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCObject)) {
		HandleObjectCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCMode)) {
		HandleModeCommand(inMessage);

	} else if (IRCMessageIs(inMessage, kIRCPing)) {
		HandlePingCommand(inMessage);

	} else {
		BroadcastMessage(msg_IRC_UnknownCommand, inMessage);
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === Command Handlers ===

				// $$$ TO DO $$$ GHD 10/23/99
				// This code snippet is repeated many times:
				//   PP_CSTD::strlen(inMessage->GetResponseParams()) + 1
				// Perhaps there should be a function that returns this value

// ---------------------------------------------------------------------------
//		• HandlePrivmsgCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandlePrivmsgCommand(LIRCResponse * inMessage)
{
	SMessageCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theCommand.recipient, currPos);

	currPos = PP_CSTD::strtok(nil, "");
	PP_CSTD::strcpy(theCommand.message, currPos + 1); //+1 steps past colon

	//check for command character
	//	set field
	//	remove command char from message
	theCommand.isCommandMessage = (*(theCommand.message) == kIRCMsgCommandChar);
	if (theCommand.isCommandMessage){
		PP_CSTD::memmove(theCommand.message, theCommand.message + 1, PP_CSTD::strlen(theCommand.message));
		char * endCommand = PP_CSTD::strchr(theCommand.message, kIRCMsgCommandChar);
		if (endCommand)
			*endCommand = '\0';
	}

	BroadcastMessage(msg_IRC_PrivmsgCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleNoticeCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleNoticeCommand(LIRCResponse * inMessage)
{
	SMessageCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theCommand.recipient, currPos);

	currPos = PP_CSTD::strtok(nil, "");
	PP_CSTD::strcpy(theCommand.message, currPos + 1); //+1 steps past colon

	theCommand.isCommandMessage = *(theCommand.message) == kIRCMsgCommandChar;

	if (theCommand.isCommandMessage){
		PP_CSTD::memmove(theCommand.message, theCommand.message + 1, PP_CSTD::strlen(theCommand.message));
		char * endCommand = PP_CSTD::strchr(theCommand.message, kIRCMsgCommandChar);
		if (endCommand)
			*endCommand = '\0';
	}

	BroadcastMessage(msg_IRC_NoticeCommand, &theCommand);
}


// ---------------------------------------------------------------------------
//		• HandleJoinCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleJoinCommand(LIRCResponse * inMessage)
{
	SJoinCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.channel,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_JoinCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandlePartCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandlePartCommand(LIRCResponse * inMessage)
{
	SPartCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.channel,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_PartCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleQuitCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleQuitCommand(LIRCResponse * inMessage)
{
	SQuitCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.message,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_QuitCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleTopicCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTopicCommand(LIRCResponse * inMessage)
{
	STopicCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	//Get the channel
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theCommand.channel, currPos);

	//Get the nick
	currPos = PP_CSTD::strtok(nil, "");
	PP_CSTD::strcpy(theCommand.topic, currPos + 1); //+1 walks past colon

	BroadcastMessage(msg_IRC_TopicCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleInviteCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleInviteCommand(LIRCResponse * inMessage)
{
	SInviteCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.channel,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_InviteCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleKickCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleKickCommand(LIRCResponse * inMessage)
{
	SKickCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	//Get the channel
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theCommand.channel, currPos);

	//Get the victim
	currPos = PP_CSTD::strtok(nil, " ");
	PP_CSTD::strcpy(theCommand.victim, currPos);

	//Get the comment
	currPos = PP_CSTD::strtok(nil, "");
	if (currPos)
		PP_CSTD::strcpy(theCommand.comment, currPos + 1); //+1 walks past colon
	else
		theCommand.comment[0] = '\0';

	BroadcastMessage(msg_IRC_KickCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleNickCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleNickCommand(LIRCResponse * inMessage)
{
	SNickCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.newnick,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_NickCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleSQuitCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleSQuitCommand(LIRCResponse * inMessage)
{
	SSQuitCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	//Get the server
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theCommand.server, currPos);

	//Get the comment
	currPos = PP_CSTD::strtok(nil, "");
	if (currPos)
		PP_CSTD::strcpy(theCommand.comment, currPos + 1); //+1 walks past colon
	else
		theCommand.comment[0] = '\0';

	BroadcastMessage(msg_IRC_SQuitCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleObjectCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleObjectCommand(LIRCResponse * inMessage)
{
	SObjectCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	PP_CSTD::strcpy(theCommand.object,inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_ObjectCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• HandleModeCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleModeCommand(LIRCResponse * inMessage)
{
	SModeCommand theCommand;

	ParseNick(inMessage->GetResponsePrefix(), theCommand.nick, theCommand.nickExtension);

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		PP_CSTD::strcpy(theCommand.channelnick, currPos);

		//Get the modes
		char tempModes[128];
		currPos = PP_CSTD::strtok(nil, " ");
		if (*currPos == ':')			//walk past colon if necessary
			currPos++;
		PP_CSTD::strcpy(tempModes, currPos);

		//Get the paramlist
		char tempParams[128];
		currPos = PP_CSTD::strtok(nil, "");
		if (currPos)
			PP_CSTD::strcpy(tempParams, currPos);
		else
			*tempParams = '\0';

		//start strtok on the params in case we have multiples we can
		//	parse them as we walk the param list
		Boolean getParam;
		currPos = PP_CSTD::strtok(tempParams, " ");
		for (SInt16 i = 0; tempModes[i]; i++) {
			//check to see if we have a mode change character
			if (tempModes[i] == '+' || tempModes[i] == '-') {
				theCommand.ison = (tempModes[i] == '+');
				continue;
			}

			//get the mode flag
			theCommand.modeflag = (IRCModeFlag)tempModes[i];

			//catch flags with params and pass on the params too
			switch (theCommand.modeflag) {
				case MODE_IRC_ChanUserLimit:
				case MODE_IRC_ChanOperatorPrivs:
				case MODE_IRC_ChanChannelKey:
				case MODE_IRC_ChanSpeakAbility:
				case MODE_IRC_ChanBanMask:
					getParam = true;
					break;
				default:
					getParam = false;
					break;
			}

			//Get the param if necessary
			if (getParam && currPos) {
				PP_CSTD::strcpy(theCommand.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else {
				theCommand.param[0] = '\0';
			}

			BroadcastMessage(msg_IRC_ModeCommand, &theCommand);
		}
	}
}

// ---------------------------------------------------------------------------
//		• HandlePingCommand
// ---------------------------------------------------------------------------

void
LIRCConnection::HandlePingCommand(LIRCResponse * inMessage)
{
	SendPONG(inMessage->GetResponseParams());

	BroadcastResponseParams(msg_IRC_PingCommand, inMessage);
}

// ===========================================================================

#pragma mark -
#pragma mark === Error Handlers ===

// ---------------------------------------------------------------------------
//		• HandleNoParamError
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleNoParamError(LIRCResponse * inMessage)
{
	SErrorReply theReply;
	theReply.param1[0] = '\0';
	theReply.param2[0] = '\0';
	PP_CSTD::strcpy(theReply.response, inMessage->GetResponseParams());

	MessageT theErrorMessage;
	switch (inMessage->GetResponseCode()) {
		case ERR_NOORIGIN:
			theErrorMessage = msg_IRC_NoOriginError;
			break;
		case ERR_NORECIPIENT:
			theErrorMessage = msg_IRC_NoRecipientError;
			break;
		case ERR_NOTEXTTOSEND:
			theErrorMessage = msg_IRC_NoTextToSendError;
			break;
		case ERR_NOMOTD:
			theErrorMessage = msg_IRC_NoMOTDError;
			break;
		case ERR_FILEERROR:
			theErrorMessage = msg_IRC_FileError;
			break;
		case ERR_NONICKNAMEGIVEN:
			theErrorMessage = msg_IRC_NoNickGivenError;
			break;
		case ERR_SUMMONDISABLED:
			theErrorMessage = msg_IRC_SummonDisabledError;
			break;
		case ERR_USERSDISABLED:
			theErrorMessage = msg_IRC_UsersDisabledError;
			break;
		case ERR_NOTREGISTERED:
			theErrorMessage = msg_IRC_NotRegisteredError;
			break;
		case ERR_ALREADYREGISTRED:
			theErrorMessage = msg_IRC_AlreadyRegisteredError;
			break;
		case ERR_NOPERMFORHOST:
			theErrorMessage = msg_IRC_NoPerformHostError;
			break;
		case ERR_PASSWDMISMATCH:
			theErrorMessage = msg_IRC_PasswdMismatchError;
			break;
		case ERR_YOUREBANNEDCREEP:
			theErrorMessage = msg_IRC_YourBannedError;
			break;
		case ERR_NOPRIVILEGES:
			theErrorMessage = msg_IRC_NoPrivlegesError;
			break;
		case ERR_CANTKILLSERVER:
			theErrorMessage = msg_IRC_CantKillServerError;
			break;
		case ERR_NOOPERHOST:
			theErrorMessage = msg_IRC_NoOperHostError;
			break;
		case ERR_UMODEUNKNOWNFLAG:
			theErrorMessage = msg_IRC_UModeUnknownFlagError;
			break;
		case ERR_USERSDONTMATCH:
			theErrorMessage = msg_IRC_UsersDontMatchError;
			break;

		default:
			BroadcastMessage(msg_IRC_UnknownReply, inMessage);
			break;
	}

	BroadcastMessage(theErrorMessage, &theReply);
}

// ---------------------------------------------------------------------------
//		• HandleOneParamError
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleOneParamError(LIRCResponse * inMessage)
{
	SErrorReply theReply;
	theReply.param2[0] = '\0';

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	//Get the param
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	PP_CSTD::strcpy(theReply.param1, currPos);

	//Get the response
	currPos = PP_CSTD::strtok(nil, "");
	PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

	MessageT theErrorMessage;
	switch (inMessage->GetResponseCode()) {
		case ERR_NOSUCHNICK:
			theErrorMessage = msg_IRC_NoSuchNickError;
			break;
		case ERR_NOSUCHSERVER:
			theErrorMessage = msg_IRC_NoSuchServerError;
			break;
		case ERR_NOSUCHCHANNEL:
			theErrorMessage = msg_IRC_NoSuchChannelError;
			break;
		case ERR_CANNOTSENDTOCHAN:
			theErrorMessage = msg_IRC_CannotSendToChanError;
			break;
		case ERR_TOOMANYCHANNELS:
			theErrorMessage = msg_IRC_TooManyChannelsError;
			break;
		case ERR_WASNOSUCHNICK:
			theErrorMessage = msg_IRC_WasNoSuchNickError;
			break;
		case ERR_TOOMANYTARGETS:
			theErrorMessage = msg_IRC_TooManyTargetsError;
			break;
		case ERR_NOTOPLEVEL:
			theErrorMessage = msg_IRC_NoTopLevelError;
			break;
		case ERR_WILDTOPLEVEL:
			theErrorMessage = msg_IRC_WildTopLevelError;
			break;
		case ERR_UNKNOWNCOMMAND:
			theErrorMessage = msg_IRC_UnknownCommandError;
			break;
		case ERR_NOADMININFO:
			theErrorMessage = msg_IRC_NoAdminInfoError;
			break;
		case ERR_ERRONEUSNICKNAME:
			theErrorMessage = msg_IRC_ErroneusNickError;
			break;
		case ERR_NICKNAMEINUSE:
			theErrorMessage = msg_IRC_NickInUseError;
			break;
		case ERR_NICKCOLLISION:
			theErrorMessage = msg_IRC_NickCollisionError;
			break;
		case ERR_NOTONCHANNEL:
			theErrorMessage = msg_IRC_NotOnChannelError;
			break;
		case ERR_USERONCHANNEL:
			theErrorMessage = msg_IRC_UserOnChannelError;
			break;
		case ERR_NOLOGIN:
			theErrorMessage = msg_IRC_NoLoginError;
			break;
		case ERR_NEEDMOREPARAMS:
			theErrorMessage = msg_IRC_NeedMoreParamsError;
			break;
		case ERR_KEYSET:
			theErrorMessage = msg_IRC_KeySetError;
			break;
		case ERR_CHANNELISFULL:
			theErrorMessage = msg_IRC_ChannelFullError;
			break;
		case ERR_UNKNOWNMODE:
			theErrorMessage = msg_IRC_UnknownModeError;
			break;
		case ERR_INVITEONLYCHAN:
			theErrorMessage = msg_IRC_InviteOnlyChanError;
			break;
		case ERR_BANNEDFROMCHAN:
			theErrorMessage = msg_IRC_BannedFromChanError;
			break;
		case ERR_BADCHANNELKEY:
			theErrorMessage = msg_IRC_BadChannelKeyError;
			break;
		case ERR_CHANOPRIVSNEEDED:
			theErrorMessage = msg_IRC_ChanOpPrivsNeededError;
			break;

		default:
			BroadcastMessage(msg_IRC_UnknownReply, inMessage);
			break;
	}

	BroadcastMessage(theErrorMessage, &theReply);
}

// ---------------------------------------------------------------------------
//		• HandleTwoParamError
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTwoParamError(LIRCResponse * inMessage)
{
	SErrorReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");

	//Get the first param
	PP_CSTD::strcpy(theReply.param1, currPos);

	//Get the second param
	currPos = PP_CSTD::strtok(nil, " ");
	PP_CSTD::strcpy(theReply.param2, currPos);

	//Get the response
	currPos = PP_CSTD::strtok(nil, "");
	PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

	MessageT theErrorMessage;
	switch (inMessage->GetResponseCode()) {
		case ERR_USERNOTINCHANNEL:
			theErrorMessage = msg_IRC_UserNotInChannelError;
			break;

		default:
			BroadcastMessage(msg_IRC_UnknownReply, inMessage);
			break;
	}

	BroadcastMessage(theErrorMessage, &theReply);
}

// ===========================================================================

#pragma mark -
#pragma mark === Reply Handlers ===

// ---------------------------------------------------------------------------
//		• HandleUserHostReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleUserHostReply(LIRCResponse * inMessage)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currReply = PP_CSTD::strtok(tempResponse, " ");
	while (currReply) {
		SUserHostReply theReplyStruct;
		char * seperator = PP_CSTD::strchr(currReply, '=');
		if (!seperator)
			continue;

		//Get the nick and determine if they are operator
		UInt16 nickLen = (UInt16) (seperator - currReply);
		PP_CSTD::strncpy(theReplyStruct.nick, currReply, nickLen);
		theReplyStruct.isOperator = theReplyStruct.nick[nickLen-1] == '*';
		if (theReplyStruct.isOperator)
			nickLen--;
		theReplyStruct.nick[nickLen] = '\0';

		//Get the host and determine if they are away
		PP_CSTD::strcpy(theReplyStruct.host, seperator + 1);
		theReplyStruct.isAway = theReplyStruct.host[0] == '-';
		PP_CSTD::memmove(&(theReplyStruct.host[0]), &(theReplyStruct.host[1]), PP_CSTD::strlen(&(theReplyStruct.host[1])) + 1);

		BroadcastMessage(msg_IRC_UserHostReply, &theReplyStruct);

		currReply = PP_CSTD::strtok(nil, " ");
	}
}

// ---------------------------------------------------------------------------
//		• HandleIsonReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleIsonReply(LIRCResponse * inMessage)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currNick = PP_CSTD::strtok(tempResponse, " ");
	while (currNick) {
		char theNick[kIRCMaxNickLen];
		PP_CSTD::strcpy(theNick, currNick);
		BroadcastMessage(msg_IRC_IsonReply, theNick);

		currNick = PP_CSTD::strtok(nil, " ");
	}
}

// ---------------------------------------------------------------------------
//		• HandleAwayReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleAwayReply(LIRCResponse * inMessage)
{
	SAwayReply theReply;
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	char * messageOffset = PP_CSTD::strstr(tempResponse, " :");
	if (messageOffset) {
		//Get the nick
		UInt16 nickLen = (UInt16) (messageOffset - tempResponse);
		PP_CSTD::strncpy(theReply.nick, tempResponse, nickLen);
		theReply.nick[nickLen] = '\0';

		//Get the message
		PP_CSTD::strcpy(theReply.message, messageOffset + 2);

		BroadcastMessage(msg_IRC_AwayReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoisUserReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisUserReply(LIRCResponse * inMessage)
{
	SWhoisUserReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the user
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.user, currPos);

		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//Get the astrix
		currPos = PP_CSTD::strtok(nil, " ");
		theReply.hadAstrix = (currPos[0] == '*');

		//Get the realname
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.realname, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhoisUserReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoisServerReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisServerReply(LIRCResponse * inMessage)
{
	SWhoisServerReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the sever
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the serverinfo
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.serverinfo, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhoisServerReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoisOperatorReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisOperatorReply(LIRCResponse * inMessage)
{
	SWhoisOperatorReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhoisOperatorReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoisIdleReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisIdleReply(LIRCResponse * inMessage)
{
	SWhoisIdleReply theReply;
	char tempDate[256];

	PP_CSTD::sscanf(inMessage->GetResponseParams(), "%s %lu %s", theReply.nick, &theReply.idleseconds, tempDate);

	IRCTimeToDateTime(tempDate, &(theReply.logindatetime));

	BroadcastMessage(msg_IRC_WhoisIdleReply, &theReply);
}

// ---------------------------------------------------------------------------
//		• HandleWhoisEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisEndReply(LIRCResponse * inMessage)
{
	SWhoisEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhoisEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoisChannelsReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoisChannelsReply(LIRCResponse * inMessage)
{
	SWhoisChannelReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the channel(s)
		currPos = PP_CSTD::strtok(nil, " ");
		while (currPos) {
			if (currPos[0] == ':')	//skip colon if necessary
				currPos++;

			//Check for flags
			theReply.isCHOP = (PP_CSTD::strchr(currPos, '@') != nil);
			theReply.canSpeakOnChannel = (PP_CSTD::strchr(currPos, '+') != nil);

			if (theReply.isCHOP || theReply.canSpeakOnChannel)
				currPos++;

			PP_CSTD::strcpy(theReply.channel, currPos);

			BroadcastMessage(msg_IRC_WhoisChannelsReply, &theReply);

			currPos = PP_CSTD::strtok(nil, " ");
		}
	}
}


// ---------------------------------------------------------------------------
//		• HandleWhowasUserReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhowasUserReply(LIRCResponse * inMessage)
{
	SWhowasUserReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the user
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.user, currPos);

		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//Get the astrix
		currPos = PP_CSTD::strtok(nil, " ");
		theReply.hadAstrix = (currPos[0] == '*');

		//Get the realname
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.realname, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhowasUserReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhowasEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhowasEndReply(LIRCResponse * inMessage)
{
	SWhowasEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhowasEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleListReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleListReply(LIRCResponse * inMessage)
{
	SListReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the numberusers
		currPos = PP_CSTD::strtok(nil, " ");
		theReply.numberusers = (UInt32) PP_CSTD::atol(currPos);

		//Get the topic
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.topic, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_ListReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleChannelmodeisReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleChannelmodeisReply(LIRCResponse * inMessage)
{
	SChannelModeReply theReply;
	theReply.ison = true;	//assume this in case nothing is specified

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the modes & params and parse
		char tempModes[128] = "";
		char tempParams[128] = "";

		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(tempModes, currPos);

		currPos = PP_CSTD::strtok(nil, "");
		if (currPos)
			PP_CSTD::strcpy(tempParams, currPos);

		//start strtok on the params in case we have multiples we can
		//	parse them as we walk the param list
		currPos = PP_CSTD::strtok(tempParams, " ");
		for (SInt16 i = 0; tempModes[i]; i++) {
			//check to see if we have a mode change character
			if (tempModes[i] == '+' || tempModes[i] == '-') {
				theReply.ison = (tempModes[i] == '+');
				continue;
			}

			//get the mode flag
			theReply.modeflag = (IRCModeFlag)tempModes[i];

			//catch flags with params and pass on the params too
			if (theReply.modeflag == MODE_IRC_ChanUserLimit) {
				PP_CSTD::strcpy(theReply.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else if (theReply.modeflag == MODE_IRC_ChanOperatorPrivs) {
				PP_CSTD::strcpy(theReply.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else if (theReply.modeflag == MODE_IRC_ChanChannelKey) {
				PP_CSTD::strcpy(theReply.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else if (theReply.modeflag == MODE_IRC_ChanSpeakAbility) {
				PP_CSTD::strcpy(theReply.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else if (theReply.modeflag == MODE_IRC_ChanBanMask) {
				PP_CSTD::strcpy(theReply.param, currPos);
				currPos = PP_CSTD::strtok(nil, " ");
			} else {
				theReply.param[0] = '\0';
			}

			BroadcastMessage(msg_IRC_ChannelmodeisReply, &theReply);
		}
	}
}

// ---------------------------------------------------------------------------
//		• HandleUmodeisReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleUmodeisReply(LIRCResponse * inMessage)
{
	SUserModeReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the modes and parse
		char tempModes[128];
		PP_CSTD::strcpy(tempModes, currPos);

		for (SInt16 i = 0; tempModes[i]; i++) {
			//check to see if we have a mode change character
			if (tempModes[i] == '+' || tempModes[i] == '-') {
				theReply.ison = (tempModes[i] == '+');
				continue;
			}

			//get the mode flag
			theReply.modeflag = (IRCModeFlag)tempModes[i];

			BroadcastMessage(msg_IRC_UmodeisReply, &theReply);
		}
	}
}

// ---------------------------------------------------------------------------
//		• HandleTopicReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTopicReply(LIRCResponse * inMessage)
{
	STopicReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the topic
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.topic, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_TopicReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleInviteReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleInviteReply(LIRCResponse * inMessage)
{
	SInviteReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the nick
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the channel
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.channel, currPos);

		BroadcastMessage(msg_IRC_InviteReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleSummonReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleSummonReply(LIRCResponse * inMessage)
{
	SSummonReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the user
		PP_CSTD::strcpy(theReply.user, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_SummonReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleVersionReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleVersionReply(LIRCResponse * inMessage)
{
	SVersionReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the version
		PP_CSTD::strcpy(theReply.version, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the comments
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.comments, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_VersionReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoReply(LIRCResponse * inMessage)
{
	SWhoReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the user
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.user, currPos);

		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the nick
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the flags
		currPos = PP_CSTD::strtok(nil, ":");
		theReply.isCHOP = (PP_CSTD::strchr(currPos, '@') != nil);
		theReply.canSpeakOnChannel = (PP_CSTD::strchr(currPos, '+') != nil);
		theReply.isIRCOP = (PP_CSTD::strchr(currPos, '*') != nil);
		theReply.hasH = (PP_CSTD::strchr(currPos, 'H') != nil);
		theReply.hasG = (PP_CSTD::strchr(currPos, 'G') != nil);

		//Get the hopcount
		currPos = PP_CSTD::strtok(nil, " ");
		theReply.hopcount = (UInt32) PP_CSTD::atol(currPos);

		//Get the realname
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.realname, currPos);

		BroadcastMessage(msg_IRC_WhoReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleWhoEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleWhoEndReply(LIRCResponse * inMessage)
{
	SWhoEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the name
		PP_CSTD::strcpy(theReply.name, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_WhoEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleNamesReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleNamesReply(LIRCResponse * inMessage)
{
	SNamesReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//walk over the equal sign if there
		if (PP_CSTD::strcmp(currPos, "=") == 0)
			currPos = PP_CSTD::strtok(nil, " ");

		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the nick(s)
		currPos = PP_CSTD::strtok(nil, " ");
		while (currPos){
			if (currPos[0] == ':')
				currPos++;

			//Get the flags
			theReply.isCHOP = (PP_CSTD::strchr(currPos, '@') != nil);
			theReply.canSpeakOnChannel = (PP_CSTD::strchr(currPos, '+') != nil);

			if (theReply.isCHOP || theReply.canSpeakOnChannel)
				currPos++;

			//Get the nick
			PP_CSTD::strcpy(theReply.nick, currPos);

			BroadcastMessage(msg_IRC_NamesReply, &theReply);

			currPos = PP_CSTD::strtok(nil, " ");
		}
	}
}

// ---------------------------------------------------------------------------
//		• HandleNamesEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleNamesEndReply(LIRCResponse * inMessage)
{
	SNamesEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_NamesEndReply, &theReply);
	}
}


// ---------------------------------------------------------------------------
//		• HandleLinksReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleLinksReply(LIRCResponse * inMessage)
{
	SLinksReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the mask
		PP_CSTD::strcpy(theReply.mask, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the hopcount
		currPos = PP_CSTD::strtok(nil, " ");
		if (currPos[0] == ':')
			currPos++;
		theReply.hopcount = (UInt32) PP_CSTD::atol(currPos);

		//Get the serverinfo
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.serverinfo, currPos);

		BroadcastMessage(msg_IRC_LinksReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleLinksEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleLinksEndReply(LIRCResponse * inMessage)
{
	SLinksEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the mask
		PP_CSTD::strcpy(theReply.mask, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_LinksEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleBanlistReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleBanlistReply(LIRCResponse * inMessage)
{
	SBanlistReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the banid
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.banid, currPos);

		BroadcastMessage(msg_IRC_BanlistReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleBanlistEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleBanlistEndReply(LIRCResponse * inMessage)
{
	SBanlistEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_BanlistEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleRehashReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleRehashReply(LIRCResponse * inMessage)
{
	SRehashReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the configfile
		PP_CSTD::strcpy(theReply.configfile, currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_RehashReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTimeReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTimeReply(LIRCResponse * inMessage)
{
	STimeReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the server
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the timestring
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.timestring, currPos + 1);	//+1 skips the colon

		BroadcastMessage(msg_IRC_TimeReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleUsersReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleUsersReply(LIRCResponse * inMessage)
{
	SUsersReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the userid
		PP_CSTD::strcpy(theReply.userid, currPos);

		//Get the terminal
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.terminal, currPos);

		//Get the host
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.host, currPos);

		BroadcastMessage(msg_IRC_UsersReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleAdminMeReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleAdminMeReply(LIRCResponse * inMessage)
{
	SAdminMeReply theReply;

	//Get the server
	PP_CSTD::strcpy(theReply.server, inMessage->GetResponsePrefix());

	//Get the admininfo
	PP_CSTD::strcpy(theReply.admininfo, inMessage->GetResponseParams());

	BroadcastMessage(msg_IRC_AdminMeReply, &theReply);
}

// ---------------------------------------------------------------------------
//		• HandleTraceLinkReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceLinkReply(LIRCResponse * inMessage)
{
	STraceLinkReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the versiondebug
		PP_CSTD::strcpy(theReply.versiondebug, currPos);

		//Get the destination
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.destination, currPos);

		//Get the nextserver
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.nextserver, currPos);

		BroadcastMessage(msg_IRC_TraceLinkReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceConnectingReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceConnectingReply(LIRCResponse * inMessage)
{
	STraceConnectingReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		BroadcastMessage(msg_IRC_TraceConnectingReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceHandshakeReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceHandshakeReply(LIRCResponse * inMessage)
{
	STraceHandshakeReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		BroadcastMessage(msg_IRC_TraceConnectingReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceUnknownReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceUnknownReply(LIRCResponse * inMessage)
{
	STraceUnknownReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the clientIPAddress
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.clientIPAddress, currPos);

		BroadcastMessage(msg_IRC_TraceUnknownReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceOperatorReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceOperatorReply(LIRCResponse * inMessage)
{
	STraceOperatorReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the nick
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.nick, currPos);

		BroadcastMessage(msg_IRC_TraceOepratorReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceUserReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceUserReply(LIRCResponse * inMessage)
{
	STraceUserReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the nick
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.nick, currPos);

		BroadcastMessage(msg_IRC_TraceUserReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceServerReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceServerReply(LIRCResponse * inMessage)
{
	STraceServerReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the intS
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.intS, currPos);

		//Get the intC
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.intC, currPos);

		//Get the server
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.server, currPos);

		//Get the address
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.address, currPos);

		BroadcastMessage(msg_IRC_TraceServerReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceNewTypeReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceNewTypeReply(LIRCResponse * inMessage)
{
	STraceNewTypeReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the newtype
		PP_CSTD::strcpy(theReply.newtype, currPos);

		//Get the ZeroThing
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.ZeroThing, currPos);

		//Get the clientname
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.clientname, currPos);

		BroadcastMessage(msg_IRC_TraceNewTypeReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTraceLogReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTraceLogReply(LIRCResponse * inMessage)
{
	STraceLogReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the file
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.file, currPos);

		//Get the debuglevel
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.debuglevel, currPos);

		BroadcastMessage(msg_IRC_TraceLogReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsLinkInfoReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsLinkInfoReply(LIRCResponse * inMessage)
{
	SStatsLinkInfoReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the linkname
		PP_CSTD::strcpy(theReply.linkname, currPos);

		//Get the sendq
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.sendq, currPos);

		//Get the sentmessages
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.sentmessages, currPos);

		//Get the sentbytes
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.sentbytes, currPos);

		//Get the receivedmessages
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.receivedmessages, currPos);

		//Get the reseivedbytes
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.reseivedbytes, currPos);

		//Get the timeopen
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.timeopen, currPos);

		BroadcastMessage(msg_IRC_StatsLinkInfoReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsCommandsReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsCommandsReply(LIRCResponse * inMessage)
{
	SStatsCommandsReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the command
		PP_CSTD::strcpy(theReply.command, currPos);

		//Get the count
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.count, currPos);

		BroadcastMessage(msg_IRC_StatsCommandsReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsCLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsCLineReply(LIRCResponse * inMessage)
{
	SStatsCLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the name
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.name, currPos);

		//Get the port
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.port, currPos);

		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		BroadcastMessage(msg_IRC_StatsCLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsNLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsNLineReply(LIRCResponse * inMessage)
{
	SStatsNLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the name
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.name, currPos);

		//Get the port
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.port, currPos);

		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		BroadcastMessage(msg_IRC_StatsNLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsILineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsILineReply(LIRCResponse * inMessage)
{
	SStatsILineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the host2
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host2, currPos);

		//Get the port
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.port, currPos);

		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		BroadcastMessage(msg_IRC_StatsILineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsKLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsKLineReply(LIRCResponse * inMessage)
{
	SStatsKLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the host
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.host, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the username
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.username, currPos);

		//Get the port
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.port, currPos);

		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		BroadcastMessage(msg_IRC_StatsKLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsYLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsYLineReply(LIRCResponse * inMessage)
{
	SStatsYLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the connectionclass
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectionclass, currPos);

		//Get the pingfreq
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.pingfreq, currPos);

		//Get the connectfreq
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.connectfreq, currPos);

		//Get the maxsendq
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.maxsendq, currPos);

		BroadcastMessage(msg_IRC_StatsYLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsEndReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsEndReply(LIRCResponse * inMessage)
{
	SStatsEndReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the statsletter
		theReply.statsletter = *currPos;

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1); //+1 skips the colon

		BroadcastMessage(msg_IRC_StatsEndReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsLLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsLLineReply(LIRCResponse * inMessage)
{
	SStatsLLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the hostmask
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.hostmask, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the servername
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.servername, currPos);

		//Get the maxdepth
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.maxdepth, currPos);

		BroadcastMessage(msg_IRC_StatsLLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsOLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsOLineReply(LIRCResponse * inMessage)
{
	SStatsOLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the hostmask
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.hostmask, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the name
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.name, currPos);

		BroadcastMessage(msg_IRC_StatsOLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleStatsHLineReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleStatsHLineReply(LIRCResponse * inMessage)
{
	StatsHLineReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the hostmask
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.hostmask, currPos);

		//skip the astrix
		currPos = PP_CSTD::strtok(nil, " ");

		//Get the servername
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.servername, currPos);

		BroadcastMessage(msg_IRC_StatsHLineReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleLUserOPReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleLUserOPReply(LIRCResponse * inMessage)
{
	SLUserOPReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the count
		theReply.count = (UInt32) PP_CSTD::atol(currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1); //+1 skips the colon

		BroadcastMessage(msg_IRC_LUserOPReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleLUserUnknownReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleLUserUnknownReply(LIRCResponse * inMessage)
{
	SLUserUnknownReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the count
		theReply.count = (UInt32) PP_CSTD::atol(currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1); //+1 skips the colon

		BroadcastMessage(msg_IRC_LUserUnknownReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleLUserChannelsReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleLUserChannelsReply(LIRCResponse * inMessage)
{
	SLUserChannelsReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the count
		theReply.count = (UInt32) PP_CSTD::atol(currPos);

		//Get the response
		currPos = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(theReply.response, currPos + 1); //+1 skips the colon

		BroadcastMessage(msg_IRC_LUserChannelsReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleCreationTimeReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleCreationTimeReply(LIRCResponse * inMessage)
{
	SCreationTimeReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the time
		currPos = PP_CSTD::strtok(nil, "");
		IRCTimeToDateTime(currPos, &(theReply.datetime));

		BroadcastMessage(msg_IRC_CreationTimeReply, &theReply);
	}
}

// ---------------------------------------------------------------------------
//		• HandleTopicInformationReply
// ---------------------------------------------------------------------------

void
LIRCConnection::HandleTopicInformationReply(LIRCResponse * inMessage)
{
	STopicInfoReply theReply;

	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());
	char * currPos = PP_CSTD::strtok(tempResponse, " ");
	if (currPos) {
		//Get the channel
		PP_CSTD::strcpy(theReply.channel, currPos);

		//Get the nick
		currPos = PP_CSTD::strtok(nil, " ");
		PP_CSTD::strcpy(theReply.nick, currPos);

		//Get the time
		currPos = PP_CSTD::strtok(nil, "");
		IRCTimeToDateTime(currPos, &(theReply.datetime));

		BroadcastMessage(msg_IRC_TopicInfoReply, &theReply);
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === Outgoing Messages ===
#pragma mark »»» Connection regsitration (RFC 1459 - Sec. 4.1) «««

// ---------------------------------------------------------------------------
//		• SendPASS
// ---------------------------------------------------------------------------

void
LIRCConnection::SendPASS(const char * inPassword)
{
	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for PASS command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inPassword) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCPass, inPassword);
	SendCmd(commandString);
}


// ---------------------------------------------------------------------------
//		• SendNICK
// ---------------------------------------------------------------------------

void
LIRCConnection::SendNICK(const char * inNickname)
{
	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for NICK command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickname) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCNick, inNickname);
	SendCmd(commandString);

	//Update the nick name
	if (mNickname == inNickname)
		mNickname = inNickname;
}

// ---------------------------------------------------------------------------
//		• SendUSER
// ---------------------------------------------------------------------------

void
LIRCConnection::SendUSER(const char * inUserName,
						const char * inServerName,
						const char * inRealName)
{
	char			commandString[kMaxIRCCommandLen + 1];

	//+11 is overhead for USER command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inUserName)
						+ *mLocalAddress
						+ PP_CSTD::strlen(inServerName)
						+ PP_CSTD::strlen(inRealName)
						+ 11);

	PP_CSTD::sprintf(commandString, "%s %s ", kIRCUser, inUserName);
	::CopyPascalStringToC(mLocalAddress, commandString + PP_CSTD::strlen(commandString));
	PP_CSTD::sprintf(commandString + PP_CSTD::strlen(commandString), " %s :%s", inServerName, inRealName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendOPER
// ---------------------------------------------------------------------------

void
LIRCConnection::SendOPER(const char * inUserName, const char * inPassword)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for OPER command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inUserName) + PP_CSTD::strlen(inPassword) + 8);

	PP_CSTD::sprintf(commandString, "%s %s %s", kIRCOper, inUserName, inPassword);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendQUIT
// ---------------------------------------------------------------------------

void
LIRCConnection::SendQUIT(const char * inQuitMessage)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for QUIT command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inQuitMessage) + 8);

	if (inQuitMessage)
		PP_CSTD::sprintf(commandString, "%s :%s", kIRCQuit, inQuitMessage);
	else
		PP_CSTD::sprintf(commandString, "%s", kIRCQuit);
	SendCmd(commandString);
}

// ===========================================================================

#pragma mark »»» Channel operations (RFC 1459 - Sec. 4.2) «««

// ---------------------------------------------------------------------------
//		• SendJOIN
// ---------------------------------------------------------------------------

void
LIRCConnection::SendJOIN(const char * inChannelName, const char * inKey)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for JOIN command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + PP_CSTD::strlen(inKey) + 8);

	PP_CSTD::sprintf(commandString, "%s %s %s", kIRCJoin, inChannelName, inKey);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendPART
// ---------------------------------------------------------------------------

void
LIRCConnection::SendPART(const char * inChannelName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for PART command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCPart, inChannelName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendMODE
// ---------------------------------------------------------------------------
// Handels both channel and user modes.

void
LIRCConnection::SendMODE(const char * inChannelNick,
						IRCModeFlag inMode,
						Boolean inStateIsOn,
						const char * inParam)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+9 is overhead for MODE command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelNick)
						+ 2 				//for the mode flag & state
						+ PP_CSTD::strlen(inParam)
						+ 9);

	char state = inStateIsOn ? '+' : '-';

	PP_CSTD::sprintf(commandString, "%s %s %c%c %s", kIRCMode,
								inChannelNick,
								state,
								(char)inMode,
								inParam);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendMODERequest
// ---------------------------------------------------------------------------
// Sends a reuest to retrieve modes for a channel or the user.

void
LIRCConnection::SendMODERequest(const char * inChannelNick)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	PP_CSTD::sprintf(commandString, "%s %s", kIRCMode, inChannelNick);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendTOPIC
// ---------------------------------------------------------------------------
// Pass empty string for inTopic to display the topic for the channel specified

void
LIRCConnection::SendTOPIC(const char * inChannelName,
						const char * inTopic)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+9 is overhead for TOPIC command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + PP_CSTD::strlen(inTopic) + 9);

	PP_CSTD::sprintf(commandString, "%s %s %s", kIRCTopic, inChannelName, inTopic);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendNAMES
// ---------------------------------------------------------------------------
// Pass empty string for inChannelName to display the names on all visible channels

void
LIRCConnection::SendNAMES(const char * inChannelName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for NAMES command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + 8);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCNames, inChannelName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendLIST
// ---------------------------------------------------------------------------
// Pass empty string (or no param) for inChannelName to display the names of all visible channels

void
LIRCConnection::SendLIST(const char * inChannelName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for LIST command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCList, inChannelName);

	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendINVITE
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendINVITE(const char * inChannelName,
							const char * inNickName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+10 is overhead for INVITE command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + PP_CSTD::strlen(inNickName) + 10);

	PP_CSTD::sprintf(commandString, "%s %s %s", kIRCInvite, inNickName, inChannelName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendKICK
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendKICK(const char * inChannelName,
						const char * inNickName,
						const char * inComment)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+10 is overhead for KICK command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inChannelName) + PP_CSTD::strlen(inNickName) + PP_CSTD::strlen(inComment) + 10);

	PP_CSTD::sprintf(commandString, "%s %s %s :%s", kIRCKick, inChannelName, inNickName, inComment);
	SendCmd(commandString);
}

// ===========================================================================

#pragma mark »»» Server queries and commands (RFC 1459 - Sec. 4.3) «««

// ---------------------------------------------------------------------------
//		• SendVERSION
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendVERSION(const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+10 is overhead for VERSION command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + 10);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCVersion, inServerName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendSTATS
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendSTATS(IRCSTATQuery inQuery,
							const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for STATS command
	IRC_ThrowIfTooLong_(1 + PP_CSTD::strlen(inServerName) + 8);	//+1 is stat length

	PP_CSTD::sprintf(commandString, "%s %c %s", kIRCStats, (char)inQuery, inServerName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendLINKS
// ---------------------------------------------------------------------------
// Server params are optional

void
LIRCConnection::SendLINKS(const char * inServerName,
							const char * inServerMask)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+9 is overhead for LINKS command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + PP_CSTD::strlen(inServerMask) + 9);

	PP_CSTD::sprintf(commandString, "%s %s %s", kIRCLinks, inServerName, inServerMask);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendTIME
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendTIME(const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for TIME command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCTime, inServerName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendCONNECT
// ---------------------------------------------------------------------------
// Port and RemoteServer params optional

void
LIRCConnection::SendCONNECT(const char * inServerName,
							const char * inPort,
							const char * inRemoteServer)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+12 is overhead for CONNECT command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + PP_CSTD::strlen(inPort) + PP_CSTD::strlen(inRemoteServer) + 12);

	PP_CSTD::sprintf(commandString, "%s %s %s %s", kIRCConnect, inServerName, inPort, inRemoteServer);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendTRACE
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendTRACE(const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for TRACE command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + 8);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCTrace, inServerName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendADMIN
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendADMIN(const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+8 is overhead for ADMIN command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + 8);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCAdmin, inServerName);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendINFO
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendINFO(const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for INFO command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServerName) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCInfo, inServerName);
	SendCmd(commandString);
}


// ===========================================================================

#pragma mark »»» Sending messages (RFC 1459 - Sec. 4.4) «««

// ---------------------------------------------------------------------------
//		• SendPRIVMSG
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendPRIVMSG(const char * inRecipient,
							const char * inMessage)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+12 is overhead for PRIVMSG command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inRecipient) + PP_CSTD::strlen(inMessage) + 12);

	PP_CSTD::sprintf(commandString, "%s %s :%s", kIRCPrivmsg, inRecipient, inMessage);
	SendCmd(commandString);

	//Turn the message back arround to the client... prevents everyone from having
	//	to do this themselves
	SMessageCommand theCommand;
	PP_CSTD::strcpy(theCommand.nick, GetNickname());
	PP_CSTD::strcpy(theCommand.nickExtension, "");
	PP_CSTD::strcpy(theCommand.recipient, inRecipient);
	PP_CSTD::strcpy(theCommand.message, inMessage);
	theCommand.isCommandMessage = false;
	BroadcastMessage(msg_IRC_PrivmsgCommand, &theCommand);
}

// ---------------------------------------------------------------------------
//		• SendNOTICE
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendNOTICE(const char * inRecipient,
							const char * inMessage)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+11 is overhead for NOTICE command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inRecipient) + PP_CSTD::strlen(inMessage) + 11);

	PP_CSTD::sprintf(commandString, "%s %s :%s", kIRCNotice, inRecipient, inMessage);
	SendCmd(commandString);
}

// ===========================================================================

#pragma mark »»» User based queries (RFC 1459 - Sec. 4.5) «««

// ---------------------------------------------------------------------------
//		• SendWHO
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendWHO(const char * inNickName, Boolean OperatorsOnly)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//nick required for operator only flag
	if (OperatorsOnly && *inNickName) {
		//+6 is overhead for WHO command w/ operator flag
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickName) + 6);
		PP_CSTD::sprintf(commandString, "%s %s o", kIRCWho, inNickName);
	} else {
		//+5 is overhead for WHO command w/o operator flag
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickName) + 5);
		PP_CSTD::sprintf(commandString, "%s %s", kIRCWho, inNickName);
	}

	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendWHOIS
// ---------------------------------------------------------------------------
// Server param is optional

void
LIRCConnection::SendWHOIS(const char * inNickMask,
							const char * inServerName)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	if (inServerName) {
		//+9 is overhead for WHOIS command w/ inServerName
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickMask) + PP_CSTD::strlen(inServerName) + 9);
		PP_CSTD::sprintf(commandString, "%s %s %s", kIRCWhois, inServerName, inNickMask);
	} else {
		//+8 is overhead for WHOIS command w/o inServerName
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickMask) + 8);
		PP_CSTD::sprintf(commandString, "%s %s", kIRCWhois, inNickMask);
	}
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendWHOWAS
// ---------------------------------------------------------------------------
// Count and Server params are optional however count must be included if
//	 server stipulated

void
LIRCConnection::SendWHOWAS(const char * inNickname,
							const char * inServerName,
							UInt16 inCount)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	if (inCount && inServerName) {
		//+11 is overhead for WHOWAS command w/ inServerName & inCount
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickname) + PP_CSTD::strlen(inServerName) + 11);
		PP_CSTD::sprintf(commandString, "%s %s %u %s", kIRCWhowas, inNickname, inCount, inServerName);
	} else if (inCount) {
		//+10 is overhead for WHOWAS command w/ inCount
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickname) + PP_CSTD::strlen(inServerName) + 10);
		PP_CSTD::sprintf(commandString, "%s %s %u", kIRCWhowas, inNickname, inCount);
	} else {
		//+9 is overhead for WHOWAS command w/ inNickname only
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickname) + 9);
		PP_CSTD::sprintf(commandString, "%s %s", kIRCWhowas, inNickname);
	}

	SendCmd(commandString);
}


// ===========================================================================

#pragma mark »»» Miscellaneous messages (RFC 1459 - Sec. 4.6) «««

// ---------------------------------------------------------------------------
//		• SendKILL
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendKILL(const char * inNickname,
						const char * inComment)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+9 is overhead for KILL command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNickname) + PP_CSTD::strlen(inComment) + 9);

	PP_CSTD::sprintf(commandString, "%s %s :%s", kIRCKill, inNickname, inComment);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendPONG
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendPONG(const char * inPingFrom)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	PP_CSTD::sprintf(commandString, "%s :%s", kIRCPong, inPingFrom);
	SendCmd(commandString);
}

// ===========================================================================

#pragma mark »»» Optionals (RFC 1459 - Sec. 5) «««

// ---------------------------------------------------------------------------
//		• SendAWAY
// ---------------------------------------------------------------------------
//	Call w/o param to remove away message

void
LIRCConnection::SendAWAY(const char * inComment)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	if (inComment) {
		//+8 is overhead for AWAY command
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inComment) + 8);
		PP_CSTD::sprintf(commandString, "%s :%s", kIRCAway, inComment);
	} else {
		PP_CSTD::sprintf(commandString, "%s", kIRCAway);
	}

	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendREHASH
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendREHASH()
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	PP_CSTD::sprintf(commandString, "%s", kIRCRehash);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendRESTART
// ---------------------------------------------------------------------------
//

void
LIRCConnection::SendRESTART()
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	PP_CSTD::sprintf(commandString, "%s", kIRCRestart);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendSUMMON
// ---------------------------------------------------------------------------
//	Server param is optional

void
LIRCConnection::SendSUMMON(const char * inUser,
							const char * inServer)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	if (inServer) {
		//+10 is overhead for SUMMON command w/ inServer
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inUser) + PP_CSTD::strlen(inServer) + 10);
		PP_CSTD::sprintf(commandString, "%s %s %s", kIRCSummon, inUser, inServer);
	} else {
		//+9 is overhead for SUMMON command w/o inServer
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inUser) + 9);
		PP_CSTD::sprintf(commandString, "%s %s", kIRCSummon, inUser);
	}

	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendUSERS
// ---------------------------------------------------------------------------
//	Server param is optional

void
LIRCConnection::SendUSERS(const char * inServer)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	if (inServer) {
		//+8 is overhead for USERS command w/ inServer
		IRC_ThrowIfTooLong_(PP_CSTD::strlen(inServer) + 8);
		PP_CSTD::sprintf(commandString, "%s %s", kIRCUsers, inServer);
	} else {
		PP_CSTD::sprintf(commandString, "%s", kIRCUsers);
	}

	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendUSERHOST
// ---------------------------------------------------------------------------
//	inNicknameList must be space delimited list of <= 5 nicknames

void
LIRCConnection::SendUSERHOST(const char * inNicknameList)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+11 is overhead for USERHOST command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNicknameList) + 11);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCUserhost, inNicknameList);
	SendCmd(commandString);
}

// ---------------------------------------------------------------------------
//		• SendISON
// ---------------------------------------------------------------------------
//	inNicknameList must be space delimited list of nicknames <= ~500 characters

void
LIRCConnection::SendISON(const char * inNicknameList)
{
	ThrowIfNot_(mIRCState == IRCConnected);

	char			commandString[kMaxIRCCommandLen + 1];

	//+7 is overhead for ISON command
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inNicknameList) + 7);

	PP_CSTD::sprintf(commandString, "%s %s", kIRCIson, inNicknameList);
	SendCmd(commandString);
}

// ===========================================================================

#pragma mark -
#pragma mark === Utilities ===

// ---------------------------------------------------------------------------
//		• ParseNick
// ---------------------------------------------------------------------------

Boolean
LIRCConnection::ParseNick(const char * inPrefix, char * outNick, char * outExtension)
{
	*outNick = '\0';
	*outExtension = '\0';

	char temp[kMaxIRCCommandLen];
	PP_CSTD::strcpy(temp, inPrefix);

	char * bangOffset = PP_CSTD::strtok(temp, "!");
	if (bangOffset) {
		PP_CSTD::strcpy(outNick, bangOffset);

		bangOffset = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy(outExtension, bangOffset);
		return true;
	}

	return false;
}

// ---------------------------------------------------------------------------
//		• IRCMessageIs
// ---------------------------------------------------------------------------

Boolean
LIRCConnection::IRCMessageIs(LIRCResponse * msg, const char * command)
{
	return (PP_CSTD::strcmp(msg->GetResponseCommandText(), command) == 0);
}

// ---------------------------------------------------------------------------
//		• SendCmd
// ---------------------------------------------------------------------------

void
LIRCConnection::SendCmd(char * inString)
{
	//+2 is overhead for CRLF
	//	other routines have probably check this already, but it couldn't hurt
	IRC_ThrowIfTooLong_(PP_CSTD::strlen(inString) + 2);

	PP_CSTD::strcat(inString, CRLF);
	SendData(inString, PP_CSTD::strlen(inString));
}

// ---------------------------------------------------------------------------
//		• IRCTimeToDateTime
// ---------------------------------------------------------------------------

void
LIRCConnection::IRCTimeToDateTime(const char * inIRCTime, DateTimeRec * outDateTime)
{
	SInt32			ourGmtDelta;
	MachineLocation theLoc;

	ReadLocation(&theLoc);

	ourGmtDelta = ::BitAnd(theLoc.u.gmtDelta, 0x00FFFFFF);
	if ( ::BitTst(&ourGmtDelta, 23) ) {
		ourGmtDelta = ::BitOr(ourGmtDelta, 0xFF000000)/(60*60);
	}

	//convert hours to seconds
	ourGmtDelta = ourGmtDelta * 3600;

	long IRCTime = PP_CSTD::atol(inIRCTime);

	//Adjust IRC seconds to Mac seconds
	IRCTime += kIRCTimeOffset + ourGmtDelta;

	::SecondsToDate((UInt32) IRCTime, outDateTime);
}

// ---------------------------------------------------------------------------
//		• BroadcastResponseParams
// ---------------------------------------------------------------------------

void
LIRCConnection::BroadcastResponseParams(MessageT inBroadcastMessage, LIRCResponse * inMessage)
{
	StPointerBlock tempResponse((SInt32) (PP_CSTD::strlen(inMessage->GetResponseParams()) + 1), true);
	PP_CSTD::strcpy(tempResponse, inMessage->GetResponseParams());

	BroadcastMessage(inBroadcastMessage, tempResponse);
}

PP_End_Namespace_PowerPlant
