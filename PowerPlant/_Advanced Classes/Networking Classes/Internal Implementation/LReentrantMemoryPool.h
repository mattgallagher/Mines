// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LReentrantMemoryPool.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LReentrantMemoryPool
#define _H_LReentrantMemoryPool
#pragma once

#include <PP_Prefix.h>
#include <LInterruptSafeList.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LRPFreeBlockInfo;
class LRPPoolBlockInfo;

struct SFreeBlockInfo;

// ===========================================================================
//		¥ LReentrantMemoryPool
// ===========================================================================
//	A reentrant memory manager for use by interrupt callbacks. Based on the
//	NEWMODE_FAST implementation of operator new.

class LReentrantMemoryPool {
public:
					LReentrantMemoryPool(
							UInt32 inInitialSize);
					~LReentrantMemoryPool();

	void			AddPool(
							UInt32 inPoolSize);

	// All of the following functions are reentrant and may be called
	// at interrupt time. They behave essentially identically to the
	// Memory Manager routines of the same names. Note that there
	// is no MemError() function and that no exceptions are thrown
	// by these methods. (Any errors are silently ignored.)

	void*			NewPtr(
							UInt32 inByteCount);
	void*			NewPtrClear(
							UInt32 inByteCount);
	void			DisposePtr(
							void* inPtr);

	UInt32			GetPtrSize(
							void* inPtr);

	UInt32			TotalMem();
	UInt32			FreeMem();

private:
	void*			AllocFrom (
							SFreeBlockInfo* inBlockInfo,
							UInt32 inByteCount);
	void			MakeFreeBlock (
							void* inBlock,
							UInt32 inByteCount);

	LInterruptSafeList		mMemoryPools;				// of LRPPoolBlockInfo
	LInterruptSafeList		mFreeBlocks;				// pointer to previously-used free block


					LReentrantMemoryPool();		// do not use
					LReentrantMemoryPool(LReentrantMemoryPool&);

};

// ===========================================================================
//		¥ LRMPObject
// ===========================================================================
//	Base class for objects which may be allocated in a reentrant memory pool.
//	Defines special operators new and delete which may be called at interrupt time.
//
//	USAGE NOTE: There are two operator new functions. The operator new without
//	placement operator CANNOT be called at interrupt time. It is a wrapper for the
//	general operator new. Objects created with this operator new cannot be deleted
//	at interrupt time.
//
//	The second operator new (syntax:  ÒCMyThing* thing = new(pool) CMyThing(...);Ó)
//	allocates space for the object using the NewPtr function of LReentrantMemoryPool.
//	Objects created with this operator new CAN be deleted at interrupt time or
//	any other time.
//
//	A 4-byte overhead is incurred for all objects to store the pointer to the memory pool.

class LRMPObject {

public:
	void*		operator new(
						UInt32		inByteCount);

	void		operator delete(
						void*		inPtr);

	void*		operator new(
						UInt32		inByteCount,
						LReentrantMemoryPool& inPool);

	void		operator delete(
						void*		inPtr,
						LReentrantMemoryPool& inPool);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
