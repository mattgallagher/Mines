// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternetConfig.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility class for handling Internet Config support. An IC instance is
//	created the first time a utility function is called. The instance will
//	be destroyed when the application quits.

#ifndef _H_UInternetConfig
#define _H_UInternetConfig
#pragma once

#include <InternetConfig.h>

#include <LURL.h>
#include <LCleanupTask.h>
#include <Components.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

#define	CheckICAvailable_()														\
	do {																		\
		if (!UInternetConfig::PP_ICAvailable()) return icConfigNotFoundErr;		\
	} while (false)

#define	CheckICAvailableBool_()										\
	do {															\
		if (!UInternetConfig::PP_ICAvailable()) return false;		\
	} while (false)

#define ReturnIfOSStatus_(err)										\
	do {															\
		OSStatus	__theErr = err;									\
		if (__theErr != noErr) {									\
			return __theErr;										\
		}															\
	} while (false)

class LCloseInternetConfig;

class UInternetConfig {

public:
	static ICInstance * 		PP_GetICInstance();
	static Boolean				PP_ICAvailable();
	static Boolean				PP_SeedHasChanged();
	static SInt32				PP_GetNewSeed();

	static OSStatus				PP_ICParseURL(ConstStr255Param hint, Ptr data, SInt32 len, SInt32 *selStart, SInt32 *selEnd, Handle url);
	static OSStatus				PP_ICLaunchURL(ConstStr255Param hint, Ptr data, SInt32 len, SInt32 *selStart, SInt32 *selEnd);

	static OSStatus				PP_ICGetPref(ConstStr255Param key, Str255& outPref);
	static OSStatus				PP_ICFindPrefHandle(ConstStr255Param key, Handle outHandle);

	static OSStatus				PP_ICMapFilename(ConstStr255Param filename, ICMapEntry& entry);
	static OSStatus				PP_ICMapTypeCreator(OSType fType, OSType fCreator, ConstStr255Param filename, ICMapEntry& entry);
	static OSStatus				PP_ICGetListFont(ICFontRecord& outFont);
	static OSStatus				PP_ICGetScreenFont(ICFontRecord& outFont);
	static OSStatus				PP_ICGetPrinterFont(ICFontRecord& outFont);
	static OSStatus				PP_ICGetSMTPHost(Str255& outHost);
	static OSStatus				PP_ICEmailAddress(Str255& outEmailAddress);
	static OSStatus				PP_ICGetMailAccountInfo(Str255& outUser, Str255& outHost, Str255& outPassword);
	static OSStatus				PP_ICGetMailHeaders(Handle outHeaders);

	static OSStatus				PP_ICGetMailNotificationDefaults(Boolean& outFlashIcon,
													Boolean& outUseDialog,
													Boolean& outPlaySound,
													Str255& outSoundName);

	static OSStatus				PP_ICGetRealName(Str255& outRealName);
	static OSStatus				PP_ICGetOrganization(Str255& outOrg);
	static OSStatus				PP_ICGetSignature(Handle outSignature);
	static OSStatus				PP_ICGetDownloadFolder(FSSpec & outFolder);
	static OSStatus				PP_ICQuotingString(Str255& outQuoteString);
	static OSStatus				PP_ICGetFingerHost(Str255& outHost);
	static OSStatus				PP_ICGetFTPHost(Str255& outHost);
	static OSStatus				PP_ICGetWWWHomePage(LURL& outURL);
	static OSStatus				PP_ICGetCharacterSet(ICCharTable& outTable);

	static OSStatus				PP_ICEditPreferences(ConstStr255Param key);
	static void					UnscramblePString(Str255& inOutString);

	// IC Initialization/Deinitialization routines
	// NOTE: You will normally *NOT* need to call these yourself.
	//		They are left public so that people with IC pref file
	//		handling issues can call them directly when the application
	//		launches
	static OSStatus				PP_ICStart(OSType inCreator = 0, ICDirSpecArrayPtr inDirArrayPtr = nil);
	static void					PP_ICStop();

protected:
	static bool						sICAvailable;
	static bool						sICInited;
	static ICInstance				sICInstance;
	static SInt32					sICSeed;
	static LCloseInternetConfig *	sCloseICTask;

	friend class LCloseInternetConfig;
};

// ===========================================================================

#pragma mark -
#pragma mark === LCloseInternetConfig ===

class LCloseInternetConfig : public LCleanupTask {
public:
					LCloseInternetConfig();
	virtual			~LCloseInternetConfig();

protected:
	virtual void	DoCleanup();

	friend class UInternetConfig;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
