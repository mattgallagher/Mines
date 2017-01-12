// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDebugStream.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LDebugStream
#define _H_LDebugStream
#pragma once

#include <PP_Debug.h>
#include <LStream.h>	// needed for EStreamFrom
#include <LString.h>

#include <ConditionalMacros.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

	// Allow flushing to a console via std::cout? This is provided as an
	// option so you aren't required to use an ANSI C/C++ library if you
	// do not wish to. No effort is made to ensure there is a console to
	// flush to, nor that this console will work adequately within
	// PowerPlant (consider using SIOUX and LSIOUXAttachment, or some
	// similar 3rd party replacement).

#ifndef PP_Debug_Allow_DebugStream_Flush_To_Console
	#define	PP_Debug_Allow_DebugStream_Flush_To_Console				0	// OFF by default
#endif


PP_Begin_Namespace_PowerPlant

enum EFlushLocation {
	flushLocation_File					=	1,
	flushLocation_Debugger				=	2,

#ifdef Debug_Throw
	flushLocation_DebugThrow			=	4,
#endif

#ifdef Debug_Signal
	flushLocation_DebugSignal			=	5,
#endif

#if PP_Debug_Allow_DebugStream_Flush_To_Console
	flushLocation_Console				=	6,
#endif

	flushLocation_Default				=	0
};

	// Used by BaseNumToString
enum ENumberBase {
	numberBase_Binary					=	2,
	numberBase_Base2					=	numberBase_Binary,

	numberBase_Octal					=	8,
	numberBase_Base8					=	numberBase_Octal,

	numberBase_Decimal					=	10,
	numberBase_Base10					=	numberBase_Decimal,

	numberBase_Hexidecimal				=	16,
	numberBase_Hex						=	numberBase_Hexidecimal,
	numberBase_Base16					=	numberBase_Hexidecimal
};


class LDebugStream {
public:

									LDebugStream();
									LDebugStream(
											EFlushLocation	inFlushLocation,
											bool			inAutoFlush,
											bool			inAppendToFile = true);

	virtual							~LDebugStream();

					// Handle operations

			void					SetDataHandle(
											Handle			inHandle);
			Handle					GetDataHandle() const
										{
											return mDataH;
										}
			Handle					DetachDataHandle();

					// Misc file operations

	virtual	void					SetFilename(
											ConstStringPtr	inName = nil);
	virtual	void					TimeStamp(
											LStr255&		outStamp);
	virtual	Handle					GetHeader();

			void					SetAppendToFile(
											bool			inAppend)
										{
											// true, append to existing file
											// false, overwrite existing file
											mAppendToFile = inAppend;
										}
			bool					GetAppendToFile() const
										{
											return mAppendToFile;
										}

					// Flush operations
	virtual	void					Flush(	EFlushLocation	inFlushLocation		= flushLocation_Default,
											bool			inDisposeAfterFlush	= true);

			bool					GetAutoFlush() const
										{
											return mAutoFlush;
										}
			void					SetAutoFlush( bool inAutoFlush)
										{
											mAutoFlush = inAutoFlush;
										}

			EFlushLocation			GetFlushLocation() const
										{
											return mFlushLocation;
										}
			void					SetFlushLocation(
											EFlushLocation	inFlushLocation)
										{
											mFlushLocation = inFlushLocation;
										}

					// marker/length
	virtual void					SetMarker(
											SInt32			inOffset,
											EStreamFrom		inFromWhere);
	virtual SInt32					GetMarker() const;

	virtual void					SetLength(
											SInt32			inLength);
	virtual SInt32					GetLength() const;

			Boolean					AtEnd() const
										{
											return GetMarker() >= GetLength();
										}

					// Write Operations
	virtual ExceptionCode			PutBytes(
											const void*		inBuffer,
											SInt32&			ioByteCount);

			SInt32					WriteData(
											const void*		inBuffer,
											SInt32			inByteCount)
										{
											PutBytes(inBuffer, inByteCount);
											return inByteCount;
										}

			void					WriteBlock(
											const void*		inBuffer,
											SInt32			inByteCount);

			LDebugStream&			operator << (ConstStringPtr inString)	// Pascal string
										{
											WritePString(inString);
											return (*this);
										}

			LDebugStream&			operator << (const char* inString)
										{
											WriteCString(inString);
											return (*this);
										}

			LDebugStream&			operator << (const Rect& inRect)
										{
											WriteRect(inRect);
											return (*this);
										}

			LDebugStream&			operator << (const Point& inPoint)
										{
											WritePoint(inPoint);
											return (*this);
										}

			LDebugStream&			operator << (const RGBColor& inColor)
										{
											WriteRGBColor(inColor);
											return (*this);
										}

#if PP_Target_Classic					// Regions are opaque in Carbon

			LDebugStream&			operator << (const MacRegion& inRgn)
										{
											WriteRegion(inRgn);
											return (*this);
										}
#endif


			LDebugStream&			operator << (const RgnHandle inRgnH)
										{
											WriteRegion(inRgnH);
											return (*this);
										}

			LDebugStream&			operator << (const NumVersion& inVers)
										{
											WriteNumVersion(inVers);
											return (*this);
										}

			LDebugStream&			operator << (Handle inHandle)
										{
											WriteHandle(inHandle);
											return (*this);
										}

						// The C++ standard treats a plain "char", a "signed char" and an
						// "unsigned char" as three distinct types (¤3.9.1 paragraph 1).
						// Be careful of how you use chars, characters, and character
						// literals with regards to these operator << overloads.
						//
						// A character literal such as '\r' is treated as type char (¤2.13.2,
						// paragraph 1). Consequently "theDebugStream << '\r'" will call
						// the char overload, be treated as a character, and call WriteBlock()
						// (and in this case actually insert a carriage return).
						//
						// A character constant such as char_Return (PP_KeyCodes.h) is
						// strongly typed as a UInt8. Consequently "theDebugStream << char_Return"
						// will call the UInt8 overload, be treated as an unsigned integter,
						// and WriteInteger() will be called (and in this case, insert text
						// of "13").

			LDebugStream&			operator << (SInt8 inNum)
										{
											WriteInteger(inNum);
											return (*this);
										}

			LDebugStream&			operator << (UInt8 inNum)
										{
											WriteInteger(inNum);
											return (*this);
										}

			LDebugStream&			operator << (char inChar)
										{
											WriteBlock(&inChar, sizeof(inChar));
											return (*this);
										}

			LDebugStream&			operator << (SInt16 inNum)
										{
											WriteInteger(inNum);
											return (*this);
										}

			LDebugStream&			operator << (UInt16 inNum)
										{
											WriteInteger(inNum);
											return (*this);
										}

			LDebugStream&			operator << (SInt32 inNum)
										{
											WriteInteger(inNum);
											return (*this);
										}

			LDebugStream&			operator << (FourCharCode inNum)
										{
											// a FourCharCode is an unsigned long, so be
											// mindful when using operator << with a
											// UInt32.

											WriteFourCharCode(inNum);
											return (*this);
										}

#if TARGET_OS_MAC

			LDebugStream&			operator << (double inNum)
										{
											WriteDouble(inNum);
											return (*this);
										}

			LDebugStream&			operator << (long double inNum)
										{
											WriteDouble(inNum);
											return (*this);
										}

#endif // TARGET_OS_MAC

			LDebugStream&			operator << (bool inBool)
										{
											WriteBool(inBool);
											return (*this);
										}

#if TYPE_LONGLONG

			LDebugStream&			operator << (SInt64 inLongLong)
										{
											Str255 theStr;
											LDebugStream::LLBaseNumToString(
															inLongLong,
															theStr,
															10,
															true );
											WritePString(theStr);
											return (*this);
										}
			LDebugStream&			operator << (UInt64 inULongLong)
										{
											Str255 theString;
											LDebugStream::LLBaseNumToString(
															static_cast<SInt64>(inULongLong),
															theString,
															10,
															false);
											WritePString(theString);
											return (*this);
										}
#endif	// TYPE_LONGLONG


			// Data-specific read/write functions
			//   There is an equivalent Shift operator for each one
			//	 except: WritePtr/ReadPtr (since Ptr is really a char*,
			//	 which is the same as a C string); FourCharCode (UInt32)
			//	 and Fixed (SInt32)

			SInt32			WritePtr(			const Ptr			inPtr);
			SInt32			WriteHandle(		const Handle		inHandle);
			SInt32			WritePString(		ConstStringPtr		inString);
			SInt32			WriteCString(		const char*			inString);
			SInt32			WriteInteger(		SInt32				inInteger);
			SInt32			WriteBool(			bool				inBool);
			SInt32			WriteRect(			const Rect&			inRect);
			SInt32			WritePoint(			const Point&		inPoint);
			SInt32			WriteRGBColor(		const RGBColor&		inColor);

#if PP_Target_Classic					// Regions are opaque in Carbon
			SInt32			WriteRegion(		const MacRegion&	inRgn);
#endif

			SInt32			WriteRegion(		const RgnHandle		inRgnH);
			SInt32			WriteFourCharCode(	FourCharCode		inCode);
			SInt32			WriteFixed(			Fixed				inFixed);
			SInt32			WriteNumVersion(	const NumVersion&	inNumVersion);

#if TARGET_OS_MAC

		// These constants are defined in <fp.h>. Because of conflicts
		// between <fp.h> and <cmath>, we don't want to #include <fp.h>
		// in this header file. So, we define the constants here, if necessary.

	#ifndef FLOATDECIMAL
		#define      FLOATDECIMAL   ((char)(0))
	#endif
	
	#ifndef FIXEDDECIMAL
		#define      FIXEDDECIMAL   ((char)(1))
	#endif


			SInt32			WriteDouble(		double			inDouble,
												char			inStyle		= FIXEDDECIMAL,
												SInt16			inDigits	= 2);

			SInt32			WriteDouble(		long double		inDouble,
												char			inStyle		= FIXEDDECIMAL,
												SInt16			inDigits	= 2);
#endif

					// Data dumping

			void			DumpRaw(
									const void*		inPtr,
									Size			inSize,
									const char*		inFile = nil,
									SInt32			inLine = 0);

			void			DumpHandle(
									const Handle	inHandle,
									bool			inDumpData = false,
									const char*		inFile = nil,
									SInt32			inLine = 0);

			void			DumpPtr(
									const Ptr		inPtr,
									const char*		inFile = nil,
									SInt32			inLine = 0);

					// Misc

	static	void			BaseNumToString(	SInt32		inNumber,
												StringPtr	outString,
												UInt16		inBase			= 10,
												bool		inIsSigned		= true);

#if TYPE_LONGLONG
	static	void			LLBaseNumToString(	SInt64		inNumber,
												StringPtr	outString,
												UInt16		inBase			= 10,
												bool		inIsSigned		= true);
#endif // TYPE_LONGLONG

protected:

			SInt32					mMarker;
			SInt32					mLength;
			Handle					mDataH;
			bool					mAutoFlush;
			bool					mAppendToFile;
			EFlushLocation			mFlushLocation;
			FSSpec					mFileLocation;

private:
			void					Init();

									LDebugStream(const LDebugStream& inOriginal);
			LDebugStream&			operator=(const LDebugStream& inRhs);

};

// ---------------------------------------------------------------------------
//	Macros to facilitate the use of the Dump methods

#if PP_Debug

	#define DumpRaw_(ptr, size)																			\
		do {																							\
			try {																						\
				StDisableDebugThrow_();	StDisableDebugSignal_();										\
				PP_PowerPlant::LDebugStream	__dStream(PP_PowerPlant::flushLocation_File, false, true);	\
				__dStream.DumpRaw(ptr, size, __FILE__, __LINE__);										\
				__dStream.Flush();																		\
			} catch (...) { SignalStringLiteral_("DumpRaw_ failed"); }									\
		} while(false)

	#define DumpHandle_(h)																				\
		do {																							\
			try {																						\
				StDisableDebugThrow_();	StDisableDebugSignal_();										\
				PP_PowerPlant::LDebugStream	__dStream(PP_PowerPlant::flushLocation_File, false, true);	\
				__dStream.DumpHandle(h, true, __FILE__, __LINE__);										\
				__dStream.Flush();																		\
			} catch (...) { SignalStringLiteral_("DumpHandle_ failed"); }								\
		} while(false)

	#define DumpPtr_(p)																					\
		do {																							\
			try {																						\
				StDisableDebugThrow_();	StDisableDebugSignal_();										\
				PP_PowerPlant::LDebugStream	__dStream(PP_PowerPlant::flushLocation_File, false, true);	\
				__dStream.DumpPtr(p, __FILE__, __LINE__);												\
				__dStream.Flush();																		\
			} catch (...) { SignalStringLiteral_("DumpPtr_ failed"); }									\
		} while(false)

#else

	#define DumpRaw_(ptr, size)
	#define DumpHandle_(h)
	#define DumpPtr_(p)

#endif


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LDebugStream
