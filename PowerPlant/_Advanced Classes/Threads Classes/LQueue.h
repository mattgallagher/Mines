// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LQueue.h					PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LQueue
#define _H_LQueue
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ Forward declarations
// ===========================================================================


// forward class declarations
class	LLink;


// ===========================================================================
// ¥ Data structures
// ===========================================================================


// iterator function
typedef void	(*LQueueIterator)(LLink& aLink, void* arg);


// ===========================================================================
// ¥ class LQueue
// ===========================================================================


class LQueue
{
public:

	// constructors / destructors
					LQueue();
	virtual			~LQueue();

	// inquiries
	virtual UInt32	GetSize() const;
	virtual Boolean	IsEmpty() const;

	// queue manipulation
	virtual void	NextPut(LLink* aLink);
	virtual LLink*	NextGet();
	virtual Boolean	Remove(LLink* aLink);

	// iterators
	virtual void	DoForEach(LQueueIterator proc, void* arg);


protected:

	LLink*			mFirst;		// first element in queue
	LLink*			mLast;		// last element in queue
	UInt32			mSize;		// number of elements in queue


private:

	// constructors & destructors
					LQueue(const LQueue&);

	// assignment
	LQueue&			operator = (const LQueue&);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif // _H_LQueue
