// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LApplication.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LApplication.h>
#include <LMenuBar.h>
#include <LModelDirector.h>
#include <LPeriodical.h>
#include <LString.h>
#include <LWindow.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UAppleEventsMgr.h>
#include <UCursor.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UEventMgr.h>
#include <UEnvironment.h>
#include <UMemoryMgr.h>
#include <UModalDialogs.h>
#include <UWindows.h>

#include <new>

#include <AEObjects.h>
#include <Devices.h>
#include <Dialogs.h>
#include <LowMem.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LApplication							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LApplication::LApplication()
{
	mState = programState_StartingUp;
	sTopCommander = this;

		// The default sleep time is 6 ticks (0.1 seconds). You may
		// wish to change this, either by setting mSleepTime directly
		// in the constructor of your Application subclass or by
		// calling SetSleepTime()

	mSleepTime = 6;

	UEnvironment::InitEnvironment();

	SetUseSubModelList(true);
	SetModelKind(cApplication);
}


// ---------------------------------------------------------------------------
//	¥ ~LApplication							Destructor				  [public]
// ---------------------------------------------------------------------------

LApplication::~LApplication()
{
}


// ---------------------------------------------------------------------------
//	¥ StartUp													   [protected]
// ---------------------------------------------------------------------------
//	Perform actions at application start up when launched without any
//	documents.
//
//	Override if you wish to perform some default action, such as
//	creating a new, untitled document

void
LApplication::StartUp()
{
}


// ---------------------------------------------------------------------------
//	¥ MakeMenuBar												   [protected]
// ---------------------------------------------------------------------------
//	Create MenuBar object for this Application
//
//	Override to use a class other than LMenuBar

void
LApplication::MakeMenuBar()
{
		// Mac OS X automatically adds an Application menu which contains
		// items such as "Preferences" and "Quit". On earlier systems,
		// programs put these items in their File and/or Edit menus.
		//
		// If you want the same program to run on multiple systems, you
		// may wish to use separate MBAR and MENU resources to avoid
		// duplicate items. In such cases, #define PP_Uses_Aqua_MenuBar
		// to 1 (true) in your prefix file. Then use MBAR_Standard (128)
		// for Mac OS 9 and earlioer menus, and MBAR_Aqua (129) for
		// Mac OS X menus.

	#if TARGET_API_MAC_CARBON
	
		ResIDT	MBARid = MBAR_Standard;
		
		#if PP_Uses_Aqua_MenuBar
		
			if (UEnvironment::HasFeature(env_HasAquaTheme)) {
				MBARid = MBAR_Aqua;
			}
			
		#endif
		
		new LMenuBar(MBARid);
	
	#else
	
		new LMenuBar(MBAR_Initial);
		
	#endif
}


// ---------------------------------------------------------------------------
//	¥ MakeModelDirector											   [protected]
// ---------------------------------------------------------------------------
//	Create ModelDirector (AppleEvent handle) object for this Application
//
//	Override to use a class other than LModelDirector

void
LApplication::MakeModelDirector()
{
	new LModelDirector(this);
}


// ---------------------------------------------------------------------------
//	¥ Initialize												   [protected]
// ---------------------------------------------------------------------------
//	Last chance to initialize Application before processing events

void
LApplication::Initialize()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetState												 [inline] [public]
// ---------------------------------------------------------------------------

#pragma mark LApplication::GetState

//	EProgramState
//	LApplication::GetState()					// Defined inline


// ---------------------------------------------------------------------------
//	¥ SetSleepTime											 [inline] [public]
// ---------------------------------------------------------------------------

#pragma mark LApplication::SetSleepTime

//	void
//	LApplication::SetSleepTime(					// Defined inline
//		UInt32	inSleepTime)


// ---------------------------------------------------------------------------
//	¥ GetSleepTime											 [inline] [public]
// ---------------------------------------------------------------------------

#pragma mark LApplication::GetSleepTime

//	UInt32
//	LApplication::GetSleepTime() const			// Defined inline


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Run															  [public]
// ---------------------------------------------------------------------------
//	Run the Application by processing events until quitting

void
LApplication::Run()
{
	try {
		MakeMenuBar();
		MakeModelDirector();
		Initialize();

		ForceTargetSwitch(this);
		UCursor::Normalize();
		UpdateMenus();

		mState = programState_ProcessingEvents;
	}

	catch (...) {

		// Initialization failed. After signalling, the program
		// will terminate since mState will not have been
		// set to programState_ProcessingEvents.

		SignalStringLiteral_("App Initialization failed.");
	}

	while (mState == programState_ProcessingEvents) {
		try {
			ProcessNextEvent();
		}

			// You should catch all exceptions in your code.
			// If an exception reaches here, we'll signal
			// and continue running.

		catch (PP_STD::bad_alloc) {
			SignalStringLiteral_("bad_alloc (out of memory) exception caught "
								 "in LApplication::Run");
		}

		catch (const LException& inException) {
			CatchException(inException);
		}

		catch (ExceptionCode inError) {
			CatchExceptionCode(inError);
		}

		catch (...) {
			SignalStringLiteral_("Exception caught in LApplication::Run");
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DoReopenApp												   [protected]
// ---------------------------------------------------------------------------
//	Respond to Reopen Application AppleEvent
//
//	The system sends the reopen application AppleEvent when the user
//	resumes an application that has no open windows.

void
LApplication::DoReopenApp()
{
}	// Do nothing


// ---------------------------------------------------------------------------
//	¥ DoPreferences												   [protected]
// ---------------------------------------------------------------------------
//	Handle Preferences settings
//
//	Many programs have a "Preferences" menu item to let the user specify
//	application-level settings. On Mac OS 9 and earlier, this item is
//	often in the Edit menu. On Mac OS X, the system provides a Preferences
//	item in the Application menu.

void
LApplication::DoPreferences()
{
}	// Do nothing


// ---------------------------------------------------------------------------
//	¥ SendAEQuit													  [public]
// ---------------------------------------------------------------------------
//	Send a Quit AppleEvent to this program

void
LApplication::SendAEQuit()
{
	try {
								// Send Quit AppleEvent for recording only
		AppleEvent	quitEvent;
		UAppleEventsMgr::MakeAppleEvent(kCoreEventClass, kAEQuitApplication,
								quitEvent);
		UAppleEventsMgr::SendAppleEvent(quitEvent, ExecuteAE_No);
	}

	catch (...) { }

	DoQuit();
}


// ---------------------------------------------------------------------------
//	¥ DoQuit														  [public]
// ---------------------------------------------------------------------------
//	Respond to request to quit the application
//
//	The application may not actually quit. This function calls AttemptQuit()
//	to verify the quit operation.
//
//	If you call this function from an initialization routine before the
//	event loop starts, the program will not quit. To abort during
//	initialization, you should throw an exception.

void
LApplication::DoQuit(
	SInt32	inSaveOption)
{
	if (AttemptQuit(inSaveOption)) {
		mState = programState_Quitting;
		SetUpdateCommandStatus(false);
	}
}


// ---------------------------------------------------------------------------
//	¥ ProcessNextEvent												  [public]
// ---------------------------------------------------------------------------
//	Retrieve and handle the next event in the event queue

void
LApplication::ProcessNextEvent()
{
	EventRecord		macEvent;

		// When on duty (application is in the foreground), adjust the
		// cursor shape before waiting for the next event. Except for the
		// very first time, this is the same as adjusting the cursor
		// after every event.

	if (IsOnDuty()) {

		UEventMgr::GetMouseAndModifiers(macEvent);
		AdjustCursor(macEvent);
	}

		// Retrieve the next event. Context switch could happen here.

	SetUpdateCommandStatus(false);
	Boolean	gotEvent = ::WaitNextEvent(everyEvent, &macEvent, mSleepTime,
										mMouseRgn);

		// Let Attachments process the event. Continue with normal
		// event dispatching unless suppressed by an Attachment.

	if (LAttachable::ExecuteAttachments(msg_Event, &macEvent)) {
		if (gotEvent) {
			DispatchEvent(macEvent);
		} else {
			UseIdleTime(macEvent);
		}
	}

									// Repeaters get time after every event
	LPeriodical::DevoteTimeToRepeaters(macEvent);

									// Update status of menu items
	if (IsOnDuty() && GetUpdateCommandStatus()) {
		UpdateMenus();
	}
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
LApplication::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	ResIDT	theMenuID;
	SInt16	theMenuItem;
	if (IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {

		if (theMenuID == MENU_Apple) {

			#if PP_Target_Classic
									// Handle selection from the Apple Menu
				Str255	appleItem;
				::GetMenuItemText(GetMenuHandle(theMenuID), theMenuItem, appleItem);
				::OpenDeskAcc(appleItem);

			#endif

		} else {
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
		}

	} else {

		switch (inCommand) {

			case cmd_About:
				ShowAboutBox();
				break;

			case cmd_Quit:
				SendAEQuit();
				break;
				
			case cmd_Preferences:
				DoPreferences();
				break;

			default:
				cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
				break;
		}
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
LApplication::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_About:
		case cmd_Quit:
		case SyntheticCommandForMenu_(MENU_Apple):
			outEnabled = true;
			break;

		case cmd_Undo:
			outEnabled = false;
			::GetIndString(outName, STRx_UndoEdit, str_CantRedoUndo);
			break;

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowAboutBox													  [public]
// ---------------------------------------------------------------------------
//	Display the About Box for the Application
//
//	This implementation just puts up an Alert Box. Override if you wish
//	to display a more elaborate About Box.

void
LApplication::ShowAboutBox()
{
	UModalAlerts::Alert(ALRT_About);
}


// ---------------------------------------------------------------------------
//	¥ CatchException											   [protected]
// ---------------------------------------------------------------------------
//	Called when Run() catches an exception with an LException parameter
//
//	Override to provide a more meaningful error message to the user.

void
LApplication::CatchException(
	const LException&	inException)
{
	LStr255	msg(StringLiteral_("Exception caught in LApplication.\r"
							   "Error Code: "));
	msg += inException.GetErrorCode();
	msg += char_Return;
	msg += inException.GetErrorString();
	SignalString_(msg);
}


// ---------------------------------------------------------------------------
//	¥ CatchExceptionCode										   [protected]
// ---------------------------------------------------------------------------
//	Called when Run() catches an exception with an ExceptionCode parameter
//
//	Override to provide a more meaningful error message to the user.

void
LApplication::CatchExceptionCode(
	ExceptionCode	inError)
{
	LStr255	msg(StringLiteral_("Exception caught in LApplication.\rError Code: "));
	msg += inError;
	SignalString_(msg);
}

#pragma mark -
// ===========================================================================
// ¥ Apple Event Handlers								Apple Event Handlers ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------

void
LApplication::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	SInt32				inAENumber)
{
	switch (inAENumber) {

		case ae_OpenApp:
			StartUp();
			break;

		case ae_ReopenApp:
			DoReopenApp();
			break;
			
		case ae_Preferences:
			DoPreferences();
			break;

		case ae_Quit: {
			DescType	theType;
			Size		theSize;
			SInt32		saveOption = kAEAsk;

			::AEGetParamPtr(&inAppleEvent, keyAESaveOptions,
						typeEnumeration, &theType, &saveOption,
						sizeof(SInt32), &theSize);

				// We'll ignore the error here. Just proceeding with the
				// default of kAEAsk is reasonable.

			DoQuit(saveOption);
			
				// If the program state hasn't changed to "quitting", then
				// the AppleEvent has failed and we must report an error.
				// The most likely reason is that the user chose "cancel"
				// when asked to whether to save the changes to a document.
			
			if (mState != programState_Quitting) {
				ThrowOSErr_(userCanceledErr);
			}
			break;
		}

		case ae_SwitchTellTarget: {
			StAEDescriptor	targD;
			LModelObject	*newTarget = NULL;

			targD.GetOptionalParamDesc(inAppleEvent, keyAEData, typeWildCard);
			if (targD.mDesc.descriptorType != typeNull) {
				StAEDescriptor	token;
				LModelDirector::Resolve(targD.mDesc, token.mDesc);
				newTarget = GetModelFromToken(token);
			}

			SetTellTarget(newTarget);
			break;
		}

		case ae_GetData:
		case ae_GetDataSize:
		case ae_SetData:

				// If we reach this point, no other object has handled
				// this get/set event. That means whatever thing the
				// event is trying to get/set doesn't exist or isn't
				// supported.

			Throw_(errAEEventNotHandled);
			break;

		default:
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,
								outResult, inAENumber);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ CountSubModels												  [public]
// ---------------------------------------------------------------------------

SInt32
LApplication::CountSubModels(
	DescType	inModelID) const
{
	SInt32	count = 0;

	switch (inModelID) {

		case cWindow: {
			WindowPtr	windowP = ::GetWindowList();
			while (windowP != nil) {
				count++;
				windowP = ::MacGetNextWindow(windowP);
			}
			break;
		}

		default:
			count = LModelObject::CountSubModels(inModelID);
			break;
	}

	return count;
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByPosition											  [public]
// ---------------------------------------------------------------------------

void
LApplication::GetSubModelByPosition(
	DescType		inModelID,
	SInt32			inPosition,
	AEDesc&			outToken) const
{
	switch (inModelID) {

		case cWindow: {
			WindowPtr	windowP = UWindows::FindNthWindow((SInt16) inPosition);
			if (windowP != nil) {
				PutInToken(LWindow::FetchWindowObject(windowP), outToken);
			} else {
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}

		default:
			LModelObject::GetSubModelByPosition(inModelID, inPosition,
													outToken);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetSubModelByName												  [public]
// ---------------------------------------------------------------------------
//	Pass back a token to a SubModel specified by name

void
LApplication::GetSubModelByName(
	DescType		inModelID,
	Str255			inName,
	AEDesc&			outToken) const
{
	switch (inModelID) {

		case cWindow: {
			WindowPtr	windowP = UWindows::FindNamedWindow(inName);
			if (windowP != nil) {
				PutInToken(LWindow::FetchWindowObject(windowP), outToken);
			} else {
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}

		default:
			LModelObject::GetSubModelByName(inModelID, inName, outToken);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPositionOfSubModel											  [public]
// ---------------------------------------------------------------------------
//	Return the position (1 = first) of a SubModel within an Application

SInt32
LApplication::GetPositionOfSubModel(
	DescType				inModelID,
	const LModelObject*		inSubModel) const
{
	SInt32	position;

	switch (inModelID) {

		case cWindow: {
			const LWindow*	window = dynamic_cast<const LWindow*>(inSubModel);
			if (window != nil) {
				position = UWindows::FindWindowIndex(window->GetMacWindow());
			} else {
				Throw_(errAENoSuchObject);
			}
			break;
		}

		default:
			position = LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
			break;
	}

	return position;
}


// ---------------------------------------------------------------------------
//	¥ MakeSelfSpecifier											   [protected]
// ---------------------------------------------------------------------------
//	Make an Object Specifier for an Application

void
LApplication::MakeSelfSpecifier(
	AEDesc&		/* inSuperSpecifier */,
	AEDesc&		outSelfSpecifier) const
{
	outSelfSpecifier.descriptorType = typeNull;
	outSelfSpecifier.dataHandle = nil;
}


PP_End_Namespace_PowerPlant
