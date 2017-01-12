// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworkMessages.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UNetworkMessages
#define _H_UNetworkMessages
#pragma once

#include <LInterruptSafeList.h>
#include <LReentrantMemoryPool.h>
#include <PP_Networking.h>
#include <LInternetAddress.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LEndpoint;
class LInternetAddress;


// ===========================================================================
//		¥ LNetMessage
// ===========================================================================
//	LNetMessage objects are created and sent by endpoints and mappers
//	to inform you about events that occur asynchronously (such as
//	connections being closed or broken, or I/O requests completing).

class LNetMessage : 	public LInterruptSafeListMember,
						public LRMPObject
{

public:
						LNetMessage(
								MessageT	inMessageType,
								ResultT		inResultCode,
								LEndpoint*	inEndpoint);
	virtual				~LNetMessage();

	virtual MessageT	GetMessageType() const;
	virtual ResultT		GetResultCode() const;
	LEndpoint*			GetEndpoint() const;

protected:
	MessageT			mMessageType;
	ResultT				mResultCode;
	LEndpoint*			mEndpoint;

private:
						LNetMessage();		// do not use
						LNetMessage(LNetMessage&);

};


// ===========================================================================
//		¥ LMemoryReleasedMsg
// ===========================================================================
//	This message is returned by when the memory buffer for data sent
//	to a remote host has been released. (It is only sent when the
//	endpoint is asynchronous and AckSends() is turned on.)

class LMemoryReleasedMsg : public LNetMessage {

public:
						LMemoryReleasedMsg(
								MessageT		inMessageType,
								ResultT			inResultCode,
								void*			inDataBuffer,
								UInt32			inDataSize,
								LEndpoint*		inEndpoint);
	virtual				~LMemoryReleasedMsg();

	void*				GetDataBuffer() const;
	UInt32				GetDataSize() const;


	void*				mDataBuffer;
	UInt32				mDataSize;

private:
						LMemoryReleasedMsg();		// do not use
						LMemoryReleasedMsg(LMemoryReleasedMsg&);

};


// ===========================================================================
//		¥ LDataArrived
// ===========================================================================
//	This message is sent when data arrives via an asynchronous channel.

class LDataArrived : public LNetMessage {

public:
						LDataArrived(
								MessageT			inMessageType,
								ResultT				inResultCode,
								void*				inDataBuffer,
								UInt32				inDataSize,
								LInternetAddress*	inRemoteAddress,
								Boolean				inMustReleaseMemory,
								LEndpoint*			inEndpoint);
	virtual				~LDataArrived();

	void*				GetDataBuffer() const;
	UInt32				GetDataSize() const;
	LInternetAddress*	GetRemoteAddress() const;

protected:
	void*				mDataBuffer;
	UInt32				mDataSize;
	LInternetAddress*	mRemoteAddress;
	Boolean				mMustReleaseMemory;

private:
						LDataArrived();					// do not use
						LDataArrived(LDataArrived&);	// do not use

};

// ===========================================================================
//		¥ LICMPMessage
// ===========================================================================
//	This message is sent when data arrives via an asynchronous ICMP message.

class LICMPMessage : public LNetMessage {

public:
						LICMPMessage(
								MessageT			inMessageType,
								ResultT				inResultCode,
								SInt32				inErrorCode,
								UInt32&				inRemoteAddress,
								UInt16&				inRemotePort,
								LEndpoint*			inEndpoint);
	virtual				~LICMPMessage();

	SInt32				GetErrorCode() const;
	const LInternetAddress*	GetRemoteAddress() const;


protected:
	SInt32					mErrorCode;
	LInternetAddress		mRemoteAddress;

private:
						LICMPMessage();					// do not use
						LICMPMessage(LICMPMessage&);	// do not use

};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
