// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCMAttachment.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	Thanx to Guy Fullerton, Per Nielsen, Pete Gontier, Dair Grant, Rick
//	Aurbach and (especially) David Catmull for ideas and input. Special
//	thanx to Mark D. Gerl for collaborations.
//
//	LCMAttachment implements a contextual menu (using the Contextual
//	Menu Manager CMM).
//
//	To implement CMM support in your application via LCMAttachment:
//
//	-	Add the following files to your project:
//
//			LCMAttachment.cp
//			UCMMUtils.cp
//			PP Cursors.rsrc
//			ContextualMenu (Mac OS system library, CFM-targets only, weak-link)
//
//			Other files may be necessary as well, depending upon
//			your project setup.
//
//	-	Add LCMAttachments to whatever objects (in your PPob's) you
//		wish to have contextual menus, such as EditText's, Window's,
//		etc. Since this is a menu, commands are used for action dispatch
//		(the same way menus work throughout PowerPlant, e.g. LMenu).
//		Due to this, there is a notion of CommandTarget -- the Commander/target
//		to which the menu command will ultimately be dispatched. This
//		can be the Attachment's host, another Pane in the hierarchy,
//		or a Commander object that you specify. Due to this design,
//		the LCMAttachment can be attached to Commander/Pane objects,
//		but doesn't necessarily have to be.
//
//		Set the proprties for your LCMAttachment accordingly. The
//		various CTYP properties are listed below.
//
//	-	Attach an LCMAttachment to your Application object:
//
//			CMyApp::CMyApp()
//			{
//				...
//
//				AddAttachment(new LCMAttachment);
//			}
//
//		This is necessary to handle CM-clicks on non-content portions
//		of Windows, such as inDrag, inGrow, inGoAway, etc.
//
//	-	RegisterClass_(LCMAttachment), if you create any instances
//		from a PPob DataStream.
//
//	-	Ensure UCMMUtils::Initialize() is called somewhere early in
//		your application's startup code. A good location is after
//		Toolbox initialization takes place (and before you create/add
//		any LCMAttachment instances). This call must be made as
//		LCMAttachment uses UCMMUtils and UEnvironment mechanisms
//		to determine OS support at runtime.
//
//	LCMAttachment tries to do as much for you as possible, automatically.
//	It creates a little overhead as it has to scan around, but for
//	the flexibility of not always needing to subclass, ease of
//	changing a resource vs. having to recompile, and aiming for
//	safety over speed (but speed is still an important factor.
//
//	The CTYP layout for LCMAttachment is as follows:
//
//	- Execute Message
//		Should always be set to msg_AnyMessage. LCMAttachment needs to
//		respond to multiple MessageT types, and having the internal
//		mMessage set to msg_AnyMessage is the only way to accomplish
//		this.
//
//	- Execute Host
//		This can be set as you'd like, but typically should be set to
//		true.
//
//	- Host Owns Me
//		This can be set as you'd like, but typically should be set to
//		true.
//
//	- Class ID
//		This can be set as you'd like/need. The default is set to
//		LCMAttachment::class_ID.
//
//	- Default Menu ID
//		The menuID for the actual menu, as passed to ::NewMenu(). The
//		default setting of zero will be interpreted as "generate a
//		unique menuID on the fly" -- currently this is hardcoded
//		to the MENU_Contextual constant. Feel free to change this
//		ID as you need, but do ensure the number is unique
//
//	- CMM Cursor ID
//		The ResIDT of a 'CURS' resource for the CMM cursor. OS support
//		for this cursor (kThemeContextualMenuArrowCursor) was not
//		introduced until Mac OS 8.5 and Appearance Manager 1.1, so
//		under pre-AM 1.1 situations we must use this resource
//		mechanism. Be certain to have "PP Cursors.rsrc" in your
//		project/target.
//
//	- Help Type
//		The type of Help your application supports: no help, Apple
//		Guide, some other help mechanism. This constant is passed
//		to ::ContextualMenuSelect(). LCMAttachment does not
//		implement any help mechanism(s), but does provide a ShowHelp()
//		method where you can hook in your help for this context.
//
//	- Help Item String
//		The menu item text for the Help menu item. This is passed to
//		::ContextualMenuSelect().
//
//	- Command ID's
//		The CMM functions upon menuID's and menu items (like traditional
//		menu selection determination was done). However, PowerPlant
//		is based upon commandID's, and Apple added this support to the
//		Menu Manager with Appearance 1.0.
//
//		In this property, you will list all possible commands (menu items)
//		that could be available for this object in any and all of
//		its possible contexts (there are mechanisms that will filter
//		out commands that do not fit the current context)
//
//		To add commands, select the "Command ID's" title in the Property
//		Inspector, then select "New Command" from the Edit menu
//		(cmd-K) to add commands. The order in which you list the
//		commands will be their display order in the menu.
//
//		Unfortunately, there's no handy popup menu in Constructor nor this
//		CTYP from which to select the commands (as you an in the
//		MENU editor in the menu item Property Inspector). Due to this,
//		you must enter the command manually, e.g. enter "12" for
//		cmd_Cut, or "0" for cmd_Nothing (which inserts a divider line).
//
//	- CmdTarget PaneID
//		The PaneIDT of the CommandTarget for the LCMAttachment. This is
//		mostly useful if CommandTarget is to be some other Pane in the
//		visual hierarchy of that Window/View/PPob. If the CommandTarget
//		is the mOwnerHost or a non-Pane object (e.g. Application object),
//		the default setting of "Unspecified/Use OwnerHost" (which
//		cooresponds to PaneIDT_Unspecified) should suffice. See how
//		FindCommandTarget() works.
//
//	Known limitations (as of 990319)
//
//	-	Contextual Menus in floating windows, tho theoretically possible,
//		do have problems under PowerPlant. First, PowerPlant doesn't
//		allow targetting of floating windows (nor their contents) by
//		default. It can be done, but PowerPlant doesn't provide a
//		generic means for accomplishing this. Second, although the
//		explicit targetting of a mouseDown could circumvent the first
//		point, there are target switch/restore issues that are difficult
//		to address.
//
//	-	Cursor support is hard-coded to a resource for now. This should
//		change to support the kThemeContextualMenuArrowCursor, but will
//		have to wait for UCursor to be updated for AM 1.1.
//
//	-	LCMAttachment makes an assumption that the top-most view is an
//		LWindow (or LWindow subclass).
//
//	-	Dispatching a command that could (in)directly cause the instance
//		of the LCMAttachment that dispatched the command to be deleted
//		out from under itself could cause problems. For example, an
//		LCMAttachment on a Window contains a "Close Window" command (cmd_Close).
//		Selecting this menu item will dispatch cmd_Close in DispatchCommand(),
//		which in turn will ultimately cause the window to be deleted,
//		and all of its contents, including the Attachment, with it. As
//		the stack unwinds, we return to LCMAttachment (and ultimately
//		LAttachment) which accesses data members is what is now invalid
//		memory. From here, who knows what will happen, but certainly bad things.
//		To circumvent this, consider subclassing and posting the command
//		"asynchronously". The Appearance Demo application contains an
//		LOneTimeTask class and demonstrates how this can be cleanly accomplished.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCMAttachment.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LStream.h>
#include <PP_KeyCodes.h>
#include <PP_Resources.h>
#include <UCMMUtils.h>
#include <UCursor.h>
#include <UEnvironment.h>
#include <LWindow.h>
#include <UDesktop.h>
#include <TArrayIterator.h>
#include <UTBAccessors.h>

#include <Fonts.h>
#include <MacWindows.h>

	// for ::ContextualMenuSelect()'s command ID fix. See comments in DoCMMClick
	// for complete information.
#ifndef	PP_CMS_Strip_Cmd_ID_Fix
	#define	PP_CMS_Strip_Cmd_ID_Fix			1	// We always use the fix
#endif

#if PP_CMS_Strip_Cmd_ID_Fix
	#include <UMemoryMgr.h>
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCMAttachment							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LCMAttachment::LCMAttachment()

	: LAttachment(msg_AnyMessage, true)
{
	mMENUid			=	MENU_Contextual;
	mCMMCursorID	=	CURS_CMMCursor;
	mHelpType		=	kCMHelpItemNoHelp;
	mTarget			=	nil;
	mTargetPaneID	=	PaneIDT_Unspecified;

	Init();
}


// ---------------------------------------------------------------------------
//	¥ LCMAttachment							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LCMAttachment::LCMAttachment(
	LCommander*		inTarget,
	PaneIDT			inTargetPaneID)

	: LAttachment(msg_AnyMessage, true)
{
	mMENUid			=	MENU_Contextual;
	mCMMCursorID	=	CURS_CMMCursor;
	mHelpType		=	kCMHelpItemNoHelp;

	mTarget			=	inTarget;
	mTargetPaneID	=	inTargetPaneID;

	Init();
}


// ---------------------------------------------------------------------------
//	¥ LCMAttachment							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LCMAttachment::LCMAttachment(
	LStream*	inStream)

	: LAttachment(inStream)
{
	*inStream >> mMENUid;
	if (mMENUid == 0) {
		mMENUid = MENU_Contextual;
	}

	*inStream >> mCMMCursorID;
	*inStream >> mHelpType;

	inStream->ReadPString(mHelpString);

	UInt16 numCommands;						// Read the command list
	*inStream >> numCommands;

	for (SInt16 i = 1; i <= numCommands; i++) {
		CommandT theCommand;
		*inStream >> theCommand;
		mCommandList.AddItem(theCommand);
	}

	mTarget	= nil;
	*inStream >> mTargetPaneID;

	Init();
}


// ---------------------------------------------------------------------------
//	¥ ~LCMAttachment						Destructor				  [public]
// ---------------------------------------------------------------------------

LCMAttachment::~LCMAttachment()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Private Initializer		 [private]
// ---------------------------------------------------------------------------

void
LCMAttachment::Init()
{
	mMessage = msg_AnyMessage;		// We must respond to multiple message
									//	types, so we will accept any message.
									//	ExecuteSelf() should then filter out
									//	unsupported messages.

		// The Attachment can ExecuteSelf only if the Contextual
		// Menu Manager is present and initialized. Check here and
		// cache the value for speed purposes.
	mCMMInitialized = ( UCMMUtils::IsCMMPresent()
						&& UCMMUtils::IsCMMInitialized()
						&& UEnvironment::HasFeature(env_HasAppearance));
}


// ---------------------------------------------------------------------------
//	¥ SetMessage										[public, virtual]
// ---------------------------------------------------------------------------
//	Specify the message for which an Attachment is executed.
//
//	LCMAttachment's mMessage must be set to msg_AnyMessage as the
//	Attachment must respond to multiple message types.

void
LCMAttachment::SetMessage(
	MessageT	inMessage)
{
	Assert_(inMessage == msg_AnyMessage);

	LAttachment::SetMessage(inMessage);
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf										[protected, virtual]
// ---------------------------------------------------------------------------

void
LCMAttachment::ExecuteSelf(
	MessageT	inMessage,
	void*		ioParam)
{
	SetExecuteHost(true);

		// Ensure the CMM is installed and initialized. If not, just
		// return quietly.
	if (not mCMMInitialized) {
		return;
	}

		// Dispatch to the various messages we handle.

	if (inMessage == msg_AdjustCursor) {

			// Only display the cursor if the control key is depressed
		if (((static_cast<EventRecord*>(ioParam))->modifiers & controlKey) != 0) {

				// Treat CURS_CMMCursor as a special flag to use the
				// Appearance Manager theme cursor for contextual menus
				
			if (mCMMCursorID == CURS_CMMCursor) {
				UCursor::SetThemeCursor(kThemeContextualMenuArrowCursor);
			} else {
				UCursor::SetCursorID(mCMMCursorID);
			}
			
			SetExecuteHost(false);
		}

	} else if (inMessage == msg_Event) {

			// This should only happen in response to the LCMAttachment attached to
			// the Application object (or some other LEventDispatcher)

		EventRecord* theMacEvent = static_cast<EventRecord*>(ioParam);

		if (::IsShowContextualMenuClick(theMacEvent)) {

			WindowPtr	theMacWindowP;
			SInt16		thePart = ::MacFindWindow(theMacEvent->where, &theMacWindowP);

			if ((thePart == inMenuBar)
				|| (thePart == inSysWindow)
				|| (thePart == inDesk)) {

				return;
			}

			LWindow*	theWindow = LWindow::FetchWindowObject(theMacWindowP);
			if (theWindow == nil) {
				return;
			}

			switch (thePart) {

				case inDrag:
				case inGrow:
				case inGoAway:
				case inZoomIn:
				case inZoomOut:
				case inCollapseBox:
				case inProxyIcon: {

						// Bring the window to the front, if necessary
					if (not UDesktop::WindowIsSelected(theWindow)
						&& not UDesktop::FrontWindowIsModal()) {
						theWindow->Select();
						theWindow->Activate();
						theWindow->UpdatePort();
					}

					LCommander*	restoreTarget = theWindow->GetTarget();

						// And display the menu for the window's context

					if (theWindow->IsActive()) {
						const TArray<LAttachment*>*	theAttachments = theWindow->GetAttachmentsList();

						if (theAttachments != nil) {
							TArrayIterator<LAttachment*> iterate(*theAttachments);

							LAttachment*	theAttach;
							while (iterate.Next(theAttach)) {
								LCMAttachment*	theCMAttach = dynamic_cast<LCMAttachment*>(theAttach);
								if (theCMAttach != nil) {

									SMouseDownEvent theMouseDown;
									theMouseDown.wherePort		= theMacEvent->where;
									theWindow->GlobalToPortPoint(theMouseDown.wherePort);
									theMouseDown.whereLocal		= theMouseDown.wherePort;
									theMouseDown.macEvent		= *theMacEvent;
									theMouseDown.delaySelect	= false;

									SetExecuteHost(theCMAttach->Execute(msg_Click, &theMouseDown));

									if (IsCommanderAlive(restoreTarget)) {
										restoreTarget->RestoreTarget();
									}

									break;
								}
							}
						}
					}
				}
				break;

				case inContent: {

						// Select the window
					if (not UDesktop::FrontWindowIsModal()) {
						if (not UDesktop::WindowIsSelected(theWindow)) {
							theWindow->Select();
							theWindow->Activate();
							theWindow->UpdatePort();
						}
					} else if (theWindow != UDesktop::FetchTopModal()) {
						return;
					}

					LCommander*	restoreTarget = theWindow->GetTarget();

						// Starting at the deepest subpane that could respond to this
						// click (which is usually the one that would receive the
						// click normally), check that pane and each of its superView's
						// for an LCMAttachment. As soon as we find one, that's the
						// contextual menu that we'll display.

					Point	theHitPortPoint = theMacEvent->where;
					theWindow->GlobalToPortPoint(theHitPortPoint);
					LPane*	theHitPane = FindSubPaneWithAttachment(
												theWindow->FindDeepSubPaneContaining(
															theHitPortPoint.h, theHitPortPoint.v));

					if ((theHitPane == nil)
						|| (((not theHitPane->IsEnabled()) || (not theHitPane->IsActive())))) {

						theHitPane = theWindow;
					}

					const TArray<LAttachment*>*	theAttachments = theHitPane->GetAttachmentsList();

					if (theAttachments != nil) {
						TArrayIterator<LAttachment*> iterate(*theAttachments);

						LAttachment*	theAttach;
						while (iterate.Next(theAttach)) {
							LCMAttachment* theCMAttach = dynamic_cast<LCMAttachment*>(theAttach);
							if (theCMAttach != nil) {

								SMouseDownEvent theMouseDown;
								theMouseDown.wherePort		= theMacEvent->where;
								theWindow->GlobalToPortPoint(theMouseDown.wherePort);
								theMouseDown.whereLocal		= theMouseDown.wherePort;
								theMouseDown.macEvent		= *theMacEvent;
								theMouseDown.delaySelect	= false;

								SetExecuteHost(theCMAttach->Execute(msg_Click, &theMouseDown));

								if (IsCommanderAlive(restoreTarget)
									&& (LCommander::GetTarget() == theWindow)) {
									restoreTarget->RestoreTarget();
								}

								break;
							}
						}
					}

				}
				break;

			} // end - switch (thePart)

		} // end - if (::IsShowContextualMenuClick())

	} else if (inMessage == msg_Click) {

			// It's a click, but is it a CMM click?
		if (::IsShowContextualMenuClick(
				&(static_cast<SMouseDownEvent*>(ioParam))->macEvent)) {

				// It is; let's handle it. However, first we must switch the
				// target to our object, and update the port.
				// +++ items in floating windows will have problems here as
				// +++ PowerPlant floating windows (and items within them)
				// +++ cannot be targetted by default.

			LCommander*	target = FindCommandTarget();
			if ((target != nil) && LCommander::SwitchTarget(target)) {
				LPane*	ownerAsPane = dynamic_cast<LPane*>(mOwnerHost);
				if (ownerAsPane != nil) {
					ownerAsPane->UpdatePort();
				}

				DoCMMClick((static_cast<SMouseDownEvent*>(ioParam))->macEvent);
				SetExecuteHost(false);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DoCMMClick										[protected, virtual]
// ---------------------------------------------------------------------------
//	Performs the actual processing of the CMM click. Generally you shouldn't
//	need to override this method but instead override the submethods that
//	it calls.

void
LCMAttachment::DoCMMClick(
	const EventRecord&	inMacEvent)
{
		// Create our menu, but don't do anything with it just yet.
	StMenuHandleDisposer	theMenuH(CreateMenu());

	GetContext();							// Obtain the selection

	PreCMSelect(inMacEvent);

	PopulateMenu(theMenuH);					// Build the menu
	FinalizeMenu(theMenuH);					// Perform last-minute touches

	SInt16 menuID = ::GetMenuID(theMenuH);	// Store this off for later

		// According to Apple DTS, there apparently was an off-by-one error
		// which caused the wrong extended menu information to be stripped
		// after the first call to ::ContextualMenuSelect(). Apparently
		// this bug has been fixed in the Appearance 1.0.1 API, which means
		// the bug should only be present in vanilla Mac OS 8 (Appearance 1.0).
		//
		// We compensate for this bug by making a local copy of the main menu's
		// command ID's (submenus are not affected by this bug). Thanx to
		// David Catmull for the workaround code.
		//
		// Since this does present a slight performance and memory issue, it is
		// offered with the PP_CMS_Strip_Cmd_ID_Fix macro to process it out.
		// If you will be supporting Appearance 1.0, you must allow this fix
		// (which is the default behavior). If Appearance 1.0 is not an issue
		// for you, you should be able to safely remove this code by #defining
		// PP_CMS_Strip_Cmd_ID_Fix to 0 (in your prefix file or some such).

	#if PP_CMS_Strip_Cmd_ID_Fix
		SInt16 theItemCount = (SInt16) ::CountMenuItems(theMenuH);
		StPointerBlock commandBlock((Size) (sizeof(UInt32) * theItemCount));
		UInt32 *commandList = reinterpret_cast<UInt32*>((Ptr)commandBlock);

		for (SInt16 i = 1; i <= theItemCount; i++) {
			::GetMenuItemCommandID(theMenuH, i, &commandList[i-1]);
		}
	#endif

	::MacInsertMenu(theMenuH, hierMenu);	// Insert the menu

		// And finally make it all happen
	UInt32	theSelectionType	= kCMNothingSelected;
	SInt16	theMenuID			= 0;
	UInt16	theMenuItem			= 0;

	AEDesc*	selDesc = mSelection;
	if (mSelection.IsNull()) {
		selDesc = nil;
	}

	OSStatus err = ::ContextualMenuSelect(
						theMenuH,
						inMacEvent.where,
						false,
						mHelpType,
						mHelpString,
						selDesc,
						&theSelectionType,
						&theMenuID,
						&theMenuItem);

	if ((err != noErr) && (err != userCanceledErr)) {
		Throw_(err);
	}

	PostCMSelect(inMacEvent);				// Perform any cleanup actions

		// Dispatch the command
	UInt32	theCommand = cmd_Nothing;
	if (theSelectionType == kCMMenuItemSelected) {

	#if PP_CMS_Strip_Cmd_ID_Fix
		if (theMenuID == menuID) {

				// It's the main menu, so we have to get the command
				// from our backup list
			theCommand = commandList[theMenuItem - 1];

		} else
	#endif // PP_CMS_Strip_Cmd_ID_Fix

		{
			MenuHandle selectedMenu = ::GetMenuHandle(theMenuID);

			if (selectedMenu != nil) {
				::GetMenuItemCommandID(selectedMenu, static_cast<SInt16>(theMenuItem), &theCommand);
			}
		}
	}

	DispatchCommand(theSelectionType, static_cast<CommandT>(theCommand));
}


// ---------------------------------------------------------------------------
//	¥ÊPreCMSelect								[protected, virtual]
// ---------------------------------------------------------------------------
//	Provides a means for modifying the click target prior to the menu
//	being built and displayed. Useful if you need to hilite the
//	selection to indicate target and/or proper context, etc.

void
LCMAttachment::PreCMSelect(
	const EventRecord&	/* inMacEvent */)
{
}		// Empty. You must subclass to implement


// ---------------------------------------------------------------------------
//	¥ PostCMSelect								[protected, virtual]
// ---------------------------------------------------------------------------
//	Like PreCMSelect but called after the menu selection has occured, but
//	before the event is dispatched. This is called before command
//	dispatch as it is possible for the this attachment to be deleted in
//	the course of event processing (e.g. a CM to close the host object's
//	window, which deletes the window, its subpanes, and their attachments :)
//	If this was called after command processing and "this" was deleted,
//	the vtable lookup and dispatch to here would cause a crash.

void
LCMAttachment::PostCMSelect(
	const EventRecord&	/* inMacEvent */)
{
}		// Empty. You must subclass to implement


// ---------------------------------------------------------------------------
//	¥ DispatchCommand							[protected, virtual]
// ---------------------------------------------------------------------------
//	Given the menu selection type and the command, find the target and
//	dispatch the command.

void
LCMAttachment::DispatchCommand(
	UInt32		inSelectionType,
	CommandT	inCommand)
{
	switch (inSelectionType) {

		case kCMNothingSelected:
			break;

		case kCMShowHelpSelected:
			ShowHelp();
			break;

		case kCMMenuItemSelected: {
			LCommander*	target = FindCommandTarget();
			if (target != nil) {
				target->ProcessCommand(inCommand);
			}
			break;
		}

		default:
			SignalStringLiteral_("Unknown selection type");
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetContext								[protected, virtual]
// ---------------------------------------------------------------------------
//	Obtain the object specifier for the current selection. Used by
//	::ContextualMenuSelect() to actually determine our context and how
//	to build the menu accordingly (used by CMM plugins).
//
//	You shouldn't need to override this method, however you may need to
//	implement LPane::GetSelection(AEDesc&) in the CommandTarget object.

void
LCMAttachment::GetContext()
{
	mSelection.Dispose();

		// First try the CommandTarget. If that fails, try the host.
		// This is useful if the CommandTarget is not a Pane but
		// the host is (e.g. host is a StaticText and Application
		// object is the CommandTarget).

	LCommander*	targetAsCmdr = FindCommandTarget();
	if (targetAsCmdr != nil) {
		LPane*	targetAsPane = dynamic_cast<LPane*>(targetAsCmdr);
		if (targetAsPane == nil) {
			targetAsPane = dynamic_cast<LPane*>(mOwnerHost);
		}

		if (targetAsPane != nil) {
			targetAsPane->GetSelection(mSelection);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ShowHelp									[protected, virtual]
// ---------------------------------------------------------------------------
//	A hook for when ::ContextualMenuSelect() returns kCMShowHelpSelected.
//	In your subclass you should display your help either through the
//	Apple-recommended AppleGuide, or whatever your help mechanism might
//	be.

void
LCMAttachment::ShowHelp()
{
}	// Empty. You must subclass to implement


// ---------------------------------------------------------------------------
//	¥ CreateMenu								[protected, virtual]
// ---------------------------------------------------------------------------
//	Creates the menu, but does not manipulate the menu (e.g. add items).
//
//	One can optionally specify if the menu should always be Appearance
//	theme-savvy regardless of whether the user has chosen "System-wide
//	Platinum Appearance" in the Appearance control panel or not. This is
//	false by default.


MenuHandle
LCMAttachment::CreateMenu(
	bool	inAlwaysBeThemeSavvy)
{
	return LMenu::MakeNewMacMenu(mMENUid, Str_Empty, inAlwaysBeThemeSavvy);
}


// ---------------------------------------------------------------------------
//	¥ PopulateMenu								[protected, virtual]
// ---------------------------------------------------------------------------
//	Populate the menu with items and commands.

void
LCMAttachment::PopulateMenu(
	MenuHandle	inMenuH)
{
	UInt32	numCommands = mCommandList.GetCount();

	for (ArrayIndexT i = 1; i <= numCommands; i++) {
		CommandT	theCommand = mCommandList[i];
		AddCommand(inMenuH, theCommand);
	}
}


// ---------------------------------------------------------------------------
//	¥ FinalizeMenu								[protected, virtual]
// ---------------------------------------------------------------------------
//	Perform any last minute touches to the menu to ensure it will look
//	correct when displayed.

void
LCMAttachment::FinalizeMenu(
	MenuHandle	inMenuH)
{
	Assert_(inMenuH != nil);

		// Strip any out of place divider lines
	UCMMUtils::StripDoubleDividers(inMenuH);
	UCMMUtils::StripFirstDivider(inMenuH);
	UCMMUtils::StripLastDivider(inMenuH);
}


// ---------------------------------------------------------------------------
//	¥ AddCommand								[protected, virtual]
// ---------------------------------------------------------------------------
//	Adds a command (item) to the contextual menu. Returns true if the command
//	was added (false if not).
//
//	According to Apple Human Interface Guidelines (HIG) for Mac OS 8
//	contextual menus:
//
//		Contextual menus should never supersede menu bar items; there
//		shouldn't be any items in a contextual menu which are not also
//		accessible through the menu bar.
//
//	(After some discussions with Apple engineers, the above guideline could
//	actually be bent and still be ok, e.g. the contextual menu presented
//	by a web browser to go "Back in Frame" or "Load Selected Image").
//
//	You must provide the MenuHandle and the CommandT for the item. If no
//	item text is provided (default), AddCommand will iterate the menu
//	bar looking for the specified command. It's slower than directly adding
//	the item, but allows for syncronization between menus for the same
//	item/command/behavior.
//
//	If this auto-add feature is utilized and the found menu item
//	is disabled, the item will not be added to the contextual menu.
//	If you wish for disabled items to be added, you should pass
//	true for inOverrideDisable. Generally tho you should NOT override this
//	disable feature as Apple's HIG states:
//
//		You should never place a command in a contextual menu which is
//		disabled in or cannot be chosen from another menu in the application.
//
//	Passing cmd_Nothing will insert a divider line.
//
//	NB: No check is done to ensure the given CommandT is "legal", however
//	any invalid CommandT's should be safely and quietly ignored.
//
//	You can also provide the location to insert the command (after
//	what item). Default is to append to the end of the menu.
//
//	Thanx to David Catmull for this idea.

bool
LCMAttachment::AddCommand(
	MenuHandle		inMenuH,
	CommandT		inCommand,
	ConstStringPtr	inItemText,
	SInt16			inAfterItem,
	bool			inOverrideDisable)
{
	Assert_(inMenuH != nil);

	bool	commandWasAdded = false;

	LStr255	theMenuText(inItemText);

	if ((inCommand != cmd_Nothing) && (theMenuText[0] == 0)) {

			// No provided menu text so walk the menubar looking for
			// the corresponding item.
		LMenuBar* theMenuBar = LMenuBar::GetCurrentMenuBar();
		Assert_(theMenuBar != nil);

			// Find the status of the command
		Boolean		itemEnabled		= false;
		Boolean		itemUsesMark	= false;
		UInt16		itemMark		= noMark;

		LCommander*	target = FindCommandTarget();
		if (target != nil) {
			target->ProcessCommandStatus(	inCommand,
											itemEnabled,
											itemUsesMark,
											itemMark,
											theMenuText);
		} else {
			SignalStringLiteral_("target is nil");
		}

		if ((inOverrideDisable == false) && (itemEnabled == false)) {
				// The item is disabled in the menubar, so do not add
				// the item/command to the contextual menu
			return commandWasAdded;
		}

			// Find the actual menu item in the menubar
		LMenu*	theBarMenu = nil;

		while (theMenuBar->FindNextMenu(theBarMenu)) {

			SInt16 theIndex = theBarMenu->IndexFromCommand(inCommand);

			if (theIndex == 0) {
					// Command not in that menu, keep looking
				continue;
			}

				// Command in this menu. Obtain information about the
				// menubar menu and apply the same to the contextual
				// menu.

			MenuHandle theMacMenuH = theBarMenu->GetMacMenuH();
			Assert_(theMacMenuH != nil);

			if (theMenuText[0] == 0) {
				::GetMenuItemText(theMacMenuH, theIndex, theMenuText);		// item text
			}

				// Insert the item, enforcing the bounds for inAfterItem
			SInt16 numItems = (SInt16) ::CountMenuItems(inMenuH);
			if (inAfterItem > numItems) {
				inAfterItem = numItems;
			} else if (inAfterItem < 0) {
				inAfterItem = 0;
			}

			::MacInsertMenuItem(inMenuH, Str_Dummy, inAfterItem);
			::SetMenuItemText(inMenuH, static_cast<SInt16>(inAfterItem + 1), theMenuText);
			::SetMenuItemCommandID(inMenuH, static_cast<SInt16>(inAfterItem + 1), static_cast<UInt32>(inCommand));

				// Made it this far, all must be well
			commandWasAdded = true;
		}

	} else {

			// User provided the necessary information
		if (inCommand == cmd_Nothing) {
				// they want a divider line
			theMenuText = char_Dash;
		}

			// Insert the item, enforcing the bounds for inAfterItem
		SInt16 numItems = (SInt16) ::CountMenuItems(inMenuH);
		if (inAfterItem > numItems) {
			inAfterItem = numItems;
		} else if (inAfterItem < 0) {
			inAfterItem = 0;
		}

			// To ignore metacharacters, must call MacInsertMenuItem()
			// with a dummy item, then set item's text to the
			// proper string
		::MacInsertMenuItem(inMenuH, Str_Dummy, inAfterItem);
		::SetMenuItemText(inMenuH, static_cast<SInt16>(inAfterItem + 1), theMenuText);

		::SetMenuItemCommandID(inMenuH, static_cast<SInt16>(inAfterItem + 1), static_cast<UInt32>(inCommand));

		commandWasAdded = true;
	}

	return commandWasAdded;
}


// ---------------------------------------------------------------------------
//	¥ FindSubPaneWithAttachment						[protected, virtual]
// ---------------------------------------------------------------------------
//	Given an LPane, see if it has an LCMAttachment attached to it. If it does,
//	return a pointer to the LPane. If it does not, obtain the pane's superView
//	and repeat the process. If no instances of the attachment are found, the
//	method returns nil.
//
//	Typically when you call this you will want to pass the deepest possible
//	subpane (FindDeepSubPaneContaining).
//
//	+++ This certainly could be better implemented as a template (where "T"
//	+++ is the attachment we're looking for).

LPane*
LCMAttachment::FindSubPaneWithAttachment(
	LPane*	inPane)
{
	LPane*	targetPane = nil;

	if (inPane == nil) {
		return targetPane;
	}

		// Walk the Attachment list for the passed in Pane. If it
		// has the desired Attachment, we can stop.

	const TArray<LAttachment*>*	theAttachmentList = inPane->GetAttachmentsList();

	if (theAttachmentList != nil) {

		TArrayIterator<LAttachment*>	iterate(*theAttachmentList);
		LAttachment*	theAttachment;

		while (iterate.Next(theAttachment)) {
			LCMAttachment*	theCMAttachment = dynamic_cast<LCMAttachment*>(theAttachment);
			if (theCMAttachment != nil) {
				targetPane = inPane;
				break;
			}
		}
	}


		// Didn't find one yet. Walk up to the superView and try again.
	if (targetPane == nil) {

		LView*	superView = inPane->GetSuperView();
		if (superView != nil) {
			targetPane = FindSubPaneWithAttachment(superView);
		}
	}

	return targetPane;
}


// ---------------------------------------------------------------------------
//	¥ IsCommanderAlive							[protected, virtual]
// ---------------------------------------------------------------------------
//	Given an LCommander, attempt to determine if this object is still
//	valid. By default the search will start from the top commander and
//	iterate through the commander chain tree looking for this object. If it
//	is found in the tree, return true, else return false.
//
//	This function is useful if you have a cached pointer to an LCommander
//	object that could potentially have been deleted.

bool
LCMAttachment::IsCommanderAlive(
	LCommander*		inCommander,
	LCommander*		inStartFrom)
{
	Assert_(inCommander != nil);
	Assert_(inStartFrom != nil);

		// Start at the specified Commander (inStartFrom) and iterate
		// the entire Commander tree looking for the the specified
		// target (inCommander). If the target is found, assume it to still
		// be valid, alive, and a potentially valid Commander Target.

	bool	isAlive = (inStartFrom == inCommander);
	if (not isAlive) {

		TArrayIterator<LCommander*>	iterate(inStartFrom->GetSubCommanders());
		LCommander*					theSub;

		while (iterate.Next(theSub)) {
			isAlive = IsCommanderAlive(inCommander, theSub);
			if (isAlive) {
				break;
			}
		}
	}

	return isAlive;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandTarget							[protected, virtual]
// ---------------------------------------------------------------------------
//	Attempt to locate the CommandTarget (the Commander/Target to which the
//	menu command will ultimately be dispatched).

LCommander*
LCMAttachment::FindCommandTarget()
{
		// The procedure to find the target is as follows:
		//	1.	If we have what seems to be a valid target, validate it.
		//		If valid, use that, else continue along attempting to
		//		find the target.
		//	2.	If a PaneIDT has been specified, attempt to find a Pane
		//		within the mOwnerHost's View. This does assume the
		//		mOwnerHost is an LPane, but it's not fatal if it's not.
		//	3.	If no PaneIDT, attempt to use the mOwnerHost.
		//
		//	In any case, failure will set/leave the target as nil.

	if (mTarget != nil) {
		if (not IsCommanderAlive(mTarget)) {
			mTarget = nil;
			//mTargetPaneID = PaneIDT_Unspecified;	????
		}
	}

	if (mTarget == nil) {
		if (mTargetPaneID != PaneIDT_Unspecified) {
			LView*	container = GetTopmostView(dynamic_cast<LPane*>(mOwnerHost));
			if (container != nil) {
				LPane* targetPane = container->FindPaneByID(mTargetPaneID);
				if (targetPane != nil) {
					mTarget = dynamic_cast<LCommander*>(targetPane);
				}
			}
		} else {
			LCommander*	hostAsCmdr = dynamic_cast<LCommander*>(mOwnerHost);
			if (hostAsCmdr != nil) {
				mTarget = hostAsCmdr;
			}
		}
	}

	Assert_(mTarget != nil);		// Programmer error

	if ((mTarget != nil) && (mTargetPaneID == PaneIDT_Unspecified)) {
		LPane* targAsPane = dynamic_cast<LPane*>(mTarget);
		if (targAsPane != nil) {
			mTargetPaneID = targAsPane->GetPaneID();
		}
	}

	return mTarget;
}


// ---------------------------------------------------------------------------
//	¥ GetTopmostView							[protected, virtual]
// ---------------------------------------------------------------------------
//	Given a Pane, walk up the SuperView's until the top-most SuperView
//	is found and return that. If the given Pane is the top-most SuperView,
//	the given Pane itself is returned.

LView*
LCMAttachment::GetTopmostView(
	LPane*	inStartPane)
{
										// Begin with the start Pane as a
										//   View. Will be nil if start Pane
										//   is nil or is not an LView.
	LView*	theView = dynamic_cast<LView*>(inStartPane);
	
	if (inStartPane != nil) {
										// Look at SuperView of start Pane
		LView*	superView = inStartPane->GetSuperView();
		
		while (superView != nil) {		// Move up view hierarchy until
			theView = superView;		//   reaching a nil SuperView
			superView = theView->GetSuperView();
		}
	}
	
	return theView;
}


// ---------------------------------------------------------------------------
//	¥ SetCommandTarget							[public, virtual]
// ---------------------------------------------------------------------------
//	Set the CommandTarget to the specified Pane. This Pane must also be
//	a Commander. If the Pane cannot be found or the Pane isn't a Commander,
//	the CommandTarget will be set to nil.
//
//	The search for the Pane will be within the View containing the
//	Attachment's mOwnerHost. This requires the mOwnerHost to be an
//	LPane; if it is not, the CommandTarget will be set to nil.

void
LCMAttachment::SetCommandTarget(
	PaneIDT		inTargetPaneID)
{
	Assert_(inTargetPaneID != PaneIDT_Unspecified);

	mTargetPaneID = inTargetPaneID;

	LCommander*	theTarget = nil;

	LView*	container = GetTopmostView(dynamic_cast<LPane*>(mOwnerHost));
	if (container != nil) {
		LPane*	targetAsPane = container->FindPaneByID(inTargetPaneID);
		if (targetAsPane != nil) {
			theTarget = dynamic_cast<LCommander*>(targetAsPane);
		}
	}

	SetCommandTarget(theTarget);
}

PP_End_Namespace_PowerPlant
