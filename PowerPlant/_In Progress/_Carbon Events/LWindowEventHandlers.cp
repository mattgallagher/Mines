// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindowEventHandlers.cp		PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#include <LWindowEventHandlers.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <UCarbonEvents.h>
#include <UCursor.h>
#include <URegions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LWindowEventHandlers
// ---------------------------------------------------------------------------

LWindowEventHandlers::LWindowEventHandlers(
	LWindow*	inWindow)
{
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------
//	¥ ~LWindowEventHandlers
// ---------------------------------------------------------------------------

LWindowEventHandlers::~LWindowEventHandlers()
{
}


// ---------------------------------------------------------------------------
//	¥ InstallEventHandlers
// ---------------------------------------------------------------------------

void
LWindowEventHandlers::InstallEventHandlers()
{
	WindowPtr	windowP = mWindow->GetMacWindow();
	
	if (windowP == nil) {
		return;
	}

	EventTargetRef	eventTarget = ::GetWindowEventTarget(windowP);

	mDrawContent.Install(	eventTarget,
							kEventClassWindow,
							kEventWindowDrawContent,
							this,
						 	&LWindowEventHandlers::DrawContent );

	mActivated.Install(		eventTarget,
							kEventClassWindow,
							kEventWindowActivated,
							this,
							&LWindowEventHandlers::Activated );

	mDeactivated.Install(	eventTarget,
							kEventClassWindow,
							kEventWindowDeactivated,
							this,
							&LWindowEventHandlers::Deactivated );

	mClickContentRgn.Install(
							eventTarget,
							kEventClassWindow,
							kEventWindowClickContentRgn,
							this,
							&LWindowEventHandlers::ClickContentRgn );
							
	mGetMinimumSize.Install(
							eventTarget,
							kEventClassWindow,
							kEventWindowGetMinimumSize,
							this,
							&LWindowEventHandlers::GetMinimumSize );
							
	mGetMaximumSize.Install(
							eventTarget,
							kEventClassWindow,
							kEventWindowGetMaximumSize,
							this,
							&LWindowEventHandlers::GetMaximumSize );
							
	mBoundsChanged.Install(	eventTarget,
							kEventClassWindow,
							kEventWindowBoundsChanged,
							this,
							&LWindowEventHandlers::BoundsChanged );
							
	mZoom.Install(			eventTarget,
							kEventClassWindow,
							kEventWindowZoom,
							this,
							&LWindowEventHandlers::Zoom );

	mClose.Install(			eventTarget,
							kEventClassWindow,
							kEventWindowClose,
							this,
							&LWindowEventHandlers::Close );

	mMouseMoved.Install(	eventTarget,
							kEventClassMouse,
							kEventMouseMoved,
							this,
							&LWindowEventHandlers::MouseMoved );
	
	mInputText.Install(		eventTarget,
							kEventClassTextInput,
							kEventTextInputUnicodeForKeyEvent,
							this,
							&LWindowEventHandlers::InputText );
	
	mShowWindow.Install(	eventTarget,
							kEventClassWindow,
							kEventWindowShown,
							this,
							&LWindowEventHandlers::ShowWindow );
}


// ---------------------------------------------------------------------------
//	¥ DrawContent
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::DrawContent(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	mWindow->Draw(nil);

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ Activated
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::Activated(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	mWindow->Activate();

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ Deactivated
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::Deactivated(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	mWindow->Deactivate();

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ ClickContentRgn
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::ClickContentRgn(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
#if PP_Uses_ContextMenus
	if (::IsShowContextualMenuEvent(inEventRef)) {
		OSStatus		status = eventNotHandledErr;
		Point			globalPt, portPt;
		::GetEventParameter(inEventRef, kEventParamMouseLocation, typeQDPoint,
							nil, sizeof(Point), nil, &globalPt);
		portPt = globalPt;
		mWindow->GlobalToPortPoint(portPt);
		LPane *			subPane = mWindow->FindDeepSubPaneContaining(portPt.h, portPt.v);
		if (subPane != nil) {
			status = subPane->ContextClick(globalPt);
		} else {
			status = mWindow->ContextClick(globalPt);
		}
		if (status != eventNotHandledErr) {
			return status;
		}
	}
#endif
		// Create old-style EventRecord by extracting the
		// appropriate data from the EventRef

	EventRecord		clickEvent;
	UCarbonEvents::MakeOldEventRecord(inEventRef, clickEvent);
						 
	clickEvent.what = mouseDown;
	
		// Allow application attachments to run -- this is needed to support
		// LInPlaceEditField and related objects that depend on application-level
		// attachments being executed in response to mouse clicks.
	if (LCommander::GetTopCommander()->LAttachable::ExecuteAttachments(msg_Event, &clickEvent)) {
		mWindow->ClickInContent(clickEvent);
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ GetMinimumSize
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::GetMinimumSize(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
	Rect	minMaxSize;
	mWindow->GetMinMaxSize(minMaxSize);

	Point		minSize = topLeft(minMaxSize);
	
	::SetEventParameter( inEventRef,
						  kEventParamDimensions,
						  typeQDPoint,
						  sizeof(Point),
						  &minSize );
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ GetMaximumSize
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::GetMaximumSize(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
	Rect	minMaxSize;
	mWindow->GetMinMaxSize(minMaxSize);

	Point		maxSize = botRight(minMaxSize);
	
	::SetEventParameter( inEventRef,
						  kEventParamDimensions,
						  typeQDPoint,
						  sizeof(Point),
						  &maxSize );
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ BoundsChanged
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::BoundsChanged(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
	OSStatus	status;
								  
	UInt32	attributes;
	status = ::GetEventParameter( inEventRef,
								  kEventParamAttributes,
								  typeUInt32, nil,
								  sizeof(UInt32), nil,
								  &attributes );
								  
	mWindow->AdaptToBoundsChange(attributes);

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ Zoom
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::Zoom(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	mWindow->SendAESetZoom();

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ Close
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::Close(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */)
{
	mWindow->ProcessCommand(cmd_Close);

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ MouseMoved
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::MouseMoved(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef)
{
	EventRecord		event;
	UCarbonEvents::MakeOldEventRecord(inEventRef, event);
	
	Rect	contentBounds;
	::GetWindowBounds(mWindow->GetMacWindow(), kWindowContentRgn, &contentBounds);
	
	if (::MacPtInRect(event.where, &contentBounds)) {
		Point	portMouse = event.where;
		mWindow->GlobalToPortPoint(portMouse);
		
		StRegion	mouseRgn;
		mWindow->AdjustContentMouse(portMouse, event, mouseRgn);
		
	} else {
	
		UCursor::SetArrow();
	}

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ InputText
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::InputText(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			inEventRef ) 
{
	EventRecord		event;
	event.what = keyDown;
	event.when = ::EventTimeToTicks(::GetEventTime(inEventRef));
	event.where.h = event.where.v = 0;
	
	EventRef		rawKey;
	::GetEventParameter(inEventRef, kEventParamTextInputSendKeyboardEvent, typeEventRef, nil, 
								sizeof(EventRef), nil, &rawKey);
	UInt32			keyCode = 0;
	UInt32			modifiers = 0;
	char			charCode = 0;
	::GetEventParameter(rawKey, kEventParamKeyMacCharCodes, typeChar, nil, sizeof(char), nil, &charCode);
	::GetEventParameter(rawKey, kEventParamKeyCode, typeUInt32, nil, sizeof(UInt32), nil, &keyCode);
	::GetEventParameter(rawKey, kEventParamKeyModifiers, typeUInt32, nil, sizeof(UInt32), nil, &modifiers);

	event.message = ((keyCode & 0x000000FF) << 8) + charCode;
	event.modifiers = modifiers;
	
		// Check if the keystroke is a Menu Equivalent
	SInt32			menuChoice;
	CommandT		keyCommand	= cmd_Nothing;
	LMenuBar*		theMenuBar	= LMenuBar::GetCurrentMenuBar();

	if (theMenuBar != nil) {
		keyCommand = theMenuBar->FindKeyCommand(event, menuChoice);
	}

	LCommander *	tgt = LCommander::GetTarget();
	if (tgt != nil) {
		if (keyCommand != cmd_Nothing) {
			StUnhiliteMenu	unhiliter;	// Destructor unhilites menu title
			LCommander::SetUpdateCommandStatus(true);
			tgt->ProcessCommand(keyCommand, &menuChoice);
		} else {
			tgt->ProcessKeyPress(event);
		}
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ ShowWindow
// ---------------------------------------------------------------------------

OSStatus
LWindowEventHandlers::ShowWindow(
	EventHandlerCallRef	/* inCallRef */,
	EventRef			/* inEventRef */ ) 
{
	mWindow->Refresh();
	
	return eventNotHandledErr;
}


PP_End_Namespace_PowerPlant
