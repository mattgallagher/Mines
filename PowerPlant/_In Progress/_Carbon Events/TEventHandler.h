// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TEventHandler.h				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Carbon Event handler template class where the Toolbox callback calls
//	a member function of an object

#ifndef _H_TEventHandler
#define _H_TEventHandler
#pragma once

#include <LEventHandler.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

template <class T> class TEventHandler : public LEventHandler {
public:

typedef	 OSStatus (T:: * HandlerFunc)(EventHandlerCallRef, EventRef);

// ---------------------------------------------------------------------------
//	¥ TEventHandler
// ---------------------------------------------------------------------------

TEventHandler()
{
	mHandlerObject	= nil;
	mHandlerFunc	= nil;
}


// ---------------------------------------------------------------------------
//	¥ TEventHandler
// ---------------------------------------------------------------------------

TEventHandler(
	EventTargetRef			inTarget,
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList,
	T*						inHandlerObject,
	HandlerFunc				inFunc)
{
	mHandlerObject	= inHandlerObject;
	mHandlerFunc	= inFunc;

	Install(inTarget, inNumTypes, inTypeList);
}


// ---------------------------------------------------------------------------
//	¥ TEventHandler
// ---------------------------------------------------------------------------

TEventHandler(
	EventTargetRef	inTarget,
	UInt32			inEventClass,
	UInt32			inEventType,
	T*				inHandlerObject,
	HandlerFunc		inFunc)
{
	mHandlerObject	= inHandlerObject;
	mHandlerFunc	= inFunc;

	Install(inTarget, inEventClass, inEventType);
}


// ---------------------------------------------------------------------------
//	¥ ~TEventHandler						Destructor
// ---------------------------------------------------------------------------

virtual
~TEventHandler()
{
}


// ---------------------------------------------------------------------------
//	¥ GetHandlerObject
// ---------------------------------------------------------------------------

T*
GetHandlerObject() const
{
	return mHandlerObject;
}


// ---------------------------------------------------------------------------
//	¥ SetHandlerObject
// ---------------------------------------------------------------------------

void
SetHandlerObject(
	T*	inHandlerObject)
{
	mHandlerObject = inHandlerObject;
}


// ---------------------------------------------------------------------------
//	¥ GetHandlerFunc
// ---------------------------------------------------------------------------

HandlerFunc
GetHandlerFunc() const
{
	return mHandlerFunc;
}


// ---------------------------------------------------------------------------
//	¥ SetHandlerFunc
// ---------------------------------------------------------------------------

void
SetHandlerFunc(
	HandlerFunc		inFunc)
{
	mHandlerFunc = inFunc;
}


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------

using LEventHandler::Install;


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------

OSStatus
Install(
	EventTargetRef			inTarget,
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList,
	T*						inHandlerObject,
	HandlerFunc				inFunc)
{
	mHandlerObject	= inHandlerObject;
	mHandlerFunc	= inFunc;
	
	return LEventHandler::Install(inTarget, inNumTypes, inTypeList);
}


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------

OSStatus
Install(
	EventTargetRef	inTarget,
	UInt32			inEventClass,
	UInt32			inEventType,
	T*				inHandlerObject,
	HandlerFunc		inFunc)
{
	mHandlerObject	= inHandlerObject;
	mHandlerFunc	= inFunc;

	return LEventHandler::Install(inTarget, inEventClass, inEventType);
}


// ---------------------------------------------------------------------------
//	¥ DoEvent
// ---------------------------------------------------------------------------
//	Call member function of the handler object
//
//	Function should return noErr if it handles the event, or
//	eventNotHandledErr if it wants to pass on the event to the next handler
							
virtual
OSStatus
DoEvent(
	EventHandlerCallRef	inCallRef,
	EventRef			inEventRef)
{
	OSStatus	status = eventNotHandledErr;
	
	if ( (mHandlerObject != nil) && (mHandlerFunc != nil) ) {
	
		status = (mHandlerObject->*mHandlerFunc)(inCallRef, inEventRef);
	}
	
	return status;
}


// ===========================================================================
//	Instance Variables
// ===========================================================================

private:
	T*					mHandlerObject;
	HandlerFunc			mHandlerFunc;
};


PP_End_Namespace_PowerPlant

#endif
