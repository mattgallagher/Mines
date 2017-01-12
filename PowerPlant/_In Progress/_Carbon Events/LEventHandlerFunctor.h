// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventHandlerFunctor.h		PowerPlant 2.2.2		 ©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Carbon Event handler where the Toolbox callback calls an ordinary
//	C function of the form:
//
//		OSStatus MyEventHandler( EventHandlerCallRef	inCallRef,
//								 EventRef				inEvetnRef );

#ifndef _H_LEventHandlerFunctor
#define _H_LEventHandlerFunctor
#pragma once

#include <LEventHandler.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LEventHandlerFunctor : public LEventHandler {
public:

	typedef	 OSStatus (* HandlerFunc)(EventHandlerCallRef, EventRef);
	
					LEventHandlerFunctor();
					
					LEventHandlerFunctor(
							EventTargetRef			inTarget,
							UInt32					inNumTypes,
							const EventTypeSpec*	inTypeList,
							HandlerFunc				inFunc);

					LEventHandlerFunctor(
							EventTargetRef		inTarget,
							UInt32				inEventClass,
							UInt32				inEventType,
							HandlerFunc			inFunc);
							
	virtual			~LEventHandlerFunctor();
	
	void			SetEventTarget( EventTargetRef inTarget );
	
	HandlerFunc		GetHandlerFunc() const;
	
	void			SetHandlerFunc( HandlerFunc inFunc );
							
	virtual OSStatus	DoEvent(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef);

private:
	HandlerFunc		mHandlerFunc;
};


// ===========================================================================
//	Inline Functions
// ===========================================================================

inline
LEventHandlerFunctor::HandlerFunc
LEventHandlerFunctor::GetHandlerFunc() const
{
	return mHandlerFunc;
}


inline
void
LEventHandlerFunctor::SetHandlerFunc(
	HandlerFunc	inFunc)
{
	mHandlerFunc = inFunc;
}

PP_End_Namespace_PowerPlant

#endif
