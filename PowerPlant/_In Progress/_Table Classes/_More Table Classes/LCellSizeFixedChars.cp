// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellSizeFixedChars.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple sizer which truncates to a fixed width of characters

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LCellSizeFixedChars.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LCellSizeFixedChars
// ---------------------------------------------------------------------------
//	Normal Constructor

LCellSizeFixedChars::LCellSizeFixedChars(
	LTextTableView	*inView,
	Boolean inTrunc,
	UInt16	inFixedSize) :

	LCellSizer(inView, inTrunc),
	mMaxChars(inFixedSize)
{}


// ---------------------------------------------------------------------------
//	¥ LCellSizeFixedChars  - Default Constructor
// ---------------------------------------------------------------------------
// Constructor if expect LTableTextView to call SetTableView on us

LCellSizeFixedChars::LCellSizeFixedChars(
	UInt16	inFixedSize,
	Boolean inTrunc) :

	LCellSizer(0, inTrunc),
	mMaxChars(inFixedSize)
{}


// ---------------------------------------------------------------------------
//	¥ ~LCellSizeFixedChars
// ---------------------------------------------------------------------------
//	Destructor

LCellSizeFixedChars::~LCellSizeFixedChars()
{
}


// ---------------------------------------------------------------------------
//	¥ TruncatedLength
// ---------------------------------------------------------------------------
// truncates at the first character that fits in the fixed length
// optionally trimming off a partial word if we do truncation

UInt16
LCellSizeFixedChars::TruncatedLength(
								const char		*inValue,	// C string
								UInt16			inItemSize,
								TableIndexT		/*inCol*/)
{
	if (inItemSize <= mMaxChars) {
		return inItemSize;
	}

	UInt16 ret = mMaxChars;  // we truncated the item!

	if (mTruncateAtWord) {
		// start truncation from one char past fitted string, in case
		// word ended exactly at the boundary

		ret = TruncateTrailingWord(inValue, (UInt16) (mMaxChars+1));
	}

	return ret;
}


// ---------------------------------------------------------------------------
//	¥ SetSize & GetSize accessors
// ---------------------------------------------------------------------------

void
LCellSizeFixedChars::SetSize(UInt16	inSize)
{
	mMaxChars = inSize;
}


UInt16
LCellSizeFixedChars::GetSize() const
{
	return mMaxChars;
}

PP_End_Namespace_PowerPlant
