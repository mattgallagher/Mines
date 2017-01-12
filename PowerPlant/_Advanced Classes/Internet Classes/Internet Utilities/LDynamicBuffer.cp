// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDynamicBuffer.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Wrapper class for dealing with a resizeable Buffer as either an
//	LTemporaryFileStream, or an LHandleStream.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDynamicBuffer.h>
#include <cctype>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LDynamicBuffer
// ---------------------------------------------------------------------------
//	Constructor

LDynamicBuffer::LDynamicBuffer(SInt32 inExpectedSize)
{
	SetMemoryThreshold();
	InitMembers();
	if (inExpectedSize)
		CheckStreamSize(inExpectedSize);
}

// ---------------------------------------------------------------------------
//		¥ LDynamicBuffer
// ---------------------------------------------------------------------------
//	Alternate Constructor
// This version accepts a C string

LDynamicBuffer::LDynamicBuffer(const char * inBuffer)
{
	SetMemoryThreshold();
	InitMembers();
	ConcatenateBuffer(inBuffer);
}

// ---------------------------------------------------------------------------
//		¥ LDynamicBuffer
// ---------------------------------------------------------------------------
//	Alternate Constructor

LDynamicBuffer::LDynamicBuffer(const char * inBuffer, SInt32 inByteCount)
{
	SetMemoryThreshold();
	InitMembers();
	ConcatenateBuffer(inBuffer, inByteCount);
}

// ---------------------------------------------------------------------------
//		¥ LDynamicBuffer
// ---------------------------------------------------------------------------
//	Alternate Constructor

LDynamicBuffer::LDynamicBuffer(LStream * inStream, SInt32 inByteCount)
{
	SetMemoryThreshold();
	InitMembers();
	ConcatenateBuffer(inStream, inByteCount);
}

// ---------------------------------------------------------------------------
//		¥ LDynamicBuffer
// ---------------------------------------------------------------------------
//	Copy Constructor
//		NOTE: This copies all the data as well.

LDynamicBuffer::LDynamicBuffer(const LDynamicBuffer& copyBuffer)
{
	mMemoryThreshold = copyBuffer.mMemoryThreshold;
	InitMembers();
	if (copyBuffer.mStream)
		ConcatenateBuffer(copyBuffer.mStream);
}

// ---------------------------------------------------------------------------
//		¥ ~LDynamicBuffer
// ---------------------------------------------------------------------------
//	Destructor

LDynamicBuffer::~LDynamicBuffer()
{
	ResetBuffer();
}

// ---------------------------------------------------------------------------
//		¥ InitMembers
// ---------------------------------------------------------------------------
//	Initializes member variables

void
LDynamicBuffer::InitMembers()
{
	mStream = nil;
	mHandleStream = nil;
	mTempFileStream = nil;
	mOwnsStream = true;
}

// ---------------------------------------------------------------------------
//		¥ ResetBuffer
// ---------------------------------------------------------------------------
//	Resets member variables for reuse of the object

void
LDynamicBuffer::ResetBuffer() {
	if (mOwnsStream)
		delete mStream;
	InitMembers();
}

// ---------------------------------------------------------------------------
//		¥ SetBuffer
// ---------------------------------------------------------------------------
//	Clears the data buffer and resets it to a new value

void
LDynamicBuffer::SetBuffer(const char * inBuffer, SInt32 inByteCount)
{
	ResetBuffer();
	ConcatenateBuffer(inBuffer, inByteCount);
}

// ---------------------------------------------------------------------------
//		¥ SetBuffer
// ---------------------------------------------------------------------------
//	Clears the data buffer and resets it to a new value

void
LDynamicBuffer::SetBuffer(LStream * inStream, SInt32 inByteCount)
{
	ResetBuffer();
	ConcatenateBuffer(inStream, inByteCount);
}

// ---------------------------------------------------------------------------
//		¥ SetBuffer
// ---------------------------------------------------------------------------
//	Clears the data buffer and resets it to a new value

void
LDynamicBuffer::SetBuffer(LDynamicBuffer * inBuffer)
{
	ResetBuffer();
	ConcatenateBuffer(inBuffer);
}

// ---------------------------------------------------------------------------
//		¥ SetStream
// ---------------------------------------------------------------------------
//	Sets the stream for this buffer... The buffer may or may not actually own
//		the stream set here.
//
//	NOTE: The stream set here does not need to be either a LHandleStream or
//			LTemporaryFileStream, but if it is, the appropriate member variable
//			will be set since we have done some optimizations for these specific
//			stream types.

void
LDynamicBuffer::SetStream(LStream * inStream, Boolean inOwnStream)
{
	ResetBuffer();
	mStream = inStream;
	mOwnsStream = inOwnStream;

		// Set members for specific stream types. If the stream isn't
		// the matching type, dynamic_cast returns nil.

	mHandleStream = dynamic_cast<LHandleStream*>(mStream);
	mTempFileStream = dynamic_cast<LTemporaryFileStream*>(mStream);
}

// ---------------------------------------------------------------------------
//		¥ GetBufferStream
// ---------------------------------------------------------------------------

LStream*
LDynamicBuffer::GetBufferStream()
{
	if (!mStream)
		CheckStreamSize(kLDynBuf_MemoryThreshold);

	return mStream;
}

// ---------------------------------------------------------------------------
//		¥ ConcatenateBuffer
// ---------------------------------------------------------------------------
//	Concatenates data to the end of the data buffer

void
LDynamicBuffer::ConcatenateBuffer(const char * inBuffer, SInt32 inByteCount)
{
	if (!inBuffer)
		return;

	CheckStreamSize(inByteCount);

	//make sure we're at the end of the buffer
	mStream->SetMarker(0, streamFrom_End);
	ThrowIf_(mStream->WriteData(inBuffer, inByteCount) != inByteCount);
}

// ---------------------------------------------------------------------------
//		¥ ConcatenateBuffer
// ---------------------------------------------------------------------------
//	Concatenates a LDynamicBuffer to the end of this buffer

void
LDynamicBuffer::ConcatenateBuffer(LDynamicBuffer * inBuffer)
{
	if (!inBuffer)
		return;

	ConcatenateBuffer(inBuffer->GetBufferStream());
}

// ---------------------------------------------------------------------------
//		¥ ConcatenateBuffer
// ---------------------------------------------------------------------------
//	Concatenates a stream to the end of the data buffer

void
LDynamicBuffer::ConcatenateBuffer(LStream * inStream, SInt32 inByteCount)
{
	if (!inStream)
		return;

	// If no inByteCount specified, assume the whole stream and reset the marker
	if (inByteCount < 0) {
		inStream->SetMarker(0, streamFrom_Start);
		inByteCount = inStream->GetLength();
	}

	//Check size first so we can optimize handle copies
	CheckStreamSize(inByteCount);
	//make sure we're at the end of the buffer
	mStream->SetMarker(0, streamFrom_End);

	//Optimization for copies to LHandleStream
	if (mHandleStream) {
		mHandleStream->SetLength(inByteCount + mHandleStream->GetLength());
		Handle ourHandle = mHandleStream->GetDataHandle();
		StHandleLocker locked(ourHandle);
		Ptr tempP = *ourHandle + mHandleStream->GetMarker();
		inStream->ReadData(tempP, inByteCount);
	} else {
		//Try to allocate some swap space to speed things up
		//	Get the smaller of available memory or the inByteCount value
		SInt32 bufferSize = ::MaxBlock();
		bufferSize = (bufferSize < inByteCount) ? bufferSize : inByteCount;
		StPointerBlock tempSwap(bufferSize);

		while (inByteCount > 0) {
			bufferSize = inStream->ReadData(tempSwap, bufferSize);
			mStream->WriteData(tempSwap, bufferSize);
			inByteCount -= bufferSize;
		}
	}
}

// ---------------------------------------------------------------------------
//		¥ PrependBuffer
// ---------------------------------------------------------------------------
//	Prepends data to the head of the data buffer

void
LDynamicBuffer::PrependBuffer(const char * inBuffer, SInt32 inByteCount)
{
	if ((inBuffer) && (*inBuffer)) {
		LDynamicBuffer * tempBuffer = new LDynamicBuffer(inBuffer, inByteCount);
		tempBuffer->ConcatenateBuffer(this);

		SetStream(tempBuffer->DetachStream());
		delete tempBuffer;
	}
}

// ---------------------------------------------------------------------------
//		¥ InsertBuffer
// ---------------------------------------------------------------------------
//	Inserts data into any location of the data buffer

void
LDynamicBuffer::InsertBuffer(const char * inBuffer, SInt32 position, SInt32 inByteCount)
{
	if ((!inBuffer) || (!*inBuffer)) {
		return;
	}

	if ((!mStream) || position >= GetBufferLength()) {
		ConcatenateBuffer(inBuffer, inByteCount);
	} else {
		mStream->SetMarker(0, streamFrom_Start);
		LDynamicBuffer tempBuffer(mStream, position);

		tempBuffer.ConcatenateBuffer(inBuffer, inByteCount);
		mStream->SetMarker(position, streamFrom_Start);
		tempBuffer.ConcatenateBuffer(mStream, (GetBufferLength() - position));

		SetStream(tempBuffer.DetachStream());
	}
}

// ---------------------------------------------------------------------------
//		¥ RemoveFromBuffer
// ---------------------------------------------------------------------------
//	Removes data from within the buffer

void
LDynamicBuffer::RemoveFromBuffer(SInt32 inPosition, SInt32 inByteCount)
{
	if (!mStream)
		return;

	if (inPosition >= GetBufferLength())
		return;

	//Optimization for removes in LHandleStream
	if (mHandleStream) {
		Handle ourHandle = mHandleStream->GetDataHandle();
		::HLock(ourHandle);

		char * beginRemoveSection = *ourHandle + inPosition;
		char * endRemoveSection = beginRemoveSection + inByteCount;
		SInt32 oldLength = mHandleStream->GetLength();
		UInt32 lengthToMove = (UInt32) (oldLength - (inPosition + inByteCount));

		PP_CSTD::memmove(beginRemoveSection, endRemoveSection, lengthToMove);

		::HUnlock(ourHandle);
		mHandleStream->SetLength(oldLength - inByteCount);
	} else if (inPosition == 0) {
		mStream->SetMarker(inByteCount, streamFrom_Start);
		LDynamicBuffer tempBuffer(mStream, (mStream->GetLength() - mStream->GetMarker()));
		SetStream(tempBuffer.DetachStream());
	} else {
		mStream->SetMarker(0, streamFrom_Start);
		LDynamicBuffer tempBuffer(mStream, inPosition);
		mStream->SetMarker(inPosition + inByteCount, streamFrom_Start);
		tempBuffer.ConcatenateBuffer(mStream, (mStream->GetLength() - mStream->GetMarker()));
		SetStream(tempBuffer.DetachStream());
	}
}

// ---------------------------------------------------------------------------
//		¥ GetBufferHandle
// ---------------------------------------------------------------------------
//	Attempts to return handle to the buffer
//
//	NOTE:
//		This returns the actual handle used by the buffer object. Be very
//		careful that you don't do anything to it that might confuse
//		the stream. I suggest you use GetBufferDataH to get a copy of the
//		buffer in memory.

Boolean
LDynamicBuffer::GetBufferHandle(Handle& outHandle) const
{
	if (mHandleStream)
		outHandle = mHandleStream->GetDataHandle();
	else
		return false;

	return true;
}

// ---------------------------------------------------------------------------
//		¥ GetBufferFile
// ---------------------------------------------------------------------------
//	Attempts to return file spec for the buffer
//	NOTE:
//		This returns the actual file spec used by the buffer object. Be very
//		careful that you don't do anything to it that might confuse
//		the stream.

Boolean
LDynamicBuffer::GetBufferFile(
	FSSpec	&outFileSpec)
{
	if (mTempFileStream)
		mTempFileStream->GetSpecifier(outFileSpec);
	else
		return false;

	return true;
}

// ---------------------------------------------------------------------------
//		¥ GetBufferData
// ---------------------------------------------------------------------------
//	Returns handle to copy of data in buffer. If data will not fit in memory,
//	this routine returns nil.
//	User is responsible for deleting handle

Handle
LDynamicBuffer::GetBufferDataH(SInt32 inLength)
{
	if (!inLength || (inLength > GetBufferLength()))
		inLength = GetBufferLength();

	Handle returnData = ::NewHandle(inLength);
	if (returnData && mStream) {
		StHandleLocker locked(returnData);
		mStream->SetMarker(0, streamFrom_Start);
		mStream->ReadData(*returnData, inLength);
	}
	return returnData;
}

// ---------------------------------------------------------------------------
//		¥ BufferToHandle
// ---------------------------------------------------------------------------
//	Resizes (if necessary) and copies data to the handle specified.
//	If requested data will not fit in memory, this routine returns false.

Boolean
LDynamicBuffer::BufferToHandle(Handle outHandle, SInt32 inLength, Boolean inTerminate)
{
	if ((!inLength) || (inLength > GetBufferLength()))
		inLength = GetBufferLength();

	if (inTerminate)
		::SetHandleSize(outHandle, inLength + 1);
	else
		::SetHandleSize(outHandle, inLength);

	if (MemError())
		return false;

	if (mStream) {
		StHandleLocker locked(outHandle);
		mStream->SetMarker(0, streamFrom_Start);
		mStream->ReadData(*outHandle, inLength);
	}

	if (inTerminate)
		(*outHandle)[inLength] = '\0';

	return true;
}

// ---------------------------------------------------------------------------
//		¥ BufferToFile
// ---------------------------------------------------------------------------

void
LDynamicBuffer::BufferToFile(LFile& outFile)
{
	if (!mStream) {
		return;
	}

	mStream->SetMarker(0, streamFrom_Start);

	SInt16 fileRef = outFile.OpenDataFork(fsRdWrPerm);

		// Optimization for copies from LHandleStream

	if (mHandleStream) {
		Handle ourHandle = mHandleStream->GetDataHandle();
		StHandleLocker locked(ourHandle);
		outFile.WriteDataFork(*ourHandle, mHandleStream->GetLength());

	} else {	// Try to allocate some swap space to speed things up
				//	Get the smaller of available memory or the stream size

		SInt32 streamSize = mStream->GetLength();
		SInt32 bufferSize = ::MaxBlock();
		bufferSize = (bufferSize < streamSize) ? bufferSize : streamSize;

		OSErr	err = ::SetFPos(fileRef, fsFromStart, 0);
		ThrowIfOSErr_(err);

		StPointerBlock tempSwap(bufferSize);
		SInt32	bytesWritten;
		while (streamSize > 0) {
			bytesWritten = mStream->ReadData(tempSwap, bufferSize);
			err = ::FSWrite(fileRef, &bytesWritten, tempSwap);
			ThrowIfOSErr_(err);
			streamSize -= bytesWritten;
		}

		err = ::SetEOF(fileRef, mStream->GetLength());
		ThrowIfOSErr_(err);
	}

	outFile.CloseDataFork();
}

// ---------------------------------------------------------------------------
//		¥ LowerBuffer
// ---------------------------------------------------------------------------
//	Converts all char's in the data buffer to lower case

void
LDynamicBuffer::LowerBuffer()
{
	if (!mStream)
		return;

	mStream->SetMarker(0, streamFrom_Start);

	while (1) {
		char currChar, lowerChar;

		if (!mStream->PeekData(&currChar, 1))
			break;

		lowerChar = (char) PP_CSTD::tolower(currChar);
		mStream->WriteData(&lowerChar, 1);
	}
}

// ---------------------------------------------------------------------------
//		¥ UpperBuffer
// ---------------------------------------------------------------------------
//	Converts char's in the data buffer to upper case

void
LDynamicBuffer::UpperBuffer()
{
	if (!mStream)
		return;

	mStream->SetMarker(0, streamFrom_Start);

	while (1) {
		char currChar, upperChar;

		if (!mStream->PeekData(&currChar, 1))
			break;

		upperChar = (char) PP_CSTD::toupper(currChar);
		mStream->WriteData(&upperChar, 1);
	}
}

// ---------------------------------------------------------------------------
//		¥ CheckStreamSize
// ---------------------------------------------------------------------------
//	Checks length of stream.
//	¥ Creates stream if none exists.
//	¥ If inGrowBy outgrows handle then it switches to a file stream automatically

void
LDynamicBuffer::CheckStreamSize(SInt32 inGrowBy)
{
	// If we already have a file stream then just return there is nothing to check
	if (mTempFileStream)
		return;

	//Get the smaller of available memory or the mMemoryThreshold value
	SInt32 memAvail = ::MaxBlock();
	memAvail = (memAvail < mMemoryThreshold) ? memAvail : (SInt32) mMemoryThreshold;
	Boolean isLargerThanMaxHandle = (GetBufferLength() + inGrowBy) > memAvail;

	//	case: No stream, and data larger than max handle size
	//		- create new file stream
	if ((!mStream) && isLargerThanMaxHandle) {
		mTempFileStream = new LTemporaryFileStream();
		mStream = mTempFileStream;

	//	case: No stream, and data smaller than max handle size
	//		- create new handle stream
	} else if ((!mStream) && (!isLargerThanMaxHandle)) {
		mHandleStream = new LHandleStream();
		mStream = mHandleStream;

	//	case: Have HandleStream, and new data larger than max handle size
	//		- create new file stream
	//		- copy data to file stream
	//		- delete the handle stream
	} else if (mHandleStream && isLargerThanMaxHandle) {
		mTempFileStream = new LTemporaryFileStream();
		mStream = mTempFileStream;

		{
			Handle dataHandle = mHandleStream->GetDataHandle();
			StHandleLocker locked(dataHandle);
			mTempFileStream->SetMarker(0, streamFrom_Start);
			mTempFileStream->WriteData(*dataHandle, ::GetHandleSize(dataHandle));
		}

		delete mHandleStream;
		mHandleStream = nil;
	}
}

PP_End_Namespace_PowerPlant
