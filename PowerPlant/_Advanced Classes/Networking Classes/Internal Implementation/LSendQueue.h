// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSendQueue.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSendQueue
#define _H_LSendQueue
#pragma once

#include <LInterruptSafeList.h>
#include <LEndpoint.h>
#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LSendData;

// ===========================================================================
#pragma mark === LSendQueue ===

class LSendQueue :	public LThread
{

public:
							LSendQueue(LEndpoint * inEndpoint);
							~LSendQueue();

	virtual void			KillQueue();
	virtual void			Append(LSendData* inItem);

	virtual	void*			Run();
	virtual Boolean			IsBusy() { return mBusy; }

	virtual UInt32			WaitingDataSize();

	virtual UInt32			GetMaxPendingRelease() const
							{ 	return mMaxPendingRelease; }

	virtual UInt32			GetBlockingDataSize() const
							{ 	return mBlockingDataSize; }

	virtual void			SetMaxPendingRelease(UInt32 inReleaseSize)
							{ 	mMaxPendingRelease = inReleaseSize; }

	virtual void			SetBlockingDataSize(UInt32 inReleaseSize)
							{ 	mBlockingDataSize = inReleaseSize; }

protected:
	virtual void			Int_InternalSend() = 0;
	virtual void			NotifyRelease(LSendData* inData) = 0;

	virtual void			Int_SendComplete();
	virtual void			InternalClearReleaseQueue();

	TInterruptSafeList<LSendData*>		mWaitingQueue;		// data waiting to be sent
	TInterruptSafeList<LSendData*>		mPendingQueue;		// data waiting completion indication
	TInterruptSafeList<LSendData*>		mReleaseQueue;		// data waiting to be released

	Boolean					mContinue;
	Boolean					mDeleteData;
	Boolean					mEndpointDead;
	Boolean					mBusy;
	UInt32					mReleaseWaiting;
	LEndpoint *				mEndpoint;

	LThread *				mThread;

	UInt32					mMaxPendingRelease;
	UInt32					mBlockingDataSize;
};

// ===========================================================================
#pragma mark -
#pragma mark === LSendData ===

class LSendData : public LInterruptSafeListMember
{
public:
							LSendData(
								void*					inData,
								UInt32					inDataSize)
							{
								mData = (char*) inData;
								mDataSize = inDataSize;
							}

							~LSendData() { }

	char*					mData;
	UInt32					mDataSize;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
