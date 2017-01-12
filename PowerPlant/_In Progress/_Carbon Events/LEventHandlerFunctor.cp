// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventHandlerFunctor.cp		PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//
//	A Carbon Event handler where the Toolbox callback calls an ordinary
//	C function


#include <LEventHandlerFunctor.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LEventHandlerFunctor					Default Constructor		  [public]
// ---------------------------------------------------------------------------

LEventHandlerFunctor::LEventHandlerFunctor()
{
	mHandlerFunc = nil;
}


// ---------------------------------------------------------------------------
//	¥ LEventHandlerFunctor					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LEventHandlerFunctor::LEventHandlerFunctor(
	EventTargetRef			inTarget,
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList,
	HandlerFunc				inFunc)
{
	mHandlerFunc = inFunc;
	
	Install(inTarget, inNumTypes, inTypeList);
}


// ---------------------------------------------------------------------------
//	¥ LEventHandlerFunctor					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LEventHandlerFunctor::LEventHandlerFunctor(
	EventTargetRef		inTarget,
	UInt32				inEventClass,
	UInt32				inEventType,
	HandlerFunc			inFunc)
{
	mHandlerFunc = inFunc;

	Install(inTarget, inEventClass, inEventType);
}


// ---------------------------------------------------------------------------
//	¥ ~LEventHandlerFunctor					Destructor				  [public]
// ---------------------------------------------------------------------------

LEventHandlerFunctor::~LEventHandlerFunctor()
{
}


// ---------------------------------------------------------------------------
//	¥ DoEvent														  [public]
// ---------------------------------------------------------------------------
//	Call function to handle the event
//
//	Function should return noErr if it handles the event, or
//	eventNotHandledErr if it wants to pass on the event to the next handler

OSStatus
LEventHandlerFunctor::DoEvent(
		EventHandlerCallRef	inCallRef,
		EventRef			inEventRef)
{
	OSStatus	status = eventNotHandledErr;
	
	if (mHandlerFunc != nil) {
		status = (*mHandlerFunc)(inCallRef, inEventRef);
	}
	
	return status;
}


PP_End_Namespace_PowerPlant
