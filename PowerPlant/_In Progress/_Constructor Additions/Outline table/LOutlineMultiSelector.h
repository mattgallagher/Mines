// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineMultiSelector.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOutlineMultiSelector
#define _H_LOutlineMultiSelector
#pragma once

#include <LTableMultiSelector.h>
#include <LMarqueeTask.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LOutlineTable;


// ===========================================================================
//		¥ LOutlineMultiSelector
// ===========================================================================
//	LOutlineMultiSelector adds support for Finder-like marqee selection.
//	It can only be used with LOutlineTable (or descendants).

class LOutlineMultiSelector :	public LTableMultiSelector,
								public LMarqueeReceiver {

public:
						LOutlineMultiSelector(
								LOutlineTable*			inOutlineTable);
	virtual				~LOutlineMultiSelector();

	// drag selection

	virtual Boolean		DragSelect(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);

	// selection drawing (overrides from LMarqueeReceiver)
	virtual void		FocusMarqueeArea();

	virtual const LView*	GetTargetView() const;

protected:
	virtual Boolean		CheckSelectionChanged(
								const LMarqueeTask&		inMarqueeTask,
								const Rect&				inOldMarqueeRect,
								const Rect&				inNewMarqueeRect);
	virtual void		UpdateSelection(
								const LMarqueeTask&		inMarqueeTask,
								const Rect&				inOldMarqueeRect,
								const Rect&				inNewMarqueeRect);
	// selection testing

	virtual void		CheckCells(
								const Rect&				inOldMarqueeRect,
								const Rect&				inNewMarqueeRect);
	virtual void		AdjustSelection(
								const RgnHandle			inRgnHandle,
								Boolean					inAddToSelection);


	LOutlineTable*		mOutlineTable;
	RgnHandle			mAddToSelection;
	RgnHandle			mRemoveFromSelection;
	RgnHandle			mInvertSelection;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
