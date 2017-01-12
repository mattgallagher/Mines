// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPushButtonImp.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPushButtonImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LGAControlImp.h>

#include <LControl.h>
#include <LStream.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <LString.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Constants

const SInt16	pushButton_BevelWidth = 3;
const SInt16	pushButton_TextOffset = 9;


// ---------------------------------------------------------------------------
//	¥ LGAPushButtonImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAPushButtonImp::LGAPushButtonImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAPushButtonImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPushButtonImp::LGAPushButtonImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mDefault	= false;
	mHasIcon	= false;
	mIconOnLeft = true;
	mTransform	= kTransformNone;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAPushButtonImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPushButtonImp::~LGAPushButtonImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializor		  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializor [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
	mControlPane  = inControlPane;
	mTitle		  = inTitle;
	mTextTraitsID = inTextTraitsID;

	mHasIcon	  = (inControlKind == kControlPushButLeftIconProc) ||
					(inControlKind == kControlPushButRightIconProc);

	mIconOnLeft   = (inControlKind == kControlPushButLeftIconProc);

										// If we have an icon then we get the resource
										// ID from the max field
	if (HasIcon()) {
		SetCIconResourceID ( (SInt16) mControlPane->GetMaxValue ());

										// Make sure we have the transform correctly
										// setup based on the state of the icon
		mTransform = IsEnabled () ? kTransformDisabled : kTransformNone;

	} else {

		mTransform = kTransformNone;
	}
										// The default flag is set to false by default
										// we will use it in a subclass
	mDefault = false;
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGAPushButtonImp::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mTitle, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::CalcTitleRect(
	Rect&	outRect)
{
	StTextState			theTextState;
	StColorPenState		thePenState;
	SInt16				bevelWidth = 2;

									// Get some loal variables setup including the
									// rect for the title
	ResIDT	textTID = GetTextTraitsID ();

									// Get the port setup with the text traits
	UTextTraits::SetPortTextTraits ( textTID );

									// Figure out the height of the text for the
									// selected font
	FontInfo fInfo;
	GetFontInfo ( &fInfo );
	SInt16		textHeight   = (SInt16) (fInfo.ascent + fInfo.descent);
	SInt16		textBaseline = fInfo.ascent;

									// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );
									// If the button is currently a default button
									// then we need to make sure we inset the title
									// rect enough to accomodate the default ring
	SInt16	horizInset = bevelWidth;
	if ( IsDefaultButton ()) {
		horizInset += 3;
	}
	::MacInsetRect ( &outRect, horizInset, bevelWidth );

									// Figure out the height of the rect
	SInt16		height = UGraphicUtils::RectHeight ( outRect );

									// Adjust the title rect to match the height of
									// the font
	outRect.top   += (( height - textBaseline) / 2) - 2;
	outRect.bottom = (SInt16) (outRect.top + textHeight);

									// If there is an icon present then we need to
									// modify the width of the title based on the
									// location of the icon
	if ( HasIcon ()) {
		Rect	iconRect;
		CalcLocalCIconRect ( iconRect );
		SDimension16	iconSize = GetCIconSize ();
		SInt16	titleWidth = CalcTitleWidth ();
		if ( HasIconOnLeft ()) {
			outRect.left = (SInt16) (iconRect.right + pushButton_TextOffset);
			if ( (outRect.left + titleWidth) <= outRect.right ) {
				outRect.right = (SInt16) (outRect.left + titleWidth);
			}

		} else {

			outRect.right = (SInt16) (iconRect.left - pushButton_TextOffset);
			if ((outRect.right - titleWidth) > outRect.left ) {
				outRect.left = (SInt16) (outRect.right - titleWidth);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalCIconRect											  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::CalcLocalCIconRect(
	Rect&	outRect)
{
													// We always start by getting the icon location
													// recalculated
	Point	iconLocation;
	CalcCIconLocation ( iconLocation );

													// If we have an icon suite handle we figure out
													// the size of the rectangle it would occupy
	if ( mCIconH != nil ) {
		SDimension16	iconSize = GetCIconSize ();

													// Setup the rectangle based on the location and
													// size of the icon
		outRect.top    = iconLocation.v;
		outRect.left   = iconLocation.h;
		outRect.bottom = (SInt16) (iconLocation.v + iconSize.height);
		outRect.right  = (SInt16) (outRect.left + iconSize.width);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetValue(
	SInt32		inValue)
{
	SetPushedState (inValue != Button_Off);
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetTextTraitsID(
	ResIDT	 inTextTraitsID)
{
	if ( mTextTraitsID != inTextTraitsID ) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetPushedState(
	Boolean		inPushedState)
{
													// We only change the state if we have to
	if ( mPushed != inPushedState ) {
		mPushed = inPushedState;
													// Things have changed so we need to make sure
													// that the transform is in synch with the
													// change
		if ( HasIcon ()) {
			mTransform = inPushedState && IsEnabled () ? kTransformSelected :
										(IsEnabled () ? kTransformNone :  kTransformDisabled);
		}

		// Get the button redrawn so the changed state can take effect
		if ( IsVisible () && mControlPane->FocusDraw ()) {
			Rect	localFrame;
			CalcLocalFrameRect ( localFrame );

			StColorDrawLoop	theLoop(localFrame);
			SInt16			depth;
			while (theLoop.NextDepth(depth)) {
				DrawPushButton(depth);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultButton												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetDefaultButton(
	Boolean		inDefault,
	Boolean		inRedraw)
{
													// Only change the default setting if it is
													// different
	if ( mDefault != inDefault ) {
		mDefault = inDefault;

													// Get the size and location of the host pane
		SDimension16	buttonSize;
		mControlPane->GetFrameSize ( buttonSize );

													// Adjust the size and location of the button
													// depending on whether it is the default or not
		if ( inDefault ) {
													// When we become the default button we need to
													// adjust the location by 3 pixels up and left
			mControlPane->MoveBy ( -3, -3, false );

													// When we become the default button we need to
													// grow by 6 pixels
			mControlPane->ResizeFrameTo ( (SInt16) (buttonSize.width + 6),
										  (SInt16) (buttonSize.height + 6),
										  inRedraw );

		} else {
													// When we are no longer the default button we
													// need to shrink by 6 pixels
			mControlPane->ResizeFrameTo ( (SInt16) (buttonSize.width - 6),
										  (SInt16) (buttonSize.height - 6),
										  inRedraw );

													// When we are no longer the default button we
													// need to adjust the location by 3 pixels down
													// and right
			mControlPane->MoveBy ( 3, 3, false );
		}
													// Now we redraw everything, if needed
		if ( inRedraw ) {
			mControlPane->Draw ( nil );
			mControlPane->DontRefresh();
		}
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::EnableSelf()
{
													// Get the transform changed
	if ( HasIcon ()) {
		mTransform = kTransformNone;
	}
													// Get things redrawn so that we can see the
													// state change
	mControlPane->Draw ( nil );
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------
//

void
LGAPushButtonImp::DisableSelf()
{
													// Get the transform changed
	if ( HasIcon ()) {
		mTransform = kTransformDisabled;
	}
													// Get things redrawn so that we can see the
													// state change
	mControlPane->Draw ( nil );
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::ActivateSelf()
{
													// Get everything redrawn so the changes take
													// effect
	if ( IsActive ()) {
													// Adjust the transform and then refresh
		if ( HasIcon ()) {
			mTransform = kTransformNone;
		}

		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DeactivateSelf()
{
													// Get everything redrawn so the changes take
													// effect
	if ( !IsActive ()) {
													// Adjust the transform and then refresh
		if ( HasIcon ()) {
			mTransform = kTransformDisabled;
		}

		Refresh ();
	}
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawSelf()
{
													// Draw the button
	DrawPushButton (mControlPane->GetBitDepth());
													// If this is a default button then get the
													// outline drawn around the button in the
													// appropriate bit depth
	if ( mDefault ) {
													// Get the frame for the control
		Rect	localFrame;
		CalcLocalFrameRect ( localFrame );

		DrawDefaultOutline ( mControlPane->GetBitDepth() );
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPushButton												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawPushButton(
		SInt16	inDepth )
{
	StColorPenState::Normalize();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	// BLACK & WHITE
	if ( inDepth < 4 ) {
		// Get the control drawn in its various states
		if ( IsEnabled ()) {
			if ( IsPushed ()) {
				DrawButtonHilitedBW ();
			} else {
				DrawButtonNormalBW ();
			}

		} else {
			DrawButtonDimmedBW ();
		}

	} else {		// COLOR
												// Get the control drawn in its various states
		if ( IsEnabled () && IsActive ()) {
			if ( IsPushed ()) {
				DrawButtonHilitedColor ();
			} else {
				DrawButtonNormalColor ();
			}

		} else {
			DrawButtonDimmedColor ();
		}
	}
												// If we have an icon then we get it drawn last
	if ( HasIcon ()) {
		DrawPushButtonIcon (inDepth);
	}
												// Call our title drawing routine to get the
												// title drawn
	DrawButtonTitle ( inDepth );
}


// ---------------------------------------------------------------------------
//	¥ DrawButtonTitle												  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonTitle(
	SInt16	inDepth)
{
	StColorPenState::Normalize();

	ResIDT	textTID = GetTextTraitsID();
	UTextTraits::SetPortTextTraits(textTID);

	RGBColor	textColor;			// Get text color set by Text Traits
	::GetForeColor(&textColor);
	::TextMode ( srcOr );

	Rect	titleRect;
	CalcTitleRect(titleRect);

	if (IsPushed()) {				// Adjust color based on button state
		textColor = Color_White;

	} else if (inDepth < 4) {
		textColor = Color_Black;

	} else if (!IsEnabled() || !IsActive()) {
		UTextDrawing::DimTextColor(textColor);
	}

	::RGBForeColor(&textColor);

	UTextDrawing::DrawTruncatedWithJust(mTitle, titleRect, teCenter, true);
}


// ---------------------------------------------------------------------------
//	¥ DrawPushButtonIcon											  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawPushButtonIcon(
	SInt16	inDepth)
{
	Rect iconRect;
	CalcLocalCIconRect(iconRect);

	StClipRgnState		saveClip;
	ClipToPushButtonFace();				// Don't draw over edges of button

	UGraphicUtils::DrawCIconGraphic(mCIconH, iconRect, mTransform, inDepth);
}


// ---------------------------------------------------------------------------
//	¥ DrawDefaultOutline											  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawDefaultOutline (
		SInt16	inDepth )
{
	StColorPenState::Normalize ();

	RGBColor	tempColor;

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

													// Handle the drawing based on the screen depth
													// passed in
													// BLACK & WHITE
	RGBColor	colorArray[5];
	if ( inDepth < 4 ) {
													// Draw the frame for the outline around the
													// outside
		colorArray[0] = Color_Black;
		colorArray[1] = Color_Black;
		colorArray[2] = Color_Black;
		colorArray[3] = Color_Black;
		colorArray[4] = Color_Black;
		UGraphicUtils::AngledCornerFrame ( localFrame, colorArray, 3 );
		::MacInsetRect ( &localFrame, 1, 1 );
		UGraphicUtils::AngledCornerFrame ( localFrame, colorArray, 2 );

													// Now we also need to paint a couple of pixels
													// in the corners to make things look right
													// TOP LEFT
		tempColor = Color_Black;
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.left + 2),
								(SInt16) (localFrame.top + 1),
								tempColor );
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.left + 1),
								(SInt16) (localFrame.top + 2),
								tempColor );

													// TOP RIGHT
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - 3),
								(SInt16) (localFrame.top + 1),
								tempColor );
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - 2),
								(SInt16) (localFrame.top + 2),
								tempColor );

													// BOTTOM RIGHT
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - 3),
								(SInt16) (localFrame.bottom - 2),
								tempColor );
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - 2),
								(SInt16) (localFrame.bottom - 3),
								tempColor );

													// BOTTOM LEFT
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.left + 2),
								(SInt16) (localFrame.bottom - 2),
								tempColor );
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.left + 1),
								(SInt16) (localFrame.bottom - 3),
								tempColor );

	} else if ( inDepth >= 4 ) {			// COLOR

		if ( IsEnabled () && IsActive ()) {

													// Draw the frame for the outline around the
													// outside
			colorArray[0] = Color_Black;
			colorArray[1] = UGAColorRamp::GetColor(12);
			colorArray[2] = Color_Black;
			colorArray[3] = Color_Black;
			colorArray[4] = UGAColorRamp::GetColor(12);
			UGraphicUtils::AngledCornerFrame ( localFrame, colorArray, 3 );

													//	Draw light colored ring
			::MacInsetRect ( &localFrame, 1, 1 );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
			::RGBForeColor ( &tempColor );
			::FrameRoundRect ( &localFrame, 8, 8 );

													// Draw the dark colored ring
			::MacInsetRect ( &localFrame, 1, 1 );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			::RGBForeColor ( &tempColor );
			::FrameRoundRect ( &localFrame, 8, 8 );
			::MacInsetRect ( &localFrame, -2, -2 );

													// Fixup some corner pixels
													// TOP LEFT
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.top + 2),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.top + 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 4),
									(SInt16) (localFrame.top + 3),
									tempColor );

													// TOP RIGHT
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 1),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.top + 2),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 2),
									(SInt16) (localFrame.top + 3),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 2),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.top + 3),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 5),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 4),
									tempColor );

													// BOTTOM RIGHT
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 5),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.bottom - 5),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.bottom - 4),
									tempColor );

													// BOTTOM LEFT
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 1),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 2),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 5),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 4),
									(SInt16) (localFrame.bottom - 4),
									tempColor );

													// Draw bottom left shadows
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
			::RGBForeColor ( &tempColor );
			::MoveTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 2) );
			::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 2) );
			::MoveTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 4) );
			::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.top + 4) );

		} else {

													// Setup the color array
			RGBColor 			colorArray[5];

													// Draw the frame for the outline around the
													// outside
			colorArray[0] = UGAColorRamp::GetColor(7);
			colorArray[1] = UGAColorRamp::GetColor(7);
			colorArray[2] = UGAColorRamp::GetColor(7);
			colorArray[3] = UGAColorRamp::GetColor(7);
			colorArray[4] = UGAColorRamp::GetColor(7);
			UGraphicUtils::AngledCornerFrame ( localFrame, colorArray, 3 );

													//	Fill in the default frame
			::MacInsetRect ( &localFrame, 1, 1 );
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
			::RGBForeColor ( &tempColor );
			::FrameRoundRect ( &localFrame, 8, 8 );
			::MacInsetRect ( &localFrame, 1, 1 );
			::FrameRoundRect ( &localFrame, 8, 8 );
			::MacInsetRect ( &localFrame, -2, -2 );

													// Fixup some corner pixels
													// TOP LEFT
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.top + 2),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.top + 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 4),
									(SInt16) (localFrame.top + 3),
									tempColor );

													// TOP RIGHT
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 1),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.top + 2),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 2),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 2),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 5),
									(SInt16) (localFrame.top + 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.top + 4),
									tempColor );

													// BOTTOM RIGHT
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 5),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.bottom - 5),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 4),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 3),
									(SInt16) (localFrame.bottom - 4),
									tempColor );

													// BOTTOM LEFT
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 1),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 2),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 2),
									(SInt16) (localFrame.bottom - 4),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 3),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 3),
									(SInt16) (localFrame.bottom - 5),
									tempColor );
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.left + 4),
									(SInt16) (localFrame.bottom - 4),
									tempColor );

		}
	}
}


#pragma mark -
#pragma mark === DATA HANDLING

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
								// If default tag is being set for the button
								// then we call our function for turning the
								// default ring on or off
	if ( inTag == kControlPushButtonDefaultTag ) {
		Boolean defaultOn = *(Boolean *) inDataPtr;
		SetDefaultButton ( defaultOn, true );
		
	} else {
		LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
	}
}



// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	if ( inTag == kControlPushButtonDefaultTag ) {
							// We return whether the button currently has a
							// default ring or not
		if ( outDataSize != nil ) {
			*outDataSize = sizeof ( mDefault );
		}

		*(Boolean *)inBuffer = mDefault;
		
	} else {
		LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
									inBuffer, outDataSize);
	
	}
}

// ---------------------------------------------------------------------------
//	¥ DrawButtonNormalBW										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonNormalBW()
{
	StColorPenState::Normalize();

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}
												// Draw the frame for the control outside of
												// everything else the frame is drawn in black
	::FrameRoundRect ( &localFrame, 8, 8 );

												// Before we draw anything we need to erase the
												// control in case we we had previously been hilited
	::MacInsetRect ( &localFrame, 1, 1 );
	::EraseRoundRect ( &localFrame, 4, 4 );

}


// ---------------------------------------------------------------------------
//	¥ DrawButtonHilitedBW										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonHilitedBW()
{
	StColorPenState::Normalize();

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}

												// Draw the frame for the control outside of
												// everything else the frame is drawn in black
	::FrameRoundRect ( &localFrame, 8, 8 );

												// Now we paint the inside of the button black as
												// well
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRoundRect ( &localFrame, 4, 4 );

}


// ---------------------------------------------------------------------------
//	¥ DrawButtonDimmedBW										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonDimmedBW()
{
	StColorPenState::Normalize();
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}
												// Draw the frame for the control outside of
												// everything else the frame is drawn in black
	::FrameRoundRect ( &localFrame, 8, 8 );
												// Before we draw anything we need to erase the
												// control in case we had previously been hilited
	::MacInsetRect ( &localFrame, 1, 1 );
	::EraseRoundRect ( &localFrame, 4, 4 );

}


// ---------------------------------------------------------------------------
//	¥ DrawButtonNormalColor										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonNormalColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}

												// Draw the frame for the control outside of
												// everything else
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

												// Paint the face of the control first
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
	::RGBForeColor ( &tempColor );
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

												// LIGHT EDGES
												// Start by rendering the bevelled edges of the
												// sides facing the light source
	::RGBForeColor ( &Color_White );
	UGraphicUtils::TopLeftSide ( 	localFrame,
						2, 					//	TOP
						2, 					//	LEFT
						3, 					// BOTTOM
						3 );					// RIGHT
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 3),
							(SInt16) (localFrame.top + 3),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 1),
							(SInt16) (localFrame.top + 2),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 2),
							(SInt16) (localFrame.top + 1),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 1),
							(SInt16) (localFrame.bottom - 3),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 2),
							(SInt16) (localFrame.bottom - 2),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.top + 1),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 2),
							(SInt16) (localFrame.top + 2),
							tempColor );

												// SHADOW EDGES
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	::RGBForeColor ( &tempColor );
	::MoveTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 2) );
	::MoveTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.top + 3) );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame,
							3, 				//	TOP
							3, 				//	LEFT
							2, 				// BOTTOM
							2 );				// RIGHT
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.bottom - 3),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 4),
							(SInt16) (localFrame.bottom - 4),
							tempColor );

}


// ---------------------------------------------------------------------------
//	¥ DrawButtonHilitedColor									   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonHilitedColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}
												// Draw the frame for the control outside of
												// everything else
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

												// Paint the face of the control first
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
	::RGBForeColor ( &tempColor );
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

												// LIGHT EDGES
												// Start by rendering the bevelled edges of the
												// sides facing the light source
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
	::RGBForeColor ( &tempColor );
	::MoveTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.top + 2) );
	::MoveTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.top + 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.top + 1) );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide (
							localFrame,
							2, 				//	TOP
							2, 				//	LEFT
							3, 				// BOTTOM
							3 );				// RIGHT
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 3),
							(SInt16) (localFrame.top + 3),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 2),
							(SInt16) (localFrame.top + 2),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.left + 2),
							(SInt16) (localFrame.bottom - 2),
							tempColor );
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 2),
							(SInt16) (localFrame.top + 2),
							tempColor );

												// SHADOW EDGES
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	::RGBForeColor ( &tempColor );
	::MoveTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 2) );
	::MoveTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.top + 3) );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide (
							localFrame,
							3, 				//	TOP
							3, 				//	LEFT
							2, 				// BOTTOM
							2 );				// RIGHT
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.bottom - 3),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 4),
							(SInt16) (localFrame.bottom - 4),
							tempColor );

}


// ---------------------------------------------------------------------------
//	¥ DrawButtonDimmedColor										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DrawButtonDimmedColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
												// If the button is set to be a default button then
												// we need to make sure we inset the rectangle
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &localFrame, 3, 3 );
	}
												// Draw the frame for the control outside of
												// everything else
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	::RGBForeColor ( &tempColor );
	::FrameRoundRect ( &localFrame, 8, 8 );

												// Paint the face of the control
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
	::RGBForeColor ( &tempColor );
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ PointInHotSpot											   [protected]
// ---------------------------------------------------------------------------

Boolean
LGAPushButtonImp::PointInHotSpot(
	Point 	inPoint,
	SInt16	/* inHotSpot */) const
{

	Rect	frameRect;
	CalcLocalFrameRect ( frameRect );

												// Now if the button is a default button we will
												// inset the rect so that we only track the actual
												// button portion of the button
	if ( IsDefaultButton ()) {
		::MacInsetRect ( &frameRect, 3, 3 );
	}
												// We only return true if the mouse is in the
												// actual button portion of the control not in
												// the default ring
	return ::MacPtInRect ( inPoint, &frameRect );
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::DoneTracking(
	SInt16 	/* inHotSpot*/,
	Boolean	/* inGoodTrack */)
{
	SetPushedState ( false );
}



#pragma mark -
#pragma mark === MISCELLANEOUS

// ---------------------------------------------------------------------------
//	¥ CalcCIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::CalcCIconLocation(
	Point&	outIconLocation)
{
	Rect		localFrame;
	SInt16		width, height;
	SInt16		bevelWidth = pushButton_BevelWidth;

													// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	outIconLocation.h = localFrame.left;
	outIconLocation.v = localFrame.top;

													// Setup the width and height values
	width  = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SDimension16	iconSize = GetCIconSize ();
	SInt16	iconHeight = iconSize.height;
	SInt16	iconWidth = iconSize.width;
	SInt16	titleWidth = CalcTitleWidth ();

													// If the title width is greater than or equal
													// to the width of the button less the width of
													// the icon we need to adjust the title width
	if ( titleWidth > (width - (iconWidth + pushButton_TextOffset))) {
		SInt16	edgeOffset = (SInt16) (bevelWidth + 1 + (IsDefaultButton() ? 4 : 0));
		titleWidth = (SInt16) ((width - (iconWidth + pushButton_TextOffset + (edgeOffset * 2))));
	}
													// Now we calculate the location of the icon
													// based on the graphic alignment

	SInt16	textOffset = 0;							// Spacing between icon and title if title
	if (mTitle.Length() > 0) {						// isn't empty
		textOffset = pushButton_TextOffset;
	}

	outIconLocation.h += (width - (iconWidth + titleWidth + textOffset)) / 2;
	outIconLocation.v += (height - iconHeight) / 2;
													// If the icon has been specified to be on the
													// left side of the title then we need to make
													// sure that the location is setup for that
													// position
	if ( !HasIconOnLeft ()) {
		outIconLocation.h += titleWidth + textOffset;
	}
}


// ---------------------------------------------------------------------------
//	¥ ClipToPushButtonFace										   [protected]
// ---------------------------------------------------------------------------

void
LGAPushButtonImp::ClipToPushButtonFace()
{
		// What we are going to do here is find the
		// intersection between the face of the button
		// and the current clipping region

	StRegion		clipRgn;
	::GetClip ( clipRgn );

	Rect 	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, 3, 3 );
	clipRgn &= localFrame;
								// Now set the clipping to that intersection
	::SetClip ( clipRgn );
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleWidth											   [protected]
// ---------------------------------------------------------------------------

SInt16
LGAPushButtonImp::CalcTitleWidth()
{
	StTextState		theTextState;

	UTextTraits::SetPortTextTraits(GetTextTraitsID());

	return ::StringWidth(mTitle);
}


PP_End_Namespace_PowerPlant
