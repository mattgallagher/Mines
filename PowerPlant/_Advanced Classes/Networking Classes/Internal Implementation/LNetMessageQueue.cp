// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LNetMessageQueue.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Queue of messages, typically generated at interrupt time, that will be
//	broadcast to registered listeners at primary task time via a periodical.

#include <LNetMessageQueue.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LNetMessageQueue						Constructor
// ---------------------------------------------------------------------------

LNetMessageQueue::LNetMessageQueue(
	LBroadcaster&	inBroadcaster)

	: mBroadcaster(&inBroadcaster)
{
	::MacGetCurrentProcess(&mPSN);
	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ LNetMessageQueue						Destructor
// ---------------------------------------------------------------------------

LNetMessageQueue::~LNetMessageQueue()
{
	LInterruptSafeListMember*	message;
	LInterruptSafeListIterator	iter(*this);
	while (iter.Next(message)) {
		delete message;
	}
}


// ---------------------------------------------------------------------------
//	¥ SpendTime
// ---------------------------------------------------------------------------
//

void
LNetMessageQueue::SpendTime(const EventRecord & /* inMacEvent */)
{
	if (IsEmpty()) {
		return;
	}

	// Broadcast the whole list for effeciency...
	// otherwise we'd have a lot of delays between broadcasts

	LInterruptSafeListMember*	p;
	LInterruptSafeListIterator	iter(*this);
	while (iter.Next(p))
	{
		LNetMessage* nextMessage = static_cast<LNetMessage*>(p);

		try {
			mBroadcaster->BroadcastMessage(nextMessage->GetMessageType(), nextMessage);
		}

		catch(...) { }

		// Get the current item again to make sure it wasn't already deleted as a
		//	result of the broadcast message.
		//	This avoids double-deletion problems should the endpoint be deleted
		//	by the broadcast message.

		if (iter.Current(p)) {
			delete nextMessage;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ Append
// ---------------------------------------------------------------------------

void
LNetMessageQueue::Append(
	LInterruptSafeListMember* inItem)
{
	LInterruptSafeList::Append(inItem);
	::WakeUpProcess(&mPSN);
}


PP_End_Namespace_PowerPlant
