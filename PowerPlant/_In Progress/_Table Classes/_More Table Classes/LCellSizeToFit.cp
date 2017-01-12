// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCellSizeToFit.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple sizer which truncates to a fixed width of characters

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LCellSizeToFit.h>
#include <LTextTableView.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LCellSizeToFit
// ---------------------------------------------------------------------------
//	Normal Constructor

LCellSizeToFit::LCellSizeToFit(
	LTextTableView	*inView,
	Boolean inTrunc) :

	LCellSizer(inView, inTrunc),
	mMeasureArray(0),
	mMeasureLen(0)
{}


// ---------------------------------------------------------------------------
//	¥ LCellSizeToFit  - Default Constructor
// ---------------------------------------------------------------------------
// Constructor if expect LTableTextView to call SetTableView on us

LCellSizeToFit::LCellSizeToFit(Boolean inTrunc) :
	LCellSizer(0, inTrunc),
	mMeasureArray(0),
	mMeasureLen(0)
{}


// ---------------------------------------------------------------------------
//	¥ ~LCellSizeToFit
// ---------------------------------------------------------------------------
//	Destructor

LCellSizeToFit::~LCellSizeToFit()
{
	delete[] mMeasureArray;
}


// ---------------------------------------------------------------------------
//	¥ TruncatedLength
// ---------------------------------------------------------------------------
// truncates at the first character that fits in the current col width
// optionally trimming off a partial word if we do truncation
//
// WARNING: must have a port and the correct text traits current to work!
//
// FUTURE IDEAS: maybe cache typical string lengths for the current col



UInt16
LCellSizeToFit::TruncatedLength(
								const char		*inValue,	// C string
								UInt16			inItemSize,
								TableIndexT	inCol)
{
	if (inItemSize==0)
		return 0;  //trivial exit

	if (mMeasureLen <= inItemSize) {
		delete[] mMeasureArray;
		mMeasureLen = (UInt16) (inItemSize+1);
		mMeasureArray = new short[mMeasureLen];
	}

	MeasureText((SInt16) inItemSize, inValue, mMeasureArray);

	UInt16 colWidth = (UInt16) (mTableView->GetColWidth(inCol) - 2);  // 2 pixel gap at end of col
	UInt16 ret = inItemSize; // as 1-based array indexed 1..inItemSize
	while (ret > 1) {  // 2nd cell is width of 1st char, effectively 1-based array
		if (mMeasureArray[ret] > colWidth)
			--ret;
		else {  // it fits!
			if (ret < inItemSize)  // we truncated it
				if (mTruncateAtWord) {
				// start truncation from one char past fitted string, in case
				// word ended exactly at the boundary
					ret = TruncateTrailingWord(inValue, ret);  // ret is currently 1-based
				}
			break;
		}
	}
	return ret;
}

PP_End_Namespace_PowerPlant
