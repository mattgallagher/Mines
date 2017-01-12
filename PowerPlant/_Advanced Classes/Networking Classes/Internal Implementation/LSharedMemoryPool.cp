// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharedMemoryPool.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Implements static shared memory pool that creates/delete itself as needed.

#include <LSharedMemoryPool.h>

PP_Begin_Namespace_PowerPlant

LSharedMemoryPool * LSharedMemoryPool::sSharedMemoryPool = nil;

// ---------------------------------------------------------------------------
//		¥ LSharedMemoryPool
// ---------------------------------------------------------------------------
//	Constructor

LSharedMemoryPool::LSharedMemoryPool(UInt32 inPoolSize)
	: LReentrantMemoryPool(inPoolSize)
{
	//Create a second pool so that we minimize problems with
	//	LReentrantMemoryPool::AllocFrom being interrupted and falsely
	//	reporting no data available.
	AddPool(inPoolSize);
}

// ---------------------------------------------------------------------------
//		¥ LSharedMemoryPool
// ---------------------------------------------------------------------------
//	Destructor

LSharedMemoryPool::~LSharedMemoryPool()
{
	LSharedMemoryPool::sSharedMemoryPool = nil;
}

// ---------------------------------------------------------------------------
//		¥ GetSharedPool
// ---------------------------------------------------------------------------
//	Returns pointer to static shared memory pool. Creates pool if necessary.

LSharedMemoryPool *
LSharedMemoryPool::GetSharedPool()
{
	if (!sSharedMemoryPool)
			sSharedMemoryPool = new LSharedMemoryPool();

	return sSharedMemoryPool;
}

// ---------------------------------------------------------------------------
//		¥ AddPoolUser
// ---------------------------------------------------------------------------
//	Adds a user to the pool. Can NOT be called at interrupt time.

void
LSharedMemoryPool::AddPoolUser(
	void*	inUser,
	UInt32	inPoolSize)
{
	if (FreeMem() < inPoolSize)
		AddPool(inPoolSize);

	AddUser(inUser);
}

// ---------------------------------------------------------------------------
//		¥ RemovePoolUser
// ---------------------------------------------------------------------------
//	Removes a user from the pool.

void
LSharedMemoryPool::RemovePoolUser(
	void*	inUser)
{
	RemoveUser(inUser);
}

PP_End_Namespace_PowerPlant
