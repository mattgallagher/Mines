// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTableDropZone.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author: Andy Dent
//
//	A simple drop zone which appends to the table and highlights the entire
//	table

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LTableDropZone.h>
#include <LTableDragMsg.h>
#include <LTableView.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	¥ LTableDropZone
// ---------------------------------------------------------------------------
//	Normal Constructor

LTableDropZone::LTableDropZone(LTableView* inTable, FlavorType inFlavor) :
	LDragAndDrop(UQDGlobals::GetCurrentWindowPort(), inTable),
	mTargetTable(inTable),
	mSingleFlavor(inFlavor)
{}


// ---------------------------------------------------------------------------------
//		¥ ItemIsAcceptable
// ---------------------------------------------------------------------------------

Boolean
LTableDropZone::ItemIsAcceptable(
	DragReference	inDragRef,
	ItemReference	inItemRef )
{
	// Make sure the table is enabled and
	// there's text in the drag data.
	FlavorFlags	theFlags;
	return mTargetTable->IsEnabled() && (::GetFlavorFlags( inDragRef,
		inItemRef, mSingleFlavor, &theFlags ) == noErr);
}

PP_End_Namespace_PowerPlant
