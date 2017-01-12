// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCommander.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Mix-in class for an object that responds to menu commands and keystrokes

#ifndef _H_LCommander
#define _H_LCommander
#pragma once

#include <LAttachable.h>
#include <TArray.h>

#include <Events.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LAction;

// ---------------------------------------------------------------------------

class	LCommander : public virtual LAttachable {
public:
						LCommander();

						LCommander( const LCommander& inOriginal );

						LCommander( LCommander* inSuper );

	virtual				~LCommander();

	virtual void		SetSuperCommander( LCommander* inSuper );

	LCommander*			GetSuperCommander() const
									{ return mSuperCommander; }

	virtual Boolean		AllowSubRemoval( LCommander* inSub );

	virtual Boolean		AttemptQuit( SInt32 inSaveOption );

	static LCommander*	GetTopCommander()
									{ return sTopCommander; }

	static LCommander*	GetTarget()
									{ return sTarget; }

	static Boolean		SwitchTarget( LCommander* inNewTarget );

	static void			ForceTargetSwitch( LCommander* inNewTarget );

	virtual void		RestoreTarget();

	Boolean				IsTarget() const		{ return (this == sTarget); }
	Boolean				IsOnDuty() const		{ return (mOnDuty == triState_On); }
	ETriState			GetOnDutyState() const	{ return mOnDuty; }

	virtual LCommander*	GetLatentSub() const;

	virtual void		SetLatentSub( LCommander* inSub );

	virtual Boolean		ProcessCommand(
								CommandT			inCommand,
								void*				ioParam = nil);

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam = nil);

	virtual void		ProcessCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual Boolean		ProcessKeyPress( const EventRecord& inKeyEvent );

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	TArray<LCommander*>&	GetSubCommanders()		{ return mSubCommanders; }

	virtual	void		PostAction( LAction* inAction = nil );
	
	// -----------------------------------------------------------------------
	//	Static Interface

	static	void		PostAnAction( LAction* inAction = nil );

	static LCommander*	GetDefaultCommander()
									{ return sDefaultCommander; }

	static void			SetDefaultCommander( LCommander* inCommander )
									{ sDefaultCommander = inCommander; }

	static Boolean		IsSyntheticCommand(
								CommandT			inCommand,
								ResIDT&				outMenuID,
								SInt16&				outMenuItem);

	static Boolean		IsHelpMenuCommand(
								CommandT			inCommand,
								SInt16&				outMenuItem);

	static void			SetUpdateCommandStatus( Boolean inDirty )
									{ sUpdateCommandStatus = inDirty; }

	static Boolean		GetUpdateCommandStatus()
									{ return sUpdateCommandStatus; }

protected:
	static LCommander*	sTopCommander;
	static LCommander*	sTarget;
	static LCommander*	sVisualTarget;
	static LCommander*	sDefaultCommander;
	static Boolean		sUpdateCommandStatus;

	LCommander*			mSuperCommander;
	TArray<LCommander*>	mSubCommanders;
	ETriState			mOnDuty;

	virtual void		AddSubCommander( LCommander* inSub );

	virtual void		RemoveSubCommander( LCommander* inSub );

	virtual Boolean		AttemptQuitSelf( SInt32	 inSaveOption );

	static void			SetTarget( LCommander* inNewTarget );

	virtual Boolean		AllowBeTarget( LCommander* inNewTarget );

	virtual Boolean		AllowDontBeTarget( LCommander* inNewTarget );

	virtual void		BeTarget();
	virtual void		DontBeTarget();

	void				TakeChainOffDuty( LCommander* inUpToCmdr );

	void				PutChainOnDuty( LCommander* inNewTarget );

	virtual void		PutOnDuty( LCommander* inNewTarget );

	virtual void		TakeOffDuty();

	virtual void		SubTargetChanged();

private:
	void				InitCommander( LCommander* inSuper );
};


typedef struct	SCommandStatus {
	Boolean*		enabled;
	Boolean*		usesMark;
	UInt16*			mark;
	unsigned char*	name;
	CommandT		command;
} SCommandStatus, *SCommandStatusP;


// Macro for determining the special synthetic command number indicating
// an entire menu. For use within FindCommandStatus().

#define	SyntheticCommandForMenu_(MENUid)	(-(((PP_PowerPlant::CommandT) MENUid) << 16))

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
