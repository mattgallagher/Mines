// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDNSCache.h					PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UDNSCache
#define _H_UDNSCache
#pragma once

#include <LArray.h>
#include <LComparator.h>
#include <LString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


#define	kDNSCacheSize	500

typedef struct SDNSCacheElem {
	UInt32		HostIP;
	Str255 		HostName;
} SDNSCacheElem;

// ===========================================================================

#pragma mark === UDNSCache ===

class UDNSCache {

public:

	static void				CheckCache();
	static UInt32			GetNameFromCache(ConstStringPtr inHostName);
	static Boolean			GetAddressFromCache(UInt32 inHostIP, LStr255& outHostName);
	static void				AddToDNSCache(UInt32 inHostIP, ConstStringPtr inHostName);

protected:
	static void					CreateDNSCacheElem(UInt32 inHostIP, ConstStringPtr inHostName, SDNSCacheElem& outElem)
								{
									outElem.HostIP = inHostIP;
									::BlockMoveData(inHostName, outElem.HostName, inHostName[0] + 1);
								}
private:
	static LArray*				sOTDNSNameCache;
	static LArray*				sOTDNSAddressCache;
};

// ===========================================================================

#pragma mark -
#pragma mark === LDNSNameCacheComp ===

class	LDNSNameCacheComp : public LComparator {
public:
					LDNSNameCacheComp() {};
					~LDNSNameCacheComp() {};

	SInt32	Compare(const void*		inItemOne,
					const void*		inItemTwo,
					UInt32			/* inSizeOne */ ,
					UInt32			/* inSizeTwo */) const
			{
				const SDNSCacheElem * elemOne = (const SDNSCacheElem*)(inItemOne);
				const SDNSCacheElem * elemTwo = (const SDNSCacheElem*)(inItemTwo);

				return ::CompareText(&(elemOne->HostName[1]),
									&(elemTwo->HostName[1]),
									elemOne->HostName[0],
									elemTwo->HostName[0],
									nil);
			}
};

// ===========================================================================

#pragma mark -
#pragma mark === LDNSAddressCacheComp ===

class	LDNSAddressCacheComp : public LComparator {
public:
					LDNSAddressCacheComp() {};
					~LDNSAddressCacheComp() {};

	SInt32	Compare(const void*		inItemOne,
					const void*		inItemTwo,
					UInt32			/* inSizeOne */ ,
					UInt32			/* inSizeTwo */) const
			{
				const SDNSCacheElem * elemOne = (const SDNSCacheElem*)(inItemOne);
				const SDNSCacheElem * elemTwo = (const SDNSCacheElem*)(inItemTwo);

				return (SInt32) (elemOne->HostIP - elemTwo->HostIP);
			}
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
