// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMPopupGroupBoxImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Implementation for an Appearance Manager PopupGroupBox control
//
//	[Workaround] Appearance Manager Bug
//		In AM versions 1.0 to at least 1.1.1, a PopupGroupBox does not
//		properly use the custom color proc mechanism to set its text and
//		background color. It seems to always ask its SuperControl for those
//		settings. Since PowerPlant does not use control embedding, this
//		sometimes results in incorrect colors.
//
//		As a workaround, this implementation creates a PopupButton control and
//		manually draws the group box border using Theme drawing primitives.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMPopupGroupBoxImp.h>
#include <LPopupGroupBox.h>
#include <LStream.h>
#include <UAppearance.h>
#include <UDrawingState.h>
#include <URegions.h>
#include <UTBAccessors.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt16	inset_Border	= 12;	// Inset in pixels of the popup
										//   button from the sides of the
										//   group box border


// ---------------------------------------------------------------------------
//	¥ LAMPopupGroupBoxImp				Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMPopupGroupBoxImp::LAMPopupGroupBoxImp(
	LStream*	inStream)

	: LAMPopupButtonImp(inStream)
{
	mIsPrimary = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LAMPopupGroupBoxImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LAMPopupGroupBoxImp::~LAMPopupGroupBoxImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
		// We create a PopupButton control and store whether this
		// is a primary (or secondary) group box

	mIsPrimary = (inControlKind == kControlGroupBoxPopupButtonProc);
	
	LAMPopupButtonImp::Init(inControlPane, kControlPopupButtonProc, inTitle,
							inTextTraitsID, inRefCon);
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf												  [public]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::FinishCreateSelf()
{
	LAMPopupButtonImp::FinishCreateSelf();
	AlignControlRect();
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
		// The size of a popup button depends on its title
		// and the contents of its menu, either of which could
		// have changed prior to drawing. Therefore, we call
		// AdjustControlBounds() and set the popup button size.

	Rect	bounds = frame;	
	AdjustControlBounds(bounds);
	::SetControlBounds(mMacControlH, &bounds);
	
									// Draw the popup button control
#if not PP_Uses_Carbon_Events
	StPopupMenuSetter	setMenu(mControlPane, GetMacMenuH());
#endif
	LAMControlImp::DrawSelf();

		// Set clipping region to exclude the area covered by
		// the popup button, adding 3 pixel extra on the sides.
		// The 3 pixel extra is based on observation of how the
		// Appearance Manager draws a PopupGroupBox.
	
	::MacInsetRect(&bounds, -3, 0);
	
	StClipRgnState	saveClip;
	saveClip.ClipToDifference(bounds);
	
		// The top edge of the group box border aligns with the
		// baseline of the popup button title text.
	
	Rect	bestRect;
	SInt16	baseLineOffset;
	::GetBestControlRect(mMacControlH, &bestRect, &baseLineOffset);
	
	frame.top += (bestRect.bottom - bestRect.top + baseLineOffset);
	
		// Draw group box border in the proper state and style.
		// The clipping region set above ensures that the border
		// doesn't draw over the popup button.
	
	ThemeDrawState	themeState = kThemeStateInactive;
	if (IsActive() and IsEnabled()) {
		themeState = kThemeStateActive;
	}
	
	UAppearance::DrawThemeGroupBorder(&frame, themeState, mIsPrimary);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Set title of the group's popup menu

void
LAMPopupGroupBoxImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
		// New title will probably be a different pixel length than the
		// old one. We invalidate the entire header area so that both
		// the popup button and theme group border will redraw properly.

	mControlPane->FocusDraw();
	
		// Suppress drawing of the new title. It will redraw when
		// we handle the subsequent update event.
	{		
		StClipRgnState	saveClip(nil);
		::SetControlTitle(mMacControlH, inDescriptor);
	}
	
		// Invalidate the header area, which extends from the
		// left of the popup button bounds to the right edge
		// of the frame (less the inset).
	
	Rect	bounds;
	::GetControlBounds(mMacControlH, &bounds);
	
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	bounds.right = (SInt16) (frame.right - inset_Border);
	
	mControlPane->RefreshRect(bounds);
}


#if not PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::PostSetValue()
{
		// If the MenuHandle stored in the Toolbox Control is our
		// special emtpy menu, then setting the control value will
		// draw an empty selection. So we need to explicitly draw
		// the popup box to show the proper menu item.

	MenuHandle	menuH = nil;
	::GetControlData(mMacControlH, 0, mMenuHandleTag,
						sizeof(MenuHandle), (Ptr) &menuH, nil);

	if (menuH == LMenuController::GetEmptyMenuH()) {
	
			// Pass in a draw region when calling Draw(). Our
			// ControlPane is a ControlSubPane, which would draw
			// the entire view (not just the popup menu) if we
			// pass nil.
	
		Rect	frame;
		mControlPane->CalcPortFrameRect(frame);
		StRegion	drawRgn(frame);
		
		mControlPane->Draw(drawRgn);
	}
}
#endif


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LAMPopupGroupBoxImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
		// Our PopupGroupBox does not use the Toolbox control. We
		// don't draw outside our ControlPane's Frame.

	outOffsets = Rect_0000;

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if (inTag == kControlGroupBoxTitleRectTag) {
								// Pass back the bounds of the popup button
		Rect	titleRect;
		::GetControlBounds(mMacControlH, &titleRect);
		
		if (inBuffer != nil) {
			if (inBufferSize == sizeof(Rect)) {
				*(Rect *) inBuffer = titleRect;
			} else {
				ThrowOSErr_(errDataSizeMismatch);
			}
		}
		
		if (outDataSize != nil) {
			*outDataSize = sizeof(Rect);
		}
	
	} else {
		LAMPopupButtonImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustControlBounds										   [protected]
// ---------------------------------------------------------------------------

void
LAMPopupGroupBoxImp::AdjustControlBounds(
	Rect&	ioBounds)
{
#if not PP_Uses_Carbon_Events
	StPopupMenuSetter	setMenu(mControlPane, GetMacMenuH());
#endif
	
		// We set the control bounds to the frame, then ask the
		// PopupButton control to calculate its "best" bounds, which is
		// based on the size of its title.
		
	Rect	bestRect;
	SInt16	baseLineOffset;
	
	::SetControlBounds(mMacControlH, &ioBounds);
	::GetBestControlRect(mMacControlH, &bestRect, &baseLineOffset);
	
		// Move bounds to allow for the inset from the left side
		// of the border.
		
		// $$$ Hardwired number
		// Moving down 1 pixel to account for shadow at top edge
		// of popup button. We might be able to calculate this offset
		// by calling GetThemeButtonBackground()
	
	::MacOffsetRect(&bestRect, inset_Border, 1);
	
		// Limit width of PopupButton to allow for a minimum
		// width inset on the right side of the border
	
	if (bestRect.right > (ioBounds.right - inset_Border)) {
		bestRect.right = (SInt16) (ioBounds.right - inset_Border);
	}
	
	ioBounds = bestRect;
}


#if not PP_Uses_Carbon_Events
// ---------------------------------------------------------------------------
//	¥ GetMacMenuH												   [protected]
// ---------------------------------------------------------------------------

MenuHandle
LAMPopupGroupBoxImp::GetMacMenuH() const
{
	MenuHandle	menuH = nil;

	LPopupGroupBox*	box =
				dynamic_cast<LPopupGroupBox*>(mControlPane->GetSuperView());

	if (box != nil) {
		menuH = box->GetMacMenuH();
	}

	return menuH;
}
#endif

PP_End_Namespace_PowerPlant
