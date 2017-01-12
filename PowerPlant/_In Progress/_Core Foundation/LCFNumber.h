// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFNumber.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCFNumber
#define _H_LCFNumber
#pragma once

#include <LCFObject.h>
#include <CFNumber.h>

// ---------------------------------------------------------------------------

class	LCFNumber : public LCFObject {
public:
						LCFNumber(
								char			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								SInt8			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								SInt16			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								SInt32			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								SInt64			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								Float32			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								Float64			inNumber,
								CFAllocatorRef	inAllocator = nil);

						LCFNumber(
								CFNumberRef		inNumberRef);

						LCFNumber( const LCFNumber& inSource );

	LCFNumber&			operator = (const LCFNumber& inSource );

	virtual				~LCFNumber();

						operator CFNumberRef() const;

	CFNumberRef			GetTypeRef() const;
	
	void				Adopt( CFNumberRef inNumberRef );

	CFNumberType		GetNumberType() const;

	CFIndex				GetByteSize() const;

	bool				IsFloatType() const;

	bool				GetValueAs(
								CFNumberType	inType,
								void*			inValuePtr) const;

	bool				GetValueAs( char&		outValue ) const;
	bool				GetValueAs( SInt8&		outValue ) const;
	bool				GetValueAs( SInt16&		outValue ) const;
	bool				GetValueAs( SInt32&		outValue ) const;
	bool				GetValueAs( SInt64&		outValue ) const;
	bool				GetValueAs( Float32&	outValue ) const;
	bool				GetValueAs( Float64&	outValue ) const;

};

// ===========================================================================
//	Inline Functions
// ===========================================================================

inline
LCFNumber::operator CFNumberRef() const
{
	return GetTypeRef();
}


inline
CFNumberRef
LCFNumber::GetTypeRef() const
{
	return reinterpret_cast<CFNumberRef>( LCFObject::GetTypeRef() );
}


inline
void
LCFNumber::Adopt(
	CFNumberRef	inNumberRef)
{
	LCFObject::AdoptTypeRef(inNumberRef);
}


inline
CFNumberType
LCFNumber::GetNumberType() const
{
	return ::CFNumberGetType( GetTypeRef() );
}


inline
CFIndex
LCFNumber::GetByteSize() const
{
	return ::CFNumberGetByteSize( GetTypeRef() );
}


inline
bool
LCFNumber::IsFloatType() const
{
	return ::CFNumberIsFloatType( GetTypeRef() );
}


inline
bool
LCFNumber::GetValueAs(
	CFNumberType	inType,
	void*			inValuePtr) const
{
	return ::CFNumberGetValue( GetTypeRef(), inType, inValuePtr );
}


inline
bool
LCFNumber::GetValueAs(
	char&	outValue) const
{
	return GetValueAs(kCFNumberCharType, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	SInt8&	outValue) const
{
	return GetValueAs(kCFNumberSInt8Type, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	SInt16&	outValue) const
{
	return GetValueAs(kCFNumberSInt16Type, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	SInt32&	outValue) const
{
	return GetValueAs(kCFNumberSInt32Type, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	SInt64&	outValue) const
{
	return GetValueAs(kCFNumberSInt64Type, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	Float32&	outValue) const
{
	return GetValueAs(kCFNumberFloat32Type, &outValue);
}


inline
bool
LCFNumber::GetValueAs(
	Float64&	outValue) const
{
	return GetValueAs(kCFNumberFloat64Type, &outValue);
}


#endif
