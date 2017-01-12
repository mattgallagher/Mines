// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetAddress.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInternetAddress
#define _H_LInternetAddress
#pragma once

#include <LString.h>
#include <OpenTransport.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ===========================================================================
//		¥ LInternetAddress
// ===========================================================================
//	Wrapper for the IP addresses that are used internally by the
//	Internet to describe another computer's location. DNS names
//	(like "ftp.metrowerks.com") must be mapped to IP addresses (127.0.0.1)
//	before connections are established. This object will handle such
//	conversions automatically.

class LInternetAddress {

public:
								LInternetAddress(
										UInt32					inHostAddress = 0,
										UInt16					inHostPort = 0);
								LInternetAddress(
										ConstStringPtr			inHostAddress,
										UInt16					inHostPort,
										Boolean					inLookupNow);
								LInternetAddress(
										UInt16					inHostPort);
								LInternetAddress(
										const LInternetAddress& inOriginal);
	virtual						~LInternetAddress();

	// address accessors

	virtual StringPtr			GetIPDescriptor(Str255			outDescriptor,
												Boolean			withPort = false);
	virtual UInt32				GetIPAddress(Str255				outDescriptor);
	virtual UInt32				GetIPAddress();

	virtual StringPtr			GetDNSDescriptor(
												Str255			outDescriptor,
												Boolean			withPort = false);
	virtual StringPtr			GetDNSAddress(Str255			outDescriptor);


	virtual void				SetIPAddress(UInt32				inHostAddress)
								{
									mIPAddress = inHostAddress;
								}

	virtual void				SetDNSAddress(ConstStringPtr	inHostAddress)
								{
									mDNSAddress = inHostAddress;
								}

	virtual UInt16				GetHostPort()
								{
									return mHostPort;
								}

	virtual void				SetHostPort(UInt16					inHostPort)
								{
									mHostPort = inHostPort;
								}

	// helpers

	virtual LInternetAddress*	Clone();
	virtual void				MakeOTIPAddress(TNetbuf&		outAddress);
	virtual void				MakeOTDNSAddress(TNetbuf&		outAddress);

	// data members

protected:
	UInt32						mIPAddress;
	LStr255						mDNSAddress;
	UInt16						mHostPort;

private:
	void						InternalLookupAddress();
	void						InternalLookupName();

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
