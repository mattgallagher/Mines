// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCFBundle.cp				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#include <LCFBundle.h>


// ---------------------------------------------------------------------------
//	¥ LCFBundle								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from a base URL and a path component

LCFBundle::LCFBundle(
	CFURLRef		inBundleURL,
	CFAllocatorRef	inAllocator)
{
	CFBundleRef	ref = ::CFBundleCreate(inAllocator, inBundleURL);
	
	if (ref == nil) {
		throw cf_bad_create();
	}
	
	SetTypeRef(ref);
	
	mExecutableLoaded = false;
}


// ---------------------------------------------------------------------------
//	LCFBundle								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Construct from an existing CFBundleRef
//
//	We retain the CFBundleRef. Caller should release the CFBundleRef if it
//	no longer needs it.

LCFBundle::LCFBundle(
	CFBundleRef		inBundleRef)

	: LCFObject(inBundleRef)
{
	mExecutableLoaded = false;
}


// ---------------------------------------------------------------------------
//	¥ LCFBundle								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LCFBundle::LCFBundle(
	const LCFBundle&	inSource)
{
	#pragma unused(inSource)		// $$$ Not yet implemented
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LCFBundle&
LCFBundle::operator = (
	const LCFBundle&	inSource)
{
	#pragma unused(inSource)		// $$$ Not yet implemented
	
	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LCFBundle							Destructor				  [public]
// ---------------------------------------------------------------------------

LCFBundle::~LCFBundle()
{
}


// ---------------------------------------------------------------------------
//	¥ LoadExecutable												  [public]
// ---------------------------------------------------------------------------

Boolean
LCFBundle::LoadExecutable()
{
	return ::CFBundleLoadExecutable(GetTypeRef());
}


// ---------------------------------------------------------------------------
//	¥ UnloadExecutable												  [public]
// ---------------------------------------------------------------------------

void
LCFBundle::UnloadExecutable()
{
	return ::CFBundleUnloadExecutable(GetTypeRef());
}


// ---------------------------------------------------------------------------
//	¥ GetFunctionPointer											  [public]
// ---------------------------------------------------------------------------

void*
LCFBundle::GetFunctionPointer(
	CFStringRef	inFunctionName)
{
	void*	functionPtr = nil;
	
	if (not mExecutableLoaded) {
		mExecutableLoaded = LoadExecutable();
	}
	
	if (mExecutableLoaded) {
		functionPtr = ::CFBundleGetFunctionPointerForName(GetTypeRef(), inFunctionName);
	}
	
	return functionPtr;
}
