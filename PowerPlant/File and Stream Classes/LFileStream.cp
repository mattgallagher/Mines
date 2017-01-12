// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileStream.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A File which uses a Stream to access its data fork

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LFileStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LFileStream							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LFileStream::LFileStream()
{
}


// ---------------------------------------------------------------------------
//	¥ LFileStream							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a FileStream from a Toolbox File System Specification

LFileStream::LFileStream(
	const FSSpec&	inFileSpec)

	: LFile(inFileSpec)
{
}


// ---------------------------------------------------------------------------
//	¥ LFileStream							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct a FileStream from an Alias
//
//	outWasChanged indicates if the AliasHandle was changed during resolution
//	inFromFile is a File Specifier for the starting point for a relative
//		search. If nil, an absolute search is performed

LFileStream::LFileStream(
	AliasHandle		inAlias,
	Boolean&		outWasChanged,
	FSSpec*			inFromFile)

	: LFile(inAlias, outWasChanged, inFromFile)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LFileStream							Destructor				  [public]
// ---------------------------------------------------------------------------

LFileStream::~LFileStream()
{
}


// ---------------------------------------------------------------------------
//	¥ SetMarker														  [public]
// ---------------------------------------------------------------------------
//	Place the Read/Write Marker at an offset from a specified position
//
//	inFromWhere can be streamFrom_Start, streamFrom_End, or streamFrom_Marker

void
LFileStream::SetMarker(
	SInt32			inOffset,
	EStreamFrom		inFromWhere)
{
		// LStream uses positive offsets from the end to mean move
		// backwards. But, SetFPos always uses negative offsets for
		// moving backwards. To allow people to use LFileStream from
		// a LStream pointer, we need to support the PP convention
		// by switching positive offsets from the end to the negative
		// value so that SetFPos does what was intended.

	if ((inFromWhere == streamFrom_End) && (inOffset > 0)) {
		inOffset = -inOffset;
	}

	OSErr	err = ::SetFPos(GetDataForkRefNum(), inFromWhere, inOffset);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//	¥ GetMarker														  [public]
// ---------------------------------------------------------------------------
//	Return the Read/Write Marker position
//
//	Position is a byte offset from the start of the data fork

SInt32
LFileStream::GetMarker() const
{
	SInt32	theMarker;
	OSErr	err = ::GetFPos(GetDataForkRefNum(), &theMarker);
	ThrowIfOSErr_(err);
	return theMarker;
}


// ---------------------------------------------------------------------------
//	¥ SetLength														  [public]
// ---------------------------------------------------------------------------
//	Set the length, in bytes, of the data fork of a FileStream

void
LFileStream::SetLength(
	SInt32	inLength)
{
	OSErr	err = ::SetEOF(GetDataForkRefNum(), inLength);
	ThrowIfOSErr_(err);
	LStream::SetLength(inLength);
}


// ---------------------------------------------------------------------------
//	¥ GetLength														  [public]
// ---------------------------------------------------------------------------
//	Return the length, in bytes, of the data fork of a FileStream

SInt32
LFileStream::GetLength() const
{
	SInt32	theLength;
	OSErr	err = ::GetEOF(GetDataForkRefNum(), &theLength);
	ThrowIfOSErr_(err);
	return theLength;
}


// ---------------------------------------------------------------------------
//	¥ PutBytes														  [public]
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to a DataStream
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.

ExceptionCode
LFileStream::PutBytes(
	const void*		inBuffer,
	SInt32&			ioByteCount)
{
	return ::FSWrite(GetDataForkRefNum(), &ioByteCount, inBuffer);
}


// ---------------------------------------------------------------------------
//	¥ GetBytes														  [public]
// ---------------------------------------------------------------------------
//	Read bytes from a DataStream to a buffer
//
//	Returns an error code and passes back the number of bytes actually
//	read, which may be less than the number requested if an error occurred.

ExceptionCode
LFileStream::GetBytes(
	void*		outBuffer,
	SInt32&		ioByteCount)
{
	return ::FSRead(GetDataForkRefNum(), &ioByteCount, outBuffer);
}


PP_End_Namespace_PowerPlant
