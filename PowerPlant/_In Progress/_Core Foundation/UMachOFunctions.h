// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMachOFunctions.h			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UMachOFunctions
#define _H_UMachOFunctions
#pragma once

#include <CFBundle.h>
#include <CGContext.h>

// ===========================================================================
//	UMachOFunctions
// ===========================================================================


namespace UMachOFunctions {

	void			CGContextRelease( CGContextRef inContext );
	
}


// ---------------------------------------------------------------------------
//	Inline Functions
// ---------------------------------------------------------------------------

#if TARGET_RT_MAC_MACHO
// ---------------------------------------------------------------------------
//	When building MachO binaries, we can just call the Toolbox functions
//	directly as an inline, so there is no extra overhead.

inline
void
UMachOFunctions::CGContextRelease(
	CGContextRef	inContext)
{
	::CGContextRelease(inContext);
}

#endif // TARGET_RT_MAC_MACHO


#if (!TARGET_RT_MAC_MACHO && !TARGET_API_MAC_CARBON)
// ---------------------------------------------------------------------------
//	Not MachO and not Carbon means a Classic build, where the MachO
//	Toolbox functions are not available. All functions bodies are empty.

inline
void
UMachOFunctions::CGContextRelease(
	CGContextRef	/* inContext */)
{
}

#endif // (!TARGET_RT_MAC_MACHO && !TARGET_API_MAC_CARBON)


// ===========================================================================
//	USystemFramework
// ===========================================================================

namespace USystemFramework {
	
	CFBundleRef		GetBundle();
	
	void			ReleaseBundle();

	bool			LoadExecutable();
	
	void			UnloadExecutable();
	
	void*			GetFunctionPointer( CFStringRef inFunctionName );

}


#endif
