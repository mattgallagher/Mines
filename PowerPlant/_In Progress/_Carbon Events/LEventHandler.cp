// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEventHandler.cp			PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LEventHandler.h>
#include <LString.h>
#include <PP_KeyCodes.h>

#include <exception>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variable

EventHandlerUPP	LEventHandler::sHandlerUPP;


// ---------------------------------------------------------------------------
//	¥ LEventHandler							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LEventHandler::LEventHandler()
{
	mHandlerRef = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LEventHandler						Destructor				  [public]
// ---------------------------------------------------------------------------

LEventHandler::~LEventHandler()
{
	Remove();
}


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------
//	Install handler for a list of events, specified by an array of
//	(class, type) pairs

OSStatus
LEventHandler::Install(
	EventTargetRef			inTarget,
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList)
{
										// Allocate UPP first time through
	if (sHandlerUPP == nil) {
		sHandlerUPP = ::NewEventHandlerUPP(&EventHandlerCallback);
	}
	
	if (mHandlerRef != nil) {			// Already installed. Remove it.
		Remove();
	}

	return ::InstallEventHandler( inTarget,
								  sHandlerUPP,
								  inNumTypes,
								  inTypeList,
								  this,
								  &mHandlerRef );
}


// ---------------------------------------------------------------------------
//	¥ Install
// ---------------------------------------------------------------------------
//	Install handler for one event specified by class and type 

OSStatus
LEventHandler::Install(
	EventTargetRef		inTarget,
	UInt32				inEventClass,
	UInt32				inEventType)
{
	EventTypeSpec	typeSpec = { inEventClass, inEventType };
	
	return Install(inTarget, 1, &typeSpec);
}


// ---------------------------------------------------------------------------
//	¥ Remove
// ---------------------------------------------------------------------------

void
LEventHandler::Remove()
{
	if (mHandlerRef != nil) {
		::RemoveEventHandler(mHandlerRef);
		mHandlerRef = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ AddTypes
// ---------------------------------------------------------------------------

OSStatus
LEventHandler::AddTypes(
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList)
{
	// Assert (mHandlerRef != nil)

	return ::AddEventTypesToHandler(mHandlerRef, inNumTypes, inTypeList);
}


// ---------------------------------------------------------------------------
//	¥ AddOneType
// ---------------------------------------------------------------------------

OSStatus
LEventHandler::AddOneType(
	UInt32		inEventClass,
	UInt32		inEventType)
{
	EventTypeSpec	typeSpec = { inEventClass, inEventType };
	
	return ::AddEventTypesToHandler(mHandlerRef, 1, &typeSpec);
}


// ---------------------------------------------------------------------------
//	¥ RemoveTypes
// ---------------------------------------------------------------------------

OSStatus
LEventHandler::RemoveTypes(
	UInt32					inNumTypes,
	const EventTypeSpec*	inTypeList)
{
	// Assert (mHandlerRef != nil)

	return ::RemoveEventTypesFromHandler(mHandlerRef, inNumTypes, inTypeList);
}


// ---------------------------------------------------------------------------
//	¥ RemoveOneType
// ---------------------------------------------------------------------------

OSStatus
LEventHandler::RemoveOneType(
	UInt32		inEventClass,
	UInt32		inEventType)
{
	EventTypeSpec	typeSpec = { inEventClass, inEventType };
	
	return ::RemoveEventTypesFromHandler(mHandlerRef, 1, &typeSpec);
}


// ---------------------------------------------------------------------------
//	¥ DoEvent										   [pure virtual] [public]
// ---------------------------------------------------------------------------
//	Subclasses must override to handle the event
//
//	Function should return noErr if it handles the event, or
//	eventNotHandledErr if it wants to pass on the event to the next handler

#pragma mark LEventHandler::DoEvent

// OSStatus
// LEventHandler::DoEvent(
//		EventHandlerCallRef	inCallRef,
//		EventRef			inEventRef)


// ---------------------------------------------------------------------------
//	¥ CatchException									  [static] [protected]
// ---------------------------------------------------------------------------
//	Deal with an exception which occurred during event handling
//
//	Returns error code suitable for the return value of a Toolbox
//	event callback function

OSStatus
LEventHandler::CatchException()
{
		// If handler aborts due to an exception, we assume that it
		// failed trying to process the event and return noErr.

	OSStatus		result = noErr;

	try {							// Rethrow exception so we can
		throw;						//   recapture its type
	}
	
	catch (const LException& inException) {
									// A PowerPlant LException. Signal with
									//   the error code and string
		LStr255	msg(StringLiteral_("Exception caught in LEventHandler.\r"
								   "Error Code: "));
		msg += inException.GetErrorCode();
		msg += char_Return;
		msg += inException.GetErrorString();
		SignalString_(msg);
		
									// Return error code stored in
									//   LException object
		result = inException.GetErrorCode();
	}
	
	catch (const PP_STD::exception& error) {
									// A standard C++ exception
		LStr255	msg(StringLiteral_("Standard exception caught in LEventHandler"));
		msg += char_Return;
		msg += error.what();
		SignalString_(msg);
	}
	
	catch (...) {					// All other exception types
		SignalStringLiteral_("Exception caught in LEventHandler");
	}
	
	return result;
}


// ---------------------------------------------------------------------------
//	¥ EventHandlerCallback									[static] [private]
// ---------------------------------------------------------------------------
//	Function installed as the Toolbox callback
//
//	This function forwards the call to the DoEvent() virtual function of
//	the LEventHandler object, a pointer to which is stored in the user data
//	of the Toolbox callback.

pascal
OSStatus
LEventHandler::EventHandlerCallback(
	EventHandlerCallRef	inCallRef,
	EventRef			inEventRef,
	void*				inUserData)
{
	LEventHandler*	handler = static_cast<LEventHandler*>(inUserData);
	
		// Handler should return noErr if it processed the event, and
		// return eventNotHandledErr if it didn't process the event
		// (or wants the system to continue processing the event).
		//
		// Although not currently noted in the documentation, Apple
		// engineers have stated online that returning any error
		// code except eventNotHandledErr causes event handling to
		// stop and the return value is delivered back to the caller.
		
	OSStatus	result;
	
	try {
		result = handler->DoEvent(inCallRef, inEventRef);
	}
	
	catch (...) {					// Can't throw out of a Toolbox callback
		result = CatchException();	// Deal with the exception
	}
	
	return result;
}


PP_End_Namespace_PowerPlant
