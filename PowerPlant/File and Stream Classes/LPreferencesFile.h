// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPreferencesFile.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A File that will automatically locate itself in the Preferences
//  folder when created.

#ifndef _H_LPreferencesFile
#define _H_LPreferencesFile
#pragma once

#include <LFile.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LPreferencesFile : public LFile {
public:
						LPreferencesFile();

						LPreferencesFile(
								ConstStringPtr		inFileName,
								Boolean				inCreateFolder = false);

						LPreferencesFile( FSSpec& inFileSpec );

						LPreferencesFile(
								AliasHandle			inAlias,
								Boolean&			outWasChanged,
								FSSpec*				inFromFile = nil);

	virtual				~LPreferencesFile();

	virtual SInt16		OpenOrCreateResourceFork(
								SInt16				inPrivileges,
								OSType				inCreator,
								OSType				inFileType,
								ScriptCode			inScriptCode);

private:							// Copy and Assignment not allowed
						LPreferencesFile( const LPreferencesFile& );

	LPreferencesFile&	operator = ( const LPreferencesFile& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
