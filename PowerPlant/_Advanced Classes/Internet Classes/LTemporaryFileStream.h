// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTemporaryFileStream.h		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTemporaryFileStream
#define _H_LTemporaryFileStream
#pragma once

#include <LFileStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LTemporaryFileStream : public LFileStream {
public:
					LTemporaryFileStream();
					
					LTemporaryFileStream( ConstStr255Param inFileName );
							
					LTemporaryFileStream( FSSpec& inFileSpec );
							
					LTemporaryFileStream(
							AliasHandle		inAlias,
							Boolean&		outWasChanged,
							FSSpec*			inFromFile = nil);

	virtual			~LTemporaryFileStream();

	virtual void	SetMarker(
							SInt32			inOffset,
							EStreamFrom		inFromWhere);

	virtual SInt16	OpenOrCreateResourceFork(
							SInt16			inPrivileges,
							OSType			inCreator,
							OSType			inFileType,
							ScriptCode		inScriptCode);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
