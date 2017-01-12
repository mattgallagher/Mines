// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDialogBox.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDialogBox
#define _H_LDialogBox
#pragma once

#include <LWindow.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDialogBox : public LWindow,
					 public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('dlog') };

						LDialogBox();

						LDialogBox( const SWindowInfo& inWindowInfo );

						LDialogBox(
								ResIDT			inWINDid,
								UInt32			inAttributes,
								LCommander*		inSuperCommander);

						LDialogBox(
								LCommander*		inSuperCommander,
								const Rect&		inGlobalBounds,
								ConstStringPtr	inTitle,
								SInt16			inProcID,
								UInt32			inAttributes,
								WindowPtr		inBehind,
								PaneIDT			inDefaultButtonID = PaneIDT_Undefined,
								PaneIDT			inCancelButtonID = PaneIDT_Undefined);

						LDialogBox( LStream* inStream );

	virtual				~LDialogBox();

	virtual void		SetDefaultButton( PaneIDT inButtonID );

	virtual void		SetCancelButton( PaneIDT inButtonID );

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

protected:
	virtual void		FinishCreateSelf();
	
	virtual void		SetDefaultTag(
								PaneIDT			inButtonID,
								Boolean			inBeDefault);
	
	virtual void		SetCancelTag(
								PaneIDT			inButtonID,
								Boolean			inBeDefault);
								
protected:
	PaneIDT				mDefaultButtonID;
	PaneIDT				mCancelButtonID;
};


// ---------------------------------------------------------------------------
//	Structure passed as the parameter to ProcessCommand when relaying
//	Messages as Commands

struct SDialogResponse {
	LDialogBox*		dialogBox;
	void*			messageParam;
};
// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
