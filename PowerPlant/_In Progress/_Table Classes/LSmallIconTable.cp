// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSmallIconTable.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSmallIconTable.h>
#include <LTableMonoGeometry.h>
#include <LTableSingleSelector.h>
#include <LTableArrayStorage.h>

#include <Icons.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSmallIconTable						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LSmallIconTable::LSmallIconTable(
	LStream*	inStream)

	: LTableView(inStream)
{
	mTableGeometry = new LTableMonoGeometry(this, (UInt16) mFrameSize.width, 20);
	mTableSelector = new LTableSingleSelector(this);
	mTableStorage  = new LTableArrayStorage(this, sizeof(SIconTableRec));
}


// ---------------------------------------------------------------------------
//	¥ ~LSmallIconTable						Destructor				  [public]
// ---------------------------------------------------------------------------

LSmallIconTable::~LSmallIconTable()
{
}


// ---------------------------------------------------------------------------
//	¥ DrawCell													   [protected]
// ---------------------------------------------------------------------------

void
LSmallIconTable::DrawCell(
	const STableCell&	inCell,
	const Rect&			inLocalRect)
{
	SIconTableRec	iconAndName;
	UInt32			dataSize = sizeof(SIconTableRec);
	GetCellData(inCell, &iconAndName, dataSize);

	Rect	iconRect;
	iconRect.left   = (SInt16) (inLocalRect.left + 3);
	iconRect.right  = (SInt16) (iconRect.left + 16);
	iconRect.bottom = (SInt16) (inLocalRect.bottom - 2);
	iconRect.top    = (SInt16) (iconRect.bottom - 16);

	::PlotIconID(&iconRect, kAlignNone, kTransformNone, iconAndName.iconID);

	::TextSize(9);
	::TextFont(1);
	::MoveTo((SInt16) (inLocalRect.left + 22), (SInt16) (inLocalRect.bottom - 4));
	::DrawString(iconAndName.name);
}


PP_End_Namespace_PowerPlant
