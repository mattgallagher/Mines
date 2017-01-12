// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAFocusBorder.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

/*
	THEORY OF OPERATION

This class provides a view that is similar to the list box frame provided by
Mac OS 8 except that this view will allow for the cases where the user wants
to have a notch removed from the bottom portion in order to accomodate the
growbox or because of the presence of a horizontal and vertical scrollbar.

In addition, this class paints the area between it and its subpane which is
identified as the view around which the framing and focus ring are drawn when
its commander is put on duty.

When the focus ring is drawn, it is drawn around the notch if there is one
and the user has specified that it be notched.  All of this is controlled
from the CTYP provided for Constructor.

Using this class in conjunction with the LTabGroup class allows tabbing of
the focus from one focus border to another.
*/

#ifndef _H_LGAFocusBorder
#define _H_LGAFocusBorder
#pragma once

#include <LView.h>
#include <LString.h>
#include <LCommander.h>
#include <UEnvironment.h>

#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================

class LGAFocusBorder : public LView,
					   public LCommander {
public:
	enum { class_ID = FOUR_CHAR_CODE('gfbd') };

						LGAFocusBorder();

//						LGAFocusBorder(
//								const LGAFocusBorder&	inOriginal);

						LGAFocusBorder(
								LStream*	inStream);

	virtual				~LGAFocusBorder();

	virtual	void		FinishCreateSelf();


							//	Getters

	Boolean				WantsFacePainted() const
							{
								return mPaintBorderFace;
							}

	LPane*				GetInsetSubPane() const
							{
								return mInsetSubPane;
							}

	PaneIDT				GetCommanderSubPaneID() const
							{
								return mCommanderSubPaneID;
							}

	Boolean				HasFocus() const
							{
								return (sFocusedBorder == this);
							}

	Boolean				CanFocus() const
							{
								return mCanFocus;
							}

	SInt16				GetNotchWidth() const
							{
								return mNotchWidth;
							}

	Boolean				NotchInsetFrame() const
							{
								return mNotchInsetBorder;
							}

	Boolean				NotchBorderFace() const
							{
								return mNotchBorderFace;
							}

	Boolean				FrameInsetSubPane() const
							{
								return mFrameInsetSubPane;
							}

	virtual	RgnHandle	GetBorderRegion();

	virtual	RgnHandle	GetInsetFrameRegion();

	virtual	RgnHandle	GetFocusRegion();

	virtual	RgnHandle	GetThemeFocusRegion();

	static	LGAFocusBorder*	GetCurrentFocus()
								{
									return sFocusedBorder;
								}


						// Setters

	virtual	void		SetPaintBorderFace(
								Boolean	inPaintFace,
								Boolean	inRefresh = true);

	virtual	void		SetInsetSubPane(
								LPane*	inInsetSubPane);

	virtual	void		SetInsetSubPaneByID(
								PaneIDT	inInsetSubPaneID);

	virtual	void		SetCommanderSubPaneByID(
								PaneIDT	inCommanderSubPaneID);

	inline	void		SetFrameInsetSubPane(
								Boolean	inFrameInsetPane)
							{
								mFrameInsetSubPane = inFrameInsetPane;
							}

	inline	void		SetNotchInsetFrame(
								Boolean	inNotchInsetFrame)
							{
								mNotchInsetBorder = inNotchInsetFrame;
							}

	inline	void		SetNotchBorderFace(
								Boolean	inNotchFace)
							{
								mNotchBorderFace = inNotchFace;
							}

	virtual	void		BecomeFocused(
								Boolean	inBecomeFocused,
								Boolean	inRefresh = true);

	virtual	void		SetCanFocus(
								Boolean	inCanFocus);

	virtual	void		SetNotchWidth(
								SInt16	inNotchWidth,
								Boolean	inRefresh = false);


							// Activation

	virtual	void		ActivateSelf();

	virtual	void		DeactivateSelf();

	virtual	void		Activate();

	virtual	void		Deactivate();


							// Tab Focusing

	virtual	Boolean		ObeyCommand(
								CommandT	inCommand,
								void*		ioParam);

							// Drawing

	virtual	void		RefreshBorder();

	virtual	void		RefreshInsetFrameBorder();

	virtual	void		RefreshFocusBorder();

	virtual	void		RefreshInsetSubPane();

	virtual void		Draw( RgnHandle inSuperDrawRgnH );

	virtual	void		DrawSelf();

protected:

	PaneIDT		mInsetSubPaneID;		// ID of the pane that the border will be framing
										//		if the framing option is turned on
	PaneIDT		mCommanderSubPaneID;	// ID of the pane/commander that the border will be focusing
										//		if the framing option is turned on
	Boolean		mPaintBorderFace;		//	Should the face of the border be painted or not
										//		this flag allows control over the painting
										//		of the face independent of the drawing of the
										//		inset frame for the inset pane
	Boolean		mFrameInsetSubPane;		//	Do we want the specified subpane framed
	Boolean		mNotchInsetBorder;		//	Do we want the lower right corner of the
										//		border to have a notch in it, this is typically
										//		used to draw around the grow box
	Boolean		mNotchBorderFace;		//	Do we want a notch taken out of the lower right
										//		corner of the face of the border in order to
										//		accomodate a growbox
	SInt16		mNotchWidth;			//	Width for the notch if we are drawing that option
										//		by default this is set to 15
	Boolean		mCanFocus;				//	Is the ability to handle the focus turned on

	static		LGAFocusBorder* 	sFocusedBorder;	//	The currently focused border or nil
													//		if there is no current focus

	virtual	void		DrawBorder(
								SInt16		inBitDepth);

	virtual	void		DrawFocusBorder(
								SInt16		inBitDepth);

	virtual void		PutOnDuty(
								LCommander*			inNewTarget);

	virtual void		TakeOffDuty();

	virtual void		SubTargetChanged();


private:
	LPane*		mInsetSubPane;			//	A reference to the pane that we are framing if
										//		that option is turned on
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
