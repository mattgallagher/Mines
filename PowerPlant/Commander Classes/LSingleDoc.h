// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSingleDoc.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Document which associates one file with one window

#ifndef _H_LSingleDoc
#define _H_LSingleDoc
#pragma once

#include <LDocument.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LWindow;
class	LFile;

// ---------------------------------------------------------------------------

class LSingleDoc : public LDocument {
public:
						LSingleDoc();

						LSingleDoc( LCommander* inSuper );

	virtual				~LSingleDoc();

	virtual Boolean		AllowSubRemoval( LCommander* inSub );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual Boolean		UsesFileSpec( const FSSpec& inFileSpec ) const;

	virtual void		MakeCurrent();

	LWindow*			GetWindow() const	{ return mWindow; }

	LFile*				GetFile() const		{ return mFile; }

protected:
	LWindow*		mWindow;
	LFile*			mFile;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
