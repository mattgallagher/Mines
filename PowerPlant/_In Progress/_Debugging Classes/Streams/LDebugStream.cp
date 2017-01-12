// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDebugStream.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	A class for streaming data for debugging purposes. The data is optionally
//	buffered until flushed or flushed immediately. When flushed, the data
//	can be sent to a debugger, file, console (e.g. SIOUX), or to the location
//	specified by gDebugThrow or gDebugSignal (UDebugging).
//
//	The stream is one-way -- write only -- as there's no (unified) API for
//	obtaining information from debuggers.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDebugStream.h>
#include <PP_KeyCodes.h>
#include <LFileStream.h>
#include <LString.h>
#include <UProcess.h>
#include <UMemoryMgr.h>
#include <UKeyFilters.h>
#include <PP_DebugMacros.h>
#include <UCursor.h>
#include <UTBAccessors.h>

#include <UDebugUtils.h>

#include <ToolUtils.h>

#include <cstdio>
#include <cstring>

#if PP_Debug_Allow_DebugStream_Flush_To_Console
	#include <iostream>
#endif

PP_Begin_Namespace_PowerPlant

const SInt16	kLogFilenameBaseLength	=	21;
const OSType	kLogCreatorType			=	FOUR_CHAR_CODE('CWIE');
const OSType	kLogFileType			=	FOUR_CHAR_CODE('TEXT');


// ---------------------------------------------------------------------------
//	¥ LDebugStream									[public]
// ---------------------------------------------------------------------------
//	Default constructor

LDebugStream::LDebugStream()
{
	mMarker			= 0;
	mLength			= 0;
	mAutoFlush		= false;
	mAppendToFile	= true;
	mFlushLocation	= flushLocation_File;
	mDataH			= nil;

	Init();
}


// ---------------------------------------------------------------------------
//	¥ LDebugStream									[public]
// ---------------------------------------------------------------------------
//	Parameterized constructor

LDebugStream::LDebugStream(
	EFlushLocation	inFlushLocation,
	bool			inAutoFlush,
	bool			inAppendToFile)
{
	mAutoFlush		= inAutoFlush;
	mAppendToFile	= inAppendToFile;
	mFlushLocation	= inFlushLocation;
	mMarker			= 0;
	mLength			= 0;
	mDataH			= nil;

	Init();
}


// ---------------------------------------------------------------------------
//	¥ ~LDebugStream									[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LDebugStream::~LDebugStream()
{
	if (mDataH != nil) {
		::DisposeHandle(mDataH);
	}
}


// ---------------------------------------------------------------------------
//	¥ Init											[private]
// ---------------------------------------------------------------------------
//	Private initializer

void
LDebugStream::Init()
{
		// Get the FSSpec for the log file. Place it into the
		// same folder as the host application.
	SProcessInfo procInfo;
	UProcess::GetMyInfo(procInfo);
	mFileLocation = procInfo.appSpec;

	SetFilename();
}


// ---------------------------------------------------------------------------
//	¥ SetFilename									[public, virtual]
// ---------------------------------------------------------------------------

void
LDebugStream::SetFilename(
	ConstStringPtr	inName)
{
	LStr255 theName = (inName == nil) ? mFileLocation.name : inName;

		// Set the base filename to no more than 21 characters. MacOS 31
		// char filename limit minus 10 (for " debug log").
	if (theName.Length() > kLogFilenameBaseLength) {
		theName.Remove(kLogFilenameBaseLength + 1, static_cast<UInt8>(theName.Length() - 18));
	}

		// Append the rest of the base
	theName += StringLiteral_(" debug log");

	LString::CopyPStr(theName, mFileLocation.name, sizeof(StrFileName));
}


// ---------------------------------------------------------------------------
//	¥ TimeStamp										[public, virtual]
// ---------------------------------------------------------------------------
//	creates a time stamp

void
LDebugStream::TimeStamp(
	LStr255&	outStamp)
{
	UInt32	time;
	::GetDateTime(&time);
	Str255 dateString;
	::DateString(static_cast<SInt32>(time), abbrevDate, dateString, nil);
	Str255 timeString;
	::TimeString(static_cast<SInt32>(time), true, timeString, nil);

	outStamp = dateString;
	outStamp += char_Space;
	outStamp += timeString;
	outStamp += char_Return;
}


// ---------------------------------------------------------------------------
//	¥ GetHeader										[public, virtual]
// ---------------------------------------------------------------------------
//	Returns a Handle to text. This text would constitute a "header" to
//	prepend to the file log to delimit each Flush(). Could contain information
//	such as a time stamp, application name, version, etc.
//
//	Allocates a Handle and returns that Handle. It is the responsibility of
//	the caller to dispose of the returned Handle.

Handle
LDebugStream::GetHeader()
{
	// This header will create an output like this (minus the comment marks):

	//
	// ---------------------------------------------------------------------------
	// "MyKillerApp" debug log for Sun, Jan 4, 1998 6:23:32 AM
	// ---------------------------------------------------------------------------
	//

	StHandleBlock	headerH(Size_Zero);				// Our buffer

		// Insert a seperator line
	LStr255 text(StringLiteral_("\r---------------------------------------------------------------------------\r"));
	OSErr err = ::PtrAndHand(text.TextPtr(), headerH, text.LongLength());
	ThrowIfOSErr_(err);

		// Insert some nifty text
	SProcessInfo procInfo;
	UProcess::GetMyInfo(procInfo);
	text = '"';
	text += procInfo.name;
	text += '"';
	text += StringLiteral_(" debug log for ");
	err = ::PtrAndHand(text.TextPtr(), headerH, text.LongLength());
	ThrowIfOSErr_(err);

		// Insert a time stamp
	TimeStamp(text);
	err = ::PtrAndHand(text.TextPtr(), headerH, text.LongLength());
	ThrowIfOSErr_(err);

	text = StringLiteral_("---------------------------------------------------------------------------\r\r");
	err = ::PtrAndHand(text.TextPtr(), headerH, text.LongLength());
	ThrowIfOSErr_(err);

	ValidateHandle_(headerH.Get());

	return headerH.Release();
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDataHandle									[public]
// ---------------------------------------------------------------------------
//	Specify a Handle to use as the basis for the debug stream
//
//	Class assumes ownership of the input Handle and destroys the
//	existing data Handle. Call DetachDataHandle() beforehand if
//	you wish to preserve the existing data Handle.

void
LDebugStream::SetDataHandle(
	Handle	inHandle)
{
	QCVerifyHandle_(inHandle);			// ValidateHandle_ would throw if nil, but
										// that is an acceptable situation.

	if (mDataH != nil) {				// Free existing Handle
		::DisposeHandle(mDataH);
	}

	mDataH = inHandle;

	SInt32	newLength = 0;				// Set length count
	if (inHandle != nil) {
		newLength = ::GetHandleSize(inHandle);
	}

	mLength = newLength;

	SetMarker(0, streamFrom_Start);
}

// Inlined in header
#pragma mark LDebugStream::GetDataHandle


// ---------------------------------------------------------------------------
//	¥ DetachDataHandle								[public]
// ---------------------------------------------------------------------------
//	Dissociate the data Handle from the debug stream
//
//	Creates a new, empty data Handle and passes back the existing Handle.
//	Caller assumes ownership of the Handle.

Handle
LDebugStream::DetachDataHandle()
{
	Handle	dataHandle = mDataH;	// Save current data Handle

	SetMarker(0, streamFrom_Start);
	mLength = 0;
	mDataH = nil;					// Reset to nil Handle

	QCVerifyHandle_(dataHandle);

	return dataHandle;
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Flush											[public, virtual]
// ---------------------------------------------------------------------------
//	Flushes the internal buffer to the appropriate location. Allows one to
//	optionally change where to flush this time around.
//
//	After flushing, the internal buffer is optionally disposed of (for a
//	"full flush").

void
LDebugStream::Flush(
	EFlushLocation	inFlushLocation,
	bool			inDisposeAfterFlush)
{
	if (mDataH == nil) {
		return;
	}

	ValidateHandle_(mDataH);

		// Where to flush?
	EFlushLocation theLocation = ((inFlushLocation == flushLocation_Default)
									? mFlushLocation : inFlushLocation);


		// Sanity checks on flush location
	if (theLocation == flushLocation_Default) {
		theLocation = flushLocation_File;
	}
#ifndef Debug_Throw
	if (theLocation == 4) { // flushLocation_DebugThrow
		theLocation = flushLocation_File;
	}
#endif

#ifndef Debug_Signal
	if (theLocation == 5) { // flushLocation_DebugSignal
		theLocation = flushLocation_File;
	}
#endif

		// Perform the flush
	if (theLocation == flushLocation_File) {

		LFileStream	theFile(mFileLocation);

		try {
			StDisableDebugThrow_();

			theFile.CreateNewDataFile(kLogCreatorType, kLogFileType);
		} catch (LException& iErr) {

				// It's ok if the file already exists as this will append
				// or overwrite.
			if (iErr.GetErrorCode() != dupFNErr) {
				throw;
			}
		}

		theFile.OpenDataFork(fsRdWrPerm);

			// The technique of inserting the data into the file seems a bit
			// awkward, but is necessary as LFileStream/LStream outputs its
			// data in a binary format. As the output here needs to be ASCII text,
			// a little manipulation needs to be done.

			// Insert any headers
		StHandleBlock theHeaderH(GetHeader());

			// Append or overwrite?
		EStreamFrom startFrom = mAppendToFile ? streamFrom_End : streamFrom_Start;

			// Write the header
		theFile.SetMarker(0, startFrom);
		Size bufferSize = ::GetHandleSize(theHeaderH);
		StHandleLocker lock(theHeaderH);
		ThrowIfError_(theFile.PutBytes(*theHeaderH.Get(), bufferSize));

			// Write out the rest of the data
		bufferSize = ::GetHandleSize(mDataH);
		StHandleLocker lock2(mDataH);
		ThrowIfError_(theFile.PutBytes(*mDataH, bufferSize));

			// Adjust
		theFile.SetLength(theFile.GetMarker());

	} else if ( theLocation == flushLocation_Debugger
		#ifdef Debug_Throw
				|| theLocation == flushLocation_DebugThrow
		#endif
		#ifdef Debug_Signal
				|| theLocation == flushLocation_DebugSignal
		#endif
		) {

		Size bufSize = ::GetHandleSize(mDataH);
		if (bufSize > 255) {
			bufSize = 255;
		}

		StHandleLocker lock(mDataH);
		LStr255	text(*mDataH, static_cast<UInt8>(bufSize));

			// Scan the text for any carriage returns or line feeds (replacing
			// with a space). They don't work well here. Do note that semi-colons
			// can have special meaning (e.g. in MacsBug), so they are not stripped
			// out as they could be desired.
		for (SInt32 position = 1; position <= text.LongLength(); position++) {

			if (((StringPtr)text)[position] == char_LineFeed
				|| ((StringPtr)text)[position] == char_Return) {

				((StringPtr)text)[position] = char_Space;
			}
		}

			// The string is now ready for flushing, so flush based upon the
			// location.
		if ((theLocation == flushLocation_Debugger)
		#ifdef Debug_Throw
			|| ((theLocation == flushLocation_DebugThrow)
			&& (UDebugging::GetDebugThrow() == debugAction_Debugger))
		#endif
			) {

			UDebugUtils::DebugStr(text);

		}
	#ifdef Debug_Throw
		else if (theLocation == flushLocation_DebugThrow) {

				// Either alert or nothing. Always force an alert.
			UDebugging::AlertThrowAt(text, __func__, StringLiteral_(__FILE__), __LINE__);

		}
	#endif
	#ifdef Debug_Signal
		else if (theLocation == flushLocation_DebugSignal) {
			SignalString_(text);
		}
	#endif
	#ifdef Debug_Signal
		else {

				// This should never be reached.
			SignalStringLiteral_("Unknown flush location");
		}
	#endif

	}
#if PP_Debug_Allow_DebugStream_Flush_To_Console

	else if (theLocation == flushLocation_Console) {

			// Due to some MSL/SIOUX/ANSI C++ issues (not necessarily bugs),
			// perform some minor manipulation of the data before it is sent
			// to the console.

			// Due to these potential changes, make a copy of the data buffer
			// so the user's data isn't affected. However, if the internal
			// data buffer is going to be disposed of after the flush anyways
			// then avoid the extra memory overhead and use that main buffer.
		Handle	theDataH = mDataH;
		bool	madeCopy = (inDisposeAfterFlush == false);
		if (madeCopy) {
			ThrowIfOSErr_(::HandToHand(&theDataH));
		}

			// std::cout will treat this as a C string, so add a NULL terminator
		Size hSize = ::GetHandleSize(theDataH) + 1;
		::SetHandleSize(theDataH, hSize);
		OSErr err = ::MemError();
		if (err != noErr) {
			if (madeCopy) {
				::DisposeHandle(theDataH);
			}
			Throw_(err);
		}

		(*theDataH)[hSize-1] = '\0';

			// SIOUX seems to prefer \n over \r, so convert them.
		Ptr s = *theDataH;
		while (*s != '\0') {
			if (*s == char_Return) {
				*s = char_LineFeed;
			}
			s++;
		}

			// Output the text to the console.
		StHandleLocker	lock(theDataH);
		PP_STD::cout << *theDataH << PP_STD::endl;

		if (madeCopy) {
			::DisposeHandle(theDataH);
		}
	}
#endif // PP_Debug_Allow_DebugStream_Flush_To_Console
	else {
			// This point should never be reached.
		SignalStringLiteral_("Unknown EFlushLocation");
	}

		// Notification is complete. Clear the buffer?
	if (inDisposeAfterFlush) {
		::DisposeHandle(mDataH);
		mDataH	= nil;
		mMarker	= 0;
		mLength	= 0;
	}
}


	// Declared inline in the header
#pragma mark LDebugStream::GetAutoFlush
#pragma mark LDebugStream::SetAutoFlush
#pragma mark LDebugStream::GetFlushLocation
#pragma mark LDebugStream::SetFlushLocation

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetMarker										[public, virtual]
// ---------------------------------------------------------------------------
//	Place the Read/Write Marker at an offset from a specified position
//
//	inFromWhere can be streamFrom_Start, streamFrom_End, or streamFrom_Marker

void
LDebugStream::SetMarker(
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

	if (newMarker < 0) {				// Marker must be between 0 and
		newMarker = 0;					// Length, inclusive
	} else if (newMarker > GetLength()) {
		newMarker = GetLength();
	}

	mMarker = newMarker;
}


// ---------------------------------------------------------------------------
//	¥ GetMarker										[public, virtual]
// ---------------------------------------------------------------------------
//	Return the Read/Write Marker position
//
//	Position is a byte offset from the start of the Stream

SInt32
LDebugStream::GetMarker() const
{
	return mMarker;
}


// ---------------------------------------------------------------------------
//	¥ SetLength										[public, virtual]
// ---------------------------------------------------------------------------
//	Set the length, in bytes, of the internal buffer

void
LDebugStream::SetLength(
	SInt32	inLength)
{
	if (mDataH == nil) {				// Allocate new Handle for data
		mDataH = ::NewHandle(inLength);
	} else {							// Resize existing Handle
		::SetHandleSize(mDataH, inLength);
	}
	ThrowIfMemError_();

	ValidateHandle_(mDataH);

	SInt32	oldLength = GetLength();
	mLength = inLength;
										// If making Stream shorter, call
										//   SetMarker to make sure that
	if (oldLength > inLength) {			//   marker is not past the end
		SetMarker(GetMarker(), streamFrom_Start);
	}

}


// ---------------------------------------------------------------------------
//	¥ GetLength										[public, virtual]
// ---------------------------------------------------------------------------
//	Return the length, in bytes, of the stream

SInt32
LDebugStream::GetLength() const
{
	return mLength;
}

	// Inlined in header
#pragma mark LDebugStream::AtEnd


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ PutBytes										[public, virtual]
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to the internal buffer
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.
//
//	Grows data Handle if necessary.
//
//	Before storing the buffer internally, inBuffer is scanned for null
//	characters. If any are found, they are stripped and replaced with
//	a space.
//
//	Errors:
//		memFullErr		Growing Handle failed when trying to write past
//							the current end of the Stream

ExceptionCode
LDebugStream::PutBytes(
	const void*	inBuffer,
	SInt32&		ioByteCount)
{
	ExceptionCode	err = noErr;
	SInt32	endOfWrite = GetMarker() + ioByteCount;

	if (endOfWrite > GetLength()) {	// Need to grow Handle

		try {
			StDisableDebugThrow_();
			SetLength(endOfWrite);
		}

		catch (LException& inErr) {				// Grow failed. Write only what fits.
			ioByteCount = GetLength() - GetMarker();
			err = inErr.GetErrorCode();
		}
	}
							// Copy bytes into Handle
	if (ioByteCount > 0) {	//   Byte count will be zero if
							//   mDataH is nil

		err = noErr;

			// Scan for null characters. A local copy of inBuffer must be
			// made since this action could be changing portions of the
			// buffer. The method promises that inBuffer will remain const.
		try {
			StHandleBlock	localBufH(ioByteCount, true, true);
			::BlockMoveData(inBuffer, *localBufH.Get(), ioByteCount);
			ValidateHandle_(localBufH);

			Ptr s = *localBufH;	// Doesn't move memory
			for (SInt32 count = ioByteCount; count > 0; count--) {
				if (*s == '\0') {
					*s = char_Space;
				}
				s++;
			}

			::BlockMoveData(*localBufH.Get(), *mDataH + GetMarker(), ioByteCount);
			SetMarker(ioByteCount, streamFrom_Marker);

		} catch (LException& inErr) {
			err = inErr.GetErrorCode();
		}
	}

	if ((err == noErr) && mAutoFlush) {
		Flush(flushLocation_Default);
	}

	return err;
}

	// Inlined in header
#pragma mark LDebugStream::WriteData


// ---------------------------------------------------------------------------
//	¥ WriteBlock									[public, virtual]
// ---------------------------------------------------------------------------
//	Write data, specified by a pointer and byte count, to a Stream

void
LDebugStream::WriteBlock(
	const void*		inBuffer,
	SInt32			inByteCount)
{
	ThrowIfError_(PutBytes(inBuffer, inByteCount));
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ WritePtr
// ---------------------------------------------------------------------------
//	Write a Toolbox Ptr block to the debug stream
//
//	Returns the number of bytes written

SInt32
LDebugStream::WritePtr(
	const Ptr	inPtr)
{
	SInt32	bytesWritten = 0;

	if (inPtr != nil) {

		ValidatePtr_(inPtr);

		SInt32	ptrLength = ::GetPtrSize(inPtr);

		WriteBlock(inPtr, ptrLength);
		bytesWritten = ptrLength;

	} else {
		*this << StringLiteral_("Ptr is nil");
	}

	return bytesWritten;
}

// ---------------------------------------------------------------------------
//	¥ WriteHandle
// ---------------------------------------------------------------------------
//	Write a Toolbox Handle block to a Stream
//
//	Returns the number of bytes written

SInt32
LDebugStream::WriteHandle(
	const Handle	inHandle)
{
	SInt32	bytesWritten = 0;

	if (inHandle != nil) {

		ValidateHandle_(inHandle);

		SInt32	handleLength = ::GetHandleSize(inHandle);

		StHandleLocker	lockMe(inHandle);
		WriteBlock(*inHandle, handleLength);
		bytesWritten = handleLength;

	} else {
		*this << StringLiteral_("Handle is nil");
	}

	return bytesWritten;
}


// ---------------------------------------------------------------------------
//	¥ WritePString
// ---------------------------------------------------------------------------
//	Write a Pascal string to a Stream
//
//	Returns the number of bytes written

SInt32
LDebugStream::WritePString(
	ConstStringPtr	inString)
{
	SInt32	bytesToWrite = inString[0];

	WriteBlock(&inString[1], bytesToWrite);

	return bytesToWrite;
}


// ---------------------------------------------------------------------------
//	¥ WriteCString
// ---------------------------------------------------------------------------
//	Write a C string to a Stream
//
//	Returns the number of bytes written.

SInt32
LDebugStream::WriteCString(
	const char*	inString)
{
	SInt32		strLen = 0;				// Find length of C string
	const char	*s = inString;
	while (*s++ != 0) {
		strLen++;
	}

		// Do not write the null terminator.
	WriteBlock(inString, strLen);

	return strLen;
}

// ---------------------------------------------------------------------------
//	¥ WriteInteger									[public]
// ---------------------------------------------------------------------------
//	Writes an integer.

SInt32
LDebugStream::WriteInteger(
	SInt32	inInteger)
{
	Str255	intString;
	::NumToString(inInteger, intString);

	return WritePString(intString);
}


// ---------------------------------------------------------------------------
//	¥ WriteBool										[public]
// ---------------------------------------------------------------------------
//	Write a bool. Outputs text (true/false) instead of numbers (1/0). Returns
//	the number of bytes written.

SInt32
LDebugStream::WriteBool(
	bool	inBool)
{
	LStr255 boolString = (inBool) ? StringLiteral_("bool: true") : StringLiteral_("bool: false");
	return WritePString(boolString);
}


// ---------------------------------------------------------------------------
//	¥ WriteRect										[public]
// ---------------------------------------------------------------------------
//	Write a Rect. Outputs formatted text. Returns the number of bytes
//	written

SInt32
LDebugStream::WriteRect(
	const Rect&		inRect)
{
	LStr255	rectString(StringLiteral_("Rect - "));
	rectString += StringLiteral_("top: ");
	rectString += static_cast<SInt32>(inRect.top);
	rectString += StringLiteral_(" left: ");
	rectString += static_cast<SInt32>(inRect.left);
	rectString += StringLiteral_(" bottom: ");
	rectString += static_cast<SInt32>(inRect.bottom);
	rectString += StringLiteral_(" right: ");
	rectString += static_cast<SInt32>(inRect.right);

	return WritePString(rectString);
}


// ---------------------------------------------------------------------------
//	¥ WritePoint									[public]
// ---------------------------------------------------------------------------
//	Write a Point. Outputs formatted text. Returns the number of bytes
//	written.

SInt32
LDebugStream::WritePoint(
	const Point&	inPoint)
{
	LStr255 pointString(StringLiteral_("Point - "));
	pointString += StringLiteral_("v: ");
	pointString += static_cast<SInt32>(inPoint.v);
	pointString += StringLiteral_(" h: ");
	pointString += static_cast<SInt32>(inPoint.h);

	return WritePString(pointString);
}


// ---------------------------------------------------------------------------
//	¥ WriteRGBColor									[public]
// ---------------------------------------------------------------------------
//	Write an RGBColor. Outputs formatted text. Returns the number of bytes
//	written

SInt32
LDebugStream::WriteRGBColor(
	const RGBColor&		inColor)
{
	LStr255	colorString(StringLiteral_("RGBColor - "));
	colorString += StringLiteral_("red: ");
	colorString += static_cast<SInt32>(inColor.red);
	colorString += StringLiteral_(" green: ");
	colorString += static_cast<SInt32>(inColor.green);
	colorString += StringLiteral_(" blue: ");
	colorString += static_cast<SInt32>(inColor.blue);

	return WritePString(colorString);
}


// ---------------------------------------------------------------------------
//	¥ WriteRegion									[public]
// ---------------------------------------------------------------------------
//	Write a Region. Outputs formatted text. Returns the number of bytes
//	written

#if PP_Target_Classic					// Regions are opaque in Carbon

SInt32
LDebugStream::WriteRegion(
	const MacRegion&	inRgn)
{
	LStr255	rgnString(StringLiteral_("Region - "));
	rgnString += StringLiteral_("size: ");
	rgnString += static_cast<SInt32>(inRgn.rgnSize);
	rgnString += StringLiteral_(" rgnBBox:");
	rgnString += StringLiteral_(" top: ");
	rgnString += static_cast<SInt32>(inRgn.rgnBBox.top);
	rgnString += StringLiteral_(" left: ");
	rgnString += static_cast<SInt32>(inRgn.rgnBBox.left);
	rgnString += StringLiteral_(" bottom: ");
	rgnString += static_cast<SInt32>(inRgn.rgnBBox.bottom);
	rgnString += StringLiteral_(" right: ");
	rgnString += static_cast<SInt32>(inRgn.rgnBBox.right);

	return WritePString(rgnString);
}

#endif

// ---------------------------------------------------------------------------
//	¥ WriteRegion									[public]
// ---------------------------------------------------------------------------
//	Write a Region. Outputs formatted text. Returns the number of bytes
//	written

SInt32
LDebugStream::WriteRegion(
	const RgnHandle	inRgnH)
{
#if PP_Target_Carbon

	Rect	bounds;
	::GetRegionBounds(inRgnH, &bounds);

	LStr255	rgnString(StringLiteral_("Region - "));
	rgnString += StringLiteral_("rgnBBox:");
	rgnString += StringLiteral_(" top: ");
	rgnString += static_cast<SInt32>(bounds.top);
	rgnString += StringLiteral_(" left: ");
	rgnString += static_cast<SInt32>(bounds.left);
	rgnString += StringLiteral_(" bottom: ");
	rgnString += static_cast<SInt32>(bounds.bottom);
	rgnString += StringLiteral_(" right: ");
	rgnString += static_cast<SInt32>(bounds.right);

	return WritePString(rgnString);

#else

	return WriteRegion(**inRgnH);

#endif
}


// ---------------------------------------------------------------------------
//	¥ WriteFourCharCode								[public]
// ---------------------------------------------------------------------------
//	Write a FourCharCode (ResType, OSType, etc.). Outputs formatted text.
//	Returns the number of bytes written

SInt32
LDebugStream::WriteFourCharCode(
	FourCharCode	inCode)
{
	LStr255	charString(StringLiteral_("FourCharCode: "));
	charString += char_Apostrophe;
	charString += LStr255(static_cast<FourCharCode>(inCode));
	charString += char_Apostrophe;

	return WritePString(charString);
}


// ---------------------------------------------------------------------------
//	¥ WriteFixed									[public][
// ---------------------------------------------------------------------------
//	Write a Fixed. Outputs formatted text. Returns the number of bytes
//	written

SInt32
LDebugStream::WriteFixed(
	Fixed	inFixed)
{
	LStr255	fixString(StringLiteral_("Fixed: "));
	fixString += static_cast<SInt32>(HiWord(inFixed));
	fixString += char_Period;
	fixString += static_cast<SInt32>(LoWord(inFixed));

	return WritePString(fixString);
}


// ---------------------------------------------------------------------------
//	¥ WriteNumVersion								[public]
// ---------------------------------------------------------------------------
//	Write a NumVersion. Outputs formatted text. Returns the number of
//	bytes written. Logic borrowed from CVersCaption.


SInt32
LDebugStream::WriteNumVersion(
	const NumVersion&	inNumVersion)
{
	LStr255	numString(StringLiteral_("NumVersion: "));

	UInt8	ver1,
			ver2,
			ver3,
			relStatus,
			prerelNum;

	ver1 = inNumVersion.majorRev;
	ver1 = static_cast<UInt8>((((ver1 & 0xF0 ) >> 4) * 10) + (ver1 & 0x0F));
	ver2 = static_cast<UInt8>((inNumVersion.minorAndBugRev & 0xF0) >> 4);
	ver3 = static_cast<UInt8>((inNumVersion.minorAndBugRev & 0x0F));
	relStatus = inNumVersion.stage;
	prerelNum = inNumVersion.nonRelRev;
	prerelNum = static_cast<UInt8>((((prerelNum & 0xF0 ) >> 4) * 10) + (prerelNum & 0x0F));

	numString += static_cast<SInt32>(ver1);
	numString += char_Period;
	numString += static_cast<SInt32>(ver2);
	numString += char_Period;
	numString += static_cast<SInt32>(ver3);

	switch (relStatus) {
		case developStage:
			numString += 'd';
			break;

		case alphaStage:
			numString += 'a';
			break;

		case betaStage:
			numString += 'b';
			break;
	}

	if (relStatus != finalStage) {
		numString += static_cast<SInt32>(prerelNum);
	}

	return WritePString(numString);
}


// ---------------------------------------------------------------------------
//	¥ WriteDouble									[public]
// ---------------------------------------------------------------------------
//	Writes a double_t according to the format specifiers given (details on
//	these can be found in fp.h). This is only available under Mac OS

#if TARGET_OS_MAC

SInt32
LDebugStream::WriteDouble(
	double		inDouble,
	char		inStyle,
	SInt16		inDigits)
{
	LStr255		str;
	
	str.Assign(inDouble, inStyle, inDigits);

	return WritePString(str);
}

SInt32
LDebugStream::WriteDouble(
	long double	inDouble,
	char		inStyle,
	SInt16		inDigits)
{
	LStr255		str;
	
	str.Assign(inDouble, inStyle, inDigits);

	return WritePString(str);
}

#endif


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DumpRaw										[public]
// ---------------------------------------------------------------------------
//	Given a pointer and it's size, dump the raw data. The output is very
//	similar to layouts like MacsBug's 'dm' and ZoneRanger's Block window.
//	The first column displays (in hex) the offset from the beginning of the
//	block of the bytes displayed on a given line. The second column displays
//	(in hex) the 16 bytes of data for that line. The third column displays the
//	ASCII equivalent of that line.
//
//	Due to the potential length of the dump, it is recommended to avoid
//	directing this to a debugger or alert (use file or console). Furthermore,
//	this method may move memory.
//
//	Thanx to meeroh Jurisic and Joshua Golub.

void
LDebugStream::DumpRaw(
	const void*		inPtr,
	Size			inSize,
	const char*		inFile,
	SInt32			inLine)
{
	UCursor::SetWatch();			// This may take a bit (if inSize is big)

		// Add header for dump
	*this << StringLiteral_("Results of Raw dump.");

	if (inFile != nil) {
		*this	<< StringLiteral_(" FILE: \"")
				<< inFile
				<< '"';
	}
	if (inLine != 0) {
		*this	<< StringLiteral_(" LINE: ")
				<< inLine;
	}

	*this << '\r';

		// Do the dump
	if (inPtr != nil) {

			// Informational header
		*this	<< StringLiteral_("\tDumping ")
				<< inSize
				<< StringLiteral_(" bytes of memory starting at address: ");
		Str255	addrAsString;
		BaseNumToString(reinterpret_cast<SInt32>(inPtr), addrAsString, numberBase_Hex, false);
		*this	<< addrAsString
				<< '\r';

			// Layout header
		*this << StringLiteral_("\tOFFSET     | CONTENTS (HEX)                          | CONTENTS (ASCII)\r");
		*this << StringLiteral_("\t-----------------------------------------------------------------------\r");

			// Dump the raw memory
		char	theString[256];
		const UInt8*	theFirstByte	= static_cast<const UInt8*>(inPtr);
		const UInt8*	theLastByte		= theFirstByte + inSize;
		const UInt8*	theCurrentByte;
		UInt32	theCurrentOffset	= 0;
		const	UInt32	kChunkSize = 16;

		for (	theCurrentByte = theFirstByte;
				theCurrentByte < theLastByte;
				theCurrentByte += kChunkSize, theCurrentOffset += kChunkSize) {

				// Offset column
			PP_CSTD::sprintf(theString, "\t0x%08lX | ", theCurrentOffset);

				// Hex column. We only print if within our address range
			char	buf[256];
			for (SInt16 index = 0; index < kChunkSize; index++) {
				if (((theCurrentByte + index) >= theFirstByte) &&
					((theCurrentByte + index) < theLastByte)) {

						// Within address range. Print the value.
					PP_CSTD::sprintf(buf, "%2.2hX", static_cast<SInt16>(theCurrentByte[index]));
				} else {
						// Outside the range. Print spaces for formatting purposes
					PP_CSTD::sprintf(buf, "  ");
				}

					// Add a trailing space for formatting (after ever other index)
				if (((index/2)*2) != index) {
					PP_CSTD::strcat(buf, " ");
				}

					// Add work buffer to main string
				PP_CSTD::strcat(theString, buf);
			}

				// ASCII column
			PP_CSTD::strcat(theString, "| ");

				// Again, only print if within our address range
			for (SInt16 index = 0; index < kChunkSize; index++) {

				char theChar = '.';		// If within address range but not a printable
										//	character, a period works as a placeholder.

				if (((theCurrentByte + index) >= theFirstByte) &&
					((theCurrentByte + index) < theLastByte)) {

						// Within address range. Is it a printable character?
					if (UKeyFilters::IsPrintingChar(theCurrentByte[index])) {
						theChar = static_cast<char>(theCurrentByte[index]);
					}
				} else {
					theChar = ' ';	// Outside of range. Add space for formatting purposes.
				}

				buf[index] = theChar;
			}

			buf[kChunkSize] = '\0';
			PP_CSTD::strcat(theString, buf);

				// Add the string/line to our internal buffer
			*this << theString << '\r';
		}

	} else {
		*this << StringLiteral_("\tinPtr is nil.\r");
	}

	*this << StringLiteral_("End Raw dump.");
	if (inFile != nil) {
		*this	<< StringLiteral_(" FILE: \"")
				<< inFile
				<< '"';
	}
	if (inLine != 0) {
		*this	<< StringLiteral_(" LINE: ")
				<< inLine;
	}

	*this << StringLiteral_("\r\r");
}


// ---------------------------------------------------------------------------
//	¥ DumpHandle									[public]
// ---------------------------------------------------------------------------
//	Whereas WriteHandle adds the Handle's data to the stream, DumpHandle
//	obtains information about the Handle and dumps that to the stream.
//	This information includes: address of Handle; address of master pointer;
//	size of data the Handle points to; if locked, purgable, and a resource.
//	If a resource, how the ResAttrs flags are set; the ResType, ResIDT, and
//	ResName.
//
//	Due to the potential length of this dump, it's recommended to avoid
//	directing this to a debugger or alert (use file or console).
//
//	Thanx to meeroh Jurisic.

void
LDebugStream::DumpHandle(
	const Handle	inHandle,
	bool			inDumpData,
	const char*		inFile,
	SInt32			inLine)
{
	UCursor::SetWatch();			// This may take a bit (if inHandle is big)

	*this << StringLiteral_("Results of Handle dump.");

	if (inFile != nil) {
		*this	<< StringLiteral_(" FILE: \"")
				<< inFile
				<< '"';
	}
	if (inLine != 0) {
		*this	<< StringLiteral_(" LINE: ")
				<< inLine;
	}

	*this << '\r';

	if (inHandle != nil) {

			// Handle address
		*this << StringLiteral_("\tAddress: ");
		Str255	addrAsString;
		BaseNumToString(reinterpret_cast<SInt32>(inHandle), addrAsString, numberBase_Hex, false);
		*this << addrAsString;
		*this << StringLiteral_(" | ");

			// Master pointer address
		*this << StringLiteral_("Master pointer: ");
		if (*inHandle) {
			BaseNumToString(reinterpret_cast<SInt32>(*inHandle), addrAsString, numberBase_Hex, false);
			*this << addrAsString;
		} else {
			*this << StringLiteral_("nil\r\r");
			return;		// No need to go any further
		}
		*this << StringLiteral_(" | ");

			// Size
		*this << StringLiteral_("Handle data size: ");
		Size hSize = ::GetHandleSize(inHandle);
		OSErr err = ::MemError();
		if (err == noErr) {
			*this << hSize;
		} else {
			*this << StringLiteral_("ERROR - ");
			*this << err;
		}
		*this << StringLiteral_(" | ");

			// State flags
		*this << StringLiteral_("State: ");
		SInt8 state = ::HGetState(inHandle);
		err = ::MemError();
		bool isResource = false;
		if (err == noErr) {

			char	flag;

				// Locked? L == Locked, U == Unlocked
			(state & (1 << 7)) ? (flag = 'L') : (flag = 'U');
			*this << flag;

				// Purgeable? P == Purgeable, N == Not purgeable
			(state & (1 << 6)) ? (flag = 'P') : (flag = 'N');
			*this << flag;

				// Resource? R == Resource, M == Memory
			(state & (1 << 5)) ? (flag = 'R', isResource = true) : (flag = 'M');
			*this << flag;

		} else {
			*this << StringLiteral_("ERROR - ");
			*this << err;
		}
		*this << "\r";

		if (isResource) {
			*this << StringLiteral_("\tResource Info - ");

			ResType		theType;
			ResIDT		theID;
			Str255		theName;
			::GetResInfo(inHandle, &theID, &theType, theName);
			err = ::ResError();
			if (err == noErr) {

					// ResType
				*this << StringLiteral_("Type: '");
				*this << (StringPtr)LStr255(theType);	// WriteFourCharCode includes extra text so
				*this << '\'';							//	we'll do this ourselves.

					// ResIDT
				*this << StringLiteral_(" ID: ");
				*this << theID;

					// ResName
				*this << StringLiteral_(" Name: ");
				*this << theName;

				*this << '\r';

					// ResAttrs
				*this << StringLiteral_("\tResource Attributes - ");
				SInt16 resAttrs = ::GetResAttrs(inHandle);
				err = ::ResError();
				if (err == noErr) {
					*this << StringLiteral_("SysRef: ");
					*this << ((resAttrs & (1 << resSysRefBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" SysHeap: ");
					*this << ((resAttrs & (1 << resSysHeapBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" Purgeable: ");
					*this << ((resAttrs & (1 << resPurgeableBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" Locked: ");
					*this << ((resAttrs & (1 << resLockedBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" Protected: ");
					*this << ((resAttrs & (1 << resProtectedBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" Preload: ");
					*this << ((resAttrs & (1 << resPreloadBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));
					*this << StringLiteral_(" Changed: ");
					*this << ((resAttrs & (1 << resChangedBit)) ? StringLiteral_("ON") : StringLiteral_("OFF"));

				} else {
					*this << StringLiteral_("ERROR - ");
					*this << err;
				}
			} else {
				*this << StringLiteral_("ERROR - ");
				*this << err;
			}

			*this << '\r';
		}

		if (inDumpData) {
			StHandleLocker	lock(inHandle);
			DumpRaw(*inHandle, ::GetHandleSize(inHandle), inFile, inLine);
		}

	} else {
		*this << StringLiteral_("\tHandle is nil.\r");
	}

	*this << StringLiteral_("End Handle dump.");
	if (inFile != nil) {
		*this	<< StringLiteral_(" FILE: \"")
				<< inFile
				<< '"';
	}
	if (inLine != 0) {
		*this	<< StringLiteral_(" LINE: ")
				<< inLine;
	}

	*this << StringLiteral_("\r\r");
}


// ---------------------------------------------------------------------------
//	¥ÊDumpPtr										[public]
// ---------------------------------------------------------------------------
//	Dump information about a Mac OS Ptr (not a malloc/new pointer).

void
LDebugStream::DumpPtr(
	const Ptr		inPtr,
	const char*		inFile,
	SInt32			inLine)
{
	UCursor::SetWatch();			// This may take a bit (if inPtr is big)

	Size ptrSize = ::GetPtrSize(inPtr);
	ThrowIfMemError_();

	DumpRaw(inPtr, ptrSize, inFile, inLine);
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ BaseNumToString								[static, public]
// ---------------------------------------------------------------------------
//	Our own version of NumToString (thanx to Lloyd Chambers) that takes
//	the number base into account. Base can be between 2 and 16 (provides then
//	for binary, octal, decimal, and hex) and provides special formatting
//	for some of those bases (e.g. hex has an "0x" prepended to it). This
//	also does not move memory (at least in non-debug mode; those asserts
//	might cause memory to be moved if they trigger).
//
//	You are responsible for allocating the space for outString. Ensure it
//	is large enough to hold the largest possible returned value (this method
//	performs no such checking).

void
LDebugStream::BaseNumToString(
	SInt32		inNumber,
	StringPtr	outString,
	UInt16		inBase,
	bool		inIsSigned)
{
		// Ensure we have some sort of a buffer
	Assert_(outString != nil);

		// Base must be between 2 and 16
	Assert_((inBase >= numberBase_Base2) && (inBase <= numberBase_Base16));

		// Only base 10 can be signed
	if (inIsSigned && (inBase != numberBase_Base10)) {
		SignalStringLiteral_("Only base-10 can be signed");
		inIsSigned = false;
	}

	if (inNumber == 0) {
		outString[0] = 1;
		outString[1] = '0';
	} else {

			// Check if it is actually a negative number or not.
		bool isNegative = false;

		if ((inIsSigned) && (inNumber < 0) && (inBase == numberBase_Decimal)) {
			isNegative	= true;
			inNumber	= -inNumber;
		}

		UInt32	unsignedNumber = static_cast<UInt32>(inNumber);

		UInt8	tempBuf[64];	// Not a Pascal string
		UInt8	*outPtr = &tempBuf[0];

			// Add the digits in reverse order
		while (unsignedNumber > 0) {
			static unsigned char	sDigits[] = "0123456789ABCDEF";

			UInt32	tempNum		= unsignedNumber / inBase;
			UInt8	digitVal	= static_cast<UInt8>(unsignedNumber - (tempNum * inBase));

			*outPtr++ = sDigits[digitVal];

			unsignedNumber = tempNum;
		}

		if (isNegative) {
			*outPtr++ = char_Dash;
		}

		if (inBase == numberBase_Hex) {
				// Hex. Add a prefix of "0x".
			*outPtr++ = 'x';
			*outPtr++ = '0';
		}

		if (inBase == numberBase_Octal) {
				// Octal. Add prefix of "\0"
			*outPtr++ = '0';
			*outPtr++ = '\\';
		}

			// Flip flop the text
		outString[0] = static_cast<UInt8>(outPtr - &tempBuf[0]);

		for (UInt8 index = 1; index <= outString[0]; ++index) {
			outString[index] = *--outPtr;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LLBaseNumToString								[static, public]
// ---------------------------------------------------------------------------
//	The same as BaseNumToString, but functions with integers of type
//	"long long" (64-bit integers). The function is only provided (and why
//	it's also provided seperately) if "long long" support is enabled in
//	the compiler (see your compiler manual for more information).

#if TYPE_LONGLONG

void
LDebugStream::LLBaseNumToString(
	SInt64		inNumber,
	StringPtr	outString,
	UInt16		inBase,
	bool		inIsSigned)
{
		// Ensure we have some sort of a buffer
	Assert_(outString != nil);

		// Base must be between 2 and 16
	Assert_((inBase >= numberBase_Base2) && (inBase <= numberBase_Base16));

		// Only base 10 can be signed
	if (inIsSigned && (inBase != numberBase_Base10)) {
		SignalStringLiteral_("Only base-10 can be signed");
		inIsSigned = false;
	}

	if (inNumber == 0) {
		outString[0] = 1;
		outString[1] = '0';
	} else {

			// Check if it is actually a negative number or not.
		bool isNegative = false;

		if ((inIsSigned) && (inNumber < 0) && (inBase == numberBase_Decimal)) {
			isNegative	= true;
			inNumber	= -inNumber;
		}

		UInt64	unsignedNumber = static_cast<unsigned long long>(inNumber);

		UInt8	tempBuf[64];	// not a Pascal string
		UInt8	*outPtr = &tempBuf[0];

			// Add the digits in reverse order
		while (unsignedNumber > 0) {
			static unsigned char	sDigits[] = "0123456789ABCDEF";

			UInt64	tempNum		= unsignedNumber / inBase;
			UInt8	digitVal	= static_cast<UInt8>(unsignedNumber - (tempNum * inBase));

			*outPtr++ = sDigits[digitVal];

			unsignedNumber = tempNum;
		}

		if (isNegative) {
			*outPtr++ = char_Dash;
		}

		if (inBase == numberBase_Hex) {
				// Hex, add some special formatting
			*outPtr++ = 'x';
			*outPtr++ = '0';
		}

		if (inBase == numberBase_Octal) {
				// Octal. Add prefix of "\0"
			*outPtr++ = '0';
			*outPtr++ = '\\';
		}

			// Flip flop the text
		outString[0] = static_cast<UInt8>(outPtr - &tempBuf[0]);

		for (UInt8 index = 1; index <= outString[0]; ++index) {
			outString[index] = *--outPtr;
		}
	}
}

#endif	// TYPE_LONGLONG

PP_End_Namespace_PowerPlant
