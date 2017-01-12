// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDataStream.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Stream whose bytes are in block of memory defined by a pointer to
//	the first byte and a byte count

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDataStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LDataStream							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LDataStream::LDataStream()
{
	mBuffer = nil;					// Buffer is undefined
}


// ---------------------------------------------------------------------------
//	¥ LDataStream							Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Copy points to the same buffer as the original

LDataStream::LDataStream(
	const LDataStream&	inOriginal)

	: LStream(inOriginal)
{
	mBuffer = inOriginal.mBuffer;
}


// ---------------------------------------------------------------------------
//	¥ LDataStream							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a pointer and a byte count
//
//	inBuffer points to the first byte of the Stream, which is inLength
//	bytes long

LDataStream::LDataStream(
	void*	inBuffer,
	SInt32	inLength)
{
	SetBuffer(inBuffer, inLength);
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------
//	This DataStream will point to the same buffer as the original

LDataStream&
LDataStream::operator = (
	const LDataStream&	inOriginal)
{
	if (this != &inOriginal) {
		LStream::operator=(inOriginal);		// Base class assignment

		mBuffer = inOriginal.mBuffer;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LDataStream							Destructor				  [public]
// ---------------------------------------------------------------------------

LDataStream::~LDataStream()
{
}


// ---------------------------------------------------------------------------
//	¥ SetBuffer														  [public]
// ---------------------------------------------------------------------------

void
LDataStream::SetBuffer(
	void*	inBuffer,
	SInt32	inLength)
{
	mBuffer = inBuffer;
	SetLength(inLength);
}


// ---------------------------------------------------------------------------
//	¥ PutBytes														  [public]
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to a DataStream
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.
//
//	Errors:
//		writeErr		Attempt to write past end of DataStream

ExceptionCode
LDataStream::PutBytes(
	const void*		inBuffer,
	SInt32&			ioByteCount)
{
	ExceptionCode	err = noErr;

	if (GetMarker() + ioByteCount > GetLength()) {
		ioByteCount = GetLength() - GetMarker();
		err = writErr;
	}

	::BlockMoveData(inBuffer, (SInt8*) mBuffer + GetMarker(), ioByteCount);
	SetMarker(ioByteCount, streamFrom_Marker);

	return err;
}


// ---------------------------------------------------------------------------
//	¥ GetBytes														  [public]
// ---------------------------------------------------------------------------
//	Read bytes from a DataStream to a buffer
//
//	Returns an error code and passes back the number of bytes actually
//	read, which may be less than the number requested if an error occurred.
//
//	Errors:
//		readErr		Attempt to read past the end of the DataStream

ExceptionCode
LDataStream::GetBytes(
	void*		outBuffer,
	SInt32&		ioByteCount)
{
	ExceptionCode	err = noErr;
									// Upper bound is number of bytes from
									//   marker to end
	if (GetMarker() + ioByteCount > GetLength()) {
		ioByteCount = GetLength() - GetMarker();
		err = readErr;
	}

	::BlockMoveData((SInt8*) mBuffer + GetMarker(), outBuffer, ioByteCount);
	SetMarker(ioByteCount, streamFrom_Marker);

	return err;
}


PP_End_Namespace_PowerPlant
