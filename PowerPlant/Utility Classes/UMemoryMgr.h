// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UMemoryMgr.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UMemoryMgr
#define _H_UMemoryMgr
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
class	StHandleLocker {
public:
	StHandleLocker(Handle inHandle);
	~StHandleLocker();

	void	Adopt(Handle inHandle);
	Handle	Release();

protected:
	Handle	mHandle;
	bool	mWasUnlocked;

	void	RestoreLockState();
};


// ---------------------------------------------------------------------------
class	StHandleState {
public:
	StHandleState(Handle inHandle);
	~StHandleState();

protected:
	Handle	mHandle;
	SInt8	mSavedState;
};


// ---------------------------------------------------------------------------
class	StHandleBlock {
public:
	StHandleBlock(	Handle	inHandle = Handle_Nil);

	StHandleBlock(	Handle	inHandle,
					bool	inIsOwner);

	StHandleBlock(	Size	inSize,
					bool	inThrowFail = true,
					bool	inUseTempIfFull = false);

	StHandleBlock(	const StHandleBlock &inHandleBlock);

	~StHandleBlock();

	StHandleBlock&	operator = (const StHandleBlock &inHandleBlock);

	operator	Handle() const	{ return mHandle; }

	Handle		Get() const		{ return mHandle; }
	bool		IsOwner() const	{ return mIsOwner; }
	bool		IsValid() const { return (mHandle != nil); }

	bool		IsAResource() const;
	
	Handle		GetCopy() const;

	void		Adopt( Handle inHandle);
	void		CopyAndAdopt( Handle inHandle);
	Handle		Release() const;
	void		Dispose();

protected:
	Handle			mHandle;
	mutable bool	mIsOwner;
};

// ---------------------------------------------------------------------------
class	StClearHandleBlock : public StHandleBlock {
public:
	StClearHandleBlock(	Size	inSize,
						Boolean	inThrowFail = true);
};


// ---------------------------------------------------------------------------
class	StTempHandle : public StHandleBlock {
public:
	StTempHandle(	Size	inSize,
					Boolean	inThrowFail = true);
};


// ---------------------------------------------------------------------------
class	StPointerBlock {
public:
	StPointerBlock( Ptr		inPtr = Ptr_Nil);

	StPointerBlock( Ptr		inPtr,
					bool	inIsOwner);

	StPointerBlock(	Size	inSize,
					bool	inThrowFail = true,
					bool	inClearBytes = false);

	StPointerBlock( const StPointerBlock &inPointerBlock);

	~StPointerBlock();

	StPointerBlock&	operator = (const StPointerBlock &inPointerBlock);

	operator Ptr()			{ return mPtr; }

	Ptr		Get() const		{ return mPtr; }
	bool	IsOwner() const	{ return mIsOwner; }
	bool	IsValid() const	{ return (mPtr != nil); }

	void	Adopt( Ptr inPtr);
	void	CopyAndAdopt( Ptr inPtr);
	Ptr		Release() const;
	void	Dispose();
	
	static Ptr		CopyPointerBlock(Ptr inPtr);

protected:
	Ptr				mPtr;
	mutable bool	mIsOwner;
};


// ---------------------------------------------------------------------------
class	StClearPointerBlock : public StPointerBlock {
public:
	StClearPointerBlock(	Size	inSize,
							Boolean	inThrowFail = true);
};


// ---------------------------------------------------------------------------
//	Template class for automatically disposing of a RoutineDescriptor
//
//	Only valid for Classic. On Carbon, all routine descriptors are specific
//	to a particular routine.

#if PP_Target_Classic

template <class T> class StRoutineDescriptor {
public:
	StRoutineDescriptor( T	inUPP = nil )
		{
			mUPP = inUPP;
		}

	~StRoutineDescriptor()
		{
			if (mUPP != nil) {
				DisposeRoutineDescriptor((UniversalProcPtr) mUPP);
			}
		}

	void SetUPP( T	inUPP )
		{
			mUPP = inUPP;
		}

	operator T() 	{ return mUPP; }

protected:
	T	mUPP;
};

#endif


// ---------------------------------------------------------------------------
class	StResource {
public:
	StResource(	Handle			inResourceH = nil);

	StResource(	ResType			inResType,
				ResIDT			inResID,
				Boolean			inThrowFail = true,
				Boolean			inCurrResOnly = false);

	StResource(	ResType			inResType,
				ConstStringPtr	inResName,
				Boolean			inThrowFail = true,
				Boolean			inCurrResOnly = false);

	~StResource();

	operator Handle() const	{ return mResourceH; }

	Handle	GetResource(
				ResType			inResType,
				ResIDT			inResID,
				Boolean			inThrowFail = true,
				Boolean			inCurrResOnly = false);

	Handle	GetResource(
				ResType			inResType,
				ConstStringPtr	inResName,
				Boolean			inThrowFail = true,
				Boolean			inCurrResOnly = false);

	Handle	GetIndResource(
				ResType			inResType,
				SInt16			inIndex,
				Boolean			inThrowFail = true,
				Boolean			inCurrResOnly = false);

	Handle	Get() const		{ return mResourceH; }
	bool	IsValid() const	{ return (mResourceH != nil); }

	void	Adopt(	Handle		inResourceH);

	void	Detach();
	void	Dispose();

	Handle	mResourceH;

private:						// Disallow copy and assignment
	StResource(const StResource&);
	StResource&	operator = (const StResource&);
};


// ---------------------------------------------------------------------------
//	Template class for temporarily changing the value of a variable
//
//	Constructor saves the original value and changes it to a new value
//	Destructor restores the original value

template <class T>
class	StValueChanger {
public:
			StValueChanger( T& ioVariable, const T& inNewValue )
				: mVariable(ioVariable),
				  mOrigValue(ioVariable)
				{
					ioVariable = inNewValue;
				}
				
			StValueChanger( T& inVariable )
				: mVariable(inVariable),
				  mOrigValue(inVariable)
				{
				}

			~StValueChanger()
				{
					mVariable = mOrigValue;
				}

protected:
	T&	mVariable;
	T	mOrigValue;
};


// ---------------------------------------------------------------------------
//	PowerPlant implementation of the standard C++ auto_ptr template class

template <class T>
class	StDeleter {
public:
			StDeleter()								// Default Constructor
				{
					mIsOwner = false;
					mPtr	 = nil;
				}

			explicit StDeleter(T* inPtr)			// Constructor
				{
					mIsOwner = (inPtr != nil);
					mPtr	 = inPtr;
				}

			StDeleter(const StDeleter &inDeleter)	// Copy Constructor
				{
					mIsOwner = inDeleter.mIsOwner;
					mPtr	 = inDeleter.Release();
				}

													// Assignment Operator
			StDeleter&	operator = (const StDeleter &inDeleter)
				{
					if ((void*) &inDeleter != (void*) this) {
						if (mIsOwner) {
							delete mPtr;
						}
						mIsOwner = inDeleter.mIsOwner;
						mPtr	 = inDeleter.Release();
					}
					return *this;
				}

			~StDeleter()							// Destructor
				{
					if (mIsOwner) {
						delete mPtr;
					}
				}

	operator T*() const				{ return mPtr; }

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


// ---------------------------------------------------------------------------
void		InitializeHeap(	SInt16 inMoreMasters);

Boolean		BlocksAreEqual(	const void*		s1,
							const void*		s2,
							UInt32			n);

SInt32 		BlockCompare(	const void*		s1,
							const void*		s2,
							UInt32			n);

SInt32 		BlockCompare(	const void*		s1,
							const void*		s2,
							UInt32			n1,
							UInt32			n2);

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
