// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UOpenTptSupport.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UOpenTptSupport
#define _H_UOpenTptSupport
#pragma once

#include <UNetworking.h>
#include <LInterruptSafeList.h>
#include <OpenTransport.h>
#include <OpenTransportProviders.h>
#include <PP_Types.h>
#include <UCallbackUtils.h>
#include <PP_Networking.h>
#include <LCleanupTask.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

// we need to forward declare OTAddress, but it needs to remain
// outside of the PowerPlant namespace
struct OTAddress;


PP_Begin_Namespace_PowerPlant

class LInternetAddress;
class LReentrantMemoryPool;
class LOpenTpt_CloseOpenTpt;

// ===========================================================================
//	¥ Conditional compilation
// ===========================================================================

// Set 'OPENTPT_ON_68K' to zero to disable OpenTransport support on 68K code.
//	If set to zero, MacTCP interfaces will always be called from 68K code.

//Note: Calling OT interfaces from 68k code is not GUARANTEED to to 100% compatabile
//	with all past and future OS and OT combinations. As of the time of this release,
//	Apple is telling us that OT interfaces will not be accessable from 68K code
//	under System 8.
//	Further, some very early versions of OT may not be compatible with calling
//	OT interfaces from 68K code under System 7. The gestalt check in
//	UOpenTptSupport::GetOTGestalt() handles this situation, but you should
//	be aware of the potential conflicts.
//	-FV 11/1/96

#ifndef OPENTPT_ON_68K
#	define OPENTPT_ON_68K			1
#endif

#pragma mark === UOpenTptSupport ===

// ===========================================================================
//		¥ UOpenTptSupport
// ===========================================================================
//	Helpers for Open Transport classes.

class UOpenTptSupport {

public:
	static Boolean			HasOpenTransport();
	static Boolean			HasOpenTransportTCP();

	static void				StartOpenTransport();

	static LInternetAddress*	OTAddressToPPAddress(
									OTAddress*				inAddress);

	static Boolean			IsOpenTransportInitialized()
								{ return sOTInitialized; }

private:
	static void				GetOTGestalt();

	static Boolean					sOTGestaltTested;
	static LOpenTpt_CloseOpenTpt*	sCloseOpenTptTask;
	static SInt32					sOTGestaltResult;

	static Boolean					sOTInitialized;

	friend class LOpenTpt_CloseOpenTpt;
};

// ===========================================================================

#pragma mark -
#pragma mark === StOpenTptOperation ===

// ===========================================================================
//		¥ StOpenTptOperation
// ===========================================================================

class LOpenTptNotifHandler;
class StOpenTptOperation;

struct SOpenTptTMTask : public TMTask {
	StOpenTptOperation * mOperation;
};

class StOpenTptOperation :	public StAsyncOperation,
							public LInterruptSafeListMember,
							public LGlobalsContext {

public:
							StOpenTptOperation(
									LOpenTptNotifHandler *	inNotifHandler,
									OTEventCode				inEventCode,
									void *					inCookieTest = nil,
									Boolean					inTestCookie = false);
							~StOpenTptOperation();

	void					SetEventCode(OTEventCode inEventCode)
								{ mEventCode = inEventCode; }
	OTEventCode				GetEventCode()
								{ return mEventCode; }
	OTResult				GetResultCode()
								{ return mResultCode; }
	void*					GetCookie()
								{ return mCookie; }

	void					WaitForCompletion(UInt32 inTimeoutSeconds = Timeout_None);
	
	void					WaitForResult()
							{ SignalStringLiteral_("Don't call WaitForResult for StOpenTptOperation... use WaitForCompletion"); }

	Boolean					OperationTimedOut()
								{ return mOperationTimeout; }
protected:

	LOpenTptNotifHandler *	mNotifHandler;

	OTEventCode				mEventCode;
	void *					mCookieTest;
	Boolean					mTestCookie;

	OTResult				mResultCode;
	void*					mCookie;

	Boolean					mOperationTimeout;

	#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
		static pascal void			Int_TimerProc(TMTaskPtr tmTaskPtr);
	#else
		static pascal void			Int_TimerProc(TMTaskPtr tmTaskPtr : __A1);
	#endif

private:
	static TimerUPP			sOTOpTimerUPP;

	friend class LOpenTptNotifHandler;
};

// ===========================================================================

#pragma mark -
#pragma mark === LOpenTptNotifHandler ===

class LOpenTptNotifHandler {

public:
						LOpenTptNotifHandler();
	virtual				~LOpenTptNotifHandler();

	void				InstallNotifierProc(
								ProviderRef		inProvider);
	void				RemoveNotifierProc();

	void				AddNotifListener(
								StOpenTptOperation* inListener);
	void				RemoveNotifListener(
								StOpenTptOperation* inListener);

protected:
	static pascal void	Int_NotifierProc(
								void*			inContextPtr,
								OTEventCode		inEventCode,
								OTResult		inResult,
								void*			inCookie);

	void				Int_HandleAsyncEvent(
								OTEventCode		inEventCode,
								OTResult		inResult,
								void*			inCookie);

	virtual void		Int_HandleAsyncEventSelf(
								OTEventCode		inEventCode,
								OTResult		inResult,
								void*			inCookie) = 0;

	ProviderRef			mProvider;
	OTNotifyUPP			mNotifyUPP;
	TInterruptSafeList<StOpenTptOperation*>	mNotifListeners;

private:
						LOpenTptNotifHandler(LOpenTptNotifHandler&);	// do not use

	friend class StOpenTptOperation;
};

// ===========================================================================

#pragma mark -
#pragma mark === LOpenTpt_CloseOpenTpt ===

class LOpenTpt_CloseOpenTpt : public LCleanupTask {

public:

							LOpenTpt_CloseOpenTpt();
	virtual					~LOpenTpt_CloseOpenTpt();

protected:
	virtual void			DoCleanup();

	friend class UOpenTptSupport;
};

// ===========================================================================
#pragma mark -
#pragma mark === LListenCall ===

//	An LListenCall object just wraps up creation of a TCall param for convenience

class LListenCall : public LInterruptSafeListMember {
public:
					LListenCall();

		virtual		~LListenCall();

	TCall *				mTCall;
	InetHost			mRemoteHostAddress;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif

