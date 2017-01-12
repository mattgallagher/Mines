// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTCPEndpoint.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	This class represents a TCP/IP style network connection. An
//	instance of this class should be created as a representation of
//	the local side the network connection.

#include <LTCPEndpoint.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant

// ===========================================================================

#pragma mark === LTCPEndpoint ===

// ---------------------------------------------------------------------------
//		¥ LEndpoint						[protected]
// ---------------------------------------------------------------------------
//	Constructor

LTCPEndpoint::LTCPEndpoint()
{
}

// ---------------------------------------------------------------------------
//		¥ ~LEndpoint
// ---------------------------------------------------------------------------
//	Destructor

LTCPEndpoint::~LTCPEndpoint()
{
}


// ---------------------------------------------------------------------------
//		¥ SendPStr
// ---------------------------------------------------------------------------
// Utility method for SendData

void
LTCPEndpoint::SendPStr(ConstStringPtr inString)
{
	SendData((void *) &inString[1], (UInt32)inString[0]);
}

// ---------------------------------------------------------------------------
//		¥ SendCStr
// ---------------------------------------------------------------------------
// Utility method for SendData

void
LTCPEndpoint::SendCStr(char* inString)
{
	UInt32	length = 0;

	while (inString[length]) {
		length++;
	}

	SendData(inString, length);
}

// ---------------------------------------------------------------------------
//		¥ SendHandle
// ---------------------------------------------------------------------------
// Utility method for SendData

void
LTCPEndpoint::SendHandle(Handle inHandle)
{
	StHandleLocker locked(inHandle);
	SendData(*inHandle, (UInt32) ::GetHandleSize(inHandle));
}

// ---------------------------------------------------------------------------
//		¥ SendPtr
// ---------------------------------------------------------------------------
// Utility method for SendData

void
LTCPEndpoint::SendPtr(Ptr inPtr)
{
	SendData(inPtr, (UInt32) ::GetPtrSize(inPtr));
}

// ---------------------------------------------------------------------------
//		¥ ReceiveChar
// ---------------------------------------------------------------------------

Boolean
LTCPEndpoint::ReceiveChar(
		char&		outChar,
		UInt32		inTimeoutSeconds)
{
	Boolean tempExpedited;
	UInt32 ioDataSize = sizeof(char);

	ReceiveData(&outChar, ioDataSize, tempExpedited, inTimeoutSeconds);

	return (ioDataSize == sizeof(char));
}

// ---------------------------------------------------------------------------
//		¥ ReceiveLine
// ---------------------------------------------------------------------------
// If inUseLF == true then we look for a LF instead of CR to terminate the line

Boolean
LTCPEndpoint::ReceiveLine(
		char *		outString,
		UInt32&		ioDataSize,
		UInt32		inTimeoutSeconds,
		Boolean		inUseLF)
{
	Boolean tempExpedited;

	if (inUseLF)
		return ReceiveDataUntilMatch(outString, ioDataSize, tempExpedited, inTimeoutSeconds, 0x0A);
	else
		return ReceiveDataUntilMatch(outString, ioDataSize, tempExpedited, inTimeoutSeconds);
}


// ---------------------------------------------------------------------------
//		¥ ReceiveDataUntilMatch
// ---------------------------------------------------------------------------

Boolean
LTCPEndpoint::ReceiveDataUntilMatch(
		void*					outDataBuffer,
		UInt32&					ioDataSize,
		Boolean&				outExpedited,
		UInt32					inTimeoutSeconds,
		char					inMatchChar)
{
	Boolean haveMatch = false;
	UInt32 rcvCount = 0;
	char currChar;
	UInt32 startSeconds;
	::GetDateTime(&startSeconds);

	while (rcvCount < ioDataSize) {
		UInt32 currentTimeout = inTimeoutSeconds;
		UInt32 localDataSize = sizeof(char);

		//Special case Timeout_None so that we will never timeout
		//	if passed this value.
		if (currentTimeout != Timeout_None) {
			//To keep validity of inTimeoutSeconds over multiple calls to
			//	RecieveData, we adjust and test the timeout value for
			//	each call.
			UInt32 sencondsOffset;
			::GetDateTime(&sencondsOffset);
			sencondsOffset -= startSeconds;
			if (sencondsOffset >= inTimeoutSeconds) {
				Throw_(Timeout_Error);
			}
			currentTimeout = inTimeoutSeconds - sencondsOffset;
		}

		ReceiveData(&currChar, localDataSize, outExpedited, currentTimeout);

		if (localDataSize == sizeof(char)) {
			((char*)outDataBuffer)[rcvCount++] = currChar;
			if (currChar == inMatchChar) {
				haveMatch = true;
				break;
			}
		} else {
			break;
		}
	}

	ioDataSize = rcvCount;
	return haveMatch;
}

PP_End_Namespace_PowerPlant
