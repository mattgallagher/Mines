// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFObject.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper class for Core Foundation base functionality

#ifndef _H_LCFObject
#define _H_LCFObject
#pragma once

#include <CFBase.h>
#include <CFData.h>


// ---------------------------------------------------------------------------

class	LCFObject {
public:
					LCFObject();

					LCFObject( CFTypeRef inTypeRef );

	virtual			~LCFObject();

	CFTypeRef		GetTypeRef() const;

	CFTypeID		GetTypeID() const;

	CFStringRef		GetTypeIDDescription() const;

	CFStringRef		GetDescription() const;

	CFAllocatorRef	GetAllocator() const;

	CFHashCode		GetHashCode() const;

	CFIndex			GetRetainCount() const;

	virtual void	AdoptTypeRef( CFTypeRef inTypeRef );

	CFDataRef		CreateXMLData( CFAllocatorRef inAllocator = nil ) const;

protected:
	void			SetTypeRef( CFTypeRef inTypeRef );

private:
					LCFObject( const LCFObject& inSource );		// Undefined

	LCFObject&		operator = ( const LCFObject& inSource );	// Undefined

private:
	CFTypeRef		mTypeRef;
};


// ===========================================================================
//	Inline Functions
// ===========================================================================

inline
CFTypeRef
LCFObject::GetTypeRef() const
{
	return mTypeRef;
}


inline
CFTypeID
LCFObject::GetTypeID() const
{
	return ::CFGetTypeID(mTypeRef);
}


inline
CFStringRef
LCFObject::GetTypeIDDescription() const
{
	return ::CFCopyTypeIDDescription( GetTypeID() );
}


inline
CFStringRef
LCFObject::GetDescription() const
{
	return ::CFCopyDescription(mTypeRef);
}


inline
CFAllocatorRef
LCFObject::GetAllocator() const
{
	return ::CFGetAllocator(mTypeRef);
}


inline
CFHashCode
LCFObject::GetHashCode() const
{
	return ::CFHash(mTypeRef);
}


inline
CFIndex
LCFObject::GetRetainCount() const
{
	return ::CFGetRetainCount(mTypeRef);
}


inline
void
LCFObject::SetTypeRef(
	CFTypeRef	inTypeRef)
{
	mTypeRef = inTypeRef;
}


inline
bool
operator == (
	const LCFObject&	inLhs,
	const LCFObject&	inRhs)
{
	return ::CFEqual( inLhs.GetTypeRef(), inRhs.GetTypeRef() );
}

#pragma mark -
#pragma mark Class StCFRetainer
// ===========================================================================
//	StCFRetainer
// ===========================================================================

class	StCFRetainer {
public:
		StCFRetainer( CFTypeRef inTypeRef )
			{
				::CFRetain(inTypeRef);
				mTypeRef = inTypeRef;
			}

		StCFRetainer( const LCFObject& inObject )
			{
				mTypeRef = inObject.GetTypeRef();
				::CFRetain(mTypeRef);
			}

		~StCFRetainer()
			{
				::CFRelease(mTypeRef);
			}

private:
	CFTypeRef	mTypeRef;
};


// ===========================================================================
//	Exception classes
// ===========================================================================

class	cf_exception {
public:
	cf_exception() { }
	~cf_exception() { }
};

								// Failure creating a CF object
class	cf_bad_create : public cf_exception {
public:
	cf_bad_create() { }
	~cf_bad_create() { }
};

								// Attempt to adopt an incompatible type ref
class	cf_bad_type : public cf_exception {
public:
	cf_bad_type() { }
	~cf_bad_type() { }
};

								// Attempt to change an immutable object
class	cf_bad_change : public cf_exception {
public:
	cf_bad_change() { }
	~cf_bad_change() { }
};

#endif
