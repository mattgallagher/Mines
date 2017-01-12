// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInterruptSafeList.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LInterruptSafeList
#define _H_LInterruptSafeList
#pragma once

#include <PP_Prefix.h>
#include <OSUtils.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LInterruptSafeListMember;
class LInterruptSafeListIterator;


// ===========================================================================
//		¥ SMemberQueueElem
// ===========================================================================
//	Implementation detail only. Do not use this structure.

struct SMemberQueueElem {
	SMemberQueueElem*			mNextElem;
	LInterruptSafeListMember*	mListMember;
};


// ===========================================================================
//		¥ SIteratorQueueElem
// ===========================================================================
//	Implementation detail only. Do not use this structure.

struct SIteratorQueueElem {
	SIteratorQueueElem*			mNextElem;
	LInterruptSafeListIterator*	mIterator;
};


// ===========================================================================
//	¥ LInterruptSafeList
// ===========================================================================
//	Implements a container class which can add and remove members at
//	interrupt time. The current implementation uses the Toolbox calls
//	Enqueue and Dequeue, but this may change at a later time.

class LInterruptSafeList {

public:
						LInterruptSafeList();

	virtual				~LInterruptSafeList();

	virtual	void		Append( LInterruptSafeListMember* inItem );

	virtual	bool		Remove( LInterruptSafeListMember* inItem );

	virtual bool		IsEmpty() const;

protected:
	SMemberQueueElem*	GetFirstMember() const;

	SIteratorQueueElem*	GetFirstIterator() const;

private:
											// Disallow copy and assignement
						LInterruptSafeList(const LInterruptSafeList&);
	LInterruptSafeList&	operator = (const LInterruptSafeList&);

	friend class LInterruptSafeListIterator;

private:
	QHdr				mQueue;				// List members
	mutable QHdr		mIteratorQueue;		// List of iterators on this list
};

// ---------------------------------------------------------------------------
//	Inline implementations

inline
SMemberQueueElem*
LInterruptSafeList::GetFirstMember() const
{
	return (SMemberQueueElem*) mQueue.qHead;
}


inline
SIteratorQueueElem*
LInterruptSafeList::GetFirstIterator() const
{
	return (SIteratorQueueElem*) mIteratorQueue.qHead;
}


// ===========================================================================
//	¥ LInterruptSafeListIterator
// ===========================================================================
//	An iterator which traverses the list defined by LInterruptSafeList.
//	Unlike the general-purpose LListIterator, this iterator can only
//	travel forward.

class LInterruptSafeListIterator {

public:
						LInterruptSafeListIterator(
								const LInterruptSafeList& inList);

						~LInterruptSafeListIterator();

	void				Reset();

	bool				Current( LInterruptSafeListMember* &outItem) ;

	bool				Next( LInterruptSafeListMember* &outItem );

private:
	void				ListDied();

	void				ItemAppended( LInterruptSafeListMember* inItem );

	void				ItemRemoved( LInterruptSafeListMember* inItem );

private:									// Disallow default constructor,
											//   copy, and assignment
						LInterruptSafeListIterator();
						LInterruptSafeListIterator(const LInterruptSafeListIterator&);
	LInterruptSafeListIterator&	operator = (const LInterruptSafeListIterator&);

private:
	SIteratorQueueElem			mQueueElem;
	const LInterruptSafeList&	mList;
	bool						mListDied;

	LInterruptSafeListMember*	mCurrentEntry;
	LInterruptSafeListMember*	mNextEntry;

	friend class LInterruptSafeList;
};


// ===========================================================================
//	¥ LInterruptSafeListMember
// ===========================================================================
//	Mix-in class for items which can be added to LInterruptSafeLists.

class LInterruptSafeListMember {

public:
						LInterruptSafeListMember();

						LInterruptSafeListMember(const LInterruptSafeListMember&);

	virtual				~LInterruptSafeListMember();

private:							// Disallow assignment
	LInterruptSafeListMember&	operator = (const LInterruptSafeListMember&);

private:
	SMemberQueueElem		mQueueElem;
	LInterruptSafeList*		mParentList;

	friend class LInterruptSafeList;
	friend class LInterruptSafeListIterator;
};


// ===========================================================================
//	¥ TInterruptSafeList
// ===========================================================================

template <class T> class TInterruptSafeList : public LInterruptSafeList {
public:
						TInterruptSafeList()	{ }

	virtual				~TInterruptSafeList()	{ }

	virtual void		Append( T inItem )
							{
								LInterruptSafeList::Append(inItem);
							}

	virtual bool		Remove( T inItem )
							{
								return LInterruptSafeList::Remove(inItem);
							}

private:
	using LInterruptSafeList::Append;
	using LInterruptSafeList::Remove;
};


// ===========================================================================
//	¥ TInterruptSafeListIterator
// ===========================================================================

template <class T> class TInterruptSafeListIterator : public LInterruptSafeListIterator {
public:
					TInterruptSafeListIterator( const TInterruptSafeList<T> &inList )

						: LInterruptSafeListIterator(inList)	{ }

					~TInterruptSafeListIterator()	{ }

	bool			Current( T& outItem )
						{
							LInterruptSafeListMember*	item;
							bool	exists =  LInterruptSafeListIterator::Current(item);
							outItem = (T) item;
							return exists;
						}

	bool			Next( T& outItem )
						{
							LInterruptSafeListMember*	item;
							bool	exists = LInterruptSafeListIterator::Next(item);
							outItem = (T) item;
							return exists;
						}
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
