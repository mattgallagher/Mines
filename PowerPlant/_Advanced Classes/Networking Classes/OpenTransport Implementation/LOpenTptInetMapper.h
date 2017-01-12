// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOpenTptInetMapper.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Wrapper for Open Transport’s InternetServices object.

#ifndef _H_LOpenTptInetMapper
#define _H_LOpenTptInetMapper
#pragma once

#include <LInternetMapper.h>
#include <UOpenTptSupport.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LOpenTptInetMapper :	public LInternetMapper,
							public LOpenTptNotifHandler {

public:
							LOpenTptInetMapper();
	virtual					~LOpenTptInetMapper();

	// domain name <--> IP address conversion

	virtual UInt32				NameToAddress(
									ConstStringPtr inHostName);
	virtual void				AddressToName(
									UInt32 inHostIP, LStr255& outHostName);
	virtual LInternetAddress*	GetLocalAddress();

	inline InetSvcRef			GetInetSvcRef() const
										{ return mInetSvcRef; }

	virtual void				Int_HandleAsyncEventSelf(
										OTEventCode		inEventCode,
										OTResult		inResult,
										void*			inCookie);

	virtual void				AbortThreadOperation(LThread * inThread);

protected:


	static LInternetAddress	*	sOurAddress;

	InetSvcRef				mInetSvcRef;

private:
							LOpenTptInetMapper(LOpenTptInetMapper&);

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
