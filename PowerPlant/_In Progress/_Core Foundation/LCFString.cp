// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFString.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFString.h>


// ---------------------------------------------------------------------------
//	¥ LCFString								Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	Create a mutable CFString
//
//	inMaxLength of zero means an unlimited length

LCFString::LCFString(
	CFIndex			inMaxLength,
	CFAllocatorRef	inAllocator)
{
	mMutableStringRef = ::CFStringCreateMutable(inAllocator, inMaxLength);

	if (mMutableStringRef == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(mMutableStringRef);

	mMaxLength = inMaxLength;
}


// ---------------------------------------------------------------------------
//	¥ LCFString								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Create an immutable CFString from a Pascal String

LCFString::LCFString(
	ConstStringPtr		inPString,
	CFStringEncoding	inEncoding,
	CFAllocatorRef		inAllocator)
{
	CFStringRef	ref = ::CFStringCreateWithPascalString(
							inAllocator, inPString, inEncoding);

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);

	mMutableStringRef = nil;
	mMaxLength = GetLength();
}


// ---------------------------------------------------------------------------
//	¥ LCFString								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Create an immutable CFString from a C String

LCFString::LCFString(
	const char*			inCString,
	CFStringEncoding	inEncoding,
	CFAllocatorRef		inAllocator)
{
	CFStringRef	ref = ::CFStringCreateWithCString(
							inAllocator, inCString, inEncoding);

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);

	mMutableStringRef = nil;
	mMaxLength = GetLength();
}


// ---------------------------------------------------------------------------
//	¥ LCFString								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Create an immutable LCFString from an existing CFString
//
//	This object retains the CFString. Caller should release it if necessary.

LCFString::LCFString(
	CFStringRef	inStringRef)

	: LCFObject(inStringRef)
{
	mMutableStringRef = nil;
	mMaxLength = GetLength();
}


// ---------------------------------------------------------------------------
//	¥ LCFString								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Create a mutable LCFString from an existing mutable CFString
//
//	This object retains the CFString. Caller should release it if necessary.

LCFString::LCFString(
	CFMutableStringRef	inStringRef)

	: LCFObject(inStringRef)
{
	mMutableStringRef = inStringRef;
	mMaxLength = 0;		// ??? No way to get this info
}


// ---------------------------------------------------------------------------
//	¥ LCFString								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCFString::LCFString(
	const LCFString&	inSource)
{
	if (inSource.mMutableStringRef != nil) {

		mMutableStringRef = ::CFStringCreateMutableCopy(
								inSource.GetAllocator(),
								inSource.mMaxLength,
								inSource.mMutableStringRef);

		if (mMutableStringRef == nil) {
			throw cf_bad_create();
		}
		
		SetTypeRef(mMutableStringRef);

	} else {

		CFStringRef	ref = ::CFStringCreateCopy(
								inSource.GetAllocator(),
								inSource.GetTypeRef());

		if (ref == nil) {
			throw cf_bad_create();
		}

		SetTypeRef(ref);

		mMutableStringRef = nil;
	}

	mMaxLength = inSource.mMaxLength;
}


// ---------------------------------------------------------------------------
//	operator =								Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LCFString&
LCFString::operator = (
	const LCFString&	inSource)
{
	LCFString	theCopy(inSource);		// Make stack-based copy

	CFStringRef			copyRef			= theCopy.GetTypeRef();
	CFMutableStringRef	copyMutableRef	= theCopy.mMutableStringRef;
	CFIndex				copyMaxLength	= theCopy.mMaxLength;

										// Copy didn't throw. Swap values
										// of "this" string with the copy
	theCopy.SetTypeRef( GetTypeRef() );
	theCopy.mMutableStringRef	= mMutableStringRef;
	theCopy.mMaxLength			= mMaxLength;

	SetTypeRef(copyRef);
	mMutableStringRef	= copyMutableRef;
	mMaxLength			= copyMaxLength;

										// Now "this" object is a copy of
										// the source. theCopy holds "this"
										// object's original data, which will
										// be destroyed by the destructor
										// for the stack-based theCopy object.
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFString							Destructor				  [public]
// ---------------------------------------------------------------------------

LCFString::~LCFString()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AdoptTypeRef													  [public]
// ---------------------------------------------------------------------------
//	Specify a new TypeRef for this object
//
//	This is an override of the LCFObject function.
//
//	??? There doesn't seem to be a way to distinguish between a mutable
//	and immutable CF Object given just a generic CFTypeRef. For safety,
//	we assume it's immutable.

void
LCFString::AdoptTypeRef(
	CFTypeRef	inTypeRef)
{
									// Input Type must be nil or a
									//   CFString
	if ( (inTypeRef == nil)  or
		 (::CFGetTypeID(inTypeRef) == ::CFStringGetTypeID()) ) {

		LCFObject::AdoptTypeRef(inTypeRef);
	
		mMutableStringRef = nil;	// Assume string is immutable
	
	} else {
		throw cf_bad_type();		// TypeRef is not a CFStringRef
	}
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------
//	Specify a new immutable CFString for this object
//
//	Retains input CFString and releases old CFString

void
LCFString::Adopt(
	CFStringRef	inStringRef)
{
	LCFObject::AdoptTypeRef(inStringRef);
	
	mMutableStringRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------
//	Specify a new mutable CFString for this object
//
//	Retains input CFString and releases old CFString

void
LCFString::Adopt(
	CFMutableStringRef	inMutableStringRef)
{
	LCFObject::AdoptTypeRef(inMutableStringRef);
	
	mMutableStringRef = inMutableStringRef;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Append														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Append(
	CFStringRef	inString)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringAppend(mMutableStringRef, inString);
}


// ---------------------------------------------------------------------------
//	¥ Append														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Append(
	const UniChar*	inChars,
	CFIndex			inNumChars)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringAppendCharacters(mMutableStringRef, inChars, inNumChars);
}


// ---------------------------------------------------------------------------
//	¥ Append														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Append(
	ConstStringPtr		inPString,
	CFStringEncoding	inEncoding)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringAppendPascalString(mMutableStringRef, inPString, inEncoding);
}


// ---------------------------------------------------------------------------
//	¥ Append														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Append(
	const char*			inCString,
	CFStringEncoding	inEncoding)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringAppendCString(mMutableStringRef, inCString, inEncoding);
}


// ---------------------------------------------------------------------------
//	¥ Insert														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Insert(
	CFIndex		inLocation,
	CFStringRef	inString)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringInsert(mMutableStringRef, inLocation, inString);
}


// ---------------------------------------------------------------------------
//	¥ Delete														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Delete(
	CFRange		inRange)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringDelete(mMutableStringRef, inRange);
}


// ---------------------------------------------------------------------------
//	¥ Replace														  [public]
// ---------------------------------------------------------------------------

void
LCFString::Replace(
	CFRange			inRange,
	CFStringRef		inReplacementStr)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringReplace(mMutableStringRef, inRange, inReplacementStr);
}


// ---------------------------------------------------------------------------
//	¥ ReplaceAll													  [public]
// ---------------------------------------------------------------------------

void
LCFString::ReplaceAll(
	CFStringRef		inReplacementStr)
{
	if (mMutableStringRef == nil) {
		throw cf_bad_change();			// String is not mutable
	}

	::CFStringReplaceAll(mMutableStringRef, inReplacementStr);
}

