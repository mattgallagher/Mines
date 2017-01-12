// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LListener.cp				PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	An abstract mix-in class that works with LBroadcaster class to implement
//	dependencies. A Listener receives messages from its Broadcasters.
//
//	ее Public Interface
//	е Construction:
//		LListener();
//			Default constructor. Listener has no Broadcasters
//
//	е Linking to Broadcasters:
//		Link a Listener to a Broadcaster by sending an AddListener message
//		to a Broadcaster. [see LBroadcaster]
//
//	е Responding to Messages:
//		void	ListenToMessage(MessageT inMessage, void *ioParam);
//			Derived classes *must* override this function (it is a
//			pure virtual function in LListener).
//			LLBroadcaster::BroadcastMessage calls this function.
//
//		Example Implementation:
//
//		{
//			switch (inMessage) {
//
//				case msg_NameChanged:	// ioParam is a StringPtr
//					DoNameChanged((StringPtr) ioParam);
//					break;
//
//				case msg_NewValue:		// ioParam is a long*
//					DoNewValue(*(long *) ioParam);
//					break;
//			}
//		}
//
//		The programmer can define message constants and associated
//		meanings for the ioParam parameter.
//
//		A Broadcaster always sends an msg_BroadcasterDied message, with
//		a pointer to itself as the parameter, before it is deleted.
//
//	е Listening State:
//		void	StopListening();
//		void	StartListening();
//		Boolean	IsListening();
//			Turn off/on, Inspect listening state. A Listener that is
//			not listening does not receive messages from its Broadcasters.
//			Use Stop/Start Listening to temporarily alter dependencies.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LListener.h>
#include <LBroadcaster.h>
#include <TArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	е LListener								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LListener::LListener()
{
	mIsListening = true;
}


// ---------------------------------------------------------------------------
//	е LListener								Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Makes a shallow copy; Broadcaster links are not copied.

LListener::LListener(
	const LListener&	inOriginal)
{
	mIsListening = inOriginal.mIsListening;
}


// ---------------------------------------------------------------------------
//	е ~LListener							Destructor				  [public]
// ---------------------------------------------------------------------------

LListener::~LListener()
{
	TArrayIterator<LBroadcaster*> iterator(mBroadcasters);
	LBroadcaster*	theBroadcaster;
	while (iterator.Next(theBroadcaster)) {
		theBroadcaster->RemoveListener(this);
	}
}


// ---------------------------------------------------------------------------
//	е HasBroadcaster												  [public]
// ---------------------------------------------------------------------------
//	Return whether a Listener has the specified Broadcaster

bool
LListener::HasBroadcaster(
	LBroadcaster*	inBroadcaster)
{
	return (mBroadcasters.FetchIndexOf(inBroadcaster) != LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//	е AddBroadcaster											   [protected]
// ---------------------------------------------------------------------------
//	Add a Broadcaster to a Listener
//
//	You should not call this function directly. LBroadcaster::AddListener
//	will call this function
//		Right:	theBroadcaster->AddListener(theListener);
//		Wrong:	theListener->AddBroadcaster(theBroadcaster);

void
LListener::AddBroadcaster(
	LBroadcaster*	inBroadcaster)
{
	mBroadcasters.AddItem(inBroadcaster);
}


// ---------------------------------------------------------------------------
//	е RemoveBroadcaster											   [protected]
// ---------------------------------------------------------------------------
//	Remove a Broadcaster from a Listener
//
//	You should not call this function directly. LBroadcaster::RemoveListener
//	will call this function
//		Right:	theBroadcaster->RemoveListener(theListener);
//		Wrong:	theListener->RemoveBroadcaster(theBroadcaster);

void
LListener::RemoveBroadcaster(
	LBroadcaster*	inBroadcaster)
{
	mBroadcasters.Remove(inBroadcaster);
}


PP_End_Namespace_PowerPlant
