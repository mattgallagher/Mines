// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LException.cp				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//
//	PowerPlant Exception class
//
//	LException inherits from std::exception, and adds Error Code and
//	Error String member variables. The Error Code typically stores a
//	Mac Toolbox OSErr or OSStatus error number.
//
//	Stack Space
//	-----------
//	LException has a Str255 member for storing the error string. This can
//	use a lot of stack space if you write code such as:
//
//		if (errorCondition) {
//			throw LException(errorCode, errorStr);
//		}
//
//	several times in a function. Each LException object is allocated on
//	the stack.
//
//	To avoid running out of stack space, you can use the class function
//	LException::Throw():
//
//		if (errorCondition) {
//			LException::Throw(errorCode, errorStr);
//		}
//
//	The LException::Throw() function encapsulates the LException object
//	within its local stack frame. Thus, space for only one LException object
//	is allocated on the stack no matter how many times you call
//	LException::Throw() from another function.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LException.h>
#include <MacMemory.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LException							Constructor				  [public]
// ---------------------------------------------------------------------------

LException::LException(
	SInt32			inErrorCode,
	ConstStringPtr	inErrorString) _MSL_NO_THROW
{
	mErrorCode = inErrorCode;
	SetErrorString(inErrorString);
}


// ---------------------------------------------------------------------------
//	¥ LException							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LException::LException(
	const LException&	inException) _MSL_NO_THROW
{
	mErrorCode = inException.mErrorCode;

	::BlockMoveData( inException.mErrorString,
					 mErrorString,
					 inException.mErrorString[0] + 1);
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LException&
LException::operator = (
	const LException&	inException) _MSL_NO_THROW
{
	mErrorCode = inException.mErrorCode;

	::BlockMoveData( inException.mErrorString,
					 mErrorString,
					 inException.mErrorString[0] + 1);

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LException							Destructor				  [public]
// ---------------------------------------------------------------------------

#ifdef __GNUC__
LException::~LException() throw()
#else
LException::~LException() _MSL_NO_THROW
#endif
{
}


// ---------------------------------------------------------------------------
//	¥ what															  [public]
// ---------------------------------------------------------------------------
//	Returns the error message as a C string. Inherited from std::exception.

const char*
#ifdef __GNUC__
LException::what() const throw()
#else
LException::what() const _MSL_NO_THROW
#endif
{
	StringPtr	lastPtr = (StringPtr) &mErrorString[mErrorString[0]];

	if (mErrorString[0] == 255) {	// No room for terminator
		*lastPtr = 0;				// Overwrite last character
	} else {
		*(lastPtr + 1) = 0;			// Put terminator after last character
	}
									// C string starts after length byte
	return (const char*) (mErrorString + 1);
}


// ---------------------------------------------------------------------------
//	¥ GetErrorCode											 [inline] [public]
// ---------------------------------------------------------------------------

#pragma mark LException::GetErrorCode

//	SInt32
//	LException::GetErrorCode()					// Defined inline


// ---------------------------------------------------------------------------
//	¥ SetErrorCode													  [public]
// ---------------------------------------------------------------------------

void
LException::SetErrorCode(
	SInt32	inErrorCode)
{
	mErrorCode = inErrorCode;
}


// ---------------------------------------------------------------------------
//	¥ GetErrorString										 [inline] [public]
// ---------------------------------------------------------------------------
//	Returns pointer to error string for this LException

#pragma mark LException::GetErrorString

//	ConstStringPtr
//	LException::GetErrorString() const			// Defined inline


// ---------------------------------------------------------------------------
//	¥ SetErrorString												  [public]
// ---------------------------------------------------------------------------
//	Sets the text of the error string

void
LException::SetErrorString(
	ConstStringPtr	inErrorString)
{
	mErrorString[0] = 0;				// Initialize to empty string

	if (inErrorString != nil) {			// Copy input string if it exists
		::BlockMoveData(inErrorString, mErrorString, inErrorString[0] + 1);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Throw													 [static] [public]
// ---------------------------------------------------------------------------
//	Throw LExcpetion object. See comments at top of file about stack space.

void
LException::Throw(
	SInt32	inErr)
{
	throw LException(inErr);
}


// ---------------------------------------------------------------------------
//	¥ Throw													 [static] [public]
// ---------------------------------------------------------------------------
//	Throw LExcpetion object. See comments at top of file about stack space.

void
LException::Throw(
	SInt32			inErr,
	ConstStringPtr	inErrStr)
{
	throw LException(inErr, inErrStr);
}


PP_End_Namespace_PowerPlant
