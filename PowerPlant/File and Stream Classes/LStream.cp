// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStream.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for reading/writing an ordered sequence of bytes

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LStream.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant

const	SInt32	length_NilBlock = -1;

#pragma mark --- Construction & Destruction ---

// ---------------------------------------------------------------------------
//	¥ LStream								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LStream::LStream()
{
	mMarker = 0;
	mLength = 0;
}


// ---------------------------------------------------------------------------
//	¥ LStream								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LStream::LStream(
	const LStream&	inOriginal)
{
	mMarker = inOriginal.mMarker;
	mLength = inOriginal.mLength;
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LStream&
LStream::operator = (
	const LStream&	inOriginal)
{
	if (this != &inOriginal) {
		mMarker = inOriginal.mMarker;
		mLength = inOriginal.mLength;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LStream								Destructor				  [public]
// ---------------------------------------------------------------------------

LStream::~LStream()
{
}

#pragma mark --- Accessors ---

// ---------------------------------------------------------------------------
//	¥ SetMarker
// ---------------------------------------------------------------------------
//	Place the Read/Write Marker at an offset from a specified position
//
//	inFromWhere can be streamFrom_Start, streamFrom_End, or streamFrom_Marker

void
LStream::SetMarker(
	SInt32		inOffset,
	EStreamFrom	inFromWhere)
{
	SInt32	newMarker = mMarker;

	switch (inFromWhere) {

		case streamFrom_Start:
			newMarker = inOffset;
			break;

		case streamFrom_End:
			newMarker = GetLength() - inOffset;
			break;

		case streamFrom_Marker:
			newMarker += inOffset;
			break;
	}

	if (newMarker < 0) {				// marker must be between 0 and
		newMarker = 0;					// Length, inclusive
	} else if (newMarker > GetLength()) {
		newMarker = GetLength();
	}

	mMarker = newMarker;
}


// ---------------------------------------------------------------------------
//	¥ GetMarker
// ---------------------------------------------------------------------------
//	Return the Read/Write Marker position
//
//	Position is a byte offset from the start of the Stream

SInt32
LStream::GetMarker() const
{
	return mMarker;
}


// ---------------------------------------------------------------------------
//	¥ SetLength
// ---------------------------------------------------------------------------
//	Set the length, in bytes, of the Stream

void
LStream::SetLength(
	SInt32	inLength)
{
	mLength = inLength;
	
										// If marker is now past the end of
										//   the stream, set the marker to
										//   be at the end
	if (GetMarker() > mLength) {
		SetMarker(mLength, streamFrom_Start);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetLength
// ---------------------------------------------------------------------------
//	Return the length, in bytes, of the Stream

SInt32
LStream::GetLength() const
{
	return mLength;
}

#pragma mark --- Low-Level I/O ---

// ---------------------------------------------------------------------------
//	¥ PutBytes
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to a Stream
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.
//
//	Subclasses must override this function to support writing.
//
//	NOTE: You should not throw an Exception out of this function.

ExceptionCode
LStream::PutBytes(
	const void*	/* inBuffer */,
	SInt32		&ioByteCount)
{
	SignalStringLiteral_("PutBytes() not overridden");
	ioByteCount = 0;
	return writErr;
}


// ---------------------------------------------------------------------------
//	¥ WriteBlock
// ---------------------------------------------------------------------------
//	Write data, specified by a pointer and byte count, to a Stream

void
LStream::WriteBlock(
	const void	*inBuffer,
	SInt32		inByteCount)
{
	ThrowIfError_( PutBytes(inBuffer, inByteCount) );
}


// ---------------------------------------------------------------------------
//	¥ GetBytes
// ---------------------------------------------------------------------------
//	Read bytes from a Stream to a buffer
//
//	Returns an error code and passes back the number of bytes actually
//	read, which may be less than the number requested if an error occurred.
//
//	Subclasses must override this function to support reading.
//
//	NOTE: You should not throw an Exception out of this function.

ExceptionCode
LStream::GetBytes(
	void*	/* outBuffer */,
	SInt32	&ioByteCount)
{
	SignalStringLiteral_("GetBytes() not overridden");
	ioByteCount = 0;
	return readErr;
}


// ---------------------------------------------------------------------------
//	¥ ReadBlock
// ---------------------------------------------------------------------------
//	Read data from a Stream to a buffer

void
LStream::ReadBlock(
	void	*outBuffer,
	SInt32	inByteCount)
{
	ThrowIfError_( GetBytes(outBuffer, inByteCount) );
}


// ---------------------------------------------------------------------------
//	¥ PeekData
// ---------------------------------------------------------------------------
//	Read data from a Stream to a buffer, without moving the Marker
//
//	Return the number of bytes actually read, which may be less than the
//	number requested if an error occurred

SInt32
LStream::PeekData(
	void	*outBuffer,
	SInt32	inByteCount)
{
	SInt32	currentMarker = GetMarker();

	SInt32	bytesToPeek = inByteCount;
	GetBytes(outBuffer, bytesToPeek);

	SetMarker(currentMarker, streamFrom_Start);

	return bytesToPeek;
}

#pragma mark --- High-Level I/O ---

// ---------------------------------------------------------------------------
//	¥ WritePtr
// ---------------------------------------------------------------------------
//	Write a Toolbox Ptr block to a Stream
//
//	A Ptr block is written as a 4-byte count (size of the Ptr),
//	followed by the contents of the Ptr block
//
//	Returns the number of bytes written

SInt32
LStream::WritePtr(
	Ptr		inPtr)
{
	SInt32	bytesWritten = 0;

	if (inPtr != nil) {
		SInt32	ptrLength = ::GetPtrSize(inPtr);

		*this << ptrLength;

		WriteBlock(inPtr, ptrLength);
		bytesWritten = ptrLength;

	} else {
		*this << length_NilBlock;
	}

	return (bytesWritten + (SInt32) sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ ReadPtr
// ---------------------------------------------------------------------------
//	Read data from a Stream into a newly created Ptr block
//
//	A Ptr block is stored in a Stream as a 4-byte count (size of the Ptr),
//	followed by the contents of the Ptr block
//
//	Returns the number of bytes read

SInt32
LStream::ReadPtr(
	Ptr		&outPtr)
{
	SInt32	bytesRead = 0;
	outPtr = nil;

	SInt32	ptrLength;
	*this >> ptrLength;

	if (ptrLength != length_NilBlock) {
		StPointerBlock	thePointer(ptrLength);

		ReadBlock(thePointer, ptrLength);
		bytesRead = ptrLength;

		outPtr = thePointer.Release();
	}

	return (bytesRead + (SInt32) sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ WriteHandle
// ---------------------------------------------------------------------------
//	Write a Toolbox Handle block to a Stream
//
//	A Handle block is written as a 4-byte count (size of the Handle),
//	followed by the contents of the Handle block
//
//	Returns the number of bytes written

SInt32
LStream::WriteHandle(
	Handle	inHandle)
{
	SInt32	bytesWritten = 0;

	if (inHandle != nil) {
		SInt32	handleLength = ::GetHandleSize(inHandle);

		*this << handleLength;

		StHandleLocker	lockMe(inHandle);
		WriteBlock(*inHandle, handleLength);
		bytesWritten = handleLength;

	} else {
		*this << length_NilBlock;
	}

	return (bytesWritten + (SInt32) sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ ReadHandle
// ---------------------------------------------------------------------------
//	Read data from a Stream into a newly created Handle block
//
//	A Handle block is stored in a Stream as a 4-byte count (size of the
//	Handle), followed by the contents of the Handle block
//
//	Returns the number of bytes read

SInt32
LStream::ReadHandle(
	Handle	&outHandle)
{
	SInt32	bytesRead = 0;
	outHandle = nil;

	SInt32	handleLength;
	*this >> handleLength;

	if (handleLength != length_NilBlock) {
		StHandleBlock	theHandle(handleLength);
		StHandleLocker	lockMe(theHandle);

		ReadBlock(*theHandle.Get(), handleLength);
		bytesRead = handleLength;

		outHandle = theHandle.Release();
	}

	return (bytesRead + (SInt32) sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ WritePString
// ---------------------------------------------------------------------------
//	Write a Pascal string to a Stream
//
//	Returns the number of bytes written

SInt32
LStream::WritePString(
	ConstStringPtr	inString)
{
	SInt32	bytesToWrite = inString[0] + 1;

	WriteBlock(inString, inString[0] + 1);

	return bytesToWrite;
}


// ---------------------------------------------------------------------------
//	¥ ReadPString
// ---------------------------------------------------------------------------
//	Read a Pascal string from a Stream
//
//	Returns the number of bytes read

SInt32
LStream::ReadPString(
	Str255	outString)
{
	*this >> outString[0];			// Read length byte

	ReadBlock(outString + 1, outString[0]);

	return (outString[0] + 1);
}


// ---------------------------------------------------------------------------
//	¥ WriteCString
// ---------------------------------------------------------------------------
//	Write a C string to a Stream
//
//	Returns the number of bytes written.

SInt32
LStream::WriteCString(
	const char *inString)
{
	SInt32		strLen = 0;				// Find length of C string
	const char	*s = inString;
	while (*s++ != 0) {
		strLen++;
	}

		// Write C string as a 4-byte count followed by the characters.
		// Do not write the null terminator.

	*this << strLen;
	WriteBlock(inString, strLen);

	return (strLen + (SInt32) sizeof(strLen));
}


// ---------------------------------------------------------------------------
//	¥ ReadCString
// ---------------------------------------------------------------------------
//	Read a C string from a Stream
//
//	Returns the number of bytes read

SInt32
LStream::ReadCString(
	char	*outString)
{
		// C string is stored as a 4-byte count followed by the
		// characters. The null terminator is not stored and must
		// be added afterwards.

	SInt32	strLen;
	*this >> strLen;

	ReadBlock(outString, strLen);
	outString[strLen] = '\0';			// Null terminator

	return (strLen + (SInt32) sizeof(SInt32));
}


// ---------------------------------------------------------------------------
//	¥ operator << (double)
// ---------------------------------------------------------------------------
//	Write a double.  No matter what the native size of a double is, we always
//	write out a 64-bit IEEE double.

LStream&
LStream::operator << (double inNum)
{
#if TARGET_CPU_PPC || TARGET_CPU_X86
	// PowerPC and Intel doubles -- they're 8 bytes already, so just swap
	// if necessary and write.
	 
	Assert_(sizeof(inNum) == 8);
	CFSwappedFloat64 swappedDouble = CFConvertDoubleHostToSwapped(inNum);
	WriteBlock(&swappedDouble, sizeof(swappedDouble));

#elif TARGET_CPU_68K

	#if __IEEEdoubles__
		// 68K IEEE doubles -- they're 8 bytes already, so just write.

		Assert_(sizeof(inNum) == 8);
		WriteBlock(&inNum, sizeof(inNum));

	#else
		// 68K 10-byte and 12-byte doubles -- convert to short double (which
		// is always 8 bytes) before writing.

		short double	sd	= (short double) inNum;

		Assert_(sizeof(sd) == 8);
		WriteBlock(&sd, sizeof(sd));
	#endif

#else	// For other platforms, we just write out the number. This means
		// the data format is platform-dependent.

	WriteBlock(&inNum, sizeof(inNum));
#endif

	return (*this);
}


// ---------------------------------------------------------------------------
//	¥ operator >> (double&)
// ---------------------------------------------------------------------------
//	Read a double.  No matter what the native size of a double is, we always
//	read in a 64-bit IEEE double.

LStream&
LStream::operator >> (double& outNum)
{
#if TARGET_CPU_PPC || TARGET_CPU_X86
	// PowerPC and Intel doubles -- they're 8 bytes already, so just read
	// and swap if necessary.
	 
	Assert_(sizeof(outNum) == 8); 
	CFSwappedFloat64 swappedDouble;
	ReadBlock(&swappedDouble, sizeof(swappedDouble));
	outNum = CFConvertDoubleSwappedToHost(swappedDouble);

#elif TARGET_CPU_68K

	#if __IEEEdoubles__
		// 68K IEEE doubles -- they're 8 bytes already, so just read.

		Assert_(sizeof(outNum) == 8);
		ReadBlock(&outNum, sizeof(outNum));

	#else
		// 68K 10-byte and 12-byte doubles -- convert to short double (which
		// is always 8 bytes) before reading.

		short double	sd;

		Assert_(sizeof(sd) == 8);
		ReadBlock(&sd, sizeof(sd));

		outNum = sd;
	#endif

#else		// For other platforms, we just read the number. If the data
			// was written from another platform with a different size
			// double, this won't work right.

		ReadBlock(&outNum, sizeof(outNum));
#endif

	return (*this);
}

PP_End_Namespace_PowerPlant
