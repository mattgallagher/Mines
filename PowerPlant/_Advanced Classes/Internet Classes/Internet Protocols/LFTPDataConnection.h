// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFTPDataConnection.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LFTPDataConnection
#define _H_LFTPDataConnection
#pragma once

#include <LListener.h>
#include <LBroadcaster.h>
#include <LTCPEndpoint.h>
#include <LThread.h>
#include <LString.h>
#include <LFile.h>
#include <LDynamicBuffer.h>
#include <LFTPConnection.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

const UInt32 	kFTPDataDefaultSendSize = (1024 * 4);
const UInt32 	kFTPDataDefaultRcvSize = (1024 * 4);

// ---------------------------------------------------------------------------
#pragma mark LFTPDataConnection

class LFTPDataConnection :	public LListener,
							public LBroadcaster {

public:
							LFTPDataConnection(LFTPConnection * inMaster);
	virtual					~LFTPDataConnection();

	// connection configuration

	virtual void			MakeDataConnection(
									LStr255				ioHost,
									UInt32 *			ioHostNum,
									SInt16 *			ioPort,
									Boolean				usePassive = false);
	virtual void			Disconnect();
	virtual void			Abort();

	virtual Boolean			IsIdle();

	inline void				SetRemoteHost(ConstStr255Param inRemoteHost)
									{ mRemoteHostName = inRemoteHost; }
	inline void				SetRemotePort(UInt16 inRemotePort)
									{ mRemoteHostPort = inRemotePort; }

	virtual void			SetSendSize(UInt32 inSize)
									{ mSendSize = inSize; }
	inline UInt32			GetSendSize()
									{ return mSendSize; }
	virtual void			SetRcvSize(UInt32 inSize)
									{ mRcvSize = inSize; }
	inline UInt32			GetRcvSize()
									{ return mRcvSize; }

	virtual void			SendFile(
									LFile * inFile,
									SProgressMessage&	inProgressMsg);
	virtual void			SendFileMacBinary(
									LFile * inFile,
									SProgressMessage&	inProgressMsg);
	virtual void			RetrieveFile(
									LFile * inFile,
									SProgressMessage&	inProgressMsg);
	virtual void			RetrieveFileMacBinary(
									LFile *				inFile,
									SProgressMessage&	inProgressMsg);
	virtual void			RetrieveData(
									LDynamicBuffer* outBuffer,
									SProgressMessage&	inProgressMsg);
	virtual void			RetrieveData(
									LDynamicBuffer* outBuffer);
	virtual void			SendData(
									char*				inData,
									UInt32				totalSize,
									SProgressMessage&	inProgressMsg);

	virtual void			BroadcastProgress(
									MessageT				inMessage,
									SProgressMessage& 		inProgressMsg,
									Boolean					inForceBroadcast = false);

	virtual void			SetTimeoutSeconds(UInt32	inSeconds)
							{ mTimeoutSeconds = inSeconds; }

	virtual UInt32			GetTimeoutSeconds()
							{ return mTimeoutSeconds; }

protected:
	// event handling

	virtual void			WaitTillConnected();
	virtual void			ConnectThreadDone();
	virtual void			ListenToMessage(MessageT inMessage, void *ioParam);

	virtual void			HandleListen();

	LTCPEndpoint*			mEndpoint;					// our network endpoint object
	LStr255					mRemoteHostName;			// location of our remote machine
	UInt16					mRemoteHostPort;

	UInt32					mSendSize;					//size of send buffer
	UInt32					mRcvSize;					//size of rcv buffer

	LThread*				mThread;
	Boolean					mConnected;
	LFTPConnection *		mMasterConnection;

	UInt32					mTimeoutSeconds;

	friend class LConnectorThread;
};

// ---------------------------------------------------------------------------
#pragma mark -
#pragma mark LConnectorThread

// ===========================================================================
//		¥ LConnectorThread
// ===========================================================================
//	The thread that actually does the work of the connecting to an incomming
//	connection. This needs to run in a thread since we told the endpoint we were
//	threaded when we created it.

class LConnectorThread : public LThread {

public:
						LConnectorThread(
								LFTPDataConnection*	inServerMaster);
	virtual				~LConnectorThread();

protected:
	virtual void*		Run();

	LFTPDataConnection*	mMaster;

	friend class LFTPDataConnection;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
