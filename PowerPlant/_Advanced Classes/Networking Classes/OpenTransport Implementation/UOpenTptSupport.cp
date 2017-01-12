// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UOpenTptSupport.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Helpers for OpenTransport classes.
//
//	UOpenTptSupport-
//		Handles gestalt checks for OpenTransport
//		Converts OT address to PP address
//
//	StOpenTptOperation-
//		Manages thread blocking for OpenTransport interface calls
//
//	LOpenTptNotifHandler-
//		Mix-in class for OpenTransport endpoints needing completion routines
//
//	LOpenTpt_CloseOpenTpt-
//		Installs cleanup task to ensure that CloseOpenTransport is called when necessary

#include <UOpenTptSupport.h>
#include <LInternetAddress.h>
#include <UMemoryMgr.h>

#include <Gestalt.h>
#include <OpenTransport.h>

#if TARGET_API_MAC_CARBON && !OTCARBONAPPLICATION

	// You must put
	//		#define OTCARBONAPPLICATION 1
	// in your prefix file or precompiled header
	//
	// Under Carbon, InitOpenTransport()/CloseOpenTransport takes a context parameter.
	// The context can be NULL for an application, but must be set
	// for a non-application.
	//
	// $$$ CARBON $$$ This class needs to be revised to support contexts.

	#error "Temporary Wart"

#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

Boolean					UOpenTptSupport::sOTGestaltTested = false;
LOpenTpt_CloseOpenTpt*	UOpenTptSupport::sCloseOpenTptTask = nil;
SInt32					UOpenTptSupport::sOTGestaltResult = 0;
Boolean					UOpenTptSupport::sOTInitialized = false;

TimerUPP				StOpenTptOperation::sOTOpTimerUPP = NewTimerUPP((TimerProcPtr) &Int_TimerProc);


// ===========================================================================

#pragma mark === UOpenTptSupport ===

// ---------------------------------------------------------------------------
//		¥ HasOpenTransport
// ---------------------------------------------------------------------------

Boolean
UOpenTptSupport::HasOpenTransport()
{
	GetOTGestalt();
	return (sOTGestaltResult & gestaltOpenTptPresentMask) == gestaltOpenTptPresentMask;
}

// ---------------------------------------------------------------------------
//		¥ GetOTGestalt
// ---------------------------------------------------------------------------

void
UOpenTptSupport::GetOTGestalt()
{
	if (!sOTGestaltTested) {

		// Check whether Open Transport is installed.

		if (::Gestalt(gestaltOpenTpt, &sOTGestaltResult) == noErr) {

			// If running on 68K, we require version 1.1 or later, otherwise we pretend it isn't there.
			// Actually, this test is needed only if we are running on a PowerPC Mac (i.e. this is not a fat application)
			// because versions of Open Transport prior to 1.1 were never shipped for 68K Macs.
			// See Apple's Q&A NW 40 - "Open Transport's Limited Compatibility with 680x0" for more details.

			#if __MC68K__ && OPENTPT_ON_68K

			NumVersion			theVersion;

			OSErr theErr = ::Gestalt(gestaltOpenTptVersions, (long*)&theVersion);

			if (theErr != noErr || (theVersion.majorRev == 1 && theVersion.minorAndBugRev < 0x10))
				sOTGestaltResult = 0;

			#endif

		}

	}

	// There is no point in checking these Gestalts several time, so always set this flag to true.
	sOTGestaltTested = true;
}

// ---------------------------------------------------------------------------
//		¥ HasOpenTransportTCP
// ---------------------------------------------------------------------------

Boolean
UOpenTptSupport::HasOpenTransportTCP()
{
	GetOTGestalt();
	return (sOTGestaltResult &
			(gestaltOpenTptPresentMask + gestaltOpenTptTCPPresentMask))
			== (gestaltOpenTptPresentMask + gestaltOpenTptTCPPresentMask);
}


// ---------------------------------------------------------------------------
//		¥ StartOpenTransport
// ---------------------------------------------------------------------------

void
UOpenTptSupport::StartOpenTransport()
{
    if (sCloseOpenTptTask == nil) {
		ThrowIfError_(::InitOpenTransport());
        sOTInitialized = true;
        sCloseOpenTptTask = new LOpenTpt_CloseOpenTpt;
    }
}

// ---------------------------------------------------------------------------
//		¥ OTAddressToPPAddress
// ---------------------------------------------------------------------------

LInternetAddress*
UOpenTptSupport::OTAddressToPPAddress(
	OTAddress*				inAddress)
{
	switch (inAddress->fAddressType) {
		case AF_INET:
			{
				InetAddress* inetAddr = (InetAddress*)inAddress;
				return new LInternetAddress(inetAddr->fHost, inetAddr->fPort);
			}

		case AF_DNS:
			{
				DNSAddress* dnsAddr = (DNSAddress*)inAddress;
				LStr255 tempStr(dnsAddr->fName);
				return new LInternetAddress(tempStr, 0, false);
			}

		default:
			SignalPStr_("\pUOpenTptSupport::OTAddressToPPAddress - Unknown address type");
			return nil;			// dunno what to do with thisÉ
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === StOpenTptOperation ===

// ---------------------------------------------------------------------------
//		¥ StOpenTptOperation()
// ---------------------------------------------------------------------------
//	Constructor

StOpenTptOperation::StOpenTptOperation(
						LOpenTptNotifHandler *	inNotifHandler,
						OTEventCode				inEventCode,
						void *					inCookieTest,
						Boolean					inTestCookie)
	: mNotifHandler(inNotifHandler)
{
	mCookieTest = inCookieTest;
	mTestCookie = inTestCookie;

	mEventCode = inEventCode;
	mResultCode = kOTNoError;
	mCookie = nil;

	mNotifHandler->AddNotifListener(this);
}

// ---------------------------------------------------------------------------
//		¥ ~StOpenTptOperation
// ---------------------------------------------------------------------------
//	Destructor

StOpenTptOperation::~StOpenTptOperation()
{
	mNotifHandler->RemoveNotifListener(this);
}

// ---------------------------------------------------------------------------
//		¥ Int_TimerProc	[static]
// ---------------------------------------------------------------------------
//	Time Manager completion routine for timeout simulation in OT

#ifdef __MWERKS__
#pragma profile off
#endif

pascal void
StOpenTptOperation::Int_TimerProc(TMTaskPtr tmTaskPtr
									#if !TARGET_RT_MAC_CFM && !TARGET_RT_MAC_MACHO
										: __A1
									#endif
							)
{
	StOpenTptOperation* theOperation = ((SOpenTptTMTask*)tmTaskPtr)->mOperation;
	StSetupGlobals globals(*theOperation);

	theOperation->mOperationTimeout = true;
	theOperation->Int_AsyncResume(Timeout_Error);
}

#ifdef __MWERKS__
#pragma profile reset
#endif

// ---------------------------------------------------------------------------
//		¥ WaitForCompletion
// ---------------------------------------------------------------------------

void
StOpenTptOperation::WaitForCompletion(
						UInt32					inTimeoutSeconds)
{
	//Don't block the main thread
	if (mThread == LThread::GetMainThread())
		Throw_(threadProtocolErr);

	//Create time task if requested to simulate a timeout for this operation
	SOpenTptTMTask task;
	LOperationListMember * operationListMem = nil;

	try {
		if (inTimeoutSeconds != Timeout_None) {
			task.tmAddr = sOTOpTimerUPP;
			task.tmWakeUp = 0;
			task.tmReserved = 0;
			task.mOperation = this;

			::InsTime((QElemPtr)&task);
			::PrimeTime((QElemPtr)&task, (SInt32) (inTimeoutSeconds * 1000));
		}

		operationListMem = new LOperationListMember(this);

		mOperationTimeout = false;
		mThread->Block();

		if (inTimeoutSeconds != Timeout_None) {
			::RmvTime((QElemPtr)&task);
		}

		delete operationListMem;
	} catch(...) {
		if (inTimeoutSeconds != Timeout_None) {
			::RmvTime((QElemPtr)&task);
		}

		delete operationListMem;

		throw;
	}

	ThrowIfError_(mResult);
}


// ===========================================================================

#pragma mark -
#pragma mark === LOpenTptNotifHandler ===

// ---------------------------------------------------------------------------
//		¥ LOpenTptNotifHandler
// ---------------------------------------------------------------------------
//	Constructor

LOpenTptNotifHandler::LOpenTptNotifHandler()
{
	mProvider  = nil;
	mNotifyUPP = NewOTNotifyUPP(Int_NotifierProc);
}


// ---------------------------------------------------------------------------
//		¥ ~LOpenTptNotifHandler
// ---------------------------------------------------------------------------
//	Destructor

LOpenTptNotifHandler::~LOpenTptNotifHandler()
{
	if (mProvider) {
		RemoveNotifierProc();
	}

	DisposeOTNotifyUPP(mNotifyUPP);
}


// ---------------------------------------------------------------------------
//		¥ AddNotifListener
// ---------------------------------------------------------------------------

void
LOpenTptNotifHandler::AddNotifListener(
	StOpenTptOperation* inListener)
{
	mNotifListeners.Append(inListener);
}


// ---------------------------------------------------------------------------
//		¥ InstallNotifierProc
// ---------------------------------------------------------------------------

void
LOpenTptNotifHandler::InstallNotifierProc(
	ProviderRef inProvider)
{
	if (mProvider) {
		RemoveNotifierProc();
	}

	::OTInstallNotifier(inProvider, mNotifyUPP, this);
	mProvider = inProvider;
}


// ---------------------------------------------------------------------------
//		¥ RemoveNotifierProc
// ---------------------------------------------------------------------------

void
LOpenTptNotifHandler::RemoveNotifierProc()
{
	if (mProvider) {
		::OTRemoveNotifier(mProvider);
		mProvider = nil;
	}
}


// ---------------------------------------------------------------------------
//		¥ RemoveNotifListener
// ---------------------------------------------------------------------------

void
LOpenTptNotifHandler::RemoveNotifListener(
	StOpenTptOperation* inListener)
{
	mNotifListeners.Remove(inListener);
}


// ---------------------------------------------------------------------------
//		¥ NotifierProc
// ---------------------------------------------------------------------------

pascal void
LOpenTptNotifHandler::Int_NotifierProc(
	void*			inContextPtr,
	OTEventCode		inEventCode,
	OTResult		inResult,
	void*			inCookie)
{
	((LOpenTptNotifHandler*) inContextPtr)->
					Int_HandleAsyncEvent(inEventCode, inResult, inCookie);
}


// ---------------------------------------------------------------------------
//		¥ HandleAsyncEvent
// ---------------------------------------------------------------------------

void
LOpenTptNotifHandler::Int_HandleAsyncEvent(
	OTEventCode		inEventCode,
	OTResult		inResult,
	void*			inCookie)
{
	try {
			// Resume operations which match the eventCode.

		TInterruptSafeListIterator<StOpenTptOperation*> iter(mNotifListeners);
		StOpenTptOperation* operation;
		Boolean keepGoing = true;
		Boolean eventHandled = false;

		while (iter.Next(operation) && keepGoing) {
			if (inEventCode == operation->mEventCode) {
				if (operation->mTestCookie) {
					if (operation->mCookieTest != inCookie)
						return;
				}

				eventHandled = true;
				operation->mResultCode = inResult;
				operation->mCookie = inCookie;
				operation->Int_AsyncResume(inResult);
			}
		}

		if (!eventHandled) {
			Int_HandleAsyncEventSelf(inEventCode, inResult, inCookie);
		}
	}

	catch(...) { }	// Prevent exceptions from propagating back into OT.
}

// ===========================================================================

#pragma mark -
#pragma mark === LOpenTpt_CloseOpenTpt ===

// ---------------------------------------------------------------------------
//		¥ LOpenTpt_CloseOpenTpt
// ---------------------------------------------------------------------------
//	Constructor

LOpenTpt_CloseOpenTpt::LOpenTpt_CloseOpenTpt()
{
}

// ---------------------------------------------------------------------------
//		¥ ~LOpenTpt_CloseOpenTpt
// ---------------------------------------------------------------------------
//	Destructor

LOpenTpt_CloseOpenTpt::~LOpenTpt_CloseOpenTpt()
{
}

// ---------------------------------------------------------------------------
//		¥ DoCleanup
// ---------------------------------------------------------------------------

void
LOpenTpt_CloseOpenTpt::DoCleanup()
{
    if (UOpenTptSupport::sCloseOpenTptTask) {
		::CloseOpenTransport();
        UOpenTptSupport::sOTInitialized = false;
        UOpenTptSupport::sCloseOpenTptTask = nil;
	}
}

// ===========================================================================

#pragma mark -
#pragma mark === LListenCall ===

// ---------------------------------------------------------------------------
//	¥ LListenCall							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LListenCall::LListenCall()
{
										// Ptr with Clear bytes
	StPointerBlock	callPtr(sizeof(TCall), true, true);

										// Ptr with Clear bytes
	StPointerBlock	bufPtr(sizeof(struct InetAddress), true, true);

										// Both memory allocations succeeded
										// Store Ptr's in member variables
	mTCall = (TCall*) callPtr.Release();

	mTCall->addr.buf	= (UInt8*) bufPtr.Release();
	mTCall->addr.maxlen = sizeof(struct InetAddress);
	mTCall->addr.len	= sizeof(struct InetAddress);
}


// ---------------------------------------------------------------------------
//	¥ ~LListenCall							Destructor				  [public]
// ---------------------------------------------------------------------------

LListenCall::~LListenCall()
{
	::DisposePtr((Ptr)mTCall->addr.buf);
	::DisposePtr((Ptr)mTCall);
}


PP_End_Namespace_PowerPlant
