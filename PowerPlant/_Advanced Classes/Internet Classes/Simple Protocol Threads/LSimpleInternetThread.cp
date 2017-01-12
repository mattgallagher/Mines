// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSimpleInternetThread.cp	PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSimpleInternetThread.h>
#include <TArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSimpleInternetThread					Constructor				  [public]
// ---------------------------------------------------------------------------

LSimpleInternetThread::LSimpleInternetThread(LListener * inListener, Boolean inAutoDelete)
		: LThread(false, thread_DefaultStack, threadOption_Default, nil)
{
	InitMembers();

	mAutoDelete = inAutoDelete;

	if (inListener) {
		AddListener(inListener);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LSimpleInternetThread				Destructor				  [public]
// ---------------------------------------------------------------------------

LSimpleInternetThread::~LSimpleInternetThread()
{
	BroadcastMessage(Internet_ThreadDied, this);
	PurgeQueue();
}


// ---------------------------------------------------------------------------
//	¥ PurgeQueue												   [protected]
// ---------------------------------------------------------------------------

void
LSimpleInternetThread::PurgeQueue()
{
	LInternetEventElem * currEvent;
	TArrayIterator<LInternetEventElem*> iter(mEventArray);
	while (iter.Next(currEvent)) {
		delete currEvent;
	}
}


// ---------------------------------------------------------------------------
//	¥ InitMembers												   [protected]
// ---------------------------------------------------------------------------

void
LSimpleInternetThread::InitMembers()
{
	mContinue = true;
	mAutoDelete = false;
}


// ---------------------------------------------------------------------------
//	¥ QueueEvent													  [public]
// ---------------------------------------------------------------------------

void
LSimpleInternetThread::QueueEvent(LInternetEventElem * inEvent)
{
	mEventArray.AddItem(inEvent);
	this->Resume();
}


// ---------------------------------------------------------------------------
//	¥ KillThread													  [public]
// ---------------------------------------------------------------------------

void
LSimpleInternetThread::KillThread()
{
	mContinue = false;
	this->Resume();
}


// ---------------------------------------------------------------------------
//	¥ Run														   [protected]
// ---------------------------------------------------------------------------

void *
LSimpleInternetThread::Run()
{
	while (mContinue) {
		RunPendingCommands();

		if (mAutoDelete) {
			break;		//If auto-delete then just bail out
		} else {
			Suspend();	//Suspend until we have something to run again
		}
	}

	return nil;
}

#pragma mark -
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ LInternetEventElem					Constructor				  [public]
// ---------------------------------------------------------------------------

LInternetEventElem::LInternetEventElem(
	EInternetEvent	inEvent)
{
	mEvent			= inEvent;
	mUser[0]		= '\0';
	mPassword[0]	= '\0';
	mRemotePort		= 0;
}


// ---------------------------------------------------------------------------
//	¥ LInternetEventElem					Destructor				  [public]
// ---------------------------------------------------------------------------

LInternetEventElem::~LInternetEventElem()
{
}


PP_End_Namespace_PowerPlant
