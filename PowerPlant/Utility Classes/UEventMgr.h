// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UEventMgr.h					PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//	Utilities for isolating the differences between the Classic and Carbon
//	Event Manager

#ifndef _H_UEventMgr
#define _H_UEventMgr
#pragma once

#include <PP_Prefix.h>

#include <Events.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


namespace UEventMgr {

	// -----------------------------------------------------------------------
	//	¥ GetMouseAndModifiers
	// -----------------------------------------------------------------------
	//	Fill in the "where" and "modifiers" fields of an EventRecord with the
	//	current global mouse loation and state of the modifier keys

	inline
	void	GetMouseAndModifiers( EventRecord& outMacEvent )
				{
				#if TARGET_API_MAC_CARBON

				// Carbon has direct calls to get mouse and modifiers
				// Note: OSEventAvail is not in Carbon

					::GetGlobalMouse(&outMacEvent.where);
					outMacEvent.modifiers = (UInt16) ::GetCurrentKeyModifiers();

				#else

				// Calling OSEventAvail with a zero event mask will always
				// pass back a null event. However, it fills the EventRecord
				// with the information we need -- the mouse location in
				// global coordinates and the state of the modifier keys.

					::OSEventAvail(0, &outMacEvent);

				#endif
				}


	// -----------------------------------------------------------------------
	//	¥ GetKeyModifiers
	// -----------------------------------------------------------------------
	//	Fill in the "where" and "modifiers" fields of an EventRecord with the
	//	current global mouse loation and state of the modifier keys

	inline
	UInt32	GetKeyModifiers()
				{
				#if TARGET_API_MAC_CARBON

				// Carbon has a direct call to get the key modifiers
				// Note: OSEventAvail is not in Carbon

					return ::GetCurrentKeyModifiers();

				#else

				// Calling OSEventAvail with a zero event mask will always
				// pass back a null event. However, it fills the EventRecord
				// with the state of the modifier keys.

					EventRecord		event;
					::OSEventAvail(0, &event);
					return event.modifiers;

				#endif
				}


	// -----------------------------------------------------------------------
	//	¥ GetMouseUp
	// -----------------------------------------------------------------------
	//	Retrieve a mouse up event from the event queue

	inline
	bool	GetMouseUp( EventRecord& outMacEvent )
				{
				#if TARGET_API_MAC_CARBON

					// Carbon does not have GetOSEvent().
					// GetNextEvent() gives time to background tasks, which
					// we normally don't want to do here

					return ::GetNextEvent(mUpMask, &outMacEvent);

				#else

					return ::GetOSEvent(mUpMask, &outMacEvent);

				#endif
				}

}

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
