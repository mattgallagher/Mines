// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetProtocol.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInternetProtocol
#define _H_LInternetProtocol
#pragma once

#include <LBroadcaster.h>
#include <LTCPEndpoint.h>
#include <LThread.h>
#include <LString.h>
#include <LPeriodical.h>
#include <LListener.h>
#include <LDynamicBuffer.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const UInt32	kDefaultSendSize			= 4096;		// 1024 * 4
const UInt32	kDefaultRcvSize				= 1024;
const UInt32	kDefaultMinBroadcastTicks	= 60;		// Once a second

// ---------------------------------------------------------------------------

class LInternetProtocol;

enum {
	msg_OpeningConnection		= 601,
	msg_Connected				= 602,
	msg_Disconnected			= 603,
	msg_SendingData				= 604,
	msg_ReceivingData			= 605,
	msg_SendItemSuccess			= 606,
	msg_SendItemFailed			= 607,
	msg_RetrieveItemSuccess		= 608,
	msg_RetrieveItemFailed		= 609,
	msg_DeleteItemSuccess		= 610,
	msg_DeleteItemFailed		= 611,
	msg_SendingItem				= 612,
	msg_ReceivingItem			= 613,
	msg_ClosingConnection		= 614,
	msg_OpeningDataConnection	= 615,
	msg_ClosingDataConnection	= 616,
	msg_DataConnected			= 617,
	msg_DataDisconnected		= 618,
	msg_UnexpectedDisconnect	= 619
};

struct SProgressMessage {
	LInternetProtocol *		theProtocol;
	LStr255					currentItem;
	UInt32					totalItems;
	UInt32					completedItems;
	UInt32					totalBytes;
	UInt32					completedBytes;
};

// ---------------------------------------------------------------------------

class LInternetProtocol :	public LListener,
							public LBroadcaster {

public:
							LInternetProtocol(LThread& inThread);
							~LInternetProtocol();

	virtual void			ResetMembers();

	// connection configuration

	void					Connect(
									ConstStringPtr		inRemoteHost,
									UInt16				inRemotePort);
	virtual void			Disconnect();
	virtual Boolean			IsIdle();

	virtual void			Abort();

	inline void				SetThread(LThread& inThread) {mThread = &inThread;}
	virtual void			SetSendSize(UInt32 inSize)
									{ mSendSize = inSize; }
	inline UInt32			GetSendSize()
									{ return mSendSize; }
	virtual void			SetRcvSize(UInt32 inSize)
									{ mRcvSize = inSize; }
	inline UInt32			GetRcvSize()
									{ return mRcvSize; }

	virtual void			BroadcastProgress(
									MessageT				inMessage,
									SProgressMessage& 		inProgressMsg,
									Boolean					inForceBroadcast = false);
	virtual void			SendData(
									const char*			inData,
									UInt32				inTotalSize,
									SProgressMessage&	inProgressMsg);
	virtual void			SendData(
									LDynamicBuffer *	inBuffer,
									SProgressMessage&	inProgressMsg);

	virtual void			SetTimeoutSeconds(UInt32	inSeconds)
							{ mTimeoutSeconds = inSeconds; }

	virtual UInt32			GetTimeoutSeconds()
							{ return mTimeoutSeconds; }

	virtual void			SetMinBroadcastTicks(UInt32	inTicks)
							{ mMinBroadcastTicks = inTicks; }

	virtual UInt32			GetMinBroadcastTicks()
							{ return mMinBroadcastTicks; }

	virtual LTCPEndpoint*	GetEndpoint()
							{ return mEndpoint; }

protected:
	virtual void			ListenToMessage(MessageT inMessage, void *ioParam);

	virtual void			HandleDisconnect();
	virtual void			StartDisconnect();
	virtual void			HandleIncomingData();
	virtual void			HandleListen();

	LTCPEndpoint*			mEndpoint;					// our network endpoint object

	LThread*				mThread;

	Boolean					mDisconnectReceived;		// true if a T_DISCONNECT or T_ORDREL
														//		event has occurred
	Boolean					mStartDisconnect;
	UInt32					mSendSize;					//size of send buffer
	UInt32					mRcvSize;					//size of rcv buffer
	UInt32					mProgressLastTicks;
	UInt32					mTimeoutSeconds;

	UInt32					mMinBroadcastTicks;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
