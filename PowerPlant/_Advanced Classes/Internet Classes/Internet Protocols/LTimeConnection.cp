// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTimeConnection.cp		   	PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//
// Internet Time Protocol

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTimeConnection.h>
#include <LInternetAddress.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants
									// Jan 1, 1900 - 12:00:00 am in Mac time
const SInt32 kBaseInternetTime = -126144000;


// ---------------------------------------------------------------------------
//	¥ LTimeConnection						Constructor
// ---------------------------------------------------------------------------

LTimeConnection::LTimeConnection(LThread& inThread)
	: LInternetProtocol(inThread)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTimeConnection						Destructor
// ---------------------------------------------------------------------------

LTimeConnection::~LTimeConnection()
{
}


// ---------------------------------------------------------------------------
//	¥ GetTimeFrom
// ---------------------------------------------------------------------------

UInt32
LTimeConnection::GetTimeFrom(
	ConstStr255Param	inRemoteHost,
	UInt16				inPort)
{
	Connect(inRemoteHost, inPort);

	UInt32 netTime;
	WaitResponse(netTime);

	Disconnect();

	netTime += kBaseInternetTime;	// Convert 'Net seconds to Mac seconds

	return netTime;
}


// ---------------------------------------------------------------------------
//	¥ WaitResponse
// ---------------------------------------------------------------------------

void
LTimeConnection::WaitResponse(
	UInt32&		outNetTime)
{
	UInt32 dataSize = sizeof(UInt32);
	Boolean dataExpedited;

	mEndpoint->ReceiveData(&outNetTime, dataSize, dataExpedited, mTimeoutSeconds);
}


PP_End_Namespace_PowerPlant
