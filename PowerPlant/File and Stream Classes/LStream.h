// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStream.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for reading/writing an ordered sequence of bytes

#ifndef _H_LStream
#define _H_LStream
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

enum EStreamFrom {
	streamFrom_Start = 1,
	streamFrom_End,
	streamFrom_Marker
};

// ---------------------------------------------------------------------------

class	LStream {
public:
					LStream();

					LStream( const LStream& inOriginal );

	LStream&		operator = ( const LStream& inOriginal );

	virtual			~LStream();

	virtual void	SetMarker(
							SInt32			inOffset,
							EStreamFrom		inFromWhere);

	virtual SInt32	GetMarker() const;

	virtual void	SetLength( SInt32 inLength );

	virtual SInt32	GetLength() const;

	bool			AtEnd() const
						{
							return GetMarker() >= GetLength();
						}

						// Write Operations

	virtual ExceptionCode	PutBytes(
								const void		*inBuffer,
								SInt32			&ioByteCount);

	SInt32			WriteData(
							const void		*inBuffer,
							SInt32			inByteCount)
						{
							PutBytes(inBuffer, inByteCount);
							return inByteCount;
						}

	void			WriteBlock(
							const void		*inBuffer,
							SInt32			inByteCount);

	LStream&		operator << ( ConstStringPtr inString)	// Pascal string
						{
							WritePString(inString);
							return (*this);
						}

	LStream&		operator << (const char* inString)
						{
							WriteCString(inString);
							return (*this);
						}

	LStream&		operator << (const Rect	&inRect)
						{
							Rect rect;
							rect.top = CFSwapInt16HostToBig(inRect.top);
							rect.left = CFSwapInt16HostToBig(inRect.left);
							rect.right= CFSwapInt16HostToBig(inRect.right);
							rect.bottom = CFSwapInt16HostToBig(inRect.bottom);
							WriteBlock(&rect, sizeof(rect));
							return (*this);
						}

	LStream&		operator << (const Point &inPoint)
						{
							Point pt;
							pt.v = CFSwapInt16HostToBig(inPoint.v);
							pt.h = CFSwapInt16HostToBig(inPoint.h);
							WriteBlock(&pt, sizeof(pt));
							return (*this);
						}

	LStream&		operator << (Handle inHandle)
						{
							WriteHandle(inHandle);
							return (*this);
						}

	LStream&		operator << (SInt8 inNum)
						{
							WriteBlock(&inNum, sizeof(inNum));
							return (*this);
						}

	LStream&		operator << (UInt8 inNum)
						{
							WriteBlock(&inNum, sizeof(inNum));
							return (*this);
						}

	LStream&		operator << (char inChar)
						{
							WriteBlock(&inChar, sizeof(inChar));
							return (*this);
						}

	LStream&		operator << (SInt16 inNum)
						{
							SInt16 n;
							n = CFSwapInt16HostToBig(inNum);
							WriteBlock(&n, sizeof(n));
							return (*this);
						}

	LStream&		operator << (UInt16 inNum)
						{
							UInt16 n;
							n = CFSwapInt16HostToBig(inNum);
							WriteBlock(&n, sizeof(n));
							return (*this);
						}

	LStream&		operator << (SInt32 inNum)
						{
							SInt32 n;
							n = CFSwapInt32HostToBig(inNum);
							WriteBlock(&n, sizeof(n));
							return (*this);
						}

	LStream&		operator << (UInt32 inNum)
						{
							UInt32 n;
							n = CFSwapInt32HostToBig(inNum);
							WriteBlock(&n, sizeof(n));
							return (*this);
						}

	LStream&		operator << (float inNum)
						{
							CFSwappedFloat32 swappedFloat;
							swappedFloat = CFConvertFloat32HostToSwapped(inNum);
							WriteBlock(&swappedFloat, sizeof(swappedFloat));
							return (*this);
						}

	LStream&		operator << (double inNum);

#ifndef __GNUC__
	LStream&		operator << (long double inNum)
						{
							(*this) << (double) inNum;
							return (*this);
						}
						
	LStream&		operator << (short double inNum)
						{
							(*this) << (double) inNum;
							return (*this);
						}
#endif

	LStream&		operator << (bool inBool)
						{
							// The 'bool' type changes depending which compiler and compiler settings you use.
							// The code below assumes that 'bool' is 32bit, this assert checks.
							Assert_(sizeof(inBool) == 4);
							
							UInt32 boolValue;
							boolValue = CFSwapInt32HostToBig(inBool);
							WriteBlock(&boolValue, sizeof(boolValue));
							return (*this);
						}


						// Read Operations

	virtual ExceptionCode	GetBytes(
								void			*outBuffer,
								SInt32			&ioByteCount);
	virtual SInt32	ReadData(
							void			*outBuffer,
							SInt32			inByteCount)
						{
							GetBytes(outBuffer, inByteCount);
							return inByteCount;
						}

	void			ReadBlock(
							void			*outBuffer,
							SInt32			inByteCount);

	SInt32			PeekData(
							void			*outBuffer,
							SInt32			inByteCount);

	LStream&		operator >> (Str255 outString)
						{
							ReadPString(outString);
							return (*this);
						}

	LStream&		operator >> (char* outString)
						{
							ReadCString(outString);
							return (*this);
						}

	LStream&		operator >> (Rect &outRect)
						{
							ReadBlock(&outRect, sizeof(outRect));
							outRect.top = CFSwapInt16BigToHost(outRect.top);
							outRect.left = CFSwapInt16BigToHost(outRect.left);
							outRect.right= CFSwapInt16BigToHost(outRect.right);
							outRect.bottom = CFSwapInt16BigToHost(outRect.bottom);
							return (*this);
						}

	LStream&		operator >> (Point &outPoint)
						{
							ReadBlock(&outPoint, sizeof(outPoint));
							outPoint.v = CFSwapInt16BigToHost(outPoint.v);
							outPoint.h = CFSwapInt16BigToHost(outPoint.h);
							return (*this);
						}

	LStream&		operator >> (Handle &outHandle)
						{
							ReadHandle(outHandle);
							return (*this);
						}

	LStream&		operator >> (SInt8 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							return (*this);
						}

	LStream&		operator >> (UInt8 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							return (*this);
						}

	LStream&		operator >> (char &outChar)
						{
							ReadBlock(&outChar, sizeof(outChar));
							return (*this);
						}

	LStream&		operator >> (SInt16 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							outNum = CFSwapInt16BigToHost(outNum);
							return (*this);
						}

	LStream&		operator >> (UInt16 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							outNum = CFSwapInt16BigToHost(outNum);
							return (*this);
						}

	LStream&		operator >> (SInt32 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							outNum = CFSwapInt32BigToHost(outNum);
							return (*this);
						}

	LStream&		operator >> (UInt32 &outNum)
						{
							ReadBlock(&outNum, sizeof(outNum));
							outNum = CFSwapInt32BigToHost(outNum);
							return (*this);
						}

	LStream&		operator >> (float &outNum)
						{
							CFSwappedFloat32 swappedFloat;
							ReadBlock(&swappedFloat, sizeof(swappedFloat));
							outNum = CFConvertFloat32SwappedToHost(swappedFloat);
							return (*this);
						}

	LStream&		operator >> (double &outNum);

#ifndef __GNUC__
	LStream&		operator >> (long double &outNum)
						{
							double	num;
							(*this) >> num;
							outNum = num;
							return (*this);
						}
						
	LStream&		operator >> (short double &outNum)
						{
							double num;
							(*this) >> num;
							outNum = (short double) num;
							return (*this);
						}
#endif

	LStream&		operator >> (bool &outBool)
						{
							// The 'bool' type changes depending which compiler and compiler settings you use.
							// The code below assumes that 'bool' is 32bit, this assert checks.
							Assert_(sizeof(outBool) == 4);
							
							UInt32 boolValue;
							ReadBlock(&boolValue, sizeof(boolValue));
							outBool = CFSwapInt32BigToHost(boolValue);
							return (*this);
						}


		// Data-specific read/write functions
		//   There is an equivalent Shift operator for each one
		//	 except WritePtr/ReadPtr (since Ptr is really a char*,
		//	 which is the same as a C string).

	SInt32			WritePtr( Ptr inPtr );
	
	SInt32			ReadPtr( Ptr& outPtr );

	SInt32			WriteHandle( Handle inHandle );
	
	SInt32			ReadHandle( Handle& outHandle );

	SInt32			WritePString( ConstStringPtr inString );
	
	SInt32			ReadPString( Str255 outString );

	SInt32			WriteCString( const char* inString );
	SInt32			ReadCString( char* outString );

protected:
	SInt32			mMarker;
	SInt32			mLength;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
