// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageController.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPageController.h>
#include <LPageButton.h>
#include <LPageArrow.h>

#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LVariableArray.h>
#include <LArrayIterator.h>
#include <LString.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UMemoryMgr.h>
#include <UQDOperators.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


enum {
	pageController_IconOffset			= 3,
	pageController_ButtonSlop			= 24,
	pageController_BottomOffset			= 12,
	pageController_ArrowControlWidth	= 20
};


// ---------------------------------------------------------------------------
//	¥ LPageController						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPageController::LPageController()
{
	mSelectedButton		= nil;
	mControllerMessage	= msg_AnyMessage;
	mLeftArrow			= nil;
	mRightArrow			= nil;
	mPageButtons		= nil;
	mHaveScrolling		= false;
	mStringListID		= 0;
	mSelectionIndex		= 0;
	mInitialPageTitles	= nil;
	mBackColor			= pageButton_DefaultBackColor;
	mFaceColor			= pageButton_DefaultFaceColor;
	mPushedTextColor	= pageButton_DefaultFaceColor;
	mNextIndex			= 0;
	mGroupedArrows		= true;
}


// ---------------------------------------------------------------------------
//	¥ LPageController						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LPageController::LPageController(
	const LPageController	&inOriginal)

		: LView(inOriginal)
{
	mSelectedButton		= nil;
	mControllerMessage	= inOriginal.mControllerMessage;
	mLeftArrow			= nil;
	mRightArrow			= nil;
	mPageButtons		= nil;
	mHaveScrolling		= inOriginal.mHaveScrolling;
	mStringListID		= inOriginal.mStringListID;
	mSelectionIndex		= inOriginal.mSelectionIndex;
	mInitialPageTitles	= nil;
	mBackColor			= inOriginal.mBackColor;
	mFaceColor			= inOriginal.mFaceColor;
	mPushedTextColor	= inOriginal.mPushedTextColor;
	mNextIndex			= inOriginal.mNextIndex;
	mGroupedArrows		= inOriginal.mGroupedArrows;
}


// ---------------------------------------------------------------------------
//	¥ LPageController						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPageController::LPageController(
	LStream*	inStream)

		: LView(inStream)
{
	mSelectedButton		= nil;
	mLeftArrow			= nil;
	mRightArrow			= nil;
	mPageButtons		= nil;
	mHaveScrolling		= false;
	mStringListID		= 0;
	mSelectionIndex		= 0;
	mInitialPageTitles	= nil;
	mNextIndex			= 0;

	*inStream >> mTextTraitsID;
	*inStream >> mControllerMessage;
	*inStream >> mStringListID;
	*inStream >> mSelectionIndex;

	*inStream >> mBackColor.red;
	*inStream >> mBackColor.green;
	*inStream >> mBackColor.blue;
	*inStream >> mFaceColor.red;
	*inStream >> mFaceColor.green;
	*inStream >> mFaceColor.blue;
	*inStream >> mPushedTextColor.red;
	*inStream >> mPushedTextColor.green;
	*inStream >> mPushedTextColor.blue;

	*inStream >> mGroupedArrows;
}


// ---------------------------------------------------------------------------
//	¥	LPageController						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPageController::LPageController(
	const SPaneInfo& 	inPaneInfo,
	const SViewInfo& 	inViewInfo,
	ResIDT				inTextTraitsID,
	LVariableArray*		inPageTitles,
	ArrayIndexT			inSelectionIndex,
	MessageT			inMessage,
	const RGBColor&		inBackColor,
	const RGBColor&		inFaceColor,
	const RGBColor&		inPushedTextColor,
	bool				inGroupArrows)

	: LView(inPaneInfo, inViewInfo)
{

	mSelectedButton		= nil;
	mLeftArrow			= nil;
	mRightArrow			= nil;
	mPageButtons		= nil;
	mHaveScrolling		= false;
	mStringListID		= 0;
	mSelectionIndex		= 0;
	mInitialPageTitles	= nil;
	mNextIndex			= 0;
	mGroupedArrows		= inGroupArrows;
	mTextTraitsID		= inTextTraitsID;
	mInitialPageTitles	= inPageTitles;
	mSelectionIndex		= (SInt16) inSelectionIndex;
	mControllerMessage	= inMessage;
	mBackColor			= inBackColor;
	mFaceColor			= inFaceColor;
	mPushedTextColor	= inPushedTextColor;
}


// ---------------------------------------------------------------------------
//	¥ LPageController						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPageController::LPageController(
	const SPaneInfo& 	inPaneInfo,
	const SViewInfo& 	inViewInfo,
	ResIDT				inTextTraitsID,
	ResIDT				inStringListID,
	SInt16				inSelectionIndex,
	MessageT			inMessage,
	const RGBColor&		inBackColor,
	const RGBColor&		inFaceColor,
	const RGBColor&		inPushedTextColor,
	bool				inGroupArrows)

	: LView(inPaneInfo, inViewInfo)
{
	mSelectedButton		= nil;
	mLeftArrow			= nil;
	mRightArrow			= nil;
	mPageButtons		= nil;
	mHaveScrolling		= false;
	mStringListID		= 0;
	mSelectionIndex		= 0;
	mInitialPageTitles	= nil;
	mNextIndex			= 0;
	mGroupedArrows		= inGroupArrows;
	mTextTraitsID		= inTextTraitsID;
	mStringListID		= inStringListID;
	mSelectionIndex		= inSelectionIndex;
	mControllerMessage	= inMessage;
	mBackColor			= inBackColor;
	mFaceColor			= inFaceColor;
	mPushedTextColor	= inPushedTextColor;
}


// ---------------------------------------------------------------------------
//	¥ ~LPageController						Destructor				  [public]
// ---------------------------------------------------------------------------

LPageController::~LPageController()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf												  [public]
// ---------------------------------------------------------------------------

void
LPageController::FinishCreateSelf()
{
									// Build the view that holds the page buttons
	BuildPageButtonView ();
									// Now call our function that handles the actual
									// loading, we only do this if we have a string
									// list ID
	if ( mStringListID > 0 ) {
		LoadPageTitles ( mStringListID, mSelectionIndex );

	} else if ( mInitialPageTitles != nil ) {

		LoadPageTitlesFromArray ( mInitialPageTitles, mSelectionIndex );

									// Cleanup by deleting the array of titles
		delete mInitialPageTitles;

									// After the title have been loaded make sure
									// we set our reference to nil as the array has
									// been deleted by the previous call
		mInitialPageTitles = nil;
	}
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetCurrentPageIndex											  [public]
// ---------------------------------------------------------------------------

ArrayIndexT
LPageController::GetCurrentPageIndex() const
{
	ArrayIndexT		currIndex  = 0;
	LPageButton*	currButton = GetSelectedPageButton();

	if (currButton != nil) {
		LArray&		subPanes = mPageButtons->GetSubPanes();

		currIndex = subPanes.FetchIndexOf(&currButton);
	}

	return currIndex;
}


// ---------------------------------------------------------------------------
//	¥ GetValue														  [public]
// ---------------------------------------------------------------------------

SInt32
LPageController::GetValue() const
{
	return GetCurrentPageIndex();
}


// ---------------------------------------------------------------------------
//	¥ GetPageIndex													  [public]
// ---------------------------------------------------------------------------

ArrayIndexT
LPageController::GetPageIndex(
	ConstStringPtr inPageTitle)
{
	ArrayIndexT		pageIndex = 0;
	LArray&			subPanes = GetPageButtons ();
	LArrayIterator	iterator(subPanes);
	LPane*			pageButton = nil;

								// Search for PageButton with specified title
	while (iterator.Next(&pageButton))  {

		LStr255	theTitle;
		pageButton->GetDescriptor(theTitle);
		if (inPageTitle == theTitle) {
								// Match found. Get index of the button.
			pageIndex = subPanes.FetchIndexOf(&pageButton);
			break;
		}
	}

	return pageIndex;
}


// ---------------------------------------------------------------------------
//	¥ NeedsScrolling												  [public]
// ---------------------------------------------------------------------------

Boolean
LPageController::NeedsScrolling ()
{
													// Figure out the width of the controller
	if ( mPageButtons ) {
		Rect	localFrame;
		SInt16	frameWidth;
		CalcLocalFrameRect ( localFrame );
		frameWidth = (SInt16) (UGraphicUtils::RectWidth ( localFrame ) - 2);

													// Now iterate over all the buttons figuring
													// out the total width they occupy
		LArrayIterator iterator ( GetPageButtons(), LArrayIterator::from_Start );
		SInt16 totalWidth = 0;
		LPageButton* theButton;
		while ( iterator.Next ( &theButton ))  {
			SDimension16	buttonSize;
			theButton->GetFrameSize ( buttonSize );
			totalWidth += buttonSize.width;
		}

													// If the total width of the titles is greater
													// than the frame width we will need to handle scrolling
		return totalWidth > frameWidth;
	}

	return false;

}


// ---------------------------------------------------------------------------
//	¥ CalcPageButtonWidth											  [public]
// ---------------------------------------------------------------------------

SInt16
LPageController::CalcPageButtonWidth(
	ConstStringPtr	inPageTitle)
{
	StTextState		theTextState;
													// Start by making sure we setup the correct
													// textstyle for the view so that the title
													// widths can be correctly calculated
	UTextTraits::SetPortTextTraits( GetTextTraitsID() );

													// Return the width of the title plus the
													// button slop
	return ( (SInt16) (::StringWidth(inPageTitle) + pageController_ButtonSlop) );

}


// ---------------------------------------------------------------------------
//	¥ CalcControllerVisibleRect										  [public]
// ---------------------------------------------------------------------------

void
LPageController::CalcControllerVisibleRect ( Rect &outRect )
{
													// Start with the frame rect
	CalcLocalFrameRect ( outRect );
													// If we are setup for scrolling it means that
													// none of the controller is visible so we just
													// return an empty rect, otherwise we figure out
													// how much of the controller is exposed. This
													// is done by calling the function that figures
													// out the next available button location, which
													// is essentially the rightmost edge of all the
													// buttons
	if ( HaveScrolling ()) {
		outRect.left = outRect.right = outRect.top = outRect.bottom = 0;

	} else {

		SInt16	lastLoc = GetLastPageButtonLocation ();
		outRect.left += lastLoc /*+ 1*/;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetTextTraitsID(
	ResIDT	inTextTraitsID)
{
	mTextTraitsID = inTextTraitsID;
}


// ---------------------------------------------------------------------------
//	¥ SetSelectedPageButton											  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetSelectedPageButton(
	LPageButton*	inSelectedButton,
	Boolean			inSelectButton,
	Boolean			inSuppressBroadcast)
{
	if ( GetSelectedPageButton () != inSelectedButton ) {

									// Deselect current button if we are
									//   selecting a new button
		if ( mSelectedButton && inSelectButton ) {
			TogglePageButtonState ( mSelectedButton, false, true );
		}

		mSelectedButton = inSelectedButton;

		RevealSelectedPageButton ();

		if ( inSelectButton ) {		// Draw button in selected state
			TogglePageButtonState ( inSelectedButton, true, true );
		}

									// Broadcast message for page change
		if ( !inSuppressBroadcast ) {
			SInt32 currPageIndex = GetCurrentPageIndex ();
			if ( currPageIndex > 0 ) {
				BroadcastMessage ( GetControllerMessage (), &currPageIndex );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetSelectedPageButtonByIndex									  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetSelectedPageButtonByIndex(
	ArrayIndexT		inSelectionIndex,
	Boolean			inSuppressBroadcast)
{
	LPageButton*	pageButton;

	if (GetPageButtons().FetchItemAt(inSelectionIndex, &pageButton)) {
		SetSelectedPageButton(pageButton, true, inSuppressBroadcast);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetValue(
	SInt32		inValue)
{
	SetSelectedPageButtonByIndex(inValue, true);
}


// ---------------------------------------------------------------------------
//	¥ TogglePageButtonState											  [public]
// ---------------------------------------------------------------------------

void
LPageController::TogglePageButtonState(
	LPageButton*	inSelectedButton,
	Boolean			inSelected,
	Boolean			inSuppressBroadcast)
{
	if ( inSelectedButton->IsPushed () != inSelected ) {

		SInt32	newValue = 0;
		if (inSelected) {
			newValue = 1;
		}

		if ( inSuppressBroadcast ) {

			MessageT storeMessage = inSelectedButton->GetValueMessage ();
			inSelectedButton->SetValueMessage ( 0 );
			inSelectedButton->SetValue ( newValue );
			inSelectedButton->SetValueMessage ( storeMessage );
			inSelectedButton->SetPushedState ( inSelected );

		} else {

			inSelectedButton->SetValue ( newValue );
			inSelectedButton->SetPushedState ( inSelected );
		}

		inSelectedButton->Draw ( nil );
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageController::EnableSelf ()
{
	Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageController::DisableSelf ()
{
	Draw(nil);
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageController::ActivateSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LPageController::DeactivateSelf()
{
	Refresh();
}


#pragma mark -
#pragma mark === PAGE BUTTON MANAGEMENT

// ---------------------------------------------------------------------------
//	¥ AddPageButton													  [public]
// ---------------------------------------------------------------------------

void
LPageController::AddPageButton(
	ConstStringPtr  inPageTitle,
	Boolean  		inSelected)
{
													// Now we need to get a button built for the
													// new title this button is then added to the
													// end of the button view
	PaneIDT	newButtonID;
	GetPageButtonID ( newButtonID );
													// Figure out the location at which the button
													// will be placed
	SPoint32 	buttonLoc;
	buttonLoc.h = GetLastPageButtonLocation ();
	buttonLoc.v = 0;
													// Figure out the size of the button
	Rect localFrame;
	mPageButtons->CalcLocalFrameRect ( localFrame );
	SDimension16	buttonSize;
	buttonSize.width = CalcPageButtonWidth ( inPageTitle );
	buttonSize.height = UGraphicUtils::RectHeight ( localFrame );

													// Get the button created
	CreatePageButton ( 	inPageTitle,
							newButtonID,
							buttonSize,
							buttonLoc,
							inSelected );
													// After a new button has been added for the
													// title we always need to call this function
													// so that the right things can be done about
													// getting scrolling synched up if needed
	SynchScrollArrows();

}


// ---------------------------------------------------------------------------
//	¥ InsertPageButtonAt											  [public]
// ---------------------------------------------------------------------------

void
LPageController::InsertPageButtonAt(
	ConstStringPtr  inPageTitle,
	ArrayIndexT		inAtIndex)
{
													// Before we proceed we make sure that the index
													// we have been handed is not out of range, if
													// it is greater than the current button count
													// then we are going to simply call
													// AddPageButton which will get the new button
													// appended to the end of the button list
	SInt16	buttonCount = GetPageButtonCount ();
	Boolean compactIDs = false;
	ArrayIndexT buttonIndex = GetCurrentIndex () + 1;

													// INSERTING
	if ( inAtIndex > 0 && inAtIndex <= buttonCount ) {

		LArray&	buttonList = GetPageButtons ();

													// We start out by first checking to make sure
													// that there is a valid index available for
													// this button otherwise we will need to get the
													// indices compacted
		if ( buttonIndex > 99 ) {
			compactIDs = true;
		}
													// Get an ID for the button
		PaneIDT	newButtonID;
		GetPageButtonID ( newButtonID );

													// Figure out the button location
		SPoint32 	buttonLoc;
		LPageButton*	aPageButton;
		buttonList.FetchItemAt ( inAtIndex, &aPageButton );
		aPageButton->GetFrameLocation ( buttonLoc );

													// Was the button selected
		Boolean	wasSelected = aPageButton->IsSelected ();

													// Vertical location of the button is always 0
		buttonLoc.v = 0;

													// If the page controller is currently setup to
													// scroll we need to tweak the button's
													// horizontal location by the width of a scroll
													// arrow
		if ( HaveScrolling ()) {
			buttonLoc.h -= 19;
		}
													// Figure out the size of the button
		Rect localFrame;
		mPageButtons->CalcLocalFrameRect ( localFrame );
		SDimension16	buttonSize;
		buttonSize.width = CalcPageButtonWidth ( inPageTitle );
		buttonSize.height = UGraphicUtils::RectHeight ( localFrame );

													// Get the button created
		CreatePageButton ( 	inPageTitle,
									newButtonID,
									buttonSize,
									buttonLoc,
									false );
													// After a new button has been added for the
													// title we always need to call this function
													// so that the right things can be done about
													// getting scrolling synched up if needed
		SynchScrollArrows ();

													// Now we have to manipulate its location in the
													// list as it is important that it be in the
													// correct location
		ArrayIndexT	lastIndex = GetPageButtonCount ();

													// move the item fromm the end of the list to
													// the insertion location
		buttonList.MoveItem ( lastIndex, inAtIndex );

													// Finally, we need to get the locations of all
													// the buttons after this one adjusted to
													// accomodate the new button
		buttonCount = (SInt16) buttonList.GetCount ();
		for ( SInt16 index = (SInt16) (inAtIndex + 1); index <= buttonCount; index++ ) {

			buttonList.FetchItemAt ( index, &aPageButton );
			if ( aPageButton ) {
				aPageButton->MoveBy ( buttonSize.width, 0, true );
			}
		}
													// If the tab that was at this location was
													// selected then we need to make sure we do the
													// same for the new button
		if ( wasSelected ) {
			SetSelectedPageButtonByIndex ( inAtIndex );
		}
													// ADDING
	} else if ( inAtIndex > buttonCount ) {

													// We start out by first checking to make sure
													// that there is a valid index available for
													// this button otherwise we will need to get the
													// indices compacted
		if ( buttonIndex > 99 ) {
			compactIDs = true;
		}
													// Add the button to the end of the button list
		AddPageButton ( inPageTitle, false );
	}
													// If we are going to handle rebuilding the page
													// button IDs then we do it now
	if ( compactIDs ) {
		RebuildButtonIdentifiers ();
	}
													// If we do not have scroll arrows make sure
													// that we get the controller refreshed as there
													// could be some droppings left behind when a
													// button is removed
	if ( !HaveScrolling ()) {
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ RemovePageButtonAt																[public]
// ---------------------------------------------------------------------------

void
LPageController::RemovePageButtonAt (
		ArrayIndexT inAtIndex )
{
													// Before we proceed we make sure that the index
													// we have been handedbis not out of range, if
													// it is we simply do nothing
	SInt16	buttonCount = GetPageButtonCount ();
	if ( inAtIndex > 0 && inAtIndex <= buttonCount ) {

		LArray&	buttonList = GetPageButtons ();

													// Find then button that is being removed
		LPageButton*	aPageButton;
		buttonList.FetchItemAt ( inAtIndex, &aPageButton );

													// Was the button selected
		Boolean	wasSelected = aPageButton->IsPushed ();

													// Figure out the size of the button
		Rect localFrame;
		mPageButtons->CalcLocalFrameRect ( localFrame );
		SDimension16	buttonSize;
		aPageButton->GetFrameSize ( buttonSize );

													// Remove the button from the list
		aPageButton->Refresh ();
		aPageButton->Hide ();
		aPageButton->PutInside ( nil );
		delete aPageButton;

													// We need to get the locations of all the
													// buttons after this one adjusted to accomodate
													// the space left by the removed button
		buttonCount = (SInt16) buttonList.GetCount ();
		for ( SInt16 index = (SInt16) inAtIndex; index <= buttonCount; index++ ) {

			buttonList.FetchItemAt ( index, &aPageButton );
			if ( aPageButton ) {
				aPageButton->MoveBy ( -buttonSize.width, 0, true );
			}
		}
													// if the button was selected we need to get
													// another one selected, for that we just use
													// the same index
		if ( wasSelected ) {
													// Clear the button reference
			mSelectedButton = nil;

													// If the index is greater than the number of
													// buttons we will simply select the last button
													// in the controller
			if ( buttonCount < inAtIndex ) {
				inAtIndex = buttonCount;
			}
													// Get the appropriate button selected
			SetSelectedPageButtonByIndex ( inAtIndex, false );
		}
													// After the button has been removed we always
													// need to call this function so that the right
													// things can be done about getting scrolling
													// synched up if needed
		SynchScrollArrows ();
													// If we do not have scroll arrows make sure
													// that we get the controller refreshed as there
													// could be some droppings left behind when a
													// button is removed
		if ( !HaveScrolling ()) {
			Refresh ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CreatePageButton												  [public]
// ---------------------------------------------------------------------------

LPageButton*
LPageController::CreatePageButton(
	ConstStringPtr	inButtonTitle,
	PaneIDT			inButtonID,
	SDimension16	inButtonSize,
	SPoint32		inButtonPosition,
	Boolean			inButtonPushed )
{
													// Setup the pane info
	SPaneInfo paneInfo;

	paneInfo.paneID = inButtonID;
	paneInfo.width = inButtonSize.width;
	paneInfo.height = inButtonSize.height;
	paneInfo.visible = true;
	paneInfo.enabled = true;
	paneInfo.bindings.left = false;
	paneInfo.bindings.top = false;
	paneInfo.bindings.right = false;
	paneInfo.bindings.bottom = false;
	paneInfo.left = inButtonPosition.h;
	paneInfo.top = inButtonPosition.v;
	paneInfo.userCon = nil;
	paneInfo.superView = GetPageButtonView ();

													// Setup the control info
	SControlInfo	controlInfo;

	controlInfo.valueMessage = inButtonID;
	controlInfo.value = inButtonPushed ? 1 : 0;
	controlInfo.minValue = 0;
	controlInfo.maxValue = 1;

													// Now create the actual button
	RGBColor		backColor = GetPageButtonBackColor ( inButtonTitle, inButtonID );
	RGBColor		faceColor = GetPageButtonFaceColor ( inButtonTitle, inButtonID );
	RGBColor		textColor = GetPageButtonPushedTextColor ( inButtonTitle, inButtonID );

	LPageButton* aPageButton = new LPageButton ( 	paneInfo,
													controlInfo,
													inButtonTitle,
													true,
													GetTextTraitsID (),
													backColor,
													faceColor,
													textColor );

													// We need to make sure that we call
													// FinishCreate so that everything is happy
	aPageButton->FinishCreate ();
													// Add the controller as a listener to the
													// button
	aPageButton->AddListener ( this );

													// Make sure the button is set to broadcast
	aPageButton->StartBroadcasting ();
													// If the button is set to be pushed then get
													// it setup that way
	if ( inButtonPushed ) {
		SetSelectedPageButton ( aPageButton, true );
	}
													// Now return the newly built button
	return aPageButton;

}


// ---------------------------------------------------------------------------
//	¥ GetPageButtonCount											  [public]
// ---------------------------------------------------------------------------

SInt16
LPageController::GetPageButtonCount() const
{
	return (SInt16) mPageButtons->GetSubPanes().GetCount();
}


// ---------------------------------------------------------------------------
//	¥ DeleteAllPageButtons											  [public]
// ---------------------------------------------------------------------------

void
LPageController::DeleteAllPageButtons ()
{
													// To delete all of the buttons we simply call
													// the DeleteAllSubPanes method in LView which
													// works nicely for us, we also then need to do
													// a refresh so that the change shows up
	if ( mPageButtons ) {
		mPageButtons->DeleteAllSubPanes ();
		Refresh ();
	}
													// Make sure we zero out the index count
	ClearIndexCounter ();
													// We also need to make sure that the selected
													// button reference is cleared as it will be
													// invalid the next time it is called
	mSelectedButton = nil;

}


// ---------------------------------------------------------------------------
//	¥ GetLastPageButtonLocation														[public]
// ---------------------------------------------------------------------------

SInt16
LPageController::GetLastPageButtonLocation ()
{
													// If there are buttons then find the last one
													// and return the current coordinate for its
													// right edge
	LArray&	buttonList = GetPageButtons ();
	if ( buttonList.GetCount () > 0 ) {
		LPageButton*	lastButton;
		buttonList.FetchItemAt ( (ArrayIndexT) buttonList.GetCount (), &lastButton );
		Rect lastButtonFrame;
		lastButton->CalcLocalFrameRect ( lastButtonFrame );

													// Return right edge coordinate
		return lastButtonFrame.right;
	}
													// There are currently no buttons so just return
													// 0
	return 0;

}


// ---------------------------------------------------------------------------
//	¥ RevealSelectedPageButton														[public]
// ---------------------------------------------------------------------------

Boolean
LPageController::RevealSelectedPageButton ()
{

	Boolean revealed = false;

													// Now we need to potentially do a bunch of work
													// to ensure that the button is made fully
													// visible if only part of it was showing when
													// the user selected it, this is only done if
													// there is currently a selected button on which
													// this action is performed
	if ( mSelectedButton ) {
		Rect	buttonFrame;
		Rect	buttonViewFrame;
		Rect 	intersectRect;
		mSelectedButton->CalcLocalFrameRect ( buttonFrame );
		mPageButtons->CalcLocalFrameRect ( buttonViewFrame );

													// If the selected button is partially obscured
													// then we need to scroll it so that it becomes
													// fully visible
		if ( ::SectRect ( &buttonViewFrame, &buttonFrame, &intersectRect )) {

			SInt16 delta = (SInt16) (UGraphicUtils::RectWidth(buttonFrame) -
									 UGraphicUtils::RectWidth(intersectRect));
			if ( delta > 0 ) {

				if ( buttonFrame.left > 0 ) {
					ScrollPageButtons ( scrollDirection_Left, delta, true );
				} else {
					ScrollPageButtons ( scrollDirection_Right, delta, true );
				}
													// We did need to reveal the selected button
				revealed = true;
			}

		} else if ( buttonFrame.left > 0 && buttonFrame.left >= buttonViewFrame.right ) {
													// Subtract the revealed right from the button
													// left then add the button width to arrive at
													// the delta the button  has to scroll to the
													// left by
			SInt16	delta = (SInt16) (buttonFrame.right - buttonViewFrame.right);
			if ( delta > 0 ) {
				ScrollPageButtons ( scrollDirection_Left, delta, true );
																// We did need to reveal the selected button
				revealed = true;
			}

		} else if ( buttonFrame.left < 0 && buttonFrame.left < buttonViewFrame.left ) {

			SInt16	delta = (SInt16) (buttonViewFrame.left - buttonFrame.left);
			ScrollPageButtons ( scrollDirection_Right, delta, true);
			revealed = true;
		}

													// Make sure that the state of the arrows is
													// set, which basically enables an arrow if
													// there is something available to be scrolled
													// in a given direction or disables it if there
													// is not, we only do this if we actually had
													// to reveal a button
		if ( revealed ) {
			UpdateScrollArrowState ();
		}
	}
													// There either was no selected button or there
													// was no need to reveal it as it was already
													// revealed
	return revealed;

}


// ---------------------------------------------------------------------------
//	¥ RevealLeftMostPageButton										  [public]
// ---------------------------------------------------------------------------

void
LPageController::RevealLeftMostPageButton ()
{
													// When the controller becomes non-scrolling we
													// need to check to see if we have to move all
													// of the buttons over so that they are
													// correctly aligned with the left edge.  The
													// reason being is that the  buttons might have
													// beens scrolled and therefore will be left in
													// that state
	LArray&	buttonList = GetPageButtons ();
	if ( buttonList.GetCount () > 0 ) {

		Rect	buttonViewFrame;
		mPageButtons->CalcLocalFrameRect ( buttonViewFrame );

		LPageButton*	firstButton;
		buttonList.FetchItemAt ( 1, &firstButton );
		if ( firstButton ) {

			Rect	buttonFrame;
			firstButton->CalcLocalFrameRect ( buttonFrame );

			if ( buttonFrame.left < 0 ) {
													// Take the absolute value of the left edge of
													// the first button and we have the amount we
													// need to adjust the buttons by to restore the
													// visibility of the first button
				SInt16 delta = GAUtils_Absolute ( buttonFrame.left );
				if ( delta > 0 ) {
					ScrollPageButtons ( scrollDirection_Right, delta, true );
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ AlignRightMostPageButton										  [public]
// ---------------------------------------------------------------------------

void
LPageController::AlignRightMostPageButton ()
{
													// When the controller is enlarged we need to
													// check to make sure that if the buttons had
													// been scrolled to the left we adjust them so
													// that no gap is visible on the right
													// Start by first figuring out if the first
													// button is scrolled past the beginning of the
													// button view
	LArray&	buttonList = GetPageButtons ();
	if ( buttonList.GetCount () > 0 ) {

		Rect	buttonViewFrame;
		mPageButtons->CalcLocalFrameRect ( buttonViewFrame );

		LPageButton*	firstButton;
		buttonList.FetchItemAt ( 1, &firstButton );

		if ( firstButton ) {

			Rect	buttonFrame;
			firstButton->CalcLocalFrameRect ( buttonFrame );

			if ( buttonFrame.left < 0 ) {
													// The first button is passed the beginning of
													// the button view so we potentially have to
													// make an adjustment the absolute value of the
													// buttons left edge will tell us the amount
				SInt16 delta = GAUtils_Absolute ( buttonFrame.left );

													// Now get the location of the right edge of the
													// last button
				SInt16	rightEdge = GetLastPageButtonLocation ();

													// Check to see if the right edge of the button
													// is less than the right edge of the button
													// view, if it is we need to make an adjustment
				if ( rightEdge < buttonViewFrame.right ) {

					SInt16	rightDelta = (SInt16) (buttonViewFrame.right - rightEdge);
					if ( rightDelta < delta ) {
						delta = rightDelta;
					}
													// Finaly, we can do the scrolling of the
													// buttons to the right within the button view
					ScrollPageButtons ( scrollDirection_Right, delta, true );
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SynchScrollArrows												  [public]
// ---------------------------------------------------------------------------

void
LPageController::SynchScrollArrows ()
{
													// We need to make sure that we check to see if
													// we need to make the  scroll arrows visible
													// or hidden
	if ( NeedsScrolling ()) {
													// We only bother setting up the scrolling if
													// its not already setup that way
		if ( !HaveScrolling ()) {
													// Setup the scroll arrows
			SetupScrollArrows ( true );

													// Make sure the selected button is revealed
			RevealSelectedPageButton ();

													// We need to get the page controller refreshed
													// when this happens as it might have been
													// caused by a button being added at runtime
													// which would not typically result in the
													// buttons to the left of the button being
													// added getting refreshed
			Refresh ();

		} else {
													// If we are already setup for scrolling all we
													// need to do is make sure that the rightmost
													// button is not misaligned
			AlignRightMostPageButton ();
		}

	} else if ( HaveScrolling ()) {
													// We only turn scrolling off if it isn't
													// already so
		SetupScrollArrows ( false );

													// When we turn scrolling off we need to make
													// sure that we get the leftmost button aligned
													// with the left edge of the button view, just
													// in case it had previously been scrolled
													// across
		RevealLeftMostPageButton ();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPageButtonBackColor										  [public]
// ---------------------------------------------------------------------------
//
//	Here we always want the page button to be using the same back color as the controller
//	so we simply pass back the controller's back color, although someone might want
//	to do something different so we still have the same parameters for identifying
//	the button being drawn

RGBColor
LPageController::GetPageButtonBackColor(
	ConstStringPtr	/* inButtonTitle */,
	PaneIDT			/* inButtonID */) const
{
	return mBackColor;
}


// ---------------------------------------------------------------------------
//	¥ GetPageButtonFaceColor										  [public]
// ---------------------------------------------------------------------------
//
//	The title and ID of the button being created is passed into this function so that
// someone overridding it could potentially do something with the information to
//	determine the face color, by default this just returns the face color

RGBColor
LPageController::GetPageButtonFaceColor(
	ConstStringPtr	/* inButtonTitle */,
	PaneIDT			/* inButtonID */) const
{
	return mFaceColor;
}


// ---------------------------------------------------------------------------
//	¥ GetPageButtonPushedTextColor									  [public]
// ---------------------------------------------------------------------------
//
//	The title and ID of the button being created is passed into this function so that
// someone overridding it could potentially do something with the information to
//	determine the face color, by default this just returns the pushed text color

RGBColor
LPageController::GetPageButtonPushedTextColor(
	ConstStringPtr	/* inButtonTitle */,
	PaneIDT			/* inButtonID */) const
{
	return mPushedTextColor;
}


// ---------------------------------------------------------------------------
//	¥ SetPageControllerBackColor									  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetPageControllerBackColor(
	const RGBColor&	inBackColor )
{
													// Start by setting the controller's reference
													// to the back color
	mBackColor = inBackColor;
													// Now we need to make sure that all of the
													// page buttons currently installed in the
													// controller are updated
	LArrayIterator iterator ( GetPageButtons () );
	LPageButton* theButton;
	while ( iterator.Next ( &theButton )) {
		theButton->SetPageButtonBackColor ( inBackColor );
	}
													// We now need to make sure we update the arrow
													// controls if they are present
	if ( mLeftArrow != nil ) {
		mLeftArrow->SetArrowBackColor ( inBackColor );
	}
	if ( mRightArrow != nil ) {
		mRightArrow->SetArrowBackColor ( inBackColor );
	}
													// Now we need to make sure we get the whole
													// controller refreshed
	Refresh();

}


// ---------------------------------------------------------------------------
//	¥ SetPageControllerFaceColor									  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetPageControllerFaceColor(
	const RGBColor&	inFaceColor)
{
													// Start by setting the controller's reference
													// to the face color
	mFaceColor = inFaceColor;
													// Now we need to make sure that all of the
													// page buttons currently installed in the
													// controller are updated
	LArrayIterator	iterator( GetPageButtons() );
	LPageButton*	theButton;
	while (iterator.Next(&theButton)) {
		theButton->SetPageButtonFaceColor(inFaceColor);
	}
													// We now need to make sure we update the arrow
													// controls if they are present
	if (mLeftArrow != nil) {
		mLeftArrow->SetArrowFaceColor(inFaceColor);
	}

	if (mRightArrow != nil) {
		mRightArrow->SetArrowFaceColor(inFaceColor);
	}
													// Now we need to make sure we get the whole
													// controller refreshed
	Refresh();

}


// ---------------------------------------------------------------------------
//	¥ SetPageControllerPushedTextColor								  [public]
// ---------------------------------------------------------------------------

void
LPageController::SetPageControllerPushedTextColor(
	const RGBColor&	inTextColor)
{
													// Start by setting the controller's reference
													// to the pushed text color
	mPushedTextColor = inTextColor;
													// Now we need to make sure that all of the
													// page buttons currently installed in the
													// controller are updated
	LArrayIterator iterator ( GetPageButtons (), LArrayIterator::from_Start );
	LPageButton* theButton;
	while ( iterator.Next ( &theButton )) {
		theButton->SetPageButtonPushedTextColor ( inTextColor );
	}
													// Now we need to make sure we get the whole
													// controller refreshed
	Refresh ();
}


#pragma mark -
#pragma mark === LISTENING

// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------

void
LPageController::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam	)
{

	SInt32	value = *(SInt32 *) ioParam;
													// Handle the scrolling of the page buttons when
													// either one of the scroll arrows is pressed.
													// NOTE: pressing the left arrow will scroll
													// buttons to the right and vice versa for the
													// right button
	if ( inMessage == FOUR_CHAR_CODE('left') ) {

		SInt16 scrollIncrement = CalcNextRightScrollIncrement ();
		if ( scrollIncrement > 0 ) {
			ScrollPageButtons ( scrollDirection_Right, scrollIncrement, true );
		}
													// Make sure that the state of the arrows is
													// set, which basically disables an arrow if
													// there is something available to be scrolled
													// in a given direction
		UpdateScrollArrowState ();

	} else if ( inMessage == FOUR_CHAR_CODE('rght') ) {

		SInt16 scrollIncrement = CalcNextLeftScrollIncrement ();
		if ( scrollIncrement > 0 ) {
			ScrollPageButtons ( scrollDirection_Left, scrollIncrement, true );
		}
													// Make sure that the state of the arrows is
													// set, which basically disables an arrow if
													// there is something available to be scrolled
													// in a given direction
		UpdateScrollArrowState ();

	} else {
													// Locate the button that was pressed and then
													// do the right thing about turning off the
													// current button and then broadcasting the
													// index of the newly selected page
		LPageButton* pageButton = (LPageButton*)FindPaneByID ( inMessage );
		LPageButton* currButton = GetSelectedPageButton ();
		if ( value == Button_On && currButton != pageButton ) {
													// Make sure that the currently selected button
													// is turned off
			if ( currButton ) {
				TogglePageButtonState ( currButton, false );
			}

													// Setup our reference to the new page button,
													// we don't need to select the button as this
													// was done by the user clicking on it so we
													// just setup the reference
			SetSelectedPageButton ( pageButton, false );

													// Make sure that the state of the arrows is
													// set, which basically disables an arrow if
													// there is something available to be scrolled
													// in a given direction
			UpdateScrollArrowState ();

													// Finally, figure out the index of the new page
													// and broadcast that along with the controller
													// message
			SInt32 currPageIndex = GetCurrentPageIndex ();
			if ( currPageIndex > 0 ) {
				BroadcastMessage ( GetControllerMessage (), &currPageIndex );
			}
		}
	}
}


#pragma mark -
#pragma mark === RESIZING

// ---------------------------------------------------------------------------
//	¥ ResizeFrameBy													  [public]
// ---------------------------------------------------------------------------

void
LPageController::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
													// Call our superclass to handle the basic action
	LView::ResizeFrameBy ( inWidthDelta, inHeightDelta, inRefresh );

													// Make sure we synch the scrollarrows in case
													// we need to either show or hide them depending
													// on whether our superview got wider or narrower
	SynchScrollArrows();

}


#pragma mark -
#pragma mark === SEARCHING

// ---------------------------------------------------------------------------
//	¥ FindPageButtonByTitle											  [public]
// ---------------------------------------------------------------------------

LPageButton*
LPageController::FindPageButtonByTitle(
	ConstStringPtr	inPageTitle)
{
	LPageButton*	foundButton = nil;

									// Iterate over the list of page buttons looking
									// for a match with the page title passed in to
									// us
	LArrayIterator	iterator( GetPageButtons() );
	LPageButton* 	theButton;
	while (iterator.Next(&theButton)) {
									// Check the button's name against the page
									// title and if there is a match then return
									// the button
		LStr255 buttonName;
		theButton->GetDescriptor(buttonName);

		if (buttonName == inPageTitle) {
			foundButton = theButton;
			break;
		}
	}

	return foundButton;
}


// ---------------------------------------------------------------------------
//	¥ FindPageButtonByIndex											  [public]
// ---------------------------------------------------------------------------
//	Return PageButton object at the specified index (left to right ordering)
//
//	Returns nil if index is out of range

LPageButton*
LPageController::FindPageButtonByIndex(
	ArrayIndexT		inIndex)
{
	LPageButton*	button = nil;
	GetPageButtons().FetchItemAt(inIndex, &button);
	return button;
}


// ---------------------------------------------------------------------------
//	¥ FindLeftMostPageButton										  [public]
// ---------------------------------------------------------------------------

LPageButton*
LPageController::FindLeftMostPageButton()
{
	LPageButton*	leftMostButton = nil;

									// Get the frame for the page buttons view
	Rect	buttonViewFrame;
	mPageButtons->CalcLocalFrameRect(buttonViewFrame);
	SInt16	leftEdge = buttonViewFrame.left;

									// Now we need to iterate over the buttons
									// finding the one that is at the rightmost edge
									// of the button view
	LArrayIterator	iterator( GetPageButtons() );
	LPageButton*	theButton;
	while (iterator.Next(&theButton))  {
		Rect	buttonFrame;
		theButton->CalcLocalFrameRect(buttonFrame);

		if ( (buttonFrame.left <= leftEdge + 1)  &&
			 (buttonFrame.right > leftEdge) ) {

			leftMostButton = theButton;
			break;
		}
	}

	return leftMostButton;
}


// ---------------------------------------------------------------------------
//	¥ FindRightMostPageButton										  [public]
// ---------------------------------------------------------------------------

LPageButton*
LPageController::FindRightMostPageButton()
{
	LPageButton*	rightMostButton = nil;
									// Get the frame for the page buttons view
	Rect	buttonViewFrame;
	mPageButtons->CalcLocalFrameRect(buttonViewFrame);
	SInt16	rightEdge = buttonViewFrame.right;

									// Now we need to iterate over the buttons
									// finding the one that is at the rightmost edge
									// of the button view
	LArrayIterator	iterator( GetPageButtons() );
	LPageButton*	theButton;
	while (iterator.Next(&theButton)) {
		Rect	buttonFrame;
		theButton->CalcLocalFrameRect(buttonFrame);

		if ( (buttonFrame.left < rightEdge)  &&
			 (buttonFrame.right >= rightEdge) ) {

			rightMostButton = theButton;
			break;
		}
	}

	return rightMostButton;
}


#pragma mark -
#pragma mark === TITLE LOADING

// ---------------------------------------------------------------------------
//	¥ LoadPageTitles												  [public]
// ---------------------------------------------------------------------------

void
LPageController::LoadPageTitles(
	ResIDT		inStringListID,
	SInt16		inSelectionIndex)
{
	StResource	stringListH(FOUR_CHAR_CODE('STR#'), inStringListID, false, false);

	if ((stringListH.mResourceH != nil)) {
		SInt16		titleCount = * (SInt16*) *stringListH.mResourceH;

		if ( titleCount > 0 ) {
			for ( SInt16 index = 1; index <= titleCount; index++ ) {
														// Get the string from the list
				Str255	theTitle;
				::GetIndString ( theTitle, inStringListID, index );

														// Now we need to get a button built for the
														// new title
				AddPageButton ( theTitle, false );
			}

														// Once all the titles have been loaded set the
														// initially selected page button
			if ( inSelectionIndex > 0 ) {
				SetSelectedPageButtonByIndex	( inSelectionIndex );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ LoadPageTitlesFromArray										  [public]
// ---------------------------------------------------------------------------
//
//	This function is designed to be called from somewehere after the controller has been
//	built with no buttons, buttons will then be created for each title that is in the
//	array that has been passed in.

void
LPageController::LoadPageTitlesFromArray(
	LVariableArray*		inStringArray,
	SInt16				inSelectionIndex)
{
	LArrayIterator iterator (*inStringArray);
	Str255	theTitle;
	while (iterator.Next(theTitle)) {
													// Now we need to get a button built for the
													// new title
		AddPageButton (theTitle, false);
	}
													// Once all the titles have been loaded set the
													// initially selected page button
	SetSelectedPageButtonByIndex(inSelectionIndex);
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LPageController::DrawSelf()
{
	StColorPenState::Normalize();
	
	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);

	if (bitDepth < 4) {							// BLACK & WHITE
		DrawBWController();

	} else {									// COLOR
		DrawColorController();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawBWController											   [protected]
// ---------------------------------------------------------------------------

void
LPageController::DrawBWController ()
{
	Rect visRect;
	CalcControllerVisibleRect ( visRect );

	StColorState::Normalize();

										// Erase the visible rect
	::EraseRect ( &visRect );
										// Frame the visible portion of the
										//   controller
	visRect.bottom -= pageController_BottomOffset;
	::MacFrameRect ( &visRect );
	visRect.bottom += pageController_BottomOffset;

										// Now draw the shadow along the
										//   bottom of the button
	::MoveTo	( visRect.left,  (SInt16) (visRect.bottom - 12) );
	::MacLineTo ( visRect.right, (SInt16) (visRect.bottom - 12) );
	::MoveTo	( visRect.left,  (SInt16) (visRect.bottom - 11) );
	::MacLineTo ( visRect.right, (SInt16) (visRect.bottom - 11) );

}


// ---------------------------------------------------------------------------
//	¥ DrawColorController										   [protected]
// ---------------------------------------------------------------------------

void
LPageController::DrawColorController()
{
	Rect visRect;
	CalcControllerVisibleRect ( visRect );

													// Only have to worry about drawing something
													// if we get back a rect that is not empty
	if ( !::EmptyRect ( &visRect )) {
		RGBColor	tempColor;
		SInt8		colorIndex;
		bool		enabledAndActive = IsEnabled() && IsActive();

													// First make sure the face of the control is
													// drawn
		tempColor = enabledAndActive ? mFaceColor : UGraphicUtils::Lighten ( mFaceColor );
		::RGBForeColor ( &tempColor );

		visRect.bottom -= pageController_BottomOffset;
		::MacInsetRect ( &visRect, 1, 1 );
		::PaintRect ( &visRect );
		::MacInsetRect ( &visRect, -1, -1 );
		visRect.bottom += pageController_BottomOffset;

													// Background
		if (mBackColor == Color_White) {			// Interpret white to mean transparent
			mSuperView->ApplyForeAndBackColors();

		} else {
			Pattern	whitePat;
			::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
			::RGBBackColor(&mBackColor);
		}

		SInt16	oldTop = visRect.top;
		visRect.top += (SInt16) (UGraphicUtils::RectHeight ( visRect ) - pageController_BottomOffset);
		::EraseRect ( &visRect );
		visRect.top = oldTop;

													// Draw a black line along the left, top and
													// right edges
		colorIndex = colorRamp_Gray7;
		if (enabledAndActive) {
			colorIndex = colorRamp_Black;
		}
		UGAColorRamp::GetColor(colorIndex, tempColor);
		::RGBForeColor ( &tempColor );

		UGraphicUtils::TopLeftSide ( visRect, 0, 0, pageController_BottomOffset, 0 );
		UGraphicUtils::BottomRightSide ( visRect, 1, 1, pageController_BottomOffset, 0 );

													// Render the left top edge
		if (enabledAndActive) {
			tempColor = UGraphicUtils::Lighten ( mFaceColor );
			::RGBForeColor ( &tempColor );
			UGraphicUtils::TopLeftSide ( visRect, 1, 1, pageController_BottomOffset + 2, 2 );

		   										// Render the bottom right edge
			tempColor = UGraphicUtils::Darken ( mFaceColor );
			::RGBForeColor ( &tempColor );
			UGraphicUtils::BottomRightSide ( visRect, 2, 2, 13, 1 );
		}
													// Now draw the shadow along the bottom of the
													// button
		colorIndex = colorRamp_Gray5;
		if (enabledAndActive) {
			colorIndex = colorRamp_Gray8;
		}
		UGAColorRamp::GetColor(colorIndex, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( visRect.left, (SInt16) (visRect.bottom - 12) );
		::MacLineTo ( visRect.right, (SInt16) (visRect.bottom - 12) );

		colorIndex = colorRamp_Gray4;
		if (enabledAndActive) {
			colorIndex = colorRamp_Gray6;
		}
		UGAColorRamp::GetColor(colorIndex, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( visRect.left, (SInt16) (visRect.bottom - 11) );
		::MacLineTo ( visRect.right, (SInt16) (visRect.bottom - 11) );

		colorIndex = colorRamp_Gray3;
		if (enabledAndActive) {
			colorIndex = colorRamp_Gray4;
		}
		UGAColorRamp::GetColor(colorIndex, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( visRect.left, (SInt16) (visRect.bottom - 10) );
		::MacLineTo ( visRect.right, (SInt16) (visRect.bottom - 10) );
	}
}


#pragma mark -
#pragma mark === SCROLLING

// ---------------------------------------------------------------------------
//	¥ ScrollPageButtons											   [protected]
// ---------------------------------------------------------------------------

void
LPageController::ScrollPageButtons(
	EScrollDirection 	inScrollDirection,
	SInt16				inScrollAmount,
	Boolean				inRefresh)
{
													// Based on the scroll direction we iterate over
													// all of the buttons adjusting their locations
													// horizontally by the scroll amount in the
													// appropriate direction
	LArrayIterator iterator ( GetPageButtons() );
	LPageButton* theButton;
	while ( iterator.Next ( &theButton )) {

		switch ( inScrollDirection ) {

			case scrollDirection_Left: {

				theButton->MoveBy ( -inScrollAmount, 0, false );
				if ( inRefresh ) {
					theButton->Draw ( nil );
				}
			}
			break;

			case scrollDirection_Right: {

				theButton->MoveBy ( inScrollAmount, 0, false );
				if ( inRefresh ) {
					theButton->Draw ( nil );
				}
			}
			break;
			
			default:
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetupScrollArrows											   [protected]
// ---------------------------------------------------------------------------

void
LPageController::SetupScrollArrows (
	Boolean inShowArrows )
{
	Boolean adjustButtonView = false;

	if ( inShowArrows && !HaveScrolling ()) {
													// If we don't yet have the arrows built we
													// need to get them created
		if ( !HaveScrollArrows ()) {

			BuildScrollArrows ();
													// We need to make sure that we invalidate
													// the areas occupied by the arrows so that
													// they are redrawn
			InvalidateScrollArrows ();

			adjustButtonView = true;

		} else if ( !HaveScrolling ()) {

			mLeftArrow->Show ();
			mRightArrow->Show ();
													// We need to make sure that we invalidate
													// the areas occupied by the arrows so that
													// they are redrawn
			InvalidateScrollArrows ();

			adjustButtonView = true;
		}
													// Make sure that the state of the arrows is
													// set, which basically enables an arrow if
													// there is something available to be
													// scrolled in a given direction or disables
													// it if there is not
		UpdateScrollArrowState ();

	} else if ( HaveScrollArrows () && HaveScrolling ()) {

													// Hide the arrows
		mLeftArrow->Hide ();
		mRightArrow->Hide ();

													// We need to make sure that we invalidate
													// the areas occupied by the arrows so that they are redrawn
		InvalidateScrollArrows ();

		adjustButtonView = true;
	}

													// Setup our scrolling flag
	mHaveScrolling = inShowArrows;
													// When the arrows are shown we need to do a
													// little adjusting to the view that displays
													// the buttons
	if ( 	adjustButtonView ) {
		AdjustButtonViewForScrolling ( inShowArrows );
	}
}


// ---------------------------------------------------------------------------
//	¥ InvalidateScrollArrows									   [protected]
// ---------------------------------------------------------------------------

void
LPageController::InvalidateScrollArrows ()
{
													// We need to make sure that we invalidate the
													// areas occupied by the arrows
	if ( HaveScrollArrows ()) {
		mLeftArrow->Refresh ();
		mRightArrow->Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ UpdateScrollArrowState									   [protected]
// ---------------------------------------------------------------------------

void
LPageController::UpdateScrollArrowState ()
{
	SInt16 scrollIncrement;
													// If the left scroll arrow is visible then we
													// update it
	if ( mLeftArrow && mLeftArrow->IsVisible ()) {

		scrollIncrement = CalcNextRightScrollIncrement ();
		if ( scrollIncrement == 0 ) {
			mLeftArrow->Disable ();
		} else {
			mLeftArrow->Enable ();
		}
	}
													// If the right scroll arrow is visible then we
													// update it
	if ( mRightArrow && mRightArrow->IsVisible ()) {
		scrollIncrement = CalcNextLeftScrollIncrement ();
		if ( scrollIncrement == 0 ) {
			mRightArrow->Disable ();
		} else {
			mRightArrow->Enable ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustButtonViewForScrolling								   [protected]
// ---------------------------------------------------------------------------

void
LPageController::AdjustButtonViewForScrolling(
	Boolean		inScrolling)
{
	SDimension16	frameSize;
	GetFrameSize(frameSize);

	SInt16		moveAmount = (SInt16) (-(pageController_ArrowControlWidth - 1));
	SInt16		newWidth   = (SInt16) (frameSize.width - 2);

	if (inScrolling) {
		moveAmount = (SInt16) (pageController_ArrowControlWidth - 1);
		newWidth   = (SInt16) (frameSize.width - (pageController_ArrowControlWidth * 2));
	}

									// When the scrolling is shown we need to do a
									// little tweaking to the page button view so
									// that the arrows become visible.
									// Adjust the location first
	if (not mGroupedArrows) {
		mPageButtons->MoveBy(moveAmount, 0, Refresh_No);
	}
									// Now adjust the width
	mPageButtons->ResizeFrameTo(newWidth, frameSize.height, Refresh_No);

}


#pragma mark -
#pragma mark === LAYOUT

// ---------------------------------------------------------------------------
//	¥ BuildPageButtonView										   [protected]
// ---------------------------------------------------------------------------

void
LPageController::BuildPageButtonView ()
{
	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
													// Setup the pane info for the button view
	SPaneInfo paneInfo;

	paneInfo.paneID			 = FOUR_CHAR_CODE('BUTV');
	paneInfo.width			 = (SInt16) (UGraphicUtils::RectWidth ( localFrame ) - 2);
	paneInfo.height			 = UGraphicUtils::RectHeight ( localFrame );
	paneInfo.visible		 = true;
	paneInfo.enabled		 = true;
	paneInfo.bindings.left	 = true;
	paneInfo.bindings.top	 = false;
	paneInfo.bindings.right  = true;
	paneInfo.bindings.bottom = false;
	paneInfo.left			 = localFrame.left + 1;
	paneInfo.top			 = localFrame.top;
	paneInfo.userCon		 = nil;
	paneInfo.superView		 = this;
													// Setup the control info
	SViewInfo	viewInfo;

	viewInfo.imageSize.width   = 0;
	viewInfo.imageSize.height  = 0;
	viewInfo.scrollPos.v	   = 0;
	viewInfo.scrollPos.h	   = 0;
	viewInfo.scrollUnit.h	   = 0;
	viewInfo.scrollUnit.v	   = 0;
	viewInfo.reconcileOverhang = false;
													// Now create the actual view
	LView* buttonView = new LView(paneInfo, viewInfo);

													// Finally, call FinishCreate like a good boy!
	buttonView->FinishCreate();
													// Save off a reference to the button view
	mPageButtons = buttonView;
}


// ---------------------------------------------------------------------------
//	¥ BuildScrollArrows											   [protected]
// ---------------------------------------------------------------------------

void
LPageController::BuildScrollArrows ()
{
	Rect localFrame;
	CalcLocalFrameRect ( localFrame );

													// LEFT ARROW
													// Setup the pane info for the left arrow
	SPaneInfo paneInfo;

	paneInfo.paneID = FOUR_CHAR_CODE('left');
	paneInfo.width = pageController_ArrowControlWidth;
	paneInfo.height = UGraphicUtils::RectHeight ( localFrame );
	paneInfo.visible = true;
	paneInfo.enabled = true;
	paneInfo.bindings.left = not mGroupedArrows;
	paneInfo.bindings.top = true;
	paneInfo.bindings.right = mGroupedArrows;
	paneInfo.bindings.bottom = false;
	paneInfo.left = mGroupedArrows ? localFrame.right -
								((pageController_ArrowControlWidth * 2) - 1) : localFrame.left;
	paneInfo.top = localFrame.top;
	paneInfo.userCon = nil;
	paneInfo.superView = this;
													// Setup the control info
	SControlInfo	controlInfo;

	controlInfo.valueMessage = paneInfo.paneID;
	controlInfo.value = 0;
	controlInfo.minValue = 0;
	controlInfo.maxValue = 1;

													// Now create the actual arrow
	RGBColor		backColor = GetPageButtonBackColor();
	RGBColor		faceColor = GetPageButtonFaceColor();

	LPageArrow* leftArrow = new LPageArrow (paneInfo, controlInfo,
											true, backColor, faceColor);

													// Make sure we call FinishCreate
	leftArrow->FinishCreate ();
													// Make sure teh button is set to broadcast
	leftArrow->StartBroadcasting ();
													// Add the pane to the controller and save off
													// a reference
	mLeftArrow = leftArrow;
													// Add the controller as a listener to the
													// button
	leftArrow->AddListener ( this );

													// RIGHT ARROW
													// Setup the pane info for the right arrow
	paneInfo.paneID = FOUR_CHAR_CODE('rght');
	paneInfo.bindings.top = true;
	paneInfo.bindings.left = false;
	paneInfo.bindings.right = true;
	paneInfo.bindings.bottom = false;
	paneInfo.left = localFrame.right - pageController_ArrowControlWidth;

													// Setup the control info
	controlInfo.valueMessage = paneInfo.paneID;

													// Now create the actual arrow button
	LPageArrow* rightArrow = new LPageArrow(paneInfo, controlInfo,
											false, backColor, faceColor);

													// Make sure we call FinishCreate
	rightArrow->FinishCreate ();
													// Make sure the button is set to broadcast
	rightArrow->StartBroadcasting ();
													// Add the pane to the controller and save off
													// a reference
	mRightArrow = rightArrow;
													// Add the controller as a listener to the
													// button
	rightArrow->AddListener ( this );

}


#pragma mark -
#pragma mark === MISCELLANEOUS

// ---------------------------------------------------------------------------
//	¥ GetPageButtonID											   [protected]
// ---------------------------------------------------------------------------
//	ID is of the form 'pbXX', where XX is a two-digit number

void
LPageController::GetPageButtonID (
	PaneIDT&	outPaneID )
{
	LStr255		idStr( (SInt16) GetNextIndex() );

	idStr.Insert("\ppb", 1);

	outPaneID = (PaneIDT) FourCharCode(idStr);
}


// ---------------------------------------------------------------------------
//	¥ RebuildButtonIdentifiers									   [protected]
// ---------------------------------------------------------------------------
//
//	This function is called when the page index counter overflows, what it does is
// simply rebuild the button IDs starting from 1.  This will get called when we
//	run out of indices at 99, the most common cause will be that the user has added
// and deleted too many buttons and we have simply exhausted the available range of
// indices that can be used to build IDs.

void
LPageController::RebuildButtonIdentifiers()
{
													// We start by first clearing the index counter
	ClearIndexCounter();
													// Iterate over the list of tab buttons changing
													// all of their IDs so that we get the ID range
													// compacted
	LArrayIterator iterator( GetPageButtons() );
	LPageButton* theButton;
	while ( iterator.Next ( &theButton )) {
													// Get a new pane ID
		PaneIDT	newID;
		GetPageButtonID ( newID );
													// Change the button's ID to the new one
		if ( theButton ) {
			theButton->SetPaneID ( newID );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcNextLeftScrollIncrement								   [protected]
// ---------------------------------------------------------------------------
//
//	This method is going to do its best to figure out a scroll increment that makes the
// most sense given the position of the rightmost visible button.  If the button is only
// partially visible then the appropriate delta will be returned.  If the button is
// flush with the right edge then the button after it is used to determine the
// scroll increment.  If the button is the last one then the difference between its
// right edge and the right edge of the button view is used for the scroll increment

SInt16
LPageController::CalcNextLeftScrollIncrement ()
{

	LArray&	buttonList = GetPageButtons ();
	if ( buttonList.GetCount () > 0 ) {

		LPageButton*	rightButton = FindRightMostPageButton ();
		if ( rightButton ) {
			Rect	buttonViewFrame;
			mPageButtons->CalcLocalFrameRect ( buttonViewFrame );
			SInt16	rightEdge = buttonViewFrame.right;

													// We start by finding the button that is
													// currently at the left edge of the control and
													// we get its width, this will be the initial
													// scroll increment
			SInt16	index = (SInt16) buttonList.FetchIndexOf ( &rightButton );
			Rect		rightButtonFrame;
			rightButton->CalcLocalFrameRect ( rightButtonFrame );
			SInt16 scrollIncrement = UGraphicUtils::RectWidth ( rightButtonFrame );

													// Now just in case the button is only partially
													// visible we need figure out the appropriate
													// delta
			if ( rightButtonFrame.right > rightEdge ) {
				scrollIncrement = GAUtils_Absolute((SInt16) (rightButtonFrame.right - rightEdge));

			} else {
													// We are obviously at the right edge of the
													// button view in which case if there is another
													// button to the right we would like to use it
													// as the scroll increment
				if ( index < buttonList.GetCount ()) {
					buttonList.FetchItemAt ( index + 1, &rightButton );
					if ( rightButton ) {
						rightButton->CalcLocalFrameRect ( rightButtonFrame );
						scrollIncrement = UGraphicUtils::RectWidth ( rightButtonFrame );
					}
				}
			}
													// Now we need to check to see if scrolling the
													// buttons by this increment will result in the
													// last button being scrolled beyond the end of
													// the button view, if it will we then need to
													// adjust the scroll increment so that this
													// does not happen
			LPageButton*	lastButton;
			if ( buttonList.GetCount () > 0 ) {
				Rect buttonFrame;
				buttonList.FetchItemAt ((ArrayIndexT) buttonList.GetCount (), &lastButton );
				ThrowIfNil_ ( lastButton );
				lastButton->CalcLocalFrameRect ( buttonFrame );
				if ( buttonFrame.right - scrollIncrement <= rightEdge ) {
					scrollIncrement = GAUtils_Absolute((SInt16) (buttonFrame.right - rightEdge));
				}
			}
													// Return the scroll increment
			return scrollIncrement;
		}
	}
													// No button was found
	return 0;

}


// ---------------------------------------------------------------------------
//	¥ CalcNextRightScrollIncrement
// ---------------------------------------------------------------------------
//
//	This method is going to do its best to figure out a scroll increment that makes the
// most sense given the position of the leftmost visible button.  If the button is only
// partially visible then the appropriate delta will be returned.  If the button is
// flush with the left edge then the button before it is used to determine the scroll
// scroll increment.  If the button is the first one then the difference between its
// left edge and the left edge of the button view is used for the scroll increment

SInt16
LPageController::CalcNextRightScrollIncrement ()
{
	LArray&	buttonList = GetPageButtons ();
	if ( buttonList.GetCount () > 0 )
	{
		LPageButton*	leftButton = FindLeftMostPageButton ();
		if ( leftButton )
		{
			Rect	buttonViewFrame;
			mPageButtons->CalcLocalFrameRect ( buttonViewFrame );
			SInt16	leftEdge = buttonViewFrame.left;

													// We start by finding the button that is
													// currently at the left edge of the control
													// and we get its width, this will be the
													// initial scroll increment
			SInt16	index = (SInt16) buttonList.FetchIndexOf ( &leftButton );
			Rect		leftButtonFrame;
			leftButton->CalcLocalFrameRect ( leftButtonFrame );
			SInt16 scrollIncrement = UGraphicUtils::RectWidth ( leftButtonFrame );

													// Now just in case the button is only partially
													// visible we need figure out the appropriate
													// delta
			if ( leftButtonFrame.left < leftEdge ) {
				scrollIncrement = GAUtils_Absolute ( leftButtonFrame.left );

			} else {
													// We are obviously at the left edge of the
													// button view in which case if there is
													// another button to the left we would like to
													// use it as the scroll increment instead
				if ( index > 1 ) {
					buttonList.FetchItemAt ( index - 1, &leftButton );
					if ( leftButton ) {
						leftButton->CalcLocalFrameRect ( leftButtonFrame );
						scrollIncrement = UGraphicUtils::RectWidth ( leftButtonFrame );
					}
				}
			}
													// Now we need to check to see if scrolling the
													// buttons by this increment will result in the
													// last button being scrolled beyond the end of
													// the button view, if it will we then need to
													// adjust the scroll increment so that this
													// does not happen
			LPageButton*	firstButton;
			if ( buttonList.GetCount () > 0 ) {
				Rect buttonFrame;
				buttonList.FetchItemAt ( 1, &firstButton );
				ThrowIfNil_ ( firstButton );
				firstButton->CalcLocalFrameRect ( buttonFrame );
				if ( buttonFrame.left + scrollIncrement > leftEdge ) {
					scrollIncrement = GAUtils_Absolute((SInt16)  (buttonFrame.left - leftEdge));
				}
			}
													// Return the scroll increment
			return scrollIncrement;
		}
	}
													// No button was found
	return 0;
}


PP_End_Namespace_PowerPlant
