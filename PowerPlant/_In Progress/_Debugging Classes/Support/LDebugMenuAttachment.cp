// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDebugMenuAttachment.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub, MW IDE team
//
//	A "Debug" menu that provides an interface to the PowerPlant Debugging
//	Classes' capabilities.
//
//	The recommended location to create this Attachment would be in an
//	override to LApplication::Initialize() within your Application object's
//	subclass. If nothing else, the Attachment MUST be created after the
//	menubar is built (after LAppliation::MakeMenuBar() is called) as this
//	Attachment is a menu and won't work very well if there's no menubar
//	into which to install it.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDebugMenuAttachment.h>
#include <LMenuBar.h>
#include <LApplication.h>
#include <UProcess.h>
#include <UResourceMgr.h>
#include <URegistrar.h>

#include <PP_DebugMacros.h>
#include <UHeapUtils.h>
#include <UMemoryEater.h>
#include <UValidPPob.h>
#include <UDebugUtils.h>
#include <UVolume.h>
#include <LCommanderTree.h>
#include <LPaneTree.h>
#include <UCursor.h>
#include <LMenu.h>
#include <UDesktop.h>

#include <Fonts.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant

const OSType			kZoneRangerSignature	= FOUR_CHAR_CODE('mwZR');
const OSType			kHeapBossSignature		= FOUR_CHAR_CODE('HEAP');

LDebugMenuAttachment*	LDebugMenuAttachment::sDebugMenuAttachmentP = nil;


// ---------------------------------------------------------------------------
//	¥ÊInstallDebugMenu									[static, public]
// ---------------------------------------------------------------------------
//	Installs the Debug menu (onto the given Attachable, typically the
//	Application object). Creates the menu using factory defaults, registers
//	Debugging Pane/Attachment classes (it does NOT register other classes
//	such as core Pane (LStdButton) and Appearance (LPushButton) classes), and
//	everything else you need. Provided as a convenience for those that
//	wish to get up and running quickly with factory settings.
//
//	If you wish to customize the behavior of the Debug Menu, look into
//	using the LDebugMenuAttachment(SDebugInfo&) constructor, and overriding
//	other provided routines.

void
LDebugMenuAttachment::InstallDebugMenu(
	LAttachable*	inMenuHost)
{
	Assert_(inMenuHost != nil);		// Cannot ValidateObject_ here as the
									//	Attachable (Application) object may
									//	be created on the stack.

		// Ensure the Tree classes are registered
	RegisterClass_(LCommanderTree);
	RegisterClass_(LPaneTree);
	RegisterClass_(LTreeWindow);

		// Create the menu Attachment
	LDebugMenuAttachment*	theMenu = new LDebugMenuAttachment;
	ValidateObject_(theMenu);

	theMenu->InitDebugMenu();

	inMenuHost->AddAttachment(theMenu);
}


// ---------------------------------------------------------------------------
//	¥ LDebugMenuAttachment								[public]
// ---------------------------------------------------------------------------
//	Default constructor
//
//	Creates the DebugMenuAttachment object but does NOT initialize it. After
//	creating the Attachment and before AddAttachment, YOU must call
//	InitDebugMenu() to actually initialize the menu.
//
//		LDebugMenuAttachment*	theMenu = new LDebugMenuAttachment;
//		theMenu->InitDebugMenu();
//
//		theApplicationObject->AddAttachment(theMenu);
//
//	Since the Attachment responds to multiple messages, the internal
//	message to look for is msg_AnyMessage.

LDebugMenuAttachment::LDebugMenuAttachment()

	: LAttachment(		msg_AnyMessage, true),
	mCompactAction(		LHeapAction::heapAction_Compact),
	mPurgeAction(		LHeapAction::heapAction_Purge),
	mCompactPurgeAction(LHeapAction::heapAction_CompactAndPurge)
#if PP_QC_Support
	, mQCAction(		LHeapAction::heapAction_QCTests)
#endif
{
	SetDebugInfoDefaults(mDebugInfo);

	mZRAlias			= nil;
	mHeapBossAlias		= nil;
	mDebugMenuP			= nil;
	mMenuIconSuiteH		= nil;
	mCommandTreeWindow	= nil;
	mCommandTree		= nil;
	mPaneTreeWindow		= nil;
	mPaneTree			= nil;

#if PP_QC_Support
	mQCTestMenuID		= 0;
	mQCTestListH		= nil;
	mQCStateH			= nil;
	mQCTestCount		= 0;
	mQCActive			= UQC::IsActive();
	mQCTestStatesH		= nil;
#endif

	sDebugMenuAttachmentP = this;

#if PP_QC_Support
	StartRepeating();
#endif

		// A sanity check to ensure InitDebugMenu() is called
	mInitialized		= false;
}


// ---------------------------------------------------------------------------
//	¥ LDebugMenuAttachment								[public]
// ---------------------------------------------------------------------------
//	SDebugInfo constructor
//
//	Creates the DebugMenuAttachment object but does NOT initialize it. After
//	creating the Attachment and before AddAttachment, YOU must call
//	InitDebugMenu() to actually initialize the menu.
//
//		SDebugInfo	theInfo;
//		LDebugMenuAttachment::SetDebugInfoDefaults(theInfo);
//		theInfo.debugMenuIconID = 0;	// e.g. Don't use icon
//
//		LDebugMenuAttachment*	theMenu = new LDebugMenuAttachment(theInfo);
//		theMenu->InitDebugMenu();
//
//		theApplicationObject->AddAttachment(theMenu);
//
//	Since the Attachment responds to multiple messages, the internal
//	message to look for is msg_AnyMessage.

LDebugMenuAttachment::LDebugMenuAttachment(
	SDebugInfo&		inDebugInfo)

	: LAttachment(		msg_AnyMessage, true),
	mCompactAction(		LHeapAction::heapAction_Compact),
	mPurgeAction(		LHeapAction::heapAction_Purge),
	mCompactPurgeAction(LHeapAction::heapAction_CompactAndPurge)
#if PP_QC_Support
	, mQCAction(		LHeapAction::heapAction_QCTests)
#endif
{
	mDebugInfo			= inDebugInfo;

		// Perform checks and modifications to the mDebugInfo for
		// special cases.
	Assert_(mDebugInfo.debugMenuID != 0);

		// CommanderTree and PaneTree threshold times are specified in
		// seconds in the SDebugInfo, but are handled as ticks within
		// LTree. Therefore we need to convert the seconds to ticks here.
	mDebugInfo.commanderTreeThreshold *= 60;
	mDebugInfo.paneTreeThreshold *= 60;

	if (mDebugInfo.commanderTreePaneID == 0)
		mDebugInfo.commanderTreePaneID = TreeWindow_Tree;
	if (mDebugInfo.paneTreePaneID == 0)
		mDebugInfo.paneTreePaneID = TreeWindow_Tree;
	if (mDebugInfo.validPPobEditID == 0)
		mDebugInfo.validPPobEditID = ValidatePPob_EditResIDT;
	if (mDebugInfo.eatMemRadioHandleID == 0)
		mDebugInfo.eatMemRadioHandleID = EatMemoryDialog_RadioHandle;
	if (mDebugInfo.eatMemRadioPtrID == 0)
		mDebugInfo.eatMemRadioPtrID = EatMemoryDialog_RadioPtr;
	if (mDebugInfo.eatMemEditID == 0)
		mDebugInfo.eatMemEditID = EatMemoryDialog_EditAmount;
	if (mDebugInfo.eatMemRadioGroupID == 0)
		mDebugInfo.eatMemRadioGroupID = EatMemoryDialog_RadioGroup;

	mZRAlias			= nil;
	mHeapBossAlias		= nil;
	mDebugMenuP			= nil;
	mMenuIconSuiteH		= nil;
	mCommandTreeWindow	= nil;
	mCommandTree		= nil;
	mPaneTreeWindow		= nil;
	mPaneTree			= nil;

#if PP_QC_Support
	mQCTestMenuID		= 0;
	mQCTestListH		= nil;
	mQCStateH			= nil;
	mQCTestCount		= 0;
	mQCActive			= UQC::IsActive();
	mQCTestStatesH		= nil;
#endif

	sDebugMenuAttachmentP = this;

#if PP_QC_Support
	StartRepeating();
#endif

		// A sanity check to ensure InitDebugMenu() is called
	mInitialized		= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LDebugMenuAttachment								[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LDebugMenuAttachment::~LDebugMenuAttachment()
{
	Assert_(mInitialized == true);

	try {
		DisposeOf_(mCommandTreeWindow);
		mCommandTree = nil;

		Forget_(mPaneTreeWindow);
		mPaneTree = nil;

			// Dispose of the debug menu
		LMenuBar*	theMBar = LMenuBar::GetCurrentMenuBar();

		if ((theMBar != nil) && (mDebugMenuP != nil)) {
			theMBar->RemoveMenu(mDebugMenuP);
			DisposeOfSimple_(mDebugMenuP);
		}

			// Dispose of the menu's icon
		if (mMenuIconSuiteH != nil) {
			::DisposeIconSuite(mMenuIconSuiteH, true);
		}

			// Dispose of the application aliases
		if (mZRAlias != nil) {
			::DisposeHandle(reinterpret_cast<Handle>(mZRAlias));
		}
		if (mHeapBossAlias != nil) {
			::DisposeHandle(reinterpret_cast<Handle>(mHeapBossAlias));
		}

	#if PP_QC_Support
		if (mQCTestListH != nil) {
			::DisposeHandle(reinterpret_cast<Handle>(mQCTestListH));
		}

		if (mQCTestStatesH != nil) {
			::DisposeHandle(reinterpret_cast<Handle>(mQCTestStatesH));
		}

		if (mQCStateH != nil) {
			QCDisposeState(mQCStateH);
		}
	#endif

	} catch(...) {
			// Don't let exceptions propagate
		SignalStringLiteral_("Exception caught in ~LDebugMenuAttachment");
	}

	sDebugMenuAttachmentP = nil;
}


// ---------------------------------------------------------------------------
//	¥ SetDebugInfoDefaults									[static, public]
// ---------------------------------------------------------------------------
//	Initializes the given SDebugInfo structure to "factory default" settings.
//	This is handy to use in conjunction with LDebugMenuAttachment(SDebugInfo&)
//	if you wish to have almost factory settings, but just tweak a couple
//	options.

void
LDebugMenuAttachment::SetDebugInfoDefaults(
	SDebugInfo&		outDebugInfo)
{
		// Establish the mDebugInfo to "factory defaults". These constants
		// are from PP_DebugConstants.h. The Resources they refer to are
		// located in "PP Debug Support.ppob" and "PP Debug Support.rsrc".
		// You must include these files in your project/target(s).
		//
		// Furthermore, the PPob's use widgets from the Appearance Classes.
		// If you do not use the Appearance Classes, you may want to consider
		// using the LDebugMenuAttachment(SDebugInfo&) constructor instead
		// and using either the "classic" PPob's in "PP Debug Support.ppob"
		// or create your own PPob's.
		//
		// Additionally, none of the classes used in the PPob's are
		// registered (URegistrar). If these classes are not already
		// registered in your project, you will need to ensure they are
		// registered.

	outDebugInfo.debugMenuID			= MENU_DebugMenu;
	outDebugInfo.debugMenuIconID		= icsX_DebugMenuTitle;
	outDebugInfo.commanderTreePPobID	= PPob_AMLCommanderTreeWindow;
	outDebugInfo.commanderTreePaneID	= TreeWindow_Tree;
	outDebugInfo.commanderTreeThreshold	= kDefault_Threshold;
	outDebugInfo.paneTreePPobID			= PPob_AMLPaneTreeWindow;
	outDebugInfo.paneTreePaneID			= TreeWindow_Tree;
	outDebugInfo.paneTreeThreshold		= kDefault_Threshold;
	outDebugInfo.validPPobDlogID		= PPob_AMDialogValidatePPob;
	outDebugInfo.validPPobEditID		= ValidatePPob_EditResIDT;
	outDebugInfo.eatMemPPobDlogID		= PPob_AMEatMemoryDialog;
	outDebugInfo.eatMemRadioHandleID	= EatMemoryDialog_RadioHandle;
	outDebugInfo.eatMemRadioPtrID		= EatMemoryDialog_RadioPtr;
	outDebugInfo.eatMemEditID			= EatMemoryDialog_EditAmount;
	outDebugInfo.eatMemRadioGroupID		= EatMemoryDialog_RadioGroup;
}


// ---------------------------------------------------------------------------
//	¥ InitDebugMenu										[public, virtual]
// ---------------------------------------------------------------------------
//	Initializer: build the menu, install the the menu, etc.
//
//	This method MUST be called immediately after the LDebugMenuAttachment
//	object is created (see the comments above the constructors for
//	more details).
//
//	Since this assumes the menubar exists, the DebugMenuAttachment object
//	should be created after LApplication::MakeMenuBar() is called. A good
//	location to create and install this Attachment would be in your
//	Application subclass's override to LApplication::Initialize()

void
LDebugMenuAttachment::InitDebugMenu()
{
	UCursor::SetWatch();		// Might take a little time

		// Obtain the Debug menu
	Assert_(mDebugInfo.debugMenuID != 0);

	mDebugMenuP = new LMenu(mDebugInfo.debugMenuID);
	ValidateSimpleObject_(mDebugMenuP);

		// Install the Debug icon if desired
	if (mDebugInfo.debugMenuIconID != 0) {

			// Before bothering to even get the icon, ensure the Menu can
			// have the icon inserted.
			//
			// From Inside Macintosh: Text, page 7-39, also located at
			// <http://developer.apple.com/techpubs/mac/Text/Text-422.html#HEADING422-0>
			//
			// -----
			//	Using an icon for the menu title
			//
			//	If you wish to have an icon instead of text as the title of
			//	your text service component menu, first create a small-icon suite
			//	(such as 'kcs#', 'kcs4', and 'kcs8') to represent your menu title.
			//	Then, in your menu resource, make the menu title a 5-byte Pascal
			//	string (6 bytes total size), with this format:
			//
			//		Byte	Value
			//		0		$05 (length byte for menu string)
			//		1		$01 (invalid character code)
			//		2Ð5		Handle to icon suite
			//
			//	When the menu is created, the menu bar definition procedure knows
			//	from the values of the first 2 bytes that the final 4 bytes are a
			//	handle to an icon suite, and the procedure will put the icon in
			//	the menu bar.
			//-----
			//
			// The important issue is the length/size of the menu's title. If the
			// title is less than 6 bytes total, unexpected results can and will
			// occur (typically the menu text will be mangled); this is because
			// the MenuInfo::menuData is packed, and forcing the 6 bytes of
			// information necessary for the icon into less than 6 bytes of space
			// will overwrite the first menu item and subsequently corrupt the
			// rest of the menuData.
			//
			// This performs a check to ensure there are at least 6 bytes present
			// by assuming the first byte of the menuData is the length byte of
			// the menu's title (which it should be), and that this is a value
			// of at least 5 (it seems that if there is more space that there
			// are no problems other than a little wasted memory, but don't change
			// this value because the Menu Manager still needs this to determine
			// the proper offset to the next menu item).
			//
			// If there is not enough space, Signal to that effect and do not
			// insert the icon, instead using the menu's existing title. The
			// fix is simple: ensure the title of your menu (in your 'MENU'
			// resource) has at least 5 characters (the default 'MENU' provided
			// in "PP Debug Support.ppob" has a title of "DEBUG", so this is not
			// a problem).
			//
			// One issue: what if the menu's title is more than 5 characters?
			// Some quick tests seem to work ok, but is this guarenteed? The
			// text from IM quoted above doesn't address the over/under 5 character
			// issue, so could this work in the future? Could this break in the
			// future? Uncertain. The best bet, probably, is to ensure your
			// menu's original title is exactly 5 characters long, and happily
			// the word "DEBUG" is just that length.

		MenuHandle debugMenuH = mDebugMenuP->GetMacMenuH();
		Assert_(debugMenuH != nil);		// This should never fail

		OSErr	err = ::GetIconSuite(&mMenuIconSuiteH,
							mDebugInfo.debugMenuIconID, svAllSmallData);

		if ((err == noErr) && (mMenuIconSuiteH != nil)) {

			::HNoPurge(mMenuIconSuiteH);

			#if PP_Target_Carbon

				unsigned char	title[6];
				title[0] = 5;
				title[1] = 1;
				*(reinterpret_cast<long*>((&title[2]))) = reinterpret_cast<long>(mMenuIconSuiteH);

				::SetMenuTitle(debugMenuH, title);

			#else

				Byte titleLength = (**debugMenuH).menuData[0];

				if (titleLength >= 5) {
					(**debugMenuH).menuData[1] = '\1';
					*(reinterpret_cast<long*>((&(**debugMenuH).menuData[2]))) = reinterpret_cast<long>(mMenuIconSuiteH);

				} else {
						// Not enough space for the icon
					SignalStringLiteral_("Debug Menu's original title too short to install Debug icon as menu title.\r\rRefer to the comments in the LDebugMenuAttachment source for further details.");
				}

			#endif

		} else {
				// It shouldn't be fatal that getting the icon failed, as
				// then the title for the Menu specified in the 'MENU'
				// Resource will be used. But Signal of the failure as
				// this is probably user error.
			SignalStringLiteral_("Error obtaining the Debug Menu icon suite.");
		}
	}

		// Install the Debug menu in the menubar. This
		// does assume the menubar already exists (and there is
		// little reason for there not to be one!). If however this
		// does fail but your app does have a menubar, ensure you are
		// creating this Attachment after LApplication::MakeMenuBar()
		// is called (see LApplication::Run()). A good place to create
		// this Attachment is in LApplication::Intialize().
	LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();
	Assert_(theMenuBar != nil);
	theMenuBar->InstallMenu(mDebugMenuP, InstallMenu_AtEnd);

		// Walk the menu and perform any special modifications based
		// upon the runtime environment present
	PreprocessMenu(mDebugMenuP);

		// Initialization successful.
	mInitialized = true;
}


// ---------------------------------------------------------------------------
//	¥ PreprocessMenu									[protected, virtual]
// ---------------------------------------------------------------------------
//	Given an LMenu, walk the menu item by item (including submenus)
//	examining the Command attributed to each item. Depending upon
//	the Command, make any necessary modifications to the menu. This
//	allows the menu to be tweaked as runtime supports necessitate.
//
//	Checks should be made for any supports that must be verified/modified
//	and can only be done at runtime. You should change the menu for
//	things that can be determined when this Attachment is created
//	and should not change during the course of your applicaton's
//	execution. For example, if MacsBug isn't installed it's reasonable
//	to change the menu item text to reflect that here (as it would
//	require a reboot to install MacsBug). It's a matter of putting
//	the menu cosmetic changes here or in EnableCommand(), and some items
//	that have little/no chance of changing are worth putting here to
//	avoid the overhead in EnableCommand().

void
LDebugMenuAttachment::PreprocessMenu(
	LMenu*		inMenu)
{
	Assert_(inMenu != nil);
	LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();
	Assert_(theMenuBar != nil);

		// Walk the menu items one by one checking the associated
		// CommandT and take appropriate action. If the item has
		// a submenu, process the item first and then it's submenu.

	MenuHandle	menuH = inMenu->GetMacMenuH();
#if PP_Target_Carbon
	ThrowIf_(menuH == nil);
#else
	ValidateHandle_((Handle)menuH);
#endif

	for (	SInt16 index = 1;
			index <= ::CountMenuItems(menuH);
			index++ ) {

		CommandT	theCommand = inMenu->CommandFromIndex(index);
		switch (theCommand) {

		//-----
			case cmd_Debugger: {
				if (UDebugUtils::IsADebuggerPresent() == false) {
					::SetMenuItemText(menuH, index, StringLiteral_("Debugger NOT Installed"));
				}
				break;
			}

		//-----
			case cmd_ScrambleHeap: {
				if ((UDebugUtils::IsMacsBugInstalled() == false)
					#if PP_QC_Support
					&& (UQC::IsInstalled() == false)
					#endif
					) {
						::SetMenuItemText(menuH, index, StringLiteral_("Cannot Scramble Heap"));
					}
				break;
			}

		//-----
			case cmd_LaunchMemoryApp: {

					// Look for ZoneRanger
				FSSpec	zrSpec;
				bool	zrPresent = false;

				try {
					StDisableDebugThrow_();
					StDisableDebugSignal_();

						// First search for ZR by querying the desktop databases
						// as this is the fastest method and typically should
						// succeed.
					zrPresent = UVolume::FindApp(kZoneRangerSignature, zrSpec);
				} catch(...) { }

					// If ZoneRanger was found, create and cache and AliasHandle
					// to the app for later use. Else, change the menu item
					// text to reflect the lack of ZoneRanger.
				if (zrPresent) {
					OSErr err = ::NewAlias(nil, &zrSpec, &mZRAlias);
					zrPresent = (err == noErr);
				}
				
				if (not zrPresent) {
					::SetMenuItemText(menuH, index, StringLiteral_("ZoneRanger NOT Present"));
				}

				break;
			}

		//-----
			case cmd_LaunchHeapBoss: {

					// Look for HeapBoss (from HeapManager)
				FSSpec	hbSpec;
				bool	hbPresent = false;

				try {
					StDisableDebugThrow_();
					StDisableDebugSignal_();

						// First search for HB by querying the desktop databases
						// as this is the fastest method and typically should
						// succeed.
					hbPresent = UVolume::FindApp(kHeapBossSignature, hbSpec);
				} catch(...) { }

					// If HeapBoss was found, create and cache and AliasHandle
					// to the app for later use. Else, change the menu item
					// text to reflect the lack of HeapBoss.
				if (hbPresent) {
					OSErr err = ::NewAlias(nil, &hbSpec, &mHeapBossAlias);
					ThrowIfOSErr_(err);
				} else {
					::SetMenuItemText(menuH, index, StringLiteral_("HeapBoss NOT Present"));
				}

				break;
			}


		//-----
			case cmd_DebugNewValidate:
			case cmd_DebugNewReport:
			case cmd_DebugNewForget: {

			#if (PP_DebugNew_Support == 0)

				::SetMenuItemText(menuH, index, StringLiteral_("PP DebugNew Support Disabled"));
				break;

			#elif PP_DebugNew_Support && (DEBUG_NEW < DEBUG_NEW_LEAKS)

				::SetMenuItemText(menuH, index, StringLiteral_("DebugNew Leaks Checking Disabled"));
				break;

			#else

				break;	// Nothing special to do otherwise

			#endif
			}

		//------
			case cmd_DebugNewFlags:
			case cmd_DebugNewFlagsDontFree:
			case cmd_DebugNewFlagsApplZone: {

			#if (PP_DebugNew_Support == 0)

				::SetMenuItemText(menuH, index, StringLiteral_("PP DebugNew Support Disabled"));
				break;

			#elif PP_DebugNew_Support && (DEBUG_NEW < DEBUG_NEW_BASIC)

				::SetMenuItemText(menuH, index, StringLiteral_("DebugNew Disabled"));
				break;

			#else

				break;	// Nothing special to do otherwise

			#endif
			}

		//-----
			case cmd_QCAction:
			case cmd_QCAction1Sec:
			case cmd_QCAction2Sec:
			case cmd_QCAction3Sec:
			case cmd_QCAction4Sec:
			case cmd_QCAction5Sec:
			case cmd_QCActionNow:
			case cmd_QCActionStop:
			case cmd_QCActionRepeat:
			case cmd_QCActionIdle:
			case cmd_QCActivate:
			case cmd_QCSaveState:
			case cmd_QCTests: {

			#if PP_QC_Support

				if (UQC::IsInstalled()) {
					if (theCommand == cmd_QCTests) {
							// Build the tests submenu off this menu item.

							// Check for submenu (eventually this should use the
							// Menu Utilities).
						SInt16	theCmd;
						::GetItemCmd(menuH, index, &theCmd);
						if (theCmd == hMenuCmd) {
								// Has submenu. Obtain ID and build. This technique
								// works because PowerPlant requires the MENU ResIDT to
								// be the same as the menuID in the Resource's data.
							::GetItemMark(menuH, index, &mQCTestMenuID);
							LMenu*	theSubMenu = theMenuBar->FetchMenu(mQCTestMenuID);
							ValidateSimpleObject_(theSubMenu);

								// Query QC for the tests and build the menu
							mQCTestListH = QCGetTestList(&mQCTestCount);
							ThrowIfNil_(mQCTestListH);

							mQCTestStatesH = ::NewHandleClear(mQCTestCount);
							ThrowIfMemFail_(mQCTestStatesH);

							SInt32 testCount = mQCTestCount;
							MenuHandle qcMenuH = theSubMenu->GetMacMenuH();
#if PP_Target_Carbon
							ThrowIf_(qcMenuH == nil);
#else
							ValidateHandle_((Handle)qcMenuH);
#endif

							StHandleLocker lock(reinterpret_cast<Handle>(mQCTestListH));
							QCTestPtr testListP = *mQCTestListH;
							while (testCount) {
								theSubMenu->InsertCommand(StringLiteral_("dummy"), cmd_UseMenuItem, 0xFF);
								::SetMenuItemText(qcMenuH, ::CountMenuItems(qcMenuH), testListP->testName);
								testListP++;
								testCount--;
							}

						} else {
							SignalStringLiteral_("Menu item with cmd_QCTests has no associated submenu");
						}
					}
				} else {
					::SetMenuItemText(menuH, index, StringLiteral_("QC NOT Installed"));
				}

			#else

				::SetMenuItemText(menuH, index, StringLiteral_("PP QC Support Disabled"));

			#endif // PP_QC_Support

				break;
			}

		//-----
			case cmd_gDebugThrow:
			case cmd_gDebugThrowNothing:
			case cmd_gDebugThrowAlert:
			case cmd_gDebugThrowDebugger: {

			#ifndef Debug_Throw
				::SetMenuItemText(menuH, index, StringLiteral_("Debug_Throw NOT Defined"));
			#endif

				break;
			}

		//-----
			case cmd_gDebugSignal:
			case cmd_gDebugSignalNothing:
			case cmd_gDebugSignalAlert:
			case cmd_gDebugSignalDebugger: {

			#ifndef Debug_Signal
				::SetMenuItemText(menuH, index, StringLiteral_("Debug_Signal NOT Defined"));
			#endif

				break;
			 }

		//-----

		} // end: switch (inMenu->CommandFromIndex(item))

	//----------

			// Check for submenu (eventually this should use the
			// Menu Utilities).
		SInt16	theCmd;
		::GetItemCmd(menuH, index, &theCmd);
		if (theCmd == hMenuCmd) {
				// Has submenu. Obtain ID and recurse. This technique
				// works because PowerPlant requires the MENU ResIDT to
				// be the same as the menuID in the Resource's data.
			SInt16 theID;
			::GetItemMark(menuH, index, &theID);
			LMenu*	theSubMenu = theMenuBar->FetchMenu(theID);
			ValidateSimpleObject_(theSubMenu);
			PreprocessMenu(theSubMenu);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ExecuteSelf										[protected, virtual]
// ---------------------------------------------------------------------------

void
LDebugMenuAttachment::ExecuteSelf(
	MessageT		inMessage,
	void*			ioParam)
{
	bool	executeHost = true;

	if (inMessage == msg_CommandStatus) {
		if (EnableCommand(*(static_cast<SCommandStatus*>(ioParam)))) {
			executeHost = false;
		}

	} else if (ObeyCommand(inMessage, ioParam)) {
		executeHost = false;
	}

	SetExecuteHost(executeHost);
}


// ---------------------------------------------------------------------------
//	¥ EnableCommand										[protected, virtual]
// ---------------------------------------------------------------------------
//	Determine the command's menu state (enable, mark, etc.)

bool
LDebugMenuAttachment::EnableCommand(
	SCommandStatus&		ioCommand)
{
	Assert_(mInitialized == true);

	bool	isValidCommand = IsDebugMenuCommand(ioCommand.command);

		// LEventDispatcher::UpdateMenus() initializes ioCommand
		// with certain values. We'll assert those values here to ensure
		// they are set and have not been tampered with. This assumption
		// is made to try to avoid redundant coding below (as it's a big
		// enough method as it is).
		//
		// However, this will only be done if the command is a menu
		// command (as Attachment execution order is not fully
		// guarenteed, and some prior attachment could have modified these
		// items totally legally).

	if (isValidCommand) {
		Assert_(*ioCommand.enabled == false);
		Assert_(*ioCommand.usesMark == false);
		Assert_(ioCommand.name[0] == 0);
	}

#if PP_QC_Support

		// See if the command is part of our QC menu (synthetic commands)

	ResIDT	theMenuID;
	SInt16	theMenuItem;

	if (LCommander::IsSyntheticCommand(ioCommand.command, theMenuID, theMenuItem)) {
		if ((theMenuID == mQCTestMenuID) && (theMenuItem > 0)) {

			ValidateHandle_(mQCTestListH);

			StHandleLocker lock(reinterpret_cast<Handle>(mQCTestListH));

			QCTestPtr	testP	=	*mQCTestListH;
						testP	+=	(theMenuItem - 1);
			QCType		testID	=	testP->testID;

			SInt32	state;
			ThrowIfQCErr_(QCGetTestState(testID, &state));

			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>(state ? checkMark : noMark);
		}

		return isValidCommand;
	}


#endif

	switch (ioCommand.command) {

	//-----
		case cmd_Debugger: {
			if (UDebugUtils::IsADebuggerPresent()) {
				*ioCommand.enabled = true;
			}
			break;
		}

	//------
		case cmd_CompactHeap:
		case cmd_CompactHeapNow:
		case cmd_PurgeHeap:
		case cmd_PurgeHeapNow:
		case cmd_CompactAndPurge:
		case cmd_CompactAndPurgeNow:

	#ifdef Debug_Throw
		case cmd_gDebugThrow:
	#endif
	#ifdef Debug_Signal
		case cmd_gDebugSignal:
	#endif

		case cmd_CommandChain:
		case cmd_PaneTree:
		case cmd_ValidatePPob:
		case cmd_ValidateAllPPobs: {
			*ioCommand.enabled = true;
			break;
		}

	//------
		case cmd_DebugNewValidate:
		case cmd_DebugNewReport:
		case cmd_DebugNewForget: {
		#if PP_DebugNew_Support && (DEBUG_NEW >= DEBUG_NEW_LEAKS)
			*ioCommand.enabled = true;
		#endif
			break;
		}

		case cmd_DebugNewFlags: {
		#if PP_DebugNew_Support && (DEBUG_NEW >= DEBUG_NEW_BASIC)
			*ioCommand.enabled = true;
		#endif
			break;
		}


		case cmd_DebugNewFlagsDontFree: {
		#if PP_DebugNew_Support && (DEBUG_NEW >= DEBUG_NEW_BASIC)
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>((UDebugNew::GetFlags() & dnDontFreeBlocks)
														? checkMark : noMark);
		#endif
			break;
		}

		case cmd_DebugNewFlagsApplZone: {
		#if PP_DebugNew_Support && (DEBUG_NEW >= DEBUG_NEW_BASIC)
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>((UDebugNew::GetFlags() & dnCheckBlocksInApplZone)
														? checkMark : noMark);
		#endif
			break;
		}

	//------
		case cmd_LaunchMemoryApp: {
			*ioCommand.enabled = (mZRAlias != nil);
			break;
		}

	//------
		case cmd_LaunchHeapBoss: {
			*ioCommand.enabled = (mHeapBossAlias != nil);
			break;
		}

	//------
		case cmd_CommandChainShowHide: {
			if (UDesktop::FrontWindowIsModal() == false) {
				*ioCommand.enabled = true;
			}

			if ((mCommandTreeWindow != nil) && (mCommandTreeWindow->IsVisible())) {
				LString::CopyPStr(StringLiteral_("Hide Command Chain"), ioCommand.name);
			} else {
				LString::CopyPStr(StringLiteral_("Show Command Chain"), ioCommand.name);
			}
			break;
		}

 		case cmd_CommandChainNow: {
 			if ((mCommandTreeWindow != nil) && (mCommandTreeWindow->IsVisible())) {
 				*ioCommand.enabled = true;
 			}
			break;
		}

		case cmd_CommandChain1Sec:
		case cmd_CommandChain2Sec:
		case cmd_CommandChain3Sec:
		case cmd_CommandChain4Sec:
		case cmd_CommandChain5Sec: {

			*ioCommand.usesMark = true;

			if ((mCommandTreeWindow != nil) && (mCommandTreeWindow->IsVisible())
				&& (mCommandTree != nil)) {

				*ioCommand.enabled	= true;
				*ioCommand.mark		= ((ioCommand.command - cmd_CommandChain)
										== (mCommandTree->GetThreshold() / 60))
										? static_cast<UInt16>(checkMark)
										: static_cast<UInt16>(noMark);
			} else {
				*ioCommand.mark	= noMark;
			}

			break;
		}

		case cmd_CommandChainStop: {

			*ioCommand.usesMark = true;

			if ((mCommandTreeWindow != nil) && (mCommandTreeWindow->IsVisible())
				&& (mCommandTree != nil)) {

				*ioCommand.enabled	= true;
				*ioCommand.mark		= ((mCommandTree->IsRunning())
										|| (mCommandTree->GetThreshold() != 0))
										? static_cast<UInt16>(noMark)
										: static_cast<UInt16>(checkMark);
			} else {
				*ioCommand.mark = noMark;
			}

			break;
		}

	//------
		case cmd_PaneTreeShowHide:
		{
			if (UDesktop::FrontWindowIsModal() == false) {
				*ioCommand.enabled = true;
			}

			if ((mPaneTreeWindow != nil) && (mPaneTreeWindow->IsVisible())) {
				LString::CopyPStr(StringLiteral_("Hide Visual Hierarchy"), ioCommand.name);
			} else {
				LString::CopyPStr(StringLiteral_("Show Visual Hierarchy"), ioCommand.name);
			}

			break;
		}

 		case cmd_PaneTreeNow: {
 			if ((mPaneTreeWindow != nil) && (mPaneTreeWindow->IsVisible())) {
				*ioCommand.enabled = true;
			}
			break;
		}

		case cmd_PaneTree1Sec:
		case cmd_PaneTree2Sec:
		case cmd_PaneTree3Sec:
		case cmd_PaneTree4Sec:
		case cmd_PaneTree5Sec: {

			*ioCommand.usesMark = true;

			if ((mPaneTreeWindow != nil) && (mPaneTreeWindow->IsVisible())
				&& (mPaneTree != nil)) {

				*ioCommand.enabled	= true;
				*ioCommand.mark		= ((ioCommand.command - cmd_PaneTree)
										== (mPaneTree->GetThreshold() / 60))
										? static_cast<UInt16>(checkMark)
										: static_cast<UInt16>(noMark);
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}

		case cmd_PaneTreeStop: {

			*ioCommand.usesMark = true;

			if ((mPaneTreeWindow != nil) && (mPaneTreeWindow->IsVisible())
				&& (mPaneTree != nil)) {

				*ioCommand.enabled	= true;
				*ioCommand.mark		= ((mPaneTree->IsRunning())
										|| (mPaneTree->GetThreshold() != 0))
										? static_cast<UInt16>(noMark)
										: static_cast<UInt16>(checkMark);
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}

	//------
		case cmd_CompactHeapStop: {
			*ioCommand.enabled	= true;

			*ioCommand.usesMark	= true;
			*ioCommand.mark		= mCompactAction.IsRunning()
									? static_cast<UInt16>(noMark)
									: static_cast<UInt16>(checkMark);
			break;
		}

		case cmd_CompactHeapRepeat: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactAction.IsRepeating()
									&& mCompactAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_CompactHeapIdle: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactAction.IsIdling()
									&& mCompactAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_CompactHeap1Sec:
		case cmd_CompactHeap2Sec:
		case cmd_CompactHeap3Sec:
		case cmd_CompactHeap4Sec:
		case cmd_CompactHeap5Sec: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactAction.IsRunning()
									&& (ioCommand.command - cmd_CompactHeap)
									== mCompactAction.GetSeconds())
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

	//-----
		case cmd_PurgeHeapStop: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= mPurgeAction.IsRunning()
									? static_cast<UInt16>(noMark)
									: static_cast<UInt16>(checkMark);
			break;
		}

		case cmd_PurgeHeapRepeat: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mPurgeAction.IsRepeating()
									&& mPurgeAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_PurgeHeapIdle: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mPurgeAction.IsIdling()
									&& mPurgeAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_PurgeHeap1Sec:
		case cmd_PurgeHeap2Sec:
		case cmd_PurgeHeap3Sec:
		case cmd_PurgeHeap4Sec:
		case cmd_PurgeHeap5Sec: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mPurgeAction.IsRunning()
									&& (ioCommand.command - cmd_PurgeHeap)
									== mPurgeAction.GetSeconds())
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

	//-----
		case cmd_CompactAndPurgeStop: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= mCompactPurgeAction.IsRunning()
									? static_cast<UInt16>(noMark)
									: static_cast<UInt16>(checkMark);
			break;
		}

		case cmd_CompactAndPurgeRepeat: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactPurgeAction.IsRepeating()
									&& mCompactPurgeAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_CompactAndPurgeIdle: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactPurgeAction.IsIdling()
									&& mCompactPurgeAction.GetSeconds() == 0)
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

		case cmd_CompactAndPurge1Sec:
		case cmd_CompactAndPurge2Sec:
		case cmd_CompactAndPurge3Sec:
		case cmd_CompactAndPurge4Sec:
		case cmd_CompactAndPurge5Sec: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= (mCompactPurgeAction.IsRunning()
									&& (ioCommand.command - cmd_CompactAndPurge) ==
									mCompactPurgeAction.GetSeconds())
									? static_cast<UInt16>(checkMark)
									: static_cast<UInt16>(noMark);
			break;
		}

	//-----
		case cmd_ScrambleHeap: {
			if (UDebugUtils::IsMacsBugInstalled()
				#if PP_QC_Support
					|| UQC::IsInstalled()
				#endif
			) {
				*ioCommand.enabled = true;
			}

			break;
		}


	//-----
	#ifdef Debug_Throw
		case cmd_gDebugThrowNothing:
		case cmd_gDebugThrowAlert: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;

			if ((ioCommand.command - cmd_gDebugThrow - 1) == UDebugging::GetDebugThrow()) {
				*ioCommand.mark = checkMark;
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}

		case cmd_gDebugThrowDebugger: {
			*ioCommand.enabled	= UDebugUtils::IsADebuggerPresent();
			*ioCommand.usesMark	= true;

			if (((ioCommand.command - cmd_gDebugThrow - 1) == UDebugging::GetDebugThrow())
				&& (*ioCommand.enabled == true)) {
				*ioCommand.mark = checkMark;
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}
	#endif // Debug_Throw

	#ifdef Debug_Signal
		case cmd_gDebugSignalNothing:
		case cmd_gDebugSignalAlert: {
			*ioCommand.enabled	= true;
			*ioCommand.usesMark	= true;

			if ((ioCommand.command - cmd_gDebugSignal - 1) == UDebugging::GetDebugSignal()) {
				*ioCommand.mark = checkMark;
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}

		case cmd_gDebugSignalDebugger: {
			*ioCommand.enabled	= UDebugUtils::IsADebuggerPresent();
			*ioCommand.usesMark	= true;

			if (((ioCommand.command - cmd_gDebugSignal - 1) == UDebugging::GetDebugSignal())
				&& (*ioCommand.enabled == true)) {
				*ioCommand.mark	= checkMark;
			} else {
				*ioCommand.mark = noMark;
			}
			break;
		}
	#endif // Debug_Signal

	//-----
		case cmd_EatMemorySpecify: {
				// No test is made to check how low memory is (e.g.
				// check to see if the GrowZone had triggered).
			*ioCommand.enabled = true;
			break;
		}

		case cmd_ReleaseEatPools: {
			if (UMemoryEater::GetTotalPoolSize() > 0) {
				*(ioCommand.enabled) = true;
			}
			break;
		}

	//-----
	#if PP_QC_Support

		case cmd_QCAction:
		case cmd_QCActionNow: {
			*ioCommand.enabled = UQC::IsInstalled();
			break;
		}

		case cmd_QCTests: {

			// Individual tests are handled at the beginning, due to
			// the use of synthetic commands
			*ioCommand.enabled = UQC::IsActive();
			break;
		}

		case cmd_QCActivate: {
			*ioCommand.enabled = UQC::IsInstalled();

			if (UQC::IsInstalled()) {
				if (UQC::IsActive()) {
					LString::CopyPStr(StringLiteral_("Deactivate QC"), ioCommand.name);
				} else {
					LString::CopyPStr(StringLiteral_("Activate QC"), ioCommand.name);
				}
			}
			break;
		}

		case cmd_QCSaveState: {
			*ioCommand.enabled = UQC::IsActive();

			if (UQC::IsInstalled()) {
				if (mQCStateH == nil) {
					LString::CopyPStr(StringLiteral_("Save QC State"), ioCommand.name );
				} else {
					LString::CopyPStr(StringLiteral_("Restore QC State"), ioCommand.name);
				}
			}
			break;
		}

		case cmd_QCActionStop: {
			*ioCommand.enabled	= UQC::IsInstalled();
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>(mQCAction.IsRunning()
													? noMark : checkMark);
			break;
		}

		case cmd_QCActionRepeat: {
			*ioCommand.enabled	= UQC::IsInstalled();
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>((mQCAction.IsRepeating()
													&& mQCAction.GetSeconds() == 0)
													? checkMark : noMark);
			break;
		}

		case cmd_QCActionIdle: {
			*ioCommand.enabled	= UQC::IsInstalled();
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>((mQCAction.IsIdling()
													&& mQCAction.GetSeconds() == 0)
													? checkMark : noMark);
			break;
		}

		case cmd_QCAction1Sec:
		case cmd_QCAction2Sec:
		case cmd_QCAction3Sec:
		case cmd_QCAction4Sec:
		case cmd_QCAction5Sec: {
			*ioCommand.enabled	= UQC::IsInstalled();
			*ioCommand.usesMark	= true;
			*ioCommand.mark		= static_cast<UInt16>((mQCAction.IsRunning()
													&& (ioCommand.command - cmd_QCAction) ==
														mQCAction.GetSeconds())
													? checkMark : noMark);
			break;
		}

	#endif // PP_QC_Support

	}

	return isValidCommand;
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand										[public, virtual]
// ---------------------------------------------------------------------------

Boolean
LDebugMenuAttachment::ObeyCommand(
	CommandT		inCommand,
	void*			ioParam)
{
#if !defined(Debug_Throw) && !defined(Debug_Signal)
	#pragma unused(ioParam)
#endif

	Assert_(mInitialized == true);

	bool	cmdHandled = true;

#if PP_QC_Support

	ResIDT	theMenuID;
	SInt16	theMenuItem;

	if (LCommander::IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {
		if ((theMenuID == mQCTestMenuID) && (theMenuItem > 0)) {

			ValidateHandle_(mQCTestListH);
			StHandleLocker lock(reinterpret_cast<Handle>(mQCTestListH));

			QCTestPtr	testP = *mQCTestListH;
			testP += (theMenuItem - 1);
			QCType testID = testP->testID;

			SInt32	state;
			ThrowIfQCErr_(QCGetTestState(testID, &state));
			ThrowIfQCErr_(QCSetTestState(testID, !state));
		}

		return cmdHandled;
	}


#endif

	switch (inCommand) {

	//-----
		case cmd_Debugger: {
			UDebugUtils::Debugger();
			break;
		}

	//-----
		case cmd_CommandChainShowHide: {
			if ((mCommandTreeWindow == nil) && (mDebugInfo.commanderTreePPobID != 0)) {

				mCommandTreeWindow = LTreeWindow::CreateTreeWindow(	mDebugInfo.commanderTreePPobID,
																	LCommander::GetTopCommander(),
																	mDebugInfo.commanderTreeThreshold);

					// If it's nil that's ok, just do nothing
				if (mCommandTreeWindow != nil) {

					mCommandTree = FindPaneByIDNoThrow_(mCommandTreeWindow, mDebugInfo.commanderTreePaneID, LCommanderTree);

						// If there's no LCommanderTree object, all of this is pointless
					if (mCommandTree == nil) {
						SignalStringLiteral_("Cannot find LCommanderTree object within the TreeWindow");
						DisposeOf_(mCommandTreeWindow);
					} else {
						mCommandTreeWindow->AddListener(this);
						mCommandTreeWindow->Show();
					}
				}
			} else {
				if (mCommandTreeWindow->IsVisible()) {
					mCommandTreeWindow->Hide();
				} else {
					mCommandTreeWindow->Show();
				}
			}

			break;
		}

		case cmd_CommandChainNow: {
			if (mCommandTree != nil) {
				mCommandTree->Update();
			}
			break;
		}

		case cmd_CommandChain1Sec:
		case cmd_CommandChain2Sec:
		case cmd_CommandChain3Sec:
		case cmd_CommandChain4Sec:
		case cmd_CommandChain5Sec: {
			if (mCommandTree != nil) {
				mCommandTree->SetThreshold(static_cast<UInt32>((inCommand - cmd_CommandChain) * 60));
				mCommandTree->StartRepeating();
			}
			break;
		}

		case cmd_CommandChainStop: {
			if (mCommandTree != nil) {
				mCommandTree->SetThreshold(0);
				mCommandTree->StopRepeating();
			}
			break;
		}

	//-----
		case cmd_PaneTreeShowHide: {
			if ((mPaneTreeWindow == nil) && (mDebugInfo.paneTreePPobID != 0)) {

				mPaneTreeWindow = LTreeWindow::CreateTreeWindow(mDebugInfo.paneTreePPobID,
																LCommander::GetTopCommander(),
																mDebugInfo.paneTreeThreshold);

					// If it's nil that's ok, just do nothing
				if (mPaneTreeWindow != nil) {

					mPaneTree = FindPaneByIDNoThrow_(mPaneTreeWindow, mDebugInfo.paneTreePaneID, LPaneTree);

						// If there's no LPaneTree object, all of this is pointless.
					if (mPaneTree == nil) {
						SignalStringLiteral_("Cannot find LPaneTree object within the TreeWindow");
						DisposeOf_(mPaneTreeWindow);
					} else {
						mPaneTreeWindow->AddListener(this);
						mPaneTreeWindow->Show();
					}
				}
			} else {
				if (mPaneTreeWindow->IsVisible()) {
					mPaneTreeWindow->Hide();
				} else {
					mPaneTreeWindow->Show();
				}
			}

			break;
		}

		case cmd_PaneTreeNow: {
			if (mPaneTree != nil) {
				mPaneTree->Update();
			}
			break;
		}

		case cmd_PaneTree1Sec:
		case cmd_PaneTree2Sec:
		case cmd_PaneTree3Sec:
		case cmd_PaneTree4Sec:
		case cmd_PaneTree5Sec: {
			if (mPaneTree != nil) {
				mPaneTree->SetThreshold(static_cast<UInt32>((inCommand - cmd_PaneTree) * 60));
				mPaneTree->StartRepeating();
			}
			break;
		}

		case cmd_PaneTreeStop: {
			if (mPaneTree != nil) {
				mPaneTree->SetThreshold(0);
				mPaneTree->StopRepeating();
			}
			break;
		}

	//-----
		case cmd_CompactHeap:
		case cmd_CompactHeapNow: {
			UHeapUtils::CompactHeap();
			break;
		}

		case cmd_CompactHeapStop: {
			mCompactAction.Stop();
			break;
		}

		case cmd_CompactHeapRepeat: {
			mCompactAction.MakeRepeater();
			mCompactAction.Start(0);
			break;
		}

		case cmd_CompactHeapIdle: {
			mCompactAction.MakeIdler();
			mCompactAction.Start(0);
			break;
		}

		case cmd_CompactHeap1Sec:
		case cmd_CompactHeap2Sec:
		case cmd_CompactHeap3Sec:
		case cmd_CompactHeap4Sec:
		case cmd_CompactHeap5Sec: {
			mCompactAction.MakeRepeater();
			mCompactAction.Start((inCommand - cmd_CompactHeap) * 60);
			break;
		}

	//-----
		case cmd_PurgeHeap:
		case cmd_PurgeHeapNow: {
			UHeapUtils::PurgeHeap();
			break;
		}

		case cmd_PurgeHeapStop: {
			mPurgeAction.Stop();
			break;
		}

		case cmd_PurgeHeapRepeat: {
			mPurgeAction.MakeRepeater();
			mPurgeAction.Start(0);
			break;
		}

		case cmd_PurgeHeapIdle: {
			mPurgeAction.MakeIdler();
			mPurgeAction.Start(0);
			break;
		}

		case cmd_PurgeHeap1Sec:
		case cmd_PurgeHeap2Sec:
		case cmd_PurgeHeap3Sec:
		case cmd_PurgeHeap4Sec:
		case cmd_PurgeHeap5Sec: {
			mPurgeAction.MakeRepeater();
			mPurgeAction.Start((inCommand - cmd_PurgeHeap) * 60);
			break;
		}

	//-----
		case cmd_CompactAndPurge:
		case cmd_CompactAndPurgeNow: {
			UHeapUtils::CompactAndPurgeHeap();
			break;
		}

		case cmd_CompactAndPurgeStop: {
			mCompactPurgeAction.Stop();
			break;
		}

		case cmd_CompactAndPurgeRepeat: {
			mCompactPurgeAction.MakeRepeater();
			mCompactPurgeAction.Start(0);
			break;
		}

		case cmd_CompactAndPurgeIdle: {
			mCompactPurgeAction.MakeIdler();
			mCompactPurgeAction.Start(0);
			break;
		}

		case cmd_CompactAndPurge1Sec:
		case cmd_CompactAndPurge2Sec:
		case cmd_CompactAndPurge3Sec:
		case cmd_CompactAndPurge4Sec:
		case cmd_CompactAndPurge5Sec: {
			mCompactPurgeAction.MakeRepeater();
			mCompactPurgeAction.Start((inCommand - cmd_CompactAndPurge) * 60);
			break;
		}

	//-----
		case cmd_ScrambleHeap: {
			UHeapUtils::ScrambleHeap();
			break;
		}

	//-----
	#if PP_DebugNew_Support
		case cmd_DebugNewValidate: {
			UDebugNew::ValidateAll();
			break;
		}

		case cmd_DebugNewReport: {
			UDebugNew::Report();
			break;
		}

		case cmd_DebugNewForget: {
			UDebugNew::Forget();
			break;
		}

		case cmd_DebugNewFlagsDontFree: {
		#if DEBUG_NEW >= DEBUG_NEW_BASIC
			UInt32 newFlag = UDebugNew::GetFlags();
			newFlag ^= dnDontFreeBlocks;
			UDebugNew::SetFlags(newFlag);
		#endif
			break;
		}

		case cmd_DebugNewFlagsApplZone: {
		#if DEBUG_NEW >= DEBUG_NEW_BASIC
			UInt32 newFlag = UDebugNew::GetFlags();
			newFlag ^= dnCheckBlocksInApplZone;
			UDebugNew::SetFlags(newFlag);
		#endif
			break;
		}
	#endif // PP_DebugNew_Support

	//-----
	#ifdef Debug_Throw
		case cmd_gDebugThrow:
		case cmd_gDebugThrowNothing:
		case cmd_gDebugThrowAlert:
		case cmd_gDebugThrowDebugger: {
			SInt16	item = LoWord(*(static_cast<SInt32*>(ioParam)));
			SetDebugThrow_(static_cast<EDebugAction>(item - 1));
			break;
		}
	#endif

	#ifdef Debug_Signal
		case cmd_gDebugSignal:
		case cmd_gDebugSignalNothing:
		case cmd_gDebugSignalAlert:
		case cmd_gDebugSignalDebugger: {
			SInt16	item = LoWord(*(static_cast<SInt32*>(ioParam)));
			SetDebugSignal_(static_cast<EDebugAction>(item - 1));
			break;
		}
	#endif

	//-----
		case cmd_LaunchMemoryApp: {
			(void)UProcess::LaunchApp(mZRAlias);
			break;
		}

	//-----
		case cmd_LaunchHeapBoss: {
			(void)UProcess::LaunchApp(mHeapBossAlias);
			break;
		}
	//-----
		case cmd_EatMemorySpecify: {

			if (mDebugInfo.eatMemPPobDlogID != 0) {
				UMemoryEater::SetParameters(
									DebugCast_(mOwnerHost, LAttachable, LApplication),
									true,
									mDebugInfo.eatMemPPobDlogID,
									mDebugInfo.eatMemEditID,
									mDebugInfo.eatMemRadioGroupID,
									mDebugInfo.eatMemRadioHandleID,
									mDebugInfo.eatMemRadioPtrID);
			}
			break;
		}

		case cmd_ReleaseEatPools: {
			UMemoryEater::DeleteMemoryLists();
			break;
		}


	//-----
	#if PP_QC_Support
		case cmd_QCTests: {
			// Individual tests are handled at the beginning, due to
			// the use of synthetic commands
			break;
		}

		case cmd_QCActivate: {
			if (UQC::IsActive()) {
				if (mQCStateH != nil) {
					UQC::SetState(mQCStateH);
					QCDisposeState(mQCStateH);
					mQCStateH = nil;
				}
				UQC::Deactivate();
			} else {
				UQC::Activate();
			}
			break;
		}

		case cmd_QCSaveState: {
			if (mQCStateH == nil) {
				mQCStateH = UQC::GetState();
			} else {
				UQC::SetState(mQCStateH);
				QCDisposeState(mQCStateH);
				mQCStateH = nil;
			}

			break;
		}

		case cmd_QCAction:
		case cmd_QCActionNow: {
			UQC::CheckHeap();
			UQC::BoundsCheck();
			UQC::ScrambleHeap();
			break;
		}

		case cmd_QCActionStop: {
			mQCAction.Stop();
			break;
		}

		case cmd_QCActionRepeat: {
			mQCAction.MakeRepeater();
			mQCAction.Start(0);
			break;
		}

		case cmd_QCActionIdle: {
			mQCAction.MakeIdler();
			mQCAction.Start(0);
			break;
		}

		case cmd_QCAction1Sec:
		case cmd_QCAction2Sec:
		case cmd_QCAction3Sec:
		case cmd_QCAction4Sec:
		case cmd_QCAction5Sec: {
			mQCAction.MakeRepeater();
			mQCAction.Start((inCommand - cmd_QCAction) * 60);
			break;
		}

	#endif // PP_QC_Support

	//-----
		case cmd_ValidatePPob: {

			if (mDebugInfo.validPPobDlogID != 0) {
				UValidPPob::ValidatePPob(	mDebugInfo.validPPobDlogID,
											mDebugInfo.validPPobEditID);
			}
			break;
		}

		case cmd_ValidateAllPPobs: {
			UValidPPob::ValidateAllPPobs();
			break;
		}

	//-----
		default: {
			cmdHandled = false;
			break;
		}
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage							[public, virtual]
// ---------------------------------------------------------------------------
//	Listen to messages (what else?) :-)
//
//	LDebugMenuAttachment has potential for a double-delete situation. The
//	TreeWindows use the Application object as their supercommander. If the
//	Commander chain dies before the DebugMenuAttachment is deleted, then
//	a double-delete could occur (once by the Commander chain destroying its
//	subcommanders, then again in the DebugMenuAttachment destructor). Since
//	C++ has no way to notify cached pointers that they have become invalid,
//	this is worked around by having the TreeWindow broadcast when it dies; the
//	DebugMenuAttachment listens for that message and then sets its internal
//	cached pointers to nil to avoid the double-delete (Yes, I suppose
//	reference counting could have been used here as well).

void
LDebugMenuAttachment::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	Assert_(mInitialized == true);

	switch (inMessage) {

		case msg_BroadcasterDied: {

			LBroadcaster*	theDeceased = static_cast<LBroadcaster*>(ioParam);

			if (theDeceased == mCommandTreeWindow) {
				mCommandTree		= nil;
				mCommandTreeWindow	= nil;
				LCommander::SetUpdateCommandStatus(true);
			} else if (theDeceased == mPaneTreeWindow) {
				mPaneTree		= nil;
				mPaneTreeWindow	= nil;
				LCommander::SetUpdateCommandStatus(true);
			}

			break;
		} // end case msg_BroadcasterDied

	} // end switch(inMessage)
}


// ---------------------------------------------------------------------------
//	¥ IsDebugMenuCommand					[static, public]
// ---------------------------------------------------------------------------
//	Determines if a given command number is a command for the Debug menu or
//	not.

bool
LDebugMenuAttachment::IsDebugMenuCommand(
	CommandT	inCommand)
{
	bool	isCommand = false;

	// There are 2 types of potential command numbers for the Debug menu:
	//
	//	1. A reserved CommandT between cmd_DebugFirstCommand and
	//		cmd_DebugLastCommand (see PP_DebugConstants.h)
	//	2. A synthetic command for the QC menu.
	//
	//	Although not all of the reserved CommandT's are used, they do
	//	fall within the range of PowerPlant's reserved set. And since
	//	nothing else in PowerPlant uses CommandT's within this reserved
	//	subset, it is safe to assume that any command number within
	//	this range is for the Debug menu.

#if PP_QC_Support

	ResIDT	theMenuID;
	SInt16	theMenuItem;

	if (LCommander::IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {

		if ((sDebugMenuAttachmentP != nil)
			&& (theMenuID == sDebugMenuAttachmentP->mQCTestMenuID)) {
			isCommand = true;
		}
	} else
#endif
	{
		if ((inCommand >= cmd_DebugFirstCommand) && (inCommand <= cmd_DebugLastCommand)) {
			isCommand = true;
		}
	}

	return isCommand;
}


// ---------------------------------------------------------------------------
//	¥ FindUniqueMenuID							[static, public]
// ---------------------------------------------------------------------------
//	Menus must have unique menu ID's (not necessarily the same as the ResIDT
//	of a 'MENU' resource). If you generate a menu on the fly, you still need
//	to ensure the menu ID you use is unique. This method should help you.
//
//	It scans the menu list and all available 'MENU' resources (in case a
//	menu isn't in the menu list but could have the potential to be, e.g. it
//	just hasn't been loaded yet) until it finds an unused ID and then returns
//	that ID. Hopefully this should prevent any conflicts of menu ID's.
//
//	inStartID specifies the MENU ID (not the 'MENU' ResIDT) to start scanning
//	from. Note however that PowerPlant does require the MENU resource ID
//	and the MENU ID to be the same (see comments in LMenu.cp, LMenu(ResIDT)).
//
//	inStopID specifies the ID we stop at (Assert_(inStartID <= inStopID))
//
//	The range of inStartID to inStopID is inclusive.
//
//	Submenus are special. According to IM:MTE 3-95 (MenuRecord::menuID),
//	submenus must use a number between 1 and 235 for applications, and 236 to
//	255 for a desk accessory. If inSubMenu is true, inStartID is ignored and
//	we begin at 1, scanning to inStopID.

ResIDT
LDebugMenuAttachment::FindUniqueMenuID(
	ResIDT	inStartID,
	ResIDT	inStopID,
	bool	inSubMenu)
{
	Assert_(inStartID <= inStopID);

		// If a submenu, sanity check the start/stop ID's
	if (inSubMenu) {
		Assert_((inStartID >= 1) && (inStartID <= 255));
		Assert_((inStopID >= 1) && (inStopID <= 255	));
	}

		// Start by scanning the available MenuHandles
	ResIDT	theID = 0;
	bool	found = false;
	{
		StSpotlightDisable_();	// Spotlight complains when GetMenuHandle
								// returns nil, which is desired here.

		for ( ; inStartID <= inStopID; inStartID++ ) {
			if (::GetMenuHandle(inStartID) == nil) {
					// Found a unique ID
				theID = inStartID;
				found = true;
				break;
			}
		}
	}

		// If a unique ID was found above, no need to check the
		// 'MENU' resources.
	if (not found) {

			// Scan the 'MENU' resources
		StResLoad noLoad;

			// See if the 'MENU' resource exists. If it does,
			// start the whole process over again
		Handle theH = ::GetResource(FOUR_CHAR_CODE('MENU'), theID);
		if (theH != nil) {
			theID = FindUniqueMenuID(static_cast<ResIDT>(theID + 1), inStopID, inSubMenu);
		}
	}

	return theID;
}


// ---------------------------------------------------------------------------
//	The following two methods, DebugObeyCommand and DebugFindCommandStatus,
//	are provided as "hooks" into the debug menu. Situations can arise that
//	force the debug menu to be unaccessable (e.g. a dialog that explicitly
//	disables all menus). If you create such a situation, you can utilize
//	these methods to hook into the debug menu to allow it to be accessed
//	from within those sorts of situations.
//
//	A typical use might be to install these static methods as the default
//	case in the ObeyCommand/FindCommandStatus switch statement.


// ---------------------------------------------------------------------------
//	¥ DebugObeyCommand							[static, public]
// ---------------------------------------------------------------------------
//	Performs the ObeyCommand functionality for the debug menu

bool
LDebugMenuAttachment::DebugObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	bool	cmdHandled = false;

#if PP_Debug
	if (LDebugMenuAttachment::IsDebugMenuCommand(inCommand)) {
		cmdHandled = LCommander::GetTopCommander()->ProcessCommand(inCommand, ioParam);
	}
#else
	#pragma unused(inCommand, ioParam)
#endif

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ DebugFindCommandStatus					[static, public]
// ---------------------------------------------------------------------------
//	Performs the FindCommandStatus for the debug menu

void
LDebugMenuAttachment::DebugFindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
#if PP_Debug
	if (LDebugMenuAttachment::IsDebugMenuCommand(inCommand)) {
		LCommander::GetTopCommander()->ProcessCommandStatus(inCommand,
															outEnabled,
															outUsesMark,
															outMark,
															outName );
	}
#else
	#pragma unused(inCommand, outEnabled, outUsesMark, outMark, outName)
#endif
}



// ---------------------------------------------------------------------------
//	¥ SpendTime									[public, virtual]
// ---------------------------------------------------------------------------
//	It is possible for the state of QC to change yet the Debug menu not
//	be updated to reflect this change (e.g. QC is disabled, menu reflects
//	that state, then QC turned on via its hotkey yet menu still says
//	that QC is disabled). There is currenty no means by which QC can
//	notify an application of this state change, so instead we must poll
//	for this information.
//
//	This Periodical action will constantly query QC to see if the QC
//	state has changed, and if so a menu update will be forced.
//
//	This is a little excessive, but until there is a way for an application
//	to be notified by a QC state change, it must remain. One note is that
//	this support is only enabled if PP_QC_Support is on.

#if PP_QC_Support

void
LDebugMenuAttachment::SpendTime(
	const EventRecord&	/*inMacEvent*/)
{
	bool alreadyUpdating = false; // To perhaps save us some processor time

		// See if it's been turned on or off
	bool currentState = UQC::IsActive();

	if (mQCActive != currentState) {
		LCommander::SetUpdateCommandStatus(true);
		mQCActive = currentState;
		alreadyUpdating = true;
	}

		// Check to see if the status of any QC test has changed. To save
		// time, if there will already be a menu update or QC isn't active,
		// there's no need to bother with this check.

	if (!alreadyUpdating && UQC::IsActive()) {

		Assert_(mQCTestListH != nil);
		Assert_(mQCTestStatesH != nil);

			// Get the current test states
		StClearHandleBlock	currentStatesH(mQCTestCount);
		StHandleLocker		lock(reinterpret_cast<Handle>(mQCTestListH));

		QCTestPtr	testP	= *mQCTestListH;;
		QCType		testID	= qcAllTestsMask;
		SInt32		state	= 0;

		for (SInt32 i = 0; i < mQCTestCount; i++) {
			testID = testP->testID;
			testP++;

			ThrowIfQCErr_(QCGetTestState(testID, &state));

			(reinterpret_cast<SInt8*>(*currentStatesH.Get()))[i] = static_cast<SInt8>(state);
		}

			// See how the current states compare to the old state
		if (BlockCompare(*mQCTestStatesH, *currentStatesH.Get(), static_cast<UInt32>(mQCTestCount)) != 0) {

				// They are different, so update
			LCommander::SetUpdateCommandStatus(true);

				// And copy the new values in
			::BlockMoveData(*currentStatesH.Get(), *mQCTestStatesH, ::GetHandleSize(mQCTestStatesH));
		}
	}
}

#endif // PP_QC_Support

PP_End_Namespace_PowerPlant
