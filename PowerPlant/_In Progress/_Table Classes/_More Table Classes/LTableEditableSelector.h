// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableEditableSelector.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A selector which contains both the real selector and a set of editors
//	It uses the Decorator design pattern to provide the selector interface,
//	transparently passing some actions through tothe editors.

#ifndef _H_LTableEditableSelector
#define _H_LTableEditableSelector
#pragma once

#include <UTextTableHelpers.h>
#include <UTableHelpers.h>
#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


class	LCellEditor;

class	LTableEditableSelector : public LTableSelector {
public:
						LTableEditableSelector(
								LTableSelector* inRealSelector);

	virtual				~LTableEditableSelector();

	virtual Boolean		CellIsSelected(
								const STableCell		&inCell) const;
	virtual	STableCell	GetFirstSelectedCell() const;
	virtual	TableIndexT	GetFirstSelectedRow() const;

	virtual void		SelectCell(
								const STableCell		&inCell);
	virtual void		SelectAllCells();

	virtual void		UnselectCell(
								const STableCell		&inCell);
	virtual void		UnselectAllCells();

	virtual void		ClickSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown);
	virtual Boolean	DragSelect(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown);

	virtual void		InsertRows(
								UInt32					inHowMany,
								TableIndexT				inAfterRow);
	virtual void		InsertCols(
								UInt32					inHowMany,
								TableIndexT				inAfterCol);
	virtual void		RemoveRows(
								UInt32					inHowMany,
								TableIndexT				inFromRow);
	virtual void		RemoveCols(
								UInt32					inHowMany,
								TableIndexT				inFromCol);

// extra interface
	virtual void		AttachEditorToCol(
								LCellEditor* adoptedEditor,
								TableIndexT	inCol);

protected:
	virtual LCellEditor*	GetEditor(
								const STableCell		&inCell) const;

	LTableSelector*		mRealSelector;	// owned
	LArray				mEditors;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif

