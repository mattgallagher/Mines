// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UInternetConfig.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility class for handling Internet Config support. An IC instance is
//	created the first time a utility function is called. The instance will
//	be automatically destroyed when the application quits.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UInternetConfig.h>

#include <Gestalt.h>
#include <Processes.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

bool					UInternetConfig::sICAvailable = true;	//we assume it's there until we check
bool					UInternetConfig::sICInited = false;
LCloseInternetConfig*	UInternetConfig::sCloseICTask = nil;
ICInstance				UInternetConfig::sICInstance;
SInt32					UInternetConfig::sICSeed = 0;

#pragma mark === IC URL Handling ===

// ---------------------------------------------------------------------------
//	¥ PP_ICParseURL
// ---------------------------------------------------------------------------
//	Parse URL from text and return URL

OSStatus
UInternetConfig::PP_ICParseURL(
	ConstStr255Param	hint,
	Ptr					data,
	SInt32				len,
	SInt32*				selStart,
	SInt32*				selEnd,
	Handle				url)
{
	CheckICAvailable_();

	return ICParseURL(sICInstance, hint, data, len, selStart, selEnd, url);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICLaunchURL
// ---------------------------------------------------------------------------
//	Parse URL from text and pass off to default helper application specified
//	in InternetConfig

OSStatus
UInternetConfig::PP_ICLaunchURL(
	ConstStr255Param	hint,
	Ptr					data,
	SInt32				len,
	SInt32*				selStart,
	SInt32*				selEnd)
{
	CheckICAvailable_();

	return ICLaunchURL(sICInstance, hint, data, len, selStart, selEnd);
}


#pragma mark -
#pragma mark === File Name Mapping ===

// ---------------------------------------------------------------------------
//	¥ PP_ICMapFilename
// ---------------------------------------------------------------------------
//	Create Macintosh filename, filetype, etc. from a filename specification

OSStatus
UInternetConfig::PP_ICMapFilename(ConstStr255Param filename, ICMapEntry& entry)
{
	CheckICAvailable_();

	return ICMapFilename(sICInstance, filename, &entry);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICMapTypeCreator
// ---------------------------------------------------------------------------
//	Create Macintosh filetype and creator from a filename specification

OSStatus
UInternetConfig::PP_ICMapTypeCreator(OSType fType,
									OSType fCreator,
									ConstStr255Param filename,
									ICMapEntry& entry)
{
	CheckICAvailable_();

	return ICMapTypeCreator(sICInstance, fType, fCreator, filename, &entry);
}


#pragma mark -
#pragma mark === Font Preferences ===

// ---------------------------------------------------------------------------
//	¥ PP_ICGetListFont
// ---------------------------------------------------------------------------
//	Get the default font to be used for lists

OSStatus
UInternetConfig::PP_ICGetListFont(ICFontRecord& outFont)
{
	StHandleBlock prefH(1024);
	ReturnIfOSStatus_(PP_ICFindPrefHandle(kICListFont, prefH));

	StHandleLocker locked(prefH);
	::BlockMoveData(*prefH, &outFont, sizeof(ICFontRecord));

	return noErr;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetScreenFont
// ---------------------------------------------------------------------------
//	Get the default font to be used for screen display

OSStatus
UInternetConfig::PP_ICGetScreenFont(ICFontRecord& outFont)
{
	StHandleBlock prefH(1024);
	ReturnIfOSStatus_(PP_ICFindPrefHandle(kICScreenFont, prefH));

	StHandleLocker locked(prefH);
	::BlockMoveData(*prefH, &outFont, sizeof(ICFontRecord));

	return noErr;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetPrinterFont
// ---------------------------------------------------------------------------
//	Get the default font to be used for printing

OSStatus
UInternetConfig::PP_ICGetPrinterFont(ICFontRecord& outFont)
{
	StHandleBlock prefH(1024);
	ReturnIfOSStatus_(PP_ICFindPrefHandle(kICPrinterFont, prefH));

	StHandleLocker locked(prefH);
	::BlockMoveData(*prefH, &outFont, sizeof(ICFontRecord));

	return noErr;
}


#pragma mark -
#pragma mark === Mail Preferences ===

// ---------------------------------------------------------------------------
//	¥ PP_ICGetSMTPHost
// ---------------------------------------------------------------------------
//	Get the default SMTP host address

OSStatus
UInternetConfig::PP_ICGetSMTPHost(Str255& outHost)
{
	return PP_ICGetPref(kICSMTPHost, outHost);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICEmailAddress
// ---------------------------------------------------------------------------
//	Get the default email address

OSStatus
UInternetConfig::PP_ICEmailAddress(Str255& outEmailAddress)
{
	return PP_ICGetPref(kICEmail, outEmailAddress);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetMailAccountInfo
// ---------------------------------------------------------------------------
//	Get the default email account information

OSStatus
UInternetConfig::PP_ICGetMailAccountInfo(Str255& outUser,
										Str255& outHost,
										Str255& outPassword)
{
	Str255 tempBuff;
	ReturnIfOSStatus_(PP_ICGetPref(kICMailAccount, tempBuff));

	//seperate user@host info
	LString::PToCStr(tempBuff);
	char * atSign = PP_CSTD::strtok((char*)tempBuff, "@");
	if (atSign) {
		PP_CSTD::strcpy((char*)outUser, atSign);

		atSign = PP_CSTD::strtok(nil, "");
		PP_CSTD::strcpy((char*)outHost, atSign);

		LString::CToPStr((char*)outUser);
		LString::CToPStr((char*)outHost);
	}

	//Get password
	ReturnIfOSStatus_(PP_ICGetPref(kICMailPassword, outPassword));

	//Unscramble password
	UnscramblePString(outPassword);

	return noErr;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetMailHeaders
// ---------------------------------------------------------------------------
//	Get the default email headers to be added to outgoing mail messages

OSStatus
UInternetConfig::PP_ICGetMailHeaders(Handle outHeaders)
{
	return PP_ICFindPrefHandle(kICMailHeaders, outHeaders);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetMailNotificationDefaults
// ---------------------------------------------------------------------------
//	Get the default new mail notification settings

OSStatus
UInternetConfig::PP_ICGetMailNotificationDefaults(Boolean& outFlashIcon,
													Boolean& outUseDialog,
													Boolean& outPlaySound,
													Str255& outSoundName)
{
	Str255 tempBuff;

	ReturnIfOSStatus_(PP_ICGetPref(kICNewMailFlashIcon, tempBuff));
	outFlashIcon = (tempBuff[0] != 0);

	ReturnIfOSStatus_(PP_ICGetPref(kICNewMailDialog, tempBuff));
	outUseDialog = (tempBuff[0] != 0);

	ReturnIfOSStatus_(PP_ICGetPref(kICNewMailPlaySound, tempBuff));
	outPlaySound = (tempBuff[0] != 0);

	if (outPlaySound) {
		ReturnIfOSStatus_(PP_ICGetPref(kICNewMailSoundName, outSoundName));
	}

	return noErr;
}


#pragma mark -
#pragma mark === Misc. ===

// ---------------------------------------------------------------------------
//	¥ PP_ICGetRealName
// ---------------------------------------------------------------------------
//	Get the users real name

OSStatus
UInternetConfig::PP_ICGetRealName(Str255& outRealName)
{
	return PP_ICGetPref(kICRealName, outRealName);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetOrganization
// ---------------------------------------------------------------------------
//	Get the users organization

OSStatus
UInternetConfig::PP_ICGetOrganization(Str255& outOrg)
{
	return PP_ICGetPref(kICOrganization, outOrg);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetSignature
// ---------------------------------------------------------------------------
//	Get the users default signature

OSStatus
UInternetConfig::PP_ICGetSignature(Handle outSignature)
{
	return PP_ICFindPrefHandle(kICSignature, outSignature);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetDownloadFolder
// ---------------------------------------------------------------------------
//	Get the default download folder location

OSStatus
UInternetConfig::PP_ICGetDownloadFolder(FSSpec& outFolder)
{
	Handle	theFolder;
	ICAttr 	attr;
	OSStatus	err;

	CheckICAvailable_();

	theFolder = ::NewHandle(0);
	err = ::MemError();
	if (err) return err;

	err = ::ICFindPrefHandle(sICInstance, kICDownloadFolder, &attr, theFolder);

	if (err == noErr) {
		Boolean	wasChanged;
		(void)::Munger(theFolder, 0, nil,
					sizeof(ICFileSpec) - sizeof(AliasRecord), nil, 0);
		err = ::ResolveAlias(nil, (AliasHandle)theFolder,
					&outFolder, &wasChanged);
	}

	::DisposeHandle(theFolder);

	return err;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICQuotingString
// ---------------------------------------------------------------------------
//	Get the default quoting string

OSStatus
UInternetConfig::PP_ICQuotingString(Str255& outQuoteString)
{
	return PP_ICGetPref(kICQuotingString, outQuoteString);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetFingerHost
// ---------------------------------------------------------------------------
//	Get the default Finger protocol host

OSStatus
UInternetConfig::PP_ICGetFingerHost(Str255& outHost)
{
	return PP_ICGetPref(kICFingerHost, outHost);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetFTPHost
// ---------------------------------------------------------------------------
//	Get the default FTP host

OSStatus
UInternetConfig::PP_ICGetFTPHost(Str255& outHost)
{
	return PP_ICGetPref(kICFTPHost, outHost);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetWWWHomePage
// ---------------------------------------------------------------------------
//	Get the default home page

OSStatus
UInternetConfig::PP_ICGetWWWHomePage(LURL& outURL)
{
	Str255 tempBuff;
	ReturnIfOSStatus_(PP_ICGetPref(kICWWWHomePage, tempBuff));

	LString::PToCStr(tempBuff);
	outURL.SetURL((const char*)tempBuff);

	return noErr;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetCharacterSet
// ---------------------------------------------------------------------------
//	Get the default character set

OSStatus
UInternetConfig::PP_ICGetCharacterSet(ICCharTable& outTable)
{
	StHandleBlock prefH(1024);
	ReturnIfOSStatus_(PP_ICFindPrefHandle(kICCharacterSet, prefH));

	StHandleLocker locked(prefH);
	::BlockMoveData(*prefH, &outTable, sizeof(ICCharTable));

	return noErr;
}


#pragma mark -
#pragma mark === Utility Thangs ===

// ---------------------------------------------------------------------------
//	¥ PP_ICEditPreferences
// ---------------------------------------------------------------------------
//	Utility routine to launch InternetConfig to edit prefs

OSStatus
UInternetConfig::PP_ICEditPreferences(ConstStr255Param key)
{
	CheckICAvailable_();

	return ICEditPreferences(sICInstance, key);

}

// ---------------------------------------------------------------------------
//	¥ PP_ICGetPref
// ---------------------------------------------------------------------------
//	Utility routine to get a preference setting from InternetConfig

OSStatus
UInternetConfig::PP_ICGetPref(ConstStr255Param key, Str255& outPref)
{
	CheckICAvailable_();

	ICAttr attr;
	SInt32 ioSize = sizeof(Str255);
	return ICGetPref(sICInstance, key, &attr, (Ptr)outPref, &ioSize);
}

// ---------------------------------------------------------------------------
//	¥ PP_ICFindPrefHandle
// ---------------------------------------------------------------------------
//	Utility routine to get a preference setting from InternetConfig

OSStatus
UInternetConfig::PP_ICFindPrefHandle(ConstStr255Param key, Handle outHandle)
{
	CheckICAvailable_();

	ICAttr attr;
	return ICFindPrefHandle(sICInstance, key, &attr, outHandle);
}

// ---------------------------------------------------------------------------
//	¥ UnscramblePString
// ---------------------------------------------------------------------------
//	Utility routine to unscramble a string as encoded by IC

void
UInternetConfig::UnscramblePString(Str255& inOutString)
{
	for (UInt8 i = 1; i <= inOutString[0]; i++) {
		  inOutString[i] = (unsigned char) (inOutString[i] ^ (0x55 + i));
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === UInternetConfig - Low Level ===

// ---------------------------------------------------------------------------
//	¥ GetICInstance
// ---------------------------------------------------------------------------
//	Returns the "instance" variable that identifies us to Internet Config

ICInstance *
UInternetConfig::PP_GetICInstance()
{
	if (!PP_ICAvailable())
		return nil;

	return &sICInstance;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICAvailable
// ---------------------------------------------------------------------------
//	Checks to see that InternetConfig is available. Starts IC if necessary

Boolean
UInternetConfig::PP_ICAvailable()
{
	if (!sICAvailable)
		return false;

	if (!sICInited) {
		if (PP_ICStart() != noErr)
			return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
//	¥ PP_SeedHasChanged
// ---------------------------------------------------------------------------
//	Checks to see if preference file seed has changed. Gets new seed as well.

Boolean
UInternetConfig::PP_SeedHasChanged()
{
	CheckICAvailableBool_();

	SInt32	newSeed;
	ThrowIfOSStatus_(ICGetSeed(sICInstance, &newSeed));

	if (sICSeed != newSeed) {
		sICSeed = newSeed;
		return true;
	} else {
		return false;
	}
}

// ---------------------------------------------------------------------------
//	¥ PP_GetNewSeed
// ---------------------------------------------------------------------------
//	Gets new seed.

SInt32
UInternetConfig::PP_GetNewSeed()
{
	CheckICAvailable_();
	ThrowIfOSStatus_(ICGetSeed(sICInstance, &sICSeed));
	return sICSeed;
}

// ===========================================================================

#pragma mark -
#pragma mark === UInternetConfig - Start/Stop (See Note) ===

// NOTE: You will normally *NOT* need to call these yourself.
//		They are left public so that people with IC pref file
//		handling issues can call them directly when the application
//		launches

// ---------------------------------------------------------------------------
//	¥ PP_ICStart
// ---------------------------------------------------------------------------
//	Starts InternetConfig

OSStatus
UInternetConfig::PP_ICStart(OSType inCreator, ICDirSpecArrayPtr inDirArrayPtr)
{
	if ((not sICInited) && sICAvailable) {
		OSStatus err;

#if PP_Target_Classic
		SInt32 response;
		ComponentDescription ICDesc = {kICComponentType, kICComponentSubType,
										0L, 0L, 0L};

		// Gestalt check for Component Mgr and InternetConfig
		sICAvailable = false;
		err = ::Gestalt(gestaltComponentMgr, &response);

		if (err == noErr) {
			sICAvailable = (::CountComponents(&ICDesc) > 0);
		}

		if (not sICAvailable) {
			return icConfigNotFoundErr;
		}
#endif // PP_Target_Classic

		if (not inCreator) {
			ProcessSerialNumber thisProcess;
			ThrowIfOSErr_(::MacGetCurrentProcess(&thisProcess));

			//Get the signature of this application to pass along to ICStart
			ProcessInfoRec info;
			info.processName = nil;
			info.processAppSpec = nil;
			info.processInfoLength = sizeof(info);

			ThrowIfOSErr_(::GetProcessInformation(&thisProcess, &info));
			inCreator = info.processSignature;
		}

		err = ICStart(&sICInstance, inCreator);
		if (err) {
			sICAvailable = false;
			return err;
		}

		#if CALL_NOT_IN_CARBON
			err = ICGeneralFindConfigFile(sICInstance, true, false, 0, inDirArrayPtr);
			if (err)
				 return err;
		#else
			#pragma unused(inDirArrayPtr)
		#endif

		err = ICGetSeed(sICInstance, &sICSeed);
		if (err)
			 return err;

        sCloseICTask = new LCloseInternetConfig;
        sICInited = true;

        return noErr;
	}

	return icConfigNotFoundErr;
}

// ---------------------------------------------------------------------------
//	¥ PP_ICStop
// ---------------------------------------------------------------------------
//	Stops InternetConfig

void
UInternetConfig::PP_ICStop()
{
	if (sICInited) {
		 ICStop(sICInstance);
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === LCloseInternetConfig ===


// ---------------------------------------------------------------------------
//	¥ LCloseInternetConfig					Constructor
// ---------------------------------------------------------------------------

LCloseInternetConfig::LCloseInternetConfig()
{
}


// ---------------------------------------------------------------------------
//	¥ ~LCloseInternetConfig					Destructor
// ---------------------------------------------------------------------------

LCloseInternetConfig::~LCloseInternetConfig()
{
}


// ---------------------------------------------------------------------------
//	¥ DoCleanup
// ---------------------------------------------------------------------------
//	Ensures that InternetConfig is properly close when application quits or
//	ExitToShell is called.

void
LCloseInternetConfig::DoCleanup()
{
	UInternetConfig::PP_ICStop();
}

PP_End_Namespace_PowerPlant
