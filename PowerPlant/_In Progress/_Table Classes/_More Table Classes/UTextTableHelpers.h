// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UTextTableHelpers.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//  Family of helpers for LTextTableView

// 	Contains abstract base classes
//		LCellSizer
//		LTableDragger

#ifndef _H_UTextTableHelpers
#define _H_UTextTableHelpers
#pragma once

#include <UTables.h>
#include <LPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant
	class LTableView;

class LTextTableView;

// ===========================================================================
//	LCellSizer
// ===========================================================================
//
//	Abstract base for a family that measures text.

class LCellSizer {
public:
	LCellSizer(LTextTableView	*inView=0, Boolean inTrunc=true) :
		mTableView(inView),
		mTruncateAtWord(inTrunc)
	{};

	virtual				~LCellSizer() {};

	virtual UInt16	TruncatedLength(
								const char		*inValue,	// C string
								UInt16			inItemSize,
								TableIndexT		inCol) = 0;

	void	SetTableView(LTextTableView	*inView) { mTableView = inView; };

	void	SetWordTruncation(Boolean inTrunc) { mTruncateAtWord = inTrunc; };
	
	Boolean	GetWordTruncation() { return mTruncateAtWord; };

	UInt16	TruncateTrailingWord(
								const char		*inValue,	// C string
								UInt16			inEndWord) const;

protected:
	LTextTableView	*mTableView;
	Boolean			mTruncateAtWord;
};


// ===========================================================================
//	LTableDragger
// ===========================================================================
//
//	Abstract base for a family that starts Drag Manager tasks from a table

class LTableDragger {
public:
	LTableDragger() {};

	virtual	~LTableDragger() {};

	virtual void	CreateDragTask(LTableView	*fromTable, const SMouseDownEvent& inMouseDown)=0;	// maybe later take just LTableView

protected:
	virtual void	CalcDragRect(LTableView	*fromTable, STableCell startingCell, Rect& outRect);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
