// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LApplication.h				PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LApplication
#define _H_LApplication
#pragma once

#include <LCommander.h>
#include <LEventDispatcher.h>
#include <LModelObject.h>

#include <AERegistry.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

// ---------------------------------------------------------------------------

PP_Begin_Namespace_PowerPlant

enum	EProgramState {
	programState_StartingUp,
	programState_ProcessingEvents,
	programState_Quitting
};

// ---------------------------------------------------------------------------

class	LApplication :	public LCommander,
						public LEventDispatcher,
					 	public LModelObject {
public:
						LApplication();
	virtual				~LApplication();

	EProgramState		GetState() const;

	void				SetSleepTime( UInt32 inSleepTime );

	UInt32				GetSleepTime() const;

	virtual void		Run();
	virtual void		ProcessNextEvent();
	virtual void		ShowAboutBox();

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void*				ioParam);

	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

		// ее Actions

	virtual void		SendAEQuit();
	virtual void		DoQuit( SInt32 inSaveOption = kAEAsk );

		// ее Apple Events

			// е Handle AppleEvents

	virtual void		HandleAppleEvent(
								const AppleEvent&	inAppleEvent,
								AppleEvent&			outAEReply,
								AEDesc&				outResult,
								SInt32				inAENumber);

	virtual SInt32		CountSubModels( DescType inModelID ) const;

	virtual void		GetSubModelByPosition(
								DescType			inModelID,
								SInt32				inPosition,
								AEDesc&				outToken) const;

	virtual void		GetSubModelByName(
								DescType			inModelID,
								Str255				inName,
								AEDesc&				outToken) const;

	virtual SInt32		GetPositionOfSubModel(
								DescType			inModelID,
								const LModelObject*	inSubModel) const;

protected:
	EProgramState		mState;
	UInt32				mSleepTime;

	virtual void		StartUp();
	virtual void		DoReopenApp();
	virtual void		DoPreferences();

	virtual void		MakeMenuBar();
	virtual void		MakeModelDirector();
	virtual void		Initialize();

	virtual void		MakeSelfSpecifier(
								AEDesc&				inSuperSpecifier,
								AEDesc&				outSelfSpecifier) const;

	virtual void		CatchException( const LException& inException );

	virtual void		CatchExceptionCode( ExceptionCode inError );
};


// ===========================================================================
//	Inline function definitions

// ---------------------------------------------------------------------------
//	е GetState												 [inline] [public]
// ---------------------------------------------------------------------------

inline EProgramState
LApplication::GetState() const
{
	return mState;
}


// ---------------------------------------------------------------------------
//	е SetSleepTime											 [inline] [public]
// ---------------------------------------------------------------------------

inline void
LApplication::SetSleepTime(
	UInt32	inSleepTime)
{
	mSleepTime = inSleepTime;
}


// ---------------------------------------------------------------------------
//	е GetSleepTime											 [inline] [public]
// ---------------------------------------------------------------------------

inline UInt32
LApplication::GetSleepTime() const
{
	return mSleepTime;
}

// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
