// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNodeArrayTree.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LNodeArrayTree.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LNodeArrayTree						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LNodeArrayTree::LNodeArrayTree()
	 
	 : mHierarchyArray(sizeof(SHierarchyNode*))
{
	mExposedNodes = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LNodeArrayTree						Destructor				  [public]
// ---------------------------------------------------------------------------

LNodeArrayTree::~LNodeArrayTree()
{
	SHierarchyNode	*theNode;
	SInt32			nodeCount = (SInt32) mHierarchyArray.GetCount();
	for (SInt32 i = 1; i <= nodeCount; i++) {
		mHierarchyArray.FetchItemAt(i, &theNode);
		delete theNode;
	}
}

#pragma mark --- Accessors ---

// ---------------------------------------------------------------------------
//	¥ CountNodes
// ---------------------------------------------------------------------------
//	Return the number of nodes in a Tree

UInt32
LNodeArrayTree::CountNodes() const
{
	return mHierarchyArray.GetCount();
}


// ---------------------------------------------------------------------------
//	¥ CountExposedNodes
// ---------------------------------------------------------------------------
//	Return the number of exposed nodes in a Tree

UInt32
LNodeArrayTree::CountExposedNodes() const
{
	return mExposedNodes;
}


// ---------------------------------------------------------------------------
//	¥ GetWideOpenIndex
// ---------------------------------------------------------------------------
//	Return the wide-open index of a node specified by its exposed index

UInt32
LNodeArrayTree::GetWideOpenIndex(
	UInt32	inExposedIndex) const
{
	UInt32		wideOpenIndex = 0;

	if ((inExposedIndex > 0) && (inExposedIndex <= mExposedNodes)) {
			// +++ Replace with binary search +++

		SHierarchyNode	*theRow;
		while (mHierarchyArray.FetchItemAt((SInt32) ++wideOpenIndex, &theRow)) {
			if (theRow->exposedIndex == inExposedIndex) {
				break;
			}
		}
	}

	return wideOpenIndex;
}


// ---------------------------------------------------------------------------
//	¥ GetExposedIndex
// ---------------------------------------------------------------------------
//	Return the exposed index of a node specified by its wide-open index

UInt32
LNodeArrayTree::GetExposedIndex(
	UInt32	inWideOpenIndex) const
{
	UInt32		exposedIndex = 0;

	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow)) {
		exposedIndex = theRow->exposedIndex;
	}

	return exposedIndex;
}


// ---------------------------------------------------------------------------
//	¥ GetParentIndex
// ---------------------------------------------------------------------------
//	Return the wide-open index of the parent of a Node

UInt32
LNodeArrayTree::GetParentIndex(
	UInt32	inWideOpenIndex) const
{
	UInt32	parentIndex = 0;

	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow) &&
		(theRow->parent != nil)) {
		parentIndex = theRow->parent->wideOpenIndex;
	}

	return parentIndex;
}


// ---------------------------------------------------------------------------
//	¥ GetNestingLevel
// ---------------------------------------------------------------------------
//	Return the nesting level of a Node
//
//	Top level nodes are at nesting level 0

UInt32
LNodeArrayTree::GetNestingLevel(
	UInt32	inWideOpenIndex) const
{
	UInt32	nestingLevel = 0;

	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow)) {
		nestingLevel = theRow->nestingLevel;
	}

	return nestingLevel;
}


// ---------------------------------------------------------------------------
//	¥ IsCollapsable
// ---------------------------------------------------------------------------
//	Return whether a Node is collapsable (can have children)

Boolean
LNodeArrayTree::IsCollapsable(
	UInt32	inWideOpenIndex) const
{
	Boolean	collapsable = false;

	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow)) {
		collapsable = theRow->collapsable;
	}

	return collapsable;
}


// ---------------------------------------------------------------------------
//	¥ MakeCollapsable
// ---------------------------------------------------------------------------
//	Set the collapsable flag for a Node
//
//	Setting a Node that has children to be not collapsable is not allowed.

void
LNodeArrayTree::MakeCollapsable(
	UInt32	inWideOpenIndex,
	Boolean	inCollapsable)
{
	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow)) {

		if (!inCollapsable && (theRow->firstChild != nil)) {
			SignalStringLiteral_("Can't make Node with children not collapsable");

		} else {
			theRow->collapsable = inCollapsable;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ IsExpanded
// ---------------------------------------------------------------------------
//	Return whether a Node is expanded (rather than collapsed)

Boolean
LNodeArrayTree::IsExpanded(
	UInt32	inWideOpenIndex) const
{
	Boolean	expanded = true;

	SHierarchyNode	*theRow;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theRow)) {
		expanded = theRow->expanded;
	}

	return expanded;
}


// ---------------------------------------------------------------------------
//	¥ CountAllDescendents
// ---------------------------------------------------------------------------
//	Return the total number of descendents of a Node

UInt32
LNodeArrayTree::CountAllDescendents(
	UInt32	inWideOpenIndex) const
{
	UInt32	descendents = 0;

	SHierarchyNode	*theNode;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theNode)) {

		if (theNode->firstChild != nil) {	// Node has some descendents

				// In our flattened array of Nodes, the descendents of a
				// Node are all the nodes between that Node and the Node's
				// sibling (or the sibling of its closest ancestor that
				// has a sibling).

			SHierarchyNode	*prevSibling = theNode;
			while ((prevSibling != nil) && (prevSibling->sibling == nil)) {
				prevSibling = prevSibling->parent;
			}

			if ((prevSibling != nil) && (prevSibling->sibling != nil)) {
				descendents = prevSibling->sibling->wideOpenIndex -
								inWideOpenIndex - 1;

			} else {
				descendents = CountNodes() - inWideOpenIndex;
			}
		}
	}

	return descendents;
}


// ---------------------------------------------------------------------------
//	¥ CountExposedDescendents
// ---------------------------------------------------------------------------
//	Return the number of exposed descendents of a Node

UInt32
LNodeArrayTree::CountExposedDescendents(
	UInt32	inWideOpenIndex) const
{
	UInt32	exposedDescendents = 0;

	SHierarchyNode	*theNode;
	if (mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &theNode)) {

		if ( (theNode->exposedIndex > 0) &&
			 theNode->expanded &&
			 (theNode->firstChild != nil) ) {

				// Node is exposed, expanded, and has descendents.
				// In our flattened array of Nodes, the descendents of a
				// Node are all the nodes between that Node and the Node's
				// sibling (or the sibling of its closest ancestor that
				// has a sibling).

			SHierarchyNode	*prevSibling = theNode;
			while ((prevSibling != nil) && (prevSibling->sibling == nil)) {
				prevSibling = prevSibling->parent;
			}

			if ((prevSibling != nil) && (prevSibling->sibling != nil)) {
				exposedDescendents = prevSibling->sibling->exposedIndex -
										theNode->exposedIndex - 1;

			} else {
				exposedDescendents = mExposedNodes - theNode->exposedIndex;
			}
		}
	}

	return exposedDescendents;
}

#pragma mark --- Insert/Delete Nodes ---

// ---------------------------------------------------------------------------
//	¥ InsertSiblingNodes
// ---------------------------------------------------------------------------
//	Add nodes to a Tree as siblings of the specified node
//
//	If inAfterIndex is 0, Nodes are inserted at the beginning of the Tree.
//	If inAfterIndex too big, Nodes are inserted at the end of the Tree,
//		but at the top level
//
//	Returns the wide open index of the first inserted node

UInt32
LNodeArrayTree::InsertSiblingNodes(
	UInt32	inHowMany,
	UInt32	inAfterIndex,				// WideOpen index
	Boolean	inCollapsable)
{
		// Determine where to insert the new Nodes

	SHierarchyNode	*parent = nil;
	SHierarchyNode	*afterSibling = nil;
	SHierarchyNode	*beforeSibling = nil;
	UInt32			wideOpenIndex;
	UInt32			exposedIndex;
	UInt32			nestingLevel;
	UInt32			lastIndex = CountNodes();
	UInt32			afterIndex;

	if ((inAfterIndex == 0) || (lastIndex == 0)) {
										// Insert at the beginning
		mHierarchyArray.FetchItemAt(1, &beforeSibling);
		wideOpenIndex = 1;
		exposedIndex = 1;
		nestingLevel = 0;
		afterIndex = 0;

	} else if (inAfterIndex > 0) {
		mHierarchyArray.FetchItemAt((SInt32) inAfterIndex, &afterSibling);

		if (afterSibling != nil) {		// Insert after this sibling
			parent = afterSibling->parent;
			beforeSibling = afterSibling->sibling;
			wideOpenIndex = afterSibling->wideOpenIndex +
							CountAllDescendents(afterSibling->wideOpenIndex) + 1;
			exposedIndex = afterSibling->exposedIndex;
			if (exposedIndex > 0) {
				exposedIndex += CountExposedDescendents(afterSibling->wideOpenIndex) + 1;
			}
			nestingLevel = afterSibling->nestingLevel;
			afterIndex = wideOpenIndex - 1;

		} else {						// Insert at the end at the top level
			wideOpenIndex = lastIndex + 1;
			exposedIndex = mExposedNodes + 1;
			nestingLevel = 0;
			afterIndex = lastIndex;

										// Sibling of new rows will be the
										//   last Node at the top level.
										//   We find this Node by starting
										//   with the last Node and following
										//   the parent links.
			mHierarchyArray.FetchItemAt((SInt32) lastIndex, &afterSibling);
			while (afterSibling->parent != nil) {
				afterSibling = afterSibling->parent;
			}
		}
	}

										// Make space for Nodes in Array
	mHierarchyArray.InsertItemsAt(inHowMany, (SInt32) afterIndex, nil);

		// Create Node objects for new items and store pointers to them in
		// the Hierarchy Array. Do this in reverse order so that the
		// sibling field can be set properly.

	SHierarchyNode	*sibling = beforeSibling;
	SHierarchyNode	*theNode;

	for (UInt32 i = inHowMany; i >= 1; i--) {
		theNode = new SHierarchyNode;
		theNode->parent = parent;
		theNode->firstChild = nil;
		theNode->sibling = sibling;
		theNode->wideOpenIndex = wideOpenIndex + i - 1;
		theNode->exposedIndex = 0;
		if (exposedIndex > 0) {
			theNode->exposedIndex = exposedIndex + i - 1;
		}
		theNode->nestingLevel = nestingLevel;
		theNode->collapsable = inCollapsable;
		theNode->expanded = true;

		mHierarchyArray.AssignItemsAt(1, (SInt32) (afterIndex + i), &theNode);

		sibling = theNode;
	}

	if (afterSibling != nil) {
		afterSibling->sibling = theNode;
	}

										// Adjust indexes for all Nodes
										//   after the inserted ones
	lastIndex = CountNodes();
	for (UInt32 j = afterIndex + inHowMany + 1; j <= lastIndex; j++) {
		mHierarchyArray.FetchItemAt((SInt32) j, &theNode);

		theNode->wideOpenIndex = j;

		if ((exposedIndex > 0) && (theNode->exposedIndex > 0)) {
			theNode->exposedIndex += inHowMany;
		}
	}

	if (exposedIndex > 0) {
		mExposedNodes += inHowMany;
	}

	return afterIndex + 1;
}


// ---------------------------------------------------------------------------
//	¥ InsertChildNodes
// ---------------------------------------------------------------------------
//	Add nodes to a Tree as the first children of the specified node

void
LNodeArrayTree::InsertChildNodes(
	UInt32	inHowMany,
	UInt32	inParentIndex,				// WideOpen index
	Boolean	inCollapsable)
{
	SHierarchyNode	*parent = nil;
	if (!mHierarchyArray.FetchItemAt((SInt32) inParentIndex, &parent)) {
		SignalStringLiteral_("Invalid Parent Index");
		return;
	}
										// Make space for Nodes in Array
	mHierarchyArray.InsertItemsAt(inHowMany, (SInt32) inParentIndex + 1, nil);

		// Create Node objects for new items and store pointers to them in
		// the Hierarchy Array. Do this in reverse order so that the
		// sibling field can be set properly.

	SHierarchyNode	*theNode;
	SHierarchyNode	*sibling = parent->firstChild;
	UInt32			exposedIndex = 0;
	if ((parent->exposedIndex > 0) && parent->expanded) {
		exposedIndex = parent->exposedIndex;
	}
	UInt32			nestingLevel = parent->nestingLevel + 1;

	for (UInt32 i = inHowMany; i >= 1; i--) {
		theNode = new SHierarchyNode;
		theNode->parent = parent;
		theNode->firstChild = nil;
		theNode->sibling = sibling;
		theNode->wideOpenIndex = parent->wideOpenIndex + i;
		theNode->exposedIndex = 0;
		if (exposedIndex > 0) {
			theNode->exposedIndex = exposedIndex + i;
		}
		theNode->nestingLevel = nestingLevel;
		theNode->collapsable = inCollapsable;
		theNode->expanded = true;

		mHierarchyArray.AssignItemsAt(1, (SInt32) (inParentIndex + i), &theNode);

		sibling = theNode;
	}

	parent->firstChild = theNode;
	parent->collapsable = true;

										// Adjust indexes for all Nodes
										//   after the inserted ones
	UInt32	lastIndex = CountNodes();
	for (UInt32 j = inParentIndex + inHowMany + 1; j <= lastIndex; j++) {
		mHierarchyArray.FetchItemAt((SInt32) j, &theNode);

		theNode->wideOpenIndex = j;

		if ((exposedIndex > 0) && (theNode->exposedIndex > 0)) {
			theNode->exposedIndex += inHowMany;
		}
	}

	if (exposedIndex > 0) {
		mExposedNodes += inHowMany;
	}
}


// ---------------------------------------------------------------------------
//	¥ AddLastChildNode
// ---------------------------------------------------------------------------
//	Add a single Node as the last child of the specified node

UInt32
LNodeArrayTree::AddLastChildNode(
	UInt32	inParentIndex,				// WideOpen index
	Boolean	inCollapsable)
{
	if (inParentIndex == 0) {			// Put at end at the top level
		return InsertSiblingNodes(1, CountNodes() + 1, inCollapsable);
	}

	SHierarchyNode	*parent = nil;
	if (!mHierarchyArray.FetchItemAt((SInt32) inParentIndex, &parent)) {
		SignalStringLiteral_("Invalid Parent Index");
		return 0;
	}

	if (parent->firstChild == nil) {	// Parent has no children, so new
										//   node will be its first child
		InsertChildNodes(1, inParentIndex, inCollapsable);
		return inParentIndex + 1;

	} else {							// Find last child by following
										//   children's sibling chain
		SHierarchyNode	*child = parent->firstChild;
		while (child->sibling != nil) {
			child = child->sibling;
		}

		return InsertSiblingNodes(1, child->wideOpenIndex, inCollapsable);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveNode
// ---------------------------------------------------------------------------
//	Remove a single Node (and all its descendents) from the Tree
//
//	Return the number of exposed nodes deleted by the removal

void
LNodeArrayTree::RemoveNode(
	UInt32	inWideOpenIndex,
	UInt32	&outTotalRemoved,
	UInt32	&outExposedRemoved)
{
	SHierarchyNode	*removedNode;
	if (!mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &removedNode)) {
		SignalStringLiteral_("Invalid index for Node to Remove");
		return;
	}

		// Deleting this node deletes all its descendent nodes

	outTotalRemoved = CountAllDescendents(inWideOpenIndex) + 1;
	outExposedRemoved = 0;
	if (removedNode->exposedIndex > 0) {
		outExposedRemoved = CountExposedDescendents(inWideOpenIndex) + 1;
	}

		// Some other Node points to the Removed Node (unless we are
		// removing the first Node). That Node is either its parent
		// (if it is the first child) or its previous sibling. We must
		// change that Node to point to the Removed Node's next sibling.

	SHierarchyNode	*previousSibling = nil;
	mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex - 1, &previousSibling);
										// Node before this one could be an
										//   ancestor of the previous sibling.
										//   Move up the parent chain find
										//   the previous sibling.
	while ((previousSibling != nil) && (previousSibling->sibling != removedNode)) {
		previousSibling = previousSibling->parent;
	}

	if (previousSibling != nil) {		// Removed Node has a previous sibling
		previousSibling->sibling = removedNode->sibling;
	} else if (removedNode->parent != nil) {
										// Remove Node is a first child
		removedNode->parent->firstChild = removedNode->sibling;
	}

		// Delete Node objects for Removed Node and all its descendents.

	UInt32	lastToDelete = inWideOpenIndex + outTotalRemoved - 1;
	for (UInt32 i = inWideOpenIndex; i <= lastToDelete; i++) {
		SHierarchyNode	*nodeToDelete ;
		mHierarchyArray.FetchItemAt((SInt32) i, &nodeToDelete);
		delete nodeToDelete;
	}

		// Remove pointers to deleted Nodes from the Hierarchy array

	mHierarchyArray.RemoveItemsAt(outTotalRemoved, (SInt32) inWideOpenIndex);

		// Adjust the indexes of the Nodes after the deleted one

	UInt32	lastNode = CountNodes();
	for (UInt32 j = inWideOpenIndex; j <= lastNode; j++) {
		SHierarchyNode	*theNode;
		mHierarchyArray.FetchItemAt((SInt32) j, &theNode);
		theNode->wideOpenIndex = j;

		if (theNode->exposedIndex != 0) {
			theNode->exposedIndex -= outExposedRemoved;
		}
	}

	mExposedNodes -= outExposedRemoved;
}

#pragma mark --- Collapse/Expand Nodes ---

// ---------------------------------------------------------------------------
//	¥ CollapseNode
// ---------------------------------------------------------------------------
//	Collapse a Node in the Tree, concealing its child Nodes
//
//	Return the number of nodes concealed by the collapse

UInt32
LNodeArrayTree::CollapseNode(
	UInt32	inWideOpenIndex)
{
	UInt32		nodesConcealed = 0;

	SHierarchyNode	*collapsedNode;
	if (!mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &collapsedNode)) {
		SignalStringLiteral_("Invalid index for Node to Collapse");
		return 0;
	}

	if (collapsedNode->expanded) {		// Node is currently expanded

		if (collapsedNode->exposedIndex > 0) {
										// Node is exposed
			nodesConcealed = CountExposedDescendents(inWideOpenIndex);

			if (nodesConcealed > 0) {
										// Conceal all descendent Nodes
				UInt32	lastToConceal = collapsedNode->wideOpenIndex +
									CountAllDescendents(inWideOpenIndex);
				for (UInt32 i = collapsedNode->wideOpenIndex + 1;
					 i <= lastToConceal; i++) {
					SHierarchyNode	*descendent;
					mHierarchyArray.FetchItemAt((SInt32) i, &descendent);
					descendent->exposedIndex = 0;
				}

										// Adjust exposed indexes for all
										//   Nodes after the concealed ones
				UInt32	currentCount = CountNodes();
				for (UInt32 j = lastToConceal + 1; j <= currentCount; j++) {
					SHierarchyNode	*theNode;
					mHierarchyArray.FetchItemAt((SInt32) j, &theNode);

					if (theNode->exposedIndex > 0) {
						theNode->exposedIndex -= nodesConcealed;
					}
				}

				mExposedNodes -= nodesConcealed;
			}
		}
		collapsedNode->expanded = false;
	}

	return nodesConcealed;
}


// ---------------------------------------------------------------------------
//	¥ DeepCollapseNode
// ---------------------------------------------------------------------------
//	Collapse a Node and all its descendent Nodes
//
//	Return the number of nodes concealed by the deep collapse

UInt32
LNodeArrayTree::DeepCollapseNode(
	UInt32	inWideOpenIndex)
{
										// Collapse this Node
	UInt32	nodesConcealed = CollapseNode(inWideOpenIndex);

	SHierarchyNode	*collapsedNode;
	mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &collapsedNode);

										// Collapse all descendents
	UInt32	lastDescendent = inWideOpenIndex +
								CountAllDescendents(inWideOpenIndex);
	for (UInt32 i = inWideOpenIndex + 1; i <= lastDescendent; i++) {
		CollapseNode(i);
	}

	return nodesConcealed;
}


// ---------------------------------------------------------------------------
//	¥ ExpandNode
// ---------------------------------------------------------------------------
//	Expand a Node in the Tree, revealing its child Nodes
//
//	Return the number of Nodes revealed by the expansion

UInt32
LNodeArrayTree::ExpandNode(
	UInt32	inWideOpenIndex)
{
	UInt32			nodesRevealed = 0;

	SHierarchyNode	*expandedNode;
	if (!mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &expandedNode)) {
		SignalStringLiteral_("Invalid index for Node to Expand");
		return 0;
	}

	if (!expandedNode->expanded) {		// Node is currently collapsed

		if (expandedNode->exposedIndex > 0) {

				// Node is exposed. Check each descendent. If expanding
				// this Node exposes the descendent, set its exposedIndex.

			UInt32	exposedIndex = expandedNode->exposedIndex + 1;
			AdjustNestedExposedIndexes(expandedNode->firstChild, exposedIndex);

			nodesRevealed = exposedIndex - expandedNode->exposedIndex - 1;

			if (nodesRevealed > 0) {
										// Adjust exposedIndex of all Nodes
										//   after the expanded ones
				UInt32	nextIndex = inWideOpenIndex + 1 +
									CountAllDescendents(inWideOpenIndex);
				UInt32	lastIndex = CountNodes();
				for (UInt32 j = nextIndex; j <= lastIndex; j++) {
					SHierarchyNode	*theNode;
					mHierarchyArray.FetchItemAt((SInt32) j, &theNode);

					if (theNode->exposedIndex > 0) {
						theNode->exposedIndex += nodesRevealed;
					}
				}

				mExposedNodes += nodesRevealed;
			}
		}
		expandedNode->expanded = true;
	}

	return nodesRevealed;
}


// ---------------------------------------------------------------------------
//	¥ DeepExpandNode
// ---------------------------------------------------------------------------
//	Expand a Node and all its descendent Nodes
//
//	Return the number of Nodes revealed by the expansion

UInt32
LNodeArrayTree::DeepExpandNode(
	UInt32	inWideOpenIndex)
{
	SHierarchyNode	*expandedNode;
	if (!mHierarchyArray.FetchItemAt((SInt32) inWideOpenIndex, &expandedNode)) {
		SignalStringLiteral_("Invalid index for Node to Deep Expand");
		return 0;
	}

										// Expand all descendents
	UInt32	lastDescendent = inWideOpenIndex +
								CountAllDescendents(inWideOpenIndex);
	for (UInt32 i = inWideOpenIndex + 1; i <= lastDescendent; i++) {
		ExpandNode(i);
	}

	return ExpandNode(inWideOpenIndex);	// Expand this Node
}



// ---------------------------------------------------------------------------
//	¥ AdjustNestedExposedIndexes
// ---------------------------------------------------------------------------

void
LNodeArrayTree::AdjustNestedExposedIndexes(
	SHierarchyNode	*inNode,
	UInt32			&ioExposedIndex)
{
	while (inNode != nil) {
		inNode->exposedIndex = ioExposedIndex++;
		if (inNode->expanded) {
			AdjustNestedExposedIndexes(inNode->firstChild, ioExposedIndex);
		}
		inNode = inNode->sibling;
	}
}

PP_End_Namespace_PowerPlant
