// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSendQueue.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
// Queues data for asynchronous style data sending for MacTCP. Used primarily for
// OT style acked data transfers which are not native to MacTCP.

#include <LSendQueue.h>

PP_Begin_Namespace_PowerPlant

#define kMaxPendingRelease	 8192	//8K
#define kBlockingDataSize	65536	//64K


// ---------------------------------------------------------------------------
//	¥ LSendQueue							Constructor
// ---------------------------------------------------------------------------

LSendQueue::LSendQueue(
	LEndpoint* inEndpoint)

	: LThread(false, thread_DefaultStack, threadOption_Default, nil)
{
	mContinue		= true;
	mDeleteData		= false;
	mBusy			= false;
	mEndpointDead	= false;
	mThread			= nil;
	mReleaseWaiting	= 0;
	mEndpoint		= inEndpoint;

	SetMaxPendingRelease(kMaxPendingRelease);
	SetBlockingDataSize(kBlockingDataSize);
}

// ---------------------------------------------------------------------------
//	¥ LSendQueue							Destructor
// ---------------------------------------------------------------------------

LSendQueue::~LSendQueue()
{
	// Dispose all the data queues... we are going away so
	//	unsent data will just be killed.

	InternalClearReleaseQueue();

	LSendData*	nextData;
	TInterruptSafeListIterator<LSendData*>	sendIter(mWaitingQueue);
	while (sendIter.Next(nextData))
	{
		if (mDeleteData) {
			delete [] nextData->mData;
		}

		delete nextData;
	}

	TInterruptSafeListIterator<LSendData*> pendingIter(mPendingQueue);
	while (pendingIter.Next(nextData)) {

		if (mDeleteData) {
			delete [] nextData->mData;
		}

		delete nextData;
	}
}


// ---------------------------------------------------------------------------
//	¥ KillQueue
// ---------------------------------------------------------------------------

void
LSendQueue::KillQueue()
{
	if (mContinue) {
		InternalClearReleaseQueue();

		mContinue = false;
		//The endpoint might go away before our destructor is called
		//	so save the state of the ackSend flag.
		mDeleteData = !mEndpoint->IsAckingSends();
		mEndpointDead = true;
		LThread::ThreadAsynchronousResume(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ Append
// ---------------------------------------------------------------------------

void
LSendQueue::Append(LSendData* inItem)
{
	mWaitingQueue.Append(inItem);

	// Block the current thread if we have a lot of pending data to send
	// this is equivalent to the OT T_GODATA process
	if (WaitingDataSize() >= mBlockingDataSize) {
		mThread = LThread::GetCurrentThread();

		// Don't block the main thread
		if (mThread == LThread::GetMainThread()) {
			Throw_(threadProtocolErr);
		}

		mThread->Block();
	}

	// Kick things off if necessary
	if (mBusy == false) {
		mBusy = true;
		Int_InternalSend();
	}
}


// ---------------------------------------------------------------------------
//	¥ Run
// ---------------------------------------------------------------------------
// Run only handles releasing data and the ACK'ing notifications...
//	the data is sent via chained notifier routine (started via Append())

void*
LSendQueue::Run()
{
	while (mContinue) {
		Block();
		InternalClearReleaseQueue();
	}
	return nil;
}


// ---------------------------------------------------------------------------
//	¥ InternalClearReleaseQueue
// ---------------------------------------------------------------------------

void
LSendQueue::InternalClearReleaseQueue()
{
	LSendData*	data;
	TInterruptSafeListIterator<LSendData*>	iter(mReleaseQueue);
	while (iter.Next(data))
	{
		mReleaseQueue.Remove(data);

		Boolean killData;
		if (mEndpointDead)
			killData = mDeleteData;
		else
			killData = !mEndpoint->IsAckingSends();

		if ((!mEndpointDead) && mEndpoint->IsAckingSends())
			NotifyRelease(data);

		if (killData) {
			delete [] data->mData;
		}

		delete data;
	}
}


// ---------------------------------------------------------------------------
//	¥ WaitingDataSize
// ---------------------------------------------------------------------------
//	Returns the size of the pending data in the queue

UInt32
LSendQueue::WaitingDataSize()
{
	TInterruptSafeListIterator<LSendData*>	iter(mWaitingQueue);
	LSendData* data;

	UInt32 waitingDataSize = 0;

	while (iter.Next(data)) {
		waitingDataSize += data->mDataSize;
	}

	return waitingDataSize;
}


// ---------------------------------------------------------------------------
//	¥ Int_SendComplete
// ---------------------------------------------------------------------------
//	NOTE: This routine *might* be called at interrupt time

void
LSendQueue::Int_SendComplete()
{
	//Bailout if we have an error from the last completion
	if (mResult) {
		LThread::ThreadAsynchronousResume(this);
		return;
	}

	//Move pending queue to release queue
	TInterruptSafeListIterator<LSendData*> iter(mPendingQueue);
	LSendData* data;
	while (iter.Next(data)) {
		mPendingQueue.Remove(data);
		mReleaseQueue.Append(data);
		mReleaseWaiting += 	data->mDataSize;
	}

	if (!mWaitingQueue.IsEmpty()) {
		if (mReleaseWaiting >= GetMaxPendingRelease())
			LThread::ThreadAsynchronousResume(this);
		Int_InternalSend();
	} else {
		mBusy = false;
		LThread::ThreadAsynchronousResume(this);
	}
}


PP_End_Namespace_PowerPlant
