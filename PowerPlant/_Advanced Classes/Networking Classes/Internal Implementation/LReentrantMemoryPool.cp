// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LReentrantMemoryPool.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	A reentrant memory manager for use by interrupt callbacks. Based on the
//	NEWMODE_FAST implementation of operator new.


#include <LReentrantMemoryPool.h>

#include <new>
#include <UException.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

typedef LReentrantMemoryPool* LRMPPointer;
struct SPoolInfo : public LInterruptSafeListMember {
	UInt32					itsSize;

	SPoolInfo(UInt32 inSize);
};

SPoolInfo::SPoolInfo(UInt32 inSize)
{
	itsSize = inSize;
}

// ---------------------------------------------------------------------------

struct SFreeBlockInfo : public LInterruptSafeListMember {
	UInt32					itsSize;

	SFreeBlockInfo(UInt32 inSize);
};

SFreeBlockInfo::SFreeBlockInfo(UInt32 inSize)
{
	itsSize = inSize;
}

// ===========================================================================
//		• LReentrantMemoryPool class
// ===========================================================================

// ---------------------------------------------------------------------------
//		• LReentrantMemoryPool
// ---------------------------------------------------------------------------
//	Constructor, specifying the initial size of the memory pool.
//	IMPORTANT: MUST NOT BE CALLED AT INTERRUPT TIME!

LReentrantMemoryPool::LReentrantMemoryPool (UInt32 inInitialSize)
{
	AddPool(inInitialSize);
}

// ---------------------------------------------------------------------------
//		• ~LReentrantMemoryPool
// ---------------------------------------------------------------------------
//	Destructor
//	IMPORTANT: MUST NOT BE CALLED AT INTERRUPT TIME!

LReentrantMemoryPool::~LReentrantMemoryPool (void)
{
	LInterruptSafeListIterator	theIterator(mMemoryPools);
	LInterruptSafeListMember*	theMember;

	// We assume that we won't be interrupted while doing this.
	// It would be a bad thing since for a few moments the free
	// blocks in the list are not contained within Mac blocks any more.
	// Remove each pool from the list and release it.

	while (theIterator.Next(theMember)) {
		mMemoryPools.Remove(theMember);
		::DisposePtr((Ptr) theMember);
	}
}

// ---------------------------------------------------------------------------
//		• AddPool
// ---------------------------------------------------------------------------
//	Call to add additional memory to a memory pool object. Uses the
//	MacOS Memory Manager to grab a large pointer block which is later
//	subdivided into smaller pointers. These pointers are never released
//	until the entire memory pool is destroyed.
//
//	IMPORTANT: MUST NOT BE CALLED AT INTERRUPT TIME!

void
LReentrantMemoryPool::AddPool (UInt32 inPoolSize)
{
	Ptr						thePool;
	SPoolInfo*				thePoolInfo;

	// Allocate a new memory pool of the requested size.

	ThrowIfMemFail_(thePool = ::NewPtr((SInt32) inPoolSize));

	// Initialize its header and create a single large free block inside it.

	thePoolInfo = new (thePool) SPoolInfo(inPoolSize);
	MakeFreeBlock(thePool + sizeof (SPoolInfo), inPoolSize - sizeof (SPoolInfo));

	// Place this new block in service.

	mMemoryPools.Append(thePoolInfo);
}

// ===========================================================================

#pragma mark -
#pragma mark •• reentrant routines

// ---------------------------------------------------------------------------
//		• NewPtr
// ---------------------------------------------------------------------------
//	Allocate a new non-relocatable block within the reentrant memory pool.
//	May be called at interrupt time.

void*
LReentrantMemoryPool::NewPtr (UInt32 inByteCount)
{
	// Sanity check: disallow enormous allocations.

	if (inByteCount > 0x7FFFFFF0) {
		return nil;				// ADM 000410 - Return nil to indicate error code.
	}

	// Round the size of the request to the nearest 4-byte multiple
	// (2-byte is necessary for correctness on 68000 processor,
	// 4-byte is necessary for speed on modern processors).
	// Add overhead for the used block header.

	inByteCount = sizeof (SFreeBlockInfo) + ((inByteCount + 3L) & 0xFFFFFFFC);

	// Loop through the free blocks looking for one which is large enough.

	LInterruptSafeListIterator 	theIterator(mFreeBlocks);
	LInterruptSafeListMember* 	theFreeBlock;
	SFreeBlockInfo*			theFreeBlockInfo;
	void*					theResult;

	while (theIterator.Next(theFreeBlock)) {
		theFreeBlockInfo = static_cast<SFreeBlockInfo*>(theFreeBlock);

		// If we find a suitable free block, try to allocate from it.
		// Allocation might fail if somebody interrupts us and grabs the block faster than us.
		// If so, continue looping, otherwise we can return this result.

		if (theFreeBlockInfo->itsSize >= inByteCount)
		{
			theResult = AllocFrom(theFreeBlockInfo, inByteCount);
			if (theResult)
				return theResult;
			else
				theIterator.Reset();	//Reset so that we start walking the list at the top
										// if AllocFrom failed.
		}
	}

	return nil;				// Return nil to indicate error code.
}


// ---------------------------------------------------------------------------
//		• NewPtrClear
// ---------------------------------------------------------------------------
//	Allocate a new non-relocatable block and clear it.
//	May be called at interrupt time.

void*
LReentrantMemoryPool::NewPtrClear (UInt32 inByteCount)
{
	void*					theResult;
	char*					thePtr;

	theResult = this->NewPtr(inByteCount);
	if (theResult)
		for (thePtr = static_cast<char*>(theResult); inByteCount; inByteCount--)
			*(thePtr++) = 0;

	return theResult;
}


// ---------------------------------------------------------------------------
//		• DisposePtr
// ---------------------------------------------------------------------------
//	Free a non-relocatable block from this memory pool.
//	May be called at interrupt time.

void
LReentrantMemoryPool::DisposePtr (void* inPtr)
{
	SFreeBlockInfo				*theBlockInfo, *thePrevBlockInfo, *theNextBlockInfo;

	if (inPtr) {
		theBlockInfo = (static_cast<SFreeBlockInfo*>(inPtr)) - 1;

		// Loop through free blocks to check whether this block
		//	should be merged with adjacent free blocks.

		LInterruptSafeListIterator		theIterator(mFreeBlocks);
		LInterruptSafeListMember*	theFreeBlock;

		thePrevBlockInfo = theNextBlockInfo = nil;
		while (theIterator.Next(theFreeBlock)) {

			// If the block before our block is free, remove it from the free block list.

			if (reinterpret_cast<Ptr>(theFreeBlock) +
				(static_cast<SFreeBlockInfo*>(theFreeBlock))->itsSize == reinterpret_cast<Ptr>(theBlockInfo)) {
				if (mFreeBlocks.Remove(theFreeBlock))
					thePrevBlockInfo = (SFreeBlockInfo*)theFreeBlock;
			}

			// If the block after our block is free, remove it from the free block list.

			else if (reinterpret_cast<Ptr>(theBlockInfo) +
						theBlockInfo->itsSize == reinterpret_cast<Ptr>(theFreeBlock)) {
				if (mFreeBlocks.Remove(theFreeBlock))
					theNextBlockInfo = static_cast<SFreeBlockInfo*>(theFreeBlock);
			}
		}

		// Make a free block which comprehends the current block plus the
		// adjacent blocks if they exist, and add it to the list of free blocks.

		if (thePrevBlockInfo && theNextBlockInfo)
			MakeFreeBlock(thePrevBlockInfo,
					thePrevBlockInfo->itsSize
					+ theBlockInfo->itsSize
					+ theNextBlockInfo->itsSize);
		else if (thePrevBlockInfo)
			MakeFreeBlock(thePrevBlockInfo,
					thePrevBlockInfo->itsSize
					+ theBlockInfo->itsSize);
		else if (theNextBlockInfo)
			MakeFreeBlock(theBlockInfo,
					theBlockInfo->itsSize
					+ theNextBlockInfo->itsSize);
		else
			mFreeBlocks.Append(theBlockInfo);
	}
}

// ---------------------------------------------------------------------------
//		• GetPtrSize
// ---------------------------------------------------------------------------
//	Return the size of the non-relocatable block at inPtr.
//	NOTE: The size will be rounded up to the nearest multiple of 4.
//
//	May be called at interrupt time.

UInt32
LReentrantMemoryPool::GetPtrSize (void* inPtr)
{
	SFreeBlockInfo*			theFreeBlockInfo;

	if (inPtr) {
		theFreeBlockInfo = (static_cast<SFreeBlockInfo*>(inPtr)) - 1;
		return theFreeBlockInfo->itsSize;
	}
	return 0;
}


// ---------------------------------------------------------------------------
//		• TotalMem
// ---------------------------------------------------------------------------
//	Return the total amount of memory used by this memory pool.
//	May be called at interrupt time.

UInt32
LReentrantMemoryPool::TotalMem (void)
{
	LInterruptSafeListIterator		theIterator(mMemoryPools);
	LInterruptSafeListMember*	thePool;
	UInt32					theTotalSize = 0L;

	// Loop through pools, adding up their sizes.
	// The size is stored to avoid calling ::GetPtrSize, which is not reentrant.

	while (theIterator.Next(thePool))
		theTotalSize += (static_cast<SPoolInfo*>(thePool))->itsSize;
	return theTotalSize;
}


// ---------------------------------------------------------------------------
//		• FreeMem
// ---------------------------------------------------------------------------
//	Return the total amount of free memory in this memory pool.
//	May be called at interrupt time.

UInt32
LReentrantMemoryPool::FreeMem (void)
{
	LInterruptSafeListIterator 	theIterator(mFreeBlocks);
	LInterruptSafeListMember*	theFreeBlock;
	UInt32					theFreeSize = 0L;

	// Loop through free blocks, adding up their sizes.

	while (theIterator.Next(theFreeBlock))
		theFreeSize += (static_cast<SFreeBlockInfo*>(theFreeBlock))->itsSize;

	return theFreeSize;
}


// ===========================================================================

#pragma mark -
#pragma mark •• implementation helpers (private)

// ---------------------------------------------------------------------------
//		• AllocFrom
// ---------------------------------------------------------------------------
void*
LReentrantMemoryPool::AllocFrom (SFreeBlockInfo* inBlockInfo, UInt32 inByteCount)
{
	Ptr						theNextBlock;
	UInt32					theNextSize;

	// Grab this block so we can operate on it.
	// This can fail if we were interrupted.

	if (!mFreeBlocks.Remove(inBlockInfo))
		return nil;

	// If this free block is large enough to store the requested
	// size *plus* a new, smaller, non-empty free block, then split it.

	if (inBlockInfo->itsSize > inByteCount + sizeof (SFreeBlockInfo)) {

		// There’s a bunch of extra space, so split this
		// block and create a new free block.

		theNextBlock = ((Ptr) inBlockInfo) + inByteCount;
		theNextSize = inBlockInfo->itsSize - inByteCount;
		inBlockInfo->itsSize = inByteCount;
		MakeFreeBlock(theNextBlock, theNextSize);
	}

	// Now return pointer to the user data block.

	return (inBlockInfo + 1);
}


// ---------------------------------------------------------------------------
//		• MakeFreeBlock
// ---------------------------------------------------------------------------

void
LReentrantMemoryPool::MakeFreeBlock (void* inBlock, UInt32 inByteCount)
{
	SFreeBlockInfo*			theFreeBlockInfo;

	theFreeBlockInfo = new (inBlock) SFreeBlockInfo(inByteCount);
	mFreeBlocks.Append(theFreeBlockInfo);
}

// ===========================================================================

#pragma mark -
#pragma mark === LRMPObject ===

// ---------------------------------------------------------------------------
//		• operator new
// ---------------------------------------------------------------------------
//	Operator new, using the global operator new and delete.
//	Stores a nil pointer to the memory pool as a signal that this object
//	was not created using a reentrant memory pool.
//
//	IMPORTANT: MUST NOT BE CALLED AT INTERRUPT TIME!
//	Also note: The object created by this operator new may NOT be
//	deleted at interrupt time.

void*
LRMPObject::operator new(
	UInt32 inByteCount)
{
	void* ptr = ::operator new(inByteCount + 4);

	LRMPPointer* poolPtr = (LRMPPointer*)ptr;
	*poolPtr = nil;
	return (++poolPtr);
}


// ---------------------------------------------------------------------------
//		• operator delete
// ---------------------------------------------------------------------------
//	Reads the pointer to the memory pool manager which was stored
//	by operator new. If the object was created using a pool, calls that
//	pool’s DisposePtr member function to release the memory; otherwise,
//	calls global operator delete.
//
//	May or may not be called at interrupt time, depending on when object
//	was created. (See notes for operator new above.)

void
LRMPObject::operator delete(
	void* inPtr)
{
	if (inPtr) {
		LRMPPointer* poolPtr = reinterpret_cast<LRMPPointer*>(inPtr);
		poolPtr--;
		if (*poolPtr)
			(*poolPtr)->DisposePtr(poolPtr);
		else
			::operator delete(poolPtr);
	}
}


// ---------------------------------------------------------------------------
//		• operator new
// ---------------------------------------------------------------------------
//	Operator new, using the reentrant memory pool manager.
//	Stores a pointer to the pool so that operator delete knows which
//	pool to use when releasing this object.
//
//	May be called at interrupt time. The object created by this operator new
//	may be deleted at any time (interrupt level or otherwise).

void*
LRMPObject::operator new(
	UInt32 				inByteCount,
	LReentrantMemoryPool&	inPool)
{
	void* ptr = inPool.NewPtr(inByteCount + sizeof (LRMPPointer));

	LRMPPointer* poolPtr = reinterpret_cast<LRMPPointer*>(ptr);
	*poolPtr = &inPool;
	return (++poolPtr);
}


// ---------------------------------------------------------------------------
//		• operator delete
// ---------------------------------------------------------------------------
//	Reads the pointer to the memory pool manager which was stored
//	by operator new. If the object was created using a pool, calls that
//	pool’s DisposePtr member function to release the memory; otherwise,
//	calls global operator delete.
//
//	May or may not be called at interrupt time, depending on when object
//	was created. (See notes for operator new above.)

void
LRMPObject::operator delete(
	void*		inPtr,
	LReentrantMemoryPool&	/* inPool */)
{
	if (inPtr) {
		LRMPPointer* poolPtr = reinterpret_cast<LRMPPointer*>(inPtr);
		poolPtr--;
		if (*poolPtr)
			(*poolPtr)->DisposePtr(poolPtr);
		else
			::operator delete(poolPtr);
	}
}


PP_End_Namespace_PowerPlant
