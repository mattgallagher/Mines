// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPeriodical.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	An abstract building block class for objects that want to receive a
//	function call at regular intervals

#ifndef _H_LPeriodical
#define _H_LPeriodical
#pragma once

#include <TArray.h>
#include <TTimerTask.h>

#include <Events.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPeriodical {
public:
						LPeriodical();
						
	virtual				~LPeriodical();

	virtual	void		StartIdling();
	
	virtual	void		StopIdling();
	
	bool				IsIdling() const;
	
	static	void		DevoteTimeToIdlers( const EventRecord& inMacEvent );

	virtual	void		StartRepeating();
	
	virtual	void		StopRepeating();
	
	bool				IsRepeating() const;
	
	static	void		DevoteTimeToRepeaters( const EventRecord& inMacEvent );

						// Pure Virtual. Concrete subclasses must override
	virtual	void		SpendTime( const EventRecord& inMacEvent ) = 0;
	
	// -----------------------------------------------------------------------
	//	Timer Support
	
	virtual void		StartIdling( EventTimerInterval inInterval );
	
	virtual void		StartRepeating( EventTimerInterval inInterval );
	
#if PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
protected:

	TTimerTask<LPeriodical>*		mTimerTask;
	
	void				InstallTimer( EventTimerInterval inInterval );
	
	void				RemoveTimer();
	
	void				DoTimerTask( LTimerTask* inTask );
	
#endif // PP_Uses_Periodical_Timers && TARGET_API_MAC_CARBON
	
	
	// -----------------------------------------------------------------------
	//	Static Interface

public:
	static void			DeleteIdlerAndRepeaterQueues();

protected:
	static	TArray<LPeriodical*>*	sIdlerQ;
	static	TArray<LPeriodical*>*	sRepeaterQ;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
