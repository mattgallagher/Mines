// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMIMEMessage.cp 			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
// Class for dealing with MIME Messages.
//
// Note: Use of mBodyList and mBody are mutually exclusive within a MIME message.

#include <LMIMEMessage.h>
#include <UInternet.h>

#include <LArray.h>
#include <LArrayIterator.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LMIMEMessage
// ---------------------------------------------------------------------------
//	Constructor
//
// Creates a new empty message.

LMIMEMessage::LMIMEMessage()
{
	InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ LMIMEMessage
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


LMIMEMessage::LMIMEMessage(const char * inMessage, UInt32 inLength)
{
	InitMembers();
	if (!inLength)
		inLength = PP_CSTD::strlen(inMessage);
	SetMessage(inMessage, inLength);
}

// ---------------------------------------------------------------------------
//		¥ LInternetMessage
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

LMIMEMessage::LMIMEMessage(LDynamicBuffer * inBuffer)
{
	InitMembers();
	SetMessage(inBuffer);
}

// ---------------------------------------------------------------------------
//		¥ LInternetMessage
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

LMIMEMessage::LMIMEMessage(LStream * inStream, SInt32 inLength)
{
	InitMembers();
	SetMessage(inStream, inLength);
}


// ---------------------------------------------------------------------------
//		¥ ~LMIMEMessage
// ---------------------------------------------------------------------------
//	Destructor
//

LMIMEMessage::~LMIMEMessage()
{
}

// ---------------------------------------------------------------------------
//		¥ InitMembers
// ---------------------------------------------------------------------------
//
// Initializes member variables for the object

void
LMIMEMessage::InitMembers()
{
	mIsMIME = false;
	mSegmentDepth = 1;

	//	Partial send/rcv globals
	mHaveBoundaryStart = false;
	mFinishedSegment = false;
	mSentSegmentSeperator = false;

	LInternetMessage::InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ ResetMembers
// ---------------------------------------------------------------------------
//
// Re-initializes member variables for the object

void
LMIMEMessage::ResetMembers()
{
	mMIMEPreamble = "";
	mContentType = "";
	mBoundary = "";

	// LInternetMessage::ResetMembers calls InitMembers for us
	LInternetMessage::ResetMembers();
}

// ===========================================================================

#pragma mark -
#pragma mark === Mail Message Specific Accessors/Setters ===

// ---------------------------------------------------------------------------
//		¥ AddMessageBodySegment
// ---------------------------------------------------------------------------
//	inMessage must have been created on the heap with new. The message object
//	assumes responsibility for deleting it.

void
LMIMEMessage::AddMessageBodySegment(LMIMEMessage * inMessage)
{
	ThrowIfNil_(inMessage);

	//Use of mBody and mBodyList are mutually exclusive within a mail message
	if (mBody.GetBufferLength())
		mBody.ResetBuffer();

	//Sanity Check... Don't recursively add ourselves. This would
	//	create an endless loop when we try to build the message.
	// NOTE: This does not prevent recursive additions within nested
	//		messages. Be careful out there.
	ThrowIf_(inMessage == this);

	//Let the new segment figure out its depth within this set
	//	this is done primarily so that boundaries within nested multi-part
	//	messages will be kept straight.
	inMessage->AdjustSegmentDepth(mSegmentDepth + 1);

	mBodyList.AppendItem(&inMessage);

	SetIsMIME();
}

// ---------------------------------------------------------------------------
//		¥ AdjustSegmentDepth
// ---------------------------------------------------------------------------
//	Reset all the message depths in case a nested set is added to another set

void
LMIMEMessage::AdjustSegmentDepth(UInt32 inDepth)
{
	mSegmentDepth = inDepth;

	if (mBodyList.GetCount() > 0) {
		LArrayIterator	iter(mBodyList);
		LMIMEMessage * currMessage;
		while(iter.Next(&currMessage)) {
			currMessage->AdjustSegmentDepth(mSegmentDepth + 1);
		}
	}
}

// ---------------------------------------------------------------------------
//		¥ GetMessageBodyList
// ---------------------------------------------------------------------------
//

LInternetMessageList *
LMIMEMessage::GetMessageBodyList()
{
	return &mBodyList;
}

// ---------------------------------------------------------------------------
//		¥ GetIsMIME
// ---------------------------------------------------------------------------
//

Boolean
LMIMEMessage::GetIsMIME()
{
	return mIsMIME;
}

// ---------------------------------------------------------------------------
//		¥ SetIsMIME
// ---------------------------------------------------------------------------
//

void
LMIMEMessage::SetIsMIME(Boolean inValue, const char * inVersion)
{
	mIsMIME = inValue;

	if (inValue) {
		mMIMEVers = inVersion;

		if (mIsMIME && (!mContentType.size()))
			SetContentType();	//Set default type
	} else {
		mMIMEVers = "";
		mContentType = "";
	}
}

// ---------------------------------------------------------------------------
//		¥ GetMIMEVersion
// ---------------------------------------------------------------------------
//

const char *
LMIMEMessage::GetMIMEVersion()
{
	return mMIMEVers.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetBoundary
// ---------------------------------------------------------------------------
//

void
LMIMEMessage::SetBoundary(const char * inBoundary)
{
	if (inBoundary) {
		mBoundary = inBoundary;
	} else {
		char tempBoundary[255];
		PP_CSTD::sprintf(tempBoundary, "==_Boundary-%lu_==", mSegmentDepth);
		mBoundary = tempBoundary;
	}
}

// ---------------------------------------------------------------------------
//		¥ GetBoundary
// ---------------------------------------------------------------------------
//

const char *
LMIMEMessage::GetBoundary()
{
	if (!mBoundary.size())
		SetBoundary();

	return mBoundary.c_str();
}

// ---------------------------------------------------------------------------
//		¥ SetContentType
// ---------------------------------------------------------------------------
//	Pass nil, no param, or empty string for inContentType if you want the class to determin a
//		default MIME type for this message.
//	If a multipart type is passed, we will scan for and set the boundary info
//		automatically.
//
//	Defaults are:
//		multipart/mixed - for messges with other imbedded LMIMEMessage objects
//		text/plain; charset="us-ascii" - for all others

void
LMIMEMessage::SetContentType(const char * inContentType)
{
	mContentType = "";

	if ((!inContentType) || (!*inContentType)) {
		if (mBodyList.GetCount() > 0)
			SetContentType(kMixedContentType);
		else
			SetContentType(kDefaultContentType);

		return;
	}

	mContentType = inContentType;

	//Check for multipart and extract boundary
	char tempStr[513];
	PP_CSTD::strcpy(tempStr, mContentType.c_str());

	//temporarily lower the string for our strstr calls
	::LowercaseText(tempStr, (SInt16) PP_CSTD::strlen(tempStr), nil);

	if (PP_CSTD::strstr(tempStr, kMultiContentType)) {
		char * boundaryStart = PP_CSTD::strstr(tempStr, kMIMEBoundaryTag);
		if (boundaryStart) {
			UInt32 boundaryPos = (UInt32) (boundaryStart - tempStr);
			PP_CSTD::strcpy(tempStr, mContentType.c_str());			//copy string again with orig. case
			boundaryStart += PP_CSTD::strlen(kMIMEBoundaryTag);
			if (*boundaryStart == '"') {
				char * boundaryEnd = PP_CSTD::strchr(++boundaryStart, '"');
				PP_STD::string tempString(boundaryStart, (UInt32) (boundaryEnd - boundaryStart));
				SetBoundary(tempString.c_str());
			}

			//remove boundary info from the field,
			//	we will put it back later when/if we build the header
			//Copy back to temp again for case-insensitive search
			mContentType.erase(boundaryPos);
		}
	}

	if (!GetIsMIME())
		SetIsMIME(true);
}

// ---------------------------------------------------------------------------
//		¥ GetContentType
// ---------------------------------------------------------------------------
//

const char *
LMIMEMessage::GetContentType()
{
	return mContentType.c_str();
}

// ---------------------------------------------------------------------------
//		¥ GetMIMEPreamble
// ---------------------------------------------------------------------------
//

const char *
LMIMEMessage::GetMIMEPreamble()
{
	return mMIMEPreamble.c_str();
}

void
LMIMEMessage::SetMIMEPreamble(const char * inPreamble, SInt32 inLength)
{
	if (!inLength)
		inLength = (SInt32) PP_CSTD::strlen(inPreamble);

	if (inLength > 0) {
		//strip extra trailing line terminations
		while ((inPreamble[inLength - 1] == CR) ||
					(inPreamble[inLength - 1] == LF)) {
				inLength--;
		}

		mMIMEPreamble.assign(inPreamble, (UInt32) inLength);

		if (mMIMEPreamble.length() > 0)
			mMIMEPreamble += CRLF;
	}
}

// ---------------------------------------------------------------------------
//		¥ GetMessageBodySize
// ---------------------------------------------------------------------------
//	Returns the size of the message body(ies). May not be 100% accurate as
//	it does not adjust for boundaries, preambles, etc. that will be added when
//	a message is created.

UInt32
LMIMEMessage::GetMessageBodySize()
{
	UInt32 totalSize = 0;
	if (mBodyList.GetCount() > 0) {
		LArrayIterator	iter(mBodyList);
		LMIMEMessage * currMessage;
		while(iter.Next(&currMessage)) {
			totalSize += currMessage->GetMessageBodySize();
		}
	} else {
		totalSize = (UInt32) mBody.GetBufferLength();
	}

	return totalSize;
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
//	MIME
// 	Content-Type & MIME Boundary (if multipart/mixed)

void
LMIMEMessage::GetDefaultsFromHeader()
{
	LHeaderField	theField;

	LInternetMessage::GetDefaultsFromHeader();

	//	MIME
	if (UInternet::FindField(kFieldMIME, mHeader.c_str(), &theField))
		SetIsMIME(true, theField.GetBody());

	// 	Content-Type
	if (UInternet::FindField(kFieldContentType, mHeader.c_str(), &theField)) {
		SetContentType(theField.GetBody());	//Set default type
	}
}

// ---------------------------------------------------------------------------
//		¥ BuildHeader
// ---------------------------------------------------------------------------
//

void
LMIMEMessage::BuildHeader(LDynamicBuffer * outHeader)
{
	LInternetMessage::BuildHeader(outHeader);

	//If header is custom we use it "as is"
	if (mCustomHeader)
		return;

	//MIME-Version
	if (GetIsMIME())
		AddFieldToBuffer(kFieldMIME, GetMIMEVersion(), outHeader);

	//Content-Type
	//Check content type for multi-part and append boundary to the field if so
	char tempStr[513];
	PP_CSTD::strcpy(tempStr, GetContentType());
	::LowercaseText(tempStr, (SInt16) PP_CSTD::strlen(tempStr), nil);
	if (PP_CSTD::strstr(tempStr, kMultiContentType)) {
		char mixedTypeBody[255];
		PP_CSTD::sprintf(mixedTypeBody, "%s; %s\"%s\"", GetContentType(), kMIMEBoundaryTag, GetBoundary());
		AddFieldToBuffer(kFieldContentType, mixedTypeBody, outHeader);
	} else {
		AddFieldToBuffer(kFieldContentType, GetContentType(), outHeader);
	}
}

// ---------------------------------------------------------------------------
//		¥ BuildMessageBody
// ---------------------------------------------------------------------------
// Returns full message body (including imbedded MIME semgments) in outMessage.
//	Returns false if no body available.

Boolean
LMIMEMessage::BuildMessageBody(LDynamicBuffer * outMessage)
{
	if (mBodyList.GetCount() > 0) {
		char tempBoundaryString[255];
		LDynamicBuffer * tempBuffer;
		LMIMEMessage * currMsgBody;
		LArrayIterator iter(mBodyList);
		PP_CSTD::sprintf(tempBoundaryString, "\r\n--%s\r\n", GetBoundary());
		while(iter.Next(&currMsgBody)) {
			outMessage->ConcatenateBuffer(GetMIMEPreamble());			//Preamble
			outMessage->ConcatenateBuffer(tempBoundaryString);			//Boundary

			tempBuffer = currMsgBody->GetMessage();				//Message Body
			outMessage->ConcatenateBuffer(tempBuffer->GetBufferStream());
			delete tempBuffer;
		}
		//Terminating Boundary
		PP_CSTD::sprintf(tempBoundaryString, "\r\n--%s--\r\n", GetBoundary());
		outMessage->ConcatenateBuffer(tempBoundaryString);

		return true;
	}
	else {
		return LInternetMessage::BuildMessageBody(outMessage);
	}
}

// ---------------------------------------------------------------------------
//		¥ SetMessageBody
// ---------------------------------------------------------------------------
// This routine copies the info passed.
// Here we check to see if we have a multi-part type. if so, we break it
//	up and set the message body segments.
// NOTE: This assumes you have set the header information first!
//	Assumes inBody to be null terminated string

void
LMIMEMessage::SetMessageBody(const char * inBody, UInt32 inLength)
{
	if (!inLength)
		inLength = PP_CSTD::strlen(inBody);

	//Use of mBody and mBodyList are mutually exclusive within a mail message
	if (mBodyList.GetCount() > 0)
		mBodyList.ResetList();

	//We assume you want a mBody if the content type has not been set
	const char * contentType = GetContentType();
	if (!contentType)
		SetContentType();

	char tempStr[513];
	PP_CSTD::strcpy(tempStr, GetContentType());
	::LowercaseText(tempStr, (SInt16) PP_CSTD::strlen(tempStr), nil);
	Boolean haveMultiType = PP_CSTD::strstr(tempStr, kMultiContentType) != 0;
	if (GetIsMIME() && haveMultiType) {
		if (mBody.GetBufferLength())
			mBody.ResetBuffer();

		char theBoundary[256];
		PP_CSTD::sprintf(theBoundary, "--%s", GetBoundary());

		//Check for boundary and get the MIME section preamble
		const char * nextStart, * nextEnd = inBody;
		nextStart = PP_CSTD::strstr(nextEnd, theBoundary);
		if (nextStart && (nextStart < (inBody + inLength)))
			SetMIMEPreamble(inBody, nextStart - inBody);
		else
			LInternetMessage::SetMessageBody(inBody, inLength);

		while (nextStart) {
			//Point past the boundary to the start of the section
			nextStart = UInternet::PointToNextLine(nextStart);
			if (!(*nextStart && (nextStart < (inBody + inLength))))
				break;

			//Find the terminating boundary
			nextEnd = PP_CSTD::strstr(nextStart, theBoundary);
			if (!(nextEnd && (nextEnd < (inBody + inLength))))
				break;

			LMIMEMessage * tempMessage;
			tempMessage = new LMIMEMessage(nextStart, (UInt32) (nextEnd - nextStart));

			AddMessageBodySegment(tempMessage);

			nextStart = nextEnd;	//Start of next section is end of last section
		}
	}
	else
		LInternetMessage::SetMessageBody(inBody, inLength);
}

// ---------------------------------------------------------------------------
//		¥ SetMessageBody
// ---------------------------------------------------------------------------
// This routine copies the info passed.
// Here we check to see if we have a multi-part type. if so, we break it
//	up and set the message body segments.
// NOTE: This assumes you have set the header information first!
//
//	We assume the stream marker is at the start of the body,
//		It will no longer point to the body when we return.

void
LMIMEMessage::SetMessageBody(LStream * inStream, SInt32 inLength, Boolean ownStream)
{
	//Use of mBody and mBodyList are mutually exclusive within a mail message
	if (mBodyList.GetCount() > 0)
		mBodyList.ResetList();

	//We assume you want a mBody if the content type has not been set
	const char * contentType = GetContentType();
	if (!contentType)
		SetContentType();

	char tempStr[513];
	PP_CSTD::strcpy(tempStr, GetContentType());
	::LowercaseText(tempStr, (SInt16) PP_CSTD::strlen(tempStr), nil);
	Boolean haveMultiType = PP_CSTD::strstr(tempStr, kMultiContentType) != 0;
	if (GetIsMIME() && haveMultiType) {
		if (mBody.GetBufferLength())
			mBody.ResetBuffer();

		//Check for boundary and get the MIME section preamble
		SInt32 streamStarts, sectionStarts = 0;
		streamStarts = inStream->GetMarker();
		if (FindBoundaryInStream(inStream)) {
			sectionStarts = inStream->GetMarker();
			StPointerBlock text(sectionStarts - streamStarts);
			inStream->SetMarker(streamStarts, streamFrom_Start);
			inStream->ReadData(text, sectionStarts - streamStarts);
			SetMIMEPreamble(text, sectionStarts - streamStarts);
			inStream->SetMarker(sectionStarts, streamFrom_Start);
		} else {
			LInternetMessage::SetMessageBody(inStream, inLength, ownStream);
		}

		while (sectionStarts) {
			//Point past the boundary to the start of the section
			UInternet::PointToNextLine(inStream);
			sectionStarts = inStream->GetMarker();

			if (!FindBoundaryInStream(inStream))
				break;

			SInt32 sectionEnds;
			sectionEnds = inStream->GetMarker();

			//reset stream marker to begining of section
			inStream->SetMarker(sectionStarts, streamFrom_Start);

			//Create new mail message and add to list
			LMIMEMessage * tempMessage;
			tempMessage = new LMIMEMessage(inStream, sectionEnds - sectionStarts);
			AddMessageBodySegment(tempMessage);

			sectionStarts = sectionEnds;
			//reset stream marker to end of last section
			inStream->SetMarker(sectionEnds, streamFrom_Start);
		}
	}
	else
		LInternetMessage::SetMessageBody(inStream, inLength, ownStream);
}


// ---------------------------------------------------------------------------
//		¥ SetPartialMessageBody
// ---------------------------------------------------------------------------
// This routine gets what it needs from the inBuffer and removes what it used.
// Here we check to see if we have a multi-part type. if so, we break it
//	up and set the message body segments.
//
// NOTE: For this to work most efficiently, you should pass relatively small
//		buffers in inBuffer. This will work best if the stream behind inBuffer
//		is resident in memory, however it will work with any type or size of buffer.


void
LMIMEMessage::SetPartialMessage(LDynamicBuffer * inBuffer)
{
	//set the header first so we can tell if we have multipart message
	//	if we don't find a header then just return
	if (mHeader.length() == 0) {
		if (!SetHeader(inBuffer, true))
			return;
		else {
			mCurrentWorkingSegment = 1;
			mHaveBoundaryStart = false;
		}
	}

	char tempStr[513];
	PP_CSTD::strcpy(tempStr, GetContentType());
	::LowercaseText(tempStr, (SInt16) PP_CSTD::strlen(tempStr), nil);
	Boolean haveMultiType = PP_CSTD::strstr(tempStr, kMultiContentType) != 0;
	if (GetIsMIME() && haveMultiType) {
		while (1) {
			LStream * inStream;
			SInt32 boundaryStarts, sectionStarts, sectionEnds;
			boundaryStarts = sectionStarts = sectionEnds = 0;

			inStream = inBuffer->GetBufferStream();
			inStream->SetMarker(0, streamFrom_Start);

			//look for boundary in stream
			//	pull out preamble, and set buffer to start at new section
			StHandleBlock currPreamble(1);
			**currPreamble = '\0';
			if(!mHaveBoundaryStart) {
				if (FindBoundaryInStream(inStream)) {

					//Bail out if this is the terminating boundary
					if (IsTerminationBoundary(inStream))
						break;

					sectionStarts = inStream->GetMarker();

					//Get the MIME section preamble if necessary
					if (sectionStarts != 0) {
						ThrowIfNot_(inBuffer->BufferToHandle(currPreamble, sectionStarts, true));
					}

					//now point past the boundary string and possible preamble and
					//	remove from message.
					inStream->SetMarker(sectionStarts, streamFrom_Start);
					UInternet::PointToNextLine(inStream);
					sectionStarts = inStream->GetMarker();
					inBuffer->RemoveFromBuffer(0, sectionStarts);
					mHaveBoundaryStart = true;

					//get stream again in case we need to use it again
					inStream = inBuffer->GetBufferStream();
					inStream->SetMarker(0, streamFrom_Start);
				} else {
					mHaveBoundaryStart = false;
					return;	//nothing to do until we get a boundary
				}
			}

			//Create new or get current message and call SetPartialMessage
			LMIMEMessage * currMsgBody;
			if (mBodyList.GetCount() < mCurrentWorkingSegment) {
				currMsgBody = new LMIMEMessage();
				AddMessageBodySegment(currMsgBody);
			} else {
				mBodyList.FetchItemAt((ArrayIndexT) mCurrentWorkingSegment, &currMsgBody);
			}

			StHandleLocker locked(currPreamble);
			currMsgBody->SetMIMEPreamble(*currPreamble);

			//Look for end... which may not be in buffer yet
			LDynamicBuffer tempBuffer;
			if (FindBoundaryInStream(inStream)) {
				sectionEnds = inStream->GetMarker();

				//copy out portion past boundary so we can pass a complete
				//	buffer to the imbedded message
				SInt32 leftOverLength = (inStream->GetLength() - sectionEnds);
				tempBuffer.SetBuffer(inStream, leftOverLength);
				inBuffer->RemoveFromBuffer(sectionEnds, leftOverLength);

				//Reset globals for next iteration
				mCurrentWorkingSegment++;
				mHaveBoundaryStart = false;
			}
			else
				sectionEnds = inStream->GetLength();

			currMsgBody->SetPartialMessage(inBuffer);

			//copy back any leftover data and try loop again, or break
			if (tempBuffer.GetBufferLength() > 0)
				inBuffer->SetStream(tempBuffer.DetachStream());
			else
				break;
		}
	}
	else
		LInternetMessage::SetPartialMessage(inBuffer);
}

// ---------------------------------------------------------------------------
//		¥ GetPartialMessage
// ---------------------------------------------------------------------------
//	Returns next segment of the message body in outMessage.
//	Returns true with the last segment of data
//
//	This routine useful when attempting to stream out a message. It prevents
//	any unnecessary duplication of data by handing off data streams where ever
//	possible.

Boolean
LMIMEMessage::GetPartialMessage(LDynamicBuffer * outMessage, Boolean inStartGet)
{
	outMessage->ResetBuffer();

	UInt32 bodyCount = mBodyList.GetCount();
	if (bodyCount == 0) {
		return LInternetMessage::GetPartialMessage(outMessage, inStartGet);
	}
	else {
		//Return header first time arround
		if (inStartGet) {
			mCurrentWorkingSegment = 1;
			mSentSegmentSeperator = false;
			mFinishedSegment = true;	//Set to true as indication of start to GetPartialMessage
			BuildHeader(outMessage);
			// add header seperation
			outMessage->ConcatenateBuffer(CRLF);
			return false;
		}

		//Return true once completed all the body segments
		if (mCurrentWorkingSegment > bodyCount) {
			//We're done with this segment, return terminating Boundary
			char tempBoundaryString[255];
			PP_CSTD::sprintf(tempBoundaryString, "\r\n--%s--\r\n", GetBoundary());
			outMessage->ConcatenateBuffer(tempBoundaryString);
			return true;
		}

		//send the boundary sep or the MIME segment
		if (!mSentSegmentSeperator)	{
			char tempBoundaryString[255];
			PP_CSTD::sprintf(tempBoundaryString, "\r\n--%s\r\n", GetBoundary());
			outMessage->ConcatenateBuffer(tempBoundaryString);
			mSentSegmentSeperator = true;
			return false;
		} else {
			LMIMEMessage * currMsgBody;
			mBodyList.FetchItemAt((ArrayIndexT) mCurrentWorkingSegment, &currMsgBody);
			mFinishedSegment = currMsgBody->GetPartialMessage(outMessage, mFinishedSegment);
		}

		//If done with this segment, reset for next time.
		if (mFinishedSegment) {
			mCurrentWorkingSegment++;
			mFinishedSegment = true;	//Set to true as indication of start to GetPartialMessage
			mSentSegmentSeperator = false;
		}

		return false;
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === Utility Things ===

// ---------------------------------------------------------------------------
//		¥ FindBoundaryInStream
// ---------------------------------------------------------------------------
// Utility function to scan a stream (from the current maker) for the begining
//	of a MIME boundary.
//	Marker is moved to begining of the boundary designation

Boolean
LMIMEMessage::FindBoundaryInStream(LStream * inStream)
{
	SInt32 oldMarker = inStream->GetMarker();
	PP_STD::string theBoundary = GetBoundary();
	SInt32 boundaryLen = (SInt32) theBoundary.size();

	while (1) {
		//scan for -- at start of lines
		while (1) {
			char dashTest[2];
			if (inStream->PeekData(dashTest, 2) != 2) {	//Peek so we can read empty lines correctly
				inStream->SetMarker(oldMarker, streamFrom_Start);
				return false;
			}

			if ((dashTest[0] == '-') && (dashTest[1] == '-')) {
				inStream->SetMarker(2, streamFrom_Marker);
				break;
			}

			UInternet::PointToNextLine(inStream);
		}

		//get the chars following the '--' and check against boundary
		char tempData[256];
		if (inStream->PeekData(tempData, boundaryLen) != boundaryLen){
			inStream->SetMarker(oldMarker, streamFrom_Start);
			return false;
		}

		PP_STD::string testBounday;
		testBounday.assign(tempData, (UInt32) boundaryLen);

		if (testBounday == theBoundary) {
			//point back to start of boundary
			inStream->SetMarker(-2, streamFrom_Marker);
			return true;
		}
	}
}

// ---------------------------------------------------------------------------
//		¥ IsTerminationBoundary
// ---------------------------------------------------------------------------
// Utility function to test current stream position for termination boundary

Boolean
LMIMEMessage::IsTerminationBoundary(LStream * inStream)
{
	//Create termination boundary
	PP_STD::string theBoundary = "--";
	theBoundary += GetBoundary();
	theBoundary += "--";
	SInt32 boundaryLen = (SInt32) theBoundary.size();

	//Get the data at the head of the stream
	char tempData[256];
	if (inStream->PeekData(tempData, boundaryLen) != boundaryLen)
		return false;
	PP_STD::string testBounday;
	testBounday.assign(tempData, (UInt32) boundaryLen);

	//Test for termination boundary
	if (testBounday == theBoundary) {
		return true;
	}

	return false;
}


// ---------------------------------------------------------------------------
//		¥ Clone
// ---------------------------------------------------------------------------
//	Creates a deep copy by duplicating the items in the MIME Message.

void
LMIMEMessage::Clone(const LMIMEMessage& inOriginal)
{
	ResetMembers();

	LInternetMessage::Clone(inOriginal);

	//MIME Support
	LMIMEMessage*	currField;
	LArrayIterator	iter(inOriginal.mBodyList);
	LMIMEMessage*	tempMessage = nil;
	while(iter.Next(&currField)) {
		tempMessage = new LMIMEMessage();
		tempMessage->Clone(*currField);
		AddMessageBodySegment(tempMessage);
	}

	SetIsMIME(inOriginal.mIsMIME, inOriginal.mMIMEVers.c_str());
	SetMIMEPreamble(inOriginal.mMIMEPreamble.c_str());
	SetContentType(inOriginal.mContentType.c_str());
	SetBoundary(inOriginal.mBoundary.c_str());
	mSegmentDepth = inOriginal.mSegmentDepth;

	//Partial Get/Put globals
	mCurrentWorkingSegment = inOriginal.mCurrentWorkingSegment;
	mFinishedSegment = inOriginal.mFinishedSegment;
	mSentSegmentSeperator = inOriginal.mSentSegmentSeperator;
	mHaveBoundaryStart = inOriginal.mHaveBoundaryStart;
}


PP_End_Namespace_PowerPlant
