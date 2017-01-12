// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDNSCache.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Local cache for DNS entries.

#include <UDNSCache.h>
#include <OpenTransportProviders.h>

PP_Begin_Namespace_PowerPlant

LArray*				UDNSCache::sOTDNSNameCache = nil;
LArray*				UDNSCache::sOTDNSAddressCache = nil;

void
UDNSCache::CheckCache()
{
	if (sOTDNSNameCache == nil || sOTDNSAddressCache == nil) {
		LComparator *theComparator = new LDNSNameCacheComp();
		sOTDNSNameCache = new LArray(sizeof(SDNSCacheElem), theComparator, true);
		theComparator = new LDNSAddressCacheComp();
		sOTDNSAddressCache = new LArray(sizeof(SDNSCacheElem), theComparator, true);
     }
}

UInt32
UDNSCache::GetNameFromCache(ConstStringPtr inHostName)
{
	CheckCache();

	SDNSCacheElem theAddress;
	CreateDNSCacheElem(0, inHostName, theAddress);
	ArrayIndexT theIndex = sOTDNSNameCache->FetchIndexOf(&theAddress);
	if (theIndex) {
		sOTDNSNameCache->FetchItemAt(theIndex, &theAddress);
		return theAddress.HostIP;
	}

	return 0;
}

Boolean
UDNSCache::GetAddressFromCache(
			UInt32 inHostIP,
			LStr255& outHostName)
{
	CheckCache();

	SDNSCacheElem theAddress;
	CreateDNSCacheElem(inHostIP, "\p", theAddress);
	ArrayIndexT theIndex = sOTDNSAddressCache->FetchIndexOf(&theAddress);
	if (theIndex) {
		sOTDNSAddressCache->FetchItemAt(theIndex, &theAddress);
		outHostName = theAddress.HostName;
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
//		¥ AddToDNSCache
// ---------------------------------------------------------------------------

void
UDNSCache::AddToDNSCache(UInt32 inHostIP, ConstStringPtr inHostName)
{
	CheckCache();

	ArrayIndexT theIndex;
	SDNSCacheElem theAddress;
	CreateDNSCacheElem(inHostIP, inHostName, theAddress);

	//Flush the caches if they are getting too large
	//	This is could be a lot smarter by tracking timestamps or hits
	//	for now we just arbitrarily whack off the top of each list
	if (sOTDNSNameCache->GetCount() > kDNSCacheSize) {
		sOTDNSNameCache->RemoveItemsAt(kDNSCacheSize/3, 1);
	}
	if (sOTDNSAddressCache->GetCount() > kDNSCacheSize) {
		sOTDNSAddressCache->RemoveItemsAt(kDNSCacheSize/2, 1);
	}

	//Add (or replace) to the Name Cache
	theIndex = sOTDNSNameCache->FetchIndexOf(&theAddress);
	if (theIndex) {
		sOTDNSNameCache->AssignItemsAt(1, theIndex, &theAddress);
	} else {
		sOTDNSNameCache->InsertItemsAt(1, 1, &theAddress);
	}

	//Add (or replace) to the Address Cache
	//First, check to see if the "name" we are going to add is simply the
	//	IP address in dotted decimal format. If so, we don't bother to add
	//	it to the list so that we don't get into a situation where the address
	//	cache might return the dotted decimal name as the cononical name.
	LStr255 IPString;
	LStr255 numStr = (SInt32) (inHostIP >> 24 & 0xFF);
	IPString += numStr;
	IPString += "\p.";
	numStr = (SInt32) (inHostIP >> 16 & 0xFF);
	IPString += numStr;
	IPString += "\p.";
	numStr = (SInt32) (inHostIP >> 8 & 0xFF);
	IPString += numStr;
	IPString += "\p.";
	numStr = (SInt32) (inHostIP & 0xFF);
	IPString += numStr;
	if (IPString != inHostName) {
		theIndex = sOTDNSAddressCache->FetchIndexOf(&theAddress);
		if (theIndex) {
			sOTDNSAddressCache->AssignItemsAt(1, theIndex, &theAddress);
		} else {
			sOTDNSAddressCache->InsertItemsAt(1, 1, &theAddress);
		}

	}
}

PP_End_Namespace_PowerPlant
