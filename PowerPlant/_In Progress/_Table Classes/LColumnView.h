// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LColumnView.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LColumnView
#define _H_LColumnView
#pragma once

#include <LTableView.h>
#include <LDragAndDrop.h>
#include <LBroadcaster.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#pragma options align=mac68k

struct	SColumnViewInfo {
	UInt16		colWidth;
	UInt16		rowHeight;
	Boolean		useSingleSelector;
	Boolean		useDragSelect;
	UInt32		dataSize;
	MessageT	doubleClickMsg;
	MessageT	selectionMsg;
};

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LColumnView : public LTableView,
					  public LDragAndDrop,
					  public LBroadcaster {
public:
	enum { class_ID = FOUR_CHAR_CODE('colv') };

						LColumnView( LStream* inStream );

	virtual				~LColumnView();

	void				SetDoubleClickMsg( MessageT inMessage )
							{
								mDoubleClickMsg = inMessage;
							}

	MessageT			GetDoubleClickMsg()		{ return mDoubleClickMsg; }

	void				SetSelectionMsg( MessageT inMessage )
							{
								mSelectionMsg = inMessage;
							}

	MessageT			GetSelectionMsg()		{ return mSelectionMsg; }

	virtual void		SelectionChanged();

	virtual void		ResizeFrameBy(
								SInt16				inWidthDelta,
								SInt16				inHeightDelta,
								Boolean				inRefresh);

	virtual void		Click( SMouseDownEvent& inMouseDown );

							// Column operations to disallow

	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol,
								const void*			inDataPtr,
								UInt32				inDataSize,
								Boolean				inRefresh);

	virtual void		RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol,
								Boolean				inRefresh);

protected:
	MessageT		mDoubleClickMsg;
	MessageT		mSelectionMsg;
	FlavorType		mDragFlavor;
	FlavorFlags		mFlavorFlags;
	Boolean			mUseFrameWidth;

	virtual void		ClickCell(
								const STableCell&		inCell,
								const SMouseDownEvent&	inMouseDown);

	virtual void		HiliteDropArea( DragReference inDragRef );

	virtual Boolean		ItemIsAcceptable(
								DragReference		inDragRef,
								ItemReference		inItemRef);

private:
	void				InitColumnView( SColumnViewInfo& inInfo );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
