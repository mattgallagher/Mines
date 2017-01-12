// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptInetMapper.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Wrapper for Open Transport’s InternetServices object.

#include <LOpenTptInetMapper.h>
#include <UOpenTptSupport.h>
#include <UDNSCache.h>

#if PP_Target_Carbon && !OTCARBONAPPLICATION

	// You must put
	//		#define OTCARBONAPPLICATION 1
	// in your prefix file or precompiled header
	//
	// Under Carbon, OTOpenInternetServices() takes a context parameter.
	// The context can be NULL for an application, but must be set
	// for a non-application.
	//
	// $$$ CARBON $$$ This class needs to be revised to support contexts.

	#error "Temporary Wart"

#endif

PP_Begin_Namespace_PowerPlant

LInternetAddress *	LOpenTptInetMapper::sOurAddress = nil;

// ===========================================================================

#pragma mark === LOpenTptInetMapper ===

// ---------------------------------------------------------------------------
//		• LOpenTptInetMapper
// ---------------------------------------------------------------------------
//	Constructor

LOpenTptInetMapper::LOpenTptInetMapper()
{
	mInetSvcRef = nil;
	UOpenTptSupport::StartOpenTransport();

	OSStatus result;
	mInetSvcRef = ::OTOpenInternetServices(kDefaultInternetServicesPath, 0, &result);
	ThrowIfError_(result);

	InstallNotifierProc(mInetSvcRef);
	::OTSetAsynchronous(mInetSvcRef);
}


// ---------------------------------------------------------------------------
//		• ~LOpenTptInetMapper
// ---------------------------------------------------------------------------
//	Destructor

LOpenTptInetMapper::~LOpenTptInetMapper()
{
	RemoveNotifierProc();
	if (mInetSvcRef)
		::OTCloseProvider(mInetSvcRef);		// ignore errors
}


// ---------------------------------------------------------------------------
//		• NameToAddress
// ---------------------------------------------------------------------------
//	Call to convert a DNS address to an IP address.

UInt32
LOpenTptInetMapper::NameToAddress(
	ConstStringPtr inHostName)
{
	InetHostInfo			theInfo;
	StOpenTptOperation		operation(this, T_DNRSTRINGTOADDRCOMPLETE, &theInfo, true);
	Str255					hostName;

#if DNS_USECACHE
	//Check the local cache first
	UInt32 returnAddress = UDNSCache::GetNameFromCache(inHostName);
	if (returnAddress)
		return returnAddress;
#endif

	//Do the DNS lookup if not in the local cache
	LString::CopyPStr(inHostName, hostName);
	LString::PToCStr(hostName);

	ThrowIfError_(::OTInetStringToAddress(GetInetSvcRef(),
											reinterpret_cast<char*>(hostName),
											&theInfo));
	operation.WaitForCompletion();

#if DNS_USECACHE && DNS_CACHELOOKUPS
	//Add this to the cache
	UDNSCache::AddToDNSCache(reinterpret_cast<UInt32>(theInfo.addrs[0]), inHostName);
#endif

	return theInfo.addrs[0];
}


// ---------------------------------------------------------------------------
//		• AddressToName
// ---------------------------------------------------------------------------
//	Call to convert an IP address to a DNS address.

void
LOpenTptInetMapper::AddressToName(
	UInt32 inHostIP,
	LStr255& outHostName)
{
	Str255					hostName;

#if DNS_USECACHE
	if (UDNSCache::GetAddressFromCache(inHostIP, outHostName))
		return;
#endif

	StOpenTptOperation operation(this, T_DNRADDRTONAMECOMPLETE, nil, false);
	ThrowIfError_(::OTInetAddressToName(GetInetSvcRef(),
								static_cast<InetHost>(inHostIP),
								reinterpret_cast<char*>(hostName)));

	operation.WaitForCompletion();

	outHostName = reinterpret_cast<char*>(hostName);

	if (outHostName.EndsWith('.'))		// strip trailing '.'
		outHostName[0]--;

#if DNS_USECACHE && DNS_CACHELOOKUPS
	//Add this to the cache
	UDNSCache::AddToDNSCache(inHostIP, outHostName);
#endif
}


// ---------------------------------------------------------------------------
//		• GetLocalAddress
// ---------------------------------------------------------------------------
//	Call to retrieve the local address of this machine.

LInternetAddress*
LOpenTptInetMapper::GetLocalAddress()
{
	InetInterfaceInfo info;
	ThrowIfError_(::OTInetGetInterfaceInfo(&info, kDefaultInetInterface));

	if (!sOurAddress) {
		sOurAddress =  new LInternetAddress(info.fAddress, 0);
	}
	else {
		// Check if the local address of this machine has changed and
		// update the global sOurAddress if needed.
		if ( sOurAddress->GetIPAddress() != info.fAddress ) {
			sOurAddress->SetIPAddress(info.fAddress);
			sOurAddress->SetHostPort(0);
			sOurAddress->SetDNSAddress("\p");
		}
	}

	return new LInternetAddress(*sOurAddress);
}

// ---------------------------------------------------------------------------
//		• HandleAsyncEventSelf
// ---------------------------------------------------------------------------
//	Called by NotifyProc to handle notifications for this endpoint.
//	You will be notified via a Broadcast at primary task time of the event if necessary.
//	Warning: This routine will probably be called at interrupt time.

void
LOpenTptInetMapper::Int_HandleAsyncEventSelf(
				OTEventCode		/* inEventCode */,
				OTResult		/* inResult */,
				void*			/* inCookie */)
{
}

// ---------------------------------------------------------------------------
//		• AbortThreadOperation
// ---------------------------------------------------------------------------
// Abort the pending operation on the thread indicated

void
LOpenTptInetMapper::AbortThreadOperation(LThread * inThread)
{
	StOpenTptOperation * theOperation =
		static_cast<StOpenTptOperation*>(StAsyncOperation::GetThreadOperation(inThread));
	if (theOperation) {
		theOperation->AbortOperation();
	}
}

PP_End_Namespace_PowerPlant
