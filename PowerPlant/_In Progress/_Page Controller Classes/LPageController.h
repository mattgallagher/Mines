// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageController.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

/*
THEORY OF OPERATION

A PageController lets the user select pages from a set of horizontal page
titles displayed in buttons that can be scrolled horizontally if needed.

When on of the buttons is selected the button slides down below the bottom of the
controller to indicate that it is the current selection and its associated title is
hilited.

When the controller is large enough to accomodate all of the buttons no scrolling is
available, but once the controller becomes smaller than the set of buttons a pair of
scroll arrows are displayed either grouped at the right edge of the controller or with
an arrow on each end of the controller.  This option is controlled by the mGroupedArrows
flag which can be set from within Constructor.

If a button is partially visible and the user selects it the button is automatically
adjusted such that it is fully visible within the controller.  Likewise, whenever the
size of the controller is adjusted it automatically does the right thing about making
sure that the buttons are correctly maintained such that the current selection is
always visible and that no trailing space is visible on the right if the buttons had
been scrolled to the left.

A controller can be created either procedurally or using Constructor.  For the
procedural approach a list of title has to be provided, which are then used to build
the controller.  For a controller created in Constructor a 'STR#' resource needs to
be provided, the resource ID is then entered in the custom pane type along with the
index for the intially selected item.  The controller will then construct the needed
buttons and store the title.

When one of the page buttons is selected the controller broadcasts the index of the
selected page button, anyone listening can then take the appropriate action
such as displaying the associated page.

The page controller allows a number of different colors to be setup from the CTYP in
Constructor [or procedurally].  These colors are the background color which is the
color that is drawn in the area BELOW the actual controller.  Face color which is the
color that is used to render the face of the controller, as well as any buttons that
get created by the controller. The text pushed color, which is the color the title of
a page button will be rendered in when the button is selected.

When the controller is disabled the face color is lightened to arrive at a dimmed
color.  The face color is also used to calculate colors for the light and shadow
edges of the  button.

The CTYP provided for the page controller has been setup with default colors that we
consider to be the standard.  If you choose to use different colors don't blame us
if it doesn't look right.


LIMITATIONS:

Currently the only major limitations are as follows;

	1É	There can only be 99 page buttons as the controller automatically assigns
		pane IDs to the buttons by concatenating 'pb' and the page title index. However
		there is a mechanism for compacting the IDs should they run out, which can
		occur if a lot of insertion and removals of pages are done.  This isn't a good
		thing to be doing anyway but if it is then this prevents an out of range problem
		from occuring.
	2É	It is possible to scroll the currently selected button so that it is no longer
		visible within the scoller.  The only clue that this has happened will be the
		fact that its associated page has not changed.  Selecting any one of the other
		buttons will turn off the current selection and the new selection will now be
		visible.
	3É	If the controller is drawn in a view that has some form of edge shadowing those
		shadows will be covered where the controller renders as it completely covers
		the background in the area it occupies.

CAVEATS

	1É When placing the page controller be aware that it draws a black line around the
		left, top, right edges.  This means that if you want to place it at the edges
		of a window and don't want to have a double black line on these edges you need
		to position it so that it has these edges outside its superview's frame.  Look
		at the example PPob to see how this is done.

ASSOCIATED CONTROLS:

There are three other controls that are associated with and used by the controller;

	LPageButton			-	this is the control that is used for the page buttons
	LLeftPageArrow		-	the left arrow displayed by the controller when scrolling
	LRightPageArrow		-	the right arrow displayed by the controller when scrolling

NOTE:

The page buttons are responsible for drawing the button in both states as expected,
but be aware that the buttons draw their basic shape as well as the background and
shadow in the area below the button in its normal state.  This means that when the
button is selected it is merely drawing differently within itself, it does not actaully
render over some other part of the controller. The controller draws the same normal
appearance as the page buttons so that it can effectively serve as filler for any
parts that are not occupied by buttons.

Likewise, the arrow buttons draw the same normal appearance as the buttons or the
controller, with the addition of displaying an arrow.  These are created procedurally
by the controller whenever they are needed.

What this all means is that if you want to alter the rendering of the controller it
needs to be correctly factored in each of the three drawing functions.

It also means that when the mouse is tracked it currently is tracked in what appears
to be the background of the control where it is in fact still in the button.  This
is also true for the arrows but I have an option for the arrows that will allow
tracking to only occur within the mask of the arrow.

MORE DETAILS:

The controller consists of a view that contains another view into which the page
buttons are installed.  Whenever scrolling is enabled or disabled the size and
location of this inner button view is adjusted to make space for the left and right
scroll arrows.  Whenever any of the page buttons are scrolled the scrolling is done
entirely within this button view.

The first time scrolling is required the controller will build the left and right
scroll arrows and make them visible, anytime after that should the scrolling state
change the arrows are hidden or shown as appropriate.

CAVEAT:

The controller has been visually optimized to render at a height of >> 33 << pixels,
it should still look fine at heights greater than that but it might look wierd at
lesser heights.  The main thing that will appear odd is that the arrow icons might be
to high within their boxes if the controller is made very tall.  This can be tweaked
within the code for that controller by altering the "pageController_IconOffset"
constant.

*/


#ifndef _H_LPageController
#define _H_LPageController
#pragma once

#include <LPageButton.h>

#include <LView.h>
#include <LListener.h>
#include <LBroadcaster.h>
#include <LVariableArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

typedef		SInt32			PageIDT;

class LPageArrow;

									// Constants for the scrolling directions within
									// the page controller
enum EScrollDirection {
	scrollDirection_None,
	scrollDirection_Left,
	scrollDirection_Right
};


// ===========================================================================
//	CLASS: LPageController
// ===========================================================================

class LPageController : public LView,
						public LListener,
						public LBroadcaster {
public:
	enum { class_ID = FOUR_CHAR_CODE('pctl') };

						LPageController();

						LPageController(
								const LPageController&	inOriginal);

						LPageController(
								LStream* 			inStream);

						LPageController(
								const SPaneInfo& 	inPaneInfo,
								const SViewInfo& 	inViewInfo,
								ResIDT				inTextTraitsID,
								LVariableArray*		inPageTitles,
								ArrayIndexT			inSelectionIndex,
								MessageT			inMessage,
								const RGBColor&		inBackColor = pageButton_DefaultBackColor,
								const RGBColor&		inFaceColor = pageButton_DefaultFaceColor,
								const RGBColor&		inPushedTextColor = pageButton_TitleHiliteColor,
								bool				inGroupArrows = true );

						LPageController(
								const SPaneInfo& 	inPaneInfo,
								const SViewInfo& 	inViewInfo,
								ResIDT				inTextTraitsID,
								ResIDT				inStringListID,
								SInt16				inSelectionIndex,
								MessageT			inMessage,
								const RGBColor&		inBackColor = pageButton_DefaultBackColor,
								const RGBColor&		inFaceColor = pageButton_DefaultFaceColor,
								const RGBColor&		inPushedTextColor = pageButton_TitleHiliteColor,
								bool				inGroupArrows = true );

	virtual				~LPageController();

	virtual	void		FinishCreateSelf();


							// Getters

	ResIDT				GetTextTraitsID() const		{ return mTextTraitsID; }

	LArray&				GetPageButtons()			{ return mPageButtons->GetSubPanes (); }

	ArrayIndexT			GetNextIndex()				{ return ++mNextIndex; }

	ArrayIndexT			GetCurrentIndex() const		{ return mNextIndex; }

	virtual	ArrayIndexT	GetCurrentPageIndex() const;

	virtual ArrayIndexT	GetPageIndex( ConstStringPtr inPageTitle );

	LPageButton*		GetSelectedPageButton() const	{ return mSelectedButton; }

	MessageT			GetControllerMessage() const	{ return mControllerMessage; }

	bool				HaveScrollArrows() const		{ return mLeftArrow && mRightArrow; }

	virtual	Boolean		NeedsScrolling();

	bool				HaveScrolling() const			{ return mHaveScrolling; }

	virtual	SInt16		CalcPageButtonWidth( ConstStringPtr inPageTitle );

	virtual	void		CalcControllerVisibleRect( Rect& outRect );

	LView*				GetPageButtonView() const		{ return mPageButtons; }

	virtual SInt32		GetValue() const;

						// Setters

	virtual	void		SetTextTraitsID( ResIDT inTextTraitsID );

	virtual	void		SetSelectedPageButton(
								LPageButton*		inSelectedButton,
								Boolean				inSelectButton = true,
								Boolean				inSuppressBroadcast = true);

	virtual	void		SetSelectedPageButtonByIndex(
								ArrayIndexT			inSelectionIndex,
								Boolean				inSuppressBroadcast = true);

	virtual void		SetValue( SInt32 inValue );

	virtual	void		TogglePageButtonState(
								LPageButton*		inSelectedButton,
								Boolean				inSelected = true,
								Boolean				inSuppressBroadcast = true);

	virtual	void		ClearIndexCounter()			{ mNextIndex = 0; }


	virtual	void		EnableSelf();
	virtual	void		DisableSelf();


	virtual	void		ActivateSelf();
	virtual	void		DeactivateSelf();


						// Page Button Management

	virtual	void		AddPageButton(
								ConstStringPtr  	inPageTitle,
								Boolean  			inSelected);

	virtual	void		InsertPageButtonAt(
								ConstStringPtr  	inPageTitle,
								ArrayIndexT 		inAtIndex);

	virtual	void		RemovePageButtonAt( ArrayIndexT inAtIndex );

	virtual	LPageButton*	CreatePageButton(
								ConstStringPtr		inButtonTitle,
								PaneIDT				inButtonID,
								SDimension16		inButtonSize,
								SPoint32			inButtonPosition,
								Boolean				inButtonPushed);

	virtual	SInt16		GetPageButtonCount() const;
	virtual	void		DeleteAllPageButtons();
	virtual	SInt16		GetLastPageButtonLocation();
	virtual	Boolean		RevealSelectedPageButton();
	virtual	void		RevealLeftMostPageButton();
	virtual	void		AlignRightMostPageButton();
	virtual	void		SynchScrollArrows();

												// ¥ Override these methods if you want to create
												// buttons with different face colors, the face of
												// the controller itself is always drawn in gray9.
												// The title and ID of the button is passed in so
												// that this info could be used to determine the
												// face color

	virtual	RGBColor	GetPageButtonBackColor(
								ConstStringPtr		inButtonTitle = Str_Empty,
								PaneIDT				inButtonID = -1) const;

	virtual	RGBColor	GetPageButtonFaceColor(
								ConstStringPtr		inButtonTitle = Str_Empty,
								PaneIDT				inButtonID = -1) const;

	virtual	RGBColor	GetPageButtonPushedTextColor(
								ConstStringPtr		inButtonTitle = Str_Empty,
								PaneIDT				inButtonID = -1) const;

	virtual	void		SetPageControllerBackColor(
								const RGBColor&		inBackColor);

	virtual	void		SetPageControllerFaceColor(
								const RGBColor&		inFaceColor);

	virtual	void		SetPageControllerPushedTextColor(
								const RGBColor&		inTextColor);


						// Listening

	virtual	void		ListenToMessage(
								MessageT			inMessage,
								void*				ioParam);


						// Resizing

	virtual	void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

						// Searching

	virtual	LPageButton*	FindPageButtonByTitle( ConstStringPtr inPageTitle );

	virtual LPageButton*	FindPageButtonByIndex( ArrayIndexT inIndex );

	virtual	LPageButton*	FindLeftMostPageButton();
	virtual	LPageButton*	FindRightMostPageButton();


						// Title Loading

	virtual	void		LoadPageTitles(
								ResIDT				inStringListID,
								SInt16				inSelectionIndex);

	virtual	void		LoadPageTitlesFromArray(
								LVariableArray*		inStringArray,
								SInt16				inSelectionIndex);


						// Drawing

	virtual	void		DrawSelf();


protected:

	MessageT		mControllerMessage;		//	Message to be sent when the controller has a
											//		page change selected
	LPageButton*	mSelectedButton;		// Currently selected page button
	ResIDT			mTextTraitsID;			//	ID of the controller's text traits
	RGBColor		mBackColor;				//	Background color for the control, this is
											//		the background as used by the controller
	RGBColor		mFaceColor;				//	Face color for the controller, this is used
											//		by the controller when rendering its face
	RGBColor		mPushedTextColor;		//	This is the color that will be used to draw
											//		the button's title when it is pushed
	bool			mGroupedArrows;			//	This flag is used to tell if the arrows are
											//		to be built as a group at the right edge
											//		of the controller

						// Drawing

	virtual	void		DrawBWController();
	virtual	void		DrawColorController();


						// Scrolling

	virtual	void		ScrollPageButtons(
								EScrollDirection 	inScrollDirection,
								SInt16				inScrollAmount,
								Boolean				inRefresh);

	virtual	void		SetupScrollArrows( Boolean inShowArrows );

	virtual	void		InvalidateScrollArrows();

	virtual	void		UpdateScrollArrowState();

	virtual	void		AdjustButtonViewForScrolling( Boolean inScrolling );


						// Layout

	virtual	void		BuildPageButtonView();
	virtual	void		BuildScrollArrows();


						// Miscellaneous

	virtual	void		GetPageButtonID( PaneIDT& outPaneID );

	virtual	void		RebuildButtonIdentifiers();

	virtual	SInt16		CalcNextLeftScrollIncrement();

	virtual	SInt16		CalcNextRightScrollIncrement();


private:

	LView*				mPageButtons;			//	View that contains the page buttons, we
												//		keep a reference to the view so that
												//		we can easily access the buttons
	LPageArrow*			mLeftArrow;				//	A reference to the arrow that scrolls
												//		pages to the left
	LPageArrow*			mRightArrow;			//	A reference to the arrow that scrolls
												//		pages to the right
	bool				mHaveScrolling;			//	Flag used to tell if scrolling is enabled
	ResIDT				mStringListID;			//	ID for the string list containing the page
												//		titles
	SInt16				mSelectionIndex;		//	Index for the initially selected button
	LVariableArray*		mInitialPageTitles;		//	A potential list of page title that are
												//		used to create the buttons in the page
												//		controller, this is nil unless set when
												//		a controller is created procedurally
	ArrayIndexT			mNextIndex;				//	This is used to keep a running count for
												//		assigned indices, it is only used to build
												//		IDs for buttons.  Should the button count
												//		overflow this is cleared and the button
												// 		indices are compacted
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
