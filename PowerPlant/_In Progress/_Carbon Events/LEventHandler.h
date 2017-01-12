// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventHandler.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LEventHandler
#define _H_LEventHandler
#pragma once

#include <PP_Prefix.h>
#include <CarbonEvents.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LEventHandler {
public:
						LEventHandler();
					
	virtual				~LEventHandler();
	
	OSStatus			Install(
								EventTargetRef			inTarget,
								UInt32					inNumTypes,
								const EventTypeSpec*	inTypeList);
	
	OSStatus			Install(
								EventTargetRef		inTarget,
								UInt32				inEventClass,
								UInt32				inEventType);
	
	void				Remove();					
					
	OSStatus			AddTypes(
								UInt32					inNumTypes,
								const EventTypeSpec*	inTypeList);
							
	OSStatus			AddOneType(
								UInt32		inEventClass,
								UInt32		inEventType);
							
	OSStatus			RemoveTypes(
								UInt32					inNumTypes,
								const EventTypeSpec*	inTypeList);

	OSStatus			RemoveOneType(
								UInt32		inEventClass,
								UInt32		inEventType);
							
	virtual OSStatus	DoEvent(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef) = 0;

protected:								
	static OSStatus		CatchException();
							
private:
	EventHandlerRef		mHandlerRef;

private:					// Static class interface
	static pascal
	OSStatus			EventHandlerCallback(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef,
								void*				inUserData);
	
	static EventHandlerUPP	sHandlerUPP;
};

PP_End_Namespace_PowerPlant

#endif
