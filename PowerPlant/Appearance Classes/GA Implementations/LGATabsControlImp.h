// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATabsControlImp.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGATabsControlImp
#define _H_LGATabsControlImp
#pragma once

#include <LGAControlImp.h>
#include <LListener.h>
#include <LGATabsButton.h>
#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LVariableArray;

// ---------------------------------------------------------------------------

class LGATabsControlImp : public LGAControlImp,
						  public LListener {
public:
						LGATabsControlImp(
								LControlPane*	inControlPane);

						LGATabsControlImp(
								LStream* 		inStream);

	virtual				~LGATabsControlImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual ResIDT		GetTextTraitsID() const
							{
								return mTextTraitsID;
							}

	virtual	LArray&		GetTabButtons() const
							{
								return mTabButtons->GetSubPanes();
							}

	virtual	ArrayIndexT	GetNextIndex()
							{
								return ++mNextIndex;
							}

	virtual	ArrayIndexT	GetCurrentIndex()
							{
								return mNextIndex;
							}

	virtual	ArrayIndexT	GetCurrentTabIndex();

	virtual	ArrayIndexT	GetTabIndex(
								ConstStringPtr inTabTitle );

	virtual	LGATabsButton*		GetTabButtonByIndex (
												ArrayIndexT	inIndex ) const;

	virtual	LGATabsButton*		GetSelectedTabButton	() const
										{
											return mSelectedButton;
										}

	virtual	SInt16		CalcTabButtonWidth(
								ConstStr255Param inTabTitle,
								Boolean	inHasIcon = false );

	virtual	void		CalcLocalTabPanelRect(
								Rect	&outRect ) const;

	virtual	void		CalcTabButtonClipping(
												Rect	&outRect );

	virtual	LView*		GetTabButtonView()
							{
								return mTabButtons;
							}

	bool				WantsLargeTab() const
							{
								return mWantsLargeTab;
							}

	virtual SInt16		FindHotSpot(
								Point			inPoint) const;

							// Setters

	virtual	void					SetTextTraitsID (
												ResIDT 			inTextTraitsID );

	virtual	void					SetValue	(
												SInt32		inValue );
	virtual	void					PostSetValue ();

	virtual	void					SetSelectedTabButton	(
												LGATabsButton*	inSelectedButton,
												Boolean			inSelectButton = true,
												Boolean			inSuppressBroadcast = true );
	virtual	void					SetSelectedTabButtonByIndex	(
												ArrayIndexT	inSelectionIndex,
												Boolean		inSuppressBroadcast = true );
	virtual	void					ToggleTabButtonState	(
												LGATabsButton*	inSelectedButton,
												Boolean			inSelected = true,
												Boolean			inSuppressBroadcast = true );
	virtual	void					ClearIndexCounter	()
										{
											mNextIndex = 0;
										}


							// Colors

	virtual 	bool					GetForeAndBackColors (
												RGBColor		*outForeColor,
												RGBColor		*outBackColor ) const;


							// Tab Button Management

	virtual	void					AddTabButton (
												ConstStringPtr	inTabTitle,
												ResIDT	inIconSuiteID,
												Boolean  inSelected	);
	virtual	void					InsertTabButtonAt (
												ConstStringPtr  		inTabTitle,
												ResIDT		inIconSuiteID,
												ArrayIndexT inAtIndex );
	virtual	void					RemoveTabButtonAt (
												ArrayIndexT	inAtIndex );
	virtual	LGATabsButton*		CreateTabButton  (
												ConstStringPtr		inButtonTitle,
												ResIDT				inIconSuiteID,
												PaneIDT				inButtonID,
												SDimension16		inButtonSize,
												SPoint32			inButtonPosition,
												Boolean				inButtonPushed );

	virtual	SInt16					GetTabButtonCount	();
	virtual	SInt16					GetLastTabButtonLocation ();

	virtual	void					DeleteAllTabButtons ();
	virtual	void					LoadTabTitlesFromTabList (
												ResIDT	inTabListID,
												SInt16		inSelectionIndex );


							// Listening

	virtual	void					ListenToMessage (
												MessageT inMessage,
												void*		ioParam	);


							// Data Handling

	virtual	void					SetDataTag (
												SInt16			inPartCode,
												FourCharCode	inTag,
												Size			inDataSize,
												void*			inDataPtr );

	virtual	void					GetDataTag 	(
												SInt16			inPartCode,
												FourCharCode	inTag,
												Size			inBufferSize,
												void*			inBuffer,
												Size			*outDataSize = nil ) const;

							// Drawing

	virtual	void					DrawSelf	();



protected:

	bool				mWantsLargeTab;		//	True if we want a large tab, false for a
											//		small tab
	LGATabsButton*		mSelectedButton;	// Currently selected tab button
	ResIDT				mTextTraitsID;		//	ID of the panel's text traits

	LView*				mTabButtons;		//	View that contains the tab buttons, we
											//		keep a reference to the view so that
											//		we can easily access the buttons
	ResIDT				mTabListResID;		//	ID for the string list if we are building
											//		the tab panel procedurally from a string
											//		list resource
	SInt16				mSelectionIndex;	//	Index for the initially selected button
	LVariableArray*		mInitialTabTitles;	//	This field is used to temprarily hold the
											//		list of titles for the tab buttons it
											//		is then used from within FinishCreateSelf
											//		to create all of the buttons
	ArrayIndexT			mNextIndex;			//	This is used to keep a running count for
											//		assigned indices, it is only used to build
											//		IDs for buttons.  This means that you have
											//		to be very careful when adding and removing
											//		buttons as this could overflow. Clearing
											//		all buttons causes this to get cleared.


							// Drawing

	virtual	void			DrawColorTabPanel	();


							// Layout

	virtual	void			BuildTabButtonView ();


							// Searching

	virtual	LGATabsButton*		FindTabButtonByTitle	(
												ConstStringPtr	inTabTitle );

	virtual	Boolean			FindTabTitleByIndex	(
												Str255		outTitle,
												ArrayIndexT	inIndex );


							// Miscellaneous

	virtual	ArrayIndexT		GetTabButtonID (
												PaneIDT	&outPaneID );

	virtual	void			RebuildButtonIdentifiers ();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
