// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleInternetThread.h		PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSimpleInternetThread
#define _H_LSimpleInternetThread
#pragma once

#include <LThread.h>
#include <LListener.h>
#include <LBroadcaster.h>
#include <TArray.h>
#include <LString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LInternetEventElem;

enum EInternetEvent {
	Internet_ThreadDied = 500,

	// FTP
	FTP_Exception,
	FTP_ConnectEvent,
	FTP_DisconnectEvent,
	FTP_GetFileEvent,
	FTP_PutFileEvent,
	FTP_GetFileAutoEvent,
	FTP_PutFileAutoEvent,
	FTP_ChangeDirEvent,
	FTP_ListFolderEvent,
	FTP_RenameFileEvent,

	// SMTP
	SMTP_Exception,
	SMTP_SendOneMessageEvent,

	// HTTP
	HTTP_Exception,
	HTTP_GetEvent,
	HTTP_HeadEvent,
	HTTP_PostEvent,
	HTTP_PutEvent
};

// ---------------------------------------------------------------------------
#pragma mark LSimpleInternetThread

class LSimpleInternetThread: 	public LThread,
								public LBroadcaster {
public:
						LSimpleInternetThread(LListener * inListener = nil, Boolean inAutoDelete = true);
						~LSimpleInternetThread();

	void				KillThread();
	void				QueueEvent(LInternetEventElem * inEvent);

protected:
	void				InitMembers();
	void *				Run();
	void				PurgeQueue();

	virtual void		RunPendingCommands() = 0;

	Boolean				mContinue;
	Boolean				mAutoDelete;

	TArray<LInternetEventElem*>	mEventArray;
};

// ---------------------------------------------------------------------------
#pragma mark -
#pragma mark LInternetEventElem

class LInternetEventElem {
public:
				LInternetEventElem(EInternetEvent	inEvent);

	virtual		~LInternetEventElem();

	EInternetEvent			mEvent;

	LStr255					mRemoteHost;
	char  					mUser[256];
	char		 			mPassword[256];
	SInt16					mRemotePort;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
