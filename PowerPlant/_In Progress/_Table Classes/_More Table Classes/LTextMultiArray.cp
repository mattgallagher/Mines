// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextMultiArray.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	An ordered collection of variable-size text strings. Positions in the list
//	are one-based--the first item is at index 1.
//
//	Class maintains two Handles, one stores the data for each item and the
//	other stores the offsets within the data Handle to the start of the
//	data for each item.
//
//	The offsets Handle contains one more entry than the number of items.
//	The last entry is the size of the data Handle, which is the offset
//	of the "next" item (if there was one). This simplifies operations
//	on the offsets Handle dealing with the last item.

#include <LTextMultiArray.h>
#include <LTextMultiArray.h>
#include <cstring>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTextMultiArray
// ---------------------------------------------------------------------------
//	Constructor for an empty Array

LTextMultiArray::LTextMultiArray(
	LComparator	*inComparator,
	Boolean		inKeepSorted)
		: LVariableArray(inComparator, inKeepSorted)
{
}


// ---------------------------------------------------------------------------
//	¥ LTextMultiArray
// ---------------------------------------------------------------------------
//	Constructor from existing offsets and data Handles

LTextMultiArray::LTextMultiArray(
	Handle			inItemsHandle,
	ArrayOffsetH	inOffsetsHandle,
	LComparator		*inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)
		: LVariableArray(inItemsHandle, inOffsetsHandle, inComparator, inIsSorted, inKeepSorted)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTextMultiArray
// ---------------------------------------------------------------------------
//	Destructor

LTextMultiArray::~LTextMultiArray()
{
}


// ---------------------------------------------------------------------------
//	¥ AppendItem and operator+=
// ---------------------------------------------------------------------------
//	Appends a string after the last item currently stored.
//
//	inValue is a pointer to the item data. The Array makes and stores
//	a copy of the item data.

void
LTextMultiArray::AppendItem(
	const char		*inValue,
	UInt32			inItemSize)
{
	InsertItemsAt(1, (ArrayIndexT) mItemCount +1, inValue, inItemSize);
}


void
LTextMultiArray::operator+=(
	const char		*inString)
{
	AppendItem(inString, PP_CSTD::strlen(inString));
}


// ---------------------------------------------------------------------------
//	¥ AppendArray
// ---------------------------------------------------------------------------
//	Appends a number of strings, terminated by an empty string.
//
// Does nothing if passed blank array

void
LTextMultiArray::AppendArray(
	const char		**inValues)
{
	if (!inValues)
		return;

	UInt32 i = 0;
	while (inValues[i] && (inValues[i][0]!='\0') ){
		UInt32 itemLen = PP_CSTD::strlen(inValues[i]);
		InsertItemsAt(1, (ArrayIndexT) mItemCount +1, inValues[i], itemLen);
		i++;
	}
}

PP_End_Namespace_PowerPlant
