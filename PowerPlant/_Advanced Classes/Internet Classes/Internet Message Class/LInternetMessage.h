// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMessage.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Class for doing basic RFC822 style messages with simple Header and Message
//	Body. See LMailMessage or LHTTPMessage for specific implimentations.

#ifndef _H_LInternetMessage
#define _H_LInternetMessage
#pragma once

#include <LHeaderField.h>
#include <LHeaderFieldList.h>
#include <LDynamicBuffer.h>
#include <LArray.h>

#include <string>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const char kDefaultMIMEVers[]		= "1.0";
const char kFieldMIME[]				= "MIME-Version";
const char kFieldContentType[]		= "Content-Type";
const char kDefaultContentType[]	= "text/plain; charset=\"us-ascii\"";
const char kMultiContentType[]		= "multipart";
const char kMixedContentType[]		= "multipart/mixed";
const char kMIMEBoundaryTag[]		= "boundary=";

// ---------------------------------------------------------------------------
// Standard RFC822 Month and Day definitions

static const Str32 kRFC822Months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const Str32 kRFC822Days[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

// ---------------------------------------------------------------------------

class	LInternetMessage {
public:
								LInternetMessage();
								LInternetMessage(const char * inMessage, UInt32 inLength = 0);
								LInternetMessage(LDynamicBuffer * inBuffer);
	virtual 					~LInternetMessage();
	virtual void				InitMembers();
	virtual void				ResetMembers();

	virtual Boolean				HasCustomHeader() { return mCustomHeader; }

	virtual LDynamicBuffer *	GetMessage();
	virtual void				GetMessage(LDynamicBuffer * outBuffer);
	virtual void				SetMessage(const char * inMessage, UInt32 inLength = 0);
	virtual void				SetMessage(LDynamicBuffer * inBuffer);
	virtual void				SetMessage(LStream * inStream, SInt32 inLength = -1);

	virtual Boolean				GetPartialMessage(LDynamicBuffer * outMessage, Boolean inStartGet);
	virtual void				SetPartialMessage(LDynamicBuffer * inBuffer);

	virtual LDynamicBuffer *	GetHeader();
	virtual Boolean				SetHeader(LDynamicBuffer * inBuffer, Boolean inRemoveFromBuffer = false);
	virtual	void				SetHeader(const char * inHeader, UInt32 inLength = 0);

	virtual LDynamicBuffer *	GetMessageBody();
	virtual void				SetMessageBody(const char * inBody, UInt32 inLength = 0);
	virtual void				SetMessageBody(LStream * inStream, SInt32 inLength = -1, Boolean ownStream = false);

	virtual LHeaderFieldList *	GetArbitraryFieldList();
	virtual Boolean				GetArbitraryField(const char * inFieldTitle, LHeaderField * outHeaderField);
	virtual Boolean				GetArbitraryField(const char * inFieldTitle, ArrayIndexT * outLocation);
	virtual void				SetArbitraryField(const char * inFieldTitle, const char * inFieldContents = nil);
	virtual void				RemoveArbitraryField(const char * inFieldTitle);
	virtual inline void			AddArbitraryField(const char * inFieldTitle, const char * inFieldContents) {
										SetArbitraryField(inFieldTitle, inFieldContents);
									}

	virtual LDynamicBuffer * 	GetInternalMessageBody() { return &mBody; }
	virtual PP_STD::string *   	GetInternalHeaderString() { return &mHeader; }

	void						Clone(const LInternetMessage& inOriginal);

protected:
	virtual void		GetDefaultsFromHeader();
	virtual void		BuildHeader(LDynamicBuffer * outHeader);
	virtual Boolean		BuildMessageBody(LDynamicBuffer * outMessage);
	void				AddFieldToBuffer(LHeaderField * inField, LDynamicBuffer * outBuffer);
	void				AddFieldToBuffer(const char * inTitle, const char * inBody, LDynamicBuffer * outBuffer);
	void				AddFieldToBuffer(const char * inTitle, LArray * inList, LDynamicBuffer * outBuffer);

	PP_STD::string		mHeader;
	LDynamicBuffer		mBody;
	LHeaderFieldList	mFieldList;
	Boolean				mCustomHeader;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
