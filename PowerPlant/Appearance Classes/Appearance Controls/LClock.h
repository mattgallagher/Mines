// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LClock.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LClock
#define _H_LClock
#pragma once

#include <LControlPane.h>
#include <LCommander.h>
#include <LPeriodical.h>

#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LClock : public LControlPane,
			   public LCommander,
			   public LPeriodical {

public:
	enum {	class_ID		= FOUR_CHAR_CODE('clck'),
			imp_class_ID	= FOUR_CHAR_CODE('iclk') };

						LClock(	LStream*	inStream,
								ClassIDT	inImpID = imp_class_ID);

						LClock(	const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt16				inClockKind,
								ResIDT				inTextTraitsID,
								ControlClockFlags	inClockFlags,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LClock();

	void				SetLongDate( const LongDateRec& inLongDate );

	void				GetLongDate( LongDateRec& outLongDate ) const;

	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );

	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );

	virtual Boolean		ObeyCommand(
								CommandT		inCommand,
								void*			ioParam);

	virtual void		UserChangedClock();

	void				Start();
	void				Stop();

	virtual void		SpendTime( const EventRecord& inMacEvent );

protected:
	bool			mIsDisplayOnly;
	bool			mIsLive;
	SInt16			mFocusPart;

	virtual void		BeTarget();
	virtual void		DontBeTarget();
	
	virtual void		HideSelf();
	virtual void		DisableSelf();

private:
	void				InitClock( ControlClockFlags inClockFlags );

							// Unimplemented
						LClock();
						LClock(	const LClock& );
	LClock&				operator=( const LClock& );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
