// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharedMemoryPool.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSharedMemoryPool
#define _H_LSharedMemoryPool
#pragma once

#include <LReentrantMemoryPool.h>
#include <LSharable.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

enum { LSharedMemoryPool_InitialSize = 1024 * 8 };

class LSharedMemoryPool	:	public LReentrantMemoryPool,
							public LSharable {
public:
								LSharedMemoryPool(UInt32 inPoolSize = LSharedMemoryPool_InitialSize);
								~LSharedMemoryPool();

	static LSharedMemoryPool *	GetSharedPool();

	void						AddPoolUser(
									void*	inUser,
									UInt32	inPoolSize = LSharedMemoryPool_InitialSize);

	void						RemovePoolUser(
									void*	inUser);

private:
	static LSharedMemoryPool *	sSharedMemoryPool;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
