// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LUndoer.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Attachment for implementing Undo

#ifndef _H_LUndoer
#define _H_LUndoer
#pragma once

#include <LAttachment.h>
#include <LCommander.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LAction;
class	LStream;

// ---------------------------------------------------------------------------

class	LUndoer : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('undo') };

						LUndoer();

						LUndoer( LStream* inStream );

	virtual				~LUndoer();

protected:
	LAction*			mAction;

	virtual void		ExecuteSelf(
								MessageT		inMessage,
								void* 			ioParam);

	virtual void		PostAction( LAction* inAction );

	virtual void		ToggleAction();

	virtual void		FindUndoStatus( SCommandStatus* ioStatus );

private:					// Unimplemented copy and assignment
						LUndoer( const LUndoer& );
	LUndoer&			operator = ( const LUndoer& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
