// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHandleStream.cp			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Stream whose bytes are in a Handle block in memory

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LHandleStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LHandleStream							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LHandleStream::LHandleStream()
{
	mDataH = nil;						// No data yet
}


// ---------------------------------------------------------------------------
//	¥ LHandleStream							Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Copies data Handle of the original

LHandleStream::LHandleStream(
	const LHandleStream&	inOriginal)

	: LStream(inOriginal)
{
	mDataH = inOriginal.mDataH;

	if (mDataH != nil) {
		ThrowIfOSErr_( ::HandToHand(&mDataH) );
	}
}


// ---------------------------------------------------------------------------
//	¥ LHandleStream							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct from an existing Handle
//
//	The LHandleStream object assumes ownership of the Handle

LHandleStream::LHandleStream(
	Handle	inHandle)
{
	mDataH = inHandle;

	if (inHandle != nil) {				// Adjust length count
		LStream::SetLength(::GetHandleSize(inHandle));
	}
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------
//	Disposes of existing data Handle and copies data Handle of original

LHandleStream&
LHandleStream::operator = (
	const LHandleStream&	inOriginal)
{
	if (this != &inOriginal) {

		LStream::operator=(inOriginal);

		Handle	copyH = inOriginal.mDataH;		// Copy original's Handle
												//   into a local Handle
		if (copyH != nil) {
			ThrowIfOSErr_( ::HandToHand(&copyH) );
		}

		if (mDataH != nil) {					// Copy succeeded. Delete
			::DisposeHandle(mDataH);			//   our existing Handle
		}

		mDataH = copyH;							// Store copied Handle
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LHandleStream						Destructor				  [public]
// ---------------------------------------------------------------------------

LHandleStream::~LHandleStream()
{
	if (mDataH != nil) {
		::DisposeHandle(mDataH);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetLength														  [public]
// ---------------------------------------------------------------------------
//	Set the length, in bytes, of the HandleStream

void
LHandleStream::SetLength(
	SInt32	inLength)
{
	if (mDataH == nil) {				// Allocate new Handle for data
		mDataH = ::NewHandle(inLength);

	} else {							// Resize existing Handle
		::SetHandleSize(mDataH, inLength);
	}

	ThrowIfMemError_();
	LStream::SetLength(inLength);		// Adjust length count
}


// ---------------------------------------------------------------------------
//	¥ PutBytes
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to a HandleStream
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.
//
//	Grows data Handle if necessary.
//
//	Errors:
//		memFullErr		Growing Handle failed when trying to write past
//							the current end of the Stream

ExceptionCode
LHandleStream::PutBytes(
	const void*		inBuffer,
	SInt32&			ioByteCount)
{
	ExceptionCode	err = noErr;
	SInt32			endOfWrite = GetMarker() + ioByteCount;

	if (endOfWrite > GetLength()) {		// Need to grow Handle

		try {
			SetLength(endOfWrite);
		}

		catch (ExceptionCode inErr) {	// Grow failed. Write only what fits.
			ioByteCount = GetLength() - GetMarker();
			err = inErr;
		}

		catch (const LException& inException) {
			ioByteCount = GetLength() - GetMarker();
			err = inException.GetErrorCode();
		}
	}
										// Copy bytes into Handle
	if (ioByteCount > 0) {				//   Byte count will be zero if
										//   mDataH is nil
		::BlockMoveData(inBuffer, *mDataH + GetMarker(), ioByteCount);
		SetMarker(ioByteCount, streamFrom_Marker);
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ GetBytes
// ---------------------------------------------------------------------------
//	Read bytes from a HandleStream to a buffer
//
//	Returns an error code and passes back the number of bytes actually
//	read, which may be less than the number requested if an error occurred.
//
//	Errors:
//		readErr		Attempt to read past the end of the HandleStream

ExceptionCode
LHandleStream::GetBytes(
	void*		outBuffer,
	SInt32&		ioByteCount)
{
	ExceptionCode	err = noErr;
									// Upper bound is number of bytes from
									//   marker to end
	if ((GetMarker() + ioByteCount) > GetLength()) {
		ioByteCount = GetLength() - GetMarker();
		err = readErr;
	}
									// Copy bytes from Handle into buffer
	if (mDataH != nil) {
		::BlockMoveData(*mDataH + GetMarker(), outBuffer, ioByteCount);
		SetMarker(ioByteCount, streamFrom_Marker);
	}

	return err;
}


// ---------------------------------------------------------------------------
//	¥ SetDataHandle
// ---------------------------------------------------------------------------
//	Specify a Handle to use as the basis for a HandleStream
//
//	Class assumes ownership of the input Handle and destroys the
//	existing data Handle. Call DetachDataHandle() beforehand if
//	you wish to preserve the existing data Handle.

void
LHandleStream::SetDataHandle(
	Handle	inHandle)
{
	if (mDataH != nil) {				// Free existing Handle
		::DisposeHandle(mDataH);
	}

	mDataH = inHandle;

	SInt32	newLength = 0;				// Set length count
	if (inHandle != nil) {
		newLength = ::GetHandleSize(inHandle);
	}
	LStream::SetLength(newLength);

	SetMarker(0, streamFrom_Start);
}


// ---------------------------------------------------------------------------
//	¥ DetachDataHandle
// ---------------------------------------------------------------------------
//	Dissociate the data Handle from a HandleStream.
//
//	Creates a new, empty data Handle and passes back the existing Handle.
//	Caller assumes ownership of the Handle.

Handle
LHandleStream::DetachDataHandle()
{
	Handle	dataHandle = mDataH;		// Save current data Handle

	SetMarker(0, streamFrom_Start);
	LStream::SetLength(0);
	mDataH = nil;						// Reset to nil Handle

	return dataHandle;
}


PP_End_Namespace_PowerPlant
