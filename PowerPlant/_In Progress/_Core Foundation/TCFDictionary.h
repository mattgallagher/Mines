// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TCFDictionary.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//	Template class wrapper for CFDictionary, a collection of
//	(Key, Value) pairs
//
//	Both the Key and Value types for CFDictionary must be the size of a
//	pointer value

#ifndef _H_TCFDictionary
#define _H_TCFDictionary

#include <LCFObject.h>
#include <CFDictionary.h>


template <class TKey, class TValue> class TCFDictionary : public LCFObject {
public:

// ---------------------------------------------------------------------------
//	¥ TCFDictionary							Default Constructor
// ---------------------------------------------------------------------------
//	Create an empty mutable dictionary
//
//	NOTE: Pass &kCFTypeDictionaryKeyCallBacks for inKeyCallBacks if all the
//	Dictionary Keys are CFTypes. Pass &kCFTypeDictionaryValueCallBacks for
//	in ValueCallBacks if all the Dictionary Values are CFTypes.

TCFDictionary(
	CFIndex								inCapacity = 0,
	const CFDictionaryKeyCallBacks*		inKeyCallBacks = nil,
	const CFDictionaryValueCallBacks*	inValueCallBacks = nil,
	CFAllocatorRef						inAllocator = nil)
{
	mMutableDictionaryRef = ::CFDictionaryCreateMutable(
									inAllocator,
									inCapacity,
									inKeyCallBacks,
									inValueCallBacks);

	if (mMutableDictionaryRef == nil) {
		// throw std:bad_alloc
	}

	SetTypeRef(mMutableDictionaryRef);

	mCapacity = inCapacity;
}


// ---------------------------------------------------------------------------
//	¥ TCFDictionary							Constructor
// ---------------------------------------------------------------------------
//	Create on immutable dictionary with the specified entries
//
//	NOTE: Pass &kCFTypeDictionaryKeyCallBacks for inKeyCallBacks if all the
//	Dictionary Keys are CFTypes. Pass &kCFTypeDictionaryValueCallBacks for
//	in ValueCallBacks if all the Dictionary Values are CFTypes.

TCFDictionary(
	const TKey*							inKeys,
	const TValue*						inValues,
	CFIndex								inNumValues,
	const CFDictionaryKeyCallBacks*		inKeyCallBacks = nil,
	const CFDictionaryValueCallBacks*	inValueCallBacks = nil,
	CFAllocatorRef						inAllocator = nil)
{
	CFDictionaryRef	ref = ::CFDictionaryCreate(
								inAllocator,
								inKeys, inValues, inNumValues,
								inKeyCallBacks,
								inValueCallBacks);

	if (ref == nil) {
		// throw std::bad_alloc
	}

	SetTypeRef(ref);

	mMutableDictionaryRef	= nil;
	mCapacity				= inNumValues;
}


// ---------------------------------------------------------------------------
//	¥ TCFDictionary							Constructor
// ---------------------------------------------------------------------------
//	Construct on immutable dictionary from an existing CFDictionary

TCFDictionary(
	CFDictionaryRef	inDictionaryRef)
	
	: LCFObject(inDictionaryRef)
{
	mMutableDictionaryRef	= nil;
	mCapacity				= 0;
}


// ---------------------------------------------------------------------------
//	¥ TCFDictionary							Constructor
// ---------------------------------------------------------------------------
//	Construct o mutable dictionary from an existing mutable CFDictionary

TCFDictionary(
	CFMutableDictionaryRef	inDictionaryRef)
	
	: LCFObject(inDictionaryRef)
{
	mMutableDictionaryRef	= inDictionaryRef;
	mCapacity				= 0;
}


// ---------------------------------------------------------------------------
//	¥ TCFDictionary							Copy Constructor
// ---------------------------------------------------------------------------

TCFDictionary(
	const TCFDictionary&	inSourceDictionary)
{
	if (inSourceDictionary.mMutableDictionaryRef != nil) {

		mMutableDictionaryRef = ::CFDictionaryCreateMutableCopy(
									inSourceDictionary.GetAllocator(),
									inSourceDictionary.mCapacity,
									inSourceDictionary.mMutableDictionaryRef);

		if (mMutableDictionaryRef == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(mMutableDictionaryRef);

	} else {

		CFDictionaryRef	ref = ::CFDictionaryCreateCopy(
									inSourceDictionary.GetAllocator(),
									inSourceDictionary.GetTypeRef());

		if (ref == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(ref);
		mMutableDictionaryRef = nil;
	}

	mCapacity = inSourceDictionary.mCapacity;
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator
// ---------------------------------------------------------------------------

TCFDictionary&
operator = (
	const TCFDictionary&	inSourceDictionary)
{
										// Try to make a copy of the source
	TCFDictionary<TKey, TValue>	theCopy(inSourceDictionary);

										// Copy didn't throw. Swap values
										// of "this" dictionary with the copy
	CFDictionaryRef			copyRef				= theCopy.GetTypeRef();
	CFMutableDictionaryRef	copyMutableRef		= theCopy.mMutableDictionaryRef;
	CFIndex					copyCapacity		= theCopy.mCapacity;

	theCopy.SetTypeRef( GetTypeRef() );
	theCopy.mMutableDictionaryRef	= mMutableDictionaryRef;
	theCopy.mCapacity				= mCapacity;

	SetTypeRef(copyRef);
	mMutableDictionaryRef	= copyMutableRef;
	mCapacity				= copyCapacity;

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~TCFDictionary						Destructor
// ---------------------------------------------------------------------------

~TCFDictionary()
{
}


// ---------------------------------------------------------------------------
//	¥ operator CFDictionaryRef				Conversion Operator
// ---------------------------------------------------------------------------

operator CFDictionaryRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef
// ---------------------------------------------------------------------------

CFDictionaryRef
GetTypeRef() const
{
	return reinterpret_cast<CFDictionaryRef>( LCFObject::GetTypeRef() );
}


// ---------------------------------------------------------------------------
//	¥ AdoptTypeRef
// ---------------------------------------------------------------------------
//	Specify a new TypeRef for this object
//
//	This is an override of the LCFObject function.
//
//	??? There doesn't seem to be a way to distinguish between a mutable
//	and immutable CF Object given just a generic CFTypeRef. For safety,
//	we assume it's immutable.

void
AdoptTypeRef(
	CFTypeRef	inTypeRef)
{
									// Input Type must be nil or a
									//   CFDictionary
	if ( (inTypeRef == nil)  or
		 (::CFGetTypeID(inTypeRef) == ::CFArrayGetTypeID()) ) {

		LCFObject::AdoptTypeRef(inTypeRef);
	
		mMutableDictionaryRef = nil;	// Assume dictionary is immutable
	
	} else {
		// Error: TypeRef is not a CFDictionaryRef
	}
}


// ---------------------------------------------------------------------------
//	¥ Adopt
// ---------------------------------------------------------------------------
//	Specify a new immutable CFDictionary for this object
//
//	Retains input CFDictionary and releases old CFDictionary

void
Adopt(
	CFDictionaryRef	inDictionaryRef)
{
	LCFObject::AdoptTypeRef(inDictionaryRef);
	
	mMutableDictionaryRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ Adopt
// ---------------------------------------------------------------------------
//	Specify a new mutable CFDictionary for this object
//
//	Retains input CFDictionary and releases old CFDictionary

void
Adopt(
	CFMutableDictionaryRef	inMutableDictionaryRef)
{
	LCFObject::AdoptTypeRef(inMutableDictionaryRef);
	
	mMutableDictionaryRef = inMutableDictionaryRef;
}


// ---------------------------------------------------------------------------
//	¥ IsMutable
// ---------------------------------------------------------------------------
//	Return whether or not the Dictionary can be changed

bool
IsMutable() const
{
	return (mMutableDictionaryRef != nil);
}


// ---------------------------------------------------------------------------
//	¥ GetCount
// ---------------------------------------------------------------------------
//	Return the number of items in the Dictionary

CFIndex
GetCount() const
{
	return ::CFDictionaryGetCount(GetTypeRef());
}


// ---------------------------------------------------------------------------
//	¥ GetCountOfKey
// ---------------------------------------------------------------------------

CFIndex
GetCountOfKey(
	TKey	inKey) const
{
	return ::CFDictionaryGetCountOfKey(GetTypeRef(), inKey);
}


// ---------------------------------------------------------------------------
//	¥ GetCountOfValue
// ---------------------------------------------------------------------------

CFIndex
GetCountOfValue(
	TValue	inValue) const
{
	return ::CFDictionaryGetCountOfValue(GetTypeRef(), inValue);
}


// ---------------------------------------------------------------------------
//	¥ ContainsKey
// ---------------------------------------------------------------------------

bool
ContainsKey(
	TKey	inKey) const
{
	return ::CFDictionaryContainsKey(GetTypeRef(), inKey);
}


// ---------------------------------------------------------------------------
//	¥ ContainsValue
// ---------------------------------------------------------------------------

bool
ContainsValue(
	TValue	inValue) const
{
	return ::CFDictionaryContainsValue(GetTypeRef(), inValue);
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------

TValue
GetValue(
	TKey	inKey) const
{
	return reinterpret_cast<TValue>(::CFDictionaryGetValue(GetTypeRef(), inKey));
}


// ---------------------------------------------------------------------------
//	¥ operator []
// ---------------------------------------------------------------------------

TValue
operator [] (
	TKey	inKey) const
{
	return GetValue(inKey);
}


// ---------------------------------------------------------------------------
//	¥ GetValueIfPresent
// ---------------------------------------------------------------------------

bool
GetValueIfPresent(
	TKey		inKey,
	TValue&		outValue) const
{
	return ::CFDictionaryGetValueIfPresent(GetTypeRef(), inKey, &outValue);
}


// ---------------------------------------------------------------------------
//	¥ GetKeysAndValues
// ---------------------------------------------------------------------------
//	Pass back parallel arrays of keys and values
//
//	outKeys and outValues must be pointers to arrays with enough space to
//	hold all dictionary entries. Call GetCount() to determine this size.

void
GetKeysAndValues(
	TKey*		outKeys,
	TValue*		outValues) const
{
	::CFDictionaryGetKeysAndValues(GetTypeRef(), outKeys, outValues);
}


// ---------------------------------------------------------------------------
//	¥ ApplyFunction
// ---------------------------------------------------------------------------

void
ApplyFunction(
	CFDictionaryApplierFunction		inFunction,
	void*							inParam) const
{
	::CFDictionaryApplyFunction(GetTypeRef(), inFunction, inParam);
}


// ---------------------------------------------------------------------------
//	¥ AddValue
// ---------------------------------------------------------------------------

void
AddValue(
	TKey	inKey,
	TValue	inValue)
{
	if (mMutableDictionaryRef != nil) {
		::CFDictionaryAddValue(mMutableDictionaryRef, inKey, inValue);

	} else {
		// Error: Can't add a value to an immutable Dictionary
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------

void
SetValue(
	TKey	inKey,
	TValue	inValue)
{
	if (mMutableDictionaryRef != nil) {
		::CFDictionarySetValue(mMutableDictionaryRef, inKey, inValue);

	} else {
		// Error: Can't set value in an immutable Dictionary
	}
}


// ---------------------------------------------------------------------------
//	¥ ReplaceValue
// ---------------------------------------------------------------------------

void
ReplaceValue(
	TKey	inKey,
	TValue	inValue)
{
	if (mMutableDictionaryRef != nil) {
		::CFDictionaryReplaceValue(mMutableDictionaryRef, inKey, inValue);

	} else {
		// Error: Can't replace value in an immutable Dictionary
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveValue
// ---------------------------------------------------------------------------

void
RemoveValue(
	TKey	inKey)
{
	if (mMutableDictionaryRef != nil) {
		::CFDictionaryRemoveValue(mMutableDictionaryRef, inKey);

	} else {
		// Error: Can't remove value from an immutable Dictionary
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllValues
// ---------------------------------------------------------------------------

void
RemoveAllValues()
{
	if (mMutableDictionaryRef != nil) {
		::CFDictionaryRemoveAllValues(mMutableDictionaryRef);

	} else {
		// Error: Can't remove all values from an immutable Dictionary
	}
}


// ---------------------------------------------------------------------------
//	Instance Variables
// ---------------------------------------------------------------------------
private:
	CFMutableDictionaryRef	mMutableDictionaryRef;
	CFIndex					mCapacity;
};

#endif
