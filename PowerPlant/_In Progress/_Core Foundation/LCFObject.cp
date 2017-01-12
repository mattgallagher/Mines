// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFObject.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFObject.h>

#include <CFPropertyList.h>

// ---------------------------------------------------------------------------
//	¥ LCFObject								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCFObject::LCFObject()
{
	mTypeRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ LCFObject								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing CF object reference
//
//	We retain the CF object. Caller should release the CF object if it
//	no longer needs it.

LCFObject::LCFObject(
	CFTypeRef	inTypeRef)
{
	mTypeRef = ::CFRetain(inTypeRef);
}


// ---------------------------------------------------------------------------
//	¥ ~LCFObject							Destructor				  [public]
// ---------------------------------------------------------------------------

LCFObject::~LCFObject()
{
	if (mTypeRef != nil) {
		::CFRelease(mTypeRef);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdoptTypeRef													  [public]
// ---------------------------------------------------------------------------
//	Specify a new TypeRef for this object
//
//	Retains input TypeRef and releases old TypeRef

void
LCFObject::AdoptTypeRef(
	CFTypeRef	inTypeRef)
{
	if (inTypeRef != mTypeRef) {		// Do nothing if same TypeRef
	
		if (inTypeRef != nil) {			// Retain new TypeRef
			::CFRetain(inTypeRef);
		}

		if (mTypeRef != nil) {			// Release old TypeRef
			::CFRelease(mTypeRef);
		}

		mTypeRef = inTypeRef;
	}
}


// ---------------------------------------------------------------------------
//	¥ CreateXMLData													  [public]
// ---------------------------------------------------------------------------

CFDataRef
LCFObject::CreateXMLData(
	CFAllocatorRef	inAllocator) const
{
	return ::CFPropertyListCreateXMLData(inAllocator, mTypeRef);
}


// ---------------------------------------------------------------------------
//	¥ SetTypeRef												   [protected]
// ---------------------------------------------------------------------------
//	Specify the CF Type Ref for this object. The Type must already be
//	retained by the caller, which transfers retention to this class.
//
//	Subclasses which create a CF type will need to call this function.
//	A typical subclass will have constructors of the following form:
//
//		LCFSomeType::LCFSomeType( SomeDataType  inValue )
//		{
//			CFSomeTypeRef	ref = ::CFSomeTypeCreate(inValue);
//
//			if (ref == nil) throw someError;
//
//			SetTypeRef(ref);
//
//	We can't use the LCFObject constructor that takes a CFTypeRef because
//	the CF type gets created within the body of the subclass's constructor.

#pragma mark LCFObject::SetTypeRef
// Defined inline
