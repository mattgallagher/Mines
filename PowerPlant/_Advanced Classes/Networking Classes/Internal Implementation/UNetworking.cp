// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworking.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
// Utilities for network releated operations.
//
// StAsyncOperation -
//		Base class for handling asynchronous network operations needing to
//		block threads.
//
// LOperationListMember-
//		List member for pending async operations.

#include <UNetworking.h>
#include <PP_Networking.h>

PP_Begin_Namespace_PowerPlant

TInterruptSafeList<LOperationListMember*> *
	StAsyncOperation::sPendingOperations = new TInterruptSafeList<LOperationListMember*>;


// ===========================================================================
//	¥ StAsyncOperation						Constructor
// ===========================================================================

StAsyncOperation::StAsyncOperation()
{
	mThread = LThread::GetCurrentThread();
	mResult = noErr;
}


// ===========================================================================
//	¥ StAsyncOperation						Destructor
// ===========================================================================

StAsyncOperation::~StAsyncOperation()
{
}


// ---------------------------------------------------------------------------
//	¥ Int_AsyncResume
// ---------------------------------------------------------------------------
//	NOTE: This routine *might* be called at interrupt time
//
//	IMPORTANT: 	If you override this operation, be sure to override AbortOperation as
//				well if necessary.

void
StAsyncOperation::Int_AsyncResume(
	OSStatus	inResult)
{
	mResult = inResult;

	if (mThread != nil) {
		LThread::ThreadAsynchronousResume(mThread);
	}
	else {
		SignalStringLiteral_("StAsyncOperation::Int_AsyncResume - thread is nil");
	}
}


// ---------------------------------------------------------------------------
//	¥ AbortOperation
// ---------------------------------------------------------------------------

void
StAsyncOperation::AbortOperation()
{
	mResult = Abort_Error;

	if (mThread != nil) {
		LThread::ThreadAsynchronousResume(mThread);

	} else {
		SignalStringLiteral_("StAsyncOperation::AbortOperation - thread is nil");
	}
}


// ---------------------------------------------------------------------------
//	¥ WaitForResult
// ---------------------------------------------------------------------------

void
StAsyncOperation::WaitForResult()
{
	//Don't block the main thread
	if (mThread == LThread::GetMainThread()) {
		Throw_(threadProtocolErr);
	}

	mThread->Block();
	ThrowIfError_(mResult);
}


// ---------------------------------------------------------------------------
//	¥ GetThreadOperation
// ---------------------------------------------------------------------------

StAsyncOperation *
StAsyncOperation::GetThreadOperation(
	LThread * inThread)
{
	LOperationListMember*	nextElem;
	TInterruptSafeListIterator<LOperationListMember*>	iter(*sPendingOperations);

	while (iter.Next(nextElem)) {
		if (nextElem->mOperation->mThread == inThread) {
			return nextElem->mOperation;
		}
	}

	return nil;
}


// ===========================================================================
//	LOperationListMember
// ===========================================================================
#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LOperationListMember					Constructor
// ---------------------------------------------------------------------------

LOperationListMember::LOperationListMember(StAsyncOperation * inOperation)
{
	mOperation = inOperation;
	StAsyncOperation::sPendingOperations->Append(this);
}


// ---------------------------------------------------------------------------
//	¥ ~LOperationListMember					Destructor
// ---------------------------------------------------------------------------

LOperationListMember::~LOperationListMember()
{
}


PP_End_Namespace_PowerPlant
