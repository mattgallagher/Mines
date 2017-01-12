// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworking.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UNetworking
#define _H_UNetworking
#pragma once

#include <LThread.h>
#include <LInterruptSafeList.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LOperationListMember;

// ===========================================================================
//	¥ StAsyncOperation
// ===========================================================================
//	A helper class used by the networking classes to delay thread execution
//	while an asynchronous operation executes.

class StAsyncOperation {

public:
							StAsyncOperation();
	virtual					~StAsyncOperation();

	virtual void			Int_AsyncResume(OSStatus	inResult);
	virtual void			AbortOperation();
	void					WaitForResult();

	static StAsyncOperation *		GetThreadOperation(LThread * inThread);

protected:

	LThread*				mThread;				// thread that's waiting for async operation
	OSStatus				mResult;				// result of async operation

private:
	static TInterruptSafeList<LOperationListMember*> *	sPendingOperations;

	friend class LOperationListMember;
};


// ===========================================================================
//	LOperationListMember
// ===========================================================================

class LOperationListMember : public LInterruptSafeListMember{
public:
							LOperationListMember(StAsyncOperation * inOperation);
	virtual					~LOperationListMember();

	StAsyncOperation *		mOperation;

	friend class StAsyncOperation;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
