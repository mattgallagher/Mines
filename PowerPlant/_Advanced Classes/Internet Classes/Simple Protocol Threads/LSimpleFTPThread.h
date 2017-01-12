// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleFTPThread.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSimpleFTPThread
#define _H_LSimpleFTPThread
#pragma once

#include <LSimpleInternetThread.h>
#include <LFTPConnection.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LSimpleFTPThread ===

class LSimpleFTPThread : public LSimpleInternetThread {
public:
						LSimpleFTPThread(LListener * inListener = nil, Boolean inAutoDelete = true);
						~LSimpleFTPThread();

	LFTPResponse		GetLastResponse() { return mLastResponse; }
	LFTPConnection *	GetConnection() { return mConnection; }

protected:

	virtual void		RunPendingCommands();

	LFTPConnection *	mConnection;
	LFTPResponse		mLastResponse;
};

// ===========================================================================

#pragma mark -
#pragma mark === LFTPEventElem ===

class LFTPEventElem : public LInternetEventElem {
public:
				LFTPEventElem(EInternetEvent	inEvent);

	virtual		~LFTPEventElem();

	char		 			mAccount[256];
	char		 			mOriginalFileName[256];
	LFile *	 				mFile;
	char *					mReturnFileName;
	Boolean 				mCreateUnique;
	Boolean					mUsePassive;
	EFTPTransferType		mXferType;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
