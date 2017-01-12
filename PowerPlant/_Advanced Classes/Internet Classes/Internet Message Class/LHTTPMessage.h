// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPMessage.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LHTTPMessage
#define _H_LHTTPMessage
#pragma once

#include <LHeaderField.h>
#include <LHeaderFieldList.h>
#include <LMIMEMessage.h>
#include <LDynamicBuffer.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LHTTPMessage: public LMIMEMessage {
public:
						LHTTPMessage();
						LHTTPMessage(const char * inMessage);
	LHTTPMessage&		operator = (const LHTTPMessage& copyMessage);
	virtual 			~LHTTPMessage();
	virtual void		InitMembers();
	virtual void		ResetMembers();

	UInt32				GetContentLength();

	void				SetServer(const char * inServer);
	const char *		GetServer();

	void				SetUserAgent(const char * inServer);
	const char *		GetUserAgent();

	void				SetModSince();
	void				SetModSince(const char * inDateTime);
	void				SetModSince(DateTimeRec * inDateTime);
	DateTimeRec *		GetModSince();

	void				SetLastMod();
	void				SetLastMod(const char * inDateTime);
	void				SetLastMod(DateTimeRec * inDateTime);
	DateTimeRec *		GetLastMod();

	void				SetAllow(const char * inAllow);
	const char *		GetAllow();

	void				SetWWWAuth(const char * inAuth);
	const char *		GetWWWAuth();

	void				SetContentEnc(const char * inEnc);
	const char *		GetContentEnc();

	void				SetUserName(const char * inUserName);
	const char *		GetUserName();

	void				SetPassword(const char * inPassword);
	const char *		GetPassword();

protected:

	virtual void		GetDefaultsFromHeader();
	virtual void		BuildHeader(LDynamicBuffer * outHeader);
	void 				BuildDate(DateTimeRec * theDateTime, char * outDateString);

	PP_STD::string		mServer;
	PP_STD::string		mUserAgent;
	PP_STD::string		mAllow;
	PP_STD::string		mBasicRealm;
	PP_STD::string		mUserName;
	PP_STD::string		mPassword;
	PP_STD::string		mContentEncoding;
	Boolean				mHaveLastModified;
	Boolean				mHaveModifiedSince;
	DateTimeRec 		mModifiedSince;
	DateTimeRec 		mLastModified;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
