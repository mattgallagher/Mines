// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCarbonApp.cp				PowerPlant 2.2.2		 ©2000-2005 Metrowerks Inc.
// ===========================================================================

#include <LCarbonApp.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LModelDirector.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UAppleEventsMgr.h>
#include <UCursor.h>
#include <UEnvironment.h>
#include <UCarbonEvents.h>

#include <Sound.h>

#include <new>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCarbonApp							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCarbonApp::LCarbonApp()

		// Initialize Carbon Event Handlers. These are functors,
		// which are function pointers wrapped in an object.
		// As member variables, they will be destructed along
		// with the Application object. The destructor for a
		// functor removes the event handler.

	: mSuspendHandler(	::GetApplicationEventTarget(),
						kEventClassApplication,
						kEventAppDeactivated,
						&UCarbonEvents::DoSuspend ),

	  mResumeHandler(	::GetApplicationEventTarget(),
						kEventClassApplication,
						kEventAppActivated,
						&UCarbonEvents::DoResume ),
					   
	  mCommandHandler(	::GetApplicationEventTarget(),
	  					kEventClassCommand,
	  					kEventProcessCommand,
	  					&UCarbonEvents::DoProcessCommand ),
					   
	  mMenuEnableHandler(
	  					::GetApplicationEventTarget(),
	  					kEventClassMenu,
  						kEventMenuEnableItems,
	  					&UCarbonEvents::DoMenuEnableItems ),
	  					 
	  mKeyDownHandler(	::GetApplicationEventTarget(),
	  					kEventClassKeyboard,
	  					kEventRawKeyDown,
	  					&UCarbonEvents::DoKeyDown ),
	  					 
	  mKeyRepeatHandler(
	  					::GetApplicationEventTarget(),
	  					kEventClassKeyboard,
	  					kEventRawKeyRepeat,
	  					&UCarbonEvents::DoKeyDown ),
	  					 
	  mMouseMovedHandler(
						::GetApplicationEventTarget(),
	  					kEventClassMouse,
	  					kEventMouseMoved,
	  					&UCarbonEvents::DoMouseMoved )
{
	sTopCommander = this;
	
	UEnvironment::InitEnvironment();
	
	SetUseSubModelList(true);
	SetModelKind(cApplication);
}


// ---------------------------------------------------------------------------
//	¥ ~LCarbonApp							Destructor				  [public]
// ---------------------------------------------------------------------------

LCarbonApp::~LCarbonApp()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Run															  [public]
// ---------------------------------------------------------------------------
//	Run the Application by processing events until quitting

void
LCarbonApp::Run()
{
	try {
		MakeMenuBar();
		MakeModelDirector();
		Initialize();
				
		ForceTargetSwitch(this);
		UCursor::Normalize();
	}
	
	catch (...) {
	
		SignalStringLiteral_("App Initialization failed.");
	}
	
	::RunApplicationEventLoop();
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
LCarbonApp::StartUp()
{
}


// ---------------------------------------------------------------------------
//	¥ MakeMenuBar												   [protected]
// ---------------------------------------------------------------------------
//	Create MenuBar object for this Application
//
//	Override to use a class other than LMenuBar

void
LCarbonApp::MakeMenuBar()
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

	ResIDT	MBARid = MBAR_Standard;
	
	#if PP_Uses_Aqua_MenuBar
	
		if (UEnvironment::HasFeature(env_HasAquaTheme)) {
			MBARid = MBAR_Aqua;
		}
		
	#endif
	
	new LMenuBar(MBARid);
}


// ---------------------------------------------------------------------------
//	¥ MakeModelDirector											   [protected]
// ---------------------------------------------------------------------------
//	Create ModelDirector (AppleEvent handle) object for this Application
//
//	Override to use a class other than LModelDirector

void
LCarbonApp::MakeModelDirector()
{
	new LModelDirector(this);
}


// ---------------------------------------------------------------------------
//	¥ Initialize												   [protected]
// ---------------------------------------------------------------------------
//	Last chance to initialize Application before processing events

void
LCarbonApp::Initialize()
{
}


// ---------------------------------------------------------------------------
//	¥ DoReopenApp												   [protected]
// ---------------------------------------------------------------------------
//	Respond to Reopen Application AppleEvent
//
//	The system sends the reopen application AppleEvent when the user
//	resumes an application that has no open windows.

void
LCarbonApp::DoReopenApp()
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
LCarbonApp::DoPreferences()
{
}	// Do nothing

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DoQuit														  [public]
// ---------------------------------------------------------------------------

void
LCarbonApp::DoQuit(
	SInt32	inSaveOption)
{
	if (AttemptQuit(inSaveOption)) {
		::QuitApplicationEventLoop();
	}
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand													  [public]
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
LCarbonApp::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;
	
	switch (inCommand) {
	
		case cmd_About:
			::Alert(ALRT_About, nil);
			break;
			
		case cmd_Quit:
			DoQuit();
			break;
				
		case cmd_Preferences:
			DoPreferences();
			break;
			
		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
LCarbonApp::FindCommandStatus(
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

#pragma mark -
// ===========================================================================
// ¥ Apple Event Handlers								Apple Event Handlers ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ HandleAppleEvent												  [public]
// ---------------------------------------------------------------------------

void
LCarbonApp::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	long				inAENumber)
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
//	¥ MakeSelfSpecifier											   [protected]
// ---------------------------------------------------------------------------
//	Make an Object Specifier for an Application

void
LCarbonApp::MakeSelfSpecifier(
	AEDesc&		/* inSuperSpecifier */,
	AEDesc&		outSelfSpecifier) const
{
	outSelfSpecifier.descriptorType = typeNull;
	outSelfSpecifier.dataHandle = nil;
}


PP_End_Namespace_PowerPlant
