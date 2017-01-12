// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHeaderFieldList.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for handling a list of header fields. This object takes responsibility
//	for deleting the header field objects passed to it.

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LHeaderFieldList.h>
#include <LInternetMessage.h>
#include <LHeaderField.h>
#include <cstring>
#include <LArrayIterator.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		¥ LHeaderFieldList
// ---------------------------------------------------------------------------
//	Contruct an empty List of pointers to header fields

LHeaderFieldList::LHeaderFieldList()
		: LArray(sizeof(LHeaderField *), new CFieldComp(), true)
{
}


// ---------------------------------------------------------------------------
//		¥ ~LHeaderFieldList
// ---------------------------------------------------------------------------
//	Destructor

LHeaderFieldList::~LHeaderFieldList()
{
	ResetList();
}

// ---------------------------------------------------------------------------
//		¥ ResetList
// ---------------------------------------------------------------------------

void
LHeaderFieldList::ResetList()
{
	LHeaderField * currField;
	LArrayIterator iter(*this);
	while(iter.Next(&currField)) {
		delete currField;
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
LHeaderFieldList::SetItemAt(
	ArrayIndexT		inAtIndex,
	const void		*inItem)			// Pointer to the item
{
	LHeaderField * headerField;

	if (FetchItemAt(inAtIndex, &headerField))
		delete headerField;

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
LHeaderFieldList::AppendItem(
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
LHeaderFieldList::RemoveItem(
	ArrayIndexT		inAtIndex)
{
	LHeaderField * headerField;

	if (FetchItemAt(inAtIndex, &headerField))
		delete headerField;

	RemoveItemsAt(1, inAtIndex);
}

// ===========================================================================

#pragma mark -
#pragma mark === CFieldComp ===

CFieldComp::CFieldComp(){
};

CFieldComp::~CFieldComp(){
};

SInt32
CFieldComp::Compare(const void*		inItemOne,
					const void*		inItemTwo,
					UInt32			/* inSizeOne */,
					UInt32			/* inSizeTwo*/) const
{
	LHeaderField ** itemOneField = (LHeaderField **)inItemOne;
	const char * titleOne = (*itemOneField)->GetTitle();
	LHeaderField ** itemTwoField = (LHeaderField **)inItemTwo;
	const char * titleTwo = (*itemTwoField)->GetTitle();
	return PP_CSTD::strcmp(titleOne, titleTwo);
}

PP_End_Namespace_PowerPlant
