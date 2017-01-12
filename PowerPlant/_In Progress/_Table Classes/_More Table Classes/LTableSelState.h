// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableSelState.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent


//
//	Family of lightweight classes to save the internal state of LTableSelector subclasses

#ifndef _H_LTableSelState
#define _H_LTableSelState
#pragma once

#include <UTables.h>
#include <Quickdraw.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant
	class LArray;

class LTableSelState {
public:
	virtual ~LTableSelState() {};
};


// future classes to save state of standard classes
// LTableSingleSelector - uses LTableSingleState
// LTableMultiSelector, and all subclasses, uses LTableMultiState

class LTableSingleRowState : public LTableSelState {
public:
	LTableSingleRowState(TableIndexT inRow=0);
	virtual ~LTableSingleRowState();

	TableIndexT GetRow() const;
	void SetRow(TableIndexT);

private:
	TableIndexT mRow;
};


class LTableMultiRowState : public LTableSelState {
public:
	LTableMultiRowState(const STableCell&, const RgnHandle);
	virtual ~LTableMultiRowState();

	const STableCell&	GetAnchorCell() const;
	RgnHandle	GetRegion() const;

private:
	STableCell		mAnchorCell;
	RgnHandle		mSelectionRgn;
};


// also contains LOutlineTable expanded state
class LOutlineSelState : public LTableSelState {
public:
	LOutlineSelState(LTableSelState*,LArray* inState=0);
	virtual ~LOutlineSelState();

	LArray*	GetExpandState() const;
	void SetExpandState(LArray*	);
	LTableSelState* GetSavedSelection() const;
	void SetSavedSelection(LTableSelState*);

private:
	LTableSelState*	mWrappedSel;	// owned
	LArray*	mExpandState;	// owned, may remain 0
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif

