// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFileTypeList.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFileTypeList
#define _H_LFileTypeList
#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#include <PP_Prefix.h>
#include <UMemoryMgr.h>
#include <Navigation.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const ResIDT				open_DefaultID	= 128;
const NavTypeListHandle		fileTypes_All	= nil;


// ---------------------------------------------------------------------------

class	LFileTypeList {
public:
						LFileTypeList( ResIDT inOpenResID = open_DefaultID );

						LFileTypeList( OSType inFileType );

						LFileTypeList(
								SInt16		inNumTypes,
								OSType*		inTypeList);

						LFileTypeList( NavTypeListHandle inTypesH );

						LFileTypeList( const LFileTypeList& inFileTypeList );

	LFileTypeList&		operator = ( const LFileTypeList& inFileTypeList );

						~LFileTypeList();

	SInt16				NumberOfTypes() const;

	const OSType*		TypeListPtr() const;

	NavTypeListHandle	TypeListHandle() const;

	void				SetSignature( OSType inSignature );

	static OSType		GetProcessSignature();

protected:
	StHandleBlock	mTypeListH;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
