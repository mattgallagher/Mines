// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineItem.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOutlineItem
#define _H_LOutlineItem
#pragma once

#include <UTables.h>
#include <UTextTraits.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LArray;
class LOutlineTable;

struct SMouseDownEvent;


// ===========================================================================
//		* SOutlineDrawContents
// ===========================================================================
//	SOutlineDrawContents is used to specify the contents of a cell.
//	It is used by the clicking and drawing routines in LOutlineItem.

struct SOutlineDrawContents {

	// basic drawing info

	Rect				ioCellBounds;			// bounds of cell, adjusted for indenting
	Boolean				outShowSelection;		// true if selection should be indicated on this cell

	// icon drawing info

	Boolean				outHasIcon;				// true if an icon is drawn for this cell
	Handle				outIconSuite;			// icon to draw (small icon)
	SInt32				outIconTransform;		// transform to apply to icon
	SInt32				outIconAlign;			// alignment to apply to icon

	// text drawing info

	Str255				outTextString;			// string to draw (if any)
	TextTraitsRecord	outTextTraits;			// text traits to apply to string
	Boolean				outCanDoInPlaceEdit;	// true if in-place editing is allowed from here
	Boolean				outMultiLineText;		// true if multi-line text is supported
	SInt16				outTextBaseline;		// vertical coordinate of text baseline
	Boolean				outDoTruncation;		// true if you want to trunc the string to the cell

	// used internally

	Str255				prTruncatedString;		// truncated version of outTextString
	SInt16				prTextWidth;			// width of actual text string
	Rect				prTextFrame;			// where text is drawn
												//	(inset by 3 horiz & 2 vert)
	Rect				prIconFrame;			// where icon is drawn

};


// ===========================================================================
//		* LOutlineItem											[abstract]
// ===========================================================================
//	An OutlineItem is created for every row in an OutlineTable.
//	Subclass OutlineItem to specify the content of individual
//	cells for each row.

class LOutlineItem {

public:
							LOutlineItem();
	virtual					~LOutlineItem();

	// accessors

public:
	inline const LArray*	GetSubItems() const
									{ return mSubItems; }
	virtual UInt32			DeepCountSubItems() const;

	inline LOutlineItem*	GetSuperItem() const
									{ return mSuperItem; }

	inline SInt16			GetLeftEdge() const
									{ return mLeftEdge; }
	inline SInt16			GetIndentSize() const
									{ return mIndentSize; }
	inline UInt16			GetIndentDepth() const
									{ return mIndentDepth; }

	// disclosure triangle

	virtual void			Expand();
	virtual void			DeepExpand();
	virtual void			Collapse();

	virtual Boolean			CanExpand() const;
	Boolean					IsExpanded() const
									{ return mExpanded; }

	virtual void			RefreshDisclosureTriangle();
	virtual Boolean			CalcLocalDisclosureTriangleRect(
									Rect&					outTriangleRect) const;

	// row display helpers

	virtual TableIndexT		FindRowForItem() const;
	virtual void			ScrollItemIntoFrame();

	virtual void			RefreshSelection(
									const STableCell&		inCell);
	virtual void			MakeDragRegion(
									const STableCell&		inCell,
									RgnHandle				outGlobalDragRgn,
									Rect&					outGlobalItemBounds);

	// display information accessors

	virtual Boolean			CanDoInPlaceEdit(
									const STableCell&		inCell);
	virtual Boolean			ShowsSelection(
									const STableCell&		inCell);

	// override hooks

	virtual void			FinishCreate();

protected:
	virtual void			UpdateRowSize();

	virtual SInt16			CalcRowHeightForCell(
									const STableCell&		inCell);

	virtual void			GetDrawContentsSelf(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);

	virtual void			DrawRowAdornments(
									const Rect&				inLocalRowRect);

	virtual void			ExpandSelf();
	virtual void			CollapseSelf();

	virtual void			InsertCols(
									UInt32					inHowMany,
									TableIndexT				inAfterCol,
									const void*				inDataPtr,
									UInt32					inDataSize,
									Boolean					inRefresh);
	virtual void			RemoveCols(
									UInt32					inHowMany,
									TableIndexT				inFromCol,
									Boolean					inRefresh);

	// cell contents

	virtual void			GetDrawContents(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);
	virtual void			ResetDrawContents(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);
	virtual void			PrepareDrawContents(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);

	// drawing

	virtual void			DrawCell(
									const STableCell&		inCell,
									const Rect&				inLocalCellRect);
	virtual void			DrawDisclosureTriangle();

	virtual void			ShadeRow(
									const RGBColor&			inColor,
									const Rect&				inLocalRowRect);

	// clicking

	virtual void			ClickCell(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown);
	virtual void			TrackDisclosureTriangle(
									const SMouseDownEvent&		inMouseDown);
	virtual void			TrackContentClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents,
									Boolean						inHitText);
	virtual void			TrackEmptyClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents);

	virtual void			TrackDrag(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents);
	virtual void			SingleClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents,
									Boolean						inHitText);
	virtual void			DoubleClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents,
									Boolean						inHitText);

	// subitem list

	virtual void			AddSubItem(
									LOutlineItem*			inSubItem,
									LOutlineItem*			inAfterItem);
	virtual void			RemoveSubItem(
									LOutlineItem*			inSubItem);

	// drag selection

	virtual Boolean			CellHitByMarquee(
									const STableCell&		inCell,
									const Rect&				inMarqueeLocalRect);


	// data members

protected:
	LOutlineTable*			mOutlineTable;				// the table we belong to
	LOutlineItem*			mSuperItem;					// parent item in the outline
	LArray*					mSubItems;					// (of LOutlineItem*) subitems in this outline

	SInt16					mLeftEdge;					// left edge of icon or text in first column
	SInt16					mIndentSize;				// amount to indent for next level
	UInt16					mIndentDepth;				// number of superitems in outline
	Boolean					mExpanded;					// true if this row is expanded
	Boolean					mFlipping;					// true if this row is being expanded/collapsed


	// class variables

	static Handle			sIconHierCondensed;			// collapsed row icon
	static Handle			sIconHierExpanded;			// expanded row icon
	static Handle			sIconHierBeingFlipped;		// transition icon


	friend class LOutlineTable;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
