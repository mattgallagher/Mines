// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	TMemoryMgr.h				PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================
//	Template version of UMemoryMgr
//
//	Template rewrite: John C. Daub <mailto:hsoi@pobox.com>
//	-----
//	Memory management utility classes and functions.
//
//	Stack-based Classes:
//		Several utility classes are designed for creating stack-based
//		objects, where the Constructor performs some action and the
//		Destructor undoes the action. The advantage of stack-based
//		objects is that the Destructor automatically gets called,
//		even when there is an exception thrown. This is useful to help
//		maintain resources, prevent leaks and dangling variables,
//		ensure exception safety, etc.
//
//	Templates:
//		The classes are implemented as templates to allow the use of
//		any similar, well-defined type, as opposed to the generic
//		Handle. This helps to avoid the need for (ugly) type casting.
//
//		NB: What "T" represents in each class varies. Check the comments
//		preceeding each template class declaration to see what "T"
//		is.

#ifndef _H_TMemoryMgr
#define _H_TMemoryMgr
#pragma once

#include <PP_Prefix.h>
#include <MacMemory.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


#pragma mark THandleLocker
// ===========================================================================
//	¥ THandleLocker
// ===========================================================================
//	Constructor locks the handle.
//	Destructor restores the handle's original locked/unlocked state.
//
//	"T" represents the Handle, e.g. THandleLocker<Handle> lock1(theH);

template <class T>
class	THandleLocker {
public:

			THandleLocker(
					T		inHandle)
				{
					mHandle			= nil;
					mWasUnlocked	= false;

					Adopt(inHandle);
				}

			~THandleLocker()
				{
					RestoreLockState();
				}

				// Take control over locking
				//   and unlocking this Handle
	void	Adopt(	T		inHandle)
				{
						// Restore state of current Handle
					RestoreLockState();

						// Store new Handle
					mHandle			= inHandle;
					mWasUnlocked	= false;

					if (inHandle != nil) {			// Lock new Handle
						SInt8	theState = ::HGetState(reinterpret_cast<Handle>(inHandle));
						ThrowIfMemError_();

							// Bit 7 of state is set if Handle is locked. We want to
							// know if the Handle is currently locked or unlocked.
							// If it is unlocked, we lock it now and unlock it the
							// destructor. If it is already locked, we do nothing
							// here and nothing in the destructor.

							// Unlocked if bit 7 is clear
						mWasUnlocked = (theState & 0x80) == 0;

						if (mWasUnlocked) {
							::HLock(reinterpret_cast<Handle>(inHandle));
						}
					}
				}

				// Relinquish control of Handle
	T		Release()
				{
					RestoreLockState();

					T	outHandle	= mHandle;

					mHandle			= nil;		// We now have no Handle
					mWasUnlocked	= false;

					return outHandle;
				}

private:
	T		mHandle;
	bool	mWasUnlocked;

	void	RestoreLockState()
				{
					if (mWasUnlocked && (mHandle != nil)) {
						::HUnlock(reinterpret_cast<Handle>(mHandle));
					}
				}
};


#pragma mark THandleState
// ===========================================================================
//	¥ THandleState
// ===========================================================================
//	Constructor saves handle state.
//	Destructor restores handle state.
//
//	"T" represents the Handle, e.g. THandleState<Handle> hState(theH);

template <class T>
class THandleState {
public:

			THandleState(
					T		inHandle)
				{
					mHandle		= inHandle;
					mSavedState	= ::HGetState(reinterpret_cast<Handle>(inHandle));
					ThrowIfMemError_();
				}

			~THandleState()
				{
					::HSetState(reinterpret_cast<Handle>(mHandle), mSavedState);
				}

private:
	T		mHandle;
	SInt8	mSavedState;
};


#pragma mark THandleBlock
// ===========================================================================
//	¥ THandleBlock
// ===========================================================================
//	Manages a Toolbox Handle
//
//	"T" represents the Handle, e.g. THandleBlock<Handle>	theH;

template <class T>
class	THandleBlock {
public:

			// Construct from an existing Handle. Object takes ownership
			// of the Handle. It's OK to pass nil for the Handle. You
			// can set the Handle later by calling Adopt().
			// NB: If you write code such as:
			//		THandleBlock<Handle>	theH(nil);
			// you will get a compiler error about an ambiguous constructor
			// call. Sine nil is really just the number zero, the constructor
			// that takes a Size parameter followed by two bool's with
			// default vaules also matches the cal parameters.
			// To work around this ambiguity, you need to use typed constants
			// such as Handle_Nil and Size_Zero (or define your own). The
			// key is to have an explicity type so the compiler knows if
			// you want to use a nil Handle or create a Handle containing
			// zero bytes.

			THandleBlock(
					T			inHandle = nil)
				{
					mIsOwner	=	(inHandle != nil);
					mHandle		=	inHandle;
				}

			THandleBlock(
					T			inHandle,
					bool		inIsOwner)
				{
					mHandle		= inHandle;
					mIsOwner	= inIsOwner && (inHandle != nil);
				}

			THandleBlock(
					Size		inSize,					// Bytes to allocate
					bool		inThrowFail = true,		// Throw if allocate fails?
					bool		inUseTempIfFull = false)// Use temp memory if app heap is full?
				{
					mIsOwner	=	true;
					mHandle		=	reinterpret_cast<T>(::NewHandle(inSize));	// Try to allocate in app heap

						// Failed to allocate in app heap
					OSErr err	=	noErr;
					if (mHandle == nil) {
						err = ::MemError();

							// Try to allocate in temporary memory
						if (inUseTempIfFull) {
							mHandle = reinterpret_cast<T>(::TempNewHandle(inSize, &err));
						}
					}

					if (inThrowFail && (mHandle == nil)) {
						ThrowOSErr_(err);
					}
				}

			THandleBlock(
					const THandleBlock<T>&	inHandleBlock)
				{
					mIsOwner	=	(inHandleBlock.mHandle != nil);
					mHandle		=	inHandleBlock.mHandle;

					if (mHandle != nil) {	// Copy input's Handle
						ThrowIfOSErr_( ::HandToHand(reinterpret_cast<Handle*>(&mHandle)) );
					}
				}

			~THandleBlock()
				{
					Dispose();
				}

	THandleBlock<T>&	operator = (const THandleBlock<T>&	inHandleBlock)
		{
			if (this != &inHandleBlock) {
					// Make copy of input's Handle
				T	theCopy = inHandleBlock.mHandle;
				if (theCopy != nil) {
					ThrowIfOSErr_( ::HandToHand(reinterpret_cast<Handle*>(&theCopy)) );
				}
					// Copy was successful

				Dispose();	// Delete existing Handle

					// Store copy as our Handle
				mIsOwner = (theCopy != nil);
				mHandle  = theCopy;
			}

			return *this;
		}

	operator	T () const			{ return mHandle; }

	T			Get() const			{ return mHandle; }
	Handle		GetAsHandle() const	{ return reinterpret_cast<Handle>(mHandle); }

	bool		IsOwner() const		{ return mIsOwner; }
	bool		IsValid() const		{ return (mHandle != nil); }

					// Is Handle a Resource?
	bool		IsAResource() const
					{
							// GetResAttrs() sets ResError() to resNotFound if the
							// Handle is not a resource. This is better that calling
							// HGetState() and testing the resource bit because
							// HGetState() fails for nil master pointers (which occurs
							// when a Handle/Resource is purged).

						(void)::GetResAttrs(reinterpret_cast<Handle>(mHandle));
						return (::ResError() != resNotFound);
					}

					// Take ownership of the input Handle
	void		Adopt(	T		inHandle)
					{
						if (inHandle != mHandle) {			// Check for self-adoption
							Dispose();						// Dispose existing Handle

							mHandle		= inHandle;			// Store new Handle
						}
						mIsOwner	= (inHandle != nil);
					}
					
	void		AdoptAsHandle(
						Handle	inHandle)
					{
						if (static_cast<void*>(inHandle)
							!= static_cast<void*>(mHandle)) {			// Check for self-adoption
							Dispose();

							mIsOwner	= (inHandle != nil);
							mHandle		= reinterpret_cast<T>(inHandle);
						}
					}

					// Relinquish ownership of our Handle
	T			Release() const
					{
						mIsOwner	= false;
						return mHandle;
					}
					
	Handle		ReleaseAsHandle() const
					{
						mIsOwner	= false;
						return (reinterpret_cast<Handle>(mHandle));
					}

					// Dispose Handle, if we own it
	void		Dispose()
					{
						if (mIsOwner && (mHandle != nil)) {

								// We must treat Resources different from regular Handles.
								// GetResAttrs() returns the attributes for a Resource,
								// or sets ResError() to resNotFound if the Handle isn't
								// a Resource.

							SInt16	resAttrs = ::GetResAttrs(reinterpret_cast<Handle>(mHandle));
							if (::ResError() == noErr) {	// No error, so it must be a Resource

									// If the changed bit is set for this Resource,
									// we are in trouble. ReleaseResource won't dispose of
									// a changed Resource, and we don't know whether to
									// save or discard the changes. This is a programmer
									// error, so we Signal.

								SignalIf_(resAttrs & (1 << resChangedBit));
								::ReleaseResource(reinterpret_cast<Handle>(mHandle));

							} else {				// It's a regular Handle

								::DisposeHandle(reinterpret_cast<Handle>(mHandle));
							}

							mIsOwner = false;
							mHandle  = nil;
						}
					}

protected:
	T				mHandle;
	mutable bool	mIsOwner;
};


#pragma mark TClearHandleBlock
// ===========================================================================
//	¥ TClearHandleBlock
// ===========================================================================
//	Constructor allocates the Handle (cleared to all zeroes).
//	Destructor disposes of the Handle.
//
//	"T" represents the Handle, e.g. TClearHandleBlock<Handle> theClearH;

template <class T>
class TClearHandleBlock : public THandleBlock<T> {
public:

	TClearHandleBlock(
			Size	inSize,					// Bytes to allocate
			bool	inThrowFail = true)		// Whether to Throw if allocate fails
		{
			mHandle = reinterpret_cast<T>(::NewHandleClear(inSize));
			if (inThrowFail) {
				ThrowIfMemFail_(mHandle);
			}

			mIsOwner = (mHandle != nil);
		}

};


#pragma mark TTempHandle
// ===========================================================================
//	¥ TTempHandle
// ===========================================================================
//	Constructor allocates the Handle using temporary (System) memory.
//	Destructor disposes of the Handle.
//
//	"T" represents the Handle, e.g. TTempHandle<Handle> theTempH;

template <class T>
class TTempHandle : public THandleBlock<T> {
public:

	TTempHandle(
			Size	inSize,					// Bytes to allocate
			bool	inThrowFail = true)		// Whether to Throw if allocate fails
		{
			OSErr	err;
			mHandle = reinterpret_cast<T>(::TempNewHandle(inSize, &err));
			if (inThrowFail && (mHandle == nil)) {
				ThrowOSErr_(err);
			}

			mIsOwner = (mHandle != nil);
		}
};


#pragma mark TPointerBlock
// ===========================================================================
//	¥ TPointerBlock Class
// ===========================================================================
//	Constructor allocates the Ptr
//	Destructor disposes of the Ptr
//
//	"T" represents the Ptr, e.g. TPointerBlock<Ptr> thePtr;

template <class T>
class	TPointerBlock {
public:
			// Construct from an existing Ptr. Object takes ownership
			// of the Ptr. It's OK to pass nil for the Ptr. You
			// can set the Ptr later by calling Adopt().
			// NB: If you write code such as:
			//		TPtrBlock<Ptr>	theP(nil);
			// you will get a compiler error about an ambiguous constructor
			// call. Sine nil is really just the number zero, the constructor
			// that takes a Size parameter followed by two bool's with
			// default vaules also matches the cal parameters.
			// To work around this ambiguity, you need to use typed constants
			// such as Ptr_Nil and Size_Zero (or define your own). The
			// key is to have an explicity type so the compiler knows if
			// you want to use a nil Ptr or create a Ptr containing
			// zero bytes.

			TPointerBlock(
					T		inPtr = nil)
				{
					mIsOwner	= (inPtr != nil);
					mPtr		= inPtr;
				}

			TPointerBlock(
					T		inPtr,
					bool	inIsOwner)
				{
					mPtr		= inPtr;
					mIsOwner	= inIsOwner && (inPtr != nil);
				}

			TPointerBlock(
					Size	inSize,					// Bytes to allocate
					bool	inThrowFail = true,		// Whether to Throw if allocate fails
					bool	inClearBytes = false)	// Whether to clear all bytes to zero
				{
					mIsOwner	= true;

					if (inClearBytes) {
						mPtr = reinterpret_cast<T>(::NewPtrClear(inSize));
					} else {
						mPtr = reinterpret_cast<T>(::NewPtr(inSize));
					}

					if (inThrowFail) {
						ThrowIfMemFail_(mPtr);
					}
				}

			TPointerBlock(
					const TPointerBlock<T>&	inPointerBlock)
				{
					mPtr = CopyPointerBlock(inPointerBlock.Get());
					mIsOwner = (mPtr != nil);
				}

			~TPointerBlock()
				{
					if (mIsOwner && (mPtr != nil)) {
						::DisposePtr(reinterpret_cast<Ptr>(mPtr));
					}
				}

	TPointerBlock<T>&	operator = (const TPointerBlock<T>&	inPointerBlock)
		{
			if (this != &inPointerBlock) {

				T copyPtr = CopyPointerBlock(inPointerBlock.Get());

				Dispose();

				mIsOwner = (copyPtr != nil);
				mPtr = copyPtr;
			}

			return *this;
		}

	operator T () const			{ return mPtr; }

	T		Get() const			{ return mPtr; }
	Ptr		GetAsPtr() const	{ return (reinterpret_cast<Ptr>(mPtr)); }

	bool	IsOwner() const		{ return mIsOwner; }
	bool	IsValid() const		{ return (mPtr != nil); }

	void	Adopt(	T		inPtr)
				{
					if (inPtr != mPtr) {
						Dispose();

						mPtr		= inPtr;
					}
					mIsOwner	= (inPtr != nil);
				}
				
	void	AdoptAsPtr(
					Ptr		inPtr)
				{
					if (inPtr != nil) {
						Dispose();

						mPtr		= reinterpret_cast<T>(inPtr);
					}
					mIsOwner	= (inPtr != nil);
				}

	T		Release() const
				{
					mIsOwner	= false;
					return mPtr;
				}
				
	Ptr		ReleaseAsPtr() const
				{
					mIsOwner	= false;
					return (reinterpret_cast<Ptr>(mPtr));
				}

	void	Dispose()
				{
					if (mIsOwner && (mPtr != nil)) {
						::DisposePtr(reinterpret_cast<Ptr>(mPtr));

						mIsOwner	= false;
						mPtr		= nil;
					}
				}

protected:
	T				mPtr;
	mutable bool	mIsOwner;

	T		CopyPointerBlock(
					T		inPtr)
				{
					T	copyPtr = nil;

					if (inPtr != nil) {
						Size theSize = ::GetPtrSize(reinterpret_cast<Ptr>(inPtr));
						ThrowIfMemError_();

						copyPtr = reinterpret_cast<T>(::NewPtr(theSize));
						ThrowIfMemFail_(copyPtr);

						::BlockMoveData(inPtr, copyPtr, theSize);
					}

					return copyPtr;
				}
};


#pragma mark TClearPointerBlock
// ===========================================================================
//	¥ TClearPointerBlock Class
// ===========================================================================
//	Subclass of StPointerBlock
//	Constructor allocates a Ptr cleared to all zeroes
//
//	"T" represents the Ptr, e.g. TClearPointerBlock<Ptr> theClearPtr;

template <class T>
class TClearPointerBlock : public TPointerBlock<T> {
public:

	TClearPointerBlock(
			Size	inSize,
			bool	inThrowFail = true)
		{
			mPtr = reinterpret_cast<T>(::NewPtrClear(inSize) );
			if (inThrowFail) {
				ThrowIfMemFail_(mPtr);
			}

			mIsOwner = (mPtr != nil);
		}
};


#pragma mark TRoutineDescriptor
// ===========================================================================
//	¥ TRoutineDescriptor
// ===========================================================================
//	Template class for automatically disposing of a RoutineDescriptor
//
//	"T" represents the UPP, e.g. TRoutineDescriptor<FooUPP> theUPP;

template <class T>
class TRoutineDescriptor {
public:
		TRoutineDescriptor( T	inUPP = nil)
			{
				mUPP = inUPP;
			}

		~TRoutineDescriptor()
			{
				if (mUPP != nil) {
					DisposeRoutineDescriptor(mUPP);
				}
			}

	void SetUPP( T	inUPP)
			{
				mUPP = inUPP;
			}

	operator T () const	{ return mUPP; }

protected:
	T	mUPP;
};


#pragma mark TResource
// ===========================================================================
//	¥ TResource Class
// ===========================================================================
//	Constructor gets the resource handle
//	Destructor releases the resource handle
//
//	"T" represents the Handle, e.g. TResorce<StringHandle> theString;

template <class T>
class TResource {
public:

					// -- Adopt existing Resource Handle
				TResource(
						T				inResourceH = nil)
					{
						// Caller is responsible for being
						//   sure that inResourceH is really
						//   a Handle to a Resource (or nil)
						mResourceH = inResourceH;
					}

					// -- Get from Resource ID
				TResource(
						ResType			inResType,
						ResIDT			inResID,
						bool			inThrowFail = true,		// Throw if allocate fails ?
						bool			inCurrResOnly = false)	// Only look in current res file ?
					{
						mResourceH = nil;
						GetResource(inResType, inResID, inThrowFail, inCurrResOnly);
					}

					// -- Get from Resource name
				TResource(
						ResType			inResType,
						ConstStringPtr	inResName,
						bool			inThrowFail = true,
						bool			inCurrResOnly = false)
					{
						mResourceH = nil;
						GetResource(inResType, inResName, inThrowFail, inCurrResOnly);
					}

				~TResource()
					{
						Dispose();
					}

	operator	T () const	{ return mResourceH; }

					// -- Re-assign from Resource ID
	T			GetResource(
						ResType			inResType,
						ResIDT			inResID,
						bool			inThrowFail = true,
						bool			inCurrResOnly = false)
					{
						Dispose();

						if (inCurrResOnly) {
							mResourceH = reinterpret_cast<T>
											(::Get1Resource(inResType, inResID));
						} else {
							mResourceH = reinterpret_cast<T>
											(::GetResource(inResType, inResID));
						}
						if (inThrowFail) {
							ThrowIfResFail_((reinterpret_cast<Handle>(mResourceH)));
						}

						return mResourceH;
					}

					// -- Re-assign from Resource name
	T			GetResource(
						ResType			inResType,
						ConstStringPtr	inResName,
						bool			inThrowFail = true,
						bool			inCurrResOnly = false)
					{
						Dispose();

						if (inCurrResOnly) {
							mResourceH = reinterpret_cast<T>
											(::Get1NamedResource(inResType, inResName));
						} else {
							mResourceH = reinterpret_cast<T>
											(::GetNamedResource(inResType, inResName));
						}
						if (inThrowFail) {
							ThrowIfResFail_((reinterpret_cast<Handle>(mResourceH)));
						}

						return mResourceH;
					}

					// -- Re-assign from Resource Index
	T			GetIndResource(
						ResType			inResType,
						SInt16			inIndex,
						bool			inThrowFail = true,
						bool			inCurrResOnly = false)
					{
						Dispose();

						if (inCurrResOnly) {
							mResourceH = reinterpret_cast<T>
											(::Get1IndResource(inResType, inIndex));
						} else {
							mResourceH = reinterpret_cast<T>
											(::GetIndResource(inResType, inIndex));
						}
						if (inThrowFail) {
							ThrowIfResFail_((reinterpret_cast<Handle>(mResourceH)));
						}

						return mResourceH;
					}

	T			Get() const			{ return mResourceH; }
	Handle		GetAsHandle() const { return reinterpret_cast<Handle>(mResourceH); }

	bool		IsValid() const		{ return (mResourceH != nil); }

	void		Adopt(	T	inResourceH)
					{
						if (inResourceH != mResourceH) {
							Dispose();

							mResourceH = inResourceH;
						}
					}
					
	void		AdoptAsHandle(
						Handle	inResourceH)
					{
						if (inResourceH != mResourceH) {
							Dispose();

							mResourceH = reinterpret_cast<T>(inResourceH);
						}
					}

	void		Detach()
					{
						if (mResourceH != nil) {
							::DetachResource(reinterpret_cast<Handle>(mResourceH));
						}
					}

	void		Dispose()
					{
						if (mResourceH != nil) {
							SInt16	resAttrs = ::GetResAttrs(reinterpret_cast<Handle>(mResourceH));
							if (!::ResError()) {
								SignalIf_(resAttrs & (1 << resChangedBit));
								::ReleaseResource(reinterpret_cast<Handle>(mResourceH));
							} else {
								::DisposeHandle(reinterpret_cast<Handle>(mResourceH));
							}
							mResourceH = nil;
						}
					}

	T			mResourceH;

private:						// Disallow copy and assignment
					TResource(const TResource<T>& inOriginal);
	TResource<T>&	operator=(const TResource<T>& inRhs);
};


#pragma mark TValueChanger
// ===========================================================================
//	¥ TValueChanger Class
// ===========================================================================
//	Template class for temporarily changing the value of a variable
//
//	Constructor saves the original value and changes it to a new value
//	Destructor restores the original value
//
//	"T" represents the "value, e.g. StValueChanger<EDebugAction> or
//	StValueChanger<bool>

template <class T>
class	TValueChanger {
public:
			TValueChanger(T& ioVariable, const T& inNewValue)
				: mVariable(ioVariable), mOrigValue(ioVariable)
				{
					ioVariable = inNewValue;
				}

			~TValueChanger()
				{
					mVariable = mOrigValue;
				}

protected:
	T&		mVariable;
	T		mOrigValue;
};


#pragma mark TDeleter
// ===========================================================================
//	¥ TDeleter
// ===========================================================================
//	PowerPlant implementation of the standard C++ auto_ptr template class
//
//	"T" represents the base type of the pointer,
//	e.g. StDeleter<LPaneClass> thePane(new LPaneClass);

template <class T>
class	TDeleter {
public:
			TDeleter()								// Default Constructor
				{
					mIsOwner = false;
					mPtr	 = nil;
				}

			explicit TDeleter(T* inPtr)				// Constructor
				{
					mIsOwner = (inPtr != nil);
					mPtr	 = inPtr;
				}

			TDeleter(const TDeleter& inDeleter)		// Copy Constructor
				{
					mIsOwner = inDeleter.mIsOwner;
					mPtr	 = inDeleter.Release();
				}

													// Assignment Operator
			TDeleter&	operator = (const TDeleter& inDeleter)
				{
					if (static_cast<const void*>(&inDeleter)
						!= static_cast<void*>(this)) {
						if (mIsOwner) {
							delete mPtr;
						}
						mIsOwner = inDeleter.mIsOwner;
						mPtr	 = inDeleter.Release();
					}
					return *this;
				}

			~TDeleter()								// Destructor
				{
					if (mIsOwner) {
						delete mPtr;
					}
				}

	T&		operator * () const		{ return *mPtr; }

	T*		operator -> () const	{ return mPtr; }

	T*		Get() const				{ return mPtr; }

	T*		Release() const
				{
					mIsOwner = false;
					return mPtr;
				}

	void	Adopt(T* inPtr)
				{
					if (inPtr != mPtr) {
						if (mIsOwner) {
							delete mPtr;
						}
						mPtr	 = inPtr;
					}
					mIsOwner = (inPtr != nil);
				}

	bool	IsOwner() const			{ return mIsOwner; }

private:
		mutable bool	mIsOwner;
		T*				mPtr;
};


#pragma mark TArrayDeleter
// ===========================================================================
//	¥ TArrayDeleter
// ===========================================================================
//	PowerPlant implementation of a variation upon the standard C++ auto_ptr
//	template class -- a version that works with new[]/delete[].
//
//	"T" represents the base type of the pointer,
//	e.g. StDeleter<LPaneClass> thePane(new LPaneClass[i]);

template <class T>
class	TArrayDeleter {
public:
				// Default Constructor
			TArrayDeleter()
				{
					mIsOwner = false;
					mPtr	 = nil;
				}

				// Constructor
			explicit TArrayDeleter(T* inPtr)
				{
					mIsOwner = (inPtr != nil);
					mPtr	 = inPtr;
				}

				// Copy Constructor
			TArrayDeleter(const TArrayDeleter& inDeleter)
				{
					mIsOwner = inDeleter.mIsOwner;
					mPtr	 = inDeleter.Release();
				}

				// Assignment Operator
			TArrayDeleter&	operator = (const TArrayDeleter& inDeleter)
				{
					if (static_cast<const void*>(&inDeleter)
						!= static_cast<void*>(this)) {
						if (mIsOwner) {
							delete[] mPtr;
						}
						mIsOwner = inDeleter.mIsOwner;
						mPtr	 = inDeleter.Release();
					}
					return *this;
				}

				// Destructor
			~TArrayDeleter()
				{
					if (mIsOwner) {
						delete[] mPtr;
					}
				}

	T&		operator * () const		{ return *mPtr; }

	T*		operator -> () const	{ return mPtr; }

	T*		Get() const				{ return mPtr; }

	T*		Release() const
				{
					mIsOwner = false;
					return mPtr;
				}

	void	Adopt(T* inPtr)
				{
					if (inPtr != mPtr) {
						if (mIsOwner) {
							delete[] mPtr;
						}
						mPtr	 = inPtr;
					}
					mIsOwner = (inPtr != nil);
				}

	bool	IsOwner() const			{ return mIsOwner; }

private:
		mutable bool	mIsOwner;
		T*				mPtr;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_TMemoryMgr
