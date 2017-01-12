// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventDispatcher.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Dispatches Toolbox Events to the proper objects

#ifndef _H_LEventDispatcher
#define _H_LEventDispatcher
#pragma once

#include <LAttachable.h>
#include <URegions.h>

#include <Events.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LCommander;

// ---------------------------------------------------------------------------

class LEventDispatcher : public virtual LAttachable {
public:
							LEventDispatcher();
	virtual					~LEventDispatcher();

	virtual void			DispatchEvent	(const EventRecord& inMacEvent);
	virtual void			UseIdleTime		(const EventRecord& inMacEvent);

	virtual void			UpdateMenus();

	virtual Boolean			ExecuteAttachments(
								MessageT	inMessage,
								void*		ioParam);


protected:
	virtual void			AdjustCursor	(const EventRecord& inMacEvent);
	virtual void			EventMouseDown	(const EventRecord& inMacEvent);
	virtual void			ClickMenuBar	(const EventRecord& inMacEvent);
	virtual void			EventMouseUp	(const EventRecord& inMacEvent);
	virtual void			EventKeyDown	(const EventRecord& inMacEvent);
	virtual void			EventAutoKey	(const EventRecord& inMacEvent);
	virtual void			EventKeyUp		(const EventRecord& inMacEvent);
	virtual void			EventDisk		(const EventRecord& inMacEvent);
	virtual void			EventUpdate		(const EventRecord& inMacEvent);
	virtual void			EventActivate	(const EventRecord& inMacEvent);
	virtual void			EventOS			(const EventRecord& inMacEvent);
	virtual void			EventResume		(const EventRecord& inMacEvent);
	virtual void			EventSuspend	(const EventRecord& inMacEvent);
	virtual void			EventHighLevel	(const EventRecord& inMacEvent);

protected:
	LEventDispatcher* 			mSaveDispatcher;
	StRegion					mMouseRgn;
								
	// ----------------------------------------------------------------------
	// Static Interface

public:	
	static LEventDispatcher *GetCurrentEventDispatcher()
								{
									return sCurrentDispatcher;
								}
								
	static void				GetCurrentEvent( EventRecord& outMacEvent )
								{
									outMacEvent = sCurrentEvent;
								}

protected:
	static LEventDispatcher*	sCurrentDispatcher;
	static EventRecord			sCurrentEvent;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
