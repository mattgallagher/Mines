// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNodeArrayTree.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A CollapsableTree that uses a DynamicArray to store pointers to
//	Node objects

#ifndef _H_LNodeArrayTree
#define _H_LNodeArrayTree
#pragma once

#include <LCollapsableTree.h>
#include <LArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


struct SHierarchyNode {
	SHierarchyNode	*parent;
	SHierarchyNode	*firstChild;
	SHierarchyNode	*sibling;
	UInt32			wideOpenIndex;
	UInt32			exposedIndex;
	UInt32			nestingLevel;
	Boolean			collapsable;
	Boolean			expanded;
};

// ---------------------------------------------------------------------------

class	LNodeArrayTree : public LCollapsableTree {
public:
						LNodeArrayTree();
	virtual				~LNodeArrayTree();

	virtual UInt32		CountNodes() const;
	virtual UInt32		CountExposedNodes() const;

	virtual UInt32		GetWideOpenIndex(
								UInt32			inExposedIndex) const;
	virtual UInt32		GetExposedIndex(
								UInt32			inWideOpenIndex) const;

	virtual UInt32		GetParentIndex(
								UInt32			inWideOpenIndex) const;
	virtual UInt32		GetNestingLevel(
								UInt32			inWideOpenIndex) const;
	virtual Boolean		IsCollapsable(
								UInt32			inWideOpenIndex) const;
	virtual void		MakeCollapsable(
								UInt32			inWideOpenIndex,
								Boolean			inCollapsable);
	virtual Boolean		IsExpanded(
								UInt32			inWideOpenIndex) const;

	virtual UInt32		CountAllDescendents(
								UInt32			inWideOpenIndex) const;
	virtual UInt32		CountExposedDescendents(
								UInt32			inWideOpenIndex) const;

	virtual UInt32		InsertSiblingNodes(
								UInt32			inHowMany,
								UInt32			inAfterIndex,
								Boolean			inCollapsable);
	virtual void		InsertChildNodes(
								UInt32			inHowMany,
								UInt32			inParentIndex,
								Boolean			inCollapsable);
	virtual UInt32		AddLastChildNode(
								UInt32			inParentIndex,
								Boolean			inCollapsable);
	virtual void		RemoveNode(
								UInt32			inWideOpenIndex,
								UInt32			&outTotalRemoved,
								UInt32			&outExposedRemoved);

	virtual UInt32		CollapseNode(
								UInt32			inWideOpenIndex);
	virtual UInt32		DeepCollapseNode(
								UInt32			inWideOpenIndex);

	virtual UInt32		ExpandNode(
								UInt32			inWideOpenIndex);
	virtual UInt32		DeepExpandNode(
								UInt32			inWideOpenIndex);

protected:
	LArray			mHierarchyArray;
	UInt32			mExposedNodes;

	void				AdjustNestedExposedIndexes(
								SHierarchyNode	*inNode,
								UInt32			&ioExposedIndex);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
