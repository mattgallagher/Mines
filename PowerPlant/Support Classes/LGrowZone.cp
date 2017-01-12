// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGrowZone.cp				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	A class for managing low memory situations. The System calls a GrowZone
//	function when it wants to allocate more memory than is currently
//	available in the heap.
//
//	To monitor memory usage, this class maintains a memory reserve (a
//	Handle). When the System calls the GrowZone function, we know that
//	memory is running low. We can then empty the reserve to free up some
//	memory, and in the process set flags and notify interested parties
//	that memory is low.
//
//	This class inherits from LBroadcaster. Objects which are able to free
//	memory when needed should be Listeners which attach themselves to the
//	GrowZone object so they will be notified when memory is low.
//
//		LGrowZone::GetGrowZone()->AddListerner(myObject);
//
//	This attaches "myObject" as a Listener to the GrowZone. When memory
//	is needed, "myObject" will receive a ListenToMessage call with a
//	msg_GrowZone message and pointer to the number of bytes needed.
//
//	For example, suppose MyClass stores a myCache Handle with data
//	generated from a lengthy calculation. If memory gets low, it
//	can dispose of the cache, meaning that it will have to recalculate
//	the data if it needs it later. This is a common trade-off:
//	memory vs. speed.
//
//		MyClass::ListenToMessage(MessageT inMessage, void *ioParam)
//		{
//			if (inMessage == msg_GrowZone) {
//								// Memory is low, free our cache
//				SInt32 freedBytes = GetHandleSize(myCache);
//				DisposeHandle(myCache);
//				myCache = nil;
//				*(SInt32*)ioParam = freedBytes;	// Pass back bytes freed
//			}
//		}
//
//	This class also inherits from LPeriodical. LGrowZone's constructor
//	installs the object in the Periodical Repeater queue so that its
//	SpendTime() function gets called each time through the main event loop.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGrowZone.h>
#include <TArrayIterator.h>
#include <LListener.h>
#include <PP_Messages.h>
#include <PP_Resources.h>

#include <Dialogs.h>
#include <OSUtils.h>


PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Class Variables

LGrowZone*		LGrowZone::sGrowZone;
GrowZoneUPP		LGrowZone::sGrowZoneUPP;


// ---------------------------------------------------------------------------
//	¥ LGrowZone								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Allocates a memory reserve of the specified size and installs
//	a GrowZone function
//
//	Installs the LGrowZone object in the Periodical Repeater queue so
//	that its SpendTime() function will get called each time through the
//	main event loop.

LGrowZone::LGrowZone(
	Size	inReserveSize)
{
	mLocalReserve = ::NewHandle(inReserveSize);
	mReserveSize  = inReserveSize;
	mGiveWarning  = false;

	SignalIf_( ::MemError() );		// Failed to allocate reserve

	sGrowZone	  = this;
	sGrowZoneUPP  = NewGrowZoneUPP(GrowZoneCallBack);

	::SetGrowZone(sGrowZoneUPP);

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ ~LGrowZone							Destructor				  [public]
// ---------------------------------------------------------------------------
//	Deallocates memory reserve and de-installs GrowZone function

LGrowZone::~LGrowZone()
{
	if (mLocalReserve != nil) {
		::DisposeHandle(mLocalReserve);
	}

	::SetGrowZone(nil);
	DisposeGrowZoneUPP(sGrowZoneUPP);

	sGrowZone	 = nil;
	sGrowZoneUPP = nil;
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------
//	Try to reallocate our memory reserve if necessary and warn user if
//	memory is getting low
//
//	Called each time through the main event loop

void
LGrowZone::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
	if ((mLocalReserve == nil) || (*mLocalReserve == nil)) {
		if (::MaxBlock() > mReserveSize + 2048) {
			if (mLocalReserve == nil) {
				mLocalReserve = ::NewHandle(mReserveSize);
			} else {
				::ReallocateHandle(mLocalReserve, mReserveSize);
			}

			if (!MemError()) {
				mGiveWarning = false;
			}
		}
	}

	if (mGiveWarning) {

			// Display alert to warn user that memory is low. We
			// purposely don't deactivate the desktop or have an
			// event filter since we don't want to do more things
			// that use memory.

		if (::GetResource(FOUR_CHAR_CODE('ALRT'), ALRT_LowMemory) != nil) {
			::CautionAlert(ALRT_LowMemory, nil);
		}
		mGiveWarning = false;
	}
}


// ---------------------------------------------------------------------------
//	¥ MemoryIsLow													  [public]
// ---------------------------------------------------------------------------
//	Return whether our memory reserve has been used
//
//	Clients can call this routine if they wish to behave differently in
//	low memory situations. For example, a program could disable the "New"
//	and "Open" commands to prevent new Documents from being created when
//	memory is low.

Boolean
LGrowZone::MemoryIsLow()
{
	return ((mLocalReserve == nil) || (*mLocalReserve == nil));
}


// ---------------------------------------------------------------------------
//	¥ DoGrowZone												   [protected]
// ---------------------------------------------------------------------------
//	Called by our Toolbox GrowZone function when the system needs more
//	memory

SInt32
LGrowZone::DoGrowZone(
	Size	inBytesNeeded)
{
	SInt32	bytesFreed = AskListenersToFree(inBytesNeeded);

	if (bytesFreed < inBytesNeeded) {
		bytesFreed += UseLocalReserve(inBytesNeeded);
	}

	return bytesFreed;
}


// ---------------------------------------------------------------------------
//	¥ UseLocalReserve											   [protected]
// ---------------------------------------------------------------------------
//	Empty our local memory reserve in order to free up some memory

SInt32
LGrowZone::UseLocalReserve(
	Size	/* inBytesNeeded */)
{
	SInt32	bytesFreed = 0;

	if ( (mLocalReserve != nil) &&
		 (*mLocalReserve != nil) &&
		 (mLocalReserve != GZSaveHnd()) ) {

		::EmptyHandle(mLocalReserve);
		bytesFreed = mReserveSize;
		mGiveWarning = true;
	}

	return bytesFreed;
}


// ---------------------------------------------------------------------------
//	¥ AskListenersToFree											  [public]
// ---------------------------------------------------------------------------
//	Broadcast message to listeners that they should free up some memory

SInt32
LGrowZone::AskListenersToFree(
	Size	inBytesNeeded)
{
		// Send a msg_GrowZone message to each Listener. The parameter
		// is a pointer to the number of bytes needed. On exit,
		// Listeners should set this to the number bytes which they freed.
		// We keep a running count of the bytes needed, which could be
		// negative if some Listener frees more than we need.

	SInt32	bytesFreed = 0;
	SInt32	byteCount = inBytesNeeded;

	if (mIsBroadcasting) {
		TArrayIterator<LListener*>	iterator(mListeners);
		LListener		*theListener;
		
			// Loop through Listeners, stopping if enough bytes
			// are freed
			
		while ( iterator.Next(theListener)  &&
				(bytesFreed < inBytesNeeded) ) {
				
			if (theListener->IsListening()) {
				byteCount = inBytesNeeded - bytesFreed;
				theListener->ListenToMessage(msg_GrowZone, &byteCount);
				bytesFreed += byteCount;
			}
		}
	}

	return bytesFreed;
}


// ---------------------------------------------------------------------------
//	¥ GrowZoneCallBack									  [static] [protected]
// ---------------------------------------------------------------------------
//	This is the "real" GrowZone function registered with the System. It
//	sets up the A5 world (68K) so we can access globals, then calls a
//	virtual function for the LGrowZone class.

pascal SInt32
LGrowZone::GrowZoneCallBack(
	Size	inBytesNeeded)
{
	SInt32	theA5 = ::SetCurrentA5();
	SInt32	bytesFreed = 0;

	try {
		bytesFreed = sGrowZone->DoGrowZone(inBytesNeeded);
	}

	catch (...) { }				// Don't propagate exception into Toolbox!

	::SetA5(theA5);

	return bytesFreed;
}


PP_End_Namespace_PowerPlant
