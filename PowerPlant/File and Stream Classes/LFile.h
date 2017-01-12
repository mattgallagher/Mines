// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFile.h						PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A wrapper class for a Macintosh file with a data and a resource fork

#ifndef _H_LFile
#define _H_LFile
#pragma once

#include <PP_Prefix.h>

#include <Aliases.h>
#include <Script.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LFile {
public:
						LFile();

						LFile( const FSSpec& inFileSpec );

						LFile(	AliasHandle			inAlias,
								Boolean&			outWasChanged,
								FSSpec*				inFromFile = nil);

	virtual				~LFile();

	void				GetSpecifier( FSSpec& outFileSpec ) const;

	virtual void		SetSpecifier( const FSSpec& inFileSpec );

	bool				UsesSpecifier( const FSSpec& inFileSpec ) const;
	
	bool				SpecifierExists() const;

	virtual AliasHandle	MakeAlias( FSSpec* inFromFile = nil );

	virtual void		CreateNewFile(
								OSType				inCreator,
								OSType				inFileType,
								ScriptCode			inScriptCode = smSystemScript);

	virtual void		CreateNewDataFile(
								OSType				inCreator,
								OSType				inFileType,
								ScriptCode			inScriptCode = smSystemScript);

	virtual SInt16		OpenDataFork( SInt16 inPrivileges );

	virtual void		CloseDataFork();

	SInt16				GetDataForkRefNum() const	{ return mDataForkRefNum; }

	virtual Handle		ReadDataFork();

	virtual SInt32		WriteDataFork(
								const void*			inBuffer,
								SInt32				inByteCount);

	virtual SInt16		OpenResourceFork( SInt16 inPrivileges );

	virtual void		CloseResourceFork();

	SInt16				GetResourceForkRefNum() const
							{
								return mResourceForkRefNum;
							}

	static bool			EqualFileSpec(
								const FSSpec		&inFileSpecA,
								const FSSpec		&inFileSpecB);


									// refNum for a data or resource
									//   fork which isn't open for access
	static const SInt16	refNum_Undefined	= -1;

protected:
	FSSpec			mMacFileSpec;
	SInt16			mDataForkRefNum;
	SInt16			mResourceForkRefNum;

private:								// Copy and Assignment not allowed
						LFile( const LFile& );

	LFile&				operator = ( const LFile& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
