// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFURL.h					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCFURL
#define _H_LCFURL
#pragma once

#include <LCFObject.h>
#include <CFURL.h>

// ---------------------------------------------------------------------------

class	LCFURL : public LCFObject {
public:
						LCFURL(	CFURLRef		inBaseURL,
								CFStringRef		inPathComponent,
								bool			inIsDirectory,
								CFAllocatorRef	inAllocator = nil);

						LCFURL(	const FSRef&	inFSRef,
								CFAllocatorRef	inAllocator = nil);
								
						LCFURL(	SInt16			inVRefNum,
								OSType			inFolderType,
								bool			inCreateFolder,
								CFAllocatorRef	inAllocator = nil);
						
						LCFURL( CFURLRef inURLRef );
								
	
	virtual				~LCFURL();
	
						operator CFURLRef() const;
						
	CFURLRef			GetTypeRef() const;
	
	void				Adopt( CFURLRef inURLRef );
	
private:					// $$$ Not yet implemented
						LCFURL( const LCFURL& inSource );
	
	LCFURL&				operator = (const LCFURL& inSource );
};


// ===========================================================================
//	Inline Functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ operator CFURLRef												  [public]
// ---------------------------------------------------------------------------

inline
LCFURL::operator CFURLRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef													  [public]
// ---------------------------------------------------------------------------

inline
CFURLRef
LCFURL::GetTypeRef() const
{
	return reinterpret_cast<CFURLRef>( LCFObject::GetTypeRef() );
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------

inline
void
LCFURL::Adopt(
	CFURLRef	inURLRef)
{
	LCFObject::AdoptTypeRef(inURLRef);
}


#endif
