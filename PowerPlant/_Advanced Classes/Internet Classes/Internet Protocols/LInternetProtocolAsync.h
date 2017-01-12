// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetProtocolAsync.h	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInternetProtocolAsync
#define _H_LInternetProtocolAsync
#pragma once

#include <LBroadcaster.h>
#include <LListener.h>
#include <LTCPEndpoint.h>
#include <LThread.h>
#include <LString.h>
#include <LDynamicBuffer.h>
#include <LInternetAsyncSendQueue.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const UInt32	 kDefaultAysncSendSize	= 4096;		// 1024 * 4
const UInt32	 kDefaultAysncRcvSize	= 1024;

class LAsyncReceiveThread;
class LAsyncProtocolThread;

// ---------------------------------------------------------------------------
#pragma mark LInternetProtocolAsync

class LInternetProtocolAsync {

public:
							LInternetProtocolAsync();
	virtual					~LInternetProtocolAsync();


	void					Connect(
									ConstStringPtr		inRemoteHost,
									UInt16				inRemotePort);
	virtual void			Disconnect();
	virtual Boolean			IsIdle();

	// connection configuration

	virtual void			Abort();

	virtual void			SetSendSize(UInt32 inSize)
									{ mSendSize = inSize; }
	inline UInt32			GetSendSize()
									{ return mSendSize; }
	virtual void			SetRcvSize(UInt32 inSize)
									{ mRcvSize = inSize; }
	inline UInt32			GetRcvSize()
									{ return mRcvSize; }

	virtual UInt32			SendData(
									char*				inData,
									UInt32				totalSize);

	virtual LInternetAddress *	GetLocalAddress();


	virtual void			SetTimeoutSeconds(UInt32	inSeconds)
							{ mTimeoutSeconds = inSeconds; }

	virtual UInt32			GetTimeoutSeconds()
							{ return mTimeoutSeconds; }

	virtual char *			GetRemoteHost()
							{ return mRemoteHost; }

	virtual UInt16			GetRemotePort()
							{ return mRemotePort; }

	virtual LAsyncProtocolThread*	GetEndpointThread()
							{ return mEndpointThread; }
protected:
	// event handling

	virtual void			HandleConnect();
	virtual void			HandleDisconnect();
	virtual void			HandleDataSent(UInt32 inReference);
	virtual void			HandleIncomingData(const char * theData, UInt32 theLength);
	virtual void			HandleListen();

	LAsyncProtocolThread*	mEndpointThread;

	virtual void			ThreadDied();

	UInt32					mSendSize;					//size of send buffer(s)
	UInt32					mRcvSize;					//size of rcv buffer
	UInt32					mTimeoutSeconds;

	char 					mRemoteHost[256];
	UInt16					mRemotePort;
	LInternetAddress *		mLocalAddress;

	friend class LAsyncProtocolThread;
};

// ---------------------------------------------------------------------------
#pragma mark -
#pragma mark LAsyncProtocolThread

class LAsyncProtocolThread : public LThread,
								public LListener
{
public:
							LAsyncProtocolThread(
										LInternetProtocolAsync * inSuperConnection,
										ConstStringPtr		inRemoteHost,
										UInt16				inRemotePort);
							~LAsyncProtocolThread();

	virtual void			ListenToMessage(MessageT inMessage, void* ioParam);

	virtual void			Abort();
	void					KillThread();
	virtual UInt32			QueueOutgoingData(char * inData, UInt32 inLength);
	virtual Boolean			IsIdle();

protected:
	virtual void			SendData(LDynamicBuffer * dataBuff);
	virtual void			DataWaiting();

	virtual void*			Run();

	virtual void			InternalConnect();
	virtual void			InternalDisconnect();

	virtual void			HandleConnect();
	virtual void			HandleDisconnect();
	virtual void			HandleDataSent(UInt32 inReference);
	virtual void			HandleIncomingData(const char * theData, UInt32 theLength);
	virtual void			HandleListen();

	LInternetAddress *		mRemoteAddress;
	LInternetProtocolAsync*	mSuperConnection;
	LTCPEndpoint *			mEndpoint;
	Boolean					mContinue;
	LInternetAsyncSendQueue	mDataQueue;
	Boolean					mDataWaiting;

friend class LInternetProtocolAsync;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
