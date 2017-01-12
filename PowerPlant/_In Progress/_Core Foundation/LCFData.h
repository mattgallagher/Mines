// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFData.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCFData
#define _H_LCFData
#pragma once

#include <LCFObject.h>
#include <CFData.h>

// ---------------------------------------------------------------------------

class	LCFData : public LCFObject {
public:
						LCFData(
								CFIndex			inCapacity = 0,
								CFAllocatorRef	inAllocator = nil);

						LCFData(
								const UInt8*	inBytes,
								CFIndex			inLength,
								CFAllocatorRef	inAllocator = nil);

						LCFData( CFDataRef inDataRef );

						LCFData( CFMutableDataRef inDataRef );

	virtual				~LCFData();

						operator CFDataRef() const;

	CFDataRef			GetTypeRef() const;
	
	virtual void		AdoptTypeRef( CFTypeRef inTypeRef );
	
	void				Adopt( CFDataRef inDataRef );
	
	void				Adopt( CFMutableDataRef inMutableDataRef );
	
	bool				IsMutable() const;

	CFIndex				GetLength() const;

	const UInt8*		GetBytePtr() const;

	void				GetDataBytes(
								CFRange		inRange,
								UInt8*		outBuffer) const;

	UInt8*				GetMutableBytePtr() const;

	void				SetLength( CFIndex inLength );

	void				IncreaseLength( CFIndex inExtraLength );

	void				AppendBytes(
								const UInt8*	inBytes,
								CFIndex			inLength);

	void				ReplaceBytes(
								CFRange			inRange,
								const UInt8*	inNewBytes,
								CFIndex			inNewLength);

	void				DeleteBytes( CFRange inRange );

private:
	CFMutableDataRef	 mMutableDataRef;

private:					// $$$ Not yet implemented
						LCFData( const LCFData& inSource );
	LCFData&			operator = ( const LCFData& inSource) ;
};


// ===========================================================================
//	Inline Functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ operator CFDataRef											  [public]
// ---------------------------------------------------------------------------

inline
LCFData::operator CFDataRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef													  [public]
// ---------------------------------------------------------------------------

inline
CFDataRef
LCFData::GetTypeRef() const
{
	return reinterpret_cast<CFDataRef>( LCFObject::GetTypeRef() );
}


inline
bool
LCFData::IsMutable() const
{
	return (mMutableDataRef != nil);
}


inline
CFIndex
LCFData::GetLength() const
{
	return ::CFDataGetLength( GetTypeRef() );
}


inline
const UInt8*
LCFData::GetBytePtr() const
{
	return ::CFDataGetBytePtr( GetTypeRef() );
}


inline
void
LCFData::GetDataBytes(
	CFRange		inRange,
	UInt8*		outBuffer) const
{
	::CFDataGetBytes(GetTypeRef(), inRange, outBuffer);
}


#endif
