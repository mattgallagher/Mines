// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCarbonEvents.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	Carbon Event handler functions suitable for use with LEventHandlerFunctor

#ifndef _H_UCarbonEvents
#define _H_UCarbonEvents
#pragma once

#include <PP_Prefix.h>
#include <CarbonEvents.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

namespace UCarbonEvents {

	OSStatus		DoSuspend(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
	
	OSStatus		DoResume(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
	
	OSStatus		DoProcessCommand(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
	
	OSStatus		DoMenuEnableItems(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
	
	OSStatus		DoKeyDown(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
							
	OSStatus		DoMouseMoved(
							EventHandlerCallRef	inCallRef,
							EventRef			inEventRef);
							
	void			MakeOldEventRecord(
							EventRef			inEventRef,
							EventRecord&		outOldEvent);
							
	void			UpdateCommandMenuItem(
							UInt32				inHICommandID,
							CommandT			inPPCommand);
							
	void			UpdateMenuItem(
							MenuRef				inMacMenu,
							MenuItemIndex		inIndex,
							bool				inIsEnabled,
							bool				inUsesMark,
							CharParameter		inMarkChar,
							ConstStringPtr		inName);
}

PP_End_Namespace_PowerPlant

#endif
