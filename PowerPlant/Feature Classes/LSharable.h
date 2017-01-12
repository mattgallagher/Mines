// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharable.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes for reference counted objects which delete themselves when
//	no longer used.

#ifndef _H_LSharable
#define _H_LSharable
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ===========================================================================
//	Mixin class for reference counted objects

class	LSharable {
public:
						LSharable();

						LSharable( const LSharable& inSharable );

	LSharable&			operator = ( const LSharable& inSharable );

	virtual void		AddUser( void* inUser );

	virtual void		RemoveUser( void* inUser );

	virtual SInt32		GetUseCount() const;

protected:
	SInt32			mUseCount;

	virtual				~LSharable();		// protected destructor prevents
											//   stack-based objects
	virtual void		NoMoreUsers();
};


// ===========================================================================
//	Stack-based class for automatically adding and removing a user
//	of a Sharable object

class	StSharer {
public:
						StSharer( LSharable* inSharable );
						
						~StSharer();

	void				SetSharable( LSharable*	inSharable );

protected:
	LSharable*		mSharable;
};


// ===========================================================================
//	Template class for automatically adding and removing a user of
//	a Sharable object
//
//	Typically, you will use a TSharablePtr as a member variable of
//	a class. The coercion operator T* and the overloaded operator->
//	let you use a TSharablePtr as if it were a T* variable.

template<class T>		// T should be subclass of LSharable
class TSharablePtr {

public:
						TSharablePtr( T* inSharablePtr = nil )
							{
								mSharablePtr = nil;
								SetReference(inSharablePtr);
							}

						TSharablePtr( const TSharablePtr<T>& inSharableRef )
							{
								mSharablePtr = nil;
								SetReference(inSharableRef.mSharablePtr);
							}

						~TSharablePtr()			{ SetReference(nil); }

			operator 	T*() const				{ return mSharablePtr; }

	TSharablePtr<T>&	operator = ( T* inSharablePtr )
							{
								SetReference(inSharablePtr);
								return *this;
							}

	TSharablePtr<T>&	operator = ( const TSharablePtr<T>& inSharableRef )
							{
								SetReference(inSharableRef.mSharablePtr);
								return *this;
							}

	T*					operator -> () const	{ return mSharablePtr; }

	T*					GetObject() const		{ return mSharablePtr; }

protected:
	T*				mSharablePtr;

	void				SetReference( T* inSharablePtr);


};

// ---------------------------------------------------------------------------

template<class T>
void
TSharablePtr<T>::SetReference(T* inSharablePtr)
{
	T*	oldSharablePtr = mSharablePtr;

	if (inSharablePtr != nil) {			// Add user to new Sharable
		inSharablePtr->AddUser(this);	// Do add before remove in case
	}									//   inSharable == mSharable
	mSharablePtr = inSharablePtr;

	if (oldSharablePtr != nil) {		// Remove user from old Sharable
		oldSharablePtr->RemoveUser(this);
	}
}

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
