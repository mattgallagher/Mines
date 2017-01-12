// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworkMessages.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	LNetMessage objects are created and sent by endpoints and mappers
//	to inform you about events that occur asynchronously (such as
//	connections being closed or broken, or I/O requests completing).

#include <UNetworkMessages.h>
#include <LEndpoint.h>
#include <LInternetAddress.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LNetMessage ===

// ---------------------------------------------------------------------------
//		¥ LNetMessage
// ---------------------------------------------------------------------------
//	Constructor

LNetMessage::LNetMessage(
	MessageT	inMessageType,
	ResultT		inResultCode,
	LEndpoint*	inEndpoint)
{
	mMessageType = inMessageType;
	mResultCode = inResultCode;
	mEndpoint = inEndpoint;
}


// ---------------------------------------------------------------------------
//		¥ ~LNetMessage
// ---------------------------------------------------------------------------
//	Destructor

LNetMessage::~LNetMessage()
{
}


// ---------------------------------------------------------------------------
//		¥ GetMessageType
// ---------------------------------------------------------------------------
//	Return the class ID of the message. This can be used as a Òcheap RTTIÓ
//	to downcast to the appropriate message type.

MessageT
LNetMessage::GetMessageType() const
{
	return mMessageType;
}


// ---------------------------------------------------------------------------
//		¥ GetResultCode
// ---------------------------------------------------------------------------
//	Return the result code of an operation which corresponds to this message.

ResultT
LNetMessage::GetResultCode() const
{
	return mResultCode;
}


// ---------------------------------------------------------------------------
//		¥ GetEndpoint
// ---------------------------------------------------------------------------
//	Return the endpoint associated with this message.

LEndpoint*
LNetMessage::GetEndpoint() const
{
	return mEndpoint;
}


// ===========================================================================

#pragma mark -
#pragma mark === LMemoryReleasedMsg ===

// ---------------------------------------------------------------------------
//		¥ LMemoryReleasedMsg
// ---------------------------------------------------------------------------
//	Constructor

LMemoryReleasedMsg::LMemoryReleasedMsg(
	MessageT		inMessageType,
	ResultT			inResultCode,
	void*			inDataBuffer,
	UInt32			inDataSize,
	LEndpoint*		inEndpoint)
		: LNetMessage(inMessageType, inResultCode, inEndpoint)
{
	mDataBuffer = inDataBuffer;
	mDataSize = inDataSize;
}


// ---------------------------------------------------------------------------
//		¥ ~LMemoryReleasedMsg
// ---------------------------------------------------------------------------
//	Destructor

LMemoryReleasedMsg::~LMemoryReleasedMsg()
{
}


// ---------------------------------------------------------------------------
//		¥ GetDataBuffer
// ---------------------------------------------------------------------------
//	Get the start address of the buffer which has been released.

void*
LMemoryReleasedMsg::GetDataBuffer() const
{
	return mDataBuffer;
}


// ---------------------------------------------------------------------------
//		¥ GetDataSize
// ---------------------------------------------------------------------------
//	Get the size of the buffer which has been released.

UInt32
LMemoryReleasedMsg::GetDataSize() const
{
	return mDataSize;
}


// ===========================================================================

#pragma mark -
#pragma mark === LDataArrived ===

// ---------------------------------------------------------------------------
//		¥ LDataArrived
// ---------------------------------------------------------------------------
//	Constructor

LDataArrived::LDataArrived(
	MessageT			inMessageType,
	ResultT				inResultCode,
	void*				inDataBuffer,
	UInt32				inDataSize,
	LInternetAddress*	inRemoteAddress,
	Boolean				inMustReleaseMemory,
	LEndpoint*			inEndpoint)
		: LNetMessage(inMessageType, inResultCode, inEndpoint)
{
	mDataBuffer = inDataBuffer;
	mDataSize = inDataSize;
	mRemoteAddress = inRemoteAddress;
	mMustReleaseMemory = inMustReleaseMemory;
}


// ---------------------------------------------------------------------------
//		¥ ~LDataArrived
// ---------------------------------------------------------------------------
//	Destructor

LDataArrived::~LDataArrived()
{
}


// ---------------------------------------------------------------------------
//		¥ GetDataBuffer
// ---------------------------------------------------------------------------

void*
LDataArrived::GetDataBuffer() const
{
	return mDataBuffer;
}


// ---------------------------------------------------------------------------
//		¥ GetDataSize
// ---------------------------------------------------------------------------

UInt32
LDataArrived::GetDataSize() const
{
	return mDataSize;
}


// ---------------------------------------------------------------------------
//		¥ GetRemoteAddress
// ---------------------------------------------------------------------------

LInternetAddress*
LDataArrived::GetRemoteAddress() const
{
	return mRemoteAddress;
}

// ===========================================================================

#pragma mark -
#pragma mark === LICMPMessage ===

// ---------------------------------------------------------------------------
//		¥ LICMPMessage
// ---------------------------------------------------------------------------
//	Constructor

LICMPMessage::LICMPMessage(
	MessageT			inMessageType,
	ResultT				inResultCode,
	SInt32				inErrorCode,
	UInt32&				inRemoteAddress,
	UInt16&				inRemotePort,
	LEndpoint*			inEndpoint)
		:LNetMessage(inMessageType, inResultCode, inEndpoint)
{
	mErrorCode = inErrorCode;
	mRemoteAddress.SetIPAddress(inRemoteAddress);
	mRemoteAddress.SetHostPort(inRemotePort);
}

// ---------------------------------------------------------------------------
//		¥ ~LICMPMessage
// ---------------------------------------------------------------------------
//	Destructor

LICMPMessage::~LICMPMessage()
{
}

// ---------------------------------------------------------------------------
//		¥ GetErrorCode
// ---------------------------------------------------------------------------

SInt32
LICMPMessage::GetErrorCode() const
{
	return mErrorCode;
}


// ---------------------------------------------------------------------------
//		¥ GetRemoteAddress
// ---------------------------------------------------------------------------

const LInternetAddress*
LICMPMessage::GetRemoteAddress() const
{
	return &mRemoteAddress;
}

PP_End_Namespace_PowerPlant
