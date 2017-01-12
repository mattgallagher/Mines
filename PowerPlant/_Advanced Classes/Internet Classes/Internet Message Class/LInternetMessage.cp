// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMessage.cp 		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Class for doing basic RFC822 style messages with simple Header and Message
//	Body. See LMailMessage or LHTTPMessage for specific implimentations.


#include <LInternetMessage.h>
#include <UInternet.h>

#include <LArray.h>
#include <LArrayIterator.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <LComparator.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LInternetMessage
// ---------------------------------------------------------------------------
//	Constructor
//
// Creates a new empty message.

LInternetMessage::LInternetMessage()
{
	InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ LInternetMessage
// ---------------------------------------------------------------------------
//	Alternate Constructor
//
//	This cosntructor takes a char * to a string containing a RFC822 style message
//	consisting of a properly formatted header and a message body seperated by a
//	CRLF pair.

//	The header will be copied to mHeader and parsed for standard info
//	Other fields will be accessible via the "Arbitrary Field" accessors.
//
//	The body will be copied into mBody and no transformations will be applied to it.


LInternetMessage::LInternetMessage(const char * inMessage, UInt32 inLength)
{
	InitMembers();
	if (!inLength)
		inLength = PP_CSTD::strlen(inMessage);
	SetMessage(inMessage, inLength);
}

// ---------------------------------------------------------------------------
//	¥ LInternetMessage
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

LInternetMessage::LInternetMessage(LDynamicBuffer * inBuffer)
{
	InitMembers();
	SetMessage(inBuffer);
}


// ---------------------------------------------------------------------------
//	¥ ~LInternetMessage						Destructor
// ---------------------------------------------------------------------------

LInternetMessage::~LInternetMessage()
{
}

// ---------------------------------------------------------------------------
//		¥ InitMembers
// ---------------------------------------------------------------------------
//
// Initializes member variables for the object

void
LInternetMessage::InitMembers()
{
	mBody.ResetBuffer();
	mHeader = "";
	mCustomHeader = false;
	mFieldList.ResetList();
}

void
LInternetMessage::ResetMembers() {
	InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ SetPartialMessage
// ---------------------------------------------------------------------------
// This routine copies the info passed.

void
LInternetMessage::SetPartialMessage(LDynamicBuffer * inBuffer)
{
	//set the header first
	//	if we don't find a header then just return
	if (mHeader.length() == 0) {
		if (!SetHeader(inBuffer, true))
			return;
	}

	mBody += inBuffer;
	//remove what we used (which is everything) from the buffer
	inBuffer->ResetBuffer();
}

// ---------------------------------------------------------------------------
//		¥ SetMessage
// ---------------------------------------------------------------------------
//	Takes LDynamicBuffer. Parses header and body and sets
//	message information.

void
LInternetMessage::SetMessage(LDynamicBuffer * inBuffer)
{
	ResetMembers();
	SInt32 headerLen = UInternet::FindBodyStarts(inBuffer);
	if (headerLen) {
		StHandleBlock textH(Size_Zero);
		ThrowIfNot_(inBuffer->BufferToHandle(textH, headerLen));
		SetHeader(*textH, (UInt32) headerLen);
	}

	LStream * inStream = inBuffer->GetBufferStream();
	//set stream pos to start of body
	inStream->SetMarker(headerLen, streamFrom_Start);
	SetMessageBody(inStream, (inStream->GetLength() - inStream->GetMarker()));
}

// ---------------------------------------------------------------------------
//		¥ SetMessage
// ---------------------------------------------------------------------------
//	Takes LStream and optional length. Parses header and body and sets message
//	information. Assumes full stream and resets stream marker if inLength < 0.

void
LInternetMessage::SetMessage(LStream * inStream, SInt32 inLength)
{
	ResetMembers();

	// If no inLength specified, assume the whole stream and reset the marker
	if (inLength < 0) {
		inStream->SetMarker(0, streamFrom_Start);
		inLength = inStream->GetLength();
	}

	SInt32 streamStarts = inStream->GetMarker();

	SInt32 headerEnds = UInternet::FindBodyStarts(inStream, inLength);
	SInt32 headerLen = headerEnds - streamStarts;
	if (headerLen) {
		StPointerBlock tempP(headerLen);
		inStream->SetMarker(streamStarts, streamFrom_Start);
		inStream->ReadData(tempP, headerLen);
		SetHeader(tempP, (UInt32) headerLen);
	}

	//set stream pos to start of body
	inStream->SetMarker(streamStarts, streamFrom_Start);
	inStream->SetMarker(headerLen, streamFrom_Marker);
	SetMessageBody(inStream, (inLength - headerLen));
}

// ---------------------------------------------------------------------------
//		¥ SetMessage
// ---------------------------------------------------------------------------
//	Takes a pointer to an array of chars and a length. Parses header and body and sets
//	message information.

void
LInternetMessage::SetMessage(const char * inMessage, UInt32 inLength)
{
	if (!inLength)
		inLength = PP_CSTD::strlen(inMessage);

	UInt32 headerLength = 0;
	ResetMembers();
	char * p = UInternet::FindBodyStarts(inMessage, inLength);
	if (p != inMessage) {
		headerLength = (UInt32) (p - inMessage);
		SetHeader(inMessage, headerLength);
	}

	SetMessageBody(p, inLength - headerLength);
}

// ---------------------------------------------------------------------------
//		¥ GetMessage
// ---------------------------------------------------------------------------
//	Returns a fully qualified RFC822 style message with header and message body
// Calling function is responsible for deleting the LDynamicBuffer returned from this
// method.

LDynamicBuffer *
LInternetMessage::GetMessage()
{
	LDynamicBuffer * returnBuffer = new LDynamicBuffer;

	BuildHeader(returnBuffer);

	//Add the header seperation
	returnBuffer->ConcatenateBuffer(CRLF);

	BuildMessageBody(returnBuffer);

	return returnBuffer;
}

// ---------------------------------------------------------------------------
//		¥ GetMessage
// ---------------------------------------------------------------------------
//	Returns a fully qualified RFC822 style message with header and message body
//	within the LDynamicBuffer specified.

void
LInternetMessage::GetMessage(LDynamicBuffer * outBuffer)
{
	BuildHeader(outBuffer);

	//Add the header seperation
	outBuffer->ConcatenateBuffer(CRLF);

	BuildMessageBody(outBuffer);
}

// ---------------------------------------------------------------------------
//		¥ SetHeader
// ---------------------------------------------------------------------------
// This routine copies the header info passed.
//	NOTE:	By stipulating a header with this function you assume responsibility for
//			building all aspects of the header for outgoing messages.
//	Returns true if header found in buffer and header information set.

Boolean
LInternetMessage::SetHeader(LDynamicBuffer * inBuffer, Boolean inRemoveFromBuffer)
{
	SInt32 headerLen = UInternet::FindBodyStarts(inBuffer);
	if (headerLen) {
		StHandleBlock textH(Size_Zero);
		ThrowIfNot_(inBuffer->BufferToHandle(textH, headerLen));
		StHandleLocker locked(textH);

		//Set the message header
		SetHeader(*textH, (UInt32) headerLen);

		//remove the header from the buffer if requested
		if (inRemoveFromBuffer)
			inBuffer->RemoveFromBuffer(0, headerLen);

		return true;
	}

	return false;
}

// ---------------------------------------------------------------------------
//		¥ SetHeader
// ---------------------------------------------------------------------------
// This routine copies the header info passed.
//	NOTE:	By stipulating a header with this function you assume responsibility for
//			building all aspects of the header for outgoing messages.

void
LInternetMessage::SetHeader(const char * inHeader, UInt32 inLength)
{
	if (!inLength)
		inLength = PP_CSTD::strlen(inHeader);

	mCustomHeader = true;

	//For consitent results, we remove extra trailing CR's & LF's from the
	//	end of the header we are passed.
	while ((inHeader[inLength - 1] == CR) ||
				(inHeader[inLength - 1] == LF)) {
			inLength--;
		if (inLength == 0)
			break;
	}

	mHeader.assign(inHeader, inLength);
	mHeader += CRLF;

	GetDefaultsFromHeader();
}

// ---------------------------------------------------------------------------
//		¥ GetHeader
// ---------------------------------------------------------------------------
// Returns new LDynamicBuffer the user is responsible for deleting

LDynamicBuffer *
LInternetMessage::GetHeader()
{
	LDynamicBuffer * returnBuffer = new LDynamicBuffer;
	BuildHeader(returnBuffer);
	return returnBuffer;
}

// ---------------------------------------------------------------------------
//		¥ SetMessageBody
// ---------------------------------------------------------------------------
// This routine copies the info passed.

void
LInternetMessage::SetMessageBody(const char * inBody, UInt32 inLength)
{
	if (!inLength)
		inLength = PP_CSTD::strlen(inBody);
	mBody.SetBuffer(inBody, (SInt32) inLength);
}

// ---------------------------------------------------------------------------
//		¥ SetMessageBody
// ---------------------------------------------------------------------------
// This routine copies the info passed only if ownStream == false.
//	If ownStream == true, you should not attempt to delete the stream yourself.

void
LInternetMessage::SetMessageBody(LStream * inStream, SInt32 inLength, Boolean ownStream)
{
	if (ownStream)
		mBody.SetStream(inStream, ownStream);
	else
		mBody.SetBuffer(inStream, inLength);
}

// ---------------------------------------------------------------------------
//		¥ GetMessageBody
// ---------------------------------------------------------------------------
// Returns new LDynamicBuffer the user is responsible for deleting

LDynamicBuffer *
LInternetMessage::GetMessageBody()
{
	LDynamicBuffer * returnBuffer = new LDynamicBuffer;
	BuildMessageBody(returnBuffer);
	return returnBuffer;
}

// ---------------------------------------------------------------------------
//		¥ GetPartialMessage
// ---------------------------------------------------------------------------
//	Returns true with the last segment of data
//
//	This routine useful when attempting to stream out a message. It prevents
//	any unnecessary duplication of data by handing off data streams where ever
//	possible.

Boolean
LInternetMessage::GetPartialMessage(LDynamicBuffer * outMessage, Boolean inStartGet)
{
	//Return header first time arround
	if (inStartGet) {
		BuildHeader(outMessage);
		// add header seperation
		outMessage->ConcatenateBuffer(CRLF);
		return false;
	} else {
		outMessage->SetStream(mBody.GetBufferStream(), false);
		return true;
	}
}

// ---------------------------------------------------------------------------
//		¥ GetArbitraryFieldList
// ---------------------------------------------------------------------------
// Returns list of header fields.

LHeaderFieldList *
LInternetMessage::GetArbitraryFieldList()
{
	return &mFieldList;
}

// ---------------------------------------------------------------------------
//		¥ GetArbitraryField
// ---------------------------------------------------------------------------
// Returns true if field found. If found the field information will be place
//	 into 'outHeaderField.'

Boolean
LInternetMessage::GetArbitraryField(const char * inFieldTitle, LHeaderField * outHeaderField)
{
	if (mCustomHeader) {
		return UInternet::FindField(inFieldTitle, mHeader.c_str(), outHeaderField);
	} else {
		ArrayIndexT theLocation;
		if (GetArbitraryField(inFieldTitle, &theLocation))
			return mFieldList.FetchItemAt(theLocation, &outHeaderField);
	}
	return false;
}

// ---------------------------------------------------------------------------
//		¥ GetArbitraryField
// ---------------------------------------------------------------------------
// Returns true if field found. If found 'outLocation' will contain the location
//	 of the field in mFieldList.

Boolean
LInternetMessage::GetArbitraryField(const char * inFieldTitle, ArrayIndexT * outLocation)
{
	LHeaderField * tempField = new LHeaderField(inFieldTitle, "");
	*outLocation = mFieldList.FetchIndexOf(&tempField);
	delete tempField;

	return (Boolean)(*outLocation);
}

// ---------------------------------------------------------------------------
//		¥ SetArbitraryField
// ---------------------------------------------------------------------------
//	Exclude or pass nil for 'inFieldContents' to remove a field if
//	if exists.

void
LInternetMessage::SetArbitraryField(const char * inFieldTitle, const char * inFieldContents)
{
	RemoveArbitraryField(inFieldTitle);

	if (!inFieldContents)
		return;

	if (mCustomHeader) {
		LHeaderField tempField(inFieldTitle, inFieldContents);
		PP_STD::string tempString;

		tempField.GetField(tempString);
		mHeader += tempString;

		GetDefaultsFromHeader();
	} else {
		//the "new" object is deleted by the destructor of the list
		LHeaderField * tempField = new LHeaderField(inFieldTitle, inFieldContents);
		mFieldList.InsertItemsAt(1, LArray::index_Last, &tempField);
	}
}

// ---------------------------------------------------------------------------
//		¥ RemoveArbitraryField
// ---------------------------------------------------------------------------

void
LInternetMessage::RemoveArbitraryField(const char * inFieldTitle)
{
	if (mCustomHeader) {
		SInt32 pos, len;
		if (UInternet::FindField(inFieldTitle, mHeader.c_str(), pos, len)) {
			mHeader.erase((UInt32) pos, (UInt32) len);
		}
	} else  {
		ArrayIndexT		oldFieldLocation;
		if (GetArbitraryField(inFieldTitle, &oldFieldLocation))
			mFieldList.RemoveItem(oldFieldLocation);
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === Private ===

// ---------------------------------------------------------------------------
//		¥ GetDefaultsFromHeader
// ---------------------------------------------------------------------------
// Pulls info out of mHeader and sets the object defaults. Intended to called after
//	mHeader is set.
// Presently searches for:
//	Nothin'

void
LInternetMessage::GetDefaultsFromHeader()
{
	//This is doesn't do anything right now, but it might someday
}

// ---------------------------------------------------------------------------
//		¥ BuildHeader
// ---------------------------------------------------------------------------

void
LInternetMessage::BuildHeader(LDynamicBuffer * outHeader)
{
	//If header is custom we use it "as is"
	if (mCustomHeader) {
		outHeader->ConcatenateBuffer(mHeader.c_str());
		return;
	}

	//Add arbitrary fields
	LHeaderField *	currField;
	LArrayIterator	iter(mFieldList);
	while(iter.Next(&currField)) {
		AddFieldToBuffer(currField, outHeader);
	}

	return;
}

// ---------------------------------------------------------------------------
//		¥ BuildMessageBody
// ---------------------------------------------------------------------------

Boolean
LInternetMessage::BuildMessageBody(LDynamicBuffer * outBuffer)
{
	if (mBody.GetBufferLength()) {
		outBuffer->ConcatenateBuffer(mBody.GetBufferStream());
		return true;
	} else {
		return false;
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === Utility Things ===

void
LInternetMessage::AddFieldToBuffer(LHeaderField * inField, LDynamicBuffer * outBuffer)
{
	PP_STD::string tempBuffer;

	if (!inField)
		return;

	inField->GetField(tempBuffer);
	outBuffer->ConcatenateBuffer(tempBuffer.c_str());
}

void
LInternetMessage::AddFieldToBuffer(const char * inTitle, const char * inBody, LDynamicBuffer * outBuffer)
{
	if ((!inTitle) || (!inBody) || (!*inTitle) || (!*inBody))
		return;

	LHeaderField tempField(inTitle, inBody);
	AddFieldToBuffer(&tempField, outBuffer);
}

void
LInternetMessage::AddFieldToBuffer(const char * inTitle, LArray * inList, LDynamicBuffer * outBuffer)
{
	PP_STD::string commaList;

	if ((!inList) || (!inTitle) || (!*inTitle))
		return;

	UInternet::ListToCommaString(inList, commaList);

	if (commaList.length() > 0) {
		AddFieldToBuffer(inTitle, commaList.c_str(), outBuffer);
	}
}


// ---------------------------------------------------------------------------
//		¥ Clone
// ---------------------------------------------------------------------------
//	Creates a deep copy by duplicating the items in the Internet Message.

void
LInternetMessage::Clone(const LInternetMessage& inOriginal)
{
	ResetMembers();

	LHeaderField*	currField;
	LArrayIterator	iter(inOriginal.mFieldList);
	while(iter.Next(&currField)) {
		SetArbitraryField(currField->GetTitle(), currField->GetBody());
	}

	SetHeader(inOriginal.mHeader.c_str());
	mBody = inOriginal.mBody;
	mCustomHeader = inOriginal.mCustomHeader;
}


PP_End_Namespace_PowerPlant
