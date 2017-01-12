// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTreeWindow.cp				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	LTreeWindow is a Window designed to host LTree (or subclassed) objects.
//	Specifically, it is designed to work with the default setup of the
//	Debugging Classes.
//
//	1.	LDebugMenuAttachment manages the Tree Windows. It will create and
//		attempt to destroy them. But the Tree Windows also utilize the
//		Application object as their supercommander. If the Commander chain
//		is destroyed before the LDebugMenuAttachment is, a possibility
//		for a double-delete arises (delete the Tree Windows via the
//		Commander chain destruction, then delete again in the destructor
//		of LDebugMenuAttachment.
//
//		To circumvent this, we host the LCommanderTree and LPaneTree within
//		this LTreeWindow. LTreeWindow is an LBroadcaster (and the
//		LDebugMenuAttachment will listen to it), and when the Broadcaster
//		dies, the menu attachment is notifed and can cleanup to avoid
//		the double-delete.
//
//	2.	Having the TreeWindow as a Broadcaster and Listener, we can have
//		future expansion, such as putting "action buttons" in the window
//		(a button to refresh now, to start/stop the periodical).


#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTreeWindow.h>
#include <LTree.h>
#include <UWindows.h>
#include <LDebugStream.h>
#include <UReanimator.h>
#include <PP_DebugMacros.h>
#include <PP_DebugConstants.h>
#include <LCommanderTree.h>
#include <LPaneTree.h>
#include <LControl.h>
#include <UMemoryMgr.h>

#include <LowMem.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ MakeTreeWindow									[static, public]
// ---------------------------------------------------------------------------
//	Creates a Tree window from the specified PPob. Can build any type of
//	Tree window (Commander, Pane) as this is determined by reanimation of
//	the PPob.
//
//	Relocates the window to the upper right corner of the main monitor.

LTreeWindow*
LTreeWindow::CreateTreeWindow(
	ResIDT			inPPobID,
	LCommander*		inSuperCommander,
	UInt32			inThreshold)
{
	LWindow* theWindow = LWindow::CreateWindow(inPPobID, inSuperCommander);

	LTreeWindow* treeWindow = DebugCast_(theWindow, LWindow, LTreeWindow);

		// Ensure we have a Tree pane within the window.
	LTree*	theTree = FindPaneByID_(treeWindow, treeWindow->GetTreeID(), LTree);
	UInt32	theThreshold = inThreshold * 60L;
	theTree->SetThreshold(inThreshold);
	if (theThreshold == 0) {
		theTree->StopRepeating();
	}

		// Link up any controls/broadcasters
	UReanimator::LinkListenerToBroadcasters(treeWindow, treeWindow, inPPobID);

		// See if special checkboxes exist (it's not fatal if they do not)
		// and link and establish them.
	{
		StDisableDebugSignal_();
		LControl* theCheck = FindPaneByIDNoThrow_(treeWindow, TreeWindow_CheckAutoRefresh, LControl);
		if (theCheck != nil) {
			theCheck->AddListener(treeWindow);
			if (theTree->IsRepeating()) {
				treeWindow->SetOldThreshold(theTree->GetThreshold());
			}
			theCheck->SetValue(theTree->IsRepeating());
		}
		theCheck = FindPaneByIDNoThrow_(treeWindow, TreeWindow_CheckExtraInfo, LControl);
		if (theCheck != nil) {
			theCheck->AddListener(treeWindow);
			theCheck->SetValue(theTree->GetDisplayExtraInfo());
		}
	}

		// Reposition the window at the right-hand side of the main screen
	GDHandle	gdH			= ::GetMainDevice();
	Rect		gdRect		= (*gdH)->gdRect;
	Rect		structRect	= UWindows::GetWindowStructureRect(treeWindow->GetMacWindow());
	Rect		contRect	= UWindows::GetWindowContentRect(treeWindow->GetMacWindow());

	SInt16	mBarHeight = ::GetMBarHeight();

	treeWindow->MoveWindowTo(static_cast<SInt16>(gdRect.right - (structRect.right - structRect.left)),
							static_cast<SInt16>(gdRect.top + mBarHeight + (contRect.top - structRect.top)));

	treeWindow->Show();

	return treeWindow;
}


// ---------------------------------------------------------------------------
//	¥ LTreeWindow										[public]
// ---------------------------------------------------------------------------
//	Default constructor

LTreeWindow::LTreeWindow()
{
	mTree			= nil;
	mOldThreshold	= kDefault_Threshold;
	mTreeID			= 0;
}


// ---------------------------------------------------------------------------
//	¥ LTreeWindow										[public]
// ---------------------------------------------------------------------------
//	SWindowInfo constructor

LTreeWindow::LTreeWindow(
	const SWindowInfo&	inWindowInfo,
	PaneIDT				inTreeID)

	: LWindow(inWindowInfo)
{
	mTree			= nil;
	mOldThreshold	= kDefault_Threshold;
	mTreeID			= inTreeID;
}


// ---------------------------------------------------------------------------
//	¥ LTreeWindow										[public]
// ---------------------------------------------------------------------------
//	'WIND' resource constructor

LTreeWindow::LTreeWindow(
	ResIDT		inWINDid,
	UInt32		inAttributes,
	LCommander*	inSuperCommander,
	PaneIDT		inTreeID)

	: LWindow(inWINDid, inAttributes, inSuperCommander)
{
	mTree			= nil;
	mOldThreshold	= kDefault_Threshold;
	mTreeID			= inTreeID;
}


// ---------------------------------------------------------------------------
//	¥ LTreeWindow										[public]
// ---------------------------------------------------------------------------
//	"On the fly" constructor

LTreeWindow::LTreeWindow(
	LCommander*		inSuperCommander,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			inProcID,
	UInt32			inAttributes,
	WindowPtr		inBehind,
	PaneIDT			inTreeID)

	: LWindow(inSuperCommander, inGlobalBounds, inTitle, inProcID,
				inAttributes, inBehind)
{
	mTree			= nil;
	mOldThreshold	= kDefault_Threshold;
	mPaneID			= inTreeID;
}


// ---------------------------------------------------------------------------
//	¥ LTreeWindow										[public]
// ---------------------------------------------------------------------------
//	LStream constructor

LTreeWindow::LTreeWindow(
	LStream*	inStream)

	: LWindow(inStream)
{
	mTree			= nil;
	mOldThreshold	= kDefault_Threshold;

	*inStream >> mTreeID;
}


// ---------------------------------------------------------------------------
//	¥ ~LTreeWindow										[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LTreeWindow::~LTreeWindow()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf									[protected, virtual]
// ---------------------------------------------------------------------------

void
LTreeWindow::FinishCreateSelf()
{
	if (mTreeID != 0) {
		mTree = FindPaneByID_(this, mTreeID, LTree);
		ValidateObject_(mTree);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTree											[public, virtual]
// ---------------------------------------------------------------------------

void
LTreeWindow::SetTree(
	LTree*		inTree)
{
	mTree = inTree;

	if (mTree != nil) {
		mTreeID = mTree->GetPaneID();
	} else {
		mTreeID = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTreeID											[public, virtual]
// ---------------------------------------------------------------------------

void
LTreeWindow::SetTreeID(
	PaneIDT		inTreeID)
{
	mTreeID = inTreeID;

	if ((mTreeID != PaneIDT_Undefined) && (mTreeID != PaneIDT_Unspecified)) {
		mTree = FindPaneByID_(this, mTreeID, LTree);
	} else {
		mTree = nil;
	}
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage									[public, virtual]
// ---------------------------------------------------------------------------

void
LTreeWindow::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	Assert_(mTree != nil);

	switch (inMessage) {

		case msg_RefreshTree: {
			mTree->Update();
			break;
		}

		case msg_DumpTree: {

			LStr255	filename;

			if (dynamic_cast<LCommanderTree*>(mTree) != nil) {
				filename = StringLiteral_("Commander Tree");
			} else if (dynamic_cast<LPaneTree*>(mTree) != nil) {
				filename = StringLiteral_("Visual Hierarchy");
			} else {
				filename = StringLiteral_("Unknown Tree");
			}

			mTree->DumpTree(flushLocation_File, filename);
			break;
		}

		case msg_DisplayExtraTreeInfo: {
			mTree->SetDisplayExtraInfo(!mTree->GetDisplayExtraInfo());
			mTree->Update();
			break;
		}

		case msg_AutoRefreshTree: {

			SInt32 value = *(static_cast<SInt32*>(ioParam));
			if (value != 0) {
					// Not auto-refreshing, start.
				mTree->SetThreshold(mOldThreshold);
				mTree->StartRepeating();
			} else {
				mOldThreshold = mTree->GetThreshold();
				mTree->SetThreshold(0);
				mTree->StopRepeating();
			}

			mTree->Update();

			break;
		}

	} // end: switch (inMessage)
}

PP_End_Namespace_PowerPlant
