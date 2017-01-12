// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHeaderField.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LHeaderField
#define _H_LHeaderField
#pragma once

#include <LDynamicBuffer.h>
#include <string>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Message Field Names

const char kFieldMsgID[]			= "Message-Id";
const char kFieldDate[]				= "Date";
const char kFieldTo[]				= "To";
const char kFieldCC[]				= "CC";
const char kFieldBCC[]				= "BCC";
const char kFieldFrom[]				= "From";
const char kFieldSubject[]			= "Subject";
const char kFieldStatus[]			= "Status";
const char kFieldAllow[]			= "Allow";
const char kFieldAuthorization[]	= "Authorization";
const char kFieldContentLength[]	= "Content-Length";
const char kFieldContentEnc[]		= "Content-Encoding";
const char kFieldExpires[]			= "Expires";
const char kFieldModSinceDate[]		= "If-Modified-Since";
const char kFieldLastModDate[]		= "Last-Modified";
const char kFieldLocation[]			= "Location";
const char kFieldPragma[]			= "Pragma";
const char kFieldReferer[]			= "Referer";
const char kFieldServer[]			= "Server";
const char kFieldUserAgent[]		= "User-Agent";
const char kFieldWWWAuth[]			= "WWW-Authenticate";

// ---------------------------------------------------------------------------

const char kFoldIndent[]		= "\r\n     ";		// CRLF + 5 Spaces... arbitrary

const SInt16	kFieldOverHead	= 4;		// colon, space, CR, LF
const SInt32	kFieldFoldStart	= 60;		// Start looking to fold the field body at this char position

// ---------------------------------------------------------------------------

class	LHeaderField {
public:
					LHeaderField();

					LHeaderField(const char * inTitle, const char * inBody);

					LHeaderField(const char * inField);

		 			~LHeaderField();

	void			InitMembers();

	const char *	GetTitle();

	const char *	GetBody();

	void			SetTitle(const char * inTitle);
	void			SetTitle(const char * inTitle, UInt32 inLength);

	void			SetBody(const char * inBody);
	void			SetBody(const char * inBody, UInt32 inLength);

	void			GetField(PP_STD::string& outField);

	void			SetField(const char * inField);

	UInt32			FieldLength();

protected:
	void			UnfoldBody();
	void			Fold(const char * inField, PP_STD::string& outField);

	PP_STD::string	mTitle;
	PP_STD::string	mBody;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
