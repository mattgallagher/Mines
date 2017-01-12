// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPopupGroupBox.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPopupGroupBox.h>
#include <LControlImp.h>
#include <LStream.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPopupGroupBox						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPopupGroupBox::LPopupGroupBox(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LControlView(inStream),
	  LMenuController(nil)
{
	SControlInfo	cInfo;
	*inStream >> cInfo.valueMessage;
	*inStream >> cInfo.value;
	*inStream >> cInfo.minValue;
	*inStream >> cInfo.maxValue;

		// Always pass MENU_Unspecified so that we can maintain
		// the MenuHandle ourselves

	MakeControlSubPane(inImpID, cInfo.valueMessage, cInfo.value,
					MENU_Unspecified, cInfo.maxValue, inStream);

	mControl = mControlSubPane;

	SInt16	initialMenuItem;
	*inStream >> initialMenuItem;

										// Min Value holds the Menu ID
	InitPopupGroupBox((SInt16) cInfo.minValue, initialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LPopupGroupBox						Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPopupGroupBox::LPopupGroupBox(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	MessageT			inValueMessage,
	SInt16				inTitleOptions,
	ResIDT				inMENUid,
	SInt16				inTitleWidth,
	Boolean				inPrimary,
	ResIDT				inTextTraitsID,
	ConstStringPtr		inTitle,
	SInt16				inInitialMenuItem,
	ClassIDT			inImpID)

	: LControlView(inPaneInfo, inViewInfo, inImpID,
						inPrimary ? kControlGroupBoxPopupButtonProc
								  : kControlGroupBoxSecondaryPopupButtonProc,
						inTitle, inTextTraitsID, inValueMessage,
						inTitleOptions, MENU_Unspecified, inTitleWidth),
	  LMenuController(mControlSubPane)
{
	InitPopupGroupBox(inMENUid, inInitialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ InitPopupGroupBox						Initializer				  [public]
// ---------------------------------------------------------------------------

void
LPopupGroupBox::InitPopupGroupBox(
	ResIDT		inMenuID,
	SInt16		inInitialMenuItem)
{
	SetMenuID(inMenuID);

#if PP_Uses_Carbon_Events
	mControlSubPane->SetDataTag(kControlNoPart, kControlGroupBoxMenuHandleTag, sizeof(MenuHandle*), (Ptr) &mMenuH);
#endif

	mControlSubPane->SetValue(inInitialMenuItem);
}


// ---------------------------------------------------------------------------
//	¥ LPopupGroupBox						Destructor				  [public]
// ---------------------------------------------------------------------------

LPopupGroupBox::~LPopupGroupBox()
{
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ SetMacMenuH													  [public]
// ---------------------------------------------------------------------------
//	Specify the MenuHandle for the Popup Menu

void
LPopupGroupBox::SetMacMenuH(
	MenuHandle	inMenuH,
	bool		inOwnsMenu)
{
	if (inMenuH != mMenuH) {
		mControlSubPane->SetDataTag(0, dataTag_MenuHandle,
									sizeof(MenuHandle), &inMenuH);
		mControlSubPane->SetDataTag(kControlNoPart, kControlGroupBoxMenuHandleTag, 
									sizeof(MenuHandle*), (Ptr)&inMenuH);

		LMenuController::SetMacMenuH(inMenuH, inOwnsMenu);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------
//	Normally, you should use the specific setter functions rather than
//	this function.

void
LPopupGroupBox::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {

		case kControlGroupBoxMenuHandleTag:		// GroupBox does NOT own Menu
			SetMacMenuH( *(MenuHandle*) inDataPtr, false);
			break;

		case kControlPopupButtonMenuIDTag:
			SetMenuID( *(ResIDT*) inDataPtr);
			break;

		default:
			mControlSubPane->SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------
//	Normally, you should use the specific getter functions rather than
//	this function.

void
LPopupGroupBox::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	switch (inTag) {

		case kControlGroupBoxMenuHandleTag:
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
			mControlSubPane->GetDataTag(inPartCode, inTag, inBufferSize,
							inBuffer, outDataSize);
			break;
	}
}


PP_End_Namespace_PowerPlant
