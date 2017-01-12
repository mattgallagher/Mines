// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleSMTPThread.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSimpleSMTPThread
#define _H_LSimpleSMTPThread
#pragma once

#include <LSimpleInternetThread.h>
#include <LSMTPConnection.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================

class LSimpleSMTPThread : public LSimpleInternetThread {
public:
						LSimpleSMTPThread(LListener * inListener = nil, Boolean inAutoDelete = true);
						~LSimpleSMTPThread();

	LSMTPResponse		GetLastResponse() { return mLastResponse; }
	LSMTPConnection*	GetConnection() { return mConnection; }

protected:

	virtual void		RunPendingCommands();

	LSMTPConnection*	mConnection;
	LSMTPResponse		mLastResponse;
};


// ===========================================================================

class LSMTPEventElem : public LInternetEventElem {
public:
				LSMTPEventElem(EInternetEvent inEvent);

	virtual		~LSMTPEventElem();

	LMailMessage* 	mMailMessage;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
