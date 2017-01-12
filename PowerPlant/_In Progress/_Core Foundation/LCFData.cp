// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFData.cp					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFData.h>


// ---------------------------------------------------------------------------
//	¥ LCFData								Default Constructor		  [public]
// ---------------------------------------------------------------------------
//	Construct a mutable CFData block with the specified capacity

LCFData::LCFData(
	CFIndex			inCapacity,
	CFAllocatorRef	inAllocator)
{
	mMutableDataRef = ::CFDataCreateMutable(inAllocator, inCapacity);

	if (mMutableDataRef == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(mMutableDataRef);
}


// ---------------------------------------------------------------------------
//	¥ LCFData								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct an immutable CFData block with the specified data

LCFData::LCFData(
	const UInt8*	inBytes,
	CFIndex			inLength,
	CFAllocatorRef	inAllocator)
{
	CFDataRef	ref = ::CFDataCreate(inAllocator, inBytes, inLength);

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
	mMutableDataRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ LCFData								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing immutable DataRef

LCFData::LCFData(
	CFDataRef	inDataRef)

	: LCFObject(inDataRef)
{
	mMutableDataRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ LCFData								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing mutable DataRef

LCFData::LCFData(
	CFMutableDataRef	inDataRef)

	: LCFObject(inDataRef)
{
	mMutableDataRef = inDataRef;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFData								Destructor				  [public]
// ---------------------------------------------------------------------------

LCFData::~LCFData()
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
LCFData::AdoptTypeRef(
	CFTypeRef	inTypeRef)
{
									// Input Type must be nil or a
									//   CFDataRef
	if ( (inTypeRef == nil)  or
		 (::CFGetTypeID(inTypeRef) == ::CFDataGetTypeID()) ) {

		LCFObject::AdoptTypeRef(inTypeRef);
	
		mMutableDataRef = nil;		// Assume data is immutable
	
	} else {
		throw cf_bad_type();		// Type is not a CFDataRef
	}
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------
//	Specify a new immutable CFData for this object
//
//	Retains input CFData and releases old CFData

void
LCFData::Adopt(
	CFDataRef	inDataRef)
{
	LCFObject::AdoptTypeRef(inDataRef);
	
	mMutableDataRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------
//	Specify a new mutable CFData for this object
//
//	Retains input CFData and releases old CFData

void
LCFData::Adopt(
	CFMutableDataRef	inMutableDataRef)
{
	LCFObject::AdoptTypeRef(inMutableDataRef);
	
	mMutableDataRef = inMutableDataRef;
}

#pragma mark -

UInt8*
LCFData::GetMutableBytePtr() const
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	return ::CFDataGetMutableBytePtr(mMutableDataRef);
}


void
LCFData::SetLength(
	CFIndex	inNewLength)
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	::CFDataSetLength(mMutableDataRef, inNewLength);
}


void
LCFData::IncreaseLength(
	CFIndex	inExtraLength)
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	::CFDataIncreaseLength(mMutableDataRef, inExtraLength);
}


void
LCFData::AppendBytes(
	const UInt8*	inBytes,
	CFIndex			inLength)
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	::CFDataAppendBytes(mMutableDataRef, inBytes, inLength);
}


void
LCFData::ReplaceBytes(
	CFRange			inRange,
	const UInt8*	inNewBytes,
	CFIndex			inNewLength)
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	::CFDataReplaceBytes(mMutableDataRef, inRange, inNewBytes, inNewLength);
}


void
LCFData::DeleteBytes(
	CFRange			inRange)
{
	if (mMutableDataRef == nil) {
		throw cf_bad_change();		// Data is not mutable
	}	

	::CFDataDeleteBytes(mMutableDataRef, inRange);
}
