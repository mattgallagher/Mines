// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMemoryMgr.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Memory management utility classes and functions
//
//	Stack-based Classes:
//		Several utility classes are designed for creating stack-based
//		objects, where the Constructor performs some action and the
//		Destructor undoes the action. The advantage of stack-based
//		objects is that the Destructor automatically gets called,
//		even when there is an exception thrown.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UMemoryMgr.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ StHandleLocker Class
// ===========================================================================
//	Constructor Locks the Handle
//	Destructor restores the Handle's original locked/unlocked state

StHandleLocker::StHandleLocker(
	Handle	inHandle)
{
	mHandle		 = nil;
	mWasUnlocked = false;

	Adopt(inHandle);
}


StHandleLocker::~StHandleLocker()
{
	RestoreLockState();
}


void
StHandleLocker::RestoreLockState()
{
	if (mWasUnlocked && (mHandle != nil)) {
		::HUnlock(mHandle);
	}
}


void
StHandleLocker::Adopt(				// Take control over locking
	Handle	inHandle)				//   and unlocking this Handle
{
	RestoreLockState();				// Restore state of current Handle

	mHandle		 = inHandle;		// Store new Handle
	mWasUnlocked = false;

	if (inHandle != nil) {			// Lock new Handle

		SInt8	theState = ::HGetState(inHandle);
		ThrowIfMemError_();

			// Bit 7 of state is set if Handle is locked. We want to
			// know if the Handle is currently locked or unlocked.
			// If it is unlocked, we lock it now and unlock it the
			// destructor. If it is already locked, we do nothing
			// here and nothing in the destructor.

		mWasUnlocked = (theState & 0x80) == 0;	// Unlocked if bit 7 is clear

		if (mWasUnlocked) {
			::HLock(inHandle);
		}
	}
}


Handle								// Relinquish control of Handle
StHandleLocker::Release()
{
	RestoreLockState();

	Handle	outHandle = mHandle;

	mHandle		 = nil;				// We now have no Handle
	mWasUnlocked = false;

	return outHandle;
}


#pragma mark -
// ===========================================================================
//	¥ StHandleState Class
// ===========================================================================
//	Constructor saves Handle state
//	Destructor restores Handle state

StHandleState::StHandleState(
	Handle	inHandle)
{
	mHandle = inHandle;
	mSavedState = ::HGetState(inHandle);
	ThrowIfMemError_();
}


StHandleState::~StHandleState()
{
	::HSetState(mHandle, mSavedState);
}


#pragma mark -
// ===========================================================================
//	¥ StHandleBlock Class
// ===========================================================================
//	Manages a Toolbox Handle

// ---------------------------------------------------------------------------
//	¥ StHandleBlock							Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from an existing Handle. Object takes ownership of the Handle.
//
//	It's OK to pass nil for the Handle. You can set the Handle later by
//	calling Adopt().
//
//	Note that you will get an error if you write code such as:
//
//		StHandleBlock	theH ( nil );
//
//	about an ambiguous constructor call. Since nil is really just the
//	number zero, the constructor that takes a Size parameter followed by
//	two bools with default values also matches the call parameters.
//
//	To workaround this ambiguity, you need to use the constants Handle_Nil
//	and Size_Zero, which have explicit types, so the compiler knows if
//	you want to use a nil Handle or create a Handle containing zero bytes.

StHandleBlock::StHandleBlock(
	Handle	inHandle)				// Take ownership of this Handle
{
	mIsOwner = (inHandle != nil);
	mHandle  = inHandle;
}


StHandleBlock::StHandleBlock(
	Handle	inHandle,
	bool	inIsOwner)
{
	mHandle  = inHandle;
	mIsOwner = inIsOwner && (inHandle != nil);
}


StHandleBlock::StHandleBlock(
	Size	inSize,					// Bytes to allocate
	bool	inThrowFail,			// Throw if allocate fails?
	bool	inUseTempIfFull)		// Use temp memory if app heap is full?
{
	mIsOwner = true;
	mHandle  = ::NewHandle(inSize);	// Try to allocate in app heap

	OSErr	err = noErr;
	if (mHandle == nil) {			// Failed to allocate in app heap
		err = ::MemError();
		if (inUseTempIfFull) {		// Try to allocate in temporary memory
			mHandle = ::TempNewHandle(inSize, &err);
		}
	}

	if (inThrowFail  &&  (mHandle == nil)) {
		ThrowOSErr_(err);
	}
}


StHandleBlock::StHandleBlock(		// Copy Constructor
	const StHandleBlock	&inHandleBlock)
{
	mIsOwner = (inHandleBlock.mHandle != nil);
	mHandle  = inHandleBlock.mHandle;

	if (mHandle != nil) {		// Copy input's Handle
		ThrowIfOSErr_( ::HandToHand(&mHandle) );
	}
}


StHandleBlock::~StHandleBlock()		// Destructor
{
	Dispose();
}


StHandleBlock&						// Assignment Operator
StHandleBlock::operator = (
	const StHandleBlock	&inHandleBlock)
{
	if (this != &inHandleBlock) {
		CopyAndAdopt(inHandleBlock.mHandle);
	}

	return *this;
}


bool
StHandleBlock::IsAResource() const	// Is Handle a Resource?
{
		// GetResAttrs() sets ResError() to resNotFound if the
		// Handle is not a resource. This is better than calling
		// HGetState() and testing the resource bit because
		// HGetState() fails for nil master pointers (which occurs
		// when a Handle/Resource is purged).

	::GetResAttrs(mHandle);		// Ignore the return value

	return (::ResError() != resNotFound);
}


Handle
StHandleBlock::GetCopy() const		// Return a copy of our Handle
{
	Handle	theCopy = mHandle;
	
	if (theCopy != nil) {
		ThrowIfOSErr_( ::HandToHand(&theCopy) );
	}
	
	return theCopy;
}


void
StHandleBlock::Adopt(			// Take ownership of the input Handle
	Handle	inHandle)
{
	if (inHandle != mHandle) {	// Check for self-adoption
		Dispose();					// Dispose existing Handle

		mHandle  = inHandle;		// Store new Handle
	}
	
	mIsOwner = (inHandle != nil);
}


void
StHandleBlock::CopyAndAdopt(	// Adopt a copy of the input Handle
	Handle	inHandle)
{
	Handle	theCopy = inHandle;
	
	if (theCopy != nil) {		// Copy the input Handle
		ThrowIfOSErr_( ::HandToHand(&theCopy) );
	}
	
	Adopt(theCopy);
}


Handle
StHandleBlock::Release() const	// Relinquish ownership of our Handle
{
	mIsOwner = false;
	return mHandle;
}


void
StHandleBlock::Dispose()		// Dispose Handle if we own it
{
	if (mIsOwner && (mHandle != nil)) {

			// We must treat Resources different from regular Handles.
			// GetResAttrs() returns the attributes for a Resource,
			// or sets ResError() to resNotFound if the Handle isn't
			// a Resource.

#ifdef Debug_Signal
		SInt16	resAttrs = ::GetResAttrs(mHandle);
#endif

		if (!::ResError()) {	// No error, so it must be a Resource

				// If the changed bit is set for this Resource,
				// we are in trouble. ReleaseResource won't dispose of
				// a changed Resource, and we don't know whether to
				// save or discard the changes. This is a programmer
				// error, so we Signal.

			SignalIf_(resAttrs & (1 << resChangedBit));

			::ReleaseResource(mHandle);

		} else {				// It's a regular Handle

			::DisposeHandle(mHandle);
		}

		mIsOwner = false;
		mHandle  = nil;
	}
}


#pragma mark -
// ===========================================================================
//	¥ StClearHandleBlock Class
// ===========================================================================
//	Subclass of StHandleBlock.
//	Constructor allocates a Handle cleared to all zeroes

StClearHandleBlock::StClearHandleBlock(
	Size	inSize,					// Bytes to allocate
	Boolean	inThrowFail)			// Whether to Throw if allocate fails
{
	mHandle  = ::NewHandleClear(inSize);
	if (inThrowFail) {
		ThrowIfMemFail_(mHandle);
	}

	mIsOwner = (mHandle != nil);
}

#pragma mark -
// ===========================================================================
//	¥ StTempHandle Class
// ===========================================================================
//	Subclass of StHandleBlock.
//	Constructor allocates the Handle using temporary (System) memory

StTempHandle::StTempHandle(
	Size	inSize,					// Bytes to allocate
	Boolean	inThrowFail)			// Whether to Throw if allocate fails
{
	OSErr	err;
	mHandle = ::TempNewHandle(inSize, &err);
	if (inThrowFail  &&  (mHandle == nil)) {
		ThrowOSErr_(err);
	}

	mIsOwner = (mHandle != nil);
}


#pragma mark -
// ===========================================================================
//	¥ StPointerBlock Class
// ===========================================================================
//	Constructor allocates the Ptr
//	Destructor disposes of the Ptr

// ---------------------------------------------------------------------------
//	¥ StPointerBlock						Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor from an existing Ptr. Object takes ownership of the Ptr.
//
//	It's OK to pass nil for the Ptr. You can set the Ptr later by
//	calling Adopt().
//
//
//	Note that you will get an error if you write code such as:
//
//		StPointerBlock	theP ( nil );
//
//	about an ambiguous constructor call. Since nil is really just the
//	number zero, the constructor that takes a Size parameter followed by
//	two bools with default values also matches the call parameters.
//
//	To workaround this ambiguity, you need to use the constants Ptr_Nil
//	and Size_Zero, which have explicit types that satisfy the compiler.

StPointerBlock::StPointerBlock(
	Ptr		inPtr)
{
	mIsOwner = (inPtr != nil);
	mPtr	 = inPtr;
}


StPointerBlock::StPointerBlock(
	Ptr		inPtr,
	bool	inIsOwner)
{
	mPtr	 = inPtr;
	mIsOwner = inIsOwner && (inPtr != nil);
}


StPointerBlock::StPointerBlock(
	Size	inSize,					// Bytes to allocate
	bool	inThrowFail,			// Whether to Throw if allocate fails
	bool	inClearBytes)			// Whether to clear all bytes to zero
{
	mIsOwner = true;

	if (inClearBytes) {
		mPtr = ::NewPtrClear(inSize);
	} else {
		mPtr = ::NewPtr(inSize);
	}

	if (inThrowFail) {
		ThrowIfMemFail_(mPtr);
	}
}


StPointerBlock::StPointerBlock(			// Copy Constructor
	const StPointerBlock&	inPointerBlock)
{
	mPtr	 = CopyPointerBlock(inPointerBlock.Get());
	mIsOwner = (mPtr != nil);
}


StPointerBlock::~StPointerBlock()		// Destructor
{
	if (mIsOwner && (mPtr != nil)) {
		::DisposePtr(mPtr);
	}
}


StPointerBlock&
StPointerBlock::operator = (			// Assignment Operator
	const StPointerBlock&	inPointerBlock)
{
	if (this != &inPointerBlock) {
	
		CopyAndAdopt(inPointerBlock.Get());
	}

	return *this;
}


Ptr
StPointerBlock::CopyPointerBlock(
	Ptr		inPtr)
{
	Ptr		copyPtr = nil;

	if (inPtr != nil) {
		Size	theSize = ::GetPtrSize(inPtr);
		ThrowIfMemError_();

		copyPtr = ::NewPtr(theSize);
		ThrowIfMemFail_(copyPtr);

		::BlockMoveData(inPtr, copyPtr, theSize);
	}

	return copyPtr;
}


void
StPointerBlock::Adopt(				// Take ownership of the input Ptr
	Ptr		inPtr)
{
	if (inPtr != mPtr) {
		Dispose();

		mPtr	 = inPtr;
	}
	
	mIsOwner = (inPtr != nil);
}


void
StPointerBlock::CopyAndAdopt(		// Adopt a copy of the input Ptr
	Ptr		inPtr)
{
	Ptr	copyPtr	= CopyPointerBlock(inPtr);
	
	Adopt(copyPtr);
}


Ptr
StPointerBlock::Release() const		// Relinquish ownership of our Ptr
{
	mIsOwner = false;
	return mPtr;
}


void
StPointerBlock::Dispose()			// Dispose of Ptr if we own it
{
	if (mIsOwner && (mPtr != nil)) {
		::DisposePtr(mPtr);
	}

	mIsOwner = false;
	mPtr	 = nil;
}

#pragma mark -
// ===========================================================================
//	¥ StClearPointerBlock Class
// ===========================================================================
//	Subclass of StPointerBlock
//	Constructor allocates a Ptr cleared to all zeroes

StClearPointerBlock::StClearPointerBlock(
	Size	inSize,					// Bytes to allocate
	Boolean	inThrowFail)			// Whether to Throw if allocate fails
{
	mPtr = ::NewPtrClear(inSize);
	if (inThrowFail) {
		ThrowIfMemFail_(mPtr);
	}

	mIsOwner = (mPtr != nil);
}

#pragma mark -
// ===========================================================================
//	¥ StResource Class
// ===========================================================================
//	Constructor gets the resource handle
//	Destructor releases the resource handle

StResource::StResource(				// -- Adopt existing Resource Handle
	Handle	inResourceH)
{										// Caller is responsible for being
	mResourceH = inResourceH;			//   sure that inResourceH is really
}										//   a Handle to a Resource (or nil)


StResource::StResource(				// -- Get from Resource ID
	ResType	inResType,
	ResIDT	inResID,
	Boolean	inThrowFail,				// Throw if allocate fails ?
	Boolean	inCurrResOnly)				// Only look in current res file ?
{
	mResourceH = nil;
	GetResource(inResType, inResID, inThrowFail, inCurrResOnly);
}


StResource::StResource(				// -- Get from Resource name
	ResType				inResType,
	ConstStringPtr		inResName,
	Boolean				inThrowFail,
	Boolean				inCurrResOnly)
{
	mResourceH = nil;
	GetResource(inResType, inResName, inThrowFail, inCurrResOnly);
}


StResource::~StResource()
{
	Dispose();
}


Handle
StResource::GetResource(			// -- Re-assign from Resource ID
	ResType	inResType,
	ResIDT	inResID,
	Boolean	inThrowFail,
	Boolean	inCurrResOnly)
{
	Dispose();

	if (inCurrResOnly) {
		mResourceH = ::Get1Resource(inResType, inResID);
	} else {
		mResourceH = ::GetResource(inResType, inResID);
	}
	if (inThrowFail) {
		ThrowIfResFail_(mResourceH);
	}

	return mResourceH;
}


Handle
StResource::GetResource(			// -- Re-assign from Resource name
	ResType				inResType,
	ConstStringPtr		inResName,
	Boolean				inThrowFail,
	Boolean				inCurrResOnly)
{
	Dispose();

	if (inCurrResOnly) {
		mResourceH = ::Get1NamedResource(inResType, inResName);
	} else {
		mResourceH = ::GetNamedResource(inResType, inResName);
	}
	if (inThrowFail) {
		ThrowIfResFail_(mResourceH);
	}

	return mResourceH;
}


Handle
StResource::GetIndResource(			// -- Re-assign from Resource Index
	ResType	inResType,
	SInt16	inIndex,
	Boolean	inThrowFail,
	Boolean	inCurrResOnly)
{
	Dispose();

	if (inCurrResOnly) {
		mResourceH = ::Get1IndResource(inResType, inIndex);
	} else {
		mResourceH = ::GetIndResource(inResType, inIndex);
	}
	if (inThrowFail) {
		ThrowIfResFail_(mResourceH);
	}

	return mResourceH;
}


void
StResource::Adopt(				// -- Take ownership of the input Resource
	Handle	inResourceH)
{
	if (inResourceH != mResourceH) {
		Dispose();					// Dispose existing Resource

		mResourceH  = inResourceH;	// Store the new Resource
	}
}


void
StResource::Detach()				// -- Turn Resource into just a Handle
{
	if (mResourceH != nil) {
		::DetachResource(mResourceH);
	}
}


void
StResource::Dispose()				// -- Dispose Resource
{
	if (mResourceH != nil) {

			// Resource could have been detached, in which case it
			// is a regular Handle, which we must dispose of in a
			// different way. GetResAttrs() returns the attributes for
			// a Resource, or sets ResError() to resNotFound if the
			// Handle isn't a Resource

#ifdef Debug_Signal
		SInt16	resAttrs = ::GetResAttrs(mResourceH);
#endif

		if (!::ResError()) {	// No error, so it must be a Resource

				// If the changed bit is set for this Resource,
				// we are in trouble. ReleaseResource won't dispose of
				// a changed Resource, and we don't know whether to
				// save or discard the changes. This is a programmer
				// error, so we Signal.

			SignalIf_(resAttrs & (1 << resChangedBit));

			::ReleaseResource(mResourceH);

		} else {				// It's not a Resource, so it must
								//   be a regular Handle
			::DisposeHandle(mResourceH);
		}

		mResourceH = nil;
	}
}

#pragma mark -
// ===========================================================================
//	¥ Utility Functions
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ InitializeHeap
// ---------------------------------------------------------------------------
//	Call this function at the beginning of your program (before initializing
//	the Toolbox) to expand the heap zone to its maximum size and preallocate
//	a specified number of Master Pointer blocks.

void
InitializeHeap(
	SInt16	inMoreMasters)
{
#if TARGET_API_MAC_CARBON

	::MoreMasterPointers((UInt32) (64 * inMoreMasters));

#else

	::MaxApplZone();
	for (SInt16 i = 1; i <= inMoreMasters; i++) {
		::MoreMasters();
	}

#endif
}


// ---------------------------------------------------------------------------
//	¥ BlocksAreEqual
// ---------------------------------------------------------------------------
//	Blocks are equal if the first n bytes pointed to by s1 have the same
//	values as the first n bytes pointed to by s2. Note that this always
//	returns true if n is zero.

Boolean
BlocksAreEqual(
	const void	*s1,
	const void	*s2,
	UInt32		n)
{
	const unsigned char	*ucp1 = (const unsigned char *) s1;
	const unsigned char	*ucp2 = (const unsigned char *) s2;

	while (n > 0) {
		if (*ucp1++ != *ucp2++) {
			return false;
		}
		n--;
	}

	return true;
}


// ---------------------------------------------------------------------------
//	¥ BlockCompare(const void*, const void*, UInt32)
// ---------------------------------------------------------------------------
//	Compare two equal-length blocks of memory
//
//	Return values:
//
//		< 0		block 1 is less than block 2
//		0		block 1 is equal to block 2
//		> 0		block 1 is greater than block 2
//
// Note that this always returns 0 (equal) if n <= 0.

SInt32
BlockCompare(
	const void	*s1,
	const void	*s2,
	UInt32		n)
{
	const unsigned char	*ucp1 = (const unsigned char *) s1;
	const unsigned char	*ucp2 = (const unsigned char *) s2;

	while (n > 0) {
		if (*ucp1 != *ucp2) {
			return ((SInt32)(*ucp1 - *ucp2));
		}

		ucp1++;
		ucp2++;
		n--;
	}

	return 0;
}


// ---------------------------------------------------------------------------
//	¥ BlockCompare(const void*, const void*, UInt32, Unit32)
// ---------------------------------------------------------------------------
//	Compare two blocks of memory. Blocks may have different lengths.
//
//	Return values:
//
//		< 0		block 1 is less than block 2
//		0		block 1 is equal to block 2
//		> 0		block 1 is greater than block 2
//
// Note that this always returns 0 (equal) if n <= 0.

SInt32
BlockCompare(
	const void	*inPtr1,
	const void	*inPtr2,
	UInt32		inLen1,
	UInt32		inLen2)
{
	const unsigned char	*ucp1 = (const unsigned char *) inPtr1;
	const unsigned char	*ucp2 = (const unsigned char *) inPtr2;

	UInt32	len = inLen1;				// len is the shorter block length
	if (inLen2 < inLen1) {
		len = inLen2;
	}

	while (len > 0) {					// Compare byte by byte
		if (*ucp1 != *ucp2) {
			return ((SInt32)(*ucp1 - *ucp2));
		}

		ucp1++;
		ucp2++;
		len--;
	}
										// All bytes the same so far
	return (SInt32)(inLen1 - inLen2);	// Longer block is "bigger"
}

PP_End_Namespace_PowerPlant
