// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMapper.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInternetMapper
#define _H_LInternetMapper
#pragma once

#include <PP_Networking.h>
#include <LInternetAddress.h>
#include <LThread.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

//	Set to non-zero value to enable use of local cache for DNS calls
#ifndef DNS_USECACHE
#define		DNS_USECACHE 0
#endif

//	Set to non-zero value to enable caching of all DNS lookup calls
//	NOTE:	Please be careful using this feature as it will
//			circumvent dynamic name servers and may reduce
//			performance.

#ifndef DNS_CACHELOOKUPS
#define		DNS_CACHELOOKUPS 0
#endif

// ===========================================================================
//		¥ LInternetMapper					[abstract]
// ===========================================================================
//	Encapsulates the idea of the domain name server for Internet hosts.
//	The mapper is the object that is used to find the address of another
//	computer on the network.
//
//	NOTE: For most connection-oriented applications, it is not necessary
//	to use the mapper interface. The LEndpoint interface can accept
//	LInternetDNSAddress objects in its Connect member function, which causes
//	a name lookup to take place automatically.
//
//	LInternetMapper is an abstract base class (thus its constructor
//	is declared protected). Use one of the predefined subclasses,
//	such as LMacTCPInetMapper or LOpenTptInetMapper, or use the
//	UNetworkFactory::CreateInternetMapper function to create the
//	appropriate mapper for the system software that's installed
//	on the user's machine.

class LInternetMapper {

public:
	virtual					~LInternetMapper();

	// domain name <--> IP address conversion

	virtual UInt32				NameToAddress(
									ConstStringPtr inHostName) = 0;
	virtual void				AddressToName(
									UInt32 inHostIP, LStr255& outHostName) = 0;

	virtual LInternetAddress* GetLocalAddress() = 0;

	virtual void				AbortThreadOperation(LThread * inThread) = 0;

protected:
							LInternetMapper();			// do not use
							LInternetMapper(LInternetMapper&);

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
