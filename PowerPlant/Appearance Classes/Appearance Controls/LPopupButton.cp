// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPopupButton.cp				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPopupButton.h>
#include <LControlImp.h>
#include <LStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPopupButton							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPopupButton::LPopupButton(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlPane(inStream),
	  LMenuController(this)
{
	SInt16		controlKind;
	ResIDT		textTraitsID;
	Str255		title;
	OSType		menuResType;

	*inStream >> controlKind;
	*inStream >> textTraitsID;
	inStream->ReadPString(title);
	*inStream >> menuResType;

		// Minimum value contains the MENU ID. Always set it to
		// MENU_Unspecified when making the Imp so that we can
		// maintain the MenuHandle ourselves.

	ResIDT	menuID = (ResIDT) mMinValue;
	mMinValue = MENU_Unspecified;
	
		// Check if menu is populated by a resource type.
		// Strip off the flag afterward because we add the
		// items ourselves later on.

	if ((controlKind & kControlPopupUseAddResMenuVariant) == 0) {
		menuResType = 0;		// Resource type not used
	}
	controlKind &= ~kControlPopupUseAddResMenuVariant;

	MakeControlImp(inImpID, controlKind, title, textTraitsID, 0);

	SInt16	initialMenuItem;
	*inStream >> initialMenuItem;

	InitPopupButton(menuID, menuResType, initialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LPopupButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//
//	NOTE: Do not add the kControlPopupUseAddResMenuVariant to the control
//	kind. If you wish to populate the menu from items of a resource type,
//	pass the resource type in the inResTypeMENU parameter. Pass 0 for the
//	type if you don't want resource type items.

LPopupButton::LPopupButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inTitleOptions,
	ResIDT				inMENUid,
	SInt16				inTitleWidth,
	SInt16				inKind,
	ResIDT				inTextTraitsID,
	ConstStringPtr		inTitle,
	OSType				inResTypeMENU,
	SInt16				inInitialMenuItem,
	ClassIDT			inImpID)

	: LControlPane(inPaneInfo, inImpID, inKind, inTitle,
						inTextTraitsID, inValueMessage, inTitleOptions,
						MENU_Unspecified, inTitleWidth, 0),
	  LMenuController(this)
{
	InitPopupButton(inMENUid, inResTypeMENU, inInitialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ InitPopupButton						Private Initializer		 [private]
// ---------------------------------------------------------------------------

void
LPopupButton::InitPopupButton(
	ResIDT		inMenuID,
	OSType		inMenuResType,
	SInt16		inInitialMenuItem)
{
	SetMenuID(inMenuID);

	if ((mMenuH != nil) && (inMenuResType != 0)) {
		AppendResMenu(inMenuResType);	// Use Resource Type to build menu
	}

#if PP_Uses_Carbon_Events
	mControlImp->SetDataTag(kControlNoPart, kControlPopupButtonMenuHandleTag, sizeof(MenuHandle*), (Ptr) &mMenuH);
#endif

	SetValue(inInitialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LPopupButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LPopupButton::~LPopupButton()
{
}


#if PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ SetMacMenuH													  [public]
// ---------------------------------------------------------------------------
//	Override to also associate the menu handle with the control implementation.

void
LPopupButton::SetMacMenuH(
	MenuHandle		inMenuH,
	bool			inOwnsMenu)
{
	LMenuController::SetMacMenuH(inMenuH, inOwnsMenu);
	mControlImp->SetDataTag(kControlNoPart, kControlPopupButtonMenuHandleTag, 
							sizeof(MenuHandle*), (Ptr)&mMenuH);
}
#endif



// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------
//	Normally, you should use the specific setter functions rather than
//	this function.

void
LPopupButton::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {

		case kControlPopupButtonMenuHandleTag:	// Popup does NOT own Menu
			SetMacMenuH( *(MenuHandle*) inDataPtr, false);
			break;

		case kControlPopupButtonMenuIDTag:
			SetMenuID( *(ResIDT*) inDataPtr);
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
LPopupButton::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	switch (inTag) {

		case kControlPopupButtonMenuHandleTag:
			if (inBufferSize >= sizeof(MenuHandle)) {
				* (MenuHandle*) inBuffer = GetMacMenuH();
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(MenuHandle);
			}
			break;

		case kControlPopupButtonMenuIDTag:
			if (inBufferSize >= sizeof(ResIDT)) {
				ResIDT	menuID = GetMenuID();
				* (ResIDT*) inBuffer = menuID;
			}

			if (outDataSize != nil) {
				*outDataSize = sizeof(ResIDT);
			}
			break;

		default:
			LControlPane::GetDataTag(inPartCode, inTag, inBufferSize,
							inBuffer, outDataSize);
			break;
	}
}


PP_End_Namespace_PowerPlant
