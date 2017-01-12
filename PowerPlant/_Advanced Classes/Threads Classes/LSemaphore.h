// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSemaphore.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Paul Lalonde

#ifndef _H_LSemaphore
#define _H_LSemaphore
#pragma once

#include <PP_Prefix.h>
#include <OSUtils.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	¥ Constants
// ===========================================================================

// set non-zero to get unique semaphore IDs
#ifndef SEMAPHORE_IDS
#	define SEMAPHORE_IDS	0
#endif

const SInt32	semaphore_WaitForever	= -1;
const SInt32	semaphore_NoWait		= 0;


// error codes
enum {
	errSemaphoreDestroyed = 28020,
	errSemaphoreTimedOut,
	errSemaphoreNotOwner,
	errSemaphoreAlreadyReset
};


// ===========================================================================
//	¥ Data structures
// ===========================================================================


// forward class & structure declarations
class	LThread;


// ===========================================================================
//	¥ class LSemaphore
// ===========================================================================


class LSemaphore
{
public:

	// constructors / destructors
					LSemaphore();
					LSemaphore(SInt32 initialCount);
	virtual			~LSemaphore();

	// blocking
	virtual void			Signal();
	virtual ExceptionCode	Wait(SInt32 milliSeconds = semaphore_WaitForever);

	// inquiries
#if SEMAPHORE_IDS
	UInt32			GetID() const;
#endif

	// searching
#if SEMAPHORE_IDS
	static LSemaphore*	FindSemaphore(UInt32 itsID);
#endif


protected:

	// thread control
	ExceptionCode	BlockThread(SInt32 milliSeconds);
	LThread*		UnblockThread(QElemPtr qEl, ExceptionCode error);
	void			UnblockAll(ExceptionCode error);

	// member variables
#if SEMAPHORE_IDS
	QElemPtr		mQLink;				// link in chain of semaphores
	UInt32			mID;				// unique ID for this semaphore
#endif
	SInt32			mExcessSignals;		// excess signal / thread count
	QHdr			mThreads;			// list of waiting threads

	// static member variables
#if SEMAPHORE_IDS
	static QHdr		sSemaphores;		// list of allocated semaphores
	static UInt32	sNextSemaphoreID;	// next unique ID
#endif


private:

	// constructors & destructors
					LSemaphore(const LSemaphore&);

	// assignment
	LSemaphore&		operator = (const LSemaphore&);

	// initialisation
	void			InitSemaphore(SInt32 initialCount);

	// friends
	friend class	LThread;
};


//	Inline member functions

#if SEMAPHORE_IDS

inline UInt32
LSemaphore::GetID() const
{
	return (mID);
}

#endif

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif // _H_LSemaphore
