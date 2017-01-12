// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDragAndDrop.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Abstract class for an area of a Window which supports Drag and Drop
//	using the Drag Manager. Normally, you will use this class as a mix-in
//	base class for a LPane subclass. However, this class does not depend
//	on any other classes, so you can use it independently as a "wrapper"
//	class for the Drag Manager.
//
//	Usage Note: This class defines, but does not install, call back
//	routines for custom send data, input, and drawing hooks. If you
//	wish to use these hooks, you must install them by calling
//		SetDragSendProc, SetDragInputProc, or SetDragDrawingProc
//	yourself.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDragAndDrop.h>
#include <LView.h>
#include <UDrawingState.h>
#include <UEnvironment.h>
#include <URegions.h>

#include <Gestalt.h>


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Class Variables

StDeleter< TArray<SDropAreaEntry> >	LDropArea::sDropAreaList;
LDropArea*							LDropArea::sCurrentDropArea   = nil;
bool								LDropArea::sDragHasLeftSender = false;


// ---------------------------------------------------------------------------
//	¥ LDropArea								Constructor				  [public]
// ---------------------------------------------------------------------------

LDropArea::LDropArea(
	WindowPtr	inWindow)
{
	mDragWindow				= inWindow;
	mCanAcceptCurrentDrag	= false;
	mIsHilited				= false;

	if ((sDropAreaList.Get() == nil) && DragAndDropIsPresent()) {
										// Need to initialize Drag and Drop
		UEnvironment::SetFeature(env_HasDragManager, true);
		
			// Install handlers for tracking and receiving drags.
			// Passing nil for the second parameter, a WindowPtr,
			// means that our handlers get called for all windows.
			// Our handlers are allocated the first time a DropArea
			// is constructed and remain installed for the duration
			// of the program. All subsequent DropAreas use these
			// same handlers.
		
		static StDragTrackingUPP	sDragTrackingUPP(HandleDragTracking, nil);
		
		static StDragReceiveUPP		sDragReceiveUPP(HandleDragReceive, nil);

		sDropAreaList.Adopt( new TArray<SDropAreaEntry> );
	}

	if (UEnvironment::HasFeature(env_HasDragManager)) {
		AddDropArea(this, inWindow);
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LDropArea							Destructor				  [public]
// ---------------------------------------------------------------------------

LDropArea::~LDropArea()
{
	if (UEnvironment::HasFeature(env_HasDragManager)) {
		RemoveDropArea(this, mDragWindow);
	}
}


// ---------------------------------------------------------------------------
//	¥ FocusDropArea													  [public]
// ---------------------------------------------------------------------------
//	Set up local coordinate system and clipping region for a DropArea
//
//	The Drag Manager sets the port to the Window containing the drop
//	rectangle. However, it doesn't know anything about the local coordinates
//	and clipping region.

void
LDropArea::FocusDropArea()
{
}


// ---------------------------------------------------------------------------
//	¥ DragAndDropIsPresent									 [static] [public]
// ---------------------------------------------------------------------------
//	Returns whether DragAndDrop is implemented on the running System

bool
LDropArea::DragAndDropIsPresent()
{
	return (UEnvironment::HasGestaltAttribute(gestaltDragMgrAttr,
											  gestaltDragMgrPresent) &&
			CFM_AddressIsResolved_(::NewDrag));
}


// ---------------------------------------------------------------------------
//	¥ HiliteDropArea												  [public]
// ---------------------------------------------------------------------------
//	Hilite a DropArea to indicate that it can accept the current Drag
//
//	Subclasses should override this function to create a region
//	representing the drop area and call ShowDragHilite().

void
LDropArea::HiliteDropArea(
	DragReference	/* inDragRef */)
{
	mIsHilited = false;
}


// ---------------------------------------------------------------------------
//	¥ UnhiliteDropArea												  [public]
// ---------------------------------------------------------------------------
//	Unhilite a DropArea when a drag leaves the area
//
//	Subclasses should override this function if they override
//	HiliteDropArea() to do something other than call ShowDragHilite()

void
LDropArea::UnhiliteDropArea(
	DragReference	inDragRef)
{
	::HideDragHilite(inDragRef);
}


// ---------------------------------------------------------------------------
//	¥ EnterDropArea													  [public]
// ---------------------------------------------------------------------------
//	A Drag is entering a DropArea. This call will be followed by a
//	corresponding LeaveDropArea call (when the Drag moves out of the
//	DropArea or after the Drag is received by this DropArea).
//
//	If the DropArea can accept the Drag and the Drag is coming from outside
//	the DropArea, hilite the DropArea

void
LDropArea::EnterDropArea(
	DragReference	inDragRef,
	Boolean			inDragHasLeftSender)
{
	if (inDragHasLeftSender) {
		FocusDropArea();
		mIsHilited = true;
		HiliteDropArea(inDragRef);
	}
}


// ---------------------------------------------------------------------------
//	¥ LeaveDropArea													  [public]
// ---------------------------------------------------------------------------
//	A Drag is leaving a DropArea. This call will have been preceded by
//	a corresponding EnterDropArea call.
//
//	Remove hiliting of the DropArea if necessary

void
LDropArea::LeaveDropArea(
	DragReference	inDragRef)
{
	if (mIsHilited) {
		FocusDropArea();
		mIsHilited = false;
		UnhiliteDropArea(inDragRef);
	}

	mCanAcceptCurrentDrag = false;
}


// ---------------------------------------------------------------------------
//	¥ InsideDropArea												  [public]
// ---------------------------------------------------------------------------
//	Track a Drag while it is inside a DropArea. This function is called
//	repeatedly while an acceptable Drag is inside a DropArea.
//
//	Subclasses may override to provide additional visual feedback during
//	a Drag, such as indicating an insertion point

void
LDropArea::InsideDropArea(
	DragReference	/* inDragRef */)
{
}


// ---------------------------------------------------------------------------
//	¥ DragIsAcceptable												  [public]
// ---------------------------------------------------------------------------
//	Return whether a DropArea can accept the specified Drag
//
//	A Drag is acceptable if all items in the Drag are acceptable

Boolean
LDropArea::DragIsAcceptable(
	DragReference	inDragRef)
{
	UInt16	itemCount;
	::CountDragItems(inDragRef, &itemCount);
	
								// Assume it's acceptable if there are items
	Boolean	isAcceptable = (itemCount > 0);

	for (UInt16 item = 1; item <= itemCount; item++) {
		ItemReference	itemRef;
		::GetDragItemReferenceNumber(inDragRef, item, &itemRef);

		isAcceptable = ItemIsAcceptable(inDragRef, itemRef);
		if (!isAcceptable) {
			break;				// Stop looping upon finding an
		}						//   unaccepatable item
	}

	return isAcceptable;
}


// ---------------------------------------------------------------------------
//	¥ ItemIsAcceptable												  [public]
// ---------------------------------------------------------------------------
//	Return whether a Drag item is acceptable
//
//	You should override this function to check for the flavor types that
//	your subclass supports. Here is what the function body would be for
//	a DropArea that accepts only 'TEXT' data:
//
//		FlavorFlags		theFlags;
//		return (GetFlavorFlags(inDragRef, inItemRef, 'TEXT', &theFlags)
//					== noErr);
//
//	This function requests the FlavorFlags for the 'TEXT' flavor.
//	GetFlavorFlags() returns "badDragFlavorType" if the flavor is not
//	present. It returns "noErr" if the flavor is present.
//
//	To accept more than one flavor, make a call to GetFlavorFlags() for
//	each flavor. For example, to accept 'TEXT', 'PICT', and 'snd ':
//
//		FlavorFlags		theFlags;
//		OSErr			err;
//		err = GetFlavorFlags(inDragRef, inItemRef, 'TEXT', &theFlags);
//		if (err == noErr) return true;
//
//		err = GetFlavorFlags(inDragRef, inItemRef, 'PICT', &theFlags);
//		if (err == noErr) return true;
//
//		err = GetFlavorFlags(inDragRef, inItemRef, 'snd ', &theFlags);
//		return (err == noErr);
//
//	This function checks for each acceptable flavor, exiting immediately
//	upon finding an acceptable flavor.


Boolean
LDropArea::ItemIsAcceptable(
	DragReference	/* inDragRef */,
	ItemReference	/* inItemRef */)
{
	return false;
}


// ---------------------------------------------------------------------------
//	¥ DoDragReceive													  [public]
// ---------------------------------------------------------------------------
//	Receive the items from a completed Drag and Drop
//
//	This function gets called after items are dropped into a DropArea.
//	The drag contains items that are acceptable, as defined by the
//	DragIsAcceptable() and ItemIsAcceptable() member functions.
//
//	This function repeatedly calls ReceiveDragItem for each item in the drag.
//	Override if you want to process the dragged items all at once.

void
LDropArea::DoDragReceive(
	DragReference	inDragRef)
{
	DragAttributes	dragAttrs;
	::GetDragAttributes(inDragRef, &dragAttrs);

	UInt16	itemCount;				// Number of Items in Drag
	::CountDragItems(inDragRef, &itemCount);

	for (UInt16 item = 1; item <= itemCount; item++) {
		ItemReference	itemRef;
		::GetDragItemReferenceNumber(inDragRef, item, &itemRef);

		Rect	itemBounds;			// Get bounds of Item in Local coords
		LView::OutOfFocus(nil);
		FocusDropArea();
		::GetDragItemBounds(inDragRef, itemRef, &itemBounds);
		::GlobalToLocal(&topLeft(itemBounds));
		::GlobalToLocal(&botRight(itemBounds));

		ReceiveDragItem(inDragRef, dragAttrs, itemRef, itemBounds);
	}
}


// ---------------------------------------------------------------------------
//	¥ ReceiveDragItem												  [public]
// ---------------------------------------------------------------------------
//	Process an Item which has been dragged into a DropArea
//
//	This function gets called once for each Item contained in a completed
//	Drag. The Item will have returned true from ItemIsAcceptable().
//
//	The DropArea is focused upon entry and inItemBounds is specified
//	in the local coordinates of the DropArea.
//
//	Override this function if the DropArea can accept dropped Items.
//	You may want to call GetFlavorData and GetFlavorDataSize if there
//	is information associated with the dragged Item.

void
LDropArea::ReceiveDragItem(
	DragReference	/* inDragRef */,
	DragAttributes	/* inDragAttrs */,
	ItemReference	/* inItemRef */,
	Rect&			/* inItemBounds */)	// In Local coordinates
{
}


// ---------------------------------------------------------------------------
//	¥ DoDragSendData												  [public]
// ---------------------------------------------------------------------------
//	Send the data associated with a particular drag item
//
//	This function gets called if you installed the optional DragSendDataProc
//	for this DropArea. In which case you should override this function
//	to provide the requested data by calling SetDragItemFlavorData.

void
LDropArea::DoDragSendData(
	FlavorType		/* inFlavor */,
	ItemReference	/* inItemRef */,
	DragReference	/* inDragRef */)
{
}


// ---------------------------------------------------------------------------
//	¥ DoDragInput													  [public]
// ---------------------------------------------------------------------------
//	Modify the state of the mouse and modifier keys during a drag
//
//	This function gets called if you installed the optional DragInputProc
//	for this DropArea.

void
LDropArea::DoDragInput(
	Point*			/* ioMouse */,
	SInt16*			/* ioModifiers */,
	DragReference	/* inDragRef */)
{
}


// ---------------------------------------------------------------------------
//	¥ DoDragDrawing													  [public]
// ---------------------------------------------------------------------------
//	Draw the items for a drag in progress
//
//	This function gets called if you installed the optional DragDrawingProc
//	for this DropArea.

void
LDropArea::DoDragDrawing(
	DragRegionMessage	/* inMessage */,
	RgnHandle			/* inShowRgn */,
	Point				/* inShowOrigin */,
	RgnHandle			/* inHideRgn */,
	Point				/* inHideOrigin */,
	DragReference 		/* inDragRef */)
{
}

#pragma mark -
// ===========================================================================
// ¥ Static Functions										Static Functions ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ AddDropArea
// ---------------------------------------------------------------------------
//	Add a DropArea to the list maintained by this Class

void
LDropArea::AddDropArea(
	LDropArea*		inDropArea,
	WindowPtr		inMacWindow)
{
	if (sDropAreaList != nil) {
		SDropAreaEntry	theEntry;
		theEntry.theDropArea = inDropArea;
		theEntry.theMacWindow = inMacWindow;

		sDropAreaList->AddItem(theEntry);
	}
}


// ---------------------------------------------------------------------------
//	¥ RemoveDropArea
// ---------------------------------------------------------------------------
//	Remove a DropArea from the list maintained by this Class

void
LDropArea::RemoveDropArea(
	LDropArea*	inDropArea,
	WindowPtr	inMacWindow)
{
	if (sDropAreaList != nil) {
		SDropAreaEntry	theEntry;
		theEntry.theDropArea = inDropArea;
		theEntry.theMacWindow = inMacWindow;

		sDropAreaList->Remove(theEntry);
	}
}


// ---------------------------------------------------------------------------
//	¥ FindDropArea
// ---------------------------------------------------------------------------
//	Return the DropArea in the specified window containing the specified
//	point (in global coords) and which can receive the Drag
//
//	Returns nil if no DropArea meets the requirements.

LDropArea*
LDropArea::FindDropArea(
	WindowPtr		inMacWindow,
	Point			inGlobalPt,
	DragReference	inDragRef)
{
		// Search DropArea List from back to front for a DropArea
		// that is in the window, contains the point, and can accept
		// the drag.
		//
		// We search from back to front because nested DropAreas
		// are usually added after the surrounding DropArea. Searching
		// back to front should gives us the most deeply nested DropArea.

	LDropArea	*theDropArea = nil;
	SInt32	count = (SInt32) sDropAreaList->GetCount();

	for (SInt32 i = count; i > 0; i--) {
		SDropAreaEntry	theEntry;
		sDropAreaList->FetchItemAt(i, theEntry);
		
		if ( (theEntry.theMacWindow == inMacWindow) &&
			 (theEntry.theDropArea)->PointInDropArea(inGlobalPt) &&
			 (theEntry.theDropArea)->DragIsAcceptable(inDragRef) ) {
			   
			theDropArea = theEntry.theDropArea;
			break;
		}
	}

	return theDropArea;
}


// ---------------------------------------------------------------------------
//	¥ InTrackingWindow
// ---------------------------------------------------------------------------
//	Track a Drag while it is inside a Window

void
LDropArea::InTrackingWindow(
	WindowPtr		inMacWindow,
	DragReference	inDragRef)
{
	Point	mouseLoc;				// Get mouse location in Global coords
	::GetDragMouse(inDragRef, &mouseLoc, nil);

									// Get DropArea that contains the mouse
	LDropArea *theDropArea = FindDropArea(inMacWindow, mouseLoc, inDragRef);

	if (theDropArea == nil) {		// Mouse is not in a DropArea
		if (sCurrentDropArea != nil) {
										// Mouse left former DropArea
			sCurrentDropArea->LeaveDropArea(inDragRef);
		}
		sCurrentDropArea = nil;
		sDragHasLeftSender = true;

	} else {						// Mouse is in a DropArea that can
									//   accept the current Drag
		theDropArea->mCanAcceptCurrentDrag = true;

		if (theDropArea == sCurrentDropArea) {
										// In Same drop area
			theDropArea->InsideDropArea(inDragRef);

		} else {						// Entering new drop area
			if (sCurrentDropArea != nil) {
										// Leaving old drop area
				sCurrentDropArea->LeaveDropArea(inDragRef);
				sDragHasLeftSender = true;
			}

			sCurrentDropArea = theDropArea;
			theDropArea->EnterDropArea(inDragRef, sDragHasLeftSender);
			theDropArea->InsideDropArea(inDragRef);
		}
	}
}


// ===========================================================================
// ¥ Static Callback Functions					   Static Callback Functions ¥
// ===========================================================================

// ---------------------------------------------------------------------------
//	¥ HandleDragTracking
// ---------------------------------------------------------------------------
//	Drag Manager callback for tracking a drag in progress

pascal OSErr
LDropArea::HandleDragTracking(
	DragTrackingMessage	inMessage,
	WindowPtr			inMacWindow,
	void*				/* inRefCon */,
	DragReference		inDragRef)
{
	OSErr	err = noErr;

	try {
		StGrafPortSaver	savePort;

		switch (inMessage) {

			case kDragTrackingEnterHandler: {
				DragAttributes	dragAttr;
				::GetDragAttributes(inDragRef, &dragAttr);
				sDragHasLeftSender = (dragAttr & kDragHasLeftSenderWindow) != 0;
				break;
			}

			case kDragTrackingEnterWindow:
				break;

			case kDragTrackingInWindow:
				InTrackingWindow(inMacWindow, inDragRef);
				break;

			case kDragTrackingLeaveWindow:
				if (sCurrentDropArea != nil) {
					sCurrentDropArea->LeaveDropArea(inDragRef);
					sDragHasLeftSender = true;
				}
				sCurrentDropArea = nil;
				break;

			case kDragTrackingLeaveHandler:
				break;
		}
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = dragNotAcceptedErr;
	}

	LView::OutOfFocus(nil);
	return err;
}


// ---------------------------------------------------------------------------
//	¥ HandleDragReceive
// ---------------------------------------------------------------------------
//	Drag Manager callback for receiving a successful drop

pascal OSErr
LDropArea::HandleDragReceive(
	WindowPtr		/* inMacWindow */,
	void*			/* inRefCon */,
	DragReference	inDragRef)
{
	OSErr	err = noErr;

	try {
		if ( (sCurrentDropArea != nil)  &&
			 sCurrentDropArea->mCanAcceptCurrentDrag ) {

			StGrafPortSaver	savePort;
			LView::OutOfFocus(nil);

			sCurrentDropArea->DoDragReceive(inDragRef);

		} else {
			err = dragNotAcceptedErr;
		}
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = dragNotAcceptedErr;
	}

	LView::OutOfFocus(nil);
	return err;
}


// ---------------------------------------------------------------------------
//	¥ HandleDragSendData
// ---------------------------------------------------------------------------
//	Drag Manager callback for sending the data for an item that is part
//	of an accepted drag and drop
//
//	Class does not automatically install a send data proc. You need
//	to call ::SetDragSendProc() yourself. For example,
//
//		::SetDragSendProc( theDrag,
//						   NewDragSendDataProc(LDropArea::HandleDragSendData),
//						   nil);

pascal OSErr
LDropArea::HandleDragSendData(
	FlavorType		inFlavor,
	void*			inRefCon,
	ItemReference	inItemRef,
	DragReference	inDragRef)
{
	SInt32	theA5 = ::SetCurrentA5();
	OSErr	err   = noErr;

	try {
		(static_cast<LDropArea*>(inRefCon))->DoDragSendData(inFlavor, inItemRef,
													inDragRef);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = dragNotAcceptedErr;
	}

	::SetA5(theA5);

	return err;
}


// ---------------------------------------------------------------------------
//	¥ HandleDragInput
// ---------------------------------------------------------------------------
//	Drag Manager callback for manipulating the mouse and modifier keys
//	during a drag
//
//	Class does not automatically install a drag input proc. You need
//	to call ::SetDragDrawingProc() yourself. For example,
//
//		::SetDragDrawingProc( theDrag,
//							NewDragInputProc(LDropArea::HandleDragInput),
//							nil);

pascal OSErr
LDropArea::HandleDragInput(
	Point*			ioMouse,
	SInt16*			ioModifiers,
	void*			inRefCon,
	DragReference	inDragRef)
{
	SInt32	theA5 = ::SetCurrentA5();
	OSErr	err   = noErr;

	try {
		(static_cast<LDropArea*>(inRefCon))->DoDragInput(ioMouse, ioModifiers,
													inDragRef);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = dragNotAcceptedErr;
	}

	::SetA5(theA5);

	return err;
}


// ---------------------------------------------------------------------------
//	¥ HandleDragDrawing
// ---------------------------------------------------------------------------
//	Drag Manager callback for drawing the items during a drag
//
//	Class does not automatically install a drag input proc. You need
//	to call ::SetDragInputProc() yourself. For example,
//
//		::SetDragInputProc( theDrag,
//							NewDragDrawingProc(LDropArea::HandleDragDrawing),
//							nil);

pascal OSErr
LDropArea::HandleDragDrawing(
	DragRegionMessage	inMessage,
	RgnHandle			inShowRgn,
	Point				inShowOrigin,
	RgnHandle			inHideRgn,
	Point				inHideOrigin,
	void*				inRefCon,
	DragReference		inDragRef)
{
	SInt32	theA5 = ::SetCurrentA5();
	OSErr	err = noErr;

	try {
		(static_cast<LDropArea*>(inRefCon))->DoDragDrawing(inMessage,
												inShowRgn, inShowOrigin,
												inHideRgn, inHideOrigin,
												inDragRef);
	}

	catch (ExceptionCode inErr) {
		err = (OSErr) inErr;
	}

	catch (const LException& inException) {
		err = (OSErr) inException.GetErrorCode();
	}

	catch (...) {
		err = dragNotAcceptedErr;
	}

	::SetA5(theA5);

	return err;
}

#pragma mark -

// ===========================================================================
// ¥ LDragAndDrop												LDragAndDrop ¥
// ===========================================================================
//	Mix-in class for adding Drag and Drop support to a Pane

// ---------------------------------------------------------------------------
//	¥ LDragAndDrop							Constructor				  [public]
// ---------------------------------------------------------------------------
//
//		inMacWindow is the Toolbox window containing the DragAndDrop.
//		It may be nil if the DragAndDrop is not in a window (e.g.,
//		if printing the DragAndDrop).
//
//		inPane is the PowerPlant Pane associated with the DragAndDrop.
//		The drop area of the DragAndDrop is the Frame of the Pane.

LDragAndDrop::LDragAndDrop(
	WindowPtr	inMacWindow,
	LPane*		inPane)
	
	: LDropArea(inMacWindow)
{
	mPane = inPane;
}


// ---------------------------------------------------------------------------
//	¥ ~LDragAndDrop							Destructor				  [public]
// ---------------------------------------------------------------------------

LDragAndDrop::~LDragAndDrop()
{
}


// ---------------------------------------------------------------------------
//	¥ PointInDropArea												  [public]
// ---------------------------------------------------------------------------
//	Return whether a Point, in Global coords, is inside a DropArea

Boolean
LDragAndDrop::PointInDropArea(
	Point	inPoint)
{
	mPane->GlobalToPortPoint(inPoint);
	return mPane->IsHitBy(inPoint.h, inPoint.v);
}


// ---------------------------------------------------------------------------
//	¥ FocusDropArea													  [public]
// ---------------------------------------------------------------------------
//	Set up local coordinate system and clipping region for a DropArea

void
LDragAndDrop::FocusDropArea()
{
	mPane->FocusDraw();
}


// ---------------------------------------------------------------------------
//	¥ HiliteDropArea											   [protected]
// ---------------------------------------------------------------------------
//	Hilite a DropArea to indicate that it can accept the current Drag
//
//	For a DragAndDrop, the drop area is the Frame of its associated
//	Pane inset by one pixel to account for the border which usually
//	surrounds a Drop-capable Pane.

void
LDragAndDrop::HiliteDropArea(
	DragReference	inDragRef)
{
	mPane->ApplyForeAndBackColors();

	Rect	dropRect;
	mPane->CalcLocalFrameRect(dropRect);
	::MacInsetRect(&dropRect, 1, 1);
	StRegion	dropRgn(dropRect);

	::ShowDragHilite(inDragRef, dropRgn, true);
}


// ---------------------------------------------------------------------------
//	¥ UnhiliteDropArea											   [protected]
// ---------------------------------------------------------------------------
//	Unhilite a DropArea when a drag leaves the area

void
LDragAndDrop::UnhiliteDropArea(
	DragReference	inDragRef)
{
	mPane->ApplyForeAndBackColors();
	::HideDragHilite(inDragRef);
}

#pragma mark -

// ===========================================================================
//	StDragTrackingUPP
// ===========================================================================
//	Constructor allocates and installs a DragTrackingHandlerUPP
//	Destructor removes and disposes of it

StDragTrackingUPP::StDragTrackingUPP(
	DragTrackingHandlerProcPtr	inProcPtr,
	WindowPtr					inWindow,
	void*						inRefCon)
{
	mTrackingUPP = nil;
	mWindow		 = inWindow;
	
	if (inProcPtr != nil) {
		mTrackingUPP = NewDragTrackingHandlerUPP(inProcPtr);
		::InstallTrackingHandler(mTrackingUPP, inWindow, inRefCon);
	}
}


StDragTrackingUPP::~StDragTrackingUPP()
{
	if (mTrackingUPP != nil) {
		::RemoveTrackingHandler(mTrackingUPP, mWindow);
		DisposeDragTrackingHandlerUPP(mTrackingUPP);
	}
}


#pragma mark -

// ===========================================================================
//	StDragReceiveUPP
// ===========================================================================
//	Constructor allocates and installs a DragReceiveHandlerUPP
//	Destructor removes and disposes of it

StDragReceiveUPP::StDragReceiveUPP(
	DragReceiveHandlerProcPtr	inProcPtr,
	WindowPtr					inWindow,
	void*						inRefCon)
{
	mReceiveUPP = nil;
	mWindow		= inWindow;
	
	if (inProcPtr != nil) {
		mReceiveUPP = NewDragReceiveHandlerUPP(inProcPtr);
		::InstallReceiveHandler(mReceiveUPP, inWindow, inRefCon);
	}
}


StDragReceiveUPP::~StDragReceiveUPP()
{
	if (mReceiveUPP != nil) {
		::RemoveReceiveHandler(mReceiveUPP, mWindow);
		DisposeDragReceiveHandlerUPP(mReceiveUPP);
	}
}


PP_End_Namespace_PowerPlant
