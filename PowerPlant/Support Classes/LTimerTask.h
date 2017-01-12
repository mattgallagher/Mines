// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTimerTask.h				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//	Wrapper class for an Event Loop Timer

#ifndef _H_LTimerTask
#define _H_LTimerTask
#pragma once

#include <PP_Prefix.h>
#include <CarbonEvents.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTimerTask {
public:
						LTimerTask();
						
	virtual				~LTimerTask();
	
	OSStatus			Install(
								EventLoopRef		inEventLoop,
								EventTimerInterval	inFireDelay,
								EventTimerInterval	inInterval);
								
	void				Remove();
	
	bool				IsInstalled() const;
	
	OSStatus			SetNextFireTime( EventTimerInterval inNextFire );
	
	virtual void		SetUserData( void* inUserData );
	
	virtual void*		GetUserData() const;
	
	virtual void		DoTask() = 0;

private:
	EventLoopTimerRef	mTimerRef;
	void*				mUserData;
	
private:
	static pascal
	void				TimerCallback(
								EventLoopTimerRef	inTimerRef,
								void*				inUserData);
								
	static EventLoopTimerUPP	sTimerUPP;
};

PP_End_Namespace_PowerPlant

#endif
