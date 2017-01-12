// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LString.cp					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes for handling Pascal Strings
//
//	LString		- Abstract base class
//	LStr255		- Wrapper class for Str255
//	TString<T>	- Template class for any string type (array of unsigned char)
//	LStringRef	- Refers to a string stored elsewhere

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LString.h>
#include <PP_Resources.h>
#include <UMemoryMgr.h>

#include <fp.h>
#include <Script.h>


// ---------------------------------------------------------------------------

	// To perform conversions between strings and floating point
	// numbers, we use some Apple extensions conditionally defined
	// in fp.h. The __NOEXTENSIONS__ preprocessor symbol controls
	// whether the extensions are included.
	//
	// Due to conflicts with other headers, MSLCarbonPrefix.h turns
	// off the extensions when building Mach-O binaries.
	//
	// Since LString relies on some of these extensions, we define
	// them here when fp.h doesn't define them.

#ifdef __NOEXTENSIONS__

extern "C" {

	#define	SIGDIGLEN		36  
	#define	DECSTROUTLEN	80			// max length for dec2str output

	struct decimal {
		char		sgn;				// sign 0 for +, 1 for -
		char		unused;
		short		exp;				// decimal exponent
		struct {
			unsigned char	length;
			unsigned char	text[SIGDIGLEN];	// significant digits
			unsigned char	unused;
		} sig;
	};
	typedef struct decimal decimal;

	struct decform {
		char	style;					// FLOATDECIMAL or FIXEDDECIMAL
		char	unused;
		short	digits;
	};
	typedef struct decform decform;

	extern void		num2dec( const decform *f, double_t x, decimal *d );
	extern void		dec2str( const decform *f, const decimal *d, char *s );
	extern double	x80tod( const extended80 * x80 );
	extern void		dtox80( const double *x, extended80 *x80 );
}

#endif
// ---------------------------------------------------------------------------
	
PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ LString
// ===========================================================================
//	Abstract base class for a Pascal-style string

// ---------------------------------------------------------------------------
//	¥ operator SInt32
// ---------------------------------------------------------------------------
//	Conversion to a long integer

LString::operator SInt32() const
{
	SInt32	number;
	::StringToNum(mStringPtr, &number);
	return number;
}


// ---------------------------------------------------------------------------
//	¥ operator double
// ---------------------------------------------------------------------------
//	Conversion to a double floating point number

LString::operator double() const
{
	return static_cast<double>(StringToLongDouble(mStringPtr));
}


// ---------------------------------------------------------------------------
//	¥ operator long double
// ---------------------------------------------------------------------------
//	Conversion to a long double floating point number

LString::operator long double() const
{
	return StringToLongDouble(mStringPtr);
}


// ---------------------------------------------------------------------------
//	¥ operator FourCharCode
// ---------------------------------------------------------------------------
//	Conversion to a four character code

LString::operator FourCharCode() const
{
	FourCharCode	theCode = 0;

		// If String is less than 4 characters, resulting FourCharCode
		// is padded with 0's in the high bytes. For example,
		//
		//   String = 'ab' results in FourCharCode = 0x00006162
		//     (since 'a' = 0x61, 'b' = 0x62)
		//
		// We pad this way because this is what the compiler does
		// when assigning literal values. For example,
		//
		//   FourCharCode  fcc = 'ab';  // value of fcc is 0x00006162
		//
		// Padding strings in the same way means we get consistent
		// results when converting. For example,
		//
		//   FourCharCode  fcc = 'ab';
		//   LStr255       str;
		//   FourCharCode  gcc = str = "ab";	// gcc = 'ab'
		//
		// If we padded in any other way (such as with spaces), the
		// value of gcc above would be different from fcc.

	UInt8			len = sizeof(FourCharCode);
	if (Length() < len) {			// Copy less if string is shorter
		len = Length();
	}
									// Get pointers to the low-order bytes
	UInt8			*dp = ((UInt8*) &theCode) + sizeof(FourCharCode) - 1;
	UInt8			*sp = mStringPtr + len;

	while (len-- > 0) {				// Copy bytes one at time from
		*dp-- = *sp--;				//   right to left
	}

	return EndianU32_BtoN(theCode);
}


#pragma mark --- Assignment ---

// ---------------------------------------------------------------------------
//	¥ Assign ( LString&, UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Assigment from a substring of another String object
//
//	Use inStartPos and inCount to specify the substring of inString to
//	copy into this String. The default values of these parameters
//	will copy all of inString.
//
//	If inStartPos is beyond the end of inString or inCount is zero,
//	this String is made empty. If inCount extends beyong the end of
//	inString, only bytes from inStartPos to the end of inString
//	are copied.

LString&
LString::Assign(
	const LString&	inString,
	UInt8			inStartPos,
	UInt8			inCount)
{
	if ((inStartPos > inString.Length()) || (inCount == 0)) {
										// Substring to copy is empty
		mStringPtr[0] = 0;				//   so make this String empty

	} else {
										// If count is too big, limit it
										//   to the end of the String
		if ((inStartPos + inCount - 1) > inString.Length()) {
			inCount = (UInt8) (inString.Length() - inStartPos + 1);
		}

		LoadFromPtr(&inString[inStartPos], inCount);
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( ConstStringPtr )
// ---------------------------------------------------------------------------
//	Assigment from a Pascal string

LString&
LString::Assign(
	ConstStringPtr	inStringPtr)
{
	if (inStringPtr == nil) {
		mStringPtr[0] = 0;

	} else {
		Assign(inStringPtr + 1, inStringPtr[0]);
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( unsigned char )
// ---------------------------------------------------------------------------
//	Assigment from a character

LString&
LString::Assign(
	unsigned char	inChar)
{
	mStringPtr[0] = 1;					// 1-character string
	mStringPtr[1] = inChar;
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Set String contents from a pointer to characters and a length

LString&
LString::Assign(
	const void*		inPtr,
	UInt8			inLength)
{
	LoadFromPtr(inPtr, inLength);
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( Handle )
// ---------------------------------------------------------------------------
//	Set String contents from the data in a Handle

LString&
LString::Assign(
	Handle		inHandle)
{
	if (inHandle != nil) {

			// Copy as much of data in Handle as fits in our string

		UInt8	bytesToCopy = (UInt8) (mMaxBytes - 1);

		Size	handleSize = ::GetHandleSize(inHandle);

		if (handleSize < (Size) bytesToCopy) {
			bytesToCopy = (UInt8) handleSize;
		}
								// LoadFromPtr doesn't move memory
		LoadFromPtr((UInt8*)*inHandle, bytesToCopy);

	} else {
		mStringPtr[0] = 0;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( ResIDT, SInt16 )
// ---------------------------------------------------------------------------
//	Set String contents from a 'STR ' or 'STR#' resource
//
//	The value of inIndex determines whether to use a 'STR ' or 'STR#'.
//	Uses 'STR ' for inIndex <= 0, and 'STR#' for inIndex >= 1.

LString&
LString::Assign(
	ResIDT		inResID,
	SInt16		inIndex)
{
	if (inIndex <= 0) {
		LoadFromSTRResource(inResID);
	} else {
		LoadFromSTRListResource(inResID, inIndex);
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( SInt32 )
// ---------------------------------------------------------------------------
//	Assignment from a long integer

LString&
LString::Assign(
	SInt32		inNumber)
{
	Str31	numStr;
	::NumToString(inNumber, numStr);
	Assign(numStr + 1, numStr[0]);
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( long double, ConstStringPtr )
// ---------------------------------------------------------------------------
//	Assignment from a long double using a formatting string.
//
//	The formatting string is assumed to use the System NumberParts table,
//	which means you must localize your format strings for each Script.
//
//	Use the Assign() function below which accepts a NumberParts resource ID
//	if you want to use standard format strings that get localized at runtime.
//
//	outFormatResult may be nil. If so, not format result is passed back.

LString&
LString::Assign(
	long double			inNumber,
	ConstStringPtr		inFormatString,
	FormatResultType*	outFormatResult)
{
	NumFormatString		numFormat;
	NumberParts			partsTable;

	FetchFloatFormat(inFormatString, numFormat, partsTable);

	return Assign(inNumber, numFormat, partsTable, outFormatResult);
}


// ---------------------------------------------------------------------------
//	¥ Assign ( long double, ConstStringPtr, ResIDT )
// ---------------------------------------------------------------------------
//	Assignment from a long double using a formatting string and the
//	resource ID of a NumberParts table
//
//	This function allows you to specify number formatting strings using
//	a standard NumberParts table, and then obtain format records that
//	use the local System NumberParts as specified by the user.
//
//	For example, assuming that 'NoPt' resource 200 is a standard US
//	NumberParts table,
//
//		myString.Assign(1234.56, "\p###,###.##", 200);
//
//	On a machine with US number formatting, myString will be "1,234.56".
//	On a machine with Danish number formatting, myString will be
//	"1.234,56" (Danish use '.' as the thousands separator and ',' as
//	the decimal point).
//
//	However, if you use the Assign() routine above:
//
//		myString.Assign(1234.56, "\p###,###.##");
//
//	This will correctly set myString to "1,234.56" on a machine with
//	US number formatting. But will not work for Danish number formatting,
//	where the format string is invalid.
//
//	outFormatResult may be nil. If so, not format result is passed back.

LString&
LString::Assign(
	long double			inNumber,
	ConstStringPtr		inFormatString,
	ResIDT				inNumberPartsID,
	FormatResultType*	outFormatResult)
{
	Handle	partsH = ::GetResource(ResType_NumberParts, inNumberPartsID);
	ThrowIfResFail_(partsH);
	StHandleLocker	lock(partsH);

	NumberParts*		myFormatRec = reinterpret_cast<NumberParts*>(*partsH);
	NumFormatString		sysFormatRec;
	NumberParts			sysPartsTable;

	FetchFloatFormat(inFormatString, *myFormatRec, sysFormatRec, sysPartsTable);

	return Assign(inNumber, sysFormatRec, sysPartsTable, outFormatResult);
}


// ---------------------------------------------------------------------------
//	¥ Assign ( long double, NumFormatString&, NumberParts& )
// ---------------------------------------------------------------------------
//	Assignment from a long double using formatting data structures.
//
//	Call LString::FetchFloatFormat() to get the formatting data structures
//	which correspond to a formatting string.
//
//	outFormatResult may be nil. If so, not format result is passed back.

LString&
LString::Assign(
	long double				inNumber,
	const NumFormatString&	inNumFormat,
	const NumberParts&		inPartsTable,
	FormatResultType*		outFormatResult)
{
	// First we convert the long double to a Toolbox Float80
	// data type (old type name is extended80). This conversion
	// depends on the processor.
	//
	// Then, we convert the Float80 to a string using the
	// Toolbox call ExtendedToString().

	FormatStatus	status;

#if TARGET_OS_MAC && TARGET_CPU_68K	// Mac OS on 68K

	#if TARGET_RT_MAC_68881			// 68K using 68881 math coprocessor
									//		long double is Float96
	Float80		x80Number;

	::x96tox80(&inNumber, &x80Number);
	status = ::ExtendedToString(&x80Number, &inNumFormat, &inPartsTable, mStringPtr);

	#else							// Standard 68K
									//		long double is Float80

	status = ::ExtendedToString(&inNumber, &inNumFormat, &inPartsTable, mStringPtr);
	#endif

#elif TARGET_OS_MAC && TARGET_CPU_PPC	// Mac OS on PowerPC
										//	long double is the same as double
										//	for Metrowerks C/C++

	Float80		x80Number;
	double		doubleNumber = (double) inNumber;

	::dtox80(&doubleNumber, &x80Number);
	status = ::ExtendedToString(&x80Number, &inNumFormat, &inPartsTable, mStringPtr);

#else								// Anything else

	Float80		x80Number;

	::ldtox80(&inNumber, &x80Number);
	status = ::ExtendedToString(&x80Number, &inNumFormat, &inPartsTable, mStringPtr);

#endif

	if ((FormatResultType) status != fFormatOK) {
		SignalStringLiteral_("Bad formatting assigning long double to a string");
	}
	
	if (outFormatResult != nil) {
		*outFormatResult = (FormatResultType) status;
	}

	return *this;
}



#if TARGET_OS_MAC					// Only works for Mac OS

// ---------------------------------------------------------------------------
//	The conversion routine num2dec() in <fp.h> takes a double_t parameter.
//	The basic type of double_t is "long double" on 68K, but "double" on
//	PPC (and other platforms). The binary MathLib library which implements
//	num2dec() depends on these definitions.
//
//	Unfortunately, both <fp.h> and <cmath> define double_t. If the definitions
//	differ, num2dec() will probably fail to work properly. To enforce the
//	correct type and avoid conflicting definitions, we define our own type.

#if TARGET_CPU_68K
	typedef long double		fp_double_t;
#else
	typedef double			fp_double_t;
#endif

// ---------------------------------------------------------------------------
//	¥ Num2Dec2Str
// ---------------------------------------------------------------------------

static void
Num2Dec2Str(
	LString*	inString,
	fp_double_t	inNumber,
	char		inStyle,
	SInt16		inDigits)
{
		// Toolbox num2dec() converts a double_t number to a
		// decform, a struct which describes a number

	decform	theDecform;
	theDecform.style  = (char) inStyle;
	theDecform.digits = inDigits;
	decimal	decNumber;

	::num2dec(&theDecform, inNumber, &decNumber);

		// The Toolbox dec2str() routine takes a "decimal" number
		// and converts it into a C string that can be at most
		// DECSTROUTLEN characters (we add 1 for the null terminator).

	char	numStr[DECSTROUTLEN + 1];
	::dec2str(&theDecform, &decNumber, numStr);

	inString->Assign(numStr, LString::CStringLength(numStr));
}


// ---------------------------------------------------------------------------
//	¥ Assign ( double, char, SInt16 )
// ---------------------------------------------------------------------------
//	Assignment from floating point number
//
//	inStyle must be FLOATDECIMAL or FIXEDDECIMAL
//
//	For FLOATDECIMAL, inDigits is the number of significant digits
//		(should be > 0)
//	For FIXEDDECIMAL, inDigits is the number of digits to the right
//		of the decimal point

LString&
LString::Assign(
	double			inNumber,
	char			inStyle,
	SInt16			inDigits)
{
		// No cast needed for inNumber since there is an automatic
		// conversion if fp_double_t is larger than "double".
		
	Num2Dec2Str(this, inNumber, inStyle, inDigits);
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Assign ( long double, char, SInt16 )
// ---------------------------------------------------------------------------
//	Assignment from floating point number
//
//	inStyle must be FLOATDECIMAL or FIXEDDECIMAL
//
//	For FLOATDECIMAL, inDigits is the number of significant digits
//		(should be > 0)
//	For FIXEDDECIMAL, inDigits is the number of digits to the right
//		of the decimal point

LString&
LString::Assign(
	long double		inNumber,
	char			inStyle,
	SInt16			inDigits)
{
		// Cast inNumber to fp_double_t. There will be loss of
		// precision if fp_double_t is not "double".
		
	Num2Dec2Str(this, (fp_double_t) inNumber, inStyle, inDigits);
	return *this;
}

#endif // TARGET_OS_MAC


// ---------------------------------------------------------------------------
//	¥ Assign ( FourCharCode )
// ---------------------------------------------------------------------------
//	Assignment from a four character code

LString&
LString::Assign(
	FourCharCode	inCode)
{
	inCode = EndianU32_NtoB(inCode);
	LoadFromPtr(&inCode, sizeof(FourCharCode));
	return *this;
}

#pragma mark --- Append ---

// ---------------------------------------------------------------------------
//	¥ Append ( unsigned char )
// ---------------------------------------------------------------------------
//	Append a character to this String

LString&
LString::Append(
	unsigned char	inChar)
{
	if (mStringPtr[0] < (mMaxBytes - 1)) {
		mStringPtr[++mStringPtr[0]] = inChar;
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Append ( SInt32 )
// ---------------------------------------------------------------------------
//	Append a number to this String

LString&
LString::Append(
	SInt32	inNumber)
{
	Str31	numStr;
	::NumToString(inNumber, numStr);

	return Append(numStr);
}


// ---------------------------------------------------------------------------
//	¥ Append ( const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Append characters to the end of this String

LString&
LString::Append(
	const void*		inPtr,
	UInt8			inLength)
{
	SInt16	charsToCopy = inLength;			// Limit to max size of String
	if ((mStringPtr[0] + charsToCopy + 1) > mMaxBytes) {
		charsToCopy = (SInt16) (mMaxBytes - mStringPtr[0] - 1);
	}

	::BlockMoveData(inPtr, mStringPtr + mStringPtr[0] + 1, charsToCopy );

	mStringPtr[0] += (UInt8) charsToCopy;	// Adjust length byte

	return *this;
}

#pragma mark --- Search for Substring ---

// ---------------------------------------------------------------------------
//	¥ Find ( const void*, UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Return the index of where a substring starts within this String
//
//	Returns 0 if the substring is not in the String
//	inStartPos is the index within the String at which to begin searching.
//		The default value is 1. Value 0 is not valid.
//
//	To find the next occurrence, set inStartPos to one plus the index
//	returned by the last Find.

UInt8
LString::Find(
	const void*		inPtr,				// Ptr to first char of string to find
	UInt8			inLength,			// Length of string to find
	UInt8			inStartPos) const
{
	UInt8	startIndex = 0;
										// Last possible index depends on
										//   relative lengths of the strings
	SInt16	lastChance = (SInt16) (Length() - inLength + 1);

										// Search forward from starting pos
	for (SInt16 i = inStartPos; i <= lastChance; i++) {
		if ((*mCompareFunc)(mStringPtr + i, inPtr, inLength, inLength) == 0) {
			startIndex = (UInt8) i;
			break;
		}
	}

	return startIndex;
}


// ---------------------------------------------------------------------------
//	¥ ReverseFind ( const void*, UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Return the index of where a substring starts within this String, searching
//	from the end of the String
//
//	Returns 0 if the substring is not in the String
//	inStartPos is the index within the String at which to begin searching.
//		The default value is 255. Value 0 is not valid.
//
//	To find the previous occurrence, set inStartPos to the index returned
//	by the last ReverseFind minus 1.

UInt8
LString::ReverseFind(
	const void*		inPtr,
	UInt8			inLength,
	UInt8			inStartPos) const
{
	UInt8	startIndex = 0;
										// Highest possible index depends on
										//   relative lengths of the strings
	SInt16	lastChance = (SInt16) (Length() - inLength + 1);
	if (lastChance > 0) {				// Substring can't be longer
		if (inStartPos > lastChance) {	// Don't bother looking after lastChance
			inStartPos = (UInt8) lastChance;
		}
										// Search backward from starting pos
		for (SInt16 i = inStartPos; i >= 1; i--) {
			if ((*mCompareFunc)(mStringPtr + i, inPtr, inLength, inLength) == 0) {
				startIndex = (UInt8) i;
				break;
			}
		}
	}

	return startIndex;
}


// ---------------------------------------------------------------------------
//	¥ BeginsWith ( const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Return whether this String begins with a specified String

Boolean
LString::BeginsWith(
	const void*		inPtr,
	UInt8			inLength) const
{
										// Compare first "inLength" bytes
										//   of String with input
	return (inLength <= Length()) &&
		   ((*mCompareFunc)(mStringPtr + 1, inPtr, inLength, inLength) == 0);
}


// ---------------------------------------------------------------------------
//	¥ EndsWith ( const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Return whether this String ends with a specified String

Boolean
LString::EndsWith(
	const void*		inPtr,
	UInt8			inLength) const
{
										// Compare last "inLength" bytes
										//   of String with input
	return (inLength <= Length()) &&
		   ((*mCompareFunc)(mStringPtr + Length() - inLength + 1, inPtr,
		   					inLength, inLength) == 0);
}


// ---------------------------------------------------------------------------
//	¥ FindWithin ( const void*, UInt32, UInt8 )
// ---------------------------------------------------------------------------
//	Return the index of where this String starts within another String
//
//	Returns 0 if the substring is not in the String
//	inStartPos is the index within inPtr at which to begin searching.
//		The default value is 1. Value 0 is not valid.
//
//	To find the next occurrence, set inStartPos to one plus the index
//	returned by the last FindWithin.

UInt32
LString::FindWithin(
	const void*		inPtr,				// Ptr to first char of string to search
	UInt32			inLength,			// Length of string to search
	UInt32			inStartPos) const	// Index at which to start searching
{
	UInt32	startIndex = 0;

										// inStartPos of zero is not valid and
										//   substring can't be longer
	if ( (inStartPos > 0) && (inLength >= Length()) ) {

										// Highest possible offset depends on
										//   relative lengths of the strings
		UInt32	lastOffset = inLength - Length();

										// Search forward after starting pos
		for (UInt32 offset = inStartPos - 1; offset <= lastOffset; offset++) {
			if ((*mCompareFunc)(mStringPtr + 1, (UInt8*) inPtr + offset, Length(), Length()) == 0) {
				startIndex = offset + 1;
				break;
			}
		}
	}

	return startIndex;
}


// ---------------------------------------------------------------------------
//	¥ ReverseFindWithin ( const void*, UInt32, UInt8 )
// ---------------------------------------------------------------------------
//	Return the index of where this String starts within another String,
//	searching from the end of the String
//
//	Returns 0 if this String is not within the input string
//	inStartPos is the index within inPtr at which to begin searching.
//		Value 0 is not valid.
//
//	To find the previous occurrence, set inStartPos to the index returned
//	by the last ReverseFindWithin minus 1.

UInt32
LString::ReverseFindWithin(
	const void*		inPtr,
	UInt32			inLength,
	UInt32			inStartPos) const
{
	UInt32	startIndex = 0;

										// inStartPos of zero is not valid and
										//   substring can't be longer
	if ( (inStartPos > 0) && (inLength >= Length()) ) {
										// Highest possible offset depends on
										//   relative lengths of the strings
		UInt32	lastOffset = inLength - Length();

										// We start from the lesser of lastOffset
		if (inStartPos <= lastOffset) {	//   and inStartPos - 1
			lastOffset = inStartPos - 1;
		}

										// Search backward from last offset
		UInt32	offset = lastOffset + 1;
		do {
			if ((*mCompareFunc)(mStringPtr + 1, (UInt8*) inPtr + (--offset), Length(), Length()) == 0) {
				startIndex = offset + 1;
				break;
			}
		} while (offset > 0);
	}

	return startIndex;
}

#pragma mark --- Copy Substring ---

// ---------------------------------------------------------------------------
//	¥ operator () ( UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Return a LStr255 String object containing the specified substring
//	of this String

LStr255
LString::operator()(
	UInt8	inStartPos,
	UInt8	inCount) const
{
	LStr255	subString;

	if (inStartPos <= Length()) {
		if (inStartPos == 0) {			// Interpret 0 or 1 as the first
			inStartPos = 1;				//    character
		}

										// If count is too big, limit it
										//   to the end of the String
		if ((inStartPos + inCount - 1) > Length()) {
			inCount = (UInt8) (Length() - inStartPos + 1);
		}

		subString.Assign(mStringPtr + inStartPos, inCount);
	}

	return subString;
}

#pragma mark --- Changing Substrings ---

// ---------------------------------------------------------------------------
//	¥ Insert ( const void*, UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Insert a string into this String starting at the specified index

LString&
LString::Insert(
	const void*		inPtr,
	UInt8			inLength,
	UInt8			inAtIndex)
{
	if (inAtIndex == 0) {				// Interpret 0 or 1 as the start
		inAtIndex = 1;					//   of the String
	} else if (inAtIndex > Length() + 1) {
										// Interpret any index past end
										//   to mean to append the string
		inAtIndex = (UInt8) (Length() + 1);
	}

										// Don't insert more than bytes
										//    than this String can hold
	if ((inLength + Length() + 1) > mMaxBytes) {
		inLength = (UInt8) (mMaxBytes - Length() - 1);
	}

	if (inAtIndex <= Length()) {		// Shift up bytes after insertion
		::BlockMoveData(mStringPtr + inAtIndex,
						mStringPtr + inAtIndex + inLength,
						Length() - inAtIndex + 1);
	}

										// Copy input string
	::BlockMoveData(inPtr, mStringPtr + inAtIndex, inLength);

	mStringPtr[0] += inLength;			// Adjust length byte

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Remove ( UInt8, UInt8 )
// ---------------------------------------------------------------------------
//	Remove a given number of bytes from this String starting at the
//	specified index

LString&
LString::Remove(
	UInt8	inStartPos,
	UInt8	inCount)
{
	if (inStartPos <= Length()) {
		if (inStartPos == 0) {			// Interpret 0 or 1 as the first
			inStartPos = 1;				//    character
		}

										// If count is too big, limit it
										//   to the end of the String
		if ((inStartPos + inCount - 1) > Length()) {
			inCount = (UInt8) (Length() - inStartPos + 1);
		}

		if ((inStartPos + inCount) <= Length()) {
										// Shift down bytes after removed one
			::BlockMoveData(mStringPtr + inStartPos + inCount,
							mStringPtr + inStartPos,
							Length() - inStartPos - inCount + 1);
		}

		mStringPtr[0] -= inCount;		// Adjust length byte
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ Replace ( UInt8, UInt8, const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Replace a given number of bytes in this String starting at the
//	specified index with the another string, specified a pointer and length

LString&
LString::Replace(
	UInt8			inStartPos,			// First byte to replace
	UInt8			inCount,			// Number of bytes to replace
	const void*		inPtr,				// Ptr to replacement bytes
	UInt8			inLength)			// Number of replacement bytes
{
	Remove(inStartPos, inCount);		// Replace same as Remove & Insert
	Insert(inPtr, inLength, inStartPos);

	return *this;
}

#pragma mark --- Constructor/Destructor ---

// ---------------------------------------------------------------------------
//	¥ LString
// ---------------------------------------------------------------------------
//	Constructor from a max length and a string pointer
//
//	Subclasses must call this "protected" constructor to specify the
//	maximum string size (including the length byte) and a pointer to
//	the storage for the string

LString::LString(
	SInt16		inMaxBytes,
	StringPtr	inStringPtr)
{
	mMaxBytes = (UInt16) inMaxBytes;
	mStringPtr = inStringPtr;
	mCompareFunc = ToolboxCompareText;		// Default comparison function
}

// ---------------------------------------------------------------------------
//	¥ LString
// ---------------------------------------------------------------------------
//	Constructor from a max length, string pointer, and comparison function
//
//	Subclasses must call this "protected" constructor to specify the
//	maximum string size (including the length byte), a pointer to
//	the storage for the string, and the comparison function

LString::LString(
	SInt16		inMaxBytes,
	StringPtr	inStringPtr,
	CompareFunc	inCompareFunc)
{
	mMaxBytes = (UInt16) inMaxBytes;
	mStringPtr = inStringPtr;
	mCompareFunc = inCompareFunc;
}


// ---------------------------------------------------------------------------
//	¥ ~LString
// ---------------------------------------------------------------------------
//	Destructor is pure virtual, but we must implement it anyway

LString::~LString()
{
}

#pragma mark --- Comparison Functions ---

// ---------------------------------------------------------------------------
//	¥ ToolboxCompareText [static]
// ---------------------------------------------------------------------------
//	String comparison function that uses the Toolbox CompareText() routine

SInt16
LString::ToolboxCompareText(
	const void*		inLeft,
	const void*		inRight,
	UInt8			inLeftLength,
	UInt8			inRightLength)
{
	return ::CompareText(inLeft, inRight, inLeftLength, inRightLength, nil);
}


// ---------------------------------------------------------------------------
//	¥ CompareBytes [static]
// ---------------------------------------------------------------------------
//	Simple byte-by-byte value comparison of two strings

SInt16
LString::CompareBytes(
	const void*		inLeft,
	const void*		inRight,
	UInt8			inLeftLength,
	UInt8			inRightLength)
{
	UInt8	n = inLeftLength;				// Determine shortest length
	if (inRightLength < inLeftLength) {
		n = inRightLength;
	}

	const UInt8	*leftP = (UInt8*) inLeft;
	const UInt8 *rightP = (UInt8*) inRight;

	while (n > 0) { 						// Compare up to shortest length
		if (*leftP != *rightP) {
			if (*leftP > *rightP) {
				return 1;					// Left is greater
			}
			return -1;						// Left is smaller
		}

		leftP++;
		rightP++;
		n--;
	}

		// All bytes up to shortest length match. Therefore, the
		// longer of the two string is the greater one. If they
		// have the same length, then they are equal.

	if (inLeftLength > inRightLength) {
		return 1;

	} else if (inLeftLength < inRightLength) {
		return -1;
	}

	return 0;								// String are equal
}

// ---------------------------------------------------------------------------
//	¥ CompareIgnoringCase [static]
// ---------------------------------------------------------------------------
//	Case-insensitve string comparison

SInt16
LString::CompareIgnoringCase(
	const void*		inLeft,
	const void*		inRight,
	UInt8			inLeftLength,
	UInt8			inRightLength)
{
		// Copy both left and right strings and convert all characters
		// to uppercase before performing comparison.

	LStr255		leftStr(inLeft, inLeftLength);
	::UppercaseText(leftStr.TextPtr(), leftStr.ShortLength(), smSystemScript);

	LStr255		rightStr(inRight, inRightLength);
	::UppercaseText(rightStr.TextPtr(), rightStr.ShortLength(), smSystemScript);

	return ::CompareText(leftStr.TextPtr(),rightStr.TextPtr(),
						 leftStr.ShortLength(), rightStr.ShortLength(), nil);
}

#pragma mark --- Public Utilities ---

// ---------------------------------------------------------------------------
//	¥ CStringLength
// ---------------------------------------------------------------------------
//	Return the length of a C string, but with an upper limit of 255

UInt8
LString::CStringLength(
	const char*		inCString)
{
		// Find length of C string by searching for the terminating
		// null character. However, don't bother to look past 255
		// characters, since that's all that a Pascal string can hold.

	UInt8	strLength = 0;

	if (inCString != nil) {
		while ( (strLength < 255)  &&  (inCString[strLength] != '\0') ) {
			strLength += 1;
		}
	}

	return strLength;
}


// ---------------------------------------------------------------------------
//	¥ CopyPStr
// ---------------------------------------------------------------------------
//	Copy a Pascal string
//
//	Returns a pointer to the copied string
//
//	inSourceString may be nil, in which case outDestString is set to
//	a zero length string
//
//	outDestString must not be nil
//
//	inDestSize specifies the maximum size of the destination (INCLUDING
//		the length byte). It must be >= 1. The default is sizeof Str255.
//
//	Call this function as follows:
//
//		CopyPStr(source, dest, sizeof(dest));

StringPtr
LString::CopyPStr(
	ConstStringPtr	inSourceString,
	StringPtr		outDestString,
	SInt16			inDestSize)
{
	SInt16	dataLen = 1;

	if (inSourceString != nil) {
		dataLen = (SInt16) (inSourceString[0] + 1);
		if (dataLen > inDestSize) {
			dataLen = inDestSize;
		}

		::BlockMoveData(inSourceString, outDestString, dataLen);
	}

	outDestString[0] = (UInt8) (dataLen - 1);		// Store length byte

	return outDestString;
}


// ---------------------------------------------------------------------------
//	¥ CToPStr
// ---------------------------------------------------------------------------
//	Convert a C string to a Pascal string in place
//
//	On entry, ioStr should be a null-terminated C string
//	On exit, ioStr is a Pascal string with a length count in the first byte
//		Pascal string length is limited to 255 characters

StringPtr
LString::CToPStr(
	char*	ioStr)
{
	if (ioStr != nil) {
		UInt8	len = LString::CStringLength(ioStr);

		::BlockMoveData(ioStr, ioStr + 1, len);

		ioStr[0] = (char) len;				// Store length count
	}

	return (StringPtr) ioStr;
}


// ---------------------------------------------------------------------------
//	¥ PToCStr
// ---------------------------------------------------------------------------
//	Convert a Pascal string to a C string in place
//
//	On entry, ioStr should be a Pascal string with a length count
//	On exit, ioStr is a null-terminated C string

char*
LString::PToCStr(
	StringPtr	ioStr)
{
	if (ioStr != nil) {
		UInt8	len = ioStr[0];

		::BlockMoveData(ioStr + 1, ioStr, len);

		ioStr[len] = 0;						// Store null terminator
	}

	return (char*) ioStr;
}


// ---------------------------------------------------------------------------
//	¥ AppendPStr
// ---------------------------------------------------------------------------
//	Append two Pascal strings. The first string becomes the combination
//	of the first and second strings.
//
//	Returns a pointer to the resulting string
//
//	inDestSize specifies the maximum size of the base string
//		(INCLUDING the length byte). The default is sizeof Str255.
//
//	Call this function as follows:
//
//		AppendPStr(ioString, appendMe, sizeof(ioString));

StringPtr
LString::AppendPStr(
	Str255				ioBaseString,
	ConstStringPtr		inAppendString,
	SInt16				inDestSize)
{
								// Limit combined string to inDestSize chars
    SInt16	charsToCopy = inAppendString[0];
    if (ioBaseString[0] + charsToCopy > inDestSize - 1) {
    	charsToCopy = (SInt16) (inDestSize - 1 - ioBaseString[0]);
    }

								// Copy second to end of first string
    ::BlockMoveData(inAppendString + 1,  ioBaseString + ioBaseString[0] + 1,
    					charsToCopy);
    							// Set length of combined string
    ioBaseString[0] += (UInt8) charsToCopy;

    return ioBaseString;
}


// ---------------------------------------------------------------------------
//	¥ FourCharCodeToPStr
// ---------------------------------------------------------------------------
//	Convert a four character code to a Pascal string and return a pointer
//	to the string
//
//	outString must point to a buffer (usually a string) that can hold
//	the resulting string, including the length byte. Therefore, the buffer
//	must be at least 5-bytes large. Usually, you will allocate an array
//	of 5 unsigned char's.

StringPtr
LString::FourCharCodeToPStr(
	FourCharCode	inCode,
	StringPtr		outString)
{
	inCode = EndianU32_NtoB(inCode);
	::BlockMoveData(&inCode, outString + 1, sizeof(FourCharCode));
	outString[0] = sizeof(FourCharCode);
	return outString;
}


// ---------------------------------------------------------------------------
//	¥ PStrToFourCharCode
// ---------------------------------------------------------------------------
//	Convert a Pascal string to a four character code

void
LString::PStrToFourCharCode(
	ConstStringPtr	inString,
	FourCharCode	&outCode)
{
	::BlockMoveData(inString + 1, &outCode, sizeof(OSType));
	outCode = EndianU32_BtoN(outCode);
}


// ---------------------------------------------------------------------------
//	¥ FetchFloatFormat
// ---------------------------------------------------------------------------
//	Pass back the System formatting info for floating point numbers
//
//	Input format string is assumed to use the System script's number parts
//	formatting information.

void
LString::FetchFloatFormat(
	ConstStringPtr			inFormatString,
	NumFormatString&		outSysNumFormat,
	NumberParts&			outSysPartsTable)
{
	GetSystemNumberParts(outSysPartsTable);

		// Convert format string to internal Toolbox number format

	::StringToFormatRec(inFormatString, &outSysPartsTable, &outSysNumFormat);
}


// ---------------------------------------------------------------------------
//	¥ FetchFloatFormat
// ---------------------------------------------------------------------------
//	Pass back formatting info for floating point numbers
//
//	Input format string uses the characters described by the input
//	NumberParts table. The output NumFormatString uses the System
//	NumberParts table.
//
//	This function allows you to specify number formatting strings using
//	a standard NumberParts table, and then obtain format records that
//	use the local System NumberParts as specified by the user.

void
LString::FetchFloatFormat(
	ConstStringPtr			inFormatString,
	const NumberParts&		inPartsTable,
	NumFormatString&		outSysNumFormat,
	NumberParts&			outSysPartsTable)
{
		// Convert input format string to a format record using the
		// input NumberParts table

	NumFormatString		myFormatRec;
	FormatStatus		status = ::StringToFormatRec(inFormatString,
											&inPartsTable, &myFormatRec);

		// Convert format record back to a format string, but using
		// the System NumberParts table. This gives us a format string
		// that uses the local number formatting characters that the
		// user has specified on this machine.

	GetSystemNumberParts(outSysPartsTable);

	Str255		sysFormatStr;
	TripleInt	positions;
	status = ::FormatRecToString(&myFormatRec, &outSysPartsTable,
									sysFormatStr, positions);

		// Convert System format string to a format record using
		// the System NumberParts table.

	status = ::StringToFormatRec(sysFormatStr, &outSysPartsTable,
									&outSysNumFormat);
									
		// [Workaround] OS Number Formatting Bug
		//		When the thousands or decimal separator is the
		//		same as the left and/or right quote character,
		//		::StringToFormatRec() returns fBadPartsTable.
		//		Some Swiss number formatting systems use a
		//		single quote as the thousands separator, triggering
		//		this bug.
		//
		//		As a workaround, we replace the left/right quote
		//		characters with Ç and È, which are the preferred
		//		characters in this situation, and try again.
									
	if (status == fBadPartsTable) {
	
		outSysPartsTable.data[tokLeftQuote - 1].b = 0xC7;	// Ç
		outSysPartsTable.data[tokRightQuote - 1].b = 0xC8;	// È
	
		status = ::StringToFormatRec(sysFormatStr, &outSysPartsTable,
										&outSysNumFormat);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSystemNumberParts
// ---------------------------------------------------------------------------

void
LString::GetSystemNumberParts(
	NumberParts&	outSysPartsTable)
{
		// Get the NumberParts table from the tokens ('itl4') resource
		// for the System Script.

	Handle	tableH;
	SInt32	offset;
	SInt32	tableLen;
	::GetIntlResourceTable(smSystemScript, smNumberPartsTable,
							&tableH, &offset, &tableLen);
	ThrowIfNil_(tableH);

	::BlockMoveData((*tableH + offset), &outSysPartsTable, sizeof(NumberParts));
}


// ---------------------------------------------------------------------------
//	¥ StringToLongDouble
// ---------------------------------------------------------------------------
//	Convert a Pascal string to a long double

long double
LString::StringToLongDouble(
	ConstStringPtr			inString,
	ConstStringPtr			inFormatString,
	FormatResultType*		outFormatResult)
{
	NumFormatString		numFormat;
	NumberParts			partsTable;

	LString::FetchFloatFormat(inFormatString, numFormat, partsTable);

	return LString::StringToLongDouble(inString, numFormat,
										partsTable, outFormatResult);
}


// ---------------------------------------------------------------------------
//	¥ StringToLongDouble
// ---------------------------------------------------------------------------
//	Convert a Pascal string to a long double
//
//	outFormatResult may be nil. If so, no status result is passed back.

long double
LString::StringToLongDouble(
	ConstStringPtr			inString,
	const NumFormatString&	inNumFormat,
	const NumberParts&		inPartsTable,
	FormatResultType*		outFormatResult)
{
		// First we use StringToExtended() to convert the string to
		// a Toolbox Float80 data type (old type name is extended80).
		// Then, we convert the Float80 to a long double.
		//
		// The relationship between Float80 and long double depends
		// on the processor.

	long double		outNumber	= 0;
	FormatStatus	status		= fFormatOK;

#if TARGET_OS_MAC && TARGET_CPU_PPC	// Mac OS on PowerPC
									//		long double is same as double
									//		for Metrowerks C/C++

	Float80		x80Number;
	status = ::StringToExtended(inString, &inNumFormat, &inPartsTable, &x80Number);
	outNumber = ::x80tod(&x80Number);

#elif TARGET_OS_MAC && TARGET_CPU_68K	// Mac OS on 68K

	#if TARGET_RT_MAC_68881		// 68K using 68881 math coprocessor
								//		long double is Float96

		Float80		x80Number;
		status = ::StringToExtended(inString, &inNumFormat, &inPartsTable, &x80Number);
		::x80tox96(&x80Number, &outNumber);

	#else						// Standard 68K
								//		long double is Float80

		status = ::StringToExtended(inString, &inNumFormat, &inPartsTable, &outNumber);
	#endif

#else							// Unsupported on non-Mac OS platforms
	#pragma unused(inString, inNumFormat, inPartsTable)

#endif

	if (outFormatResult != nil) {
								// Pass back result of format conversion
		*outFormatResult = (FormatResultType) status;
	}

	return outNumber;
}


#pragma mark --- Protected Utilities ---

// ---------------------------------------------------------------------------
//	¥ LoadFromPtr
// ---------------------------------------------------------------------------
//	Fill in the String given a pointer and a byte count

void
LString::LoadFromPtr(
	const void*		inPtr,
	UInt8			inLength)
{
	if (inLength > 0) {
		if (inLength > mMaxBytes - 1) {	// mMaxBytes includes a length byte
										// Enforce maximum string length
			inLength = (UInt8) (mMaxBytes - 1);
		}

		::BlockMoveData(inPtr, mStringPtr + 1, inLength);
	}
	
	mStringPtr[0] = inLength;
}


// ---------------------------------------------------------------------------
//	¥ LoadFromSTRListResource
// ---------------------------------------------------------------------------
//	Fill in the String from a STR# resource

void
LString::LoadFromSTRListResource(
	ResIDT	inResID,
	SInt16	inIndex)
{
										// A string from an STR# resource
										//   can have up to 255 characters.
	if (mMaxBytes >= sizeof(Str255)) {
										// String is big enough to hold it
		::GetIndString(mStringPtr, inResID, inIndex);

	} else {							// String may be too small
		Str255 theTempString;			// Read chars from resource into
										//   a buffer, then load the String
										//   from that buffer (which will
										//   truncate if the String is
										//   too small
		::GetIndString(theTempString, inResID, inIndex);
		LoadFromStringPtr(theTempString);
	}
}


// ---------------------------------------------------------------------------
//	¥ LoadFromSTRResource
// ---------------------------------------------------------------------------
//	Fill in the String from a STR resource

void
LString::LoadFromSTRResource(
	ResIDT	inResID)
{
	StringHandle	strH = ::GetString(inResID);
	if (strH != nil) {
		LoadFromStringPtr(*strH);

	} else {
		SignalStringLiteral_("STR resource not found");

		mStringPtr[0] = 0;				// Empty String
	}
}

#pragma mark --- operator + ---

// ---------------------------------------------------------------------------
//	¥ operator+ (LString&, LString&)
// ---------------------------------------------------------------------------
//	Concatenate two String objects
//		leftString + rightString

LStr255
operator+(
	const LString&	inLeftString,
	const LString&	inRightString)
{
	LStr255	resultString = inLeftString;
	return resultString += inRightString;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (LString&, ConstStringPtr)
// ---------------------------------------------------------------------------
//	Concatenate a String object and a string pointer
//		leftString + "\pRight";

LStr255
operator+(
	const LString&	inLeftString,
	ConstStringPtr	inRightStringPtr)
{
	LStr255	resultString = inLeftString;
	return resultString += inRightStringPtr;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (ConstStringPtr, LString&)
// ---------------------------------------------------------------------------
//	Concatenate a string pointer and a String object
//		"\pLeft" + rightString;

LStr255
operator+(
	ConstStringPtr	inLeftStringPtr,
	const LString&	inRightString)
{
	LStr255	resultString = inLeftStringPtr;
	return resultString += inRightString;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (LString&, StringPtr)
// ---------------------------------------------------------------------------
//	Concatenate a String object and a string pointer
//		leftString + rightStringPtr;

LStr255
operator+(
	const LString&	inLeftString,
	StringPtr		inRightStringPtr)
{
	LStr255	resultString = inLeftString;
	return resultString += inRightStringPtr;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (StringPtr, LString&)
// ---------------------------------------------------------------------------
//	Concatenate a string pointer and a String object
//		leftStringPtr + rightString;

LStr255
operator+(
	StringPtr		inLeftStringPtr,
	const LString&	inRightString)
{
	LStr255	resultString = inLeftStringPtr;
	return resultString += inRightString;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (LString&, unsigned char)
// ---------------------------------------------------------------------------
//	Concatenate a String object and a character
//		leftString + 'a';

LStr255
operator+(
	const LString&	inLeftString,
	unsigned char	inRightChar)
{
	LStr255	resultString = inLeftString;
	return resultString += inRightChar;
}


// ---------------------------------------------------------------------------
//	¥ operator+ (unsigned char, LString&)
// ---------------------------------------------------------------------------
//	Concatenate a character and a String object
//		'a' + rightString;

LStr255
operator+(
	unsigned char	inLeftChar,
	const LString&	inRightString)
{
	LStr255	resultString = inLeftChar;
	return resultString += inRightString;
}


#pragma mark -
// ===========================================================================
//	¥ LStr255
// ===========================================================================
//	A String with a maximum of 255 characters

// ---------------------------------------------------------------------------
//	¥ LStr255								Default Constructor
// ---------------------------------------------------------------------------

LStr255::LStr255()

	: LString(sizeof(mString), mString)
{
	mString[0] = 0;
}


// ---------------------------------------------------------------------------
//	¥ LStr255								Copy Constructor
// ---------------------------------------------------------------------------

LStr255::LStr255(
	const LStr255&	inOriginal)
	
	: LString(sizeof(mString), mString, inOriginal.mCompareFunc)
{
	LoadFromStringPtr(inOriginal);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( const LString& )
// ---------------------------------------------------------------------------
//	Construct from an LString object

LStr255::LStr255(
	const LString&	inOriginal)
	
	: LString(sizeof(mString), mString, inOriginal.GetCompareFunc())
{
	LoadFromStringPtr(inOriginal);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( ConstStringPtr )
// ---------------------------------------------------------------------------
//	Constructor from a pointer to a string

LStr255::LStr255(
	ConstStringPtr	inStringPtr)
	
	: LString(sizeof(mString), mString)
{
	if (inStringPtr == nil) {
		mString[0] = 0;
	} else {
		LoadFromStringPtr(inStringPtr);
	}
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( unsigned char )
// ---------------------------------------------------------------------------
//	Constructor from a single character

LStr255::LStr255(
	unsigned char	inChar)
	
	: LString(sizeof(mString), mString)
{
	mString[0] = 1;
	mString[1] = inChar;
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( char )
// ---------------------------------------------------------------------------
//	Constructor from a single character

LStr255::LStr255(
	char	inChar)
	
	: LString(sizeof(mString), mString)
{
	mString[0] = 1;
	mString[1] = (UInt8) inChar;
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( const char* )
// ---------------------------------------------------------------------------
//	Constructor from a C string (null terminated)

LStr255::LStr255(
	const char*	inCString)
	
	: LString(sizeof(mString), mString)
{
	LoadFromPtr(inCString,  CStringLength(inCString));
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( const void*, UInt8 )
// ---------------------------------------------------------------------------
//	Constructor from a pointer and length

LStr255::LStr255(
	const void*		inPtr,
	UInt8			inLength)
		
	: LString(sizeof(mString), mString)
{
	LoadFromPtr(inPtr, inLength);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( Handle )
// ---------------------------------------------------------------------------
//	Constructor from the data in a Handle

LStr255::LStr255(
	Handle	inHandle)
	
	: LString(sizeof(mString), mString)
{
	Assign(inHandle);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( ResIDT, SInt16 )
// ---------------------------------------------------------------------------
//	Constructor from a 'STR ' or 'STR#' resource
//
//	The value of inIndex determines whether to use a 'STR ' or 'STR#'.
//	Uses 'STR ' for inIndex <= 0, and 'STR#' for inIndex >= 1.

LStr255::LStr255(
	ResIDT	inResID,
	SInt16	inIndex)
	
	: LString(sizeof(mString), mString)
{
	Assign(inResID, inIndex);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( SInt32 )
// ---------------------------------------------------------------------------
//	Constructor from a long integer

LStr255::LStr255(
	SInt32	inNumber)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( SInt16 )
// ---------------------------------------------------------------------------
//	Constructor from a long integer

LStr255::LStr255(
	SInt16	inNumber)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( long double, ConstStringPtr )
// ---------------------------------------------------------------------------
//	Constructor from floating point number

LStr255::LStr255(
	long double		inNumber,
	ConstStringPtr	inFormatString)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber, inFormatString);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( long double, NumFormatString&, NumberParts& )
// ---------------------------------------------------------------------------
//	Constructor from floating point number

LStr255::LStr255(
	long double				inNumber,
	const NumFormatString&	inNumFormat,
	const NumberParts&		inPartsTable)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber, inNumFormat, inPartsTable);
}

#if TARGET_OS_MAC						// Only works for Mac OS

// ---------------------------------------------------------------------------
//	¥ LStr255 ( double, SInt8, SInt16 )
// ---------------------------------------------------------------------------
//	Constructor from floating point number
//
//	inStyle must be FLOATDECIMAL or FIXEDDECIMAL
//
//	For FLOATDECIMAL, inDigits is the number of significant digits
//		(should be > 0)
//	For FIXEDDECIMAL, inDigits is the number of digits to the right
//		of the decimal point

LStr255::LStr255(
	double			inNumber,
	char			inStyle,
	SInt16			inDigits)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber, inStyle, inDigits);
}


// ---------------------------------------------------------------------------
//	¥ LStr255 ( long double, SInt8, SInt16 )
// ---------------------------------------------------------------------------
//	Constructor from floating point number
//
//	inStyle must be FLOATDECIMAL or FIXEDDECIMAL
//
//	For FLOATDECIMAL, inDigits is the number of significant digits
//		(should be > 0)
//	For FIXEDDECIMAL, inDigits is the number of digits to the right
//		of the decimal point

LStr255::LStr255(
	long double		inNumber,
	char			inStyle,
	SInt16			inDigits)
	
	: LString(sizeof(mString), mString)
{
	Assign(inNumber, inStyle, inDigits);
}

#endif // TARGET_OS_MAC

// ---------------------------------------------------------------------------
//	¥ LStr255 ( FourCharCode )
// ---------------------------------------------------------------------------
//	Constructor from a four character code, which is an unsigned long,
//	the same as ResType and OSType

LStr255::LStr255(
	FourCharCode	inCode)
	
	: LString(sizeof(mString), mString)
{
	Assign(inCode);
}


#pragma mark -
// ===========================================================================
//	¥ LStringRef
// ===========================================================================
//	Class does not contain storage for the string. The StringPtr points
//	to a Pascal string allocated elsewhere

// ---------------------------------------------------------------------------
//	¥ LStringRef							Default Constructor
// ---------------------------------------------------------------------------

LStringRef::LStringRef(
	SInt16		inMaxBytes,
	StringPtr	inStringPtr)	// Must point to buffer of inMaxBytes bytes
	
	: LString(inMaxBytes, inStringPtr)
{
}


// ---------------------------------------------------------------------------
//	¥ LStringRef							Copy Constructor
// ---------------------------------------------------------------------------
//	Copy points to the same string as the original

LStringRef::LStringRef(
	const LStringRef&	inStringRef)
	
	: LString( (SInt16) inStringRef.mMaxBytes,
			   inStringRef.mStringPtr,
			   inStringRef.mCompareFunc )
{
}


// ---------------------------------------------------------------------------
//	¥ LStringRef							Assignment Operator
// ---------------------------------------------------------------------------
//	Object points to the same string as the rhs operand

LStringRef&
LStringRef::operator = (
	const LStringRef&	inStringRef)
{
	mStringPtr	 = inStringRef.mStringPtr;
	mCompareFunc = inStringRef.mCompareFunc;
	mMaxBytes	 = inStringRef.mMaxBytes;

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ SetStringRef
// ---------------------------------------------------------------------------
//	Specify the string referenced by this object

void
LStringRef::SetStringRef(
	UInt16		inMaxBytes,
	StringPtr	inStringPtr)	// Must point to buffer of inMaxBytes bytes
{
	mMaxBytes	= inMaxBytes;
	mStringPtr	= inStringPtr;
}


PP_End_Namespace_PowerPlant
