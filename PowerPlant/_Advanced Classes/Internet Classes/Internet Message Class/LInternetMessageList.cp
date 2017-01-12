// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LInternetMessageList.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for handling a list of mail messages. This object takes
//	responsibility for deleting the Mail Messages passed to it.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LInternetMessageList.h>
#include <LInternetMessage.h>
#include <LArrayIterator.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//		¥ LInternetMessageList
// ---------------------------------------------------------------------------
//	Construct an empty List of pointers to messages

LInternetMessageList::LInternetMessageList()
		: LArray(sizeof(LInternetMessage *))
{
}


// ---------------------------------------------------------------------------
//		¥ ~LInternetMessageList
// ---------------------------------------------------------------------------
//	Destructor

LInternetMessageList::~LInternetMessageList()
{
	ResetList();
}

// ---------------------------------------------------------------------------
//		¥ ResetList
// ---------------------------------------------------------------------------

void
LInternetMessageList::ResetList()
{
	LInternetMessage *	currMsg;
	LArrayIterator	iter(*this);
	while(iter.Next(&currMsg)) {
		delete currMsg;
	}

	RemoveItemsAt(GetCount(), 1);
}

// ---------------------------------------------------------------------------
//		¥ SetItemAt
// ---------------------------------------------------------------------------
//	Sets the value of the Item at the specified index
//
//	inItem is a pointer to the item data. The Array makes and stores
//	a copy of the item data.
//
//	Does nothing if inIndex is out of range

void
LInternetMessageList::SetItemAt(
	ArrayIndexT		inAtIndex,
	const void		*inItem)			// Pointer to the item
{
	LInternetMessage * message;

	if (FetchItemAt(inAtIndex, &message))
		delete message;

	AssignItemsAt(1, inAtIndex, inItem);
}

// ---------------------------------------------------------------------------
//		¥ AppendItem
// ---------------------------------------------------------------------------
//	Sets the value of the Item at the specified index
//
//	inItem is a pointer to the item data. The Array makes and stores
//	a copy of the item data.
//
//	Does nothing if inIndex is out of range

void
LInternetMessageList::AppendItem(
	const void		*inItem)			// Pointer to the item
{
	InsertItemsAt(1, LArray::index_Last, inItem);
}

// ---------------------------------------------------------------------------
//		¥ RemoveItem
// ---------------------------------------------------------------------------
//	Removes the item from list and deletes it.
//
//	inItem is a pointer to the item data. The Array makes and stores
//	a copy of the item data.
//
//	Does nothing if inIndex is out of range

void
LInternetMessageList::RemoveItem(
	ArrayIndexT		inAtIndex)
{
	LInternetMessage * message;

	if (FetchItemAt(inAtIndex, &message))
		delete message;

	RemoveItemsAt(1, inAtIndex);
}

PP_End_Namespace_PowerPlant
