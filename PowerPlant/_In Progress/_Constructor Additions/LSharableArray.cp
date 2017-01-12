// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharableArray.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSharableArray.h>

#include <LArrayIterator.h>
#include <LSharable.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSharableArray						Constructor				  [public]
// ---------------------------------------------------------------------------

LSharableArray::LSharableArray(
	LComparator*	inComparator,
	Boolean			inKeepSorted)

	: LArray(sizeof(LSharable*), inComparator, inKeepSorted)
{
}


// ---------------------------------------------------------------------------
//	¥ LSharableArray						Constructor				  [public]
// ---------------------------------------------------------------------------

LSharableArray::LSharableArray(
	Handle			inItemsHandle,
	LComparator*	inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)

	: LArray(sizeof(LSharable*), inItemsHandle, inComparator,
				inIsSorted, inKeepSorted)
{
	InitSharableArray();
}


// ---------------------------------------------------------------------------
//	¥ LSharableArray						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LSharableArray::LSharableArray(
	const LSharableArray&	inOriginal)

	: LArray(inOriginal)
{
	InitSharableArray();
}


// ---------------------------------------------------------------------------
//	¥ operator =							Assignment Operator		  [public]
// ---------------------------------------------------------------------------

LSharableArray&
LSharableArray::operator = (
	const LSharableArray&	inArray)
{
	if (this != &inArray) {
		LArray::operator=(inArray);

		InitSharableArray();
	}

	return *this;
}


// ---------------------------------------------------------------------------
//	¥ InitSharableArray						Initializer				 [private]
// ---------------------------------------------------------------------------

void
LSharableArray::InitSharableArray()
{
	LArrayIterator iter(*this);		// Add this Array as a user of each item
	LSharable* obj;
	while (iter.Next(&obj)) {
		obj->AddUser(this);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LSharableArray						Destructor				  [public]
// ---------------------------------------------------------------------------

LSharableArray::~LSharableArray()
{
	RemoveItemsAt(GetCount(), 1);
}


// ---------------------------------------------------------------------------
//	¥ InsertItemAt
// ---------------------------------------------------------------------------
//	Add a sharable object to the array at the specified position.

void
LSharableArray::InsertItemAt(
	ArrayIndexT		inAtIndex,
	LSharable*		inItem)
{
	inItem->AddUser(this);
	LArray::InsertItemsAt(1, inAtIndex, &inItem);
}


// ---------------------------------------------------------------------------
//	¥ RemoveItem
// ---------------------------------------------------------------------------
//	Remove a sharable object from the array.

void
LSharableArray::RemoveItem(
	LSharable*	inItem)
{
	LArray::Remove(&inItem);
}


// ---------------------------------------------------------------------------
//	¥ RemoveItemsAt
// ---------------------------------------------------------------------------
//	Remove multiple sharable objects from the array. The use counts
//	are decremented as the items are removed.

void
LSharableArray::RemoveItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex)
{
	if ((inCount >= 1) && ValidIndex(inAtIndex)) {

		UInt32 count = inCount;
		LArrayIterator iter(*this, inAtIndex - 1);
		LSharable* obj;

		while (iter.Next(&obj) && count--) {
			obj->RemoveUser(this);
		}

		LArray::RemoveItemsAt(inCount, inAtIndex);
	}
}


// ---------------------------------------------------------------------------
//	¥ InsertItemsAt											[private]
// ---------------------------------------------------------------------------
//	InsertItemsAt is disallowed because it is not type-safe.
//	Use InsertItemAt instead.

ArrayIndexT
LSharableArray::InsertItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inItem,
	UInt32			inItemSize)
{
	SignalStringLiteral_("Do not call InsertItemsAt() for LSharableArray");
	return LArray::InsertItemsAt(inCount, inAtIndex, inItem, inItemSize);
}


// ---------------------------------------------------------------------------
//	¥ Remove												[private]
// ---------------------------------------------------------------------------
//	Remove(void*) is disallowed because it is not type-safe.
//	Use RemoveItem instead.

void
LSharableArray::Remove(
	const void*		inItem,
	UInt32			inItemSize)
{
	SignalStringLiteral_("Do not call Remove() for LSharableArray");
	LArray::Remove(inItem, inItemSize);
}


// ---------------------------------------------------------------------------
//	¥ AssignItemsAt											[private]
// ---------------------------------------------------------------------------
//	AssignItemsAt is disallowed because it is not type-safe and
//	doesn't provide a safe mechanism for adjusting use counts.
//	Use RemoveItem and InsertItemsAt instead.

ArrayIndexT
LSharableArray::AssignItemsAt(
	UInt32			inCount,
	ArrayIndexT		inAtIndex,
	const void*		inValue,
	UInt32			inItemSize)
{
	SignalStringLiteral_("Do not call AssignItemsAt() for LSharableArray");
	return LArray::AssignItemsAt(inCount, inAtIndex, inValue, inItemSize);
}


// ---------------------------------------------------------------------------
//	¥ MoveItem												[private]
// ---------------------------------------------------------------------------
//	MoveItem is disallowed because it doesn't provide a convenient
//	mechanism for adjusting use counts. Use RemoveItem and
//	InsertItemsAt instead.

void
LSharableArray::MoveItem(
	ArrayIndexT		inFromIndex,
	ArrayIndexT		inToIndex)
{
	SignalStringLiteral_("Do not call MoveItem() for LSharableArray");
	LArray::MoveItem(inFromIndex, inToIndex);
}


PP_End_Namespace_PowerPlant
