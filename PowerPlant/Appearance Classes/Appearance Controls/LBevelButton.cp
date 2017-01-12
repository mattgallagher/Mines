// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBevelButton.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LBevelButton.h>
#include <LControlImp.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <PP_Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LBevelButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LBevelButton::LBevelButton(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream),
	  LMenuController(this)
{
	SInt16		controlKind;
	ResIDT		textTraitsID;
	Str255		title;

	*inStream >> controlKind;
	*inStream >> textTraitsID;
	inStream->ReadPString(title);

		// If there is an attached menu (ID passed in the value field),
		// change the ID to a dummy menu before making the imp so that
		// the Toolbox won't muck with our real MenuHandle.

	ResIDT	menuID = (ResIDT) mValue;
	if (menuID != 0) {
		mValue = MENU_Empty;
	}

	MakeControlImp(inImpID, controlKind, title, textTraitsID);

	if (menuID != 0) {						// Store our copy of the Menu
		SetMenuID(menuID);
	}

	mBehavior = (SInt16) (mMinValue & 0x00000300);	// Mask off other options

	SInt16	initialValue;
	SInt16	titlePlacement;
	SInt16	titleAlignment;
	SInt16	titleOffset;
	SInt16	graphicAlignment;
	Point	graphicOffset;
	Boolean	centerPopupGlyph;

	*inStream >> initialValue;
	*inStream >> titlePlacement;
	*inStream >> titleAlignment;
	*inStream >> titleOffset;
	*inStream >> graphicAlignment;
	*inStream >> graphicOffset;
	*inStream >> centerPopupGlyph;

	InitBevelButton(initialValue, titlePlacement, titleAlignment,
					titleOffset, graphicAlignment, graphicOffset,
					centerPopupGlyph);
}


// ---------------------------------------------------------------------------
//	¥ LBevelButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Constructor for a BevelButton with no menu
//	It can be a Push, Toggle, or Sticky Button.
//
//	See Controls.h for enumerated constants for
//		bevel proc
//		behavior
//		content type
//		title placement
//		title alignment
//		graphic alignment

LBevelButton::LBevelButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt16			inBevelProc,
	SInt16			inBehavior,
	SInt16			inContentType,
	SInt16			inContentResID,
	ResIDT			inTextTraits,
	ConstStringPtr	inTitle,
	SInt16			inInitialValue,
	SInt16			inTitlePlacement,
	SInt16			inTitleAlignment,
	SInt16			inTitleOffset,
	SInt16			inGraphicAlignment,
	Point			inGraphicOffset,
	ClassIDT		inImpID)

	: LControlPane(inPaneInfo, inImpID, inBevelProc,
						inTitle, inTextTraits, inValueMessage,
						0, inBehavior + inContentType, inContentResID),
	  LMenuController(this)
{
	mBehavior = (SInt16) (inBehavior & 0x00000300);

	InitBevelButton(inInitialValue, inTitlePlacement, inTitleAlignment,
					inTitleOffset, inGraphicAlignment, inGraphicOffset,
					false);
}


// ---------------------------------------------------------------------------
//	¥ LBevelButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Constructor for a BevelButton with a Menu
//
//	See Controls.h for enumerated constants for
//		bevel proc
//		menu placement
//		content type
//		title placement
//		title alignment
//		graphic alignment

LBevelButton::LBevelButton(
	const SPaneInfo	&inPaneInfo,
	MessageT		inValueMessage,
	SInt16			inBevelProc,
	ResIDT			inMenuID,
	SInt16			inMenuPlacement,
	SInt16			inContentType,
	SInt16			inContentResID,
	ResIDT			inTextTraits,
	ConstStringPtr	inTitle,
	SInt16			inInitialValue,
	SInt16			inTitlePlacement,
	SInt16			inTitleAlignment,
	SInt16			inTitleOffset,
	SInt16			inGraphicAlignment,
	Point			inGraphicOffset,
	Boolean			inCenterPopupGlyph,
	ClassIDT		inImpID)

	: LControlPane(inPaneInfo, inImpID, (SInt16) (inBevelProc + inMenuPlacement),
						inTitle, inTextTraits, inValueMessage,
						MENU_Empty, inContentType, inContentResID),
	  LMenuController(this)
{
		// We always pass MENU_Empty to the base constructor. This
		// keeps the Toolbox from mucking with our real menu, which
		// we specify here.

	SetMenuID(inMenuID);

	mBehavior = kControlBehaviorPopup;
	InitBevelButton(inInitialValue, inTitlePlacement, inTitleAlignment,
					inTitleOffset, inGraphicAlignment, inGraphicOffset,
					inCenterPopupGlyph);
}


// ---------------------------------------------------------------------------
//	¥ InitBevelButton						Private Initializer		 [private]
// ---------------------------------------------------------------------------

void
LBevelButton::InitBevelButton(
	SInt16		inInitialValue,
	SInt16		inTitlePlacement,
	SInt16		inTitleAlignment,
	SInt16		inTitleOffset,
	SInt16		inGraphicAlignment,
	Point		inGraphicOffset,
	Boolean		inCenterPopupGlyph)
{
	mMinValue = 0;					// Button is either off or on
	mControlImp->SetMinValue(0);

	mMaxValue = 1;
	mControlImp->SetMaxValue(1);

	if (mValue == 0) {
		if (inInitialValue != 0) {	// No Menu attached
			mValue = Button_On;		// Non-zero means button is ON
		}

	} else {						// There is a Menu attached
		mValue = 0;					// Button value is unused
		mBehavior = kControlBehaviorPopup;
		SetCurrentMenuItem(inInitialValue);
	}
	mControlImp->SetValue(mValue);

	SetTextPlacement(inTitlePlacement);
	SetTextAlignment(inTitleAlignment);
	SetTextOffset(inTitleOffset);
	SetGraphicAlignment(inGraphicAlignment);
	SetGraphicOffset(inGraphicOffset);
	SetCenterPopupGlyph(inCenterPopupGlyph);
}


// ---------------------------------------------------------------------------
//	¥ ~LBevelButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LBevelButton::~LBevelButton()
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetValue
// ---------------------------------------------------------------------------

void
LBevelButton::SetValue(
	SInt32	inValue)
{
	if (inValue != mValue) {

		if ( (mBehavior == kControlBehaviorSticky) &&
			 (inValue == Button_On) ) {

				// If turning sticky button on, broadcast message so that the
				// RadioGroup (if present) will turn off the other buttons
				// in the group.

			BroadcastMessage(msg_ControlClicked, (void*) this);
		}

		LControlPane::SetValue(inValue);	// Inherited function changes value
	}
}


void
LBevelButton::SetContentInfo(
	ControlButtonContentInfo	&inInfo)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonContentTag,
							sizeof(ControlButtonContentInfo), &inInfo);
}


void
LBevelButton::GetContentInfo(
	ControlButtonContentInfo	&outInfo) const
{
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonContentTag,
							sizeof(ControlButtonContentInfo), &outInfo);
}


void
LBevelButton::SetTransform(
	IconTransformType	inTransform)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonTransformTag,
							sizeof(IconTransformType), &inTransform);
}


IconTransformType
LBevelButton::GetTransform() const
{
	IconTransformType	transform;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonTransformTag,
							sizeof(IconTransformType), &transform);
	return transform;
}


void
LBevelButton::SetTextAlignment(
	ControlButtonTextAlignment	inAlignment)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonTextAlignTag,
							sizeof(ControlButtonTextAlignment), &inAlignment);
}


ControlButtonTextAlignment
LBevelButton::GetTextAlignment() const
{
	ControlButtonTextAlignment	alignment;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonTextAlignTag,
							sizeof(ControlButtonTextAlignment), &alignment);
	return alignment;
}


void
LBevelButton::SetTextOffset(
	SInt16		inOffset)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonTextOffsetTag,
							sizeof(SInt16), &inOffset);
}


SInt16
LBevelButton::GetTextOffset() const
{
	SInt16	offset;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonTextOffsetTag,
							sizeof(SInt16), &offset);
	return offset;
}


void
LBevelButton::SetGraphicAlignment(
	ControlButtonGraphicAlignment	inAlignment)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonGraphicAlignTag,
							sizeof(ControlButtonGraphicAlignment), &inAlignment);
}


ControlButtonGraphicAlignment
LBevelButton::GetGraphicAlignment() const
{
	ControlButtonGraphicAlignment	alignment;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonGraphicAlignTag,
							sizeof(ControlButtonGraphicAlignment), &alignment);
	return alignment;
}


void
LBevelButton::SetGraphicOffset(
	Point		inOffset)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonGraphicOffsetTag,
							sizeof(Point), &inOffset);
}


void
LBevelButton::GetGraphicOffset(
	Point		&outOffset) const
{
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonGraphicOffsetTag,
							sizeof(Point), &outOffset);
}


void
LBevelButton::SetTextPlacement(
	ControlButtonTextPlacement	inPlacement)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonTextPlaceTag,
							sizeof(ControlButtonTextPlacement), &inPlacement);
}


ControlButtonTextPlacement
LBevelButton::GetTextPlacement() const
{
	ControlButtonTextPlacement	placement;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonTextPlaceTag,
							sizeof(ControlButtonTextPlacement), &placement);
	return placement;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------
//	Set the index for the selected menu item

void
LBevelButton::SetCurrentMenuItem(
	SInt16	inItem)
{
	mMenuChoice = inItem;
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------
//	Return the index of the currently selected menu item

SInt16
LBevelButton::GetCurrentMenuItem() const
{
	return mMenuChoice;
}


// ---------------------------------------------------------------------------
//	¥ SetMenuMinMax													  [public]
// ---------------------------------------------------------------------------
//	Set the min and max values to match the number of items in the Menu

void
LBevelButton::SetMenuMinMax()
{
		// This class doesn't use the control value to store the
		// menu choice. We just need to make sure that the menu choice
		// in within the range of 1 to the number of menu items.

	SInt16	numMenuItems = 0;

	if ( GetMacMenuH() != nil ) {
		numMenuItems = (SInt16) ::CountMenuItems( GetMacMenuH() );
	}

	if (mMenuChoice > numMenuItems) {
		mMenuChoice = numMenuItems;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCenterPopupGlyph											  [public]
// ---------------------------------------------------------------------------

void
LBevelButton::SetCenterPopupGlyph(
	Boolean		inCenter)
{
	mControlImp->SetDataTag(kControlButtonPart,
							kControlBevelButtonCenterPopupGlyphTag,
							sizeof(Boolean), &inCenter);
}


// ---------------------------------------------------------------------------
//	¥ GetCenterPopupGlyph											  [public]
// ---------------------------------------------------------------------------

Boolean
LBevelButton::GetCenterPopupGlyph() const
{
	Boolean	center;
	mControlImp->GetDataTag(kControlButtonPart,
							kControlBevelButtonCenterPopupGlyphTag,
							sizeof(Boolean), &center);
	return center;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------
//	Normally, you should use the specific setter functions rather than
//	this function.

void
LBevelButton::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {

		case kControlBevelButtonMenuHandleTag:	// Button does NOT own Menu
			SetMacMenuH( *(MenuHandle*) inDataPtr, false);
			break;

		case kControlPopupButtonMenuIDTag:
			SetMenuID( *(ResIDT*) inDataPtr);
			break;

		case kControlBevelButtonMenuValueTag:
			SetCurrentMenuItem( *(SInt16*) inDataPtr);
			break;

		default:
			LControlPane::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------
//	Normally, you should use the specific getter functions rather than
//	this function.

void
LBevelButton::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size			*outDataSize) const
{
	switch (inTag) {

		case kControlBevelButtonMenuHandleTag:
			if (inBufferSize >= sizeof(MenuHandle)) {
				* (MenuHandle*) inBuffer = GetMacMenuH();
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(MenuHandle);
			}
			break;

		case kControlPopupButtonMenuIDTag:
			if (inBufferSize >= sizeof(ResIDT)) {
				* (ResIDT*) inBuffer = GetMenuID();
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(ResIDT);
			}
			break;

		case kControlBevelButtonMenuValueTag:
			if (inBufferSize >= sizeof(SInt16)) {
				* (SInt16*) inBuffer = GetCurrentMenuItem();;
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(SInt16);
			}
			break;

		default:
			LControlPane::GetDataTag(inPartCode, inTag, inBufferSize,
							inBuffer, outDataSize);
			break;
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HotSpotResult
// ---------------------------------------------------------------------------
//	Respond to a click in a BevelButton

void
LBevelButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
	switch (mBehavior) {

		case kControlBehaviorPushbutton:
			BroadcastValueMessage();		// Value doesn't change but
			break;							//   broadcast click message

//		case kControlBehaviorToggles:		// Do nothing. Imp toggles value

		case kControlBehaviorSticky:
			SetValue(Button_On);			// Click always turns on
			break;

		case kControlBehaviorPopup: {		// Popup Menu broadcasts message
											//   with selected item number
			SInt16	choice;								
			mControlImp->GetDataTag(0, kControlBevelButtonMenuValueTag,
									sizeof(SInt16), &choice);
			SetCurrentMenuItem(choice);						
									
			SInt32	longChoice = mMenuChoice;
			BroadcastMessage(mValueMessage, &longChoice);
			break;
		}
	}
}


PP_End_Namespace_PowerPlant

