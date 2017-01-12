// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternet.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Utility class for dealing with misc. Internet related things.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UInternet.h>
#include <LArray.h>
#include <LArrayIterator.h>
#include <LString.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt32	kTempSwapBuffSize = 16000;


// ---------------------------------------------------------------------------
//	¥ MD5String
// ---------------------------------------------------------------------------
//	Creates a MD5 digest of the C string in 'inString'
//	MD5 code derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
void
UInternet::MD5String(const char *inString, char * outDigest)
{
	MD5_CTX theDigest;
	UInt16 i;
	char *temp = outDigest;

	MD5Init(&theDigest);
	MD5Update (&theDigest, (unsigned char *)inString, (UInt16) PP_CSTD::strlen(inString));
	MD5Final(&theDigest);

	for (i = 0; i < 16; i++) {
		PP_CSTD::sprintf(temp, "%02x", theDigest.digest[i]);
		temp += 2;
	}
	*temp = 0;
}


// ---------------------------------------------------------------------------
//	¥ UUEncode
// ---------------------------------------------------------------------------
// HTUU_encode only handles up to 48 bytes at a time...makes for < 80 char lines

const SInt16	kUUEncodeMaxBytes	= 48;
const UInt32	kMinGrowSize		= 1024;

char *
UInternet::UUEncode(const void * inData, UInt32 * InOutDataSize)
{
	short nbytes;
	UInt32 InDataSize = *InOutDataSize;
	unsigned char * bufP = (unsigned char *)inData;

	UInt32 oneLineLen = (1 + (4 * kUUEncodeMaxBytes)/3) + 1;
	StPointerBlock oneline((SInt32) oneLineLen);

	UInt32 outLen = (1 + (4 * InDataSize)/3) + 1;	//est. size of new data
	char * outData = ::NewPtr((SInt32) outLen);
	ThrowIfMemFail_(outData);

	char * outBufP = outData;

	*InOutDataSize = 0;
	while (InDataSize >= kUUEncodeMaxBytes) {
		nbytes = (short) HTUU_encode (bufP, kUUEncodeMaxBytes, oneline);
		*InOutDataSize += nbytes + 2;	//+2 for CRLF to be added later

		if (*InOutDataSize > outLen) {
			outLen = *InOutDataSize + kMinGrowSize;
			::SetPtrSize(outData, (SInt32) outLen);
			ThrowIfMemError_();
		}

		PP_CSTD::memcpy(outBufP, oneline, nbytes);		//Add it to the output buffer
		outBufP += nbytes;

		PP_CSTD::memcpy(outBufP, CRLF, 2);				//Add the CRLF to this line
		outBufP += 2;

		bufP += kUUEncodeMaxBytes;
		InDataSize -= kUUEncodeMaxBytes;
	}

//	NOTE:	If nbytes is not a multiple of three, then the byte
//			just beyond the last byte in the buffer must be 0 so
//			we copy the data and put one at the end for this round.
	if (InDataSize != 0) {
		unsigned char tempBufP[kUUEncodeMaxBytes + 1];	//max size we could need

		PP_CSTD::memcpy(tempBufP, bufP, InDataSize);
		tempBufP[InDataSize] = '\0';

		nbytes = (short) HTUU_encode (tempBufP, InDataSize, oneline);
		*InOutDataSize += nbytes + 2;	//+2 for CRLF to be added later

		if (*InOutDataSize > outLen) {
			outLen = *InOutDataSize + kMinGrowSize;
			::SetPtrSize(outData, (SInt32) outLen);
			ThrowIfMemError_();
		}

		PP_CSTD::memcpy(outBufP, oneline, nbytes);		//Add it to the output buffer
		outBufP += nbytes;

		PP_CSTD::memcpy(outBufP, CRLF, 2);				//Add the CRLF to this line
		outBufP += 2;
	}

	*outBufP = '\0';

	return outData;
}


// ---------------------------------------------------------------------------
//	¥ UUEncode
// ---------------------------------------------------------------------------
// HTUU_encode only handles up to 48 bytes at a time...makes for < 80 char lines

void
UInternet::UUEncode(const void * inData, UInt32 * InOutDataSize, LDynamicBuffer * outBuffer)
{
	short nbytes;
	UInt32 InDataSize = *InOutDataSize;

	UInt32 oneLineLen = (1 + (4 * kUUEncodeMaxBytes)/3) + 1;
	StPointerBlock oneline((SInt32) oneLineLen);

	unsigned char * bufP = (unsigned char *)inData;

	outBuffer->ResetBuffer();
	*InOutDataSize = 0;
	while (InDataSize >= kUUEncodeMaxBytes) {
		nbytes = (short) HTUU_encode (bufP, kUUEncodeMaxBytes, oneline);

		outBuffer->ConcatenateBuffer(oneline, nbytes);		//Add it to the output buffer
		outBuffer->ConcatenateBuffer(CRLF);				//Add the CRLF to this line
		*InOutDataSize += nbytes;

		bufP += kUUEncodeMaxBytes;
		InDataSize -= kUUEncodeMaxBytes;
	}

//	NOTE:	If nbytes is not a multiple of three, then the byte
//			just beyond the last byte in the buffer must be 0 so
//			we copy the data and put one at the end for this round.
	if (InDataSize != 0) {
		unsigned char tempBufP[kUUEncodeMaxBytes + 1];	//max size we could need

		PP_CSTD::memcpy(tempBufP, bufP, InDataSize);
		tempBufP[InDataSize] = '\0';

		nbytes = (short) HTUU_encode (tempBufP, InDataSize, oneline);

		outBuffer->ConcatenateBuffer(oneline, nbytes);		//Add it to the output buffer
		outBuffer->ConcatenateBuffer(CRLF);				//Add the CRLF to this line
		*InOutDataSize += nbytes;
	}

	outBuffer->ConcatenateBuffer("\0");				//Add null to end of data
}


// ---------------------------------------------------------------------------
//	¥ UUDecode
// ---------------------------------------------------------------------------

void *
UInternet::UUDecode(char * inData, UInt32 * InOutDataSize)
{
	unsigned char * outData = (unsigned char *) ::NewPtr((SInt32) *InOutDataSize);
	ThrowIfMemFail_(outData);
	char * lineTok;

	lineTok = PP_CSTD::strtok(inData, CRLF);
	*InOutDataSize = 0;
	while (lineTok) {
		*InOutDataSize += HTUU_decode (lineTok, (outData + *InOutDataSize), (int) PP_CSTD::strlen(lineTok));
		lineTok = PP_CSTD::strtok(nil, CRLF);
	}

	SetPtrSize((char*)outData, (SInt32) *InOutDataSize);
	return outData;
}


// ---------------------------------------------------------------------------
//	¥ UUDecode
// ---------------------------------------------------------------------------

void
UInternet::UUDecode(char * inData, UInt32 * InOutDataSize, LDynamicBuffer * outBuffer)
{
	unsigned char oneline[kUUEncodeMaxBytes + 1];

	outBuffer->ResetBuffer();

	char * lineTok;
	short lastByteCount;

	lineTok = PP_CSTD::strtok(inData, CRLF);
	*InOutDataSize = 0;
	while (lineTok) {
		lastByteCount = (short) HTUU_decode (lineTok, oneline, (int) PP_CSTD::strlen(lineTok));
		outBuffer->ConcatenateBuffer((const char*)oneline, lastByteCount);
		*InOutDataSize += lastByteCount;

		lineTok = PP_CSTD::strtok(nil, CRLF);
	}
}


// ---------------------------------------------------------------------------
//	¥ CheckSMTPValid
// ---------------------------------------------------------------------------
// Utility function to ensure that data is suitable for SMTP transfer.
// ¥ Returns false if "High-ASCII" is encountered
// ¥ÊReturns false if any single line is > 1000 characters

Boolean
UInternet::CheckSMTPValid (LDynamicBuffer* theBuffer)
{
	LStream * inStream = theBuffer->GetBufferStream();
	inStream->SetMarker(0, streamFrom_Start);

	char currChar, nextChar;
	while (inStream->ReadData(&currChar, 1)) {
		Boolean havePeriod;
		havePeriod = (currChar == '.');

		//Check for line length and High Ascii
		//	Walk up to the next end of line checking each character
		//	for high-ascii and line lengths > 1000
		for (UInt32 i = 1; !((currChar == CR) || (currChar == LF)); i++) {
			if (havePeriod && (i > kSMTPMaxLineLen - 1))
				return false;
			else if (i > kSMTPMaxLineLen)
				return false;

			if ((unsigned char)currChar >= 128)
				return false;

			if (!inStream->ReadData(&currChar, 1))
				return true;
		}

		//Peek at the next char to see if its a LF
		if (!inStream->PeekData(&nextChar, 1))
			return true;

		//Walk up to begining of next line if necessary
		//	we know currChar is either a CR *or* LF so check to see if we have a CRLF
		if (currChar == CR && nextChar == LF)
			inStream->ReadData(&nextChar, 1);	//read the LF to step over it
	}

	return true;
}


// ---------------------------------------------------------------------------
//	¥ SMTPEncode
// ---------------------------------------------------------------------------
// Utility function to ensure that data is suitable for SMTP transfer.
// 		- Encode periods
//		- Append CRLF to end of data
// 		- Convert CR's alone to CRLF
// 		- Convert LF's alone to CRLF
// 		- Concatenates the SMTP "CRLF.CRLF" termination sequence onto the data
// 		- Returns error if "High-ASCII" is encountered
// 		- Returns error if any single line is > 1000 characters

Boolean
UInternet::SMTPEncode (LDynamicBuffer* theBuffer)
{
	//Terminate the buffer with CRLF just in case...
	//	this greatly simplifies things later
	theBuffer->ConcatenateBuffer(CRLF);

	LStream * origStream = theBuffer->GetBufferStream();
	origStream->SetMarker(0, streamFrom_Start);

	LDynamicBuffer newBuffer(theBuffer->GetBufferLength());

	char currChar, nextChar;
	while (origStream->ReadData(&currChar, 1)) {
		if (currChar == '.')
			newBuffer.ConcatenateBuffer(&currChar, 1);

		UInt32 i;
		char currLine[kSMTPMaxLineLen];
		for (i = 0; !((currChar == CR) || (currChar == LF)); i++) {
			if (i > kSMTPMaxLineLen - 2)
				return false;	//Line length > 1000 (after CRLF added)

			if ((unsigned char)currChar >= 128)
				return false;	//High ASCII

			currLine[i] = currChar;
			origStream->ReadData(&currChar, 1);
		}

		//Walk up to begining of next line if necessary
		//	we know currChar is either a CR *or* LF so check to see if we have a CRLF
		//Peek at the next char to see if its a LF
		if (origStream->PeekData(&nextChar, 1)) {
			if (currChar == CR && nextChar == LF)
				origStream->ReadData(&nextChar, 1);	//read the LF to step over it
		}

		//Write CRLF to terminate line
		currLine[i++] = CR;
		currLine[i++] = LF;
		newBuffer.ConcatenateBuffer(currLine, (SInt32) i);
	}

	//Write the SMTP termination sequence
	newBuffer.ConcatenateBuffer(kMailTermSequence, kMailTermSequenceLength);

	//copy new stream to old buffer
	theBuffer->SetStream(newBuffer.DetachStream());

	return true;
}


// ---------------------------------------------------------------------------
//	¥ SMTPDecode
// ---------------------------------------------------------------------------
//	Utility function to remove period encoding and the CRLF.CRLF termination
//	from a buffer.

void
UInternet::SMTPDecode(LDynamicBuffer* theBuffer)
{
	LStream * origStream = theBuffer->GetBufferStream();
	origStream->SetMarker(0, streamFrom_Start);

	LDynamicBuffer newBuffer(theBuffer->GetBufferLength());

	char currChar, nextChar;
	while (origStream->ReadData(&currChar, 1)) {
		//check for period encoding at head of line
		if (currChar == '.') {
			//look to see if next char is a period
			if (origStream->PeekData(&nextChar, 1)) {
				if (nextChar == '.')
					origStream->ReadData(&currChar, 1);	//remove the extra period
				else if (nextChar == CR)
					break;				//if we have period on line alone we are done
			}
		}

		//scan to end of line
		UInt32 i;
		char currLine[kSMTPMaxLineLen];
		for (i = 0; currChar != LF; i++) {
			ThrowIf_(i > kSMTPMaxLineLen);
			currLine[i] = currChar;
			ThrowIfNot_(origStream->ReadData(&currChar, 1) == 1);
		}
		//append LF
		currLine[i] = currChar;
		//write line
		newBuffer.ConcatenateBuffer(currLine, (SInt32) (i + 1));
	}

	//set new stream to old buffer
	theBuffer->SetStream(newBuffer.DetachStream());
}


// ---------------------------------------------------------------------------
//	¥ RemoveLF
// ---------------------------------------------------------------------------
//	Utility function to remove LF's from a LDynamicBuffer.

void
UInternet::RemoveLF(LDynamicBuffer* theBuffer)
{
	SInt32 tempPSize = kTempSwapBuffSize;
	StPointerBlock tempIn(tempPSize);
	StPointerBlock tempOut(tempPSize);
	Ptr tempInP = tempIn;
	Ptr tempOutP = tempOut;

	LStream * origStream = theBuffer->GetBufferStream();
	origStream->SetMarker(0, streamFrom_Start);

	LDynamicBuffer newBuffer;

	while (1) {
		SInt32 i, j, lastAmountRead;
		lastAmountRead = origStream->ReadData(tempIn, tempPSize);

		j = 0;
		for (i = 0; i < lastAmountRead; i++) {
			if (tempInP[i] != LF) {
				tempOutP[j++] = tempInP[i];
			}
		}

		newBuffer.ConcatenateBuffer(tempOut, j);

		if (lastAmountRead != tempPSize)
			break;
	}

	//copy new stream to old buffer
	theBuffer->SetStream(newBuffer.DetachStream());
}


// ---------------------------------------------------------------------------
//	¥ GetContentLength
// ---------------------------------------------------------------------------
//	Utility function to scan a buffer for the content-length field...
//	probably most useful for use with HTTP headers.

SInt32
UInternet::GetContentLength(LDynamicBuffer* dataBuffer)
{
	SInt32 theLength = 0;

	//Check only when we have a distinguishable header
	SInt32 headerLen = FindBodyStarts(dataBuffer);
	if (!headerLen)
		return theLength;

	StHandleBlock textH(Size_Zero);
	if (dataBuffer->BufferToHandle(textH, headerLen)) {
		StHandleLocker locked(textH);
		LHeaderField theField;
		if (UInternet::FindField(kFieldContentLength, *textH, &theField))
			theLength = PP_CSTD::atol(theField.GetBody());
	}
	return theLength;
}


// ---------------------------------------------------------------------------
//	¥ FindBodyStarts
// ---------------------------------------------------------------------------
//	Finds the offset the begining of the message body, returns ptr to location.
//	Returns 0 if body could not be located.

char *
UInternet::FindBodyStarts(const char * inMessage, UInt32 inLength)
{
	//Special case for MIME messages:
	//	It's legal for MIME segments to have no header and
	//	instead start with a CRLF (some might start with LF only)
	if (inMessage[0] == CR && inMessage[1] == LF)
		return (char*)inMessage + 2;
	if (inMessage[0] == LF)
		return (char*)inMessage + 1;

	const char * p = PP_CSTD::strstr(inMessage, kHeaderSep);
	if (!p)
		p = PP_CSTD::strstr(inMessage, kAltHeaderSep);

	if (!inLength)
		inLength = (UInt32) (p - inMessage);

	if (p && (p <= (inMessage + inLength))) {
		while ((*p == CR) || (*p == LF))
			p++;	//Walk up to start of body
	} else {
		p = (char*)inMessage;	//can't find sep, return the whole message
	}

	return const_cast<char *>(p);
}


// ---------------------------------------------------------------------------
//	¥ FindBodyStarts
// ---------------------------------------------------------------------------
//	Finds the offset to begining of the message body, returns position.
//	Returns 0 if body could not be located.

SInt32
UInternet::FindBodyStarts(LDynamicBuffer* theBuffer)
{
	LStream * inStream = theBuffer->GetBufferStream();
	return FindBodyStarts(inStream);
}


// ---------------------------------------------------------------------------
//	¥ FindBodyStarts
// ---------------------------------------------------------------------------
//	Finds the offset to begining of the message body, returns position.
//	Returns 0 if body could not be located.

SInt32
UInternet::FindBodyStarts(LStream* inStream, SInt32 inLength)
{
	if (inLength < 0) {
		inStream->SetMarker(0, streamFrom_Start);
		inLength = inStream->GetLength();
	}

	SInt32 startPos = inStream->GetMarker();

	//Special case for MIME messages:
	//	It's legal for MIME segments to have no header and
	//	instead start with a CRLF (some might start with LF only)
	char CRLFTest[2];
	inStream->ReadData(CRLFTest, 2);
	if (CRLFTest[0] == CR && CRLFTest[1] == LF)
		return inStream->GetMarker();
	if (CRLFTest[0] == LF)
		return inStream->GetMarker() - 1;
	//return marker if we didn't find CRLF at start
	inStream->SetMarker(startPos, streamFrom_Start);

	char currChar;
	char lineEnd[3];
	while (inStream->ReadData(&currChar, 1)) {
		if ((inStream->GetMarker() - startPos) > inLength)
			return 0;

		// Check for CRLF+CRLF
		if (currChar == CR) {
			SInt32 amountRead = inStream->ReadData(lineEnd, 3);
			if (amountRead < 3)
				return 0;

			if (lineEnd[0] == LF
				&& lineEnd[1] == CR
				&& lineEnd[2] == LF)
				return inStream->GetMarker();

		// Check for CRLF+LF.
			if (lineEnd[0] == LF
				&& lineEnd[1] == LF)
				return ( inStream->GetMarker() - 1 );

		// Check for LF+LF
		} else if (currChar == LF) {
			if (!inStream->ReadData(&currChar, 1))
				return 0;

			if (currChar == LF)
				return inStream->GetMarker();
		}

	}

	return 0;
}


// ---------------------------------------------------------------------------
//	¥ FindField
// ---------------------------------------------------------------------------
// Utility routine to scan for a specified field within a string

Boolean
UInternet::FindField(const char * inFieldType, const char * inHeader, LHeaderField * outField)
{
	char tempFieldType[64];
	//cat a colon & space onto the field title
	PP_CSTD::size_t tempFieldTypeLen = (PP_CSTD::size_t) PP_CSTD::sprintf(tempFieldType, "%s: ", inFieldType);

	::LowercaseText(tempFieldType, (SInt16) tempFieldTypeLen, nil);

	const char * currLineHead = inHeader;
	LStr255 currLine;
	while (currLineHead[0] != '\0') {
		currLine = currLineHead;

		::LowercaseText((char*)&currLine[1], currLine[0], nil);

		if (currLine.BeginsWith(tempFieldType, (UInt8)tempFieldTypeLen)) {
			outField->SetField(currLineHead);
			return true;
		}

		//Walk up to the head of the next line
		Boolean atHead = false;
		for (; currLineHead[0] != '\0' ; currLineHead++) {
			if (currLineHead[0] == CR || currLineHead[0] == LF) {
				atHead = true;
				continue;
			}

			//	could be a wrapped header field so check
			if (atHead) {
				if (PP_CSTD::isspace(currLineHead[0])) {
					atHead = false;
				} else {
					break;
				}
			}
		}
	}
	return false;
}


// ---------------------------------------------------------------------------
//	¥ FindField
// ---------------------------------------------------------------------------
// Utility routine to scan for a specified field within a string
//	This version returns offset and length of the field if found

Boolean
UInternet::FindField(const char * inFieldType, const char * inHeader, SInt32& outPos, SInt32& outLen)
{
	outPos = outLen = 0;

	char tempFieldType[64];
	//cat a colon & space onto the field title
	PP_CSTD::size_t tempFieldTypeLen = (PP_CSTD::size_t) PP_CSTD::sprintf(tempFieldType, "%s: ", inFieldType);

	::LowercaseText(tempFieldType, (SInt16) tempFieldTypeLen, nil);

	const char * currLineHead = inHeader;
	LStr255 currLine;
	while (currLineHead[0] != '\0') {
		currLine = (const char*)currLineHead;

		::LowercaseText((char*)&currLine[1], currLine[0], nil);

		if (currLine.BeginsWith(tempFieldType, (UInt8)tempFieldTypeLen)) {
			outPos = currLineHead - inHeader;
			LHeaderField	theField(currLineHead);
			outLen = (SInt32) theField.FieldLength();
			return true;
		}

		//Walk up to the head of the next line
		Boolean atHead = false;
		for (; currLineHead[0] != '\0' ; currLineHead++) {
			if (currLineHead[0] == CR || currLineHead[0] == LF) {
				atHead = true;
				continue;
			}

			//	could be a wrapped header field so check
			if (atHead) {
				if (PP_CSTD::isspace(currLineHead[0])) {
					atHead = false;
				} else {
					break;
				}
			}
		}

	}
	return false;
}


// ---------------------------------------------------------------------------
//	¥ GetCleanAddress
// ---------------------------------------------------------------------------
// Utility routine to return clean address from address string
//	This routine works with addresses in the following formats:
//		"User Name" <user@host.suf>
//		(user@host.suf) "User Name"

void
UInternet::GetCleanAddress(LStr255& InOutAddress)
{
	UInt8 startPos, endPos;
	UInt8 length = InOutAddress.Length();

	if (!length)
		return;
	else if ((startPos = InOutAddress.Find('(')) != 0) {
		InOutAddress.Remove(startPos, (UInt8) (length - startPos + 1));
	}
	else if ((startPos = InOutAddress.Find('<')) != 0) {
		if ((endPos = InOutAddress.Find('>')) != 0) {
			InOutAddress.Remove(1, startPos);

			length = InOutAddress.Length();
			endPos = InOutAddress.Find('>');
			InOutAddress.Remove(endPos, (UInt8) (length - endPos + 1));
		}
	}

	//Remove any leading or trailing spaces
	while ((startPos = InOutAddress.Find(' ')) != 0) {
		InOutAddress.Remove(startPos, 1);
	}
}


// ---------------------------------------------------------------------------
//	¥ PointToNextLine
// ---------------------------------------------------------------------------
// Utility routine to find the head of the next line...
//	This routine assumes we are dealing with CRLF or CR or LF

char *
UInternet::PointToNextLine(const char * inPos)
{
	const char * tempPos = inPos;
	while (*tempPos && !((*tempPos == CR) || (*tempPos == LF)))
		tempPos++;

	if (!*tempPos) {
		return (char*)tempPos;
	} else if (*tempPos == LF) {
		tempPos++;
	} else if (*tempPos == CR && *(tempPos + 1) != LF) {
		tempPos++;
	} else {
		tempPos += 2;
	}

	return (char*)tempPos;
}


// ---------------------------------------------------------------------------
//	¥ PointToNextLine
// ---------------------------------------------------------------------------
//	Utility routine to find the head of the next line...
//	This routine assumes we are dealing with CRLF or CR or LF
//	On return stream marker will be at head of next line or end of buffer

void
UInternet::PointToNextLine(LStream * inStream)
{
	char currChar = '\0';
	while (!((currChar == CR) || (currChar == LF)))
		if (!inStream->ReadData(&currChar, 1))
			return;

	if (currChar == CR) {
		char nextChar;
		inStream->PeekData(&nextChar, 1);
		if (nextChar == LF)
			inStream->SetMarker(1, streamFrom_Marker);
	}
}


// ---------------------------------------------------------------------------
//	¥ CommaStringToList
// ---------------------------------------------------------------------------
//	Utility function to take a string of comma seperated values (like email
//	addresses) and convert them to an LArray.

void
UInternet::CommaStringToList(const char * inString, LArray * outList)
{
	StPointerBlock tempString((SInt32) (PP_CSTD::strlen(inString) + 1));
	PP_CSTD::strcpy(tempString, inString);
	char * p;

	p = PP_CSTD::strtok(tempString, ",");
	while (p) {
		char tempString[255];
		PP_CSTD::strncpy(tempString, p, 255);
		tempString[255-1] = '\0';
		outList->InsertItemsAt(1, LArray::index_Last, tempString);
		p = PP_CSTD::strtok(nil, ",");
	}
}


// ---------------------------------------------------------------------------
//	¥ ListToCommaString
// ---------------------------------------------------------------------------
// Utility function to take a LArray and convert them to an string of
// comma seperated values (like email addresses)

void
UInternet::ListToCommaString(LArray * inList, PP_STD::string& outString)
{
	outString = "";
	char currItem[513];
	Boolean haveList = false;

	LArrayIterator iter(*inList);
	while(iter.Next(currItem))
	{
		if (haveList)
			outString += ", ";
		else
			haveList = true;

		outString += currItem;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetGMTOffset
// ---------------------------------------------------------------------------
//	Utility function to determin the GMT offset of the local machine
//	assumes of course that the users location field is properly set.

SInt32
UInternet::GetGMTOffset()
{
	SInt32			internalGmtDelta;
	MachineLocation theLoc;

	ReadLocation(&theLoc);

	internalGmtDelta = ::BitAnd(theLoc.u.gmtDelta, 0x00FFFFFF);
	if ( ::BitTst(&internalGmtDelta, 9) ) {
		internalGmtDelta = ::BitOr(internalGmtDelta, 0xFF000000);
	}

	return internalGmtDelta;
}


PP_End_Namespace_PowerPlant
