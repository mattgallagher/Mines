// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	IRC_Constants.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_IRC_Constants
#define _H_IRC_Constants
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#define kMaxIRCCommandLen		512

#define kIRCMaxNickLen			32
#define kIRCMaxNickExtLen		128
#define kIRCMaxChannelLen		64
#define kIRCMaxHostLen			255
#define kIRCMaxRealNameLen		128
#define kIRCMaxPasswordLen		64
#define kIRCMaxUsernameLen		64
#define kIRCMaxPortLen			10

enum IRCModeFlag {
	//User Flags
	MODE_IRC_UserInvisible	 			= 'i',
	MODE_IRC_UserReceivesServerNotices 	= 's',
	MODE_IRC_UserReceivesWallops		= 'w',
	MODE_IRC_UserOperatorPrivs			= 'o',

	//Channel Flags
	MODE_IRC_ChanOperatorPrivs 			= 'o',
	MODE_IRC_ChanPrivateChannel 		= 'p',
	MODE_IRC_ChanSecretChannel 			= 's',
	MODE_IRC_ChanInviteOnly 			= 'i',
	MODE_IRC_ChanTopicSettable 			= 't',
	MODE_IRC_ChanNoMessagesFromOutside 	= 'n',
	MODE_IRC_ChanModerated 				= 'm',
	MODE_IRC_ChanUserLimit 				= 'l',
	MODE_IRC_ChanBanMask 				= 'b',
	MODE_IRC_ChanSpeakAbility 			= 'v',
	MODE_IRC_ChanChannelKey				= 'k'
};


enum IRCSTATQuery {
	STAT_IRC_Connections	 		= 'c',
	STAT_IRC_LeavesAndHubs			= 'h',
	STAT_IRC_ClientAllows			= 'i',
	STAT_IRC_BanList				= 'k',
	STAT_IRC_ServerConnections 		= 'l',
	STAT_IRC_Commands		 		= 'm',
	STAT_IRC_OperatorConnections	= 'o',
	STAT_IRC_YLines			 		= 'y',
	STAT_IRC_ServerUpTime 			= 'u'
};

//IRC Broadcast messages
enum {
	//Command Replies
	msg_IRC_UserHostReply			= 1001,
	msg_IRC_IsonReply				= 1002,
	msg_IRC_AwayReply				= 1003,
	msg_IRC_UnawayReply				= 1004,
	msg_IRC_NowawayReply			= 1005,
	msg_IRC_WhoisUserReply			= 1006,
	msg_IRC_WhoisServerReply		= 1007,
	msg_IRC_WhoisOperatorReply		= 1008,
	msg_IRC_WhoisIdleReply			= 1009,
	msg_IRC_WhoisEndReply			= 1010,
	msg_IRC_WhoisChannelsReply		= 1011,
	msg_IRC_WhowasUserReply			= 1012,
	msg_IRC_WhowasEndReply			= 1013,
	msg_IRC_ListStartReply			= 1014,
	msg_IRC_ListReply				= 1015,
	msg_IRC_ListEndReply			= 1016,
	msg_IRC_NoTopicReply			= 1017,
	msg_IRC_TopicReply				= 1018,
	msg_IRC_InviteReply				= 1019,
	msg_IRC_SummonReply				= 1020,
	msg_IRC_VersionReply			= 1021,
	msg_IRC_WhoReply				= 1022,
	msg_IRC_WhoEndReply				= 1023,
	msg_IRC_NamesReply				= 1024,
	msg_IRC_NamesEndReply			= 1025,
	msg_IRC_InfoReply				= 1026,
	msg_IRC_InfoEndReply			= 1027,
	msg_IRC_LinksReply				= 1028,
	msg_IRC_LinksEndReply			= 1029,
	msg_IRC_BanlistReply			= 1030,
	msg_IRC_BanlistEndReply			= 1031,
	msg_IRC_OperReply				= 1032,
	msg_IRC_RehashReply				= 1033,
	msg_IRC_TimeReply				= 1034,
	msg_IRC_UsersStartReply			= 1035,
	msg_IRC_UsersReply				= 1036,
	msg_IRC_UsersEndReply			= 1037,
	msg_IRC_UsersNoReply			= 1038,
	msg_IRC_AdminLoc1Reply			= 1039,
	msg_IRC_AdminLoc2Reply			= 1040,
	msg_IRC_AdminEmailReply			= 1041,
	msg_IRC_AdminMeReply			= 1042,
	msg_IRC_TraceLinkReply			= 1043,
	msg_IRC_TraceConnectingReply 	= 1044,
	msg_IRC_TraceHandshakeReply		= 1045,
	msg_IRC_TraceUnknownReply		= 1046,
	msg_IRC_TraceOepratorReply		= 1047,
	msg_IRC_TraceUserReply			= 1048,
	msg_IRC_TraceServerReply		= 1049,
	msg_IRC_TraceNewTypeReply		= 1050,
	msg_IRC_TraceLogReply			= 1051,
	msg_IRC_StatsLinkInfoReply		= 1052,
	msg_IRC_StatsCommandsReply		= 1053,
	msg_IRC_StatsCLineReply			= 1054,
	msg_IRC_StatsNLineReply			= 1055,
	msg_IRC_StatsILineReply			= 1056,
	msg_IRC_StatsKLineReply			= 1057,
	msg_IRC_StatsYLineReply			= 1058,
	msg_IRC_StatsEndReply			= 1059,
	msg_IRC_StatsLLineReply			= 1060,
	msg_IRC_StatsUpTimeReply		= 1061,
	msg_IRC_StatsOLineReply			= 1062,
	msg_IRC_StatsHLineReply			= 1063,
	msg_IRC_LUserClientReply		= 1064,
	msg_IRC_LUserMeReply			= 1065,
	msg_IRC_LUserOPReply			= 1066,
	msg_IRC_LUserUnknownReply		= 1067,
	msg_IRC_LUserChannelsReply		= 1068,
	msg_IRC_ChannelmodeisReply		= 1069,
	msg_IRC_UmodeisReply			= 1070,
	msg_IRC_MOTDStartReply			= 1071,
	msg_IRC_MOTDReceiveReply		= 1072,
	msg_IRC_MOTDCompleteReply		= 1073,
	msg_IRC_ReplyNone				= 1074,

	//Error Messages
	//	No Params
	msg_IRC_NoOriginError			= 1101,
	msg_IRC_NoRecipientError		= 1102,
	msg_IRC_NoTextToSendError		= 1103,
	msg_IRC_NoMOTDError				= 1104,
	msg_IRC_FileError				= 1105,
	msg_IRC_NoNickGivenError		= 1106,
	msg_IRC_SummonDisabledError		= 1107,
	msg_IRC_UsersDisabledError		= 1108,
	msg_IRC_NotRegisteredError		= 1109,
	msg_IRC_AlreadyRegisteredError	= 1110,
	msg_IRC_NoPerformHostError		= 1111,
	msg_IRC_PasswdMismatchError		= 1112,
	msg_IRC_YourBannedError			= 1113,
	msg_IRC_NoPrivlegesError		= 1114,
	msg_IRC_CantKillServerError		= 1115,
	msg_IRC_NoOperHostError			= 1116,
	msg_IRC_UModeUnknownFlagError	= 1117,
	msg_IRC_UsersDontMatchError		= 1118,

	//	One Param
	msg_IRC_NoSuchNickError			= 1119,
	msg_IRC_NoSuchServerError		= 1120,
	msg_IRC_NoSuchChannelError		= 1121,
	msg_IRC_CannotSendToChanError	= 1122,
	msg_IRC_TooManyChannelsError	= 1123,
	msg_IRC_WasNoSuchNickError		= 1124,
	msg_IRC_TooManyTargetsError		= 1125,
	msg_IRC_NoTopLevelError			= 1126,
	msg_IRC_WildTopLevelError		= 1127,
	msg_IRC_UnknownCommandError		= 1128,
	msg_IRC_NoAdminInfoError		= 1129,
	msg_IRC_ErroneusNickError		= 1130,
	msg_IRC_NickInUseError			= 1131,
	msg_IRC_NickCollisionError		= 1132,
	msg_IRC_NotOnChannelError		= 1133,
	msg_IRC_UserOnChannelError		= 1134,
	msg_IRC_NoLoginError			= 1135,
	msg_IRC_NeedMoreParamsError		= 1136,
	msg_IRC_KeySetError				= 1137,
	msg_IRC_ChannelFullError		= 1138,
	msg_IRC_UnknownModeError		= 1139,
	msg_IRC_InviteOnlyChanError		= 1140,
	msg_IRC_BannedFromChanError		= 1141,
	msg_IRC_BadChannelKeyError		= 1142,
	msg_IRC_ChanOpPrivsNeededError	= 1143,

	//	Two Params
	msg_IRC_UserNotInChannelError	= 1144,

	//Command Messages
	msg_IRC_PrivmsgCommand			= 1200,
	msg_IRC_JoinCommand				= 1201,
	msg_IRC_PartCommand				= 1202,
	msg_IRC_QuitCommand				= 1203,
	msg_IRC_NoticeCommand			= 1204,
	msg_IRC_TopicCommand			= 1205,
	msg_IRC_InviteCommand			= 1206,
	msg_IRC_KickCommand				= 1207,
	msg_IRC_NickCommand				= 1208,
	msg_IRC_SQuitCommand			= 1209,
	msg_IRC_ObjectCommand			= 1210,
	msg_IRC_ModeCommand				= 1211,
	msg_IRC_PingCommand				= 1212,

	//Unknown & Undocumented
	msg_IRC_UnknownReply			= 1220,
	msg_IRC_UnknownCommand			= 1221,
	msg_IRC_CreationTimeReply		= 1222,
	msg_IRC_TopicInfoReply			= 1223,

	//PP Specific
	msg_IRC_Disconnected			= 1230
};

//Command Structures
typedef struct SMessageCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char recipient[kIRCMaxChannelLen + 1];
	char message[kMaxIRCCommandLen + 1];
	Boolean isCommandMessage;
} SMessageCommand;

typedef struct SJoinCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channel[kIRCMaxChannelLen + 1];
} SJoinCommand;

typedef struct SPartCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channel[kIRCMaxChannelLen + 1];
} SPartCommand;

typedef struct SQuitCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char message[kMaxIRCCommandLen + 1];
} SQuitCommand;

typedef struct STopicCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channel[kIRCMaxChannelLen + 1];
	char topic[kMaxIRCCommandLen + 1];
} STopicCommand;

typedef struct SInviteCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channel[kIRCMaxChannelLen + 1];
} SInviteCommand;

typedef struct SKickCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channel[kIRCMaxChannelLen + 1];
	char victim[kIRCMaxNickLen + 1];
	char comment[kMaxIRCCommandLen + 1];
} SKickCommand;

typedef struct SNickCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char newnick[kIRCMaxNickLen + 1];
} SNickCommand;

typedef struct SSQuitCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char server[kIRCMaxHostLen + 1];
	char comment[kMaxIRCCommandLen + 1];
} SSQuitCommand;

typedef struct SObjectCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char object[kMaxIRCCommandLen + 1];
} SObjectCommand;

typedef struct SModeCommand {
	char nick[kIRCMaxNickLen + 1];
	char nickExtension[kIRCMaxNickExtLen + 1];
	char channelnick[kIRCMaxChannelLen + 1];
	IRCModeFlag modeflag;
	Boolean ison;
	char param[kMaxIRCCommandLen + 1];
} SModeCommand;

//Error Reply Structure
typedef struct SErrorReply {
	char param1[kMaxIRCCommandLen + 1];
	char param2[kMaxIRCCommandLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SErrorReply;

//Command Reply Structures
typedef struct SUserHostReply {
	char nick[kIRCMaxNickLen + 1];
	char host[kIRCMaxHostLen + 1];
	Boolean isOperator;
	Boolean isAway;
} SUserHostReply;

typedef struct SAwayReply {
	char nick[kIRCMaxNickLen + 1];
	char message[kMaxIRCCommandLen + 1];
} SAwayReply;

typedef struct SWhoisUserReply {
	char nick[kIRCMaxNickLen + 1];
	char user[kIRCMaxUsernameLen + 1];
	char host[kIRCMaxHostLen + 1];
	char realname[kIRCMaxRealNameLen + 1];
	Boolean hadAstrix;
} SWhoisUserReply;

typedef struct SWhoisServerReply {
	char nick[kIRCMaxNickLen + 1];
	char server[kIRCMaxHostLen + 1];
	char serverinfo[kMaxIRCCommandLen + 1];
} SWhoisServerReply;

typedef struct SWhoisOperatorReply {
	char nick[kIRCMaxNickLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SWhoisOperatorReply;

typedef struct SWhoisIdleReply {
	char nick[kIRCMaxNickLen + 1];
	UInt32 idleseconds;
	DateTimeRec logindatetime;
} SWhoisIdleReply;

typedef struct SWhoisEndReply {
	char nick[kIRCMaxNickLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SWhoisEndReply;

typedef struct SWhoisChannelReply {
	char nick[kIRCMaxNickLen + 1];
	char channel[kIRCMaxChannelLen + 1];
	Boolean isCHOP;
	Boolean canSpeakOnChannel;
} SWhoisChannelReply;

typedef struct SWhowasUserReply {
	char nick[kIRCMaxNickLen + 1];
	char user[kIRCMaxUsernameLen + 1];
	char host[kIRCMaxHostLen + 1];
	char realname[kIRCMaxRealNameLen + 1];
	Boolean hadAstrix;
} SWhowasUserReply;

typedef struct SWhowasEndReply {
	char nick[kIRCMaxNickLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SWhowasEndReply;

typedef struct SListReply {
	char channel[kIRCMaxChannelLen + 1];
	UInt32 numberusers;
	char topic[kMaxIRCCommandLen + 1];
} SListReply;

typedef struct STopicReply {
	char channel[kIRCMaxChannelLen + 1];
	char topic[kMaxIRCCommandLen + 1];
} STopicReply;

typedef struct SInviteReply {
	char channel[kIRCMaxChannelLen + 1];
	char nick[kIRCMaxNickLen + 1];
} SInviteReply;

typedef struct SSummonReply {
	char user[kIRCMaxUsernameLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SSummonReply;

typedef struct SVersionReply {
	char version[kMaxIRCCommandLen + 1];
	char server[kIRCMaxHostLen + 1];
	char comments[kMaxIRCCommandLen + 1];
} SVersionReply;

typedef struct SWhoReply {
	char channel[kIRCMaxChannelLen + 1];
	char user[kIRCMaxUsernameLen + 1];
	char host[kIRCMaxHostLen + 1];
	char server[kIRCMaxHostLen + 1];
	char nick[kIRCMaxNickLen + 1];
	UInt32 hopcount;
	char realname[kIRCMaxRealNameLen + 1];
	Boolean isCHOP;
	Boolean canSpeakOnChannel;
	Boolean isIRCOP;
	Boolean hasH;
	Boolean hasG;
} SWhoReply;

typedef struct SWhoEndReply {
	char name[kIRCMaxRealNameLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SWhoEndReply;

typedef struct SNamesReply {
	char channel[kIRCMaxChannelLen + 1];
	char nick[kIRCMaxNickLen + 1];
	Boolean isCHOP;
	Boolean canSpeakOnChannel;
} SNamesReply;

typedef struct SNamesEndReply {
	char channel[kIRCMaxChannelLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SNamesEndReply;

typedef struct SLinksReply {
	char mask[kMaxIRCCommandLen + 1];
	char server[kIRCMaxHostLen + 1];
	UInt32 hopcount;
	char serverinfo[kMaxIRCCommandLen + 1];
} SLinksReply;

typedef struct SLinksEndReply {
	char mask[kMaxIRCCommandLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SLinksEndReply;

typedef struct SBanlistReply {
	char channel[kIRCMaxChannelLen + 1];
	char banid[kMaxIRCCommandLen + 1];
} SBanlistReply;

typedef struct SBanlistEndReply {
	char channel[kIRCMaxChannelLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SBanlistEndReply;

typedef struct SRehashReply {
	char configfile[kMaxIRCCommandLen + 1];
	char response[kMaxIRCCommandLen + 1];
} SRehashReply;

typedef struct STimeReply {
	char server[kIRCMaxHostLen + 1];
	char timestring[kMaxIRCCommandLen + 1];
} STimeReply;

typedef struct SUsersReply {
	char userid[kIRCMaxUsernameLen];
	char terminal[kIRCMaxUsernameLen];
	char host[kIRCMaxUsernameLen];
} SUsersReply;

typedef struct SAdminMeReply {
	char server[kIRCMaxHostLen + 1];
	char admininfo[kMaxIRCCommandLen + 1];
} SAdminMeReply;

typedef struct STraceLinkReply {
	char versiondebug[kMaxIRCCommandLen + 1];
	char destination[kMaxIRCCommandLen + 1];
	char nextserver[kMaxIRCCommandLen + 1];
} STraceLinkReply;

typedef struct STraceConnectingReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char server[kIRCMaxHostLen + 1];
} STraceConnectingReply;

typedef struct STraceHandshakeReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char server[kIRCMaxHostLen + 1];
} STraceHandshakeReply;

typedef struct STraceUnknownReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char clientIPAddress[kIRCMaxHostLen + 1];
} STraceUnknownReply;

typedef struct STraceOperatorReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char nick[kIRCMaxNickLen + 1];
} STraceOperatorReply;

typedef struct STraceUserReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char nick[kIRCMaxNickLen + 1];
} STraceUserReply;

typedef struct STraceServerReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char intS[16];
	char intC[16];
	char server[kIRCMaxHostLen + 1];
	char address[kIRCMaxHostLen + 1];
} STraceServerReply;

typedef struct STraceNewTypeReply {
	char newtype[kMaxIRCCommandLen + 1];
	char ZeroThing[512];
	char clientname[kIRCMaxHostLen + 1];
} STraceNewTypeReply;

typedef struct STraceLogReply {
	char file[kMaxIRCCommandLen + 1];
	char debuglevel[kMaxIRCCommandLen + 1];
} STraceLogReply;

typedef struct SStatsLinkInfoReply {
	char linkname[kMaxIRCCommandLen + 1];
	char sendq[16];
	char sentmessages[16];
	char sentbytes[16];
	char receivedmessages[16];
	char reseivedbytes[16];
	char timeopen[kMaxIRCCommandLen + 1];
} SStatsLinkInfoReply;

typedef struct SStatsCommandsReply {
	char command[kMaxIRCCommandLen + 1];
	char count[16];
} SStatsCommandsReply;

typedef struct SStatsCLineReply {
	char host[kIRCMaxHostLen + 1];
	char name[kIRCMaxHostLen + 1];
	char port[kIRCMaxPortLen + 1];
	char connectionclass[kMaxIRCCommandLen + 1];
} SStatsCLineReply;

typedef struct SStatsNLineReply {
	char host[kIRCMaxHostLen + 1];
	char name[kIRCMaxHostLen + 1];
	char port[kIRCMaxPortLen + 1];
	char connectionclass[512];
} SStatsNLineReply;

typedef struct SStatsILineReply {
	char host[kIRCMaxHostLen + 1];
	char host2[kIRCMaxHostLen + 1];
	char port[kIRCMaxPortLen + 1];
	char connectionclass[kMaxIRCCommandLen + 1];
} SStatsILineReply;

typedef struct SStatsKLineReply {
	char host[kIRCMaxHostLen + 1];
	char username[kIRCMaxUsernameLen];
	char port[kIRCMaxPortLen + 1];
	char connectionclass[kMaxIRCCommandLen + 1];
} SStatsKLineReply;

typedef struct SStatsYLineReply {
	char connectionclass[kMaxIRCCommandLen + 1];
	char pingfreq[16];
	char connectfreq[16];
	char maxsendq[16];
} SStatsYLineReply;

typedef struct SStatsEndReply {
	char statsletter;
	char response[kMaxIRCCommandLen + 1];
} SStatsEndReply;

typedef struct SStatsLLineReply {
	char hostmask[kIRCMaxHostLen + 1];
	char servername[kIRCMaxHostLen + 1];
	char maxdepth[16];
} SStatsLLineReply;

typedef struct SStatsOLineReply {
	char hostmask[kIRCMaxHostLen + 1];
	char name[kIRCMaxHostLen + 1];
} SStatsOLineReply;

typedef struct SStatsHLineReply {
	char hostmask[kIRCMaxHostLen + 1];
	char servername[kIRCMaxHostLen + 1];
} StatsHLineReply;

typedef struct SLUserOPReply {
	UInt32 count;
	char response[kMaxIRCCommandLen + 1];
} SLUserOPReply;

typedef struct SLUserUnknownReply {
	UInt32 count;
	char response[kMaxIRCCommandLen + 1];
} SLUserUnknownReply;

typedef struct SLUserChannelsReply {
	UInt32 count;
	char response[kMaxIRCCommandLen + 1];
} SLUserChannelsReply;

typedef struct SCreationTimeReply {
	char channel[kIRCMaxChannelLen + 1];
	DateTimeRec datetime;
} SCreationTimeReply;

typedef struct STopicInfoReply {
	char channel[kIRCMaxChannelLen + 1];
	char nick[kIRCMaxChannelLen + 1];
	DateTimeRec datetime;
} STopicInfoReply;

typedef struct SChannelModeReply {
	char channel[kIRCMaxChannelLen + 1];
	IRCModeFlag modeflag;
	Boolean	ison;
	char param[kMaxIRCCommandLen + 1];
} SChannelModeReply;

typedef struct SUserModeReply {
	IRCModeFlag modeflag;
	Boolean	ison;
} SUserModeReply;

//IRC Responses
enum IRCResponse{
	IRC_UNDEFINED			= 1000,
	ERR_NOSUCHNICK			= 401,
	ERR_NOSUCHSERVER		= 402,
	ERR_NOSUCHCHANNEL		= 403,
	ERR_CANNOTSENDTOCHAN	= 404,
	ERR_TOOMANYCHANNELS		= 405,
	ERR_WASNOSUCHNICK		= 406,
	ERR_TOOMANYTARGETS		= 407,
	ERR_NOORIGIN			= 409,
	ERR_NORECIPIENT			= 411,
	ERR_NOTEXTTOSEND		= 412,
	ERR_NOTOPLEVEL			= 413,
	ERR_WILDTOPLEVEL		= 414,
	ERR_UNKNOWNCOMMAND		= 421,
	ERR_NOMOTD				= 422,
	ERR_NOADMININFO			= 423,
	ERR_FILEERROR			= 424,
	ERR_NONICKNAMEGIVEN		= 431,
	ERR_ERRONEUSNICKNAME	= 432,
	ERR_NICKNAMEINUSE		= 433,
	ERR_NICKCOLLISION		= 436,
	ERR_USERNOTINCHANNEL	= 441,
	ERR_NOTONCHANNEL		= 442,
	ERR_USERONCHANNEL		= 443,
	ERR_NOLOGIN				= 444,
	ERR_SUMMONDISABLED		= 445,
	ERR_USERSDISABLED		= 446,
	ERR_NOTREGISTERED		= 451,
	ERR_NEEDMOREPARAMS		= 461,
	ERR_ALREADYREGISTRED	= 462,
	ERR_NOPERMFORHOST		= 463,
	ERR_PASSWDMISMATCH		= 464,
	ERR_YOUREBANNEDCREEP	= 465,
	ERR_KEYSET				= 467,
	ERR_CHANNELISFULL		= 471,
	ERR_UNKNOWNMODE			= 472,
	ERR_INVITEONLYCHAN		= 473,
	ERR_BANNEDFROMCHAN		= 474,
	ERR_BADCHANNELKEY		= 475,
	ERR_NOPRIVILEGES		= 481,
	ERR_CHANOPRIVSNEEDED	= 482,
	ERR_CANTKILLSERVER		= 483,
	ERR_NOOPERHOST			= 491,
	ERR_UMODEUNKNOWNFLAG	= 501,
	ERR_USERSDONTMATCH		= 502,
	RPL_NONE				= 300,
	RPL_USERHOST			= 302,
	RPL_ISON				= 303,
	RPL_AWAY				= 301,
	RPL_UNAWAY				= 305,
	RPL_NOWAWAY				= 306,
	RPL_WHOISUSER			= 311,
	RPL_WHOISSERVER			= 312,
	RPL_WHOISOPERATOR		= 313,
	RPL_WHOISIDLE			= 317,
	RPL_ENDOFWHOIS			= 318,
	RPL_WHOISCHANNELS		= 319,
	RPL_WHOWASUSER			= 314,
	RPL_ENDOFWHOWAS			= 369,
	RPL_LISTSTART			= 321,
	RPL_LIST				= 322,
	RPL_LISTEND				= 323,
	RPL_CHANNELMODEIS		= 324,
	RPL_NOTOPIC				= 331,
	RPL_TOPIC				= 332,
	RPL_INVITING			= 341,
	RPL_SUMMONING			= 342,
	RPL_VERSION				= 351,
	RPL_WHOREPLY			= 352,
	RPL_ENDOFWHO			= 315,
	RPL_NAMREPLY			= 353,
	RPL_ENDOFNAMES			= 366,
	RPL_LINKS				= 364,
	RPL_ENDOFLINKS			= 365,
	RPL_BANLIST				= 367,
	RPL_ENDOFBANLIST		= 368,
	RPL_INFO				= 371,
	RPL_ENDOFINFO			= 374,
	RPL_MOTDSTART			= 375,
	RPL_MOTD				= 372,
	RPL_ENDOFMOTD			= 376,
	RPL_YOUREOPER			= 381,
	RPL_REHASHING			= 382,
	RPL_TIME				= 391,
	RPL_USERSSTART			= 392,
	RPL_USERS				= 393,
	RPL_ENDOFUSERS			= 394,
	RPL_NOUSERS				= 395,
	RPL_TRACELINK			= 200,
	RPL_TRACECONNECTING		= 201,
	RPL_TRACEHANDSHAKE		= 202,
	RPL_TRACEUNKNOWN		= 203,
	RPL_TRACEOPERATOR		= 204,
	RPL_TRACEUSER			= 205,
	RPL_TRACESERVER			= 206,
	RPL_TRACENEWTYPE		= 208,
	RPL_TRACELOG			= 261,
	RPL_STATSLINKINFO		= 211,
	RPL_STATSCOMMANDS		= 212,
	RPL_STATSCLINE			= 213,
	RPL_STATSNLINE			= 214,
	RPL_STATSILINE			= 215,
	RPL_STATSKLINE			= 216,
	RPL_STATSYLINE			= 218,
	RPL_ENDOFSTATS			= 219,
	RPL_STATSLLINE			= 241,
	RPL_STATSUPTIME			= 242,
	RPL_STATSOLINE			= 243,
	RPL_STATSHLINE			= 244,
	RPL_UMODEIS				= 221,
	RPL_LUSERCLIENT			= 251,
	RPL_LUSEROP				= 252,
	RPL_LUSERUNKNOWN		= 253,
	RPL_LUSERCHANNELS		= 254,
	RPL_LUSERME				= 255,
	RPL_ADMINME				= 256,
	RPL_ADMINLOC1			= 257,
	RPL_ADMINLOC2			= 258,
	RPL_ADMINEMAIL			= 259
};

enum IRCUndocumented {
	RPL_CREATIONTIME		= 329,
	RPL_TOPICINFORMATION	= 333
};

enum IRCUnused {
	RPL_TRACECLASS			= 209,
	RPL_SERVICEINFO			= 231,
	RPL_SERVICE				= 233,
	RPL_SERVLISTEND			= 235,
	RPL_WHOISCHANOP			= 316,
	RPL_CLOSING				= 362,
	RPL_INFOSTART			= 373,
	ERR_YOUWILLBEBANNED		= 466,
	ERR_NOSERVICEHOST		= 492,
	RPL_STATSQLINE			= 217,
	RPL_ENDOFSERVICES		= 232,
	RPL_SERVLIST			= 234,
	RPL_KILLDONE			= 361,
	RPL_CLOSEEND			= 363,
	RPL_MYPORTIS			= 384,
	ERR_BADCHANMASK			= 476
};

// IRC Commands
#define kIRCPass				"PASS"
#define kIRCNick				"NICK"
#define kIRCUser				"USER"
#define kIRCOper				"OPER"
#define kIRCQuit				"QUIT"
#define kIRCSQuit				"SQUIT"
#define kIRCJoin				"JOIN"
#define kIRCPart				"PART"
#define kIRCMode				"MODE"
#define kIRCTopic				"TOPIC"
#define kIRCNames				"NAMES"
#define kIRCList				"LIST"
#define kIRCInvite				"INVITE"
#define kIRCKick				"KICK"
#define kIRCVersion				"VERSION"
#define kIRCStats				"STATS"
#define kIRCLinks				"LINKS"
#define kIRCTime				"TIME"
#define kIRCConnect				"CONNECT"
#define kIRCTrace				"TRACE"
#define kIRCAdmin				"ADMIN"
#define kIRCInfo				"INFO"
#define kIRCPrivmsg				"PRIVMSG"
#define kIRCNotice				"NOTICE"
#define kIRCWho					"WHO"
#define kIRCWhois				"WHOIS"
#define kIRCWhowas				"WHOWAS"
#define kIRCKill				"KILL"
#define kIRCPing				"PING"
#define kIRCPong				"PONG"
#define kIRCAway				"AWAY"
#define kIRCRehash				"REHASH"
#define kIRCRestart				"RESTART"
#define kIRCSummon				"SUMMON"
#define kIRCUsers				"USERS"
#define kIRCUserhost			"USERHOST"
#define kIRCIson				"ISON"
#define kIRCObject				"OBJECT"

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
