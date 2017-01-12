// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMCheckBoxGroupBoxImp.cp	PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================
//	Implementation for an Appearance Manager CheckBoxGroupBox control
//
//	[Workaround] Appearance Manager Bug
//		In AM versions 1.0 to at least 1.1.1, a CheckBoxGroupBox does not
//		properly use the custom color proc mechanism to set its text and
//		background color. It seems to always ask its SuperControl for those
//		settings. Since PowerPlant does not use control embedding, this
//		sometimes results in incorrect colors.
//
//		As a workaround, this implementation creates a CheckBox control and
//		manually draws the group box border using Theme drawing primitives.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMCheckBoxGroupBoxImp.h>
#include <LStream.h>
#include <UAppearance.h>
#include <UDrawingState.h>

#include <Appearance.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const SInt16	inset_Border	= 10;	// Inset in pixels of the check box
										//   from the sides of the group
										//   box border


// ---------------------------------------------------------------------------
//	¥ LAMCheckBoxGroupBoxImp			Stream/Default Constructor	  [public]
// ---------------------------------------------------------------------------
//	Do not access inStream. It may be nil. Read Stream data in Init()

LAMCheckBoxGroupBoxImp::LAMCheckBoxGroupBoxImp(
	LStream*	inStream)

	: LAMControlImp(inStream)
{
	mIsPrimary = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LAMCheckBoxGroupBoxImp				Destructor				  [public]
// ---------------------------------------------------------------------------

LAMCheckBoxGroupBoxImp::~LAMCheckBoxGroupBoxImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LAMCheckBoxGroupBoxImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
		// We create a CheckBox control and store whether this
		// is a primary (or secondary) group box

	mIsPrimary = (inControlKind == kControlGroupBoxCheckBoxProc);
	
	LAMControlImp::Init(inControlPane, kControlCheckBoxProc, inTitle,
							inTextTraitsID, inRefCon);
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf												  [public]
// ---------------------------------------------------------------------------

void
LAMCheckBoxGroupBoxImp::FinishCreateSelf()
{
		// Set the control bounds of the underlying CheckBox control
		
	AlignControlRect();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LAMCheckBoxGroupBoxImp::DrawSelf()
{
	LAMControlImp::DrawSelf();			// Draw the check box
	
		// Set clipping region to exclude the area covered by
		// the check box, adding 1 pixel extra on the sides.
		// The 1 pixel extra is based on observation of how the
		// Appearance Manager draws a CheckBoxGroupBox.
	
	Rect	bounds;
	::GetControlBounds(mMacControlH, &bounds);
	::MacInsetRect(&bounds, -1, 0);
	
	StClipRgnState	saveClip;
	saveClip.ClipToDifference(bounds);
	
		// The top edge of the group box border aligns with the
		// baseline of the check box title text.
	
	Rect	bestRect;					// Get text baseline
	SInt16	baseLineOffset;
	::GetBestControlRect(mMacControlH, &bestRect, &baseLineOffset);
	
	Rect	frame;							
	CalcLocalFrameRect(frame);			// Align top of frame to baseline

	frame.top += (bestRect.bottom - bestRect.top + baseLineOffset) - 1;
	
		// Draw group box border in the proper state and style.
		// The clipping region set above ensures that the border
		// doesn't draw over the check box.
	
	ThemeDrawState	themeState = kThemeStateInactive;
	if (IsActive() and IsEnabled()) {
		themeState = kThemeStateActive;
	}
	
	UAppearance::DrawThemeGroupBorder(&frame, themeState, mIsPrimary);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------
//	Set title of the check box

void
LAMCheckBoxGroupBoxImp::SetDescriptor(
	ConstStringPtr	inDescriptor)
{
		// New title will probably be a different pixel length than the
		// old one. Thus we need to recalculate the best size for the
		// check box so that the border draws properly around it.

	mControlPane->FocusDraw();
	
		// Suppress drawing of the new title. We will invalidate the
		// title area later so that both the check box and theme
		// group border redraw properly.
	{		
		StClipRgnState	saveClip(nil);
		::SetControlTitle(mMacControlH, inDescriptor);
	}
	
		
								// Refresh check box at its current size
	Rect	bounds;
	::GetControlBounds(mMacControlH, &bounds);
	::MacInsetRect(&bounds, -1, 0);
	mControlPane->RefreshRect(bounds);
	
								// Determine size for the check box
								//   based on the new title
	CalcLocalFrameRect(bounds);
	AdjustControlBounds(bounds);
	::SetControlBounds(mMacControlH, &bounds);
	
								// Refresh check box at its new size
	::MacInsetRect(&bounds, -1, 0);
	mControlPane->RefreshRect(bounds);
}	


// ---------------------------------------------------------------------------
//	¥ GetStructureOffsets											  [public]
// ---------------------------------------------------------------------------

OSStatus
LAMCheckBoxGroupBoxImp::GetStructureOffsets(
	Rect&	outOffsets) const
{
		// Our CheckBoxGroupBox does not use the Toolbox control. We
		// don't draw outside our ControlPane's Frame.

	outOffsets = Rect_0000;

	return noErr;
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LAMCheckBoxGroupBoxImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if (inTag == kControlGroupBoxTitleRectTag) {
								// Pass back the bounds of the check box
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
		LAMControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
									inBuffer, outDataSize);
	}
}


// ---------------------------------------------------------------------------
//	¥ AdjustControlBounds										   [protected]
// ---------------------------------------------------------------------------
//	On input, ioBounds is the frame of the entire CheckBoxGroupBox. We need
//	to change this to the bounds for just the check box

void
LAMCheckBoxGroupBoxImp::AdjustControlBounds(
	Rect&	ioBounds)
{
		// We set the control bounds to the frame, then ask the
		// CheckBox control to calculate its "best" bounds, which is
		// based on the size of its title.
		
	Rect	bestRect;
	SInt16	baseLineOffset;
	
	::SetControlBounds(mMacControlH, &ioBounds);
	::GetBestControlRect(mMacControlH, &bestRect, &baseLineOffset);
	
		// Move bounds to allow for the inset from the left side
		// of the border.
	
	::MacOffsetRect(&bestRect, inset_Border, 0);
	
		// Limit width of CheckBox to allow for a minimum
		// width inset on the right side of the border
	
	if (bestRect.right > (ioBounds.right - inset_Border)) {
		bestRect.right = (SInt16) (ioBounds.right - inset_Border);
	}
	
	ioBounds = bestRect;
}


PP_End_Namespace_PowerPlant
