// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHierarchyTable.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LHierarchyTable
#define _H_LHierarchyTable
#pragma once

#include <LTableView.h>
#include <LCollapsableTree.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const Boolean	Collapsable_Yes		= true;
const Boolean	Collapsable_No		= false;


// ---------------------------------------------------------------------------

class	LHierarchyTable : public LTableView {
public:
	enum { class_ID = FOUR_CHAR_CODE('htab') };

						LHierarchyTable();
						
						LHierarchyTable( LStream* inStream );
						
	virtual				~LHierarchyTable();

	void				GetWideOpenTableSize(
								TableIndexT&	outRows,
								TableIndexT&	outCols) const;

	TableIndexT			GetWideOpenIndex( TableIndexT inExposedIndex ) const
							{
								return mCollapsableTree->GetWideOpenIndex(inExposedIndex);
							}

	TableIndexT			GetExposedIndex( TableIndexT inWideOpenIndex ) const
							{
								return mCollapsableTree->GetExposedIndex(inWideOpenIndex);
							}

	UInt32				GetParentIndex( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->GetParentIndex(inWideOpenIndex);
							}

	UInt32				GetNestingLevel( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->GetNestingLevel(inWideOpenIndex);
							}

	Boolean				IsCollapsable( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->IsCollapsable(inWideOpenIndex);
							}

	void				MakeCollapsable(
								UInt32			inWideOpenIndex,
								Boolean			inCollapsable)
							{
								mCollapsableTree->MakeCollapsable(inWideOpenIndex, inCollapsable);
							}

	Boolean				IsExpanded( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->IsExpanded(inWideOpenIndex);
							}

	UInt32				CountAllDescendents( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->CountAllDescendents(inWideOpenIndex);
							}

	UInt32				CountExposedDescendents( UInt32 inWideOpenIndex ) const
							{
								return mCollapsableTree->CountExposedDescendents(inWideOpenIndex);
							}

	virtual void		SetCollapsableTree( LCollapsableTree* inTree );

	virtual void		InsertRows(
								UInt32			inHowMany,
								TableIndexT		inAfterRow,
								const void*		inDataPtr = nil,
								UInt32			inDataSize = 0,
								Boolean			inRefresh = false);
								
	virtual TableIndexT	InsertSiblingRows(
								UInt32			inHowMany,
								TableIndexT		inAfterRow,
								const void*		inDataPtr = nil,
								UInt32			inDataSize = 0,
								Boolean			inCollapsable = false,
								Boolean			inRefresh = false);
								
	virtual TableIndexT	InsertChildRows(
								UInt32			inHowMany,
								TableIndexT		inParentRow,
								const void*		inDataPtr = nil,
								UInt32			inDataSize = 0,
								Boolean			inCollapsable = false,
								Boolean			inRefresh = false);
								
	virtual TableIndexT	AddLastChildRow(
								UInt32			inParentRow,
								const void*		inDataPtr = nil,
								UInt32			inDataSize = 0,
								Boolean			inCollapsable = false,
								Boolean			inRefresh = false);
								
	virtual void		RemoveRows(
								UInt32			inHowMany,
								TableIndexT		inFromRow,
								Boolean			inRefresh);
								
	virtual void		RemoveAllRows( Boolean inRefresh );

	virtual void		CollapseRow( TableIndexT inWideOpenRow );
								
	virtual void		DeepCollapseRow( TableIndexT inWideOpenRow );

	virtual void		ExpandRow( TableIndexT inWideOpenRow );
	
	virtual void		DeepExpandRow( TableIndexT inWideOpenRow );

	virtual void		RevealRow( TableIndexT inWideOpenRow );

protected:
	virtual void		RefreshRowsDownFrom( TableIndexT inWideOpenRow );
								
	virtual void		RefreshRowsBelow( TableIndexT inWideOpenRow );

	virtual void		ConcealRowsBelow(
								UInt32			inHowMany,
								TableIndexT		inWideOpenRow);
								
	virtual void		RevealRowsBelow(
								UInt32			inHowMany,
								TableIndexT		inWideOpenRow);

	virtual void		CalcCellFlagRect(
								const STableCell&	inCell,
								Rect&				outRect);
								
	virtual void		ClickSelf( const SMouseDownEvent& inMouseDown );
								
	virtual void		DrawDropFlag(
								const STableCell&	inCell,
								TableIndexT			inWideOpenRow);

protected:
	LCollapsableTree*	mCollapsableTree;
	Rect				mFlagRect;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
