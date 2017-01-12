// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMPopupButtonImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMPopupButtonImp.h>
#include <LPopupButton.h>
#include <UEnvironment.h>

#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMPopupButtonImp					Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMPopupButtonImp::LAMPopupButtonImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
#if not PP_Uses_Carbon_Events
		// This class requires the special empty MENU resource in order
		// to prevent the Toolbox from mucking with the real MenuHandle.
		// Signal and throw if we can't find this menu.

	MenuHandle	emptyMenuH = LMenuController::GetEmptyMenuH();

	if (emptyMenuH == nil) {
		SignalStringLiteral_("LAMPopupButtonImp requires MENU 12345. "
							 "Add EmptyMenu.rsrc to your project.");
		Throw_(resNotFound);
	}
#endif

	mMenuHandleTag = kControlPopupButtonMenuHandleTag;
}


// ---------------------------------------------------------------------------
//	¥ ~LAMPopupButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMPopupButtonImp::~LAMPopupButtonImp()
{
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ MakeMacControl											   [protected]
// ---------------------------------------------------------------------------

void
LAMPopupButtonImp::MakeMacControl(
	ConstStringPtr	inTitle,
	SInt32			inRefCon)
{
		// [Workaround] Appearance Manager bug
		//
		// Title of popup menu does not use the font set via
		// ::SetControlFontStyle() on AM 1.1.1 and earlier.
		// As a workaround, we create popup buttons with the
		// kControlUsesOwningWindowsFontVariant option and set
		// the font for the window port before drawing.

	if (mTextTraitsID != 0) {
		mControlKind += kControlUsesOwningWindowsFontVariant;
	}
	
	LAMControlImp::MakeMacControl(inTitle, inRefCon);
}


// ---------------------------------------------------------------------------
//	¥ FindHotSpot													  [public]
// ---------------------------------------------------------------------------

SInt16
LAMPopupButtonImp::FindHotSpot(
	Point		/* inPoint */) const
{
	return kControlMenuPart;
}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot													  [public]
// ---------------------------------------------------------------------------

Boolean
LAMPopupButtonImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{
#if not PP_Uses_Carbon_Events
	StPopupMenuSetter	setMenu(mControlPane, GetMacMenuH());
#endif

	return LAMControlImp::TrackHotSpot(inHotSpot, inPoint, inModifiers);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMPopupButtonImp::DrawSelf()
{
#if not PP_Uses_Carbon_Events
	StPopupMenuSetter	setMenu(mControlPane, GetMacMenuH());
#endif

	LAMControlImp::DrawSelf();
}

#pragma mark -

#if not PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LAMPopupButtonImp::PostSetValue()
{
		// If the MenuHandle stored in the Toolbox Control is our
		// special empty menu, then setting the control value will
		// draw an empty selection. So we need to explicitly draw
		// the popup box to show the proper menu item.

	MenuHandle	menuH = nil;
	::GetControlData(mMacControlH, 0, mMenuHandleTag,
						sizeof(MenuHandle), (Ptr) &menuH, nil);

	if (menuH == LMenuController::GetEmptyMenuH()) {
		mControlPane->Draw(nil);
	}
}
#endif


// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMPopupButtonImp::SetDataTag(
	SInt16			/* inPartCode */,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	if (inTag == mMenuHandleTag) {

		MenuHandle	menuH = *(MenuHandle*) inDataPtr;

#if PP_Uses_Carbon_Events
			// AM version 1.0.2 and earlier don't support setting the
			// MenuHandle to nil via the data tag. As a workaround, we
			// install a special menu that has no items.

		if (menuH == nil) {
			menuH = LMenuController::GetEmptyMenuH();
		}
#endif
		::SetControlData(mMacControlH, kControlNoPart, inTag,
							inDataSize, (Ptr) &menuH);
	}
}


#if not PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ GetMacMenuH												   [protected]
// ---------------------------------------------------------------------------

MenuHandle
LAMPopupButtonImp::GetMacMenuH() const
{
	MenuHandle		menuH = nil;

	LPopupButton*	popup = dynamic_cast<LPopupButton*>(mControlPane);

	if (popup != nil) {
		menuH = popup->GetMacMenuH();
	}

	return menuH;
}
#endif


// ---------------------------------------------------------------------------
//	¥ FocusImp														  [public]
// ---------------------------------------------------------------------------

void
LAMPopupButtonImp::FocusImp()
{
		// [Workaround] Appearance Manager bug
		//
		// Title of popup menu does not use the font set via
		// ::SetControlFontStyle() on AM 1.1.1 and earlier.
		// As a workaround, we create popup buttons with the
		// kControlUsesOwningWindowsFontVariant option and set
		// the font for the window port before drawing.

	ApplyTextTraits();
	
	LAMControlImp::FocusImp();
}


// ---------------------------------------------------------------------------
//	¥ AlignControlRect											   [protected]
// ---------------------------------------------------------------------------
//	Align bounds of Toolbox control to Pane's Frame

void
LAMPopupButtonImp::AlignControlRect()
{
	LAMControlImp::AlignControlRect();
	
		// [Workaround] Appearance Manager bug (Mac OS 8 and 9)
		//
		// When a popup menu is inside a scrolling view and gets scrolled,
		// clicking on the popup at its new location has no effect until
		// the popup redraws. Other controls do not behave this way. My
		// guess is that it has comething to do with the popup caching
		// some data regarding the popup menu.
		//
		// By experimenting, I discovered that calling ActivateControl()
		// restores proper clicking behavior. To prevent unnecessary
		// drawing, we make the call only if the popup is visible,
		// active, and within its owner window, and we make an empty clipping
		// region so that nothing actually draws on screen.

#if !TARGET_API_MAC_OSX
	if ( ::IsControlVisible(mMacControlH) &&
		 ::IsControlActive(mMacControlH) &&
		 (::GetControlOwner(mMacControlH) == mControlPane->GetMacWindow()) ) {
		 
		mControlPane->FocusDraw();
		StClipRgnState	dontDraw(nil);
		::ActivateControl(mMacControlH);
	}
#endif
}


PP_End_Namespace_PowerPlant
