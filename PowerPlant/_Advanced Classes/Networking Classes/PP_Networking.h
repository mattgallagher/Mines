// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Networking.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_PP_Networking
#define _H_PP_Networking
#pragma once

#include <PP_Prefix.h>
#include <OpenTransport.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

const UInt32 Timeout_None = 0xFFFFFFFF;


enum { Notifier_Default = nil };


enum {	Timeout_Error = kPRIVATEEVENT + 100,
		Abort_Error =  kPRIVATEEVENT + 101,
		Disconnect_Error = kPRIVATEEVENT + 102,
		OrderlyDisconnect_Error = kPRIVATEEVENT + 103};

inline bool IsDisconnectError(ExceptionCode inErr)
	{
		return ( (inErr == Disconnect_Error) || (inErr == OrderlyDisconnect_Error) );
	}

// ---------------------------------------------------------------------------
//	Operation result codes (basically a 32-bit OSErr)
// ---------------------------------------------------------------------------
//	Note: Error codes are mapped to Open Transport error numbers where
//	possible.

typedef SInt32 ResultT;


// ---------------------------------------------------------------------------
//	Connection status information returned by LEndpoint::GetState()
// ---------------------------------------------------------------------------
//	Note: These are identical to the connection status values returned by
//	OTGetEndpointState and OTSync.

enum EEndpointState {
	endpoint_Uninitialized = 0,
	endpoint_Unbound = 1,
	endpoint_Idle = 2,
	endpoint_OutConnect = 3,
	endpoint_InConnect = 4,
	endpoint_Connected = 5,
	endpoint_OutDisconnect = 6,
	endpoint_InDisconnect = 7,
	endpoint_UnknownState = 8
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
