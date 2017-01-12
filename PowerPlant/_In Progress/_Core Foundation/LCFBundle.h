// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFBundle.h					PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCFBundle
#define _H_LCFBundle
#pragma once

#include <LCFObject.h>
#include <CFBundle.h>

// ---------------------------------------------------------------------------

class	LCFBundle : public LCFObject {
public:
						LCFBundle(
							CFURLRef		inBundleURL,
							CFAllocatorRef	inAllocator = nil);
						
						LCFBundle( CFBundleRef inBundleRef );
	
	virtual				~LCFBundle();
	
						operator CFBundleRef() const;
						
	CFBundleRef			GetTypeRef() const;
	
	void				Adopt( CFBundleRef inBundleRef );
	
	Boolean				LoadExecutable();
	
	void				UnloadExecutable();
	
	void*				GetFunctionPointer( CFStringRef inFunctionName );
	
private:
	bool				mExecutableLoaded;

private:					// $$$ Not yet implemented
						LCFBundle( const LCFBundle& inSource );
	
	LCFBundle&			operator = (const LCFBundle& inSource );
};


// ===========================================================================
//	Inline Functions
// ===========================================================================


// ---------------------------------------------------------------------------
//	¥ operator CFBundleRef											  [public]
// ---------------------------------------------------------------------------

inline
LCFBundle::operator CFBundleRef() const
{
	return GetTypeRef();
}


// ---------------------------------------------------------------------------
//	¥ GetTypeRef													  [public]
// ---------------------------------------------------------------------------

inline
CFBundleRef
LCFBundle::GetTypeRef() const
{
	return reinterpret_cast<CFBundleRef>( const_cast<void*>( LCFObject::GetTypeRef() ) );
}


// ---------------------------------------------------------------------------
//	¥ Adopt															  [public]
// ---------------------------------------------------------------------------

inline
void
LCFBundle::Adopt(
	CFBundleRef	inBundleRef)
{
	LCFObject::AdoptTypeRef(inBundleRef);
}


#endif
