// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBroadcaster.cp				PowerPlant 2.2.2	й1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A mix-in class that works with LListener class to implement
//	dependencies. A Broadcaster sends messages to its Listeners.
//
//	ее How To Use
//	LBroadcaster is an abstract, mix-in class. Broadcasters have a list
//	of Listeners to which they send messages via the BroadcastMessage()
//	function. You attach a Listener to a Broadcaster using the
//	AddListener() function.
//
//	Classes derived from LBroadcaster should call the BroadcastMessage()
//	function whenever they want to announce something--usually a change
//	in state. For example, the destructor for LBroadcaster sends a
//	msg_BroadcasterDied message to its Listeners.
//
//	ее Public Interface
//	е Construction:
//		LBroadcaster();
//			Default constructor. Broadcaster has no Listeners
//
//	е Linking to Listeners:
//		void	AddListener(LListener *inListener);
//		void	RemoveListener(LListener *inListener);
//			Attaches/Detaches a Listener to/from a Broadcaster. These
//			functions call the appropriate Listener functions to maintain
//			the crosslinks between the Broadcaster and Listener.
//
//	е Sending Messages:
//		void	BroadcastMessage(MessageT inMessage,
//								 void *ioParam = nil) const;
//			Sends the specified message and parameter to all Listeners.
//			The meaning of the parameter depends on the message.
//
//	е Broadcasting State:
//		void	StartBroadcasting();
//		void	StopBroadcasting();
//		Boolean	IsBroadcasting() const;
//			Turn off/on, Inspect broadcasting state. A Broadcaster that is
//			not broadcasting does not send messages to its Listeners (i.e.,
//			BroadcastMessage does nothing). Use Stop/Start Broadcasting
//			to temporarily alter dependencies.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LBroadcaster.h>
#include <LListener.h>
#include <TArrayIterator.h>

#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	е LBroadcaster							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LBroadcaster::LBroadcaster()
{
	mIsBroadcasting = true;
}


// ---------------------------------------------------------------------------
//	е LBroadcaster							Copy Constructor		  [public]
// ---------------------------------------------------------------------------
//	Makes a shallow copy; Listener links are not copied.

LBroadcaster::LBroadcaster(
	const LBroadcaster&		inOriginal)
{
	mIsBroadcasting = inOriginal.mIsBroadcasting;
}


// ---------------------------------------------------------------------------
//	е ~LBroadcaster							Destructor				  [public]
// ---------------------------------------------------------------------------

LBroadcaster::~LBroadcaster()
{
						// Notify Listeners that Broadcaster is going away
	BroadcastMessage(msg_BroadcasterDied, this);

						// Tell all Listeners to remove this Broadcaster
	TArrayIterator<LListener*> iterator(mListeners);
	LListener*	theListener;
	while (iterator.Next(theListener)) {
		theListener->RemoveBroadcaster(this);
	}
}


// ---------------------------------------------------------------------------
//	е AddListener													  [public]
// ---------------------------------------------------------------------------
//	Add a Listener to a Broadcaster
//
//	Usage note: This is the public interface for associating Broadcasters
//	and Listeners.
//		Right:	theBroadcaster->AddListener(theListener);
//		Wrong:	theListener->AddBroadcaster(theBroadcaster);
//
//	This function takes care of notifying the Listener to update its
//	list of Broadcasters.

void
LBroadcaster::AddListener(
	LListener*	inListener)
{
	if (not HasListener(inListener)) {		// Add if not already a Listener
		mListeners.AddItem(inListener);
		inListener->AddBroadcaster(this);
	}
}


// ---------------------------------------------------------------------------
//	е RemoveListener												  [public]
// ---------------------------------------------------------------------------
//	Remove a Listener from a Broadcaster
//
//	Usage note: This is the public interface for dissociating Broadcasters
//	and Listeners.
//		Right:	theBroadcaster->RemoveListener(theListener);
//		Wrong:	theListener->RemoveBroadcaster(theBroadcaster);
//
//	This function takes care of notifying the Listener to update its
//	list of Broadcasters.

void
LBroadcaster::RemoveListener(
	LListener*	inListener)
{
	mListeners.Remove(inListener);
	inListener->RemoveBroadcaster(this);
}


// ---------------------------------------------------------------------------
//	е HasListener													  [public]
// ---------------------------------------------------------------------------
//	Return whether the Broadcaster has the specified Listener

bool
LBroadcaster::HasListener(
	LListener*	inListener)
{
	return (mListeners.FetchIndexOf(inListener) != LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//	е BroadcastMessage												  [public]
// ---------------------------------------------------------------------------
//	Send message to all associated Listeners
//
//	Does not send message if broadcasting is turned off

void
LBroadcaster::BroadcastMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	if (mIsBroadcasting) {
		TArrayIterator<LListener*>	iterator(mListeners);
		LListener*	theListener;
		while (iterator.Next(theListener)) {
			if (theListener->IsListening()) {
				theListener->ListenToMessage(inMessage, ioParam);
			}
		}
	}
}


PP_End_Namespace_PowerPlant
