// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPaneTree.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub. Thanks to Greg Bolsinga.
//
//	Inspired by LCommanderTree, LPaneTree displays a representation of the
//	Visual Hierarchy of a PowerPlant Window/View. This is similar to the
//	"Hierarchy Window" in Constructor, but whereas that displays the
//	hierarchy present in a PPob, LPaneTree will display the hierarchy at
//	runtime. It will display: the Pane by typename, PaneIDT, states (active,
//	enabled, visible), and descriptor (if any). Furthermore, it can
//	optionally display Attachment and Broadcaster/Listener information, and
//	it also will hilite the Pane currently under the mouse (for ease of
//	locating objects within the hierarchy).
//
//	To try to reduce drawing, abbreviations are used when possible. A "full"
//	line for a Pane would look similar to this:
//
//		LEditField - ID: 1 - State: AEV - Desc: Hello World
//
//		- The name/type of the object (via RTTI)
//		- The PaneIDT. Will display as integer or FourCharCode automatically
//		- The states of the object: (A)ctive, (E)nabled, (V)isible. If
//		  the state is not displayed, the state is opposite (e.g "EV"
//		  would be a deactivated, enabled, visible pane).
//		- The descriptor of the pane, if any.
//
//	A line for an Attachment would look similar to this:
//
//		LBeepAttachment - Message: 814 - Execute Host: true
//
//		- The name/type of the object (via RTTI)
//		- The MessageT to respond to (as integer)
//		- If ExecuteHost or not.
//
//	A line for a Broadcaster or Listener would look similar to this:
//
//		LRadioButton - ID: 1 - State: AEV - Desc: First Radio
//			Broadcasts to: LRadioGroupView (0)
//
//		LRadioGroupView - ID: 0 - State: AEV
//			Listens to: LRadioButton (1), LRadioButton (2)
//
//		- The Broadcaster/Listener information is always on the line
//		  following the Broadcaster/Listener, indented, and in a
//		  simple list fashion.
//		- The name of the Broadcaster/Listener
//		- If a Pane, the PaneIDT in parentheses.
//
//	LPaneTree is also an LPeriodical repeater with a user-settable
//	threshold. The tree will redraw every threshold ticks. On the
//	redraw, the mouse location is tracked and the DeepSubPaneContaining
//	the mouse is hilited.
//
//	To determine what window to target to know whose visual hierarchy
//	to display, LPaneTree walks the window list looking for the
//	front most regular and/or modal window and displays that hierarchy.
//	Windows in the floating layer are not supported as it's difficult
//	to target those (as there could be mulitple windows in the floating
//	layer, and how to pick which to display?). For now, targetting
//	floaters is not supported, but I'm always open to suggestions on
//	how this could be addressed.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPaneTree.h>
#include <UWindows.h>
#include <UDrawingUtils.h>
#include <TArray.h>
#include <TArrayIterator.h>
#include <LTextEditView.h>
#include <UEnvironment.h>
#include <LStream.h>
#include <PP_KeyCodes.h>
#include <PP_DebugMacros.h>
#include <PP_DebugConstants.h>
#include <UTBAccessors.h>

#ifdef __GNUC__
    #include <typeinfo>
#else
    #if __option(RTTI)
        #include <typeinfo>
    #else
        #error "RTTI option disabled -- Must be enabled for LPaneTree to function"
    #endif
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LPaneTree											[public]
// ---------------------------------------------------------------------------
//	Default constructor

LPaneTree::LPaneTree()
{
	mThreshold		= (kDefault_Threshold * 5);	// We don't need to update that often
	mCurMousedColor	= Color_DarkGreen;
	mCurMousedPane	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LPaneTree											[public]
// ---------------------------------------------------------------------------
//	Copy constructor

LPaneTree::LPaneTree(
	const LPaneTree&	inOriginal)

	: LTree(inOriginal)
{
	mCurMousedColor	= inOriginal.mCurMousedColor;;
	mCurMousedPane	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LPaneTree											[public]
// ---------------------------------------------------------------------------
// Parameterized constructor

LPaneTree::LPaneTree(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	UInt32				inThreshold,
	ResIDT				inTxtrID,
	const RGBColor		inMousedPaneColor)

	: LTree(inPaneInfo, inViewInfo, inThreshold, inTxtrID)
{
	mCurMousedColor	= inMousedPaneColor;
	mCurMousedPane	= nil;
}


// ---------------------------------------------------------------------------
//	¥ LPaneTree											[public]
// ---------------------------------------------------------------------------
//	LStream constructor

LPaneTree::LPaneTree(
	LStream*	inStream)

	: LTree(inStream)
{
	*inStream >> mCurMousedColor.red;
	*inStream >> mCurMousedColor.green;
	*inStream >> mCurMousedColor.blue;
	mCurMousedPane = nil;
}


// ---------------------------------------------------------------------------
//	¥Ê~LPaneTree										[public, virtual]
// ---------------------------------------------------------------------------
//	Destructor

LPaneTree::~LPaneTree()
{
}


// ---------------------------------------------------------------------------
//	¥ GetTargetWindow									[public, static]
// ---------------------------------------------------------------------------
//	Determine the Window of whose visual hierarchy will be displayed.
//
//	1.	Walk the window list to find a window.
//	2.	Ensure the window is a PowerPlant window.
//	3.	Ensure the window is in the "regular" or "modal" layers
//		("float" layer is not supported).
//
//	If any of these tests fail, return nil.
//
//	If a floater is found, keep cycling through the window list
//	until the first non-floater is found and use that window.

LWindow*
LPaneTree::GetTargetWindow(
	SInt16	inIndex)
{
	LWindow*	theWindow = nil;

		// Try to find a window, any window, and get the frontmost.
	WindowPtr windPtr = UWindows::FindNthWindow(inIndex);

	if (windPtr != nil) {

			// Have a window; see if it's a PowerPlant window
		theWindow = LWindow::FetchWindowObject(windPtr);

		if (theWindow != nil) {

				// It's a PP window; check the layer
			if ( !(theWindow->HasAttribute(windAttr_Regular)
				|| theWindow->HasAttribute(windAttr_Modal)) ) {

					// It's not regular or modal (a floater)
				theWindow = nil;

					// Keep searching
				theWindow = LPaneTree::GetTargetWindow(static_cast<SInt16>(inIndex + 1));
			}
		}
	}

	return theWindow;
}


// ---------------------------------------------------------------------------
//	¥ BuildTree										[protected, virtual]
// ---------------------------------------------------------------------------
//	Build the text (and style) for the Tree. Stored internally in a Handle.

void
LPaneTree::BuildTree(
	bool	inGenerateExtraInfo)
{
		// Get the Handles all set up to hold the text
	if (mTreeTextH != nil) {
		::DisposeHandle(mTreeTextH);
		mTreeTextH = nil;
	}

	if (mTreeTextStyleH != nil) {
		::DisposeHandle( reinterpret_cast<Handle>(mTreeTextStyleH) );
		mTreeTextStyleH = nil;
	}

	mTreeTextH = ::NewHandle(0);
	ThrowIfMemFail_(mTreeTextH);
	ValidateHandle_(mTreeTextH);

	mTreeTextStyleH = reinterpret_cast<StScrpHandle>(::NewHandleClear(sizeof(StScrpRec)));
	ThrowIfMemFail_(mTreeTextStyleH);
	ValidateHandle_((Handle)mTreeTextStyleH);
	(*mTreeTextStyleH)->scrpNStyles = 0;

	SInt32	theLevel	= 0;
	mLastTop			= 0;

	LWindow*	targetWindow = GetTargetWindow(1);
	if (targetWindow != nil) {

			// Find where the mouse is
		Verify_(targetWindow->FocusDraw());

		Point	portMouse;
		::GetMouse(&portMouse);
		mCurMousedPane = targetWindow->FindDeepSubPaneContaining(portMouse.h, portMouse.v);
		if (mCurMousedPane == nil) {
				// Mouse isn't within any subpane but may be within
				// the window itself.
			if(targetWindow->PointIsInFrame(portMouse.h, portMouse.v)) {
				mCurMousedPane = targetWindow;
			}
		}

		WalkPaneList(*targetWindow, theLevel, inGenerateExtraInfo);
	}
}


// ---------------------------------------------------------------------------
//	¥ WalkPaneList									[protected, virtual]
// ---------------------------------------------------------------------------
//	Peforms the iteration of the (sub)panes to draw the members

void
LPaneTree::WalkPaneList(
	const LPane&	inPane,
	SInt32			inLevel,
	bool			inGenerateExtraInfo)
{
	Assert_(mTreeTextH != nil);
	Assert_(mTreeTextStyleH != nil);

		// Get this pane
	LStr255 theText;
	MakePaneText(inPane, theText, inLevel);

		// Append a return character to the end of the text.
		// If the maximum number of characters exist, twiddle the
		// last character to be a return. If this isn't done, a
		// very long descriptor (e.g. from an LTextEditView) can
		// foul up the cosmetics and layout of the Tree window.
	if (theText.Length() < (theText.GetMaxLength() - 1)) {
		theText += char_Return;
	} else {
		theText[theText.Length()] = char_Return;
	}

		// Append the text to the internal text Handle
	SInt32 runOffset = ::GetHandleSize(mTreeTextH);
	OSErr err = ::PtrAndHand(theText.TextPtr(), mTreeTextH, theText.LongLength());
	ThrowIfOSErr_(err);

		// Construct the style run
	Verify_(mTextPane->FocusDraw());

	(void)UTextTraits::SetPortTextTraits(&mBaseTraits);

		// ScrpStTable holds 1601 elements and one doesn't want to
		// overflow this boundry. Create unique style runs through
		// run 1600. For the last run stick to the base style and
		// apply this to the rest of the text. Running out of style
		// runs is not a fatal condition, just a limitation of TextEdit.
		//
		// BTW, TextEdit has other limitations (TERec::teLength is a
		// short so you're limited to 32K of text; TERec::lineStarts is
		// an array with 16001 blocks so you can't have more than than many
		// line starts; TERec::viewRect/destRect are Rect's and you can
		// easily blow those short values). We don't enforce those other
		// limitations here -- BE AWARE --. Given the scope of this class
		// it'd doubtful you should hit any of these TextEdit limitation, but
		// if you do you should consider subclassing the Tree classes and
		// reimplementing them using a text engine/class that can support
		// your needs (e.g. WASTE/CWASTEEdit).

	SInt16 runCount = (*mTreeTextStyleH)->scrpNStyles;
	Assert_(runCount >= 0);

	if (runCount <= 1601) {

			// Within the boundries but only apply changes to
			// the base style for all but the last run
		if (runCount < 1601) {

				// Increase the run count
			(*mTreeTextStyleH)->scrpNStyles += 1;

				// Apply color for the moused pane
			if (&inPane == mCurMousedPane) {
				if (UEnvironment::HasFeature(env_SupportsColor)) {
					::RGBForeColor(&mCurMousedColor);
					::RGBBackColor(&Color_White);
				}
			}
		} else { // runCount == 1601
			SignalStringLiteral_("StScrpTable is now full. Subsequent text will use base style information");
		}

			// Add the style run to our list
		FontInfo theFontInfo;
		::GetFontInfo(&theFontInfo);

		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpStartChar	= runOffset;
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpHeight		= (SInt16) (theFontInfo.descent + theFontInfo.ascent);
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpAscent		= theFontInfo.ascent;
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpFont		= ::GetPortTextFont(GetMacPort());
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpFace		= ::GetPortTextFace(GetMacPort());
		((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpSize		= ::GetPortTextSize(GetMacPort());
		::GetForeColor(&((*mTreeTextStyleH)->scrpStyleTab[runCount]).scrpColor);
	} else {

			// You should never reach this point
		SignalStringLiteral_("runCount greater than 1601 -- this should never happen");
	}

		// ¥ Add extra information
	if (inGenerateExtraInfo) {

			// Walk the Pane's Attachment list and add them to the text Handle
		const TArray<LAttachment*> *attachList = inPane.GetAttachmentsList();
		if ((attachList != nil) && (attachList->GetCount() > 0)) {

			LStr255	attachInfo;
			TArrayIterator<LAttachment*> iterate(*attachList);
			LAttachment *theAttachment;
			while (iterate.Next(theAttachment)) {

					// Indent
				attachInfo = Str_Empty;
				for (SInt32 i = 1; i <= inLevel + 1; i++) {
					attachInfo += StringLiteral_("  ");
				}

					// Append Attachment information.
				attachInfo += typeid(*theAttachment).name();
				attachInfo += StringLiteral_(" - Message: ");
				attachInfo += theAttachment->GetMessage();
				attachInfo += StringLiteral_(" - Execute Host: ");
				attachInfo += (theAttachment->GetExecuteHost())
								? StringLiteral_("true\r")
								: StringLiteral_("false\r");

				err = ::PtrAndHand(attachInfo.TextPtr(), mTreeTextH, attachInfo.LongLength());
				ThrowIfOSErr_(err);

			}
		}

			// Add Broadcaster information
		const LBroadcaster*	paneAsBroadcaster = dynamic_cast<const LBroadcaster*>(&inPane);
		if (paneAsBroadcaster != nil) {

				// Convert the Broadcaster to a PeekBroadcaster so the
				// Listener list can be obtained (yes, this is a hack).
			const LPeekBroadcaster*	theBroadcaster = reinterpret_cast<const LPeekBroadcaster*>(paneAsBroadcaster);
			UInt32 numListeners = theBroadcaster->GetListeners().GetCount();
			if (numListeners > 0) {

				LStr255	listenerInfo;

				for (SInt32 i = 1; i <= inLevel + 1; i++) {
					listenerInfo += StringLiteral_("  ");	// Indent
				}

				listenerInfo += StringLiteral_("Broadcasts to: ");

				TArrayIterator<LListener*>	listenerIterator(theBroadcaster->GetListeners());
				LListener* theListener;
				bool first = true;
				while (listenerIterator.Next(theListener)) {

						// Cosmetics for the list
					if (!first) {
						listenerInfo += StringLiteral_(", ");
					} else {
						first = false;
					}

						// Listener's name
					listenerInfo += typeid(*theListener).name();

						// And if the Listener is a Pane, add its PaneIDT
					LPane* listenerAsPane = dynamic_cast<LPane*>(theListener);
					if (listenerAsPane != nil) {
						PaneIDT theID = listenerAsPane->GetPaneID();
						LStr255 theIDString(StringLiteral_(" ("));
						if (theID >= 0x20202020) {
							theIDString += LStr255(static_cast<FourCharCode>(theID));
						} else {
							theIDString += theID;
						}
						theIDString += ')';

						listenerInfo += theIDString;
					}
				}

				listenerInfo += char_Return;

				err = ::PtrAndHand(listenerInfo.TextPtr(), mTreeTextH, listenerInfo.LongLength());
				ThrowIfOSErr_(err);
			}
		}

			// Add Listener information
		const LListener* paneAsListener = dynamic_cast<const LListener*>(&inPane);
		if (paneAsListener != nil) {

				// Convert the Listener to a PeekListener so the
				// Broadcaster list can be obtained (yes, this is a hack).
			const LPeekListener* theListener = reinterpret_cast<const LPeekListener*>(paneAsListener);
			UInt32 numBroadcasters = theListener->GetBroadcasters().GetCount();
			if (numBroadcasters > 0) {

				LStr255 broadcasterInfo;

				for (SInt32 i = 1; i <= inLevel + 1; i++) {
					broadcasterInfo += StringLiteral_("  ");	// Indent
				}

				broadcasterInfo += StringLiteral_("Listens to: ");

				TArrayIterator<LBroadcaster*>	broadcasterIterator(theListener->GetBroadcasters());
				LBroadcaster* theBroadcaster;
				bool first = true;
				while (broadcasterIterator.Next(theBroadcaster)) {

						// Cosmetics for the list
					if (!first) {
						broadcasterInfo += StringLiteral_(", ");
					} else {
						first = false;
					}

						// Add the Broadcaster's name
					broadcasterInfo += typeid(*theBroadcaster).name();

						// If the Broadcaster is also a Pane, add its PaneIDT
					LPane* broadcasterAsPane = dynamic_cast<LPane*>(theBroadcaster);
					if (broadcasterAsPane != nil) {
						PaneIDT theID = broadcasterAsPane->GetPaneID();
						LStr255 theIDString(StringLiteral_(" ("));
						if (theID >= 0x20202020) {
							theIDString += LStr255(static_cast<FourCharCode>(theID));
						} else {
							theIDString += theID;
						}
						theIDString += ')';

						broadcasterInfo += theIDString;
					}
				}

				broadcasterInfo += char_Return;

				err = ::PtrAndHand(broadcasterInfo.TextPtr(), mTreeTextH, broadcasterInfo.LongLength());
				ThrowIfOSErr_(err);
			}
		}
	}


		// Just in case the text is going to be wider than the window,
		// expand the image so all fits on one line.
	SInt16 strWidth = ::StringWidth(theText);
	SDimension32 imgSize;
	GetImageSize(imgSize);
	if (imgSize.width <= strWidth) {
		mTextPane->ResizeImageBy(strWidth - imgSize.width + 10, 0, Refresh_Yes);
		LTextEditView *theView = DebugCast_(mTextPane, LView, LTextEditView);
		theView->AlignTextEditRects();
	}

		// Get it's subpanes, if any
	const LView* theView = dynamic_cast<const LView*>(&inPane);
	if (theView != nil) {
		TArrayIterator<LPane*>	iterator(
								(const_cast<TArray<LPane*>&>
								((const_cast<LView*>
								(theView))->GetSubPanes())));

		LPane*	theSub;
		while (iterator.Next(theSub) && (theSub != nil)) {
			WalkPaneList(*theSub, inLevel + 1, inGenerateExtraInfo);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ MakePaneText								[protected, virtual]
// ---------------------------------------------------------------------------
//	Makes the string containing the text to be used in each "cell".

void
LPaneTree::MakePaneText(
	const LPane&	inPane,
	LStr255&		outText,
	SInt32			inLevel)
{
		// Indent
	for (SInt32 i = 1; i <= inLevel; i++) {
		outText += StringLiteral_("  ");
	}

		// RTTI to get the object name
	outText += typeid(inPane).name();
	outText += StringLiteral_(" - ");

		// Add the PaneIDT
	outText += StringLiteral_("ID: ");
	PaneIDT theID = inPane.GetPaneID();

		// PaneIDT's can be numbers or text (FourCharCode). We'll try to
		// display that here. We test versus what should be the minimal
		// possible FourCharCode (4 spaces). If at least that, assume it
		// is text (doubtful someone would use a integral PaneIDT of that
		// size) and display it as such. (FWIW, this really isn't the
		// best way to check if a PaneIDT could be displayed as a FourCharCode,
		// but every other option I've tried has more problems :-(

	if (theID >= 0x20202020) {
			// LString doesn't have an Append(FourCharCode) method,
			// so for ease we'll just create an LStr255(FourCharCode)
			// object and append that since we have Append(LString&)
		outText += char_Apostrophe;
		outText += LStr255(static_cast<FourCharCode>(theID));
		outText += char_Apostrophe;
	} else {
			// Probably just an integral type.
		outText += theID;
	}

	outText += StringLiteral_(" - ");

		// Active, Enable, Visible states. To reduce some crowding, we'll abbreviate.
	outText += StringLiteral_("State: ");

	if (inPane.IsActive()) {
		outText += 'A';
	}
	if (inPane.IsEnabled()) {
		outText += 'E';
	}
	if (inPane.IsVisible()) {
		outText += 'V';
	}
	
	outText += StringLiteral_(" - tlhw: ");	// top (port coords), left (port coords), height width
	SPoint32 frameLoc;
	inPane.GetFrameLocation(frameLoc);
	outText += frameLoc.v;
	outText += StringLiteral_("/");
	outText += frameLoc.h;
	outText += StringLiteral_(", ");
	SDimension16 frameSize;
	inPane.GetFrameSize(frameSize);
	outText += frameSize.height;
	outText += StringLiteral_("/");
	outText += frameSize.width;
	
		// Descriptor, if any
	Str255 desc;
	desc[0] = 0;
	inPane.GetDescriptor(desc);
	if (desc[0]) {
		outText += StringLiteral_(" - Desc: ");
		outText += desc;
	}

}

PP_End_Namespace_PowerPlant
