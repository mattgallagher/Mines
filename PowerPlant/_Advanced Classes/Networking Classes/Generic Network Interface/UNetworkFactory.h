// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNetworkFactory.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UNetworkFactory
#define _H_UNetworkFactory
#pragma once

// ===========================================================================
//	¥ Conditional compilation
// ===========================================================================

// ---------------------------------------------------------------------------
//	Debugging using OT Session Watcher tool

#ifdef DEBUG_OTSESSIONWATCHER
     #define kTCPDebugName "OTSessionWatcher,tcp"
     #define kUDPDebugName "OTSessionWatcher,udp"
#else
     #define kTCPDebugName kTCPName     // "tcp" from OpenTptInternet.h
     #define kUDPDebugName kUDPName
#endif


// ---------------------------------------------------------------------------
//	MacTCP is no longer supported

#ifndef USE_MACTCP
	#define		USE_MACTCP		0
#endif

#if USE_MACTCP
	#error "MacTCP is no longer supported by PowerPlant"
#endif


// ---------------------------------------------------------------------------
//	Set to zero value to disallow use of OpenTransport calls

#ifndef USE_OPENTPT
	#define		USE_OPENTPT 1
#endif

#include <LTCPEndpoint.h>
#include <LUDPEndpoint.h>
#include <LInternetMapper.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ UNetworkFactory
// ===========================================================================
//	Use this class to create network endpoints and mappers. It will choose
//	the best configuration (i.e. MacTCP or OpenTransport) given the system
//	software that is installed on the host machine.

class UNetworkFactory {

public:
	static LTCPEndpoint*		CreateTCPEndpoint(
									UInt32		inReceiveBufferSize = 0);
	static LUDPEndpoint*		CreateUDPEndpoint();
	static LInternetMapper*		CreateInternetMapper();

	static Boolean				HasTCP();
#if USE_OPENTPT
	static Boolean				HasOpenTransport();
#endif
#if USE_MACTCP
	static Boolean				HasMacTCP();
#endif
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
