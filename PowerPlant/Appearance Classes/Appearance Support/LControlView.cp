// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LControlView.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LControlView.h>
#include <LControlImp.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <URegistrar.h>

PP_Begin_Namespace_PowerPlant


// ===========================================================================
// ¥ LControlSubPane
// ===========================================================================

// ---------------------------------------------------------------------------
// ¥ LControlSubPane						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LControlSubPane::LControlSubPane(
	const SPaneInfo&	inPaneInfo,
	ClassIDT			inImpID,
	SInt16				inControlKind,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	SInt32				inRefCon)

	: LControlPane(inPaneInfo, inImpID, inControlKind, inTitle,
						inTextTraitsID, inValueMessage, inValue,
						inMinValue, inMaxValue, inRefCon)
{
}


// ---------------------------------------------------------------------------
// ¥ LControlSubPane						Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Does not create an Implementation object. You MUST call MakeControlImp()
//	before using the ControlSubPane.
//
//	Use this constructor in the initializer list for a subclass where you
//	don't know the control kind or title until later.

LControlSubPane::LControlSubPane(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue)

	: LControlPane(inPaneInfo, inValueMessage, inValue,
						inMinValue, inMaxValue)
{
}


// ---------------------------------------------------------------------------
// ¥ ~LControlSubPane						Destructor				  [public]
// ---------------------------------------------------------------------------

LControlSubPane::~LControlSubPane()
{
}


// ---------------------------------------------------------------------------
//	¥ GetContainer
// ---------------------------------------------------------------------------
//	Return the Container of a ControlSubPane
//
//	A ControlSubPane's SuperView is an LControlView. This dual Pane
//	structure is an implementation detail. The logical container of
//	the ControlSubPane is the SuperView of its LControlView.

LPane*
LControlSubPane::GetContainer() const
{
	return mSuperView->GetSuperView();
}


// ---------------------------------------------------------------------------
// ¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LControlSubPane::HotSpotResult(
	SInt16	inHotSpot)
{
	LControlView*	superView = dynamic_cast<LControlView*>(mSuperView);

	if (superView != nil) {
		superView->HotSpotResult(inHotSpot);
	}
}


// ---------------------------------------------------------------------------
// ¥ Draw
// ---------------------------------------------------------------------------

void
LControlSubPane::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	if (inSuperDrawRgnH == nil) {
		mSuperView->Draw(nil);

	} else {
		LControlPane::Draw(inSuperDrawRgnH);
	}
}


// ---------------------------------------------------------------------------
// ¥ Refresh
// ---------------------------------------------------------------------------

void
LControlSubPane::Refresh()
{
	mSuperView->Refresh();
}


// ---------------------------------------------------------------------------
// ¥ Click
// ---------------------------------------------------------------------------

void
LControlSubPane::Click(
	SMouseDownEvent	&inMouseDown)
{
	if (!inMouseDown.delaySelect) {

			// Until now, whereLocal is actually in port coords. Now
			// that we know what Pane is handling the click, we can
			// convert it to the proper local coords.

		PortToLocalPoint(inMouseDown.whereLocal);

		UpdateClickCount(inMouseDown);

			// A ControlSubPane is an implementation detail.
			// Logically, we should executed the click attachments of
			// the LControlView containing this pane.

		if (mSuperView->ExecuteAttachments(msg_Click, &inMouseDown)) {
			ClickSelf(inMouseDown);
		}
	}
}


// ---------------------------------------------------------------------------
// ¥ AdjustMouse
// ---------------------------------------------------------------------------

void
LControlSubPane::AdjustMouse(
	Point				inPortPt,
	const EventRecord&	inMacEvent,
	RgnHandle			outMouseRgn)
{
			// A ControlSubPane is an implementation detail.
			// Logically, we should executed the cursor attachments of
			// the LControlView containing this pane.
			
	SAdjustMouseEvent	adjuster;		// Load event and mouse region
	adjuster.event	  = inMacEvent;		//   for Attachments
	adjuster.mouseRgn = outMouseRgn;

	if (mSuperView->ExecuteAttachments(msg_AdjustCursor, (void*) &adjuster)) {
		LPane::AdjustMouse(inPortPt, inMacEvent, outMouseRgn);
	}
}


// ===========================================================================
// ¥ LControlView
// ===========================================================================
#pragma mark -

// ---------------------------------------------------------------------------
// ¥ LControlView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LControlView::LControlView(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LView(inStream)
{
	SControlInfo	cInfo;
	*inStream >> cInfo.valueMessage;
	*inStream >> cInfo.value;
	*inStream >> cInfo.minValue;
	*inStream >> cInfo.maxValue;

	MakeControlSubPane(inImpID, cInfo.valueMessage, cInfo.value,
					cInfo.minValue, cInfo.maxValue, inStream);
}


// ---------------------------------------------------------------------------
// ¥ LControlView							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LControlView::LControlView(
	LStream*	inStream)

	: LView(inStream)
{
	mControlSubPane = nil;			// Client must call MakeControlSubPane()
}


// ---------------------------------------------------------------------------
// ¥ LControlView
// ---------------------------------------------------------------------------

LControlView::LControlView(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	ClassIDT			inImpID,
	SInt16				inControlKind,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue)

	: LView(inPaneInfo, inViewInfo)
{
	MakeControlSubPane(inImpID, inControlKind, inTitle, inTextTraitsID,
					inValueMessage, inValue, inMinValue, inMaxValue);
}


// ---------------------------------------------------------------------------
// ¥ ~LControlView							Destructor
// ---------------------------------------------------------------------------

LControlView::~LControlView()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf
// ---------------------------------------------------------------------------

void
LControlView::FinishCreateSelf()
{
		// We need to call CalcRevealedRect() here because the calls
		// made during LView construction were made before our
		// ControlSubPane and its Imp were completely intialized.
		// Our revealed rect will be bigger if the underlying
		// control draws outside its Frame.
		
	CalcRevealedRect();

#if PP_Uses_Carbon_Events
	if (mControlSubPane != nil) {
		ControlRef		ctlRef = mControlSubPane->GetControlImp()->GetMacControl();
		EventTargetRef	tgtRef = ::GetControlEventTarget(ctlRef);
		mDrawEvent.Install(tgtRef, kEventClassControl, kEventControlDraw, this, &LControlView::DoDrawEvent);
	}
#endif
}


// ---------------------------------------------------------------------------
// ¥ MakeControlSubPane
// ---------------------------------------------------------------------------

void
LControlView::MakeControlSubPane(
	ClassIDT			inImpID,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue,
	LStream*			inStream)
{
	SPaneInfo	paneInfo;
	paneInfo.paneID		= PaneIDT_Unspecified;
	paneInfo.width		= mFrameSize.width;
	paneInfo.height		= mFrameSize.height;
	paneInfo.visible	= true;
	paneInfo.enabled	= true;
	
	paneInfo.bindings.left		= true;
	paneInfo.bindings.top		= true;
	paneInfo.bindings.right		= true;
	paneInfo.bindings.bottom	= true;
	
	paneInfo.left		= 0;
	paneInfo.top		= 0;
	paneInfo.userCon	= inValue;	// Store initial value. Some AM controls
									//   don't properly set the value.
	paneInfo.superView	= this;

	mControlSubPane = nil;

	mControlSubPane = new LControlSubPane(paneInfo, inValueMessage,
							inValue, inMinValue, inMaxValue);

	mControlSubPane->MakeControlImp(inImpID, inStream);

	mControlSubPane->AddListener(this);

	SetDefaultAttachable(this);		// Reset so Attachments don't get
									//   attached to the ControlSubPane
}


// ---------------------------------------------------------------------------
// ¥ MakeControlSubPane
// ---------------------------------------------------------------------------

void
LControlView::MakeControlSubPane(
	ClassIDT			inImpID,
	SInt16				inControlKind,
	ConstStringPtr		inTitle,
	ResIDT				inTextTraitsID,
	MessageT			inValueMessage,
	SInt32				inValue,
	SInt32				inMinValue,
	SInt32				inMaxValue)
{
	SPaneInfo	paneInfo;
	paneInfo.paneID		= PaneIDT_Unspecified;
	paneInfo.width		= mFrameSize.width;
	paneInfo.height		= mFrameSize.height;
	paneInfo.visible	= true;
	paneInfo.enabled	= true;
	
	paneInfo.bindings.left		= true;
	paneInfo.bindings.top		= true;
	paneInfo.bindings.right		= true;
	paneInfo.bindings.bottom	= true;
	
	paneInfo.left		= 0;
	paneInfo.top		= 0;
	paneInfo.userCon	= inValue;	// Store initial value. Some AM controls
									//   don't properly set the value.
	paneInfo.superView	= this;

	mControlSubPane = nil;

	mControlSubPane = new LControlSubPane(paneInfo, inImpID, inControlKind,
							inTitle, inTextTraitsID, inValueMessage,
							inValue, inMinValue, inMaxValue, 0);
							
	mControlSubPane->AddListener(this);

	SetDefaultAttachable(this);		// Reset so Attachments don't get
									//   attached to the ControlSubPane
}


// ---------------------------------------------------------------------------
//	¥ CalcRevealedRect
// ---------------------------------------------------------------------------
//	Calculate the portion of the Frame which is revealed through the
//	Frames of all SuperViews. RevealedRect is in Port coordinates.

void
LControlView::CalcRevealedRect()
{
		// To accommodate controls which draw outside their Frame,
		// this function calculates the portion of its Structure rect
		// that is revealed
	
	if (CalcPortStructureRect(mRevealedRect)) {
									// Frame is in QD Space
		if (mSuperView != nil) {	// Intersect Frame with RevealedRect
									//   of SuperView
			Rect	superRevealed;
			mSuperView->GetRevealedRect(superRevealed);
			::SectRect(&superRevealed, &mRevealedRect, &mRevealedRect);
		}

	} else {						// Frame not in QD Space
		mRevealedRect.left =		//   so RevealedRect is empty
			mRevealedRect.top =
			mRevealedRect.right =
			mRevealedRect.bottom = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcPortStructureRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the ControlView's Structure Rectangle in Port coordinates
//
//	A ControlView may draw outside of its Frame. This feature was added to
//	support Appearance Manager controls, which may draw outside their
//	control rectangles.
//
//	Returns true if the Rect is within QuickDraw space (16-bit)
//	Returns false if the Rect is outside QuickDraw space
//		and outRect is unchanged

bool
LControlView::CalcPortStructureRect(
	Rect&	outRect) const
{
	bool	isInQDSpace = CalcPortFrameRect(outRect);

	if (isInQDSpace) {
	
		// Get structure offsets from the Imp of our ControlSubPane.
		// We do this rather than calling CalcPortStructureRect() for
		// the ControlSubPane because there are times (during a MoveBy
		// or ResizeFrameBy operation) where the ControlView and the
		// ControlSubPane are temporarily out of sync.
		
		Rect	offsets;
		mControlSubPane->GetControlImp()->GetStructureOffsets(offsets);
		
		// Structure offsets should always be >= 0, indicating
		// how far outside the frame the structure is on each side
	
		outRect.left   -= offsets.left;
		outRect.top    -= offsets.top;
		outRect.right  += offsets.right;
		outRect.bottom += offsets.bottom;
	}
	
	return isInQDSpace;
}


// ---------------------------------------------------------------------------
//	¥ CalcPortExposedRect											  [public]
// ---------------------------------------------------------------------------
//	Calculate the exposed rectangle of a Pane and return whether that
//	rectangle is empty. The rectangle is in Port coordinates.

Boolean
LControlView::CalcPortExposedRect(
	Rect&	outRect,						// In Port coords
	bool	inOKIfHidden) const
{
	bool exposed = ((mVisible == triState_On) || inOKIfHidden)  &&
				   CalcPortStructureRect(outRect)  &&
				   (mSuperView != nil);

	if (exposed) {
		Rect	superRevealed;
		mSuperView->GetRevealedRect(superRevealed);

		exposed = ::SectRect(&outRect, &superRevealed, &outRect);
	}

	return exposed;
}


// ---------------------------------------------------------------------------
// ¥ GetDescriptor
// ---------------------------------------------------------------------------

StringPtr
LControlView::GetDescriptor(
	Str255		outDescriptor) const
{
	return mControlSubPane->GetDescriptor(outDescriptor);
}


// ---------------------------------------------------------------------------
// ¥ SetDescriptor
// ---------------------------------------------------------------------------

void
LControlView::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
	mControlSubPane->SetDescriptor(inDescriptor);
}


// ---------------------------------------------------------------------------
// ¥ GetValue
// ---------------------------------------------------------------------------

SInt32
LControlView::GetValue() const
{
	return mControlSubPane->GetValue();
}


// ---------------------------------------------------------------------------
// ¥ SetValue
// ---------------------------------------------------------------------------

void
LControlView::SetValue(
	SInt32		inValue)
{
	mControlSubPane->SetValue(inValue);
}


// ---------------------------------------------------------------------------
// ¥ ListenToMessage
// ---------------------------------------------------------------------------

void
LControlView::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	BroadcastMessage(inMessage, ioParam);
}


// ---------------------------------------------------------------------------
// ¥ SetDataTag
// ---------------------------------------------------------------------------

void
LControlView::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	mControlSubPane->SetDataTag(inPartCode, inTag,
						inDataSize, inDataPtr);
}


// ---------------------------------------------------------------------------
// ¥ GetDataTag
// ---------------------------------------------------------------------------

void
LControlView::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size			*outDataSize) const
{
	mControlSubPane->GetDataTag(inPartCode, inTag,
						inBufferSize, inBuffer,
						outDataSize);
}


// ---------------------------------------------------------------------------
// ¥ SetTextTraitsID
// ---------------------------------------------------------------------------

void
LControlView::SetTextTraitsID(
	ResIDT		inTextTraitsID)
{
	mControlSubPane->SetTextTraitsID(inTextTraitsID);
}


// ---------------------------------------------------------------------------
// ¥ GetTextTraitsID
// ---------------------------------------------------------------------------

ResIDT
LControlView::GetTextTraitsID() const
{
	return mControlSubPane->GetTextTraitsID();
}


// ---------------------------------------------------------------------------
// ¥ ApplyForeAndBackColors
// ---------------------------------------------------------------------------

void
LControlView::ApplyForeAndBackColors() const
{
	if ( (mControlSubPane != nil)  &&
		 !(mControlSubPane->GetControlImp())->ApplyForeAndBackColors() ) {

		if (mSuperView != nil) {
			mSuperView->ApplyForeAndBackColors();
		}
	}
}


// ---------------------------------------------------------------------------
// ¥ GetForeAndBackColors
// ---------------------------------------------------------------------------

void
LControlView::GetForeAndBackColors(
	RGBColor	*outForeColor,
	RGBColor	*outBackColor) const
{
	if ( !(mControlSubPane->GetControlImp())->
			GetForeAndBackColors(outForeColor, outBackColor) ) {

		if (mSuperView != nil) {
			mSuperView->GetForeAndBackColors(outForeColor, outBackColor);
		}
	}
}


// ---------------------------------------------------------------------------
// ¥ HotSpotResult
// ---------------------------------------------------------------------------

void
LControlView::HotSpotResult(
	SInt16	/* inHotSpot */)
{
}


#if PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
// ¥ DoDrawEvent
// ---------------------------------------------------------------------------
// The subPane's implementation class has received a draw event. Do it. Then
// explicitly draw all of this object's subpanes (excluding the control subpane
// which has just been drawn).

OSStatus
LControlView::DoDrawEvent (
	EventHandlerCallRef				inCallRef,
	EventRef						inEventRef ) 
{
	OSStatus						status = ::CallNextEventHandler(inCallRef, inEventRef);
	
	TArrayIterator<LPane*>			iter(mSubPanes);
	LPane *							theSub;
	while (iter.Next(theSub)) {
		if (theSub != mControlSubPane) {
			theSub->Draw(nil);
		}		
	}
	return status;
}
#endif

PP_End_Namespace_PowerPlant
