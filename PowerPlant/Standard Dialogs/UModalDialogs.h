// ===========================================================================
//	UModalDialogs.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UModalDialogs
#define _H_UModalDialogs
#pragma once

#include <LCommander.h>
#include <LEventDispatcher.h>
#include <LListener.h>
#include <LPeriodical.h>

#include <Dialogs.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LWindow;

// ===========================================================================
#pragma mark StDialogHandler

class	StDialogHandler : public LEventDispatcher,
						  public LCommander,
						  public LListener {
public:
						StDialogHandler(
								ResIDT			inDialogResID,
								LCommander*		inSuper);

						StDialogHandler(
								LWindow*		inWindow,
								LCommander*		inSuper);

	virtual				~StDialogHandler();

	virtual MessageT	DoDialog();

	LWindow*			GetDialog() const	{ return mDialog; }

	void				SetSleepTime(
								UInt32			inSleepTime)
							{
								mSleepTime = inSleepTime;
							}

	virtual Boolean		AllowSubRemoval(
								LCommander*		inSub);

	virtual void		FindCommandStatus(
								CommandT		inCommand,
								Boolean&		outEnabled,
								Boolean&		outUsesMark,
								UInt16&			outMark,
								Str255			outName);

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

protected:
	LWindow			*mDialog;
	MessageT		mMessage;
	UInt32			mSleepTime;

#if PP_Uses_Carbon_Events
	bool			mFirstTime;
#endif

	void				InitDialogHandler();
};

// ===========================================================================
#pragma mark StTimedDialogHandler

class	StTimedDialogHandler : public StDialogHandler,
							   public LPeriodical {
public:
						StTimedDialogHandler(
								ResIDT			inDialogResID,
								LCommander*		inSuper,
								PaneIDT			inControlToClick,
								UInt32			inSecondsToWait );

						StTimedDialogHandler(
								LWindow*		inWindow,
								LCommander*		inSuper,
								PaneIDT			inControlToClick,
								UInt32			inSecondsToWait );

	virtual				~StTimedDialogHandler();

	UInt32				GetTimeout() const		{ return mSecondsToWait; }

	void				SetTimeout(
								UInt32	inSecondsToWait,
								bool	inRestartCountdown = true );

	UInt32				GetRemainingSeconds() const;

	bool				IsCountingDown() const { return mCountingDown; }

	PaneIDT				GetControlToClick() const { return mControlToClick; }

	void				SetControlToClick(
								PaneIDT inControlToClick )
							{
								mControlToClick = inControlToClick;
							}

	virtual	void		SpendTime( const EventRecord& inMacEvent );

	virtual	void		StartCountdown();
	virtual	void		StopCountdown();

protected:
	UInt32			mSecondsToWait;
	UInt32			mTargetTime;
	PaneIDT			mControlToClick;
	Boolean			mCountingDown;
};

// ===========================================================================
#pragma mark UModalDialogs

class	UModalDialogs {
public:
	static bool			AskForOneNumber(
								LCommander*		inSuper,
								ResIDT			inDialogID,
								PaneIDT			inEditFieldID,
								SInt32&			ioNumber);

	static bool			AskForOneString(
								LCommander*		inSuper,
								ResIDT			inDialogID,
								PaneIDT			inEditFieldID,
								Str255			ioString);
};

// ===========================================================================
#pragma mark UModalAlerts

class	UModalAlerts {
public:

	// -----------------------------------------------------------------------
	//	Wrappers for Toolbox Alert functions

	static DialogItemIndex	Alert			(ResIDT		inALRTid);
	static DialogItemIndex	StopAlert		(ResIDT		inALRTid);
	static DialogItemIndex	NoteAlert		(ResIDT		inALRTid);
	static DialogItemIndex	CautionAlert	(ResIDT		inALRTid);

	// -----------------------------------------------------------------------
	//	Event Filter for Alerts

	static void				SetModalEventFilter(
									ModalFilterProcPtr	inFilterProc)
								{
									sModalEventFilter = inFilterProc;
								}

	static ModalFilterProcPtr	GetModalEventFilter();

	static pascal Boolean	EventFilter_ModalDialog(
									DialogPtr			inDialog,
									EventRecord*		inMacEvent,
									DialogItemIndex*	outItemHit);

	// -----------------------------------------------------------------------
	//	Utility Functions

	static bool				SimulateDialogButtonClick(
									DialogPtr			inDialog,
									DialogItemIndex		inItemIndex);

	static void				ProcessModalEvent(
									EventRecord&		ioMacEvent);

protected:
	static ModalFilterProcPtr	sModalEventFilter;

	// -----------------------------------------------------------------------
	//	Event Filter for Standard File dialogs

#if PP_Target_Classic					// Standard File is only on Classic

public:
	static void				SetStdFileEventFilter(
									ModalFilterYDProcPtr	inFilterProc)
								{
									sStdFileEventFilter = inFilterProc;
								}

	static ModalFilterYDProcPtr	GetStdFileEventFilter();

	static pascal Boolean	EventFilter_StdFile(
									DialogPtr			inDialog,
									EventRecord*		inMacEvent,
									DialogItemIndex*	outItemHit,
									void*				inUserData);

protected:
	static ModalFilterYDProcPtr	sStdFileEventFilter;

#endif // PP_Target_Classic
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
