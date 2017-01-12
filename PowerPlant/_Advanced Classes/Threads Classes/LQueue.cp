// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LQueue.cp					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

// PowerPlant headers
#include <LQueue.h>
#include <LLink.h>
#include <UDebugging.h>
#include <UException.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LQueue
// ---------------------------------------------------------------------------
//	Default Constructor

LQueue::LQueue()
{
	// initialise member variables to known values
	mFirst	= NULL;
	mLast	= NULL;
	mSize	= 0;
}


// ---------------------------------------------------------------------------
//	¥ ~LQueue
// ---------------------------------------------------------------------------
//	Destructor  -- just a placeholder, really

LQueue::~LQueue()
{
	// there's nothing to do
}


// ---------------------------------------------------------------------------
//	¥ NextPut
// ---------------------------------------------------------------------------
//	Adds the given element to the end of the queue

void
LQueue::NextPut(LLink* inLinkP)
{
	// add element to the end of the queue
	inLinkP->SetLink(NULL);
	if (mLast != NULL)
		mLast->SetLink(inLinkP);
	else
		mFirst = inLinkP;
	mLast = inLinkP;

	// adjust element count
	mSize++;
}


// ---------------------------------------------------------------------------
//	¥ NextGet
// ---------------------------------------------------------------------------
//	Removes and returns the first element in the queue.

LLink*
LQueue::NextGet()
{
	LLink*	outLinkP;

	// do we have something ?
	if ((outLinkP = mFirst) != NULL)
	{
		// remove first element from queue
		mFirst	= outLinkP->GetLink();
		if (mFirst == NULL)
			mLast = NULL;
		outLinkP->SetLink(NULL);

		// adjust element count
		mSize--;
	}

	return (outLinkP);
}


// ---------------------------------------------------------------------------
//	¥ Remove
// ---------------------------------------------------------------------------
//	Remove an arbitrary element from the queue.
//
//	This function traverses the entire queue, looking for the given
//	queue element.  If the element is found, it is removed from the
//	queue.  Returns a Boolean indicating if the element was found.

Boolean
LQueue::Remove(LLink *inLinkP)
{
	LLink*	theLinkP;
	LLink*	thePrevLinkP;

	// locate requested element
	for (theLinkP = mFirst, thePrevLinkP = NULL;
		 theLinkP != NULL;
		 thePrevLinkP = theLinkP, theLinkP = theLinkP->GetLink())
	{
		// if this is the one, stop looking
		if (theLinkP == inLinkP)
			break;
	}

	// if we found the element ...
	if (theLinkP != NULL)
	{
		// fix up links
		if (theLinkP == mFirst)
		{
			mFirst = theLinkP->GetLink();
		}
		else
		{
			thePrevLinkP->SetLink(theLinkP->GetLink());
		}

		if (theLinkP == mLast)
		{
			mLast = thePrevLinkP;
		}

		theLinkP->SetLink(NULL);

		// adjust element count
		mSize--;
	}

	// return flag indicating if we found the element
	return (theLinkP != NULL);
}


// ---------------------------------------------------------------------------
//	¥ GetSize
// ---------------------------------------------------------------------------
//	Returns the number of elements in the queue.

UInt32
LQueue::GetSize() const
{
	return (mSize);
}


// ---------------------------------------------------------------------------
//	¥ IsEmpty
// ---------------------------------------------------------------------------
//	Does the queue not contain any elements?

Boolean
LQueue::IsEmpty() const
{
	return (mSize < 1);
}


// ---------------------------------------------------------------------------
//	¥ DoForEach
// ---------------------------------------------------------------------------
//	Execute a user-supplied function for each element in the queue

void
LQueue::DoForEach(LQueueIterator proc, void* arg)
{
	LLink*	theLinkP;
	LLink*	theNextP;

	for (theLinkP = mFirst; theLinkP != NULL; theLinkP = theNextP)
	{
		theNextP = theLinkP->GetLink();
		(*proc)(*theLinkP, arg);
	}
}

PP_End_Namespace_PowerPlant
