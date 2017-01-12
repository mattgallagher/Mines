// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInterruptSafeList.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Implements a container class which can add and remove members at
//	interrupt time. The current implementation uses the Toolbox calls
//	Enqueue and Dequeue, but this may change at a later time.

#include <LInterruptSafeList.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LInterruptSafeList					Default Constructor		  [public]
// ---------------------------------------------------------------------------

LInterruptSafeList::LInterruptSafeList()
{
	mQueue.qHead =
		mQueue.qTail =
			mIteratorQueue.qHead =
				mIteratorQueue.qTail = nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LInterruptSafeList					Destructor				  [public]
// ---------------------------------------------------------------------------
//	Does not delete the items in the list

LInterruptSafeList::~LInterruptSafeList()
{
									// Notify iterators of our death
	SIteratorQueueElem*	iterQElem = GetFirstIterator();
	while (iterQElem) {
		iterQElem->mIterator->ListDied();
		iterQElem = iterQElem->mNextElem;
	}
}


// ---------------------------------------------------------------------------
//	¥ Append														  [public]
// ---------------------------------------------------------------------------
//	Call to add an item to the end of a list. Note that there is currently no
//	way to insert an item at any other position in a list.
//
//	May be called at interrupt time.

void
LInterruptSafeList::Append(
	LInterruptSafeListMember* inItem)
{
	if (inItem->mParentList == nil) {	// Item must not be in another list

		inItem->mParentList = this;		// Add item to this list
		::Enqueue((QElemPtr)&(inItem->mQueueElem), &mQueue);

										// Notify iterators of the addition
		SIteratorQueueElem*	iterQElem = GetFirstIterator();
		while (iterQElem) {
			iterQElem->mIterator->ItemAppended(inItem);
			iterQElem = iterQElem->mNextElem;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Remove														  [public]
// ---------------------------------------------------------------------------
//	Call to remove an item from a list. Returns false if unable to remove the
//	item from the list (i.e. it was not a member of the list, or another thread
//	of execution removed the item first).
//
//	May be called at interrupt time.

bool
LInterruptSafeList::Remove(
	LInterruptSafeListMember* inItem)
{
	bool	itemRemoved = false;

	if (inItem->mParentList == this) {

		itemRemoved = (::Dequeue((QElemPtr)&(inItem->mQueueElem), &mQueue) == noErr);

		if (itemRemoved) {

			inItem->mParentList = nil;
											// Notify iterators of the removal
			SIteratorQueueElem*	iterQElem = GetFirstIterator();
			while (iterQElem) {
				iterQElem->mIterator->ItemRemoved(inItem);
				iterQElem = iterQElem->mNextElem;
			}
		}
	}

	return itemRemoved;
}


// ---------------------------------------------------------------------------
//	¥ IsEmpty														  [public]
// ---------------------------------------------------------------------------
//	Returns true if there are no entries in this list.

bool
LInterruptSafeList::IsEmpty() const
{
	return (mQueue.qHead == nil);
}


// ===========================================================================
//	LInterruptSafeListIterator Class
// ===========================================================================
#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LInterruptSafeListIterator			Constructor				  [public]
// ---------------------------------------------------------------------------

LInterruptSafeListIterator::LInterruptSafeListIterator(
	const LInterruptSafeList& inList)

	: mList(inList)
{
	mQueueElem.mNextElem	= nil;
	mQueueElem.mIterator	= this;
	mListDied				= false;

	::Enqueue((QElemPtr) &mQueueElem, &(mList.mIteratorQueue));

	Reset();
}


// ---------------------------------------------------------------------------
//	¥ ~LInterruptSafeListIterator			Destructor				  [public]
// ---------------------------------------------------------------------------

LInterruptSafeListIterator::~LInterruptSafeListIterator()
{
	if (not mListDied) {
		::Dequeue((QElemPtr) &mQueueElem, &(mList.mIteratorQueue));
	}
}


// ---------------------------------------------------------------------------
//	¥ Reset															  [public]
// ---------------------------------------------------------------------------
//	Returns the iterator to the beginning of the list.
//	May be called at interrupt time.

void
LInterruptSafeListIterator::Reset()
{
	mCurrentEntry = nil;
	mNextEntry    = nil;

	if (not mListDied) {
		SMemberQueueElem*	firstElem = mList.GetFirstMember();
		if (firstElem != nil) {
			mNextEntry = firstElem->mListMember;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Current														  [public]
// ---------------------------------------------------------------------------
//	Sets outItem to the address of the current item. Returns true if the item
//	pointer is valid.
//
//	May be called at interrupt time.

bool
LInterruptSafeListIterator::Current(
	LInterruptSafeListMember* &outItem)
{
	LInterruptSafeListMember* localCurrent = mCurrentEntry;
	outItem = localCurrent;
	return (localCurrent != nil);
}


// ---------------------------------------------------------------------------
//	¥ Next															  [public]
// ---------------------------------------------------------------------------
//	Sets outItem to the address of the next item in this list.
//
//	Returns true if the next item pointer is valid,
//	false if the end of the list had been reached.
//
//	May be called at interrupt time.

bool
LInterruptSafeListIterator::Next(
	LInterruptSafeListMember* &outItem)
{
	LInterruptSafeListMember* localCurrent = nil;

	if (not mListDied) {
		localCurrent = mCurrentEntry = mNextEntry;
		if (localCurrent != nil) {
			mNextEntry = nil;
			SMemberQueueElem* nextQueue = localCurrent->mQueueElem.mNextElem;
			if (nextQueue != nil) {
				mNextEntry = nextQueue->mListMember;
			}
		}
	}

	outItem = localCurrent;
	return (localCurrent != nil);
}


// ---------------------------------------------------------------------------
//	¥ ListDied														 [private]
// ---------------------------------------------------------------------------
//	Should be called only by LInterruptSafeList when the list is deleted.

void
LInterruptSafeListIterator::ListDied()
{
	mListDied = true;
	mCurrentEntry = mNextEntry = nil;
}


// ---------------------------------------------------------------------------
//	¥ ItemAppended													 [private]
// ---------------------------------------------------------------------------
//	Should be called only by LInterruptSafeList when an item is added to
//	the list.

void
LInterruptSafeListIterator::ItemAppended(
	LInterruptSafeListMember* inItem)
{
	if (mNextEntry == nil) {
		mNextEntry = inItem;
	}
}


// ---------------------------------------------------------------------------
//	¥ ItemRemoved													 [private]
// ---------------------------------------------------------------------------
//	Should be called only by LInterruptSafeList when an item is removed from
//	the list.

void
LInterruptSafeListIterator::ItemRemoved(
	LInterruptSafeListMember* inItem)
{
	if (inItem == mCurrentEntry) {
		mCurrentEntry = nil;
	}

	if (inItem == mNextEntry) {
		SMemberQueueElem*	nextQueueElem = mNextEntry->mQueueElem.mNextElem;
		mNextEntry = nil;
		if (nextQueueElem != nil) {
			mNextEntry = nextQueueElem->mListMember;
		}
	}
}


// ===========================================================================
//	LInterruptSafeListMember Class
// ===========================================================================
#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LInterruptSafeListMember				Constructor				  [public]
// ---------------------------------------------------------------------------

LInterruptSafeListMember::LInterruptSafeListMember()
{
	mQueueElem.mNextElem	= nil;
	mQueueElem.mListMember	= this;
	mParentList				= nil;
}


// ---------------------------------------------------------------------------
//	¥ LInterruptSafeListMember				Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Copy is NOT queued to the parent list of the original

LInterruptSafeListMember::LInterruptSafeListMember(
	const LInterruptSafeListMember&	/* inOriginal */)
{
	mQueueElem.mNextElem	= nil;
	mQueueElem.mListMember	= this;
	mParentList				= nil;
}


// ---------------------------------------------------------------------------
//	¥ ~LInterruptSafeListMember				Destructor				  [public]
// ---------------------------------------------------------------------------

LInterruptSafeListMember::~LInterruptSafeListMember()
{
	if (mParentList != nil) {
		mParentList->Remove(this);
	}
}


PP_End_Namespace_PowerPlant
