// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMachOFunctions.cp			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#include <UMachOFunctions.h>
#include <LCFURL.h>

#include <Folders.h>


#if (TARGET_RT_MAC_CFM && TARGET_API_MAC_CARBON)
// ---------------------------------------------------------------------------
//	For CFM Carbon builds, we must get a pointer to the MachO function
//	from the appropriate framework, accessed via a CFBundle


// ---------------------------------------------------------------------------
//	¥ CGContextRelease
// ---------------------------------------------------------------------------

void
UMachOFunctions::CGContextRelease(
	CGContextRef	inContext)
{
	typedef void (*CGContextRelease_Func)(CGContextRef);
	
	static CGContextRelease_Func	sCGContextRelease = nil;
	
	if (sCGContextRelease == nil) {
		sCGContextRelease = (CGContextRelease_Func)
			USystemFramework::GetFunctionPointer(CFSTR("CGContextRelease"));
	}
	
	if (sCGContextRelease != nil) {
		sCGContextRelease(inContext);
	}
}

#endif // (TARGET_RT_MAC_CFM && TARGET_API_MAC_CARBON)


#pragma mark -
// ===========================================================================
//	USystemFramework
// ===========================================================================

// ---------------------------------------------------------------------------
//	Static variables

namespace USystemFramework {

	static CFBundleRef	sSystemBundle = nil;
	static bool			sExecutableIsLoaded = false;
	
}


// ---------------------------------------------------------------------------
//	¥ GetBundle
// ---------------------------------------------------------------------------
//	Return the CFBundleRef for the System framework

CFBundleRef
USystemFramework::GetBundle()
{
	if (sSystemBundle == nil) {			// Create Bundle if doesn't exist
	
		LCFURL		baseURL(kOnAppropriateDisk, kFrameworksFolderType, kCreateFolder);
		LCFURL		bundleURL(baseURL, CFSTR("System.framework"), false);
		
		sSystemBundle = ::CFBundleCreate(nil, bundleURL);
		
		if (sSystemBundle == nil) {
			throw cf_bad_create();
		}
	}

	return sSystemBundle;
}


// ---------------------------------------------------------------------------
//	¥ ReleaseBundle
// ---------------------------------------------------------------------------
//	Release our Bundle for the System framework
//
//	WARNING: This will invalidate all function pointers obtained by using
//	the routines in this namespace.

void
USystemFramework::ReleaseBundle()
{
	if (sSystemBundle != nil) {
		::CFRelease(sSystemBundle);
		
		sSystemBundle		= nil;
		sExecutableIsLoaded = false;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetFunctionPointer
// ---------------------------------------------------------------------------
//	Return pointer to a function in the System framework

void*
USystemFramework::GetFunctionPointer(
	CFStringRef	inFunctionName)
{
	void*	functionPtr = nil;

	CFBundleRef	systemBundle = GetBundle();
	
	if (LoadExecutable()) {
		functionPtr = ::CFBundleGetFunctionPointerForName( systemBundle,
														   inFunctionName );
	}
	
	return functionPtr;
}


// ---------------------------------------------------------------------------
//	¥ LoadExecutable
// ---------------------------------------------------------------------------

bool
USystemFramework::LoadExecutable()
{
	if (not sExecutableIsLoaded) {
		CFBundleRef	systemBundle = GetBundle();
		sExecutableIsLoaded = ::CFBundleLoadExecutable(systemBundle);
	}
	
	return sExecutableIsLoaded;
}


// ---------------------------------------------------------------------------
//	¥ UnloadExecutable
// ---------------------------------------------------------------------------

void
USystemFramework::UnloadExecutable()
{
	if (sExecutableIsLoaded) {
		::CFBundleUnloadExecutable(sSystemBundle);
	}
}
