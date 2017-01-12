// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDragTask.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDragTask
#define _H_LDragTask
#pragma once

#include <UAppleEventsMgr.h>
#include <URegions.h>

#include <Drag.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDragTask {
public:
					LDragTask( const EventRecord& inEventRecord );

					LDragTask(
							const EventRecord&		inEventRecord,
							const Rect&				inItemRect,
							ItemReference			inItemRef,
							FlavorType				inFlavor,
							void*					inDataPtr,
							Size					inDataSize,
							FlavorFlags				inFlags);

					LDragTask(
							const EventRecord&		inEventRecord,
							RgnHandle				inItemRgn,
							ItemReference			inItemRef,
							FlavorType				inFlavor,
							void*					inDataPtr,
							Size					inDataSize,
							FlavorFlags				inFlags);

	virtual			~LDragTask();

	DragReference	GetDragReference()		{ return mDragRef; }
	RgnHandle		GetDragRegion()			{ return mDragRegion; }

	virtual OSErr	DoDrag();

	bool			DropLocationIsFinderTrash();

protected:
	DragReference		mDragRef;
	StRegion			mDragRegion;
	const EventRecord&	mEventRecord;

	virtual void	AddFlavors( DragReference inDragRef );

	virtual void	MakeDragRegion(
							DragReference			inDragRef,
							RgnHandle				inDragRegion);

	virtual void	AddRectDragItem(
							ItemReference			inItemRef,
							const Rect&				inItemRect);

private:
	void			InitDragTask();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
