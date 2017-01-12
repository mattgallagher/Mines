// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMDialogBox.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMDialogBox
#define _H_LAMDialogBox
#pragma once

#include <LWindow.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMDialogBox : public LWindow,
					   public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('adlg') };

						LAMDialogBox();

						LAMDialogBox( const SWindowInfo& inWindowInfo );

						LAMDialogBox(
								ResIDT			inWINDid,
								UInt32			inAttributes,
								LCommander*		inSuperCommander);

						LAMDialogBox(
								LCommander*		inSuperCommander,
								const Rect&		inGlobalBounds,
								ConstStringPtr	inTitle,
								SInt16			inProcID,
								UInt32			inAttributes,
								WindowPtr		inBehind,
								PaneIDT			inDefaultButtonID = PaneIDT_Undefined,
								PaneIDT			inCancelButtonID = PaneIDT_Undefined);

						LAMDialogBox( LStream* inStream );

	virtual				~LAMDialogBox();

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
								
protected:
	PaneIDT				mDefaultButtonID;
	PaneIDT				mCancelButtonID;
};


struct SAMDialogResponse {
	LAMDialogBox*	dialogBox;
	void*			messageParam;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
