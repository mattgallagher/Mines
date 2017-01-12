// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleThread.cp			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSimpleThread.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSimpleThread							Constructor				  [public]
// ---------------------------------------------------------------------------

LSimpleThread::LSimpleThread(
	ThreadProc				proc,
	void					*arg,
	Boolean					preempt,
	UInt32					stack,
	LThread::EThreadOption	flags)

	: LThread(preempt, stack, flags)
{
	THREAD_ASSERT(proc != NULL);

	mProc	= proc;
	mArg	= arg;
}


// ---------------------------------------------------------------------------
//	¥ Run
// ---------------------------------------------------------------------------
//	Execute the code for this thread.

void	*LSimpleThread::Run()
{
	(*mProc)(*this, mArg);

	return mResult;
}


PP_End_Namespace_PowerPlant
