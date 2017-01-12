// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDragAndDrop.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDragAndDrop
#define _H_LDragAndDrop
#pragma once

#include <TArray.h>
#include <UMemoryMgr.h>

#include <Drag.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LPane;
class	LDropArea;

typedef struct	SDropAreaEntry {
	LDropArea	*theDropArea;
	WindowPtr	theMacWindow;
} SDropAreaEntry, **SDropAreaEntryH;


// ---------------------------------------------------------------------------
#pragma mark LDropArea

class	LDropArea {
public:
						LDropArea( WindowPtr inWindow );

	virtual				~LDropArea();

						// Pure Virtual. Concrete subclasses must override
	virtual Boolean		PointInDropArea( Point inGlobalPt) = 0;

	virtual void		FocusDropArea();

	static bool			DragAndDropIsPresent();

	static void			RemoveHandlers();

protected:
	WindowPtr	mDragWindow;
	DragRef		mAcceptableDrag;
	Boolean		mCanAcceptCurrentDrag;
	Boolean		mIsHilited;

	virtual void		HiliteDropArea( DragReference inDragRef );
	
	virtual void		UnhiliteDropArea( DragReference inDragRef );

	virtual Boolean		DragIsAcceptable( DragReference inDragRef );
	
	virtual Boolean		ItemIsAcceptable(
								DragReference		inDragRef,
								ItemReference		inItemRef);

	virtual void		EnterDropArea(
								DragReference		inDragRef,
								Boolean				inDragHasLeftSender);
								
	virtual void		LeaveDropArea( DragReference inDragRef );
								
	virtual void		InsideDropArea( DragReference inDragRef );

	virtual void		DoDragReceive( DragReference inDragRef );
	
	virtual void		ReceiveDragItem(
								DragReference		inDragRef,
								DragAttributes		inDragAttrs,
								ItemReference		inItemRef,
								Rect&				inItemBounds);

	virtual void		DoDragSendData(
								FlavorType			inFlavor,
								ItemReference		inItemRef,
								DragReference		inDragRef);

	virtual void		DoDragInput(
								Point*				ioMouse,
								SInt16*				ioModifiers,
								DragReference		inDragRef);

	virtual void		DoDragDrawing(
								DragRegionMessage	inMessage,
								RgnHandle			inShowRgn,
								Point				inShowOrigin,
								RgnHandle			inHideRgn,
								Point				inHideOrigin,
								DragReference		inDragRef);

	// === Static Members ===


	static	StDeleter< TArray<SDropAreaEntry> >	sDropAreaList;
	static	LDropArea*							sCurrentDropArea;
	static	bool								sDragHasLeftSender;

	static void			AddDropArea(
								LDropArea*			inDropArea,
								WindowPtr			inMacWindow);
								
	static void			RemoveDropArea(
								LDropArea*			inDropArea,
								WindowPtr			inMacWindow);
								
	static LDropArea*	FindDropArea(
								WindowPtr			inMacWindow,
								Point				inGlobalPoint,
								DragReference		inDragRef);

	static void			InTrackingWindow(
								WindowPtr			inMacWindow,
								DragReference		inDragRef);

	static pascal OSErr	HandleDragTracking(
								DragTrackingMessage	inMessage,
								WindowPtr			inMacWindow,
								void*				inRefCon,
								DragReference		inDragRef);

	static pascal OSErr	HandleDragReceive(
								WindowPtr			inMacWindow,
								void*				inRefCon,
								DragReference		inDragRef);

	static pascal OSErr	HandleDragSendData(
								FlavorType			inFlavor,
								void*				inRefCon,
								ItemReference		inItemRef,
								DragReference		inDragRef);

	static pascal OSErr	HandleDragInput(
								Point*				ioMouse,
								SInt16*				ioModifiers,
								void*				inRefCon,
								DragReference		inDragRef);

	static pascal OSErr	HandleDragDrawing(
								DragRegionMessage	inMessage,
								RgnHandle			inShowRgn,
								Point				inShowOrigin,
								RgnHandle			inHideRgn,
								Point				inHideOrigin,
								void*				inRefCon,
								DragReference		inDragRef);
};


// ---------------------------------------------------------------------------
#pragma mark LDragAndDrop

class	LDragAndDrop : public LDropArea {
public:
						LDragAndDrop(
								WindowPtr			inMacWindow,
								LPane*				inPane);

	virtual				~LDragAndDrop();

	virtual Boolean		PointInDropArea( Point inGlobalPt );
	
	virtual void		FocusDropArea();

protected:
	LPane*		mPane;

	virtual void		HiliteDropArea( DragReference inDragRef );
	
	virtual void		UnhiliteDropArea( DragReference inDragRef );
};


// ---------------------------------------------------------------------------
#pragma mark StDragTrackingUPP

class StDragTrackingUPP {
public:
			StDragTrackingUPP(
					DragTrackingHandlerProcPtr	inProcPtr,
					WindowPtr					inWindow,
					void*						inRefCon = nil);
			
			~StDragTrackingUPP();
			
			operator DragTrackingHandlerUPP()	{ return mTrackingUPP; }
			
protected:
	DragTrackingHandlerUPP	mTrackingUPP;
	WindowPtr				mWindow;
};


// ---------------------------------------------------------------------------
#pragma mark StDragReceiveUPP

class StDragReceiveUPP {
public:
			StDragReceiveUPP(
					DragReceiveHandlerProcPtr	inProcPtr,
					WindowPtr					inWindow,
					void*						inRefCon = nil);
					
			~StDragReceiveUPP();
			
			operator DragReceiveHandlerUPP()	{ return mReceiveUPP; }
			
protected:
	DragReceiveHandlerUPP	mReceiveUPP;
	WindowPtr				mWindow;
};



PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
