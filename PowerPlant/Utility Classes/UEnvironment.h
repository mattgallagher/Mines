// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UEnvironment.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UEnvironment
#define _H_UEnvironment
#pragma once

#include <PP_Prefix.h>
#include <CodeFragments.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

enum {
	env_SupportsColor		= 0x00000001,
	env_HasDragManager		= 0x00000002,
	env_HasThreadsManager	= 0x00000004,
	env_HasThreadManager	= 0x00000004,
//	env_HasAOCE				= 0x00000008,	// Obsolete
//	env_HasStdMail			= 0x00000010,	// Obsolete
//	env_HasStdCatalog		= 0x00000020,	// Obsolete
//	env_HasDigiSign			= 0x00000040,	// Obsolete
	env_HasQuickTime		= 0x00000100,
	env_HasAppearance		= 0x00001000,
	env_HasAppearanceCompat	= 0x00002000,
//	env_HasAaron			= 0x00004000,	// Obsolete
	env_HasAppearance101	= 0x00008000,	// Appearance v1.0.1
	env_HasAppearance11		= 0x00010000,	// Appearance v1.1
	env_HasWindowMgr20		= 0x00020000,	// Window Manager 2.0
	env_HasAquaTheme		= 0x00040000
};

typedef UInt32	EnvironmentFeature;


#pragma options align=mac68k

typedef struct {
	EnvironmentFeature		feature;
	SInt32					sysVersion;
	SInt32					appearanceVersion;
} SEnvironment;

#pragma options align=reset

// ---------------------------------------------------------------------------

class	UEnvironment {
public:
	static void		SetFeature(
							EnvironmentFeature	inFeature,
							Boolean				inSetting);
							
	static bool		HasFeature(
							EnvironmentFeature	inFeature);

	static bool		HasGestaltAttribute(
							OSType				inSelector,
							UInt32				inAttribute);

	static void		InitEnvironment();

	static bool		IsAppearanceRunning();
	
	static bool		IsRunningOSX() { return sEnvironInfo.sysVersion >= 0x1000; }

	static SInt32	GetOSVersion()	{ return sEnvironInfo.sysVersion; }

	static SInt32	GetAppearanceVersion()
								{ return sEnvironInfo.appearanceVersion; }
								
	static Ptr		GetSharedLibSymbol(
							ConstStringPtr	inLibName,
							ConstStringPtr	inSymbolName);

private:
	static	SEnvironment	sEnvironInfo;
};


	// With CFM, programs can weak link libraries, so we may need to check
	// at runtime if a function is resolved (ie, the necessary code fragment
	// is present).

#if TARGET_RT_MAC_CFM
	#define CFM_AddressIsResolved_(addr)						\
		((addr) != (void*) kUnresolvedCFragSymbolAddress)
#else
	#define CFM_AddressIsResolved_(addr)		(true)
#endif


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
