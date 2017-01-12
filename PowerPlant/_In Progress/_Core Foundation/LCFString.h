// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFString.h					PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCFString
#define _H_LCFString
#pragma once

#include <LCFObject.h>
#include <CFString.h>


class	LCFString : public LCFObject {
public:
					LCFString(
							CFIndex				inMaxLength = 0,
							CFAllocatorRef		inAllocator = nil);

					LCFString(
							ConstStringPtr		inPString,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman,
							CFAllocatorRef		inAllocator = nil);

					LCFString(
							const char*			inCString,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman,
							CFAllocatorRef		inAllocator = nil);

					LCFString( CFStringRef inStringRef );

					LCFString( CFMutableStringRef inStringRef );

					LCFString( const LCFString& inSource );

	LCFString&		operator = ( const LCFString& inSource );

	virtual			~LCFString();

					operator CFStringRef() const;

	CFStringRef		GetTypeRef() const;
	
	virtual void	AdoptTypeRef( CFTypeRef inTypeRef );
	
	void			Adopt( CFStringRef inStringRef );
	
	void			Adopt( CFMutableStringRef inStringRef );
	
	bool			IsMutable() const;

	CFIndex			GetLength() const;

	UniChar			GetCharacterAt( CFIndex inIndex ) const;

	UniChar			operator [] ( CFIndex inIndex ) const;

	void			GetSubstring(
							CFRange		inRange,
							UniChar*	outBuffer) const;

	bool			GetPascalString(
							StringPtr			outBuffer,
							CFIndex				inBufferSize,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman) const;

	bool			GetCString(
							char*				outBuffer,
							CFIndex				inBufferSize,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman) const;

	SInt32			GetIntValue() const;

	double			GetDoubleValue() const;

	void			Append( CFStringRef inString );

	void			Append(	const UniChar*	inChars,
							CFIndex			inNumChars);

	void			Append(	ConstStringPtr		inPString,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman);

	void			Append(	const char*			inCString,
							CFStringEncoding	inEncoding = kCFStringEncodingMacRoman);

	void			Insert(	CFIndex		inLocation,
							CFStringRef	inString);

	void			Delete(	CFRange inRange );

	void			Replace(
							CFRange		inRange,
							CFStringRef	inReplacementStr);

	void			ReplaceAll( CFStringRef inReplacementStr );


private:
	CFMutableStringRef	mMutableStringRef;
	CFIndex				mMaxLength;
};


// ===========================================================================
//	Inline Functions
// ===========================================================================

inline
LCFString::operator CFStringRef() const
{
	return GetTypeRef();
}


inline
CFStringRef
LCFString::GetTypeRef() const
{
	return reinterpret_cast<CFStringRef>( LCFObject::GetTypeRef() );
}


inline
bool
LCFString::IsMutable() const
{
	return (mMutableStringRef != nil);
}


inline
CFIndex
LCFString::GetLength() const
{
	return ::CFStringGetLength( GetTypeRef() );
}


inline
UniChar
LCFString::GetCharacterAt(
	CFIndex	inIndex) const
{
	return ::CFStringGetCharacterAtIndex(GetTypeRef(), inIndex);
}


inline
UniChar
LCFString::operator [] (
	CFIndex	inIndex) const
{
	return GetCharacterAt(inIndex);
}


inline
void
LCFString::GetSubstring(
	CFRange		inRange,
	UniChar*	outBuffer) const
{
	::CFStringGetCharacters(GetTypeRef(), inRange, outBuffer);
}


inline
bool
LCFString::GetPascalString(
	StringPtr			outBuffer,
	CFIndex				inBufferSize,
	CFStringEncoding	inEncoding) const
{
	return ::CFStringGetPascalString(GetTypeRef(), outBuffer, inBufferSize, inEncoding);
}


inline
bool
LCFString::GetCString(
	char*				outBuffer,
	CFIndex				inBufferSize,
	CFStringEncoding	inEncoding) const
{
	return ::CFStringGetCString(GetTypeRef(), outBuffer, inBufferSize, inEncoding);
}


inline
SInt32
LCFString::GetIntValue() const
{
	return ::CFStringGetIntValue(GetTypeRef());
}


inline
double
LCFString::GetDoubleValue() const
{
	return ::CFStringGetDoubleValue(GetTypeRef());
}


#endif
