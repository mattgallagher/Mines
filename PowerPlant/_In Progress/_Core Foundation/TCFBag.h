// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TCFBag.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//	Template class wrapper for CFBag, an unordered collection of items
//	where duplicates are allowed
//
//	Items within a CFBag must be the size of a pointer value

#ifndef _H_TCFBag
#define _H_TCFBag

#include <LCFObject.h>
#include <CFBag.h>


template <class T> class TCFBag : public LCFObject {
public:

// ---------------------------------------------------------------------------
//	¥ TCFBag								Default Constructor
// ---------------------------------------------------------------------------
//	Create an empty mutable Bag
//
//	NOTE: Pass &kCFTypeBagCallBacks for inCallBacks if the items you are
//	storing in the Bag are all CFTypes

TCFBag(
	CFIndex					inCapacity = 0,
	const CFBagCallBacks*	inCallBacks = nil,
	CFAllocatorRef			inAllocator = nil)
{
	mMutableBagRef = ::CFBagCreateMutable(
							inAllocator, inCallBacks, inCapacity);

	if (mMutableBagRef == nil) {
		// throw std::bad_alloc
	};

	SetTypeRef(mMutableBagRef);

	mCapacity = inCapacity;
};


// ---------------------------------------------------------------------------
//	¥ TCFBag								Constructor
// ---------------------------------------------------------------------------
//	Create an immutable Bag containing the specified items
//
//	NOTE: Pass &kCFTypeBagCallBacks for inCallBacks if the items you are
//	storing in the Bag are all CFTypes

TCFBag(
	const void**			inValues,
	CFIndex					inNumValues,
	const CFBagCallBacks*	inCallBacks = nil,
	CFAllocatorRef			inAllocator = nil)
{
	CFBagRef	ref = ::CFBagCreate(
							inAllocator, inValues, inNumValues, inCallBacks);

	if (ref == nil) {
		// throw std::bad_alloc
	}

	SetTypeRef(ref);

	mMutableBagRef	= nil;
	mCapacity		= inNumValues;
}


// ---------------------------------------------------------------------------
//	¥ TCFBag								Copy Constructor
// ---------------------------------------------------------------------------

TCFBag(
	const TCFBag&	inSourceBag)
{
	if (inSourceBag.mMutableBagRef != nil) {

		mMutableBagRef = ::CFBarCreateMutableCopy(
								inSourceBag.GetAllocator(),
								inSourceBag.mCapacity,
								inSourceBag.mMutableBagRef);

		if (mMutableBagRef == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(mMutableArrayRef);

	} else {

		CFBagRef	ref = ::CFBagCreateCopy(
								inSourceBag.GetAllocator(),
								inSourceBag.GetTypeRef());

		if (ref == nil) {
			// throw std::bad_alloc
		}

		SetTypeRef(ref);
		mMutableBagRef = nil;
	}

	mCapacity = inSourceBag.mCapacity;
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator
// ---------------------------------------------------------------------------

TCFBag&
operator = (
	const TCFBag&	inSoureBag)
{
	TCFBag<T>	theCopy(inSoureBag);	// Make stack-based copy

										// Swap contents of "this" Bag
										//   and the copy
	CFBagRef			copyRef			= theCopy.GetTypeRef();
	CFMutableBagRef		copyMutableRef	= theCopy.mMutableBagRef;
	CFIndex				copyCapacity	= theCopy.mCapacity;

	theCopy.SetTypeRef( GetTypeRef() );
	theCopy.mMutableBagRef	= mMutableBagRef;
	theCopy.mCapacity		= mCapacity;

	SetTypeRef(copyRef);
	mMutableBagRef	= copyMutableRef;
	mCapacity		= copyCapacity;

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~TCFBag								Destructor
// ---------------------------------------------------------------------------

~TCFBag()
{
}


// ---------------------------------------------------------------------------
//	¥ operator TCFBag						Conversion Operator
// ---------------------------------------------------------------------------

operator TCFBag() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef
// ---------------------------------------------------------------------------

TCFBag
GetTypeRef() const
{
	return reinterpret_cast<CFBagRef>( LCFObject::GetTypeRef() );
}


// ---------------------------------------------------------------------------
//	¥ GetCount
// ---------------------------------------------------------------------------
//	Return the number of items in the Bag

CFIndex
GetCount() const
{
	return ::CFBagGetCount(GetTypeRef());
}


// ---------------------------------------------------------------------------
//	¥ GetCountOfValue
// ---------------------------------------------------------------------------

CFIndex
GetCountOfValue(
	T	inValue) const
{
	return ::CFBagGetCountOfValue(GetTypeRef(), inValue);
}


// ---------------------------------------------------------------------------
//	¥ ContainsValue
// ---------------------------------------------------------------------------

bool
ContainsValue(
	T	inValue) const
{
	return ::CFBagContainsValue(GetTypeRef(), inValue);
}


// ---------------------------------------------------------------------------
//	¥ GetValue
// ---------------------------------------------------------------------------

T
GetValue(
	T		inCandidate) const
{
	return static_cast<T>(::CFBagGetValue(GetTypeRef(), inCandidate));
}


// ---------------------------------------------------------------------------
//	¥ GetValueIfPresent
// ---------------------------------------------------------------------------

bool
GetValueIfPresent(
	T		inCandidate,
	T&		outValue) const
{
	return ::CFBagGetValueIfPresent(GetTypeRef(), inCandidate, &outValue);
}


// ---------------------------------------------------------------------------
//	¥ GetValues
// ---------------------------------------------------------------------------
//	Pass back all values in the Bag.
//
//	outValues must point to a buffer large enough to hold all the items

void
GetValue(
	T*		outValues) const
{
	::CFBagGetValues(GetTypeRef(), outValues);
}


// ---------------------------------------------------------------------------
//	¥ ApplyFunction
// ---------------------------------------------------------------------------

void
ApplyFunction(
	CFBagApplierFunction	inFunction,
	void*					inParam) const
{
	::CFBagApplyFunction(GetTypeRef(), inFunction, inParam);
}


// ---------------------------------------------------------------------------
//	¥ AddValue
// ---------------------------------------------------------------------------

void
AddValue(
	T		inValue)
{
	if (mMutableBagRef != nil) {
		::CFBagAddValue(mMutableBagRef, inValue);

	} else {
		// Error
	}
}


// ---------------------------------------------------------------------------
//	¥ ReplaceValue
// ---------------------------------------------------------------------------
//	??? CFBagReplaceValue() takes only one value parameter
//		Shouldn't there be two values, the new and old one?

void
ReplaceValue(
	T		inValue)
{
	if (mMutableBagRef != nil) {
		::CFBagReplaceValue(mMutableBagRef, inValue);

	} else {
		// Error
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------

void
SetValue(
	T		inValue)
{
	if (mMutableBagRef != nil) {
		::CFBagSetValue(mMutableBagRef, inValue);

	} else {
		// Error
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveValue
// ---------------------------------------------------------------------------

void
RemoveValue(
	T		inValue)
{
	if (mMutableBagRef != nil) {
		::CFBagRemoveValue(mMutableBagRef, inValue);

	} else {
		// Error
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveAllValues
// ---------------------------------------------------------------------------

void
RemoveAllValues(
	T		inValue)
{
	if (mMutableBagRef != nil) {
		::CFBagRemoveAllValues(mMutableBagRef);

	} else {
		// Error
	}
}


// ---------------------------------------------------------------------------
//	Instance Variables
// ---------------------------------------------------------------------------

private:
	CFMutableBagRef		mMutableBagRef;
	CFIndex				mCapacity;
};

#endif
