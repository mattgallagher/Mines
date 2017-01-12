// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCMAttachment.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LCMAttachment
#define _H_LCMAttachment
#pragma once

#include <LAttachment.h>
#include <LPane.h>
#include <LString.h>
#include <TArray.h>
#include <UAppleEventsMgr.h>
#include <LCommander.h>

#include <Menus.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCMAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('cmat') };

						LCMAttachment();
								
						LCMAttachment(
								LCommander*			inTarget,
								PaneIDT				inTargetPaneID = PaneIDT_Unspecified);
								
						LCMAttachment( LStream* inStream );

	virtual				~LCMAttachment();

	SInt16				GetMenuID() const { return mMENUid; }
			
	void				SetMenuID( SInt16 inMenuID )
							{
								mMENUid = inMenuID;
							}

	ResIDT				GetCursorID() const { return mCMMCursorID; }
			
	void				SetCursorID( ResIDT inCURSid )
							{
								mCMMCursorID = inCURSid;
							}

	UInt32				GetHelpType() const { return mHelpType; }
	
	void				SetHelpType( UInt32 inHelpType )
							{
								mHelpType = inHelpType;
							}

	void				GetHelpString( Str255 outString )
							{
								outString = mHelpString;
							}
							
	void				SetHelpString( ConstStr255Param inString )
							{
								mHelpString = inString;
							}

	virtual	LPane*		FindSubPaneWithAttachment( LPane* inPane );

	virtual	bool		IsCommanderAlive(
								LCommander*		inCommander,
								LCommander*		inStartFrom = LCommander::GetTopCommander());

	LCommander*			GetCommandTarget() const
							{
								return mTarget;
							}
							
	virtual	void		SetCommandTarget( LCommander* inTarget )
							{
								mTarget = inTarget;
								if (mTarget == nil) {
									SetTargetPaneID(PaneIDT_Unspecified);
								}
							}
							
	virtual	void		SetCommandTarget( PaneIDT inTargetPaneID );

	PaneIDT				GetTargetPaneID() const
							{
								return mTargetPaneID;
							}
							
	void				SetTargetPaneID( PaneIDT inTargetPaneID )
							{
								// Provided as a means to access the
								//	mTargetPaneID. Doesn't (re)establish
								//	mTarget to the new target. If you
								//	with to do this, use
								//	SetCommandTarget(PaneIDT) instead.

								mTargetPaneID = inTargetPaneID;
							}


	virtual void		SetMessage( MessageT inMessage );

protected:
	StAEDescriptor		mSelection;

	SInt16				mMENUid;
	ResIDT				mCMMCursorID;
	UInt32				mHelpType;
	LStr255				mHelpString;
	TArray<CommandT>	mCommandList;
	bool				mCMMInitialized;
	LCommander*			mTarget;
	PaneIDT				mTargetPaneID;

	virtual	void		ExecuteSelf(
								MessageT		inMessage,
								void*			ioParam);

	virtual	void		DoCMMClick( const EventRecord& inMacEvent );

	virtual	void		PreCMSelect( const EventRecord& inMacEvent );
	
	virtual	void		PostCMSelect( const EventRecord& inMacEvent );

	virtual	void		DispatchCommand(
								UInt32			inSelectionType,
								CommandT		inCommand);

	virtual	void		ShowHelp();

	virtual	MenuHandle	CreateMenu( bool inAlwaysBeThemeSavvy = false );
	
	virtual	void		PopulateMenu( MenuHandle inMenuH );
	
	virtual	void		FinalizeMenu( MenuHandle inMenuH );

	virtual	bool		AddCommand(
								MenuHandle		inMenuH,
								CommandT		inCommand,
								ConstStringPtr	inItemText = Str_Empty,
								SInt16			inAfterItem = 32000,	// if you have this many items in a menu...
								bool			inOverrideDisable = false );

	virtual	void				GetContext();

	virtual	LCommander*			FindCommandTarget();
	
	virtual	LView*				GetTopmostView( LPane* inStartPane );

private:
			void				Init();

								LCMAttachment( const LCMAttachment& );
			LCMAttachment&		operator = ( const LCMAttachment& );
};


// ---------------------------------------------------------------------------
//	We no longer use this curosr resource. Instead, we treat it as a
//	special flag to use kThemeContextualMenuArrowCursor

const ResIDT	CURS_CMMCursor = 450;

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
