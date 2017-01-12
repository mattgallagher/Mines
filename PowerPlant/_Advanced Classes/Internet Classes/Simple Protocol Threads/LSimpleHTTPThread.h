// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleHTTPThread.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSimpleHTTPThread
#define _H_LSimpleHTTPThread
#pragma once

#include <LSimpleInternetThread.h>
#include <LHTTPConnection.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================

class LSimpleHTTPThread: public LSimpleInternetThread {
public:
						LSimpleHTTPThread(LListener * inListener = nil, Boolean inAutoDelete = true);
						~LSimpleHTTPThread();

	LHTTPResponse		GetLastResponse() { return mLastResponse; }
	LHTTPConnection*	GetConnection() { return mConnection; }

protected:

	virtual void		RunPendingCommands();

	LHTTPConnection*	mConnection;
	LHTTPResponse		mLastResponse;
};


// ===========================================================================

class LHTTPEventElem : public LInternetEventElem {
public:
				LHTTPEventElem(EInternetEvent	inEvent);

	virtual		~LHTTPEventElem();


	LStr255			mRequest;
	LHTTPMessage* 	mMessage;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
