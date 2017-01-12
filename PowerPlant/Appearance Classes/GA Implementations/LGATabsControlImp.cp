// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATabsControlImp.cp		PowerPlant 2.2.2	й1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGATabsControlImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LArrayIterator.h>
#include <LControlView.h>
#include <LStream.h>
#include <LString.h>
#include <LVariableArray.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <URegistrar.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


enum {

	tabPanel_TabButtonSlope 		= 11,
	tabPanel_SmallTabButtonHeight	= 19,
	tabPanel_LargeTabButtonHeight	= 24,
	tabPanel_BottomOffset 			= 3,
	tabPanel_LeftEdgeOffset 		= 7,
	tabPanel_IconOffset				= 4

};


#pragma options align=mac68k

	// Format of Toolbox 'tab#' resource

struct STabInfo {
	SInt16		iconSuiteID;
	Str255		tabName;			// This is descriptive, not accurate. String is not padded to 255 characters.
	SInt32		reserved1;
	SInt16		reserved2;
};

struct STabListRec {
	SInt16		version;
	SInt16		numTabs;
	STabInfo	tabs[1];
};

#pragma options align=reset


#pragma mark === INITIALIZATION & DESTRUCTION

// ---------------------------------------------------------------------------
//	е LGATabsControlImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGATabsControlImp::LGATabsControlImp(
	LControlPane*	inControlPane)

	: LGAControlImp (inControlPane)
{
}


// ---------------------------------------------------------------------------
//	е LGATabsControlImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGATabsControlImp::LGATabsControlImp(
	LStream* 	inStream)

	: LGAControlImp (inStream)
{
	mWantsLargeTab		= false;
	mSelectedButton		= nil;
	mTextTraitsID		= 0;
	mTabButtons			= nil;
	mTabListResID		= 0;
	mSelectionIndex		= 0;
	mInitialTabTitles	= nil;
	mNextIndex			= 0;
}


// ---------------------------------------------------------------------------
//	е ~LGATabsControlImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGATabsControlImp::~LGATabsControlImp()
{
	mTabButtons = nil;
}


// ---------------------------------------------------------------------------
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init (inControlPane, controlKind, title, textTraitID);

}


// ---------------------------------------------------------------------------
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
												// Setup our fields
	mControlPane = inControlPane;
	mTextTraitsID = inTextTraitsID;
												// Setup the icon flags
	mWantsLargeTab = (inControlKind == kControlTabLargeProc);

												// Get the tab list ID
	mTabListResID = (SInt16) mControlPane->GetValue ();

												// Build the view that holds the tab buttons
	BuildTabButtonView ();
												// Now call our function that handles the actual
												// loading of the tab titles and buttons from the
												// tab list resource
	LoadTabTitlesFromTabList ( mTabListResID, 1 );
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	е GetCurrentTabIndex											  [public]
// ---------------------------------------------------------------------------

ArrayIndexT
LGATabsControlImp::GetCurrentTabIndex()
{
												// If there is a currently selected tab then we
												// proceed to use it to figure out the index
	if ( GetSelectedTabButton ()) {
		LGATabsButton* currButton = GetSelectedTabButton ();
		LArray&	tabButtons = GetTabButtons ();

												// Search through the list for the index
		return tabButtons.FetchIndexOf ( &currButton );

	}
												// There was no selected button so return nothing,
												// this should never happen
	return 0;
}


// ---------------------------------------------------------------------------
//	е GetTabIndex													  [public]
// ---------------------------------------------------------------------------

ArrayIndexT
LGATabsControlImp::GetTabIndex(
	ConstStringPtr	inTabTitle)
{
												// Get the index for the page title passed in
	LArray&	tabButtons = GetTabButtons ();
	LArrayIterator iterator ( tabButtons, LArrayIterator::from_Start );
	LGATabsButton*	aTabButton = nil;
	while ( iterator.Next ( &aTabButton )) {
		if ( aTabButton ) {
			Str255	theTitle;
			aTabButton->GetDescriptor ( theTitle );
			if ( theTitle == inTabTitle ) {
												// Return the index of the current item
				return iterator.GetCurrentIndex ();
			}
		}
	}
												// Couldn't find the title so we simply return zero
	return 0;
}


// ---------------------------------------------------------------------------
//	е GetTabButtonByIndex											  [public]
// ---------------------------------------------------------------------------

LGATabsButton*
LGATabsControlImp::GetTabButtonByIndex(
	ArrayIndexT inIndex) const
{
	LArray&	tabButtons = GetTabButtons ();

												// Search through the list for the tab button
	LGATabsButton*	tabButton = nil;
	if ( tabButtons.FetchItemAt ( inIndex, &tabButton )) {
		return tabButton;
	}
												// Couldn't find the title so we simply return zero
	return nil;
}


// ---------------------------------------------------------------------------
//	е CalcTabButtonWidth											  [public]
// ---------------------------------------------------------------------------

SInt16
LGATabsControlImp::CalcTabButtonWidth(
	ConstStringPtr	inTabTitle,
	Boolean			inHasIcon)
{
	StTextState		theTextState;
												// Start by making sure we setup the correct
												// textstyle for the view so that the title widths
												// can be correctly calculated
	ResIDT	textTID = GetTextTraitsID ();
	UTextTraits::SetPortTextTraits ( textTID );

												// Return the width of the title plus the button
												// slop and any icon if present
	SInt16 titleWidth = (SInt16) ( ::StringWidth ( inTabTitle ) + (tabPanel_TabButtonSlope * 2) );
	SInt16 iconWidth = (SInt16) (inHasIcon ? 16 + tabPanel_IconOffset : 0);

	return (SInt16) (iconWidth + titleWidth);

}


// ---------------------------------------------------------------------------
//	е CalcLocalTabPanelRect															[public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::CalcLocalTabPanelRect(
	Rect&	outRect) const
{

	StColorPenState	thePenState;

	CalcLocalFrameRect ( outRect );
												// We need to adjust the top of the panel down so
												// that we can accomodate the tab buttons, the
												// amount is based on the button size
	outRect.top += (WantsLargeTab () ? tabPanel_LargeTabButtonHeight - 3 :
												tabPanel_SmallTabButtonHeight - 3);

}


// ---------------------------------------------------------------------------
//	е CalcTabButtonClipping											  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::CalcTabButtonClipping(
	Rect&	outRect)
{
												// Figure out a rectangle that will be used when
												// we are drawing the actual panel to clip out the
												// area occupied by the buttons what we are looking
												// for is to end up with a rect that can be used
												// to clip out the drawing of the top edge of the
												// panel where the buttons are located
	CalcLocalTabPanelRect ( outRect );
	outRect.left  += tabPanel_LeftEdgeOffset;
	outRect.bottom = (SInt16) (outRect.top + 3);
	outRect.right  = (SInt16) (outRect.left + (GetLastTabButtonLocation () - 6));

}


// ---------------------------------------------------------------------------
//	е FindHotSpot													  [public]
// ---------------------------------------------------------------------------
//	TabsControl itself has no hot spots. Clicking inside the body of the
//	TabsControl does nothing. The TabButtons handle clicks inside the tabs.

SInt16
LGATabsControlImp::FindHotSpot(
	Point		/* inPoint */) const
{
	return 0;
}


// ---------------------------------------------------------------------------
//	е SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::SetTextTraitsID(
	ResIDT	inTextTraitsID)
{
	mTextTraitsID = inTextTraitsID;
}


// ---------------------------------------------------------------------------
//	е SetSelectedTabButton											  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::SetSelectedTabButton(
	LGATabsButton*	inSelectedButton,
	Boolean			inSelectButton,
	Boolean			inSuppressBroadcast)
{
	if ( GetSelectedTabButton () != inSelectedButton ) {
													// If we are going to select the button then we
													// need make sure that we turn off the current
													// selection
		if ( mSelectedButton && inSelectButton ) {
			ToggleTabButtonState ( mSelectedButton, false, inSuppressBroadcast );
		}
													// Setup the reference to the button
		mSelectedButton = inSelectedButton;

													// If as part of the process we are going to
													// get the button to show up selected then do it now
		if ( inSelectButton ) {
			ToggleTabButtonState ( inSelectedButton, true, inSuppressBroadcast );
		}
	}
}


// ---------------------------------------------------------------------------
//	е SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::SetValue(
	SInt32	inValue)
{
												// We need to make sure based on the value that we
												// get the appropriate tab button selected
	if ( GetCurrentTabIndex () != inValue ) {
		SetSelectedTabButtonByIndex ( inValue );
	}
}


// ---------------------------------------------------------------------------
//	е PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::PostSetValue()
{
												// Then we need to make sure that we poke the value
												// into the control pane so that when GetValue is
												// called the correct value will be in the control
												// pane's value field, unfortunately we have to do
												// this here otherwise SetValue messes with the
												// value because it is out of range so we have to
												// get the value from the header and use that to
												// get the value set directly
	mControlPane->PokeValue ( GetCurrentTabIndex () );

}


// ---------------------------------------------------------------------------
//	е SetSelectedTabButtonByIndex									  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::SetSelectedTabButtonByIndex(
	ArrayIndexT	inSelectionIndex,
	Boolean		inSuppressBroadcast)
{
												// Now get the titles loaded from the array we were
												// handed
	SInt16	tabCount = GetTabButtonCount ();
	if ( tabCount > 0 && inSelectionIndex <= tabCount ) {
													// Now find the button associated with this
													// index
		LArray&	buttonList = GetTabButtons ();
		LGATabsButton* tabButton;
		buttonList.FetchItemAt ( inSelectionIndex, &tabButton );
		if ( tabButton ) {
			SetSelectedTabButton ( tabButton, true, inSuppressBroadcast );
		}
	}
}


// ---------------------------------------------------------------------------
//	е ToggleTabButtonState											  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::ToggleTabButtonState(
	LGATabsButton*	inSelectedButton,
	Boolean			inSelected,
	Boolean			inSuppressBroadcast)
{
	if ( inSelectedButton->IsSelected () != inSelected ) {
													// Either turn the button on and get its value
													// toggled then redraw everything, we redraw by
													// calling the draw routine directly so that
													// things happen immeditiately, we only
													// broadcast the change if it is not being suppressed
		if ( inSuppressBroadcast ) {
			MessageT storeMessage = inSelectedButton->GetValueMessage ();
			inSelectedButton->SetValueMessage ( 0 );
			inSelectedButton->SetValue ( inSelected ? 1 : 0 );
			inSelectedButton->SetValueMessage ( storeMessage );

		} else {
			inSelectedButton->SetValue ( inSelected ? 1 : 0 );
		}
													// Now get the button redrawn
		inSelectedButton->Draw ( nil );
	}
}


#pragma mark -
#pragma mark === COLORS

// ---------------------------------------------------------------------------
//	е GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LGATabsControlImp::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if ( outForeColor != nil ) {
		*outForeColor = Color_Black;
	}

	if ( outBackColor != nil ) {
		*outBackColor = UGAColorRamp::GetColor(colorRamp_Gray1);
	}

	return true;
}

#pragma mark -
#pragma mark === TAB BUTTON MANAGEMENT

// ---------------------------------------------------------------------------
//	е AddTabButton													  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::AddTabButton(
	ConstStringPtr  inTabTitle,
	ResIDT			inIconSuiteID,
	Boolean  		inSelected)
{
								// Now we need to get a button built for the
								// new title this button is then added to the end
								// of the button view we start by getting an ID for
								// the button, if for some reason we run out of
								// indices we will do a compact
	PaneIDT	newButtonID;
	GetTabButtonID ( newButtonID );

												// Figure out the location at which the button will
												// be placed
	SPoint32 	buttonLoc;
	buttonLoc.h = GetLastTabButtonLocation ();
	buttonLoc.v = 0;

												// Figure out the size of the button
	Rect localFrame;
	mTabButtons->CalcLocalFrameRect ( localFrame );
	SDimension16	buttonSize;
	Boolean hasIcon = (inIconSuiteID != 0 && inIconSuiteID != resID_Undefined);
	buttonSize.width = CalcTabButtonWidth ( inTabTitle, hasIcon );
	if ( WantsLargeTab ()) {
		buttonSize.height = tabPanel_LargeTabButtonHeight;
	} else {
		buttonSize.height = tabPanel_SmallTabButtonHeight;
	}

												// Get the button created
	CreateTabButton(inTabTitle, inIconSuiteID, newButtonID,
					 buttonSize, buttonLoc, inSelected);
}


// ---------------------------------------------------------------------------
//	е InsertTabButtonAt												  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::InsertTabButtonAt(
	ConstStringPtr  inTabTitle,
	ResIDT			inIconSuiteID,
	ArrayIndexT 	inAtIndex)
{
												// Before we proceed we make sure that the index
												// we have been handed is not out of range, if it
												// is greater than the current button count then
												// we are going to simply call AddTabButton which
												// will get the new button appended to the end of
												// the button list
	SInt16	buttonCount = GetTabButtonCount ();
	Boolean compactIDs = false;
	ArrayIndexT buttonIndex = GetCurrentIndex () + 1;
	if ( inAtIndex > 0 && inAtIndex <= buttonCount ) {

												// We start out by first checking to make sure that
												// there is a valid index available for this button
												// otherwise we will need to get the indices
												// compacted
		if ( buttonIndex > 99 ) {
			compactIDs = true;
		}
												// Now we need to get a button built for the the
												// new title this button is then added to the end
												// of the button view
												// Now get an ID for the new button
		PaneIDT	newButtonID;
		GetTabButtonID ( newButtonID );

												// Figure out the button location
		LArray&	buttonList = GetTabButtons ();
		SPoint32 	buttonLoc;
		LGATabsButton*	aTabButton;
		buttonList.FetchItemAt ( inAtIndex, &aTabButton );
		aTabButton->GetFrameLocation ( buttonLoc );

												// Was the button selected
		Boolean	wasSelected = aTabButton->IsSelected ();

												// We need to make sure that we subtract the amount
												// of inset that exists at the left edge of the
												// panel as the coordinates of the button will get
												// adjusted when it gets added to the button view,
												// as it is a control and uses the coordinate
												// system of its superview
		buttonLoc.h -= tabPanel_LeftEdgeOffset;

												// Vertical location of the button is always 0
		buttonLoc.v = 0;

												// Figure out the size of the button
		Rect localFrame;
		mTabButtons->CalcLocalFrameRect ( localFrame );
		SDimension16	buttonSize;
		Boolean hasIcon = (inIconSuiteID != 0 || inIconSuiteID != resID_Undefined);
		buttonSize.width = CalcTabButtonWidth ( inTabTitle, hasIcon );
		buttonSize.height = UGraphicUtils::RectHeight ( localFrame );

												// Get the button created
		CreateTabButton ( 	inTabTitle,
									inIconSuiteID,
									newButtonID,
									buttonSize,
									buttonLoc,
									false );

												// Now we have to manipulate its location in the
												// list as it is important that it be in the
												// correct location
		ArrayIndexT	lastIndex = GetTabButtonCount ();

												// Move the item fromm the end of the list to the
												// insertion location
		buttonList.MoveItem ( lastIndex, inAtIndex );

												// Finally, we need to get the locations of all the
												// buttons after this one adjusted to accomodate
												// the new button
		buttonCount = (SInt16) buttonList.GetCount ();
		for ( SInt16 index = (SInt16) (inAtIndex + 1); index <= buttonCount; index++ ) {
			buttonList.FetchItemAt ( index, &aTabButton );
			if ( aTabButton ) {
				aTabButton->MoveBy ( buttonSize.width, 0, true );
			}
		}
												// If the tab that was at this location was
												// selected then we need to make sure we do the
												// same for the new button
		if ( wasSelected ) {
			SetSelectedTabButtonByIndex ( inAtIndex );
		}

	} else if ( inAtIndex > buttonCount ) {
												// We start out by first checking to make sure that
												// there is a valid index available for this button
												// otherwise we will need to get the indices
												// compacted
		if ( buttonIndex > 99 ) {
			compactIDs = true;
		}
												// Add the button to the end of the button list
		AddTabButton ( inTabTitle, inIconSuiteID, false );
	}

												// If we are going to handle synchronizing the tab
												// button IDs then we do it now
	if ( compactIDs ) {
		RebuildButtonIdentifiers ();
	}
												// Get the tab button view refreshed so the new
												// button shows up
	LView*	tabButtonView = GetTabButtonView ();
	if ( tabButtonView ) {
		tabButtonView->Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	е RemoveTabButtonAt												  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::RemoveTabButtonAt(
	ArrayIndexT	inAtIndex)
{
												// Before we proceed we make sure that the index
												// we have been handed is not out of range, if it
												// is we simply do nothing
	SInt16	tabCount = GetTabButtonCount ();
	if ( inAtIndex > 0 && inAtIndex <= tabCount ) {

		LArray&	tabButtons = GetTabButtons ();

												// Find then button that is being removed
		LGATabsButton*	tabButton;
		tabButtons.FetchItemAt ( inAtIndex, &tabButton );

												// Was the button selected
		Boolean	wasSelected = tabButton->IsSelected ();

												// Figure out the size of the button
		Rect localFrame;
		mTabButtons->CalcLocalFrameRect ( localFrame );
		SDimension16	buttonSize;
		tabButton->GetFrameSize ( buttonSize );

												// Remove the button from the list
		tabButton->Refresh ();
		tabButton->Hide ();
		tabButton->PutInside ( nil );
		delete tabButton;
												// We need to get the locations of all the buttons
												// after this one adjusted to fill in the space
												// occupied by the removed tab button
		tabCount = (SInt16) tabButtons.GetCount ();
		for ( SInt16 index = (SInt16) inAtIndex; index <= tabCount; index++ ) {
			tabButtons.FetchItemAt ( index, &tabButton );
			if ( tabButton ) {
				tabButton->MoveBy ( -buttonSize.width, 0, true );
			}
		}
												// if the button was selected we need to get
												// another one selected, for that we just use the
												// same index
		if ( wasSelected ) {
												// Clear the button reference
			mSelectedButton = nil;

												// If the index is greater than the number of
												// buttons we will simply select the last tab
												// button
			if ( tabCount < inAtIndex ) {
				inAtIndex = tabCount;
			}
												// Get the appropriate button selected
			SetSelectedTabButtonByIndex ( inAtIndex, false );
		}

												// Get the tab button view refreshed
		LView*	tabButtonView = GetTabButtonView ();
		if ( tabButtonView ) {
			tabButtonView->Refresh ();
		}
	}
}


// ---------------------------------------------------------------------------
//	е CreateTabButton												  [public]
// ---------------------------------------------------------------------------

LGATabsButton*
LGATabsControlImp::CreateTabButton(
	ConstStringPtr		inButtonTitle,
	ResIDT				inIconSuiteID,
	PaneIDT				inButtonID,
	SDimension16		inButtonSize,
	SPoint32			inButtonPosition,
	Boolean				inButtonPushed)
{
	SPaneInfo paneInfo;

	paneInfo.paneID		= inButtonID;
	paneInfo.width		= inButtonSize.width;
	paneInfo.height		= inButtonSize.height;
	paneInfo.visible	= true;
	paneInfo.enabled	= true;

	paneInfo.bindings.left		= false;
	paneInfo.bindings.top		= false;
	paneInfo.bindings.right		= false;
	paneInfo.bindings.bottom	= false;

	paneInfo.left		= inButtonPosition.h;
	paneInfo.top		= inButtonPosition.v;
	paneInfo.userCon	= nil;
	paneInfo.superView	= GetTabButtonView ();

	SControlInfo	controlInfo;

	controlInfo.valueMessage	= inButtonID;
	controlInfo.value			= inButtonPushed ? 1 : 0;
	controlInfo.minValue		= 0;
	controlInfo.maxValue		= 1;

												// Now create the actual button
	LGATabsButton* aTabButton = new LGATabsButton(	paneInfo,
													controlInfo,
													inButtonTitle,
													true,
													GetTextTraitsID(),
													inIconSuiteID,
													WantsLargeTab());

												// We need to make sure that we call FinishCreate
												// so that everything is happy
	aTabButton->FinishCreate();

												// Add the tab panel as a listener to the button
	aTabButton->AddListener(this);

												// If the button is set to be pushed then get it
												// setup that way
	if (inButtonPushed) {
		SetSelectedTabButton(aTabButton);
	}
												// Now return the newly built button
	return aTabButton;

}


// ---------------------------------------------------------------------------
//	е GetTabButtonCount												  [public]
// ---------------------------------------------------------------------------

SInt16
LGATabsControlImp::GetTabButtonCount()
{
												// Get the list of sub panes for the button view
	LArray& buttonList = GetTabButtons();

												// Return the button count which is the sub pane
												// count
	return (SInt16) buttonList.GetCount();
}


// ---------------------------------------------------------------------------
//	е GetLastTabButtonLocation										  [public]
// ---------------------------------------------------------------------------

SInt16
LGATabsControlImp::GetLastTabButtonLocation()
{
												// If there are buttons then find the last one and
												// return the current coordinate for its right edge
	LArray&	buttonList = GetTabButtons();
	if ( buttonList.GetCount () > 0 ) {
		LGATabsButton*	lastButton;
		buttonList.FetchItemAt ( (ArrayIndexT) buttonList.GetCount (), &lastButton );
		Rect lastButtonFrame;
		lastButton->CalcLocalFrameRect ( lastButtonFrame );

												// Return right edge coordinate
		return lastButtonFrame.right;
	}
												// There are currently no buttons so just return 6
	return 6;
}


// ---------------------------------------------------------------------------
//	е DeleteAllTabButtons											  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::DeleteAllTabButtons()
{
												// To delete all of the tab buttons we simply call
												// the DeleteAllSubPanes method in LView which
												// works nicely for us, we also then need to do a
												// refresh so that the change shows up
	if ( mTabButtons ) {
		mTabButtons->DeleteAllSubPanes ();
		Refresh ();
	}
												// Make sure we zero out the index count
	ClearIndexCounter ();

}


// ---------------------------------------------------------------------------
//	е LoadTabTitlesFromTabList										  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::LoadTabTitlesFromTabList(
	ResIDT		inTabListID,
	SInt16		inSelectionIndex)
{
										// Get tab# resource
	StResource	tabListH(kControlTabListResType, inTabListID, false, false);

	if ((tabListH.mResourceH != nil) && (::GetHandleSize(tabListH) >= 4)) {
		::HLock ( tabListH );

										// Format of tab# resource is:
										//		2-byte version
										//		2-byte tab count
										//		Array of STabInfo structs

		STabListRec*	tabList =
						reinterpret_cast<STabListRec*>(*tabListH.mResourceH);

		SInt16	tabCount = tabList->numTabs;

		if (tabCount > 0) {

			STabInfo	*tabInfo = tabList->tabs;

										// Make a tab button for each tab
			for (SInt16 i = 0; i < tabCount; i++) {

				AddTabButton(tabInfo->tabName, tabInfo->iconSuiteID, false);

										// TabInfo is variable-sized because
										//   of the string, so we must manually
										//   move to the next item

				tabInfo = (STabInfo*) ( (char*) tabInfo + sizeof(SInt16) +
														  tabInfo->tabName[0] + 1 +
														  sizeof(SInt32) +
														  sizeof(SInt16) );
			}

										// For a Tabs Control, min is always 1
										//   and max is the number of tabs
			mControlPane->SetMinValue(1);
			mControlPane->SetMaxValue(tabCount);
		}

		if (inSelectionIndex > 0) {		// Set initially selected tab
			SetSelectedTabButtonByIndex(inSelectionIndex);
		}
	}
}


#pragma mark -
#pragma mark === LISTENING

// ---------------------------------------------------------------------------
//	е ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{

	SInt32	value = *(SInt32 *) ioParam;

												// Locate the button that was pressed and then do
												// the right thing about turning off the current
												// button and then broadcasting the index of the
												// newly selected tab
	LView*	super = mControlPane->GetSuperView ();
	LGATabsButton* tabButton = (LGATabsButton*)super->FindPaneByID ( inMessage );
	if ( value == Button_On && tabButton ) {
												// Get the currently selected button
		LGATabsButton* currButton = GetSelectedTabButton ();

												// Make sure that the currently selected button is
												// turned off if it is not the button that has just
												// been selected, this would never happen when
												// clicking on buttons but it could happen if a
												// button is being set procedurally
		if ( currButton != nil && currButton != tabButton ) {
			ToggleTabButtonState ( currButton, false );
		}
												// Setup our reference to the new tab button, we
												// don't need to select the button as this was
												// done by the user clicking on it so we just setup
												// the reference
		SetSelectedTabButton ( tabButton, false );

												// Now we need to call the super view's SetValue
												// which will result in all the correct things
												// happening including the broadcasting of the
												// value message which is important.  Another thing
												// that will happen as part of this process is that
												// the correct value will also be poked into the
												// value field of the control panel so that the
												// correct thing will happen when GetValue is
												// called
		LControlView*	super = (LControlView*)mControlPane->GetSuperView ();
		if ( super ){
			super->SetValue ( GetCurrentTabIndex ());
		}
	}
}


#pragma mark -
#pragma mark === DATA HANDLING

// ---------------------------------------------------------------------------
//	е SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	LGATabsButton*	tabButton;
												// Handle the tags for this class, we won't handle
												// the font style tag as we are not supporting that
												// approach as we use text traits instead
	switch (inTag) {

//		case kControlTabContentRectTag: {
//			Rect contentRect = *(Rect *)inDataPtr;
												// еее Now what do we do with this? Do we resize the
												// control to fit the content rect?
//			break;
//		}

		case kControlTabEnabledFlagTag: {
			Boolean enableIt = *(Boolean *)inDataPtr;
			tabButton = GetTabButtonByIndex(inPartCode);
			if (tabButton != nil) {
				if (enableIt) {
					tabButton->Enable();
				} else {
					tabButton->Disable();
				}
			}
			break;
		}

		case kControlTabInfoTag: {
			tabButton = GetTabButtonByIndex(inPartCode);
			if (tabButton != nil) {
				ControlTabInfoRec*	info = (ControlTabInfoRec*) inDataPtr;
				tabButton->SetDescriptor(info->name);
				tabButton->SetIconResourceID(info->iconSuiteID);
			}
			break;
			
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	е GetDataTag													  [public]
// ---------------------------------------------------------------------------
//

void
LGATabsControlImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
							// Handle the tags for this class, we won't handle
							// the font style tag as we are not supporting that
							// approach as we use text traits instead
	switch (inTag) {

		case kControlTabContentRectTag: {
			Rect contentRect;
			CalcLocalTabPanelRect ( contentRect );
			::MacInsetRect ( &contentRect, 4, 4 );
			*(Rect *)inBuffer = contentRect;
			if ( outDataSize != nil )
				*outDataSize = sizeof ( contentRect );
		}
		break;

		case kControlTabEnabledFlagTag: {
			LGATabsButton*	tabButton = GetTabButtonByIndex ( inPartCode );
			if (tabButton != nil) {
				Boolean	value = tabButton->GetEnabledState ();
				*(Boolean *)inBuffer = value;
				if (outDataSize != nil) {
					*outDataSize = sizeof(value);
				}
			}
		}
		break;

		case kControlTabInfoTag: {
			LGATabsButton*	tabButton = GetTabButtonByIndex(inPartCode);
			if (tabButton != nil) {
				ControlTabInfoRec*	info = (ControlTabInfoRec*) inBuffer;
				tabButton->GetDescriptor(info->name);
				info->iconSuiteID = tabButton->GetIconResourceID();
				if (outDataSize != nil) {
					*outDataSize = sizeof(ControlTabInfoRec);
				}
			} else {
				ThrowOSErr_(errInvalidPartCode);
			}
			break;
		}
		
		default:
			LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			break;
	}
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	е DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::DrawSelf()
{
	Rect localFrame;
	CalcLocalTabPanelRect(localFrame);

	StClipRgnState	theClipState;
										// We are going to modify the clipping region so
										// that it excludes the area where buttons will be
										// drawn, that way we don't get unnecessary
										// drawing behind the buttons
	StRegion	currClipRgn;
	::GetClip(currClipRgn);

	Rect 	buttonClipping;
	CalcTabButtonClipping(buttonClipping);
	currClipRgn -= buttonClipping;

	::SetClip(currClipRgn);

	StColorPenState::Normalize();

	if (mControlPane->GetBitDepth() < 4) {	// BLACK & WHITE

											// Draw a black line along the left, top and right
											// edges
		::RGBForeColor(&Color_Black);
		::MacFrameRect(&localFrame);

											// We also need to erase the contents of the panel
											// so that things redraw properly
		::MacInsetRect(&localFrame, 1, 1);
		::EraseRect(&localFrame);

	} else {								// COLOR
		DrawColorTabPanel ();
	}
}


// ---------------------------------------------------------------------------
//	е DrawColorTabPanel											   [protected]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::DrawColorTabPanel()
{
	StColorPenState::Normalize();

	Rect localFrame;
	CalcLocalTabPanelRect ( localFrame );

												// First we frame the panel
	RGBColor	tempColor = Color_Black;

	if (!IsEnabled() || !IsActive()) {
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor(&tempColor);
	::MacFrameRect(&localFrame);

												// Setup the background color and then do an erase
	ApplyForeAndBackColors();
	::MacInsetRect ( &localFrame, 1, 1 );
	::EraseRect ( &localFrame );

												// All of the shadow drawing is only done if we are
												// active
	if ( IsEnabled () && IsActive ()) {
												// Render the gray shade along the left and top
												// edges
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

												// Render the gray shade along the right and bottom edges
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

												// Render the white edge along the left and top
												// edges
		::RGBForeColor ( &Color_White );
		UGraphicUtils::TopLeftSide ( localFrame, 1, 1, 2, 2 );

												// Render the next gray shade along the right and
												// bottom edges
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 2, 2, 1, 1 );

												// Now fix up the two corner pixels
												// BOTTOM LEFT
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
		UGraphicUtils::PaintColorPixel ( localFrame.left,
										(SInt16) (localFrame.bottom - 1),
										tempColor );

												// TOP RIGHT
		UGraphicUtils::PaintColorPixel ( (SInt16) (localFrame.right - 1),
										localFrame.top,
										tempColor );

	}
}


#pragma mark -
#pragma mark === LAYOUT

// ---------------------------------------------------------------------------
//	е BuildTabButtonView										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsControlImp::BuildTabButtonView()
{
	Rect localFrame;
	mControlPane->GetSuperView()->CalcLocalFrameRect( localFrame );

	SPaneInfo paneInfo;			// Setup the pane info for the button view

	paneInfo.paneID = FOUR_CHAR_CODE('BUTV');
	paneInfo.width = UGraphicUtils::RectWidth ( localFrame );
	paneInfo.height = WantsLargeTab () ? tabPanel_LargeTabButtonHeight :
										tabPanel_SmallTabButtonHeight;
	paneInfo.visible = true;
	paneInfo.enabled = true;
	paneInfo.bindings.left = true;
	paneInfo.bindings.top = false;
	paneInfo.bindings.right = true;
	paneInfo.bindings.bottom = false;
	paneInfo.left = localFrame.left + 1;
	paneInfo.top = localFrame.top;
	paneInfo.userCon = nil;
	paneInfo.superView = mControlPane->GetSuperView();

	SViewInfo	viewInfo;		// Setup the view info

	viewInfo.imageSize.width = 0;
	viewInfo.imageSize.height = 0;
	viewInfo.scrollPos.v = 0;
	viewInfo.scrollPos.h = 0;
	viewInfo.scrollUnit.h = 0;
	viewInfo.scrollUnit.v = 0;
	viewInfo.reconcileOverhang = false;

								// Now create the actual view
	LView* buttonView = new LView ( paneInfo, viewInfo );

	mTabButtons = buttonView;	// Save off a reference to the button view
}


#pragma mark -
#pragma mark === SEARCHING

// ---------------------------------------------------------------------------
//	е FindTabButtonByTitle										   [protected]
// ---------------------------------------------------------------------------

LGATabsButton*
LGATabsControlImp::FindTabButtonByTitle(
	ConstStringPtr	inTabTitle)
{
												// Iterate over the list of tab buttons looking
												// for a match with the tab title passed in to us
	LArrayIterator iterator ( GetTabButtons (), LArrayIterator::from_Start );
	LGATabsButton* theButton;
	while ( iterator.Next ( &theButton )) {

												// Check the button's name against the tab title
												// and if there is a match then return the button
		LStr255 buttonName;
		theButton->GetDescriptor ( buttonName );
		if ( buttonName.CompareTo ( inTabTitle ) == 0 ) {
			return theButton;
		}
	}
												// No button was found
	return nil;
}


// ---------------------------------------------------------------------------
//	е FindTabTitleByIndex										   [protected]
// ---------------------------------------------------------------------------

Boolean
LGATabsControlImp::FindTabTitleByIndex(
	Str255		outTitle,
	ArrayIndexT	inIndex)
{
												// Retrieve the title from our title list if the
												// index is not a valid one false will be returned
	LArray&	tabButtons = GetTabButtons ();
	LGATabsButton*	aTabButton = nil;
	tabButtons.FetchItemAt ( inIndex, &aTabButton );
	if ( aTabButton ) {
												// Check the button's name against the tab title
												// and if there is a match then return the button
		aTabButton->GetDescriptor ( outTitle );

												// We got the title so return true
		return true;
	}
												// Title wasn't found so return false
	return false;
}


#pragma mark -
#pragma mark === MISCELLANEOUS

// ---------------------------------------------------------------------------
//	е GetTabButtonID											   [protected]
// ---------------------------------------------------------------------------

ArrayIndexT
LGATabsControlImp::GetTabButtonID(
	PaneIDT&	outPaneID)
{
												// Start by creating an initial ID which is 'tb'
	OSType tempID = FOUR_CHAR_CODE('tb  ');
	Str255 idString;
	LString::FourCharCodeToPStr ( tempID, idString );

												// Next figure out the index for the tab title and
												// concatenate that onto the ID
	ArrayIndexT tabIndex = GetNextIndex ();
	Str255 indexString;
	::NumToString ( tabIndex, indexString );
	idString[0] = 2;
	LString::AppendPStr ( idString, indexString );

												// Convert it all back to the pane ID
	LString::PStrToFourCharCode ( idString, (FourCharCode&)outPaneID );

	return tabIndex;
}


// ---------------------------------------------------------------------------
//	е RebuildButtonIdentifiers									   [protected]
// ---------------------------------------------------------------------------
//
//	This function is called when the page index counter overflows, what it does is
// simply rebuild the button IDs starting from 1.  This will get called when we
//	run out of indices at 99, the most common cause will be that the user has added
// and deleted too many buttons and we have simply exhausted the available range of
// indices that can be used to build IDs.

void
LGATabsControlImp::RebuildButtonIdentifiers()
{

												// We start by first clearing the index counter
	ClearIndexCounter();

												// Iterate over the list of tab buttons changing
												// all of their IDs so that we get the ID range
												// compacted
	LArrayIterator iterator ( GetTabButtons (), LArrayIterator::from_Start );
	LGATabsButton* theButton;
	while ( iterator.Next ( &theButton )) {
												// Get a new pane ID
		PaneIDT	newID;
		GetTabButtonID ( newID );

												// Change the button's ID to the new one
		if ( theButton ) {
			theButton->SetPaneID ( newID );
		}
	}
}


PP_End_Namespace_PowerPlant
