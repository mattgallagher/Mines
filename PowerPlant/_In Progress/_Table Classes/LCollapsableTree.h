// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCollapsableTree.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCollapsableTree
#define _H_LCollapsableTree
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCollapsableTree {
public:
						LCollapsableTree() { };
	virtual				~LCollapsableTree() { };

	virtual UInt32		CountNodes() const = 0;
	virtual UInt32		CountExposedNodes() const = 0;

	virtual UInt32		GetWideOpenIndex(
								UInt32			inExposedIndex) const = 0;
	virtual UInt32		GetExposedIndex(
								UInt32			inWideOpenIndex) const = 0;

	virtual UInt32		GetParentIndex(
								UInt32			inWideOpenIndex) const = 0;
	virtual UInt32		GetNestingLevel(
								UInt32			inWideOpenIndex) const = 0;
	virtual Boolean		IsCollapsable(
								UInt32			inWideOpenIndex) const = 0;
	virtual void		MakeCollapsable(
								UInt32			inWideOpenIndex,
								Boolean			inCollapsable) = 0;
	virtual Boolean		IsExpanded(
								UInt32			inWideOpenIndex) const = 0;

	virtual UInt32		CountAllDescendents(
								UInt32			inWideOpenIndex) const = 0;
	virtual UInt32		CountExposedDescendents(
								UInt32			inWideOpenIndex) const = 0;

	virtual UInt32		InsertSiblingNodes(
								UInt32			inHowMany,
								UInt32			inAfterIndex,
								Boolean			inCollapsable) = 0;
	virtual void		InsertChildNodes(
								UInt32			inHowMany,
								UInt32			inParentIndex,
								Boolean			inCollapsable) = 0;
	virtual UInt32		AddLastChildNode(
								UInt32			inParentIndex,
								Boolean			inCollapsable) = 0;
	virtual void		RemoveNode(
								UInt32			inWideOpenIndex,
								UInt32			&outTotalRemoved,
								UInt32			&outExposedRemoved) = 0;

	virtual UInt32		CollapseNode(
								UInt32			inWideOpenIndex) = 0;
	virtual UInt32		DeepCollapseNode(
								UInt32			inWideOpenIndex) = 0;

	virtual UInt32		ExpandNode(
								UInt32			inWideOpenIndex) = 0;
	virtual UInt32		DeepExpandNode(
								UInt32			inWideOpenIndex) = 0;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
