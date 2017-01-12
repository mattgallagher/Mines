// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TCFArray.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//	Template class wrapper for CFArray, an ordered collection of items
//
//	Items within a CFArray must be the size of a pointer value

#ifndef _H_TCFArray
#define _H_TCFArray

#include <LCFObject.h>
#include <CFArray.h>


template <class T> class TCFArray : public LCFObject {
public:

// ---------------------------------------------------------------------------
//	¥ TCFArray								Default Constructor
// ---------------------------------------------------------------------------
//	Create a mutable array
//
//	NOTE: Pass &kCFTypeArrayCallBacks for inCallBacks if the items you
//	are storing in the array are all CFTypes

TCFArray(
	CFIndex					inCapacity = 0,
	const CFArrayCallBacks*	inCallBacks = nil,
	CFAllocatorRef			inAllocator = nil)
{
	if (sizeof(T) != sizeof(void*)) {
		throw 1;
	}

	mMutableArrayRef = ::CFArrayCreateMutable(
								inAllocator, inCapacity, inCallBacks);

	if (mMutableArrayRef == nil) {
		// throw std::bad_alloc;
	}

	SetTypeRef(mMutableArrayRef);

	mCapacity = inCapacity;
}


// ---------------------------------------------------------------------------
//	¥ TCFArray								Constructor
// ---------------------------------------------------------------------------
//	Create an immutable array of the specified values
//
//	NOTE: Pass &kCFTypeArrayCallBacks for inCallBacks if the items you
//	are storing in the array are all CFTypes

TCFArray(
	const T*				inValues,
	CFIndex					inNumValues,
	const CFArrayCallBacks*	inCallBacks = nil,
	CFAllocatorRef			inAllocator = nil)
{
	CFArrayRef	ref = ::CFArrayCreate(
						inAllocator, (const void**) inValues, inNumValues,
						inCallBacks);

	if (ref == nil) {
		// throw std::bad_alloc;
	}

	SetTypeRef(ref);

	mMutableArrayRef	= nil;
	mCapacity			= inNumValues;
}


// ---------------------------------------------------------------------------
//	¥ TCFArray								Copy Constructor
// ---------------------------------------------------------------------------

TCFArray(
	const TCFArray&		inSourceArray)
{
	if (inSourceArray.mMutableArrayRef != nil) {

		mMutableArrayRef = ::CFArrayCreateMutableCopy(
								inSourceArray.GetAllocator(),
								inSourceArray.mCapacity,	// Copy has same capacity
								inSourceArray.mMutableArrayRef);

		if (mMutableArrayRef == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(mMutableArrayRef);

	} else {

		CFArrayRef	ref = ::CFArrayCreateCopy(
								inSourceArray.GetAllocator(),
								inSourceArray.GetTypeRef());

		if (ref == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(ref);

		mMutableArrayRef = nil;
	}

	mCapacity = inSourceArray.mCapacity;
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator
// ---------------------------------------------------------------------------

TCFArray&
operator = (
	const TCFArray&		inSourceArray)
{
										// Try to make a copy of the source
	TCFArray<T>	theCopy(inSourceArray);

										// Copy didn't throw. Swap values
										// of "this" array with the copy
	CFArrayRef			copyRef				= theCopy.GetTypeRef();
	CFMutableArrayRef	copyMutableRef		= theCopy.mMutableArrayRef;
	CFIndex				copyCapacity		= theCopy.mCapacity;

	theCopy.SetTypeRef( GetTypeRef() );
	theCopy.mMutableArrayRef	= mMutableArrayRef;
	theCopy.mCapacity			= mCapacity;

	SetTypeRef(copyRef);
	mMutableArrayRef	= copyMutableRef;
	mCapacity			= copyCapacity;

										// Now "this" object is a copy of
										// the source. theCopy holds "this"
										// object's original data, which will
										// be destroyed by the destructor
										// for the stack-based theCopy object.

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~TCFArray								Destructor
// ---------------------------------------------------------------------------

~TCFArray()
{
}


// ---------------------------------------------------------------------------
//	¥ operator CFArrayRef					Conversion Operator
// ---------------------------------------------------------------------------

operator CFArrayRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef
// ---------------------------------------------------------------------------

CFArrayRef
GetTypeRef() const
{
	return reinterpret_cast<CFArrayRef>( LCFObject::GetTypeRef() );
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
									//   CFArray
	if ( (inTypeRef == nil)  or
		 (::CFGetTypeID(inTypeRef) == ::CFArrayGetTypeID()) ) {

		LCFObject::AdoptTypeRef(inTypeRef);
	
		mMutableArrayRef = nil;		// Assume array is immutable
	
	} else {
		// Error: TypeRef is not a CFArrayRef
	}
}


// ---------------------------------------------------------------------------
//	¥ Adopt
// ---------------------------------------------------------------------------
//	Specify a new immutable CFArray for this object
//
//	Retains input CFArray and releases old CFArray

void
Adopt(
	CFArrayRef	inArrayRef)
{
	LCFObject::AdoptTypeRef(inArrayRef);
	
	mMutableArrayRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ Adopt
// ---------------------------------------------------------------------------
//	Specify a new mutable CFArray for this object
//
//	Retains input CFArray and releases old CFArray

void
Adopt(
	CFMutableArrayRef	inMutableArrayRef)
{
	LCFObject::AdoptTypeRef(inMutableArrayRef);
	
	mMutableArrayRef = inMutableArrayRef;
}


// ---------------------------------------------------------------------------
//	¥ IsMutable
// ---------------------------------------------------------------------------
//	Return whether or not the Array can be changed

bool
IsMutable() const
{
	return (mMutableArrayRef != nil);
}


// ---------------------------------------------------------------------------
//	¥ GetCount
// ---------------------------------------------------------------------------
//	Return the number of items in the Array

CFIndex
GetCount() const
{
	return ::CFArrayGetCount(GetTypeRef());
}


// ---------------------------------------------------------------------------
//	¥ GetCountOfValue
// ---------------------------------------------------------------------------
//	Return the number of times a given value occurs in the Array

CFIndex
GetCountOfValue(
	T				inValue,
	const CFRange*	inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	return ::CFArrayGetCountOfValue(GetTypeRef(), range, inValue);
}


// ---------------------------------------------------------------------------
//	¥ ContainsValue
// ---------------------------------------------------------------------------
//	Return whether the Array contains a given value

bool
ContainsValue(
	T				inValue,
	const CFRange*	inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	return ::CFArrayContainsValue(GetTypeRef(), range, inValue);
}


// ---------------------------------------------------------------------------
//	¥ GetValueAt
// ---------------------------------------------------------------------------
//	Return the value at the given index in the Array

T
GetValueAt(
	CFIndex		inIndex) const
{
	return static_cast<T>(::CFArrayGetValueAtIndex(GetTypeRef(), inIndex));
}


// ---------------------------------------------------------------------------
//	¥ operator []
// ---------------------------------------------------------------------------
//	Returns value at the specified zero-based index

T
operator [] (
	CFIndex		inIndex) const
{
	return (T)(::CFArrayGetValueAtIndex(GetTypeRef(), inIndex));
}


// ---------------------------------------------------------------------------
//	¥ GetValues
// ---------------------------------------------------------------------------

void
GetValues(
	T*			outValues,
	CFRange*	inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	::CFArrayGetValues(GetTypeRef(), range, outValues);
}


// ---------------------------------------------------------------------------
//	¥ ApplyFunction
// ---------------------------------------------------------------------------

void
ApplyFunction(
	CFArrayApplierFunction	inFunction,
	void*					inParam,
	CFRange*				inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	::CFArrayApplyFunction(GetTypeRef(), range, inFunction, inParam);
}


// ---------------------------------------------------------------------------
//	¥ GetFirstIndexOf
// ---------------------------------------------------------------------------

CFIndex
GetFirstIndexOf(
	T			inValue,
	CFRange*	inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	return ::CFArrayGetFirstIndexOfValue(GetTypeRef(), range, inValue);
}


// ---------------------------------------------------------------------------
//	¥ GetLastIndexOf
// ---------------------------------------------------------------------------

CFIndex
GetLastIndexOf(
	T			inValue,
	CFRange*	inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	return ::CFArrayGetLastIndexOfValue(GetTypeRef(), range, inValue);
}


// ---------------------------------------------------------------------------
//	¥ BinarySearchFor
// ---------------------------------------------------------------------------

CFIndex
BinarySearchFor(
	T						inValue,
	CFComparatorFunction	inComparator,
	void*					inParam,
	CFRange*				inRange = nil) const
{
	CFRange		range;
	ValidateRange(inRange, &range);

	return ::CFArrayBSearchValues(GetTypeRef(), range, inValue, inComparator, inParam);
}


// ---------------------------------------------------------------------------
//	¥ Append
// ---------------------------------------------------------------------------

void
Append(
	T	inValue)
{
	if (mMutableArrayRef != nil) {
		::CFArrayAppendValue(mMutableArrayRef, inValue);

	} else {
		// Error: Can't append to an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertAt
// ---------------------------------------------------------------------------

void
InsertAt(
	CFIndex		inIndex,
	T			inValue)
{
	if (mMutableArrayRef != nil) {
		::CFArrayInsertValueAtIndex(mMutableArrayRef, inIndex, inValue);

	} else {
		// Error: Can't insert in an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValueAt
// ---------------------------------------------------------------------------

void
SetValueAt(
	CFIndex		inIndex,
	T			inValue)
{
	if (mMutableArrayRef != nil) {
		::CFArraySetValueAtIndex(mMutableArrayRef, inIndex, inValue);

	} else {
		// Error: Can't set value in an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveValueAt
// ---------------------------------------------------------------------------

void
RemoveValueAt(
	CFIndex		inIndex)
{
	if (mMutableArrayRef != nil) {
		::CFArrayRemoveValueAtIndex(mMutableArrayRef, inIndex);

	} else {
		// Error: Can't remove value from an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllValues
// ---------------------------------------------------------------------------

void
RemoveAllValues()
{
	if (mMutableArrayRef != nil) {
		::CFArrayRemoveAllValues(mMutableArrayRef);

	} else {
		// Error: Can't remove values from an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ ReplaceValues
// ---------------------------------------------------------------------------

void
ReplaceValues(
	T*			inNewValues,
	CFIndex		inCount,
	CFRange*	inRange)
{
	if (mMutableArrayRef != nil) {
		::CFArrayReplaceValues(mMutableArrayRef, *inRange, inNewValues, inCount);

	} else {
		// Error: Can't replace values in an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ ExchangeValuesAt
// ---------------------------------------------------------------------------

void
ExchangeValuesAt(
	CFIndex		inIndex1,
	CFIndex		inIndex2)
{
	if (mMutableArrayRef != nil) {
		::CFArrayExchangeValuesAtIndices(mMutableArrayRef, inIndex1, inIndex2);

	} else {
		// Error: Can't exchange values in an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ Sort
// ---------------------------------------------------------------------------

void
Sort(
	CFComparatorFunction	inComparator,
	void*					inParam,
	CFRange*				inRange = nil)
{
	if (mMutableArrayRef != nil) {
		CFRange		range;
		ValidateRange(inRange, &range);

		::CFArraySortValues(mMutableArrayRef, range, inComparator, inParam);

	} else {
		// Error: Can't sort an immutable array
	}
}


// ---------------------------------------------------------------------------
//	¥ ValidateRange
// ---------------------------------------------------------------------------
//	If inRange is nil, the copy is a range that includes the entire array.
//
//	If inRange is not nil, the location and length are checked for validity.
//	If valid, copy is a copy of the input range.
//	If not valid, function throws an std::out_of_range exception

void
ValidateRange(
	const CFRange*	inRange,
	CFRange*		outCopy)
{
	CFIndex		count = GetCount();

	if (inRange == nil) {
		outCopy->location = 0;
		outCopy->length   = count - 1;

	} else {
		if ( (inRange->location < 0)  ||
			 (inRange->location >= count ) ) {

			// Start location is not a valid index
			// throw std::out_of_range
		}

		if (inRange->length < 0) {

			// Negative length is illegal
			// throw std::out_of_range
		}

		if ( (inRange->location + inRange->length) > count ) {

			// Length is too big. Endpoint past end of array
			// throw std::out_of_range
		}

		*outCopy = *inRange;
	}
}


// ---------------------------------------------------------------------------
//	Instance Variables
// ---------------------------------------------------------------------------

private:
	CFMutableArrayRef	mMutableArrayRef;
	CFIndex				mCapacity;
};

#endif
