// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabGroup.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTabGroup
#define _H_LTabGroup
#pragma once

#include <LCommander.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LStream;

// ---------------------------------------------------------------------------

class LTabGroup : public LCommander {
public:
	enum { class_ID = FOUR_CHAR_CODE('tabg') };

						LTabGroup();

						LTabGroup( const LTabGroup& inOriginal );

						LTabGroup( LStream* inStream );

						LTabGroup( LCommander* inSuper );

	virtual				~LTabGroup();

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	virtual void		RotateTarget( Boolean inBackward );

protected:
	virtual void		BeTarget();

	virtual void		GetNextTabCommander(
								LCommander*			&ioCommander,
								SInt32&				ioIndex);

	virtual void		GetPrevTabCommander(
								LCommander*			&ioCommander,
								SInt32&				ioIndex);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
