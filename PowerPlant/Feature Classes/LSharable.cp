// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharable.cp				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Classes for reference counted objects which delete themselves when
//	no longer used.
//
//	Sharable objects must be allocated via "new".
//
//	Use LSharable as a mixin class for objects that you wish to share
//	and have deleted when they are no longer referenced.
//
//	Use StSharable for a stack-based object which maintains a reference to
//	a shared object for the duration of single function or statment block.
//
//	Use TSharablePtr template class as a member variable in a class where
//	you want to retain a reference to a shared object for the lifetime
//	of the user object.


#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LSharable.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSharable								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LSharable::LSharable()
{
	mUseCount = -1;					// Special flag for a new Sharable that
									//   doesn't have any users yet
}


// ---------------------------------------------------------------------------
//	¥ LSharable								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LSharable::LSharable(
	const LSharable&	/* inSharable */)
{
	mUseCount = -1;					// Copy has no users
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LSharable&
LSharable::operator = (
	const LSharable&	inSharable)
{
	if (this != &inSharable) {
		mUseCount = -1;				// Copy has no users
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ ~LSharable							Destructor				  [public]
// ---------------------------------------------------------------------------

LSharable::~LSharable()
{
	SignalIf_(mUseCount > 0);
}


// ---------------------------------------------------------------------------
//	¥ AddUser														  [public]
// ---------------------------------------------------------------------------
//	Specify a user for a Sharable object
//
//	This function ignores the user and just increments the use count.
//	Override if you wish to keep track of users.
//
//	NOTE: Do not throw an exception from this function

void
LSharable::AddUser(
	void*	/* inUser */)
{
	if (mUseCount < 0) {			// First user
		mUseCount = 1;

	} else {
		mUseCount++;
		SignalIf_(mUseCount <= 0);	// Counter flipped over
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveUser													  [public]
// ---------------------------------------------------------------------------
//	Specify that a user is no longer sharing a Sharable object
//
//	This function ignores the user and just decrements the use count.
//	Override if you wish to keep track users.
//
//	NOTE: Do not throw an exception from this function

void
LSharable::RemoveUser(
	void*	/* inUser */)
{
	mUseCount--;

	SignalIf_(mUseCount < 0);		// Too many releases

	if (mUseCount <= 0) {
		NoMoreUsers();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetUseCount													  [public]
// ---------------------------------------------------------------------------
//	Return the number of users of a Sharable object

SInt32
LSharable::GetUseCount() const
{
	return mUseCount;
}


// ---------------------------------------------------------------------------
//	¥ NoMoreUsers												   [protected]
// ---------------------------------------------------------------------------
//	Internal function called when use count drops to zero.

void
LSharable::NoMoreUsers()
{
	delete this;
}

#pragma mark -

// ===========================================================================
//	¥ StSharer
// ===========================================================================
//	Constructor adds user to a Sharable object
//	Destructor removes user

StSharer::StSharer(
	LSharable*	inSharable)
{
	mSharable = inSharable;

	if (inSharable != nil) {
		inSharable->AddUser(this);
	}
}


StSharer::~StSharer()
{
	if (mSharable != nil) {
		mSharable->RemoveUser(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetSharable													  [public]
// ---------------------------------------------------------------------------
//	Change the associated Sharable object

void
StSharer::SetSharable(
	LSharable*	inSharable)
{
	LSharable*	oldSharable = mSharable;

	if (inSharable != nil) {			// Add user to new Sharable
		inSharable->AddUser(this);		// Do add before remove in case
	}									//   inSharable == mSharable
	mSharable = inSharable;

	if (oldSharable != nil) {			// Remove user from old Sharable
		oldSharable->RemoveUser(this);
	}
}


PP_End_Namespace_PowerPlant

