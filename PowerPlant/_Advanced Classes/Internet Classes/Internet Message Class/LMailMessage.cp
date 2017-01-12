// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMailMessage.cp 			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
// Class for dealing with Internet Style Mail Messages.

#include <LMailMessage.h>
#include <UInternet.h>
#include <LArrayIterator.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

#define kMaxSMTPRecipients	100

// ---------------------------------------------------------------------------
//		¥ LMailMessage
// ---------------------------------------------------------------------------
//	Constructor
//
// Creates a new empty message.

LMailMessage::LMailMessage()
{
	InitMembers();
	InitMessage();
}

// ---------------------------------------------------------------------------
//		¥ LMailMessage
// ---------------------------------------------------------------------------
//	Alternate Constructors
//
//	This constructor takes a char * to a string containing a RFC822 style message
//	consisting of a properly formatted header and a message body seperated by a
//	CRLF pair. (We assume any SMTP transfer encoding has been removed if necessary.)
//
//	The header will be copied to mHeader and parsed for standard info
//	like: To/CC, From, Subject, Date/GMT Offset, Message ID, etc.. Other fields will
//	be accessible via the "Arbitrary Field" accessors.
//
//	The body will be copied into mBody and no transformations will be applied to it.

LMailMessage::LMailMessage(const char * inMessage, UInt32 inLength)
{
	InitMembers();
	if (!inLength)
		inLength = PP_CSTD::strlen(inMessage);
	SetMessage(inMessage, inLength);
}

// ---------------------------------------------------------------------------
//		¥ LMIMEMessage
// ---------------------------------------------------------------------------
//	Alternate Constructor
//
//	This cosntructor takes a LDynamicBuffer * to a buffer containing a RFC822 style message
//	consisting of a properly formatted header and a message body seperated by a
//	CRLF pair.

//	The header will be copied to mHeader and parsed for standard info
//	Other fields will be accessible via the "Arbitrary Field" accessors.
//
//	The body will be copied into mBody and no transformations will be applied to it.

LMailMessage::LMailMessage(LDynamicBuffer * inBuffer)
{
	InitMembers();
	SetMessage(inBuffer);
}

// ---------------------------------------------------------------------------
//		¥ LMIMEMessage
// ---------------------------------------------------------------------------
//	Alternate Constructor
//
//	This cosntructor takes a LDynamicBuffer * to a buffer containing a RFC822 style message
//	consisting of a properly formatted header and a message body seperated by a
//	CRLF pair.

//	The header will be copied to mHeader and parsed for standard info
//	Other fields will be accessible via the "Arbitrary Field" accessors.
//
//	The body will be copied into mBody and no transformations will be applied to it.

LMailMessage::LMailMessage(LStream * inStream, SInt32 inLength)
{
	InitMembers();
	SetMessage(inStream, inLength);
}

// ---------------------------------------------------------------------------
//		¥ ~LMailMessage
// ---------------------------------------------------------------------------
//	Destructor
//

LMailMessage::~LMailMessage()
{
	ResetMembers();
}

// ---------------------------------------------------------------------------
//		¥ InitMembers
// ---------------------------------------------------------------------------
//
// Initializes member variables for the object

void
LMailMessage::InitMembers()
{
	mToList = nil;
	mCCList = nil;
	mBCCList = nil;

	mDateValid = false;
	mGMTOffset = 0;

	mRead = false;

	mCustomTo = false;
	mCustomCC = false;

	//MIME Support
	mIsMIME = false;
	mSegmentDepth = 1;

	//	MIME - Partial send/rcv globals
	mHaveBoundaryStart = false;
	mFinishedSegment = false;
	mSentSegmentSeperator = false;

	LMIMEMessage::InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ ResetMembers
// ---------------------------------------------------------------------------
//
// Re-initializes member variables for the object

void
LMailMessage::ResetMembers() {

	delete mToList;
	delete mCCList;
	delete mBCCList;

	mFrom = "";
	mSubject = "";
	mMessageID = "";
	mToField = "";
	mCCField = "";
	mMIMEPreamble = "";
	mContentType = "";
	mBoundary = "";

	// LMIMEMessage::ResetMembers calls InitMembers for us
	LMIMEMessage::ResetMembers();
}

// ---------------------------------------------------------------------------
//		¥ InitMessage
// ---------------------------------------------------------------------------
//
//	Setup defaults for new empty messages

void
LMailMessage::InitMessage()
{
	SetDateTime();
	SetGMTOffset();
	SetMessageID();
}

// ===========================================================================

#pragma mark -
#pragma mark === Mail Message Specific Accessors/Setters ===

// ---------------------------------------------------------------------------
//		¥ GetTo
// ---------------------------------------------------------------------------
//

LArray *
LMailMessage::GetTo()
{
	return mToList;
}

// ---------------------------------------------------------------------------
//		¥ GetCustomTo
// ---------------------------------------------------------------------------
//

const char *
LMailMessage::GetCustomTo()
{
	return mToField.c_str();
}

// ---------------------------------------------------------------------------
//		¥ GetCustomCC
// ---------------------------------------------------------------------------
//

const char *
LMailMessage::GetCustomCC()
{
	return mCCField.c_str();
}

// ---------------------------------------------------------------------------
//		¥ GetCC
// ---------------------------------------------------------------------------
//

LArray *
LMailMessage::GetCC()
{
	return mCCList;
}

// ---------------------------------------------------------------------------
//		¥ GetBCC
// ---------------------------------------------------------------------------
//

LArray *
LMailMessage::GetBCC()
{
	return mBCCList;
}

// ---------------------------------------------------------------------------
//		¥ SetTo
// ---------------------------------------------------------------------------
//	Object assumes ownership of the list.

void
LMailMessage::SetTo(LArray * inList)
{
	delete mToList;
	mToList = nil;

	mToList = inList;
}

// ---------------------------------------------------------------------------
//		¥ SetTo
// ---------------------------------------------------------------------------
//	This version takes a char * assumed to be comma delimited string of addresses
//	 and creates list. Unless the inCustom field is true. If inCustom is true, we
//	 assume you want this to be the "display" string for to field.

void
LMailMessage::SetTo(const char * inList, Boolean inCustom)
{
	mCustomTo = inCustom;

	if (inCustom) {
		mToField = inList;
	} else {
		delete mToList;
		mToList = nil;

		mToList = new LArray(sizeof(Str255));
		UInternet::CommaStringToList(inList, mToList);
	}
}

// ---------------------------------------------------------------------------
//		¥ SetCC
// ---------------------------------------------------------------------------
//	Object assumes ownership of the list.

void
LMailMessage::SetCC(LArray * inList)
{
	delete mCCList;
	mCCList = nil;

	mCCList = inList;
}

// ---------------------------------------------------------------------------
//		¥ SetCC
// ---------------------------------------------------------------------------
//	This version takes a char * assumed to be comma delimited string of addresses
//	 and creates list. Unless the inCustom field is true. If inCustom is true, we
//	 assume you want this to be the "display" string for to field.

void
LMailMessage::SetCC(const char * inList, Boolean inCustom)
{
	if (inCustom) {
		mCustomCC = inCustom;

		mCCField = inList;
	} else {
		delete mCCList;
		mCCList = nil;

		mCCList = new LArray(sizeof(Str255));
		UInternet::CommaStringToList(inList, mCCList);
	}
}

// ---------------------------------------------------------------------------
//		¥ SetBCC
// ---------------------------------------------------------------------------
//	Object assumes ownership of the list.

void
LMailMessage::SetBCC(LArray * inList)
{
	delete mBCCList;
	mBCCList = nil;

	mBCCList = inList;
}

// ---------------------------------------------------------------------------
//		¥ SetBCC
// ---------------------------------------------------------------------------
//	This version takes a char * assumed to be comma delimited string of addresses
//	 and creates list.

void
LMailMessage::SetBCC(const char * inList)
{
	delete mBCCList;
	mBCCList = nil;

	mBCCList = new LArray(sizeof(Str255));
	UInternet::CommaStringToList(inList, mBCCList);
}

// ---------------------------------------------------------------------------
//		¥ AddTo
// ---------------------------------------------------------------------------
//

void
LMailMessage::AddTo(const char * inItem)
{
	if (!mToList)
		mToList = new LArray(sizeof(Str255));

	mToList->InsertItemsAt(1, LArray::index_Last, inItem);
}

// ---------------------------------------------------------------------------
//		¥ AddCC
// ---------------------------------------------------------------------------
//

void
LMailMessage::AddCC(const char * inItem)
{
	if (!mCCList)
		mCCList = new LArray(sizeof(Str255));

	mCCList->InsertItemsAt(1, LArray::index_Last, inItem);
}

// ---------------------------------------------------------------------------
//		¥ AddBCC
// ---------------------------------------------------------------------------
//

void
LMailMessage::AddBCC(const char * inItem)
{
	if (!mBCCList)
		mBCCList = new LArray(sizeof(Str255));

	mBCCList->InsertItemsAt(1, LArray::index_Last, inItem);
}

// ---------------------------------------------------------------------------
//		¥ SetDateTime
// ---------------------------------------------------------------------------
// Will use the current clock date and time.

void
LMailMessage::SetDateTime()
{
	::GetTime(&mDateTime);
	mDateValid = true;
}

// ---------------------------------------------------------------------------
//		¥ SetDateTime
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetDateTime(DateTimeRec * inDateTime)
{
	::BlockMoveData(inDateTime, &mDateTime, sizeof(DateTimeRec));
	mDateValid = true;
}

// ---------------------------------------------------------------------------
//		¥ SetDateTime
// ---------------------------------------------------------------------------
// This version takes a char * assumed to be a RFC822 style formatted date/time
//	string and sets-up a DateTimeRec with the info.
//	Example date format: Sat, 13 Nov 1965 19:01:15 -0600
//	 NOTE: The GMT offset is handled here too since we are at it.

void
LMailMessage::SetDateTime(const char * inDateTime)
{
	Boolean			haveDayOfWeek = false;
	StPointerBlock	tempDate((SInt32) (PP_CSTD::strlen(inDateTime) + 1));
	PP_CSTD::strcpy(tempDate, inDateTime);

	//Initialize the data structure
	//	Dates are commonly found in non-standard forms... if we
	//	hit any bad date formats, we bail out and this structure may
	//	only be partially configured.
	mDateTime.year = 0;
	mDateTime.month = 0;
	mDateTime.day = 0;
	mDateTime.hour = 0;
	mDateTime.minute = 0;
	mDateTime.second = 0;
	mDateTime.dayOfWeek = 0;

	//Day of Week
	char * p = PP_CSTD::strtok(tempDate, ", ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (SInt16 i = 0; i < 7; i++) {
			if (CompareText(p,
							kRFC822Days[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char*)kRFC822Days[i]),
							nil) == 0) {
				mDateTime.dayOfWeek = ++i;
				haveDayOfWeek = true;
				break;
			}
		}
	} else
		return;

	if (haveDayOfWeek)
		p = PP_CSTD::strtok(nil, " ");
	else
		mDateTime.dayOfWeek = 0;

	//Day of month
	if (p)
		mDateTime.day  = (SInt16) PP_CSTD::atoi(p);
	 else
		return;

	//Month
	p = PP_CSTD::strtok(nil, " ");
	if (p) {
		SInt16 pLen = (SInt16) PP_CSTD::strlen(p);
		for (SInt16 i = 0; i < 12; i++) {
			if (CompareText(p,
							kRFC822Months[i],
							pLen,
							(SInt16) PP_CSTD::strlen((char*)kRFC822Months[i]),
							nil) == 0) {
				mDateTime.month = ++i;
				break;
			}
		}
	} else
		return;


	//Year
	p = PP_CSTD::strtok(nil, " ");
	if (p) {
		mDateTime.year  = (SInt16) PP_CSTD::atoi(p);
		//For years not including century
		// we assume the present century.
		if (mDateTime.year < 100) {
			DateTimeRec tempDate;
			::GetTime(&tempDate);

			//Get the part of the century we want
			tempDate.year = (SInt16) (tempDate.year/100);
			tempDate.year = (SInt16) (tempDate.year * 100);

			mDateTime.year += tempDate.year;
		}
	} else
		return;

	//Hour
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mDateTime.hour  = (SInt16) PP_CSTD::atoi(p);
	 else
		return;

	//Minute
	p = PP_CSTD::strtok(nil, ":");
	if (p)
		mDateTime.minute  = (SInt16) PP_CSTD::atoi(p);
	 else
		return;

	//Second
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		mDateTime.second  = (SInt16) PP_CSTD::atoi(p);
	 else
		return;

	//GMT
	p = PP_CSTD::strtok(nil, " ");
	if (p)
		SetGMTOffset(p);
	 else
		return;

	mDateValid = true;
}

// ---------------------------------------------------------------------------
//		¥ GetDate
// ---------------------------------------------------------------------------
//

DateTimeRec *
LMailMessage::GetDateTime()
{
	return &mDateTime;
}

// ---------------------------------------------------------------------------
//		¥ SetGMTOffset
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetGMTOffset()
{
	mGMTOffset = UInternet::GetGMTOffset()/(60 * 60);
}

// ---------------------------------------------------------------------------
//		¥ SetGMTOffset
// ---------------------------------------------------------------------------

void
LMailMessage::SetGMTOffset(SInt32 inOffset)
{
	mGMTOffset = inOffset;
}

// ---------------------------------------------------------------------------
//		¥ SetGMTOffset
// ---------------------------------------------------------------------------
//	This version takes char * to string in format found in date headers
//	Example: "-0700" or "-7"

void
LMailMessage::SetGMTOffset(const char * inOffset)
{
	mGMTOffset = PP_CSTD::atol(inOffset);
	//If string had trailing 0's then strip them
	if ((mGMTOffset >= 100) || (mGMTOffset <= -100)){
		mGMTOffset = mGMTOffset/100;
	}
}

// ---------------------------------------------------------------------------
//		¥ GetGMTOffset
// ---------------------------------------------------------------------------
//

SInt32
LMailMessage::GetGMTOffset()
{
	return mGMTOffset;
}

// ---------------------------------------------------------------------------
//		¥ SetFrom
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetFrom(const char * inFrom)
{
	mFrom = inFrom;
}

// ---------------------------------------------------------------------------
//		¥ GetFrom
// ---------------------------------------------------------------------------
//

const char *
LMailMessage::GetFrom()
{
	return mFrom.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetSubject
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetSubject(const char * inSubject)
{
	mSubject = inSubject;
}

// ---------------------------------------------------------------------------
//		¥ GetSubject
// ---------------------------------------------------------------------------
//

const char *
LMailMessage::GetSubject()
{
	return mSubject.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetMessageID
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetMessageID()
{
	DateTimeRec	theDateTime;
	char		tempMessageID[513];
	PP_CSTD::srand(TickCount());

	GetTime(&theDateTime);

	PP_CSTD::sprintf(tempMessageID,
		"%02u%02u%02u%02u%02u%02u.%6.6u",
		theDateTime.year,
		theDateTime.month,
		theDateTime.day,
		theDateTime.hour,
		theDateTime.minute,
		theDateTime.second,
		PP_CSTD::rand());

	SetMessageID(tempMessageID);
}

// ---------------------------------------------------------------------------
//		¥ SetMessageID
// ---------------------------------------------------------------------------
//

void
LMailMessage::SetMessageID(const char * inMessageID)
{
	//ensure that we get open/close brackets arround
	//	the message ID... some mailers "require" it.
	if (inMessageID[0] != '<') {
		mMessageID = '<';
		mMessageID += inMessageID;
		mMessageID += '>';
	} else {
		mMessageID = inMessageID;
	}
}

// ---------------------------------------------------------------------------
//		¥ GetMessageID
// ---------------------------------------------------------------------------
//

const char *
LMailMessage::GetMessageID()
{
	return mMessageID.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetStatus
// ---------------------------------------------------------------------------
//	Status presently only supports 'R' for read or unread

void
LMailMessage::SetStatus(const char * theStatus) {
	mRead = ((*theStatus == 'R') || (*theStatus == 'r'));
}

// ---------------------------------------------------------------------------
//		¥ GetStatus
// ---------------------------------------------------------------------------
//	Status presently only supports 'R' for read or unread

Boolean
LMailMessage::GetStatus() {
	return mRead;
}

// ===========================================================================

#pragma mark -
#pragma mark === Overridden Methods ===

// ---------------------------------------------------------------------------
//		¥ GetDefaultsFromHeader
// ---------------------------------------------------------------------------
// Pulls info out of mHeader and sets the object defaults. Intended to called after
//	mHeader is set.
// Presently searches for:
//	To, CC, & BCC
//	From
//	Subject
//	Date/GMT Offset
//	Message ID
//	Status

void
LMailMessage::GetDefaultsFromHeader()
{
	LHeaderField	theField;

	LMIMEMessage::GetDefaultsFromHeader();

	// Message ID
	if (UInternet::FindField(kFieldMsgID, mHeader.c_str(), &theField))
		SetMessageID(theField.GetBody());

	// FROM
	if (UInternet::FindField(kFieldFrom, mHeader.c_str(), &theField))
		SetFrom(theField.GetBody());

	// Subject
	if (UInternet::FindField(kFieldSubject, mHeader.c_str(), &theField))
		SetSubject(theField.GetBody());

	// Date/GMT Offset
	if (UInternet::FindField(kFieldDate, mHeader.c_str(), &theField))
		SetDateTime(theField.GetBody());

	//Status
	if (UInternet::FindField(kFieldStatus, mHeader.c_str(), &theField))
		SetStatus(theField.GetBody());

	// To
	if (UInternet::FindField(kFieldTo, mHeader.c_str(), &theField))
		SetTo(theField.GetBody());

	// CC
	if (UInternet::FindField(kFieldCC, mHeader.c_str(), &theField))
		SetCC(theField.GetBody());

	// BCC - This won't normally be found in incoming mail messages,
	//	however someone might be trying to parse an outgoing message
	//	with this class.
	if (UInternet::FindField(kFieldBCC, mHeader.c_str(), &theField))
		SetBCC(theField.GetBody());
}

// ---------------------------------------------------------------------------
//		¥ BuildHeader
// ---------------------------------------------------------------------------
//

void
LMailMessage::BuildHeader(LDynamicBuffer * outHeader)
{
	LMIMEMessage::BuildHeader(outHeader);

	//If header is custom we use it "as is"
	if (mCustomHeader)
		return;

	//Message-ID:
	AddFieldToBuffer(kFieldMsgID, GetMessageID(), outHeader);

	//Add the standard stuff... putting these at the end just makes
	//	it easier for humans to read them in a message.
	//Date:
	char tempString[256];
	BuildDate(tempString);
	AddFieldToBuffer(kFieldDate, tempString, outHeader);

	//To:
	if (mCustomTo)
		AddFieldToBuffer(kFieldTo, GetCustomTo(), outHeader);
	else
		AddFieldToBuffer(kFieldTo, GetTo(), outHeader);

	//CC:
	if (mCustomCC)
		AddFieldToBuffer(kFieldCC, GetCustomCC(), outHeader);
	else
		AddFieldToBuffer(kFieldCC, GetCC(), outHeader);

	//From:
	AddFieldToBuffer(kFieldFrom, GetFrom(), outHeader);

	//Subject:
	AddFieldToBuffer(kFieldSubject, GetSubject(), outHeader);
}

// ===========================================================================

#pragma mark -
#pragma mark === Utility Things ===

// ---------------------------------------------------------------------------
//	¥ ReadyToSend
// ---------------------------------------------------------------------------
//	Checks the message to see if it's in a state that's ready for sending
// Minimum req's (for now) are:
// 	From
//	To || CC || BCC
//	Total recipients < 100... no duplicate check done
//	Message body of some sort
//		Body must meet SMTP encode parmaeters:
//			-No High ASCII
//			-No lines longer than 1000 chars

Boolean
LMailMessage::ReadyToSend()
{
	if (mFrom.length() == 0)
		return false;

	if (!(mToList || mCCList || mBCCList))
		return false;

	//Count # of recipients... SMTP has limit of 100 per envelope
	UInt32 recipientCount = 0;
	if (mToList) {
		recipientCount = mToList->GetCount();
	}
	if (mCCList) {
		recipientCount += mCCList->GetCount();
	}
	if (mBCCList) {
		recipientCount += mBCCList->GetCount();
	}

	if (recipientCount > kMaxSMTPRecipients) {
		return false;
	}

	return CheckBody();
}


// ---------------------------------------------------------------------------
//	¥ CheckBody
// ---------------------------------------------------------------------------
//	Checks for a message body of some sort and recursively checks for embedded
//	messages as well if necessary.

Boolean
LMailMessage::CheckBody()
{
	if (mBody.GetBufferLength())
		return UInternet::CheckSMTPValid(&mBody);
	else if (mBodyList.GetCount() > 0) {
		LArrayIterator	iter(mBodyList);
		LMailMessage * currMessage;
		while(iter.Next(&currMessage)) {
			if (currMessage->mBody.GetBufferLength()) {
				if (!UInternet::CheckSMTPValid(&currMessage->mBody))
					return false;
			}
		}
		return true;
	}
	else
		return false;
}


// ---------------------------------------------------------------------------
//	¥ BuildDate
// ---------------------------------------------------------------------------
//	Build standard header date format string
void
LMailMessage::BuildDate(char * outDateString)
{
	if (!mDateValid) {
		*outDateString = '\0';
		return;
	}

	DateTimeRec *	theDateTime = GetDateTime();

	//Example: Sat, 13 Nov 1965 19:01:15 -0600
	//¥ÊGMT offset presently only handles full hours
	PP_CSTD::sprintf(outDateString, "%s, %u %s %u %02u:%02u:%02u %+03li00",
			kRFC822Days[theDateTime->dayOfWeek - 1],
			theDateTime->day,
			kRFC822Months[theDateTime->month - 1],
			theDateTime->year,
			theDateTime->hour,
			theDateTime->minute,
			theDateTime->second,
			GetGMTOffset());
}


// ---------------------------------------------------------------------------
//	¥ Clone
// ---------------------------------------------------------------------------
//	Creates a deep copy by duplicating the items in the Mail Message.

void
LMailMessage::Clone(const LMailMessage& inOriginal)
{
	ResetMembers();

	LMIMEMessage::Clone(inOriginal);

	mCustomTo = inOriginal.mCustomTo;
	if (mCustomTo) {
		SetTo(inOriginal.mToField.c_str(), inOriginal.mCustomTo);
	} else {
		if (inOriginal.mToList) {
			PP_STD::string strToCommaList;
			UInternet::ListToCommaString(inOriginal.mToList, strToCommaList);
			SetTo(strToCommaList.c_str(), inOriginal.mCustomTo);
		}
	}

	mCustomCC = inOriginal.mCustomCC;
	if (mCustomCC) {
		SetCC(inOriginal.mCCField.c_str(), inOriginal.mCustomCC);
	} else {
		if (inOriginal.mCCList) {
			PP_STD::string strCCCommaList;
			UInternet::ListToCommaString(inOriginal.mCCList, strCCCommaList);
			SetCC(strCCCommaList.c_str(), inOriginal.mCustomCC);
		}
	}

	if (inOriginal.mBCCList) {
		PP_STD::string strBCCCommaList;
		UInternet::ListToCommaString(inOriginal.mBCCList, strBCCCommaList);
		SetBCC(strBCCCommaList.c_str());
	}

	SetFrom(inOriginal.mFrom.c_str());
	SetSubject(inOriginal.mSubject.c_str());
	SetMessageID(inOriginal.mMessageID.c_str());

	mDateTime = inOriginal.mDateTime;
	mDateValid = inOriginal.mDateValid;
	mGMTOffset = inOriginal.mGMTOffset;

	mRead = inOriginal.mRead;
}


PP_End_Namespace_PowerPlant
