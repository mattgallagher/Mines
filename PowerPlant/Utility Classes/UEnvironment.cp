// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UEnvironment.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//	Maintains information about the Environment (machine and system software)
//	in which a program is running. Usually, you will set the features
//	of the Environment at the beginning of the program using Gestalt calls.
//	Storing the Gestalt results in this class for later inspection is
//	faster than repeatedly calling Gestalt during the program.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UEnvironment.h>

#include <Appearance.h>
#include <Gestalt.h>
#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

SEnvironment	UEnvironment::sEnvironInfo = { env_SupportsColor, 0, 0 };


// ---------------------------------------------------------------------------
//	¥ SetFeature											  [static, public]
// ---------------------------------------------------------------------------
//	Set an Environment Feature on or off

void
UEnvironment::SetFeature(
	EnvironmentFeature	inFeature,
	Boolean				inSetting)
{
	if (inSetting) {
		sEnvironInfo.feature |= inFeature;
	} else {
		sEnvironInfo.feature &= ~inFeature;
	}
}


// ---------------------------------------------------------------------------
//	¥ HasFeature											  [static, public]
// ---------------------------------------------------------------------------
//	Return whether an Environment Feature is on or off

bool
UEnvironment::HasFeature(
	EnvironmentFeature	inFeature)
{
	return ((sEnvironInfo.feature & inFeature) == inFeature);
}


// ---------------------------------------------------------------------------
//	¥ HasGestaltAttribute									  [static, public]
// ---------------------------------------------------------------------------
//	Return whether an attribute bit of a Gestalt selector is set
//
//	inSelector should be a Gestalt selector with the "Attr" suffix
//	inAttribute is the bit number in the Atrribute to check

bool
UEnvironment::HasGestaltAttribute(
	OSType	inSelector,
	UInt32	inAttribute)
{
	SInt32	response;
	return ( (::Gestalt(inSelector, &response) == noErr)  &&
			 ((response & (1L << inAttribute)) != 0) );
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ InitEnvironment										  [static, public]
// ---------------------------------------------------------------------------
//	Checks our runtime environment and sets the sEnvironInfo struct
//	to suit. It performs ONLY the gathering of information. There
//	is no alerting or termination of the application based upon
//	this information. That's something you have to do, but can then
//	use HasFeature to easily check.
//
//	Call this method near the start of your application (e.g. after
//	toolbox initialization, whenever you perform your Gestalt checks).
//	It is mandatory to make this call else PowerPlant internals might
//	not function properly.

void
UEnvironment::InitEnvironment()
{
										// Get OS version
	::Gestalt(gestaltSystemVersion, &sEnvironInfo.sysVersion);
	
										// Check for the Appearance Manager
	SetFeature(env_HasAppearance,
		HasGestaltAttribute(gestaltAppearanceAttr, gestaltAppearanceExists)  &&
		CFM_AddressIsResolved_(::RegisterAppearanceClient));


										// Get Appearance Manager version
	sEnvironInfo.appearanceVersion = 0L;
	if (HasFeature(env_HasAppearance)) {
		if (::Gestalt(gestaltAppearanceVersion, &sEnvironInfo.appearanceVersion) == noErr) {

			SetFeature(env_HasAppearance101, sEnvironInfo.appearanceVersion >= 0x0101);
			SetFeature(env_HasAppearance11,  sEnvironInfo.appearanceVersion >= 0x0110);
		}
	}

										// Check for AM compatabilitiy mode
	SetFeature(env_HasAppearanceCompat,
		HasGestaltAttribute(gestaltAppearanceAttr, gestaltAppearanceCompatMode)  &&
		CFM_AddressIsResolved_(::RegisterAppearanceClient));

#if ! (TARGET_CPU_68K && TARGET_RT_MAC_CFM)
										// Check for Window Mgr 2.0
										// For CFM-68K, the WindowsLib symbols
										//   aren't exported, so don't bother
										//   checking.
	SetFeature(env_HasWindowMgr20,
		HasGestaltAttribute(gestaltWindowMgrAttr, gestaltWindowMgrPresentBit)  &&
		CFM_AddressIsResolved_(::CreateNewWindow));
#endif

#if ! (TARGET_CPU_68K && TARGET_OS_MAC)
										// Check for Aqua Theme
										// Aqua does not exist for Mac OS 68K
	SetFeature(env_HasAquaTheme,
		HasGestaltAttribute(gestaltMenuMgrAttr, gestaltMenuMgrAquaLayoutBit));
#endif
}


// ---------------------------------------------------------------------------
//	¥ IsAppearanceRunning									  [static, public]
// ---------------------------------------------------------------------------
//	Determines if your application is running under Appearance or not
//	(not quite the same as just checking to see if it's there).
//	This routine assumes InitEnvironment has already been called

bool
UEnvironment::IsAppearanceRunning()
{
	return (UEnvironment::HasFeature(env_HasAppearance) &&
			!UEnvironment::HasFeature(env_HasAppearanceCompat));
}


// ---------------------------------------------------------------------------
//	¥ GetOSVersion											  [static, public]
// ---------------------------------------------------------------------------
//	Returns the version of the OS in system format. e.g. System 7.0.1
//	would be 0x0701, 7.1 would be 0x0710, 7.5.3 0x0753, etc.

	// Defined inline
#pragma mark UEnvironment::GetOSVersion


// ---------------------------------------------------------------------------
//	¥ GetAppearanceVersion									  [static, public]
// ---------------------------------------------------------------------------
//	Returns the version of the Appearance Manager in hex format.
//	For example, version 1.0.1 is 0x0101, 1.1 is 0x0110, etc.

	// Defined inline
#pragma mark UEnvironment::GetAppearanceVersion


// ---------------------------------------------------------------------------
//	¥ GetSharedLibSymbol									  [static, public]
// ---------------------------------------------------------------------------
//	Return pointer to a symbol exported from a PowerPC CFM shared library
//
//	Carbon programs running on Mac OS 8/9 can use this function to access
//	a symbol (usually a function pointer) that is present in a system
//	library but not in CarbonLib

Ptr
UEnvironment::GetSharedLibSymbol(
	ConstStringPtr	inLibName,
	ConstStringPtr	inSymbolName)
{
	Ptr		symbolPtr = nil;
	
	OSErr				err;
	CFragConnectionID	connID;
	
	err = ::GetSharedLibrary(inLibName,  kPowerPCCFragArch,
								kFindCFrag, &connID, nil, nil);
	
	if (err == noErr) {
		::FindSymbol(connID, inSymbolName, &symbolPtr, nil);
	}
	
	return symbolPtr;
}


PP_End_Namespace_PowerPlant
