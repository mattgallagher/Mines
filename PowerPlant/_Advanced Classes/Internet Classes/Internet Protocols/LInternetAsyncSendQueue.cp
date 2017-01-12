// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetAsyncSendQueue.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for handling a queue of IRC messages. This object takes
//	responsibility for deleting the messages passed to it.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LInternetAsyncSendQueue.h>
#include <LArrayIterator.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LInternetAsyncSendQueue				Default Constructor
// ---------------------------------------------------------------------------
//	Construct an empty List of pointers to messages

LInternetAsyncSendQueue::LInternetAsyncSendQueue()

	: LArray(sizeof(LDynamicBuffer *))
{
}


// ---------------------------------------------------------------------------
//	¥ ~LInternetAsyncSendQueue				Destructor
// ---------------------------------------------------------------------------

LInternetAsyncSendQueue::~LInternetAsyncSendQueue()
{
	LDynamicBuffer * currBuff;
	LArrayIterator iter(*this);
	while(iter.Next(&currBuff)) {
		delete currBuff;
	}
}


// ---------------------------------------------------------------------------
//	¥ Enqueue
// ---------------------------------------------------------------------------
//	Adds item to end of queue
//
//	inItem is a pointer to the item data. The Array makes and stores
//	a copy of the item data.
//

void
LInternetAsyncSendQueue::Enqueue(
	const LDynamicBuffer** inItem)			// Pointer to the item
{
	InsertItemsAt(1, LArray::index_Last, inItem);
}


// ---------------------------------------------------------------------------
//	¥ Dequeue
// ---------------------------------------------------------------------------
//	Returns the first item in the queue and deletes it from the queue
//
//	Returns false if queue is empty

Boolean
LInternetAsyncSendQueue::Dequeue(
	LDynamicBuffer** outItem)			// Pointer to the item
{
	if (FetchItemAt(1, outItem)) {
		RemoveItemsAt(1, 1);
		return true;
	} else {
		return false;
	}
}


// ---------------------------------------------------------------------------
//	¥ ResetQueue
// ---------------------------------------------------------------------------
//	Removes all items from queue and deletes them.
//

void
LInternetAsyncSendQueue::ResetQueue()
{
	RemoveItemsAt(LArray::index_Last, 1);
}


PP_End_Namespace_PowerPlant
