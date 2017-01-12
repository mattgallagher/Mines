// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetAddress.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	This class represents both IP and DNS style Internet addresses. It will
//	automatically map between DNS style and IP style addresses for you as
//	necessary.

#include <LInternetAddress.h>
#include <LInternetMapper.h>
#include <UNetworkFactory.h>
#include <OpenTransport.h>
#include <OpenTransportProviders.h>
#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LInternetAddress
// ---------------------------------------------------------------------------
//	Constructor

LInternetAddress::LInternetAddress(
	UInt32					inHostAddress,
	UInt16					inHostPort)
{
	mIPAddress = inHostAddress;
	mDNSAddress = "";
	mHostPort = inHostPort;
}


// ---------------------------------------------------------------------------
//		¥ LInternetAddress
// ---------------------------------------------------------------------------
//	Constructor

LInternetAddress::LInternetAddress(
	ConstStringPtr			inHostAddress,
	UInt16					inHostPort,
	Boolean					inLookupNow)
{
	mIPAddress = 0;
	mDNSAddress = inHostAddress;
	mHostPort = inHostPort;

	UInt8 colonPos = mDNSAddress.Find(':');
	if (colonPos) {
		LStr255 numStr((void*)&mDNSAddress[(UInt8) (colonPos+1)], (UInt8) (mDNSAddress.Length() - colonPos));
		mHostPort = (UInt16) SInt32(numStr);
		mDNSAddress[0] = (UInt8) (colonPos-1);
	}

	if (inLookupNow) {
		try {
			InternalLookupAddress();
		} catch(...) {
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ LInternetAddress
// ---------------------------------------------------------------------------
//	Alternate Constructor... assumes you don't want to use an address

LInternetAddress::LInternetAddress(
	UInt16	inHostPort)
{
	mIPAddress = 0;
	mDNSAddress = "";
	mHostPort = inHostPort;
}


// ---------------------------------------------------------------------------
//		¥ LInternetAddress
// ---------------------------------------------------------------------------
//	Copy constructor

LInternetAddress::LInternetAddress(
	const LInternetAddress& inOriginal)
{
	mIPAddress = inOriginal.mIPAddress;
	mDNSAddress = inOriginal.mDNSAddress;
	mHostPort = inOriginal.mHostPort;
}


// ---------------------------------------------------------------------------
//		¥ ~LInternetAddress
// ---------------------------------------------------------------------------
//	Destructor

LInternetAddress::~LInternetAddress()
{
}


// ---------------------------------------------------------------------------
//		¥ GetDescriptor
// ---------------------------------------------------------------------------
//	Convert the IP address to dotted decimal format and return this string.
//	If a port number is specified, optionally return that at the end of the string.

StringPtr
LInternetAddress::GetIPDescriptor(
	Str255	outDescriptor,
	Boolean withPort)
{
	if (!mIPAddress)
		InternalLookupAddress();

	LStr255 returnStr;
	LStr255 numStr = (SInt32) (mIPAddress >> 24 & 0xFF);
	returnStr += numStr;
	returnStr += "\p.";
	numStr = (SInt32) (mIPAddress >> 16 & 0xFF);
	returnStr += numStr;
	returnStr += "\p.";
	numStr = (SInt32) (mIPAddress >> 8 & 0xFF);
	returnStr += numStr;
	returnStr += "\p.";
	numStr = (SInt32) (mIPAddress & 0xFF);
	returnStr += numStr;

	if (withPort && mHostPort) {
		returnStr += "\p:";
		numStr = (SInt32)mHostPort;
		returnStr += numStr;
	}

	return LString::CopyPStr(returnStr, outDescriptor);
}


// ---------------------------------------------------------------------------
//		¥ GetDNSDescriptor
// ---------------------------------------------------------------------------
//	Return the name of the specified host (and port number if available).
// Use GetDNSAddress if you don't care if we have tried to do a DNS lookup...
// See GetDNSAddress for more info.

StringPtr
LInternetAddress::GetDNSDescriptor(
	Str255	outDescriptor,
	Boolean withPort)
{
	if (mDNSAddress.Length() == 0)
		InternalLookupName();

	LStr255 returnStr;
	returnStr = mDNSAddress;

	//Check to see if we have the host DNS name in dotted decimal format...
	//	if so we try to lookup the address.
	Str255	tempIPDesc;
	GetIPDescriptor(tempIPDesc);
	if (static_cast<const LString&>(returnStr) == tempIPDesc) {
		InternalLookupName();
		returnStr = mDNSAddress;
	}

	if (withPort && mHostPort) {
		LStr255 numStr = (SInt32)mHostPort;
		returnStr += "\p:";
		returnStr += numStr;
	}

	return LString::CopyPStr(returnStr, outDescriptor);
}

// ---------------------------------------------------------------------------
//		¥ GetIPAddress
// ---------------------------------------------------------------------------
//	Return the 32-bit IP address of the host.

UInt32
LInternetAddress::GetIPAddress()
{
	if (!mIPAddress)
		InternalLookupAddress();

	return mIPAddress;
}


// ---------------------------------------------------------------------------
//		¥ GetIPAddress
// ---------------------------------------------------------------------------
//	Return the 32-bit IP address of the host as well as dotted decimal format.

UInt32
LInternetAddress::GetIPAddress(
	Str255	outDescriptor)
{
	GetIPDescriptor(outDescriptor);

	return mIPAddress;
}


// ---------------------------------------------------------------------------
//		¥ GetDNSAddress
// ---------------------------------------------------------------------------
// GetDNSAddress returns the contents of mDNSAddress which may be in dotted
//	decimal format (i.e. ###.###.###.###)
// Use GetDNSDescriptor if you wish to ensure we have tried to do a lookup to
//	get the real DNS style name.


StringPtr
LInternetAddress::GetDNSAddress(
	Str255	outDescriptor)
{
	if (mDNSAddress.Length() == 0)
		InternalLookupName();

	return LString::CopyPStr(mDNSAddress, outDescriptor);
}


// ---------------------------------------------------------------------------
//		¥ MakeOTIPAddress
// ---------------------------------------------------------------------------

void
LInternetAddress::MakeOTIPAddress(
	TNetbuf&				outAddress)
{
	StPointerBlock	inAddrPtr( sizeof(InetAddress), Throw_Yes, true );
	InetAddress*	inAddr = (InetAddress*) inAddrPtr.Get();

	inAddr->fAddressType = AF_INET;
	inAddr->fPort = mHostPort;
	inAddr->fHost = GetIPAddress();

	outAddress.maxlen = outAddress.len = sizeof (InetAddress);
	outAddress.buf = (UInt8*)(inAddrPtr.Release());
}


// ---------------------------------------------------------------------------
//		¥ MakeOTDNSAddress
// ---------------------------------------------------------------------------

void
LInternetAddress::MakeOTDNSAddress(
	TNetbuf&				outAddress)
{
	StPointerBlock	dnsAddrPtr( sizeof(DNSAddress), Throw_Yes, true );
	DNSAddress*		dnsAddr = (DNSAddress*) dnsAddrPtr.Get();

	dnsAddr->fAddressType = AF_DNS;
	GetDNSDescriptor((unsigned char*)dnsAddr->fName, true);
	LString::PToCStr((unsigned char*)dnsAddr->fName);

	outAddress.maxlen = sizeof (DNSAddress);
	outAddress.len = sizeof (dnsAddr->fAddressType)
					 + LString::CStringLength(dnsAddr->fName);
	outAddress.buf = (UInt8*) dnsAddrPtr.Release();
}


// ---------------------------------------------------------------------------
//		¥ Clone
// ---------------------------------------------------------------------------

LInternetAddress*
LInternetAddress::Clone()
{
	return new LInternetAddress(*this);
}


// ---------------------------------------------------------------------------
//		¥ InternalLookupAddress
// ---------------------------------------------------------------------------
//	Create mapper and lookup address from DNS name.

void
LInternetAddress::InternalLookupAddress()
{
	if (mDNSAddress.Length() != 0) {
		StDeleter<LInternetMapper> theMapper(UNetworkFactory::CreateInternetMapper());
		SetIPAddress(theMapper->NameToAddress(mDNSAddress));
	}
}


// ---------------------------------------------------------------------------
//		¥ InternalLookupName
// ---------------------------------------------------------------------------
//	Create mapper and lookup name from IP Address.

void
LInternetAddress::InternalLookupName()
{
	if (mIPAddress) {
		LInternetMapper * theMapper = UNetworkFactory::CreateInternetMapper();
		try {
			theMapper->AddressToName(mIPAddress, mDNSAddress);
		} catch(...) {
			//If lookup fails assume the address has no DNS name
			//	and insert the dotted decimal equivalent
			Str255	tempIPDesc;
			mDNSAddress = GetIPDescriptor(tempIPDesc);
		}
		delete theMapper;
	}
}

PP_End_Namespace_PowerPlant
