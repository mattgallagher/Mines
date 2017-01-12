// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDragTask.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class which encapsulates a single drag action initiated using the
//	Drag Manager. Normally, you will use this class in conjunction
//	with the LDragAndDrop class.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDragTask.h>
#include <UMemoryMgr.h>

#include <Aliases.h>
#include <Folders.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LDragTask								Constructor				  [public]
// ---------------------------------------------------------------------------
//	A DragTask must be associated with a Toolbox mouse down EventRecord

LDragTask::LDragTask(
	const EventRecord&	inEventRecord)

	: mEventRecord(inEventRecord)
{
	InitDragTask();
}


// ---------------------------------------------------------------------------
//	¥ LDragTask								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor for dragging a single, rectangular item
//
//	The inItemRect is in the local coordinates of the current port, so you
//	may need to call SetPort (or FocusDraw) beforehand.

LDragTask::LDragTask(
	const EventRecord&	inEventRecord,
	const Rect&			inItemRect,
	ItemReference		inItemRef,
	FlavorType			inFlavor,
	void*				inDataPtr,
	Size				inDataSize,
	FlavorFlags			inFlags)

	: mEventRecord(inEventRecord)
{
	InitDragTask();

	::AddDragItemFlavor(mDragRef, inItemRef, inFlavor, inDataPtr, inDataSize,
							inFlags);

	Rect	globalRect = inItemRect;
	::LocalToGlobal(&topLeft(globalRect));
	::LocalToGlobal(&botRight(globalRect));
	AddRectDragItem(inItemRef, globalRect);

	::TrackDrag(mDragRef, &mEventRecord, mDragRegion);
}


// ---------------------------------------------------------------------------
//	¥ LDragTask								Constructor				  [public]
// ---------------------------------------------------------------------------
//	Constructor for dragging a single item outlined by a region
//
//	inItemRgn must be in global coordinates, and LDragTask will NOT dispose
//	of inItemRgn.

LDragTask::LDragTask(
	const EventRecord&	inEventRecord,
	RgnHandle			inItemRgn,
	ItemReference		inItemRef,
	FlavorType			inFlavor,
	void*				inDataPtr,
	Size				inDataSize,
	FlavorFlags			inFlags)

	: mEventRecord(inEventRecord)
{
	InitDragTask();

	::AddDragItemFlavor(mDragRef, inItemRef, inFlavor, inDataPtr, inDataSize,
							inFlags);
							
	Rect	itemBounds;
	::SetDragItemBounds(mDragRef, inItemRef,
							::GetRegionBounds(inItemRgn, &itemBounds));

	::TrackDrag(mDragRef, &mEventRecord, inItemRgn);
}


// ---------------------------------------------------------------------------
//	¥ InitDragTask							Initializer				 [private]
// ---------------------------------------------------------------------------

void
LDragTask::InitDragTask()
{
	ThrowIfOSErr_( ::NewDrag(&mDragRef) );
}


// ---------------------------------------------------------------------------
//	¥ ~LDragTask							Destructor				  [public]
// ---------------------------------------------------------------------------

LDragTask::~LDragTask()
{
	::DisposeDrag(mDragRef);
}


// ---------------------------------------------------------------------------
//	¥ DoDrag														  [public]
// ---------------------------------------------------------------------------
//	Perform a DragTask
//
//	This function calls other member functions to add items to the drag
//	and build the drag region, then calls the Drag Mananger to track
//	the drag. You will not normally override this function.

OSErr
LDragTask::DoDrag()
{
	AddFlavors(mDragRef);
	MakeDragRegion(mDragRef, mDragRegion);
	return ::TrackDrag(mDragRef, &mEventRecord, mDragRegion);
}


// ---------------------------------------------------------------------------
//	¥ DropLocationIsFinderTrash										  [public]
// ---------------------------------------------------------------------------
//	Return whether the drop location of the Drag is the Finder's Trash can
//
//	Normally, you'll call this routine after the Drag completes to
//	determine whether to delete the dragged items.

bool
LDragTask::DropLocationIsFinderTrash()
{
	OSErr		err;

	StAEDescriptor	dropLocation;	// Get AE descriptor of drop location
	::GetDropLocation(mDragRef, dropLocation);
	
	StAEDescriptor	dropSpecDesc;		// Coerce drop location into
										//   an FSSpec
	err = ::AECoerceDesc(dropLocation, typeFSS, dropSpecDesc);
	
	if (err != noErr) return false;		// Drop location not defined by an
										//   FSSpec, so it can't be the trash
										
		// Coercion worked. Get FSSpec from the AE descriptor.
		// Use accessor function if available. Otherwise, copy
		// FSSpec out of the data handle.
		
	FSSpec	dropSpec;

	#if ACCESSOR_CALLS_ARE_FUNCTIONS
	
		err = ::AEGetDescData(dropSpecDesc, &dropSpec, sizeof(FSSpec));
		ThrowIfOSErr_(err);

	#else
	
		dropSpec = *((FSSpec*) *dropSpecDesc.mDesc.dataHandle);

	#endif
	
										// Handle case where drop location is
	Boolean		isFolder;				//   an alias to the trash
	Boolean		wasAlias;
	err = ::ResolveAliasFile(&dropSpec, true, &isFolder, &wasAlias);
	
										// Can't be the trash if it's a bad
										//   alias or not a folder
	if ( (err != noErr) || (not isFolder) ) return false;
		
										// FSSpec contains parent dir ID
										//   PBGetCatInfo will give us the
	CInfoPBRec	cpb;					//   actual dir ID
	cpb.hFileInfo.ioFDirIndex	= 0;
	cpb.hFileInfo.ioNamePtr		= dropSpec.name;
	cpb.hFileInfo.ioVRefNum		= dropSpec.vRefNum;
	cpb.hFileInfo.ioDirID		= dropSpec.parID;
	err = ::PBGetCatInfoSync(&cpb);
	
	if (err != noErr) return false;		// Bad file isn't the trash

										// Get vol ref num and dir ID
										//   of the Trash
	short	trashVRefNum;
	long	trashDirID;
	err = ::FindFolder(dropSpec.vRefNum, kTrashFolderType,
						kDontCreateFolder, &trashVRefNum, &trashDirID);
	
										// Finally, we can compare the drop
										//   location to the trash
	return	(err == noErr) &&
			(cpb.hFileInfo.ioVRefNum == trashVRefNum) &&
			(cpb.hFileInfo.ioDirID == trashDirID);
}


// ---------------------------------------------------------------------------
//	¥ AddFlavors												   [protected]
// ---------------------------------------------------------------------------
//	Add flavored items to the DragTask.
//
//	If you use the short form of the Constructor (EventRecord only), you
//	must override this function to add items to the DragTask

void
LDragTask::AddFlavors(
	DragReference	/* inDragRef */)
{
}


// ---------------------------------------------------------------------------
//	¥ MakeDragRegion											   [protected]
// ---------------------------------------------------------------------------
//	Build the region outlining the items to be dragged
//
//	If you use the short form of the Constructor (EventRecord only), you
//	must override this function to specify the drag region for each item
//	in the DragTask.

void
LDragTask::MakeDragRegion(
	DragReference	/* inDragRef */,
	RgnHandle		/* inDragRegion */)
{
}


// ---------------------------------------------------------------------------
//	¥ AddRectDragItem											   [protected]
// ---------------------------------------------------------------------------
//	Utilitly function for adding a rectangular item to the Drag
//
//	The inItemRect must be in Global coordinates

void
LDragTask::AddRectDragItem(
	ItemReference	inItemRef,
	const Rect&		inItemRect)
{
	StRegion	outerRgn(inItemRect);	// Make region containing item

	StRegion	innerRgn = outerRgn;	// Carve out interior of region so
	::InsetRgn(innerRgn, 1, 1);			//   that it's just a one-pixel thick
	outerRgn -= innerRgn;				//   outline of the item rectangle

	mDragRegion += outerRgn;			// Accumulate this item in our
										//   total drag region

										// Tell Drag Manager about this item
	::SetDragItemBounds(mDragRef, inItemRef, &inItemRect);
}


PP_End_Namespace_PowerPlant
