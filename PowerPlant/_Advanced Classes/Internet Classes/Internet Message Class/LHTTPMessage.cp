// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHTTPMessage.cp 			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#include <LHTTPMessage.h>
#include <UInternet.h>

#include <LArray.h>
#include <LArrayIterator.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <LComparator.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LHTTPMessage							Default Constructor
// ---------------------------------------------------------------------------
// Creates a new empty message.

LHTTPMessage::LHTTPMessage()
{
	InitMembers();
}

// ---------------------------------------------------------------------------
//	¥ LHTTPMessage							Constructor
// ---------------------------------------------------------------------------
//	This constructor takes a char * to a string containing a RFC822 style message
//	consisting of a properly formatted header and a message body seperated by a
//	CRLF pair.

//	The header will be copied to mHeader and parsed for standard info
//	like: MIME, Content-Type, Mime Boundary, etc.. Other fields will
//	be accessible via the "Arbitrary Field" accessors.
//
//	The body will be copied into mBody and no transformations will be applied to it.

LHTTPMessage::LHTTPMessage(const char * inMessage)
{
	InitMembers();
	SetMessage(inMessage);
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator
// ---------------------------------------------------------------------------

LHTTPMessage&
LHTTPMessage::operator = (const LHTTPMessage& copyMessage)
{
	mServer				= copyMessage.mServer;
	mUserAgent			= copyMessage.mUserAgent;
	mAllow				= copyMessage.mAllow;
	mBasicRealm			= copyMessage.mBasicRealm;
	mUserName			= copyMessage.mUserName;
	mPassword			= copyMessage.mPassword;
	mContentEncoding	= copyMessage.mContentEncoding;
	mHaveLastModified	= copyMessage.mHaveLastModified;
	mHaveModifiedSince	= copyMessage.mHaveModifiedSince;
	mLastModified		= copyMessage.mLastModified;

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LHTTPMessage							Destructor
// ---------------------------------------------------------------------------

LHTTPMessage::~LHTTPMessage()
{
	ResetMembers();
}


// ---------------------------------------------------------------------------
//	¥ InitMembers
// ---------------------------------------------------------------------------
//
// Initializes member variables for the object

void
LHTTPMessage::InitMembers()
{
	SetIsMIME(true);

	mHaveLastModified = false;
	mHaveModifiedSince = false;

	//Since message body is optional in many HTTP requests,
	//	set an empty body in case the user doesn't create
	//	one for us.
	SetMessageBody("", 0);

	LMIMEMessage::InitMembers();
}


// ---------------------------------------------------------------------------
//	¥ ResetMembers
// ---------------------------------------------------------------------------
//
// Re-initializes member variables for the object

void
LHTTPMessage::ResetMembers() {
	mServer = "";
	mUserAgent = "";
	mAllow = "";
	mBasicRealm = "";
	mUserName = "";
	mPassword = "";
	mContentEncoding = "";

	// LMIMEMessage::ResetMembers calls InitMembers for us
	LMIMEMessage::ResetMembers();
}


// ---------------------------------------------------------------------------
//	¥ GetContentLength
// ---------------------------------------------------------------------------
//
// Returns length of message content

UInt32
LHTTPMessage::GetContentLength()
{
	return GetMessageBodySize();
}


// ---------------------------------------------------------------------------
//	¥ SetPassword
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetPassword(const char * inPassword)
{
	mPassword = inPassword;
}


// ---------------------------------------------------------------------------
//	¥ GetUserName
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetPassword()
{
	return mPassword.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetUserName
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetUserName(const char * inUserName)
{
	mUserName = inUserName;
}


// ---------------------------------------------------------------------------
//	¥ GetUserName
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetUserName()
{
	return mUserName.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetContentEnc
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetContentEnc(const char * inEnc)
{
	mContentEncoding = inEnc;
}


// ---------------------------------------------------------------------------
//	¥ GetWWWAuth
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetContentEnc()
{
	return mContentEncoding.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetWWWAuth
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetWWWAuth(const char * inAuth)
{
	mBasicRealm = inAuth;
}


// ---------------------------------------------------------------------------
//	¥ GetWWWAuth
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetWWWAuth()
{
	return mBasicRealm.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetAllow
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetAllow(const char * inAllow)
{
	mAllow = inAllow;
}


// ---------------------------------------------------------------------------
//	¥ GetAllow
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetAllow()
{
	return mAllow.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetLastMod
// ---------------------------------------------------------------------------
// Will use the current clock date and time.

void
LHTTPMessage::SetLastMod()
{
	mHaveLastModified = true;
	::GetTime(&mLastModified);
}


// ---------------------------------------------------------------------------
//	¥ SetLastMod
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetLastMod(DateTimeRec * inDateTime)
{
	mHaveLastModified = true;
	::BlockMoveData(inDateTime, &mLastModified, sizeof(DateTimeRec));
}


// ---------------------------------------------------------------------------
//	¥ SetLastMod
// ---------------------------------------------------------------------------
// This version takes a char * assumed to be a RFC822 style formatted date/time
//	string and sets-up a DateTimeRec with the info.
//	Example date format: Sat, 13 Nov 1965 19:01:15 -0600
//	 NOTE: The GMT offset is handled here too since we are at it.

void
LHTTPMessage::SetLastMod(const char * inDateTime)
{
	StPointerBlock tempDateTime((SInt32) (PP_CSTD::strlen(inDateTime) + 1));
	PP_CSTD::strcpy(tempDateTime, inDateTime);
	mHaveLastModified = true;

	//Day of Week
	char * p = PP_CSTD::strtok(tempDateTime, ", ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (UInt16 i = 0; i < 7; i++) {
			if (CompareText(p,
							kRFC822Days[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char*)kRFC822Days[i]),
							nil) == 0) {
				mLastModified.dayOfWeek = (SInt16) ++i;
				break;
			}
		}
	}

	//Day of month
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mLastModified.day  = (SInt16) PP_CSTD::atoi(p);

	//Month
	p = PP_CSTD::strtok(nil, " ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (UInt16 i = 0; i < 12; i++) {
			if (CompareText(p,
							kRFC822Months[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char *)kRFC822Months[i]),
							nil) == 0) {
				mLastModified.month = (SInt16) ++i;
				break;
			}
		}
	}

	//Year
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mLastModified.year  = (SInt16) PP_CSTD::atoi(p);

	//Hour
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mLastModified.hour  = (SInt16) PP_CSTD::atoi(p);

	//Minute
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mLastModified.minute  = (SInt16) PP_CSTD::atoi(p);

	//Second
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mLastModified.second  = (SInt16) PP_CSTD::atoi(p);
}


// ---------------------------------------------------------------------------
//	¥ GetLastMod
// ---------------------------------------------------------------------------

DateTimeRec*
LHTTPMessage::GetLastMod()
{
	DateTimeRec*	lastMod = nil;

	if (mHaveLastModified) {
		lastMod = &mLastModified;
	}

	return lastMod;
}


// ---------------------------------------------------------------------------
//	¥ SetModSince
// ---------------------------------------------------------------------------
// Will use the current clock date and time.

void
LHTTPMessage::SetModSince()
{
	mHaveModifiedSince = true;
	::GetTime(&mModifiedSince);
}


// ---------------------------------------------------------------------------
//	¥ SetModSince
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetModSince(DateTimeRec * inDateTime)
{
	mHaveModifiedSince = true;
	::BlockMoveData(inDateTime, &mModifiedSince, sizeof(DateTimeRec));
}


// ---------------------------------------------------------------------------
//	¥ SetModSince
// ---------------------------------------------------------------------------
// This version takes a char * assumed to be a RFC822 style formatted date/time
//	string and sets-up a DateTimeRec with the info.
//	Example date format: Sat, 13 Nov 1965 19:01:15 -0600
//	 NOTE: The GMT offset is handled here too since we are at it.

void
LHTTPMessage::SetModSince(const char * inDateTime)
{
	StPointerBlock tempDateTime((SInt32) (PP_CSTD::strlen(inDateTime) + 1));
	PP_CSTD::strcpy(tempDateTime, inDateTime);

	mHaveModifiedSince = true;
	//Day of Week
	char * p = PP_CSTD::strtok(tempDateTime, ", ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (UInt16 i = 0; i < 7; i++) {
			if (CompareText(p,
							kRFC822Days[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char*)kRFC822Days[i]),
							nil) == 0) {
				mModifiedSince.dayOfWeek = (SInt16) ++i;
				break;
			}
		}
	}

	//Day of month
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mModifiedSince.day  = (SInt16) PP_CSTD::atoi(p);

	//Month
	p = PP_CSTD::strtok(nil, " ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (UInt16 i = 0; i < 12; i++) {
			if (CompareText(p,
							kRFC822Months[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char*)kRFC822Months[i]),
							nil) == 0) {
				mModifiedSince.month = (SInt16) ++i;
				break;
			}
		}
	}

	//Year
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mModifiedSince.year  = (SInt16) PP_CSTD::atoi(p);

	//Hour
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mModifiedSince.hour  = (SInt16) PP_CSTD::atoi(p);

	//Minute
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mModifiedSince.minute  = (SInt16) PP_CSTD::atoi(p);

	//Second
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mModifiedSince.second  = (SInt16) PP_CSTD::atoi(p);
}


// ---------------------------------------------------------------------------
//	¥ GetModSince
// ---------------------------------------------------------------------------

DateTimeRec *
LHTTPMessage::GetModSince()
{
	DateTimeRec*	modSince = nil;

	if (mHaveModifiedSince) {
		modSince = &mModifiedSince;
	}

	return modSince;
}


// ---------------------------------------------------------------------------
//	¥ SetServer
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetServer(const char * inServer)
{
	mServer = inServer;
}


// ---------------------------------------------------------------------------
//	¥ GetServer
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetServer()
{
	return mServer.c_str();
}


// ---------------------------------------------------------------------------
//	¥ SetUserAgent
// ---------------------------------------------------------------------------

void
LHTTPMessage::SetUserAgent(const char * inAgent)
{
	mUserAgent = inAgent;
}


// ---------------------------------------------------------------------------
//	¥ GetUserAgent
// ---------------------------------------------------------------------------

const char *
LHTTPMessage::GetUserAgent()
{
	return mUserAgent.c_str();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BuildHeader
// ---------------------------------------------------------------------------

void
LHTTPMessage::BuildHeader(LDynamicBuffer * outHeader)
{
	char tempString[1024];
	LMIMEMessage::BuildHeader(outHeader);

	//If header is custom we use it "as is"
	if (mCustomHeader)
		return;

	//User-Agent
	AddFieldToBuffer(kFieldUserAgent, GetUserAgent(), outHeader);

	//If-Modified-Since
	if (mHaveModifiedSince) {
		BuildDate(GetModSince(), tempString);
		AddFieldToBuffer(kFieldModSinceDate, tempString, outHeader);
	}

	//Authentication if necessary
	if ((mUserName.length() > 0) && (mPassword.length() > 0)) {
		char encodedAuthStr[256];
		PP_CSTD::sprintf(tempString, "%s:%s", mUserName.c_str(), mPassword.c_str());

		HTUU_encode((unsigned char *)tempString, (UInt16) PP_CSTD::strlen(tempString), encodedAuthStr);

		//7/23/98 PFV - To prevent folding of this field we build it
		//				here directly.
		PP_CSTD::sprintf(tempString, "%s: Basic %s\r\n", kFieldAuthorization, encodedAuthStr);
		*outHeader += tempString;
	}

	//Content-Length
	if (GetContentLength()) {
		PP_CSTD::sprintf(tempString, "%lu", GetContentLength());
		AddFieldToBuffer(kFieldContentLength, tempString, outHeader);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDefaultsFromHeader
// ---------------------------------------------------------------------------
// Pulls info out of mHeader and sets the object defaults. Intended to called after
//	mHeader is set.
// Presently searches for:
//	Server
//	UserAgent
//	Modified Since Date
//	Last Modified Date
// 	Allow
// 	Basic Realm (WWW-Authenticate)
// 	Content Encoding
//	MIME info

void
LHTTPMessage::GetDefaultsFromHeader()
{
	LHeaderField	theField;

	LMIMEMessage::GetDefaultsFromHeader();

	// Server
	if (UInternet::FindField(kFieldServer, mHeader.c_str(), &theField))
		SetServer(theField.GetBody());

	// UserAgent
	if (UInternet::FindField(kFieldUserAgent, mHeader.c_str(), &theField))
		SetUserAgent(theField.GetBody());

	// Modified Since Date
	if (UInternet::FindField(kFieldModSinceDate, mHeader.c_str(), &theField))
		SetModSince(theField.GetBody());

	// Last Modified Date
	if (UInternet::FindField(kFieldLastModDate, mHeader.c_str(), &theField))
		SetLastMod(theField.GetBody());

	// Allow
	if (UInternet::FindField(kFieldAllow, mHeader.c_str(), &theField))
		SetAllow(theField.GetBody());

	// Basic Realm (WWW-Authenticate)
	if (UInternet::FindField(kFieldWWWAuth, mHeader.c_str(), &theField))
		SetWWWAuth(theField.GetBody());

	// Content Encoding
	if (UInternet::FindField(kFieldContentEnc, mHeader.c_str(), &theField))
		SetContentEnc(theField.GetBody());
}


// ---------------------------------------------------------------------------
//	¥ BuildDate
// ---------------------------------------------------------------------------
// Build standard HTTP date format string

void
LHTTPMessage::BuildDate(DateTimeRec * theDateTime, char * outDateString)
{
	//Example: Sat, 13 Nov 1965 19:01:15 GMT
	PP_CSTD::sprintf(outDateString, "%s, %u %s %u %02u:%02u:%02u GMT",
			kRFC822Days[theDateTime->dayOfWeek - 1],
			theDateTime->day,
			kRFC822Months[theDateTime->month - 1],
			theDateTime->year,
			theDateTime->hour,
			theDateTime->minute,
			theDateTime->second);
}

PP_End_Namespace_PowerPlant
