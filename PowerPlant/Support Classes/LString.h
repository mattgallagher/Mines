// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LString.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Classes for handling Pascal Strings
//
//	LString		- Abstract base class
//	LStr255		- Wrapper class for Str255
//	TString<T>	- Template class for any string type (array of unsigned char)

#ifndef _H_LString
#define _H_LString
#pragma once

#include <PP_Prefix.h>
#include <NumberFormatting.h>
#include <cstddef>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#ifdef Length
	#undef Length
#endif


class LStr255;						// Forward declaration


// ---------------------------------------------------------------------------
//	¥ LString
// ---------------------------------------------------------------------------
//	Abstract base class for a Pascal-style string

class LString {
public:
									// String Comparison Function
									//   left == right, returns 0
									//   left >  right, returns 1
									//   left <  right, returns -1
	typedef	SInt16	(*CompareFunc)(
							const void*		inLeft,
							const void*		inRight,
							UInt8			inLeftLength,
							UInt8			inRightLength);

	virtual			~LString() = 0;		// Pure virtual destructor enforces
										//  that LString is abstract

									// Accessors
	UInt8 			Length() const					{ return mStringPtr[0]; }
	SInt16			ShortLength() const				{ return (SInt16) mStringPtr[0]; }
	SInt32			LongLength() const				{ return (SInt32) mStringPtr[0]; }
	UInt16			GetMaxLength() const			{ return mMaxBytes; }
	Ptr				TextPtr()						{ return (Ptr) (mStringPtr + 1); }
	const Ptr		ConstTextPtr() const			{ return (Ptr) (mStringPtr + 1); }

									// Conversion Operatores
					operator ConstStringPtr() const	{ return mStringPtr; }
					operator StringPtr()			{ return mStringPtr; }

					operator SInt32() const;

					operator double() const;
					operator long double() const;

					operator FourCharCode() const;
					
	CompareFunc		GetCompareFunc() const			{ return mCompareFunc; }

	void			SetCompareFunc( CompareFunc inCompareFunc )
						{
							mCompareFunc = inCompareFunc;
						}

									// Accessing individual characters

	UInt8& 			operator [] ( PP_CSTD::ptrdiff_t inPosition )
						{
							return mStringPtr[inPosition];
						}

	const UInt8&	operator [] ( PP_CSTD::ptrdiff_t inPosition ) const
						{
							return mStringPtr[inPosition];
						}

									// Assignment operators

	LString&		operator = ( const LString& inString )
						{
							mCompareFunc = inString.mCompareFunc;
							return Assign(inString.ConstTextPtr(), inString.Length());
						}

	LString&		operator = ( ConstStringPtr inStringPtr )
						{
							return Assign(inStringPtr);
						}

	LString& 		operator = ( unsigned char inChar )
						{
							return Assign(inChar);
						}

	LString&		operator = ( char inChar )
						{
							return Assign(inChar);
						}

	LString& 		operator = ( const char* inCString )
						{
							return Assign(inCString);
						}

	LString&		operator = ( SInt32 inNumber )
						{
							return Assign(inNumber);
						}

	LString&		operator = ( SInt16 inNumber )
						{
							return Assign(inNumber);
						}

	LString&		operator = ( FourCharCode inCode )
						{
							return Assign(inCode);
						}

									// Assignment functions
	LString&		Assign(	const LString&	inString,
							UInt8			inStartPos = 1,
							UInt8			inCount = 255);

	LString&		Assign(	ConstStringPtr	inStringPtr);

	LString&		Assign(	unsigned char inChar );

	LString&		Assign(	char inChar )
						{
							return Assign((unsigned char) inChar);
						}

	LString&		Assign(	const char* inCString )
						{
							return Assign(inCString, CStringLength(inCString));
						}

	LString&		Assign(	const void*		inPtr,
							UInt8			inLength);

	LString&		Assign(	Handle inHandle );

	LString&		Assign(	ResIDT			inResID,
							SInt16			inIndex);

	LString&		Assign(	SInt32 inNumber );

	LString&		Assign( SInt16 inNumber )
						{
							return Assign((SInt32) inNumber);
						}

	LString&		Assign(	long double			inNumber,
							ConstStringPtr		inFormatString,
							FormatResultType*	outFormatResult = nil);

	LString&		Assign(	long double			inNumber,
							ConstStringPtr		inFormatString,
							ResIDT				inNumberPartsID,
							FormatResultType*	outFormatResult = nil);

	LString&		Assign(	long double				inNumber,
							const NumFormatString&	inNumFormat,
							const NumberParts&		inPartsTable,
							FormatResultType*		outFormatResult = nil);

#if TARGET_OS_MAC

	LString&		Assign(	double			inNumber,
							char			inStyle,
							SInt16			inDigits);
							
	LString&		Assign(	long double		inNumber,
							char			inStyle,
							SInt16			inDigits);
#endif

	LString&		Assign(	FourCharCode inCode );

									// Assign & Increment operators
									//   Appends to the end of a String

	LString&		operator += ( const LString& inString )
						{
							return Append(inString);
						}

	LString&		operator += ( ConstStringPtr inStringPtr )
						{
							return Append(inStringPtr);
						}

	LString&		operator += ( const char* inCString )
						{
							return Append(inCString, CStringLength(inCString));
						}

	LString&		operator += ( unsigned char inChar )
						{
							return Append(inChar);
						}

	LString&		operator += ( char inChar )
						{
							return Append(inChar);
						}

	LString&		operator += ( SInt32 inNumber )
						{
							return Append(inNumber);
						}

	LString&		operator += ( SInt16 inNumber )
						{
							return Append(inNumber);
						}

	LString&		Append( const LString& inString )
						{
							return Append(inString.ConstTextPtr(), inString.Length());
						}

	LString&		Append( ConstStringPtr inStringPtr )
						{
							return Append(inStringPtr + 1, inStringPtr[0]);
						}

	LString&		Append( const char* inCString )
						{
							return Append(inCString, CStringLength(inCString));
						}

	LString&		Append( unsigned char inChar );

	LString&		Append( char inChar )
						{
							return Append((unsigned char) inChar);
						}

	LString&		Append( SInt32 inNumber );

	LString&		Append( SInt16 inNumber )
						{
							return Append((SInt32) inNumber);
						}

	LString&		Append(	const void*		inPtr,
							UInt8			inLength);

									// Comparison, Object to Input
									//		Object == Input, returns 0
									//		Object >  Input, returns 1
									//		Object <  Input, returns -1
									
	SInt16			CompareTo( const LString& inString ) const
						{
							return (*mCompareFunc)(mStringPtr + 1, inString.ConstTextPtr(),
												   mStringPtr[0], inString.Length());
						}

	SInt16			CompareTo( ConstStringPtr inStringPtr ) const
						{
							return (*mCompareFunc)(mStringPtr + 1, inStringPtr + 1,
												   mStringPtr[0], inStringPtr[0]);
						}

	SInt16			CompareTo( unsigned char inChar ) const
						{
							return (*mCompareFunc)(mStringPtr + 1, &inChar,
												   mStringPtr[0], 1);
						}

	SInt16			CompareTo(
							const void*		inPtr,
							UInt8			inLength) const
						{
							return (*mCompareFunc)(mStringPtr + 1, inPtr,
												   mStringPtr[0], inLength);
						}

									// Find Substring
									//   Returns index of start of substring,
									//	 searching forward from inStartPos
	UInt8			Find(	const LString&	inString,
							UInt8			inStartPos = 1) const
						{
							return Find(inString.ConstTextPtr(), inString.Length(), inStartPos);
						}

	UInt8			Find(	ConstStringPtr	inStringPtr,
							UInt8			inStartPos = 1) const
						{
							return Find(inStringPtr + 1, inStringPtr[0], inStartPos);
						}

	UInt8			Find(	unsigned char	inChar,
							UInt8			inStartPos = 1) const
						{
							return Find(&inChar, 1, inStartPos);
						}

	UInt8			Find(	const void*		inPtr,
							UInt8			inLength,
							UInt8			inStartPos = 1) const;

									// Reverse Find Substring
									//   Returns index of start of substring,
									//   searching backward from inStartPos
	UInt8			ReverseFind(
							const LString&	inString,
							UInt8			inStartPos = 255) const
						{
							return ReverseFind(inString.ConstTextPtr(), inString.Length(), inStartPos);
						}

	UInt8			ReverseFind(
							ConstStringPtr	inStringPtr,
							UInt8			inStartPos = 255) const
						{
							return ReverseFind(inStringPtr + 1, inStringPtr[0], inStartPos);
						}

	UInt8			ReverseFind(
							unsigned char	inChar,
							UInt8			inStartPos = 255) const
						{
							return ReverseFind(&inChar, 1, inStartPos);
						}

	UInt8			ReverseFind(
							const void*		inPtr,
							UInt8			inLength,
							UInt8			inStartPos = 255) const;

									// Check if a string begins with a substring
	Boolean			BeginsWith( const LString& inString ) const
						{
							return BeginsWith(inString.ConstTextPtr(), inString.Length());
						}

	Boolean			BeginsWith( ConstStringPtr inStringPtr ) const
						{
							return BeginsWith(inStringPtr + 1, inStringPtr[0]);
						}

	Boolean			BeginsWith( unsigned char inChar ) const
						{
							return BeginsWith(&inChar, 1);
						}

	Boolean			BeginsWith(
							const void*		inPtr,
							UInt8			inLength) const;

									// Check if a string ends with a substring
	Boolean			EndsWith( const LString& inString ) const
						{
							return EndsWith(inString.ConstTextPtr(), inString.Length());
						}

	Boolean			EndsWith( ConstStringPtr inStringPtr ) const
						{
							return EndsWith(inStringPtr + 1, inStringPtr[0]);
						}

	Boolean			EndsWith( unsigned char inChar ) const
						{
							return EndsWith(&inChar, 1);
						}

	Boolean			EndsWith(
							const void*		inPtr,
							UInt8			inLength) const;

									// Find String within another String
									//    Returns index of start of String
	UInt8			FindWithin(
							const LString&	inString,
							UInt8			inStartPos = 1) const
						{
							return (UInt8) FindWithin(inString.ConstTextPtr(), inString.Length(), inStartPos);
						}

	UInt8			FindWithin(
							ConstStringPtr	inStringPtr,
							UInt8			inStartPos = 1) const
						{
							return (UInt8) FindWithin(inStringPtr + 1, inStringPtr[0], inStartPos);
						}

	UInt8			FindWithin(
							unsigned char	inChar,
							UInt8			inStartPos = 1) const
						{
							return (UInt8) FindWithin(&inChar, 1, inStartPos);
						}

	UInt32			FindWithin(
							const void*		inPtr,
							UInt32			inLength,
							UInt32			inStartPos = 1) const;

									// Reverse Find String within another String
									//    Returns index of start of String
	UInt8			ReverseFindWithin(
							const LString&	inString,
							UInt8			inStartPos = 255) const
						{
							return (UInt8) ReverseFindWithin(inString.ConstTextPtr(), inString.Length(), inStartPos);
						}

	UInt8			ReverseFindWithin(
							ConstStringPtr	inStringPtr,
							UInt8			inStartPos = 255) const
						{
							return (UInt8) ReverseFindWithin(inStringPtr + 1, inStringPtr[0], inStartPos);
						}

	UInt8			ReverseFindWithin(
							unsigned char	inChar,
							UInt8			inStartPos = 1) const
						{
							return (UInt8) ReverseFindWithin(&inChar, 1, inStartPos);
						}

	UInt32			ReverseFindWithin(
							const void*		inPtr,
							UInt32			inLength,
							UInt32			inStartPos = 0xFFFFFFFF) const;

									// Copy Substring
									//    Returns a LStr255 object for a
									//    substring of the String
	LStr255			operator()(
							UInt8			inStartPos,
							UInt8			inCount) const;

									// Inserting substrings staring at the
									//   specified position
	LString&		Insert(	const LString&	inString,
							UInt8			inAtIndex)
						{
							return Insert(inString.ConstTextPtr(), inString.Length(), inAtIndex);
						}

	LString&		Insert(	ConstStringPtr	inStringPtr,
							UInt8			inAtIndex)
						{
							return Insert(inStringPtr + 1, inStringPtr[0], inAtIndex);
						}

	LString&		Insert(	unsigned char	inChar,
							UInt8			inAtIndex)
						{
							return Insert(&inChar, 1, inAtIndex);
						}

	LString&		Insert(	const void*		inPtr,
							UInt8			inLength,
							UInt8			inAtIndex);

									// Removing substring starting at a given
									//   position with a given length
	LString&		Remove(	UInt8			inStartPos,
							UInt8			inCount);

									// Replacing one substring with another
	LString&		Replace(
							UInt8			inStartPos,
							UInt8			inCount,
							const LString&	inString)
						{
							return Replace(inStartPos, inCount, inString.ConstTextPtr(), inString.Length());
						}

	LString&		Replace(
							UInt8			inStartPos,
							UInt8			inCount,
							ConstStringPtr	inStringPtr)
						{
							return Replace(inStartPos, inCount, inStringPtr + 1, inStringPtr[0]);
						}

	LString&		Replace(
							UInt8			inStartPos,
							UInt8			inCount,
							unsigned char	inChar)
						{
							return Replace(inStartPos, inCount, &inChar, 1);
						}

	LString&		Replace(
							UInt8			inStartPos,
							UInt8			inCount,
							const void*		inPtr,
							UInt8			inLength);

									// String Comparison Functions

									// Toolbox CompareText() routine using the
									//   rules for the current Script
	static SInt16	ToolboxCompareText(
							const void*		inLeft,
							const void*		inRight,
							UInt8			inLeftLength,
							UInt8			inRightLength);

									// Simple byte-by-byte value comparison
	static SInt16	CompareBytes(
							const void*		inLeft,
							const void*		inRight,
							UInt8			inLeftLength,
							UInt8			inRightLength);

									// Case-insensitive comparison
	static SInt16	CompareIgnoringCase(
							const void*		inLeft,
							const void*		inRight,
							UInt8			inLeftLength,
							UInt8			inRightLength);


									// Public utility functions

									// C string length limited to 255 characters
	static UInt8		CStringLength( const char* inCString );

									// Copy a Pascal string
	static StringPtr	CopyPStr(
							ConstStringPtr	inSourceString,
							StringPtr		outDestString,
							SInt16			inDestSize = sizeof(Str255));

									// Convert C to Pascal string in place
	static StringPtr	CToPStr( char* ioStr );

									// Convert Pascal to C string in place
	static char*		PToCStr( StringPtr ioStr );

									// Append one Pascal string to another
	static StringPtr	AppendPStr(
							StringPtr		ioBaseString,
							ConstStringPtr	inAppendString,
							SInt16			inDestSize = sizeof(Str255));

									// Convert a four character code to a Pascal string
	static StringPtr	FourCharCodeToPStr(
							FourCharCode	inCode,
							StringPtr		outString);

									// Convert a Pascal string to a four character code
	static void			PStrToFourCharCode(
							ConstStringPtr	inString,
							FourCharCode	&outCode);

									// Pass back formatting info for floating point numbers
	static void			FetchFloatFormat(
							ConstStringPtr			inFormatString,
							NumFormatString&		outSysNumFormat,
							NumberParts&			outSysPartsTable);

	static void			FetchFloatFormat(
							ConstStringPtr			inFormatString,
							const NumberParts&		inPartsTable,
							NumFormatString&		outSysNumFormat,
							NumberParts&			outSysPartsTable);

	static void			GetSystemNumberParts( NumberParts& outSysPartsTable );

									// Convert a Pascal string to a long double number
	static long double	StringToLongDouble(
							ConstStringPtr			inString,
							ConstStringPtr			inFormatString = Str_Empty,
							FormatResultType*		outFormatResult = nil);

									// Convert a Pascal string to a long double number
	static long double	StringToLongDouble(
							ConstStringPtr			inString,
							const NumFormatString&	inNumFormat,
							const NumberParts&		inPartsTable,
							FormatResultType*		outFormatResult = nil);

protected:
									// Constructor
									//   Protected since LString is abstract
						LString(
								SInt16			inMaxBytes,
								StringPtr		inStringPtr);
								
						LString(
								SInt16			inMaxBytes,
								StringPtr		inStringPtr,
								CompareFunc		inCompareFunc);

									// Utility functions
	void				LoadFromStringPtr(
								ConstStringPtr	inStringPtr)
							{
								LoadFromPtr(inStringPtr + 1, inStringPtr[0]);
							}

	void				LoadFromPtr(
								const void*		inPtr,
								UInt8			inLength);

	void				LoadFromSTRListResource(
								ResIDT			inResID,
								SInt16			inIndex);

	void				LoadFromSTRResource(
								ResIDT			inResID);

protected:
	StringPtr		mStringPtr;		// Ptr to storage allocated by a subclass
	CompareFunc		mCompareFunc;	// String comparison function
	UInt16			mMaxBytes;		// Max size, including length byte
};


// ---------------------------------------------------------------------------
//	¥ Global String Addition Operators
// ---------------------------------------------------------------------------
//	Handles "adding", which means concatenating, String objects with:
//		(1) Another String object		leftString + rightString
//		(2) A pointer to a string		String + StringPtr, StringPtr + String
//		(3) A character					String + char, char + String
//
//	The result of String addition is a LStr255


LStr255	operator + (
				const LString&	inLeftString,
				const LString&	inRightString);
				
LStr255	operator + (
				const LString&	inLeftString,
				ConstStringPtr	inRightStringPtr);
				
LStr255	operator + (
				ConstStringPtr	inLeftStringPtr,
				const LString&	inRightString);
				
LStr255	operator + (
				const LString&	inLeftString,
				StringPtr		inRightStringPtr);
				
LStr255	operator + (
				StringPtr		inLeftStringPtr,
				const LString&	inRightString);
				
LStr255	operator + (
				const LString&	inLeftString,
				unsigned char	inRightChar);
				
LStr255	operator + (
				unsigned char	inLeftChar,
				const LString&	inRightString);



// ---------------------------------------------------------------------------
//	¥ Global String Comparison Operators
// ---------------------------------------------------------------------------
//	Comparison of String objects with:
//		(1) Another String object		leftString ? rightString
//		(2) A pointer to a string		String ? StringPtr, StringPtr ? String
//		(3) A character					String ? char, char ? String

// ------------------------------------	Equals
inline
Boolean operator == (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) == 0;
			}

inline
Boolean operator == (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) == 0;
			}

inline
Boolean operator == (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) == 0;
			}

inline
Boolean operator == (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) == 0;
			}

inline
Boolean operator == (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) == 0;
			}

// ------------------------------------	Not Equals
inline
Boolean operator != (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) != 0;
			}

inline
Boolean operator != (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) != 0;
			}

inline
Boolean operator != (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) != 0;
			}

inline
Boolean operator != (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) != 0;
			}

inline
Boolean operator != (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) != 0;
			}

// ------------------------------------	Greater Than
inline
Boolean operator > (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) > 0;
			}

inline
Boolean operator > (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) > 0;
			}

inline
Boolean operator > (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) < 0;
			}

inline
Boolean operator > (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) > 0;
			}

inline
Boolean operator > (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) < 0;
			}

// ------------------------------------	Less Than
inline
Boolean operator < (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) < 0;
			}

inline
Boolean operator < (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) < 0;
			}

inline
Boolean operator < (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) > 0;
			}

inline
Boolean operator < (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) < 0;
			}

inline
Boolean operator < (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) > 0;
			}

// ------------------------------------	Greater Than or Equals
inline
Boolean operator >= (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) >= 0;
			}

inline
Boolean operator >= (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) >= 0;
			}

inline
Boolean operator >= (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) <= 0;
			}

inline
Boolean operator >= (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) >= 0;
			}

inline
Boolean operator >= (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) <= 0;
			}

// ------------------------------------	Less Than or Equals
inline
Boolean operator <= (
				const LString&	inLhs,
				const LString&	inRhs)
			{
				return inLhs.CompareTo(inRhs) <= 0;
			}

inline
Boolean operator <= (
				const LString&	inLhs,
				ConstStringPtr	inRhs)
			{
				return inLhs.CompareTo(inRhs) <= 0;
			}

inline
Boolean operator <= (
				ConstStringPtr	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) >= 0;
			}

inline
Boolean operator <= (
				const LString&	inLhs,
				unsigned char	inRhs)
			{
				return inLhs.CompareTo(inRhs) <= 0;
			}

inline
Boolean operator <= (
				unsigned char	inLhs,
				const LString&	inRhs)
			{
				return inRhs.CompareTo(inLhs) >= 0;
			}

#pragma mark === LStr255 ===

// ---------------------------------------------------------------------------
//	¥ LStr255
// ---------------------------------------------------------------------------
//	A String with a maximum of 255 characters

class LStr255 : public LString {
public:
									// Constructors
					LStr255();

					LStr255( const LStr255& inOriginal );

					LStr255( const LString& inOriginal );

					LStr255( ConstStringPtr inStringPtr );

					LStr255( unsigned char inChar );

					LStr255( char inChar );

					LStr255( const char* inCString );

					LStr255(const void*		inPtr,
							UInt8			inLength);

					LStr255( Handle inHandle );

					LStr255(ResIDT			inResID,
							SInt16			inIndex);

					LStr255( SInt32 inNumber );

					LStr255 (SInt16 inNumber );

					LStr255(long double		inNumber,
							ConstStringPtr	inFormatString);

					LStr255(long double				inNumber,
							const NumFormatString&	inNumFormat,
							const NumberParts&		inPartsTable);

#if TARGET_OS_MAC
					LStr255(double			inNumber,
							char			inStyle,
							SInt16			inDigits);
							
					LStr255(long double		inNumber,
							char			inStyle,
							SInt16			inDigits);
#endif

					LStr255( FourCharCode inCode );

									// Assignment operators

	LStr255&		operator = ( const LString& inString )
						{
							LString::operator=(inString);
							return *this;
						}

	LStr255&		operator = ( ConstStringPtr inStringPtr )
						{
							LString::operator=(inStringPtr);
							return *this;
						}

	LStr255& 		operator = ( unsigned char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	LStr255& 		operator = ( char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	LStr255&		operator = ( const char* inCString )
						{
							LString::operator=(inCString);
							return *this;
						}

	LStr255&		operator = ( SInt32 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	LStr255&		operator = ( SInt16 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	LStr255&		operator = ( FourCharCode inCode )
						{
							LString::operator=(inCode);
							return *this;
						}

private:
	Str255			mString;
};

#pragma mark === TString ===

// ---------------------------------------------------------------------------
//	¥ TString
// ---------------------------------------------------------------------------
//	Template String class

template <class T> class TString : public LString {
public:
									// Constructors
					TString()
								: LString(sizeof(T), mString)
						{
							mString[0] = 0;
						}

					TString( const TString& inOriginal )
								: LString(sizeof(T), mString, inOriginal.mCompareFunc)
						{
							LoadFromStringPtr(inOriginal);
						}

					TString( const LString& inOriginal )
								: LString(sizeof(T), mString, inOriginal.GetCompareFunc())
						{
							LoadFromStringPtr(inOriginal);
						}

					TString( ConstStringPtr inStringPtr )
								: LString(sizeof(T), mString)
						{
							if (inStringPtr == nil) {
								mString[0] = 0;
							} else {
								LoadFromStringPtr(inStringPtr);
							}
						}

					TString( unsigned char inChar )
								: LString(sizeof(T), mString)
						{
							mString[0] = 1;
							mString[1] = inChar;
						}

					TString( char inChar )
								: LString(sizeof(T), mString)
						{
							mString[0] = 1;
							mString[1] = inChar;
						}

					TString( const char* inCString )
								: LString(sizeof(T), mString)
						{
							LoadFromPtr(inCString, CStringLength(inCString));
						}

					TString(
							const void*		inPtr,
							UInt8			inLength)
								: LString(sizeof(T), mString)
						{
							LoadFromPtr(inPtr, inLength);
						}

					TString( Handle inHandle )
								: LString(sizeof(T), mString)
						{
							Assign(inHandle);
						}

					TString(
							ResIDT			inResID,
							SInt16			inIndex)
								: LString(sizeof(T), mString)
						{
							Assign(inResID, inIndex);
						}

					TString( SInt32 inNumber )
								: LString(sizeof(T), mString)
						{
							Assign(inNumber);
						}

					TString( SInt16 inNumber )
								: LString(sizeof(T), mString)
						{
							Assign(inNumber);
						}

					TString(
							long double		inNumber,
							ConstStringPtr	inFormatString)
								: LString(sizeof(T), mString)
						{
							Assign(inNumber, inFormatString);
						}

					TString(
							long double				inNumber,
							const NumFormatString&	inNumFormat,
							const NumberParts&		inPartsTable)
								: LString(sizeof(T), mString)
						{
							Assign(inNumber, inNumFormat, inPartsTable);
						}

#if TARGET_OS_MAC
					TString(
							double			inNumber,
							char			inStyle,
							SInt16			inDigits)
								: LString(sizeof(T), mString)
						{
							Assign(inNumber, inStyle, inDigits);
						}

					TString(
							long double		inNumber,
							char			inStyle,
							SInt16			inDigits)
								: LString(sizeof(T), mString)
						{
							Assign(inNumber, inStyle, inDigits);
						}
#endif

					TString( FourCharCode inCode )
								: LString(sizeof(T), mString)
						{
							Assign(inCode);
						}

									// Assignment operators
	TString&		operator = ( const LString& inString )
						{
							LString::operator=(inString);
							return *this;
						}

	TString&		operator = ( ConstStringPtr inStringPtr )
						{
							LString::operator=(inStringPtr);
							return *this;
						}

	TString& 		operator = ( unsigned char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	TString& 		operator = ( char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	TString&		operator = ( const char* inCString )
						{
							LString::operator=(inCString);
							return *this;
						}

	TString&		operator = ( SInt32 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	TString&		operator = ( SInt16 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	TString&		operator = ( FourCharCode inCode )
						{
							LString::operator=(inCode);
							return *this;
						}

private:
	T				mString;
};


#pragma mark === LStringRef ===

// ---------------------------------------------------------------------------
//	¥ LStringRef
// ---------------------------------------------------------------------------
//	String class which uses external storage
//
//	Client supplies a pointer to storage and a maximum length

class	LStringRef : public LString {
public:
						LStringRef(
								SInt16		inMaxBytes,
								StringPtr	inStringPtr);

						LStringRef( const LStringRef& inStringRef );

	LStringRef&			operator = ( const LStringRef& inStringRef );

	void				SetStringRef(
								UInt16		inMaxBytes,
								StringPtr	inStringPtr);

									// Assignment operators

	LStringRef&		operator = ( const LString& inString )
						{
							LString::operator=(inString);
							return *this;
						}

	LStringRef&		operator = ( ConstStringPtr inStringPtr )
						{
							LString::operator=(inStringPtr);
							return *this;
						}

	LStringRef& 	operator = ( unsigned char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	LStringRef& 	operator = ( char inChar )
						{
							LString::operator=(inChar);
							return *this;
						}

	LStringRef&		operator = ( const char* inCString )
						{
							LString::operator=(inCString);
							return *this;
						}

	LStringRef&		operator = ( SInt32 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	LStringRef&		operator = ( SInt16 inNumber )
						{
							LString::operator=(inNumber);
							return *this;
						}

	LStringRef&		operator = ( FourCharCode inCode )
						{
							LString::operator=(inCode);
							return *this;
						}
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
