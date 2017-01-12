// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextArray.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
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

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTextArray.h>
#include <cstring>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LTextArray
// ---------------------------------------------------------------------------
//	Constructor for an empty Array

LTextArray::LTextArray(
	LComparator	*inComparator,
	Boolean		inKeepSorted)
		: LVariableArray(inComparator, inKeepSorted),
			mOwnsContents(false)
{
}


// ---------------------------------------------------------------------------
//	¥ LTextArray
// ---------------------------------------------------------------------------
//	Constructor from existing offsets and data Handles

LTextArray::LTextArray(
	Handle			inItemsHandle,
	ArrayOffsetH	inOffsetsHandle,
	LComparator		*inComparator,
	Boolean			inIsSorted,
	Boolean			inKeepSorted)
		: LVariableArray(inItemsHandle, inOffsetsHandle, inComparator, inIsSorted, inKeepSorted),
			mOwnsContents(false)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LTextArray
// ---------------------------------------------------------------------------
//	Destructor

LTextArray::~LTextArray()
{
	if (mOwnsContents) {
		DeleteAllContents();
	}
}


// ---------------------------------------------------------------------------
//	¥ AppendItem and operator+=
// ---------------------------------------------------------------------------
//	Appends a string after the last item currently stored.
//
//	inValue is a pointer to the item data. The Array makes and stores
//	a copy of the item data.

void
LTextArray::AppendItem(
	const char		*inValue,
	UInt32			inItemSize)
{
	InsertItemsAt(1, (ArrayIndexT) mItemCount +1, inValue, inItemSize);
}


void
LTextArray::operator+=(
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
LTextArray::AppendArray(
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


// ---------------------------------------------------------------------------
//	¥ SetItemsDelimited
// ---------------------------------------------------------------------------
//	Appends a number of strings in a C string. Useful for tab-delimited data
//
// Does nothing if passed blank array
//
// WARNING sets mOwnsContents flag - do NOT mix with using operator+=


void
LTextArray::SetItemsDelimited(
								const char	*inStr,		// string of delimited strings, null terminated
								char		inDelimiter,
								UInt32		/* inAppendFrom */)	// can replace strings from certain point
{
	if (!inStr)
		return;

	DeleteAllContents();
	mOwnsContents = true;
	if (mItemsH != nil) {			// Dispose Item storage space
		::DisposeHandle(mItemsH);
		mItemsH = nil;
	}
	mItemCount = 0;

	UInt32 i = 0;
	while (inStr[i]!='\0') {
// at start of a string
		UInt32 startStr = i;
		while (inStr[i]!=inDelimiter && inStr[i]!='\0')
			++i;
		UInt32 itemLen = i - startStr;
		char* theItem = new char[itemLen+1]; // may insert null strings - allow for '\0'
		PP_CSTD::memcpy(theItem, &inStr[startStr], itemLen);
		theItem[itemLen] = '\0';
		InsertItemsAt(1, (ArrayIndexT) mItemCount +1, theItem, itemLen);
		i++;
	}
}


// ---------------------------------------------------------------------------
//	¥ DeleteAllContents
// ---------------------------------------------------------------------------
//	Deletes all the strings pointed to by the array.
//

void
LTextArray::DeleteAllContents()
{
	if (!mOwnsContents)
		return;

	const UInt32 numItems = mItemCount;
	for (ArrayIndexT i=0; i<numItems; ++i) {
			char** itemStr = (char**)GetItemPtr(i);
			delete[] *itemStr;
	}
}

PP_End_Namespace_PowerPlant
