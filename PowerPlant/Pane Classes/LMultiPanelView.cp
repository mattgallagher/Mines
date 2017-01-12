// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMultiPanelView.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	A container View for multiple Panels, only one of which is visible
//	at any given time.
//
//	Each Panel is a separate View, which is usually created from a PPob
//	resource. The MultiPanelView (MPV) maintains an ordered list of Panels,
//	which behave similar to a radio button group. Calling SwtichToPanel()
//	hides the current Panel and shows the specified Panel (referred to
//	by a 1-based index number).
//
//	You can link a MPV (which is a Listener) with a Control so that changing
//	the Control's value switches Panels. You do so by making the MPV a
//	Listener to the Control and setting the MPV's switch message to be
//	the same as the Control's value message. Typcally, a TabsControl and
//	PopupGroupBox will use a MPV.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMultiPanelView.h>
#include <LBroadcaster.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UReanimator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LMultiPanelView						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LMultiPanelView::LMultiPanelView()
{
	mCurrentIndex  = 0;
	mSwitchMessage = 0;
}


// ---------------------------------------------------------------------------
//	¥ LMultiPanelView						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LMultiPanelView::LMultiPanelView(
	LStream*	inStream)

	: LView(inStream)
{
	UInt16	numPanels;					// Install Panels
	*inStream >> numPanels;

	for (SInt16 i = 0; i < numPanels; i++) {
		ResIDT	PPobID;
		*inStream >> PPobID;
		AddPanel(PPobID, nil, LArray::index_Last);
	}

	*inStream >> mCurrentIndex;			// Initial Panel
	if (mCurrentIndex > numPanels) {
		mCurrentIndex = numPanels;
	}

	*inStream >> mSwitchMessage;		// Message triggering a Panel switch

	Boolean		listenToSuper;			// With "listen to super" ON, we
	*inStream >> listenToSuper;			//   make the MultiPanelView a
										//   Listener to its SuperView
	if (listenToSuper) {
		LBroadcaster	*super = dynamic_cast<LBroadcaster*>(mSuperView);
		if (super != nil) {				// Use RTTI to determine if SuperView
			super->AddListener(this);	//   is a Broadcaster
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LMultiPanelView						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LMultiPanelView::LMultiPanelView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	LCommander*			inSuperCommander)

	: LView(inPaneInfo, inViewInfo),
	  LCommander(inSuperCommander)
{
	mCurrentIndex  = 0;
	mSwitchMessage = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LMultiPanelView						Destructor				  [public]
// ---------------------------------------------------------------------------

LMultiPanelView::~LMultiPanelView()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LMultiPanelView::FinishCreateSelf()
{
	if (mCurrentIndex > 0) {		// Upon creation, show initial panel
		ShowPanel(mCurrentIndex);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AddPanel														  [public]
// ---------------------------------------------------------------------------
//	Add Panel to the MPV at the specified index
//
//	Only one of inPPobID and inPanel has to be specified, although you
//	can specify both. If it exists, inPanel must already be an invisible
//	subpane of the MPV.
//
//	The current Panel remains the same. The current index is incremented
//	by one if you add the Panel at or before the current index.

void
LMultiPanelView::AddPanel(
	ResIDT		inPPobID,			// PPob of a View to add as a panel
	LView		*inPanel,			// View object to add as a panel
	UInt16		inAtIndex)			// Position at which to add the panel
{
	SPanelAndID	pid;
	pid.panel  = inPanel;
	pid.PPobID = inPPobID;
									// Store entry in Panel list
	inAtIndex = (UInt16) mPanels.InsertItemsAt(1, inAtIndex, pid);

	if (inAtIndex <= mCurrentIndex) {
		mCurrentIndex += 1;			// Panel added at or before current one,
	}								//   so increment current index
}


// ---------------------------------------------------------------------------
//	¥ RemovePanel													  [public]
// ---------------------------------------------------------------------------
//	Remove Panel from the MPV and pass back a pointer to the Panel's View
//
//	Panel is still a subpane of the MPV, but it's invisible and can't
//	be accessed via the functions that manipulate Panels.

LView*
LMultiPanelView::RemovePanel(
	UInt16		inIndex)			// 1-based index of panel to remove
{
	LView	*thePanel = nil;

	if ( (inIndex > 0) && (inIndex <= GetPanelCount()) ) {
		thePanel = GetPanel(inIndex);

		mPanels.RemoveItemsAt(1, inIndex);

		if (inIndex < mCurrentIndex) {	// Panel was before current one
			mCurrentIndex -= 1;			//   so decrement current index

		} else if (inIndex == mCurrentIndex) {
										// Current Panel removed
			SwitchToPanel(0);				// Panel 0 is no Panel
		}

	}

	return thePanel;
}


// ---------------------------------------------------------------------------
//	¥ SwitchToPanel													  [public]
// ---------------------------------------------------------------------------
//	Hide the current panel and make visible the specifed panel
//
//	You can pass 0 for inIndex to hide the current panel without
//	showing another panel.
//
//	Pass true for inDeleteCurrent to delete the current panel rather
//	than just hiding it.

void
LMultiPanelView::SwitchToPanel(
	UInt16		inIndex,
	bool		inDeleteCurrent)
{
	if (inIndex > mPanels.GetCount()) {
		SignalStringLiteral_("Bad Index");
		return;
	}

	if (inIndex != mCurrentIndex) {

		LView	*panel = GetCurrentPanel();
		if (panel != nil) {
		
				// $$$ Should this code be in LView::Hide() ?
				// Set visible region so that no drawing occurs within
				// the panel being hidden. This makes for a smooth
				// transition to the new panel.
		
			StVisibleRgn	visRgn( GetMacPort() );
			
			Rect	panelFrame;
			panel->CalcPortFrameRect(panelFrame);
			PortToLocalPoint(topLeft(panelFrame));
			PortToLocalPoint(botRight(panelFrame));
			
			StRegion	panelRgn(panelFrame);
			
			visRgn.DiffWithCurrent(panelRgn);
		
			panel->Hide();					// Hide current Panel
			if (inDeleteCurrent) {
				delete panel;
				mPanels[mCurrentIndex].panel = nil;
			}
		}

		mCurrentIndex = inIndex;
		if (inIndex > 0) {					// Show new Panel
			ShowPanel(inIndex);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CreatePanel													  [public]
// ---------------------------------------------------------------------------
//	Create the View for a Panel (if it hasn't already been created) and
//	return a pointer to the View
//
//	Returns nil if inIndex is not valid or if the View isn't successfully
//	created (usually an invalid PPob or out of memory).
//
//	Note the difference between GetPanel() and CreatePanel(). GetPanel()
//	will return nil if the View hasn't been created yet. CreatePanel()
//	will create the View if necessary.

LView*
LMultiPanelView::CreatePanel(
	UInt16		inIndex)
{
	SPanelAndID	pid;
	pid.panel = nil;					// In case something goes wrong

	if (mPanels.FetchItemAt(inIndex, pid)) {
										// Index is valid
		if (pid.panel == nil) {			// View hasn't been created
			try {
				pid.panel = UReanimator::CreateView(pid.PPobID, this, this);
			}

			catch (...) { }				// Failure to create View isn't fatal

			mPanels[inIndex].panel = pid.panel;
		}
	}

	return pid.panel;
}


// ---------------------------------------------------------------------------
//	¥ CreateAllPanels												  [public]
// ---------------------------------------------------------------------------
//	Create the View for each Panel
//
//	If created from a PPob, the MPV creates a Panel's View when it first
//	shows the Panel. This saves time and effort if a Panel is never shown.
//
//	Call this function if you want to create the View for each Panel
//	immediately.

void
LMultiPanelView::CreateAllPanels()
{
	UInt16	numPanels = GetPanelCount();

	for (UInt16 i = 1; i <= numPanels; i++) {
		CreatePanel(i);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPanel														  [public]
// ---------------------------------------------------------------------------
//	Set the Panel at the specified index
//
//	Only one of inPPobID and inPanel has to be specified, although you
//	can specify both. If it exists, inPanel must already be an invisible
//	subpane of the MPV.
//
//	inDeleteOld specifies whether to delete the Panel that presently
//	occupies the input index.
//
//	You can set the Panel at the current index. If you do, we hide (or delete)
//	the current Panel and show the input Panel.

void
LMultiPanelView::SetPanel(
	ResIDT		inPPobID,
	LView		*inPanel,
	UInt16		inIndex,
	bool		inDeleteOld)
{
	if ( (inIndex > 0) && (inIndex <= GetPanelCount()) ) {

										// Save old Panel
		LView	*oldPanel = mPanels[inIndex].panel;

		SPanelAndID	pid;				// Store new Panel in list
		pid.panel = inPanel;
		pid.PPobID = inPPobID;
		mPanels[inIndex] = pid;

		if (inDeleteOld) {				// Delete old Panel if desired
			delete oldPanel;
			oldPanel = nil;
		}

		if (inIndex == mCurrentIndex) {
										// We are setting the current Panel
			if (oldPanel != nil) {
				oldPanel->Hide();		// Must hide old Panel
			}

			ShowPanel(inIndex);			// Show new Panel
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPanel														  [public]
// ---------------------------------------------------------------------------
//	Return the View for the specified panel index
//
//	Returns nil if the index is invalid
//
//	Note the difference between GetPanel() and CreatePanel(). GetPanel()
//	will return nil if the View hasn't been created yet. CreatePanel()
//	will create the View if necessary.

LView*
LMultiPanelView::GetPanel(
	UInt16		inIndex) const
{
	SPanelAndID	pid;
	pid.panel = nil;
	mPanels.FetchItemAt(inIndex, pid);
	return pid.panel;
}


// ---------------------------------------------------------------------------
//	¥ GetPanelResID													  [public]
// ---------------------------------------------------------------------------
//	Return the PPob Resource ID for the specified panel index
//
//	Returns 0 if the index is invalid or if the View wasn't created
//	from a PPob.

ResIDT
LMultiPanelView::GetPanelResID(
	UInt16		inIndex) const
{
	SPanelAndID	pid;
	pid.PPobID = 0;
	mPanels.FetchItemAt(inIndex, pid);
	return pid.PPobID;
}


// ---------------------------------------------------------------------------
//	¥ ShowPanel													   [protected]
// ---------------------------------------------------------------------------
//	Make the specified panel visible

void
LMultiPanelView::ShowPanel(
	UInt16		inIndex)
{
	LView	*thePanel = CreatePanel(inIndex);

	if (thePanel != nil) {
		thePanel->Show();
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------

void
LMultiPanelView::DrawSelf()
{
		// Erase contents to the background color/pattern.
		// To avoid pattern alignment problems, we erase in
		// port coordinates.

	StClipOriginState	saveClipOrigin(Point_00);

	Rect	frame;
	CalcPortFrameRect(frame);
	
	ApplyForeAndBackColors();
	::EraseRect(&frame);
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------
//	If the message is our "switch panel" message, switch to the panel whose
//	index number is in the ioParam

void
LMultiPanelView::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	if (inMessage == mSwitchMessage) {
		SwitchToPanel((UInt16) *(SInt32*)(ioParam));
	}
}


PP_End_Namespace_PowerPlant
