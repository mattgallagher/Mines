// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFNumber.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFNumber.h>


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a char

LCFNumber::LCFNumber(
	char			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberCharType,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an 8-bit integer

LCFNumber::LCFNumber(
	SInt8			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberSInt8Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a 16-bit integer

LCFNumber::LCFNumber(
	SInt16			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberSInt16Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a 32-bit integer

LCFNumber::LCFNumber(
	SInt32			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberSInt32Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a 64-bit integer

LCFNumber::LCFNumber(
	SInt64			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberSInt64Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a 32-bit floating point number

LCFNumber::LCFNumber(
	Float32			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberFloat32Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a 64-bit floating point number

LCFNumber::LCFNumber(
	Float64			inNumber,
	CFAllocatorRef	inAllocator)
{
	CFNumberRef		ref = ::CFNumberCreate(	inAllocator,
											kCFNumberFloat64Type,
											&inNumber );

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	LCFNumber								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing CFNumberRef
//
//	We retain the CFNumberRef. Caller should release the CFNumberRef if it
//	no longer needs it.

LCFNumber::LCFNumber(
	CFNumberRef		inNumberRef)

	: LCFObject(inNumberRef)
{
}


// ---------------------------------------------------------------------------
//	¥ LCFNumber								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCFNumber::LCFNumber(
	const LCFNumber&	inSource)
{
										// Get specifications of source number
	CFAllocatorRef	allocator	= inSource.GetAllocator();
	CFNumberType	numberType	= inSource.GetNumberType();
	CFIndex			numberSize	= inSource.GetByteSize();
	char*			valuePtr	= new char[numberSize];

	inSource.GetValueAs(numberType, valuePtr);

										// Use specs to make a copy
	CFNumberRef	ref = ::CFNumberCreate(allocator, numberType, valuePtr);

	delete[] valuePtr;

	if (ref == nil) {
		throw cf_bad_create();
	}

	SetTypeRef(ref);
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LCFNumber&
LCFNumber::operator = (
	const LCFNumber&	inSource)
{
	LCFNumber	theCopy(inSource);		// Make stack-based copy

										// Swap TypeRef's of "this" and "copy"
	CFNumberRef	copyRef = theCopy.GetTypeRef();

	theCopy.SetTypeRef( GetTypeRef() );

	SetTypeRef(copyRef);

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFNumber							Destructor				  [public]
// ---------------------------------------------------------------------------

LCFNumber::~LCFNumber()
{
}
