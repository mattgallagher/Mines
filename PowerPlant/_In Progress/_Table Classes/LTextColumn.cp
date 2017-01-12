// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextColumn.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//
//	View for a single column of text strings
//
//	TextColumn has a single TextTraits that applies to all cells
//	and uses a 'STR#' resource to specify the initial items in the column
//
//	Data is text with no length byte. To add rows at runtime, call
//	SetCellData(), passing a pointer to the first character and byte count.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LTextColumn.h>
#include <LStream.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LTextColumn							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LTextColumn::LTextColumn(
	LStream*	inStream)

	: LColumnView(inStream)
{
	*inStream >> mTxtrID;

		// Initial text items in column come from a 'STR#' resource

	ResIDT	theSTRxID;
	*inStream >> theSTRxID;

	Handle	strxH = ::GetResource(FOUR_CHAR_CODE('STR#'), theSTRxID);
	if (strxH != nil) {
		SInt16	strCount = *(SInt16*)(*strxH);
		InsertRows(strCount, 0, nil, 0, Refresh_No);

		STableCell	cell(0, 1);
		Str255		str;
		for (SInt16 i = 1; i <= strCount; ++i) {
			::GetIndString(str, theSTRxID, i);
			cell.row = i;
			SetCellData(cell, str+1, str[0]);	// Store text without length byte
		}

		// Note: We don't release the STR# resource.
		// Mark it purgeable to allow the System to release it.
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LTextColumn							Destructor				  [public]
// ---------------------------------------------------------------------------

LTextColumn::~LTextColumn()
{
}


// ---------------------------------------------------------------------------
//	¥ DrawCell
// ---------------------------------------------------------------------------
//	Draw text within a Cell

void
LTextColumn::DrawCell(
	const STableCell&	inCell,
	const Rect&			inLocalRect)
{
	Rect	textRect = inLocalRect;
	::MacInsetRect(&textRect, 2, 0);

	char	str[255];				// Data is text with no length byte
	UInt32	len = sizeof(str);
	GetCellData(inCell, str, len);

	SInt16	just = UTextTraits::SetPortTextTraits(mTxtrID);
	UTextDrawing::DrawWithJustification(str, (SInt32) len, textRect, just);
}


PP_End_Namespace_PowerPlant
