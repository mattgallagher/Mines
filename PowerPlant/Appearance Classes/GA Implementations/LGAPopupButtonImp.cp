// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPopupButtonImp.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPopupButtonImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LGAControlImp.h>

#include <LControl.h>
#include <LPopupButton.h>
#include <LStream.h>
#include <LString.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>
#include <UTBAccessors.h>

#include <LowMem.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant


const SInt16	gsPopup_RightInset 			= 	16;		// Used to position the title rect
const SInt16	gsPopup_ArrowLeftInset		= 	6;		// Used to position the popup arrow
const SInt16	gsPopup_TitleInset			= 	8;		// Apple specification
const SInt16	gsPopup_TitleTrailingInset	= 	3;		// Space between end of title and the
														//		button portion of the popup
const SInt16	gsPopup_LabelOffset 		= 	2;		//	Offset of label from popup
const SInt16	gsPopup_ArrowButtonWidth 	= 	22;		//	Width used in drawing the arrow only
const SInt16	gsPopup_ArrowButtonHeight	= 	18;		//	Height used for drawing arrow only
const SInt16	gsPopup_ArrowHeight			= 	5;		//	Actual height of the arrow
const SInt16	gsPopup_ArrowWidth			= 	9;		//	Actual width of the arrow at widest
const SInt16	gsPopup_MultiArrowHeight	= 	10;		//	Actual height of the two arrow
const SInt16	gsPopup_MultiArrowWidth		= 	7;		//	Actual width of the arrow at widest
const SInt16	gsPopup_TopOffset			=	4;		//	Used to figure out the best height
const SInt16	gsPopup_BottomOffset		=	2;		//		for the popup when calculating
														//		the best rect


// ---------------------------------------------------------------------------
//	¥ LGAPopupButtonImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAPopupButtonImp::LGAPopupButtonImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAPopupButtonImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPopupButtonImp::LGAPopupButtonImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mLabelWidth		= 0;
	mLabelStyle		= 0;
	mFixedWidth		= true;
	mUseWindowFont	= false;
	mPulldown		= true;
	mArrowOnly		= false;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAPopupButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPopupButtonImp::~LGAPopupButtonImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;
	SInt32	macRefCon;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);
	*inStream >> macRefCon;

	Init ( inControlPane, controlKind, title, textTraitID, macRefCon );
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/* inRefCon */)
{
													// Setup our fields
	mControlPane = inControlPane;
													// Get our flags setup from the control kind
	mTitle = inTitle;
	mTextTraitsID = inTextTraitsID;

	mFixedWidth = (inControlKind & kControlPopupFixedWidthVariant) != 0;
	mUseWindowFont = (inControlKind & kControlPopupUseWFontVariant) != 0;

													// Now we need to map some of the values as used
													// by the standard popup to our fields

													// Label text style
	mLabelStyle = (SInt16) ((mControlPane->GetValue () >> 8) & 0xFF);

													// Label justification
	mLabelJust = (SInt16) (mControlPane->GetValue () & 0xFF);
	if ( mLabelJust == 255 )
		mLabelJust = teFlushRight;

													// Get the width of the label, if this value is
													// -1 it means the user wants us to calculate the
													// width needed based on the font and style for
													// the label
	mLabelWidth = (SInt16) mControlPane->GetMaxValue ();
	if ( mLabelWidth == -1 ) {
		mLabelWidth = CalcPopupLabelWidth ();
	}
													// Setup some our private data, this is never
													// going to be used and is an artiffact from
													// days gone by when AGA popups were pulldown
													// instead of popups.  This has been left in here
													// in case it is needed in the future
	mPulldown = false;

													// Figure out if we are going to display just the
													// arrow portion of the popup, we do this by
													// looking at a number of factors: no label plus
													// a width of less than 24 pixels
	SDimension16	frameSize;
	mControlPane->GetFrameSize ( frameSize );

	mArrowOnly = (mLabelWidth == 0) && (frameSize.width < 24);

													// If the popup is set o be arrow only then we
													// will automatically make it the minimum
													// recommended size, if it isn't already
	if ( mArrowOnly ) {
		ResizeForArrowOnly ();
	}
}

void
LGAPopupButtonImp::FinishCreateSelf()
{
	if ( !mFixedWidth && !IsArrowOnly ()) {
		AdjustPopupWidth ();
	}
}

#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGAPopupButtonImp::GetDescriptor (
		Str255	outDescriptor ) const
{
	return LString::CopyPStr ( mTitle, outDescriptor );
}


// ---------------------------------------------------------------------------
//	¥ GetMacMenuH													  [public]
// ---------------------------------------------------------------------------

MenuHandle
LGAPopupButtonImp::GetMacMenuH() const
{
	MenuHandle	menuH = nil;

	LPopupButton	*popup = dynamic_cast<LPopupButton*>(mControlPane);
	if (popup) {
		menuH = popup->GetMacMenuH();
	}

	return menuH;
}


// ---------------------------------------------------------------------------
//	¥ GetCurrentItemTitle											  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::GetCurrentItemTitle(
	Str255	outItemTitle)
{
	MenuHandle	popupMenu = GetMacMenuH();
	if ( popupMenu ) {
		::GetMenuItemText(popupMenu, (SInt16) mControlPane->GetValue(), outItemTitle);
	} else {
		outItemTitle[0] = 0;			// Empty string
	}
}


// ---------------------------------------------------------------------------
//	¥ GetMenuFontSize												  [public]
// ---------------------------------------------------------------------------

SInt16
LGAPopupButtonImp::GetMenuFontSize() const
{
													// Get the font size for the popup so we can
													// figure out the correct mark to apply
	StTextState			theTextState;

	ResIDT	textTID = GetTextTraitsID ();
	TextTraitsH traitsH = UTextTraits::LoadTextTraits ( textTID );
	if ( traitsH ) {
		return (**traitsH).size;
	}
													// Because we should never really get here we
													// simply return zero
	return 0;

}


// ---------------------------------------------------------------------------
//	¥ HasLabel														  [public]
// ---------------------------------------------------------------------------

Boolean
LGAPopupButtonImp::HasLabel () const
{
	Str255	labelString;
	GetDescriptor ( labelString );

	return labelString[0] > 0;

}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetValue ( SInt32 inValue )
{
													// We also get the current item marked we need
													// to do this before the value gets set so that
													// we still have the old item around
	SetupCurrentMenuItem ( GetMacMenuH (), (SInt16) inValue );

}


// ---------------------------------------------------------------------------
//	¥ PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::PostSetValue ()
{
													// Get the control redrawn so that we can see
													// the change
	mControlPane->Draw ( nil );

}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	mControlPane->Refresh ();

}


// ---------------------------------------------------------------------------
//	¥ SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetTextTraitsID(
	ResIDT	 inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetLabelWidth													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetLabelWidth(
	SInt16	inLabelWidth)
{
	if ( mLabelWidth != inLabelWidth ) {
		mLabelWidth = inLabelWidth;

													// If the popup has been setup to have a variable
													// width and the popup is not being dispalyed as
													// only the arrow portion, then we also need to
													// make sure that we get the width adjusted
		if ( !mFixedWidth && !IsArrowOnly ()) {
			AdjustPopupWidth ();
		}
													// Get the whole thing refreshed
		mControlPane->Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetLabelStyle													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetLabelStyle(
	SInt16	inLabelStyle)
{
	if ( mLabelStyle != inLabelStyle ) {
		mLabelStyle = inLabelStyle;
		mControlPane->Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetLabelJust													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetLabelJust(
	SInt16	inLabelJust)
{
	if (mLabelJust != inLabelJust) {
		mLabelJust = inLabelJust;
		mControlPane->Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ GetAllValues													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::GetAllValues(
	SInt32&		outValue,
	SInt32&		outMinValue,
	SInt32&		outMaxValue)
{
	outMinValue = 1;
	outMaxValue = 1;
	MenuHandle	menuH = GetMacMenuH();
	if (menuH != nil) {
		CalcPopupMinMaxValues(menuH, outMinValue, outMaxValue);
		if (outValue > outMaxValue) {
			outValue = 0;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetupCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::SetupCurrentMenuItem(
	MenuHandle	inMenuH,
	SInt16		inCurrentItem)
{
	if (inMenuH != nil) {

		SInt16		oldItem = (SInt16) mControlPane->GetValue();
		
			// Remove mark from old item if it is different
			// from the new current item
		
		if ( (oldItem != inCurrentItem)  &&
			 (oldItem > 0)  &&
			 (oldItem <= mControlPane->GetMaxValue()) ) {

			::SetItemMark(inMenuH, oldItem, 0);
		}
		
			// Mark new current item

		if ( (inCurrentItem > 0)  &&
			 (inCurrentItem <= mControlPane->GetMaxValue()) ) {

			SInt16	mark = checkMark;
			if (GetMenuFontSize() < 12) {
				mark = char_Bullet;
			}

			::SetItemMark(inMenuH, inCurrentItem, mark);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ FindCurrMarkedMenuItem										  [public]
// ---------------------------------------------------------------------------

SInt16
LGAPopupButtonImp::FindCurrMarkedMenuItem(
	MenuHandle	inMenuH)
{
		// Walk the menu looking for the currently marked item so that
		// we can manage the marking of menus independent of the control
		// value which we don't always have access to in the correct sequence

	SInt16	markedItem = 0;

	if (inMenuH != nil) {
		SInt16 itemCount = (SInt16) ::CountMenuItems(inMenuH);

		for (SInt16 index = 1; index < itemCount + 1; index++) {

			SInt16	currMark;
			::GetItemMark(inMenuH, index, &currMark);

			if (currMark != 0) {
				markedItem = index;
				break;
			}
		}
	}

	return markedItem;
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalPopupFrameRect										  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::CalcLocalPopupFrameRect(
	Rect&	outRect) const
{
													// Get the local frame rectangle and adjust its
													// width to the width of the popups label
	CalcLocalFrameRect ( outRect );

	if ( IsArrowOnly ()) {
													// Make sure that we are at least the correct
													// size for drawing the arrow button, if not we
													// will just be rendering in the size specified
													// with unpredictable results
		SDimension16	frameSize;
		mControlPane->GetFrameSize ( frameSize );
		if ( frameSize.width >= gsPopup_ArrowButtonWidth )
			outRect.left = (SInt16) (outRect.right - gsPopup_ArrowButtonWidth);

													// Make sure that we can accomodate the height
													// required for our our drawing, if the popup
													// height has been setup smaller then we just
													// draw to the specified height and hope for the
													// best
		if ( frameSize.height >= gsPopup_ArrowButtonHeight ) {
			outRect.top += ( UGraphicUtils::RectHeight ( outRect ) - gsPopup_ArrowButtonHeight) / 2;
			outRect.bottom = (SInt16) (outRect.top + gsPopup_ArrowButtonHeight);
		}

	} else {
		SInt16	bestWidth, bestHeight, bestBaseline;
		CalcBestControlRect(bestWidth, bestHeight, bestBaseline);

		SInt16	diff = (SInt16) ((outRect.bottom - outRect.top) - bestHeight);

		if (diff > 0) {
			outRect.top += diff / 2;
			outRect.bottom = (SInt16) (outRect.top + bestHeight);
		}

		outRect.left += GetLabelWidth ();
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------
//
//	This method is used to calculate the title rect for the currently selected item in
// 	the popup, this title is drawn inside the popup

void
LGAPopupButtonImp::CalcTitleRect	( Rect	&outRect )
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
	CalcLocalPopupFrameRect ( outRect );
	::MacInsetRect ( &outRect, 0, bevelWidth );
	outRect.right -= gsPopup_RightInset;
	outRect.left += gsPopup_TitleInset;

													// Adjust the title rect to match the height of
													// the font
	outRect.top += (( UGraphicUtils::RectHeight ( outRect ) - textBaseline) / 2) - 2;
	outRect.bottom = (SInt16) (outRect.top + textHeight);

}


// ---------------------------------------------------------------------------
//	¥ CalcLabelRect													  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::CalcLabelRect	(
		Rect	&outRect ) const
{
	if ( HasLabel ()) {
		StTextState			theTextState;
		StColorPenState	thePenState;

														// Get some loal variables setup including
														// the rect for the title
		ResIDT	textTID = GetTextTraitsID ();

														// Get the port setup with the text traits
		UTextTraits::SetPortTextTraits ( textTID );

														// Figure out the height of the text for the
														// selected font
		FontInfo fInfo;
		GetFontInfo ( &fInfo );
		SInt16		textHeight   = (SInt16) (fInfo.ascent + fInfo.descent);
		SInt16		textBaseline = fInfo.ascent;

														// Get the local frame rectangle and adjust
														// its width to the width of the poups label
		CalcLocalFrameRect ( outRect );
		outRect.right = (SInt16) (outRect.left + (mLabelWidth - gsPopup_LabelOffset));
		::MacInsetRect ( &outRect, 0, 2 );

														// Adjust the title rect to match the height
														// of the font
		outRect.top += (( UGraphicUtils::RectHeight ( outRect ) - textBaseline) / 2) - 2;
		outRect.bottom = (SInt16) (outRect.top + textHeight);

	} else {
		outRect = Rect_0000;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPopupMenuPosition											  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::GetPopupMenuPosition (
		Point	&outPopupLoc )
{
													// Get the local popup button frame, this will
													// be correctly setup depending on whether the
													// button is arrow only or not
	Rect	popupRect;
	CalcLocalPopupFrameRect ( popupRect );
	Rect labelRect;
	SInt16	labelTop;
													// In this next section of code we are going to
													// figure out where the top of the label text
													// rect would be located as we use this value to
													// position the popup menu when it is selected.
													//  If there is a label then we simply get the
													// label rect and use its top, otherwise we need
													// to figure it out.
	if ( HasLabel ()) {
		CalcLabelRect ( labelRect );
		labelTop = labelRect.top;

	} else {
													// We don't really need the label rect when there
													// is no label except we still need to know where
													// the top of the rect would be located as we use
													// that to position the popup
		StTextState			theTextState;
		StColorPenState	thePenState;

		ResIDT	textTID = GetTextTraitsID ();

													// Get the port setup with the text traits
		UTextTraits::SetPortTextTraits ( textTID );

													// Figure out the height of the text for the selected font
		FontInfo fInfo;
		GetFontInfo ( &fInfo );
		SInt16		textBaseline = fInfo.ascent;

													// Get the local frame rectangle so that we can
													// use it as the basis for calculating the label top
		CalcLocalFrameRect ( labelRect );
		::MacInsetRect ( &labelRect, 0, 2 );
		labelTop = labelRect.top;
		labelTop += (( UGraphicUtils::RectHeight ( labelRect ) - textBaseline) / 2) - 2;
	}

													// Setup the vertical value based on whether it
													// is a pulldown or popup menu
	outPopupLoc.v = IsPulldownMenu() ? popupRect.bottom : labelTop;

													// If the popup is setup to have a pulldown menu
													// then calculate the appropriate location for
													// the popup menu to appear based on whether only
													// the arrow button is being displayed or the
													// entire popup button is visible. If it is a
													// normal popup then set up the horizontal value
													// appropriately
	if ( IsPulldownMenu ()) {
		outPopupLoc.h = (SInt16) (popupRect.left + 2);

	} else {
		outPopupLoc.h =  (SInt16) (
			IsArrowOnly () ? popupRect.left + (popupRect.right - popupRect.left) -
																	gsPopup_ArrowButtonWidth :
																		popupRect.left + 1);
	}
													// Because PopMenuSelect needs the location for
													// the popup menu global coordinates we need to
													// convert the location
	mControlPane->LocalToPortPoint ( outPopupLoc );
	mControlPane->PortToGlobalPoint ( outPopupLoc );

}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawSelf	()
{
													// If the popup has been setup to have a variable
													// width and the popup is not being displayed as
													// only the arrow portion, then we also need to
													// make sure that we get the width adjusted
	if ( !mFixedWidth && !IsArrowOnly ()) {
		if (AdjustPopupWidth () ) {
			return;
		}
	}

	StColorPenState::Normalize ();



													// Get the control drawn in its various states
	if ( IsEnabled () && IsActive ()) {
		if ( IsPushed ()) {
			DrawPopupHilited ();
		} else {
			DrawPopupNormal ();
		}
	} else {
		DrawPopupDimmed ();
	}
													// Get the arrow drawn
	DrawPopupArrow ();

													// Draw the popup Label
	if ( !IsArrowOnly ()) {
		DrawPopupLabel ();
	}
													// Get the title for the popup drawn
	if ( !IsArrowOnly ()) {
		DrawPopupTitle ();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupNormal												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupNormal()
{
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

	if (mControlPane->GetBitDepth() < 4) {			// BLACK & WHITE
		DrawPopupNormalBW();
	} else {										// COLOR
		DrawPopupNormalColor();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupHilited												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupHilited()
{
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

	if (mControlPane->GetBitDepth() < 4) {			// BLACK & WHITE
		DrawPopupHilitedBW();
	} else {										// COLOR
		DrawPopupHilitedColor();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupDimmed												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupDimmed()
{
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

	if (mControlPane->GetBitDepth() < 4) {			// BLACK & WHITE
		DrawPopupDimmedBW();
	} else {										// COLOR
		DrawPopupDimmedColor();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupTitle												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupTitle	()
{
													// Get some local variables setup including the
													// rect for the title
	ResIDT	textTID = GetTextTraitsID ();
	Rect	titleRect;
													// Figure out what the justification is from the
													// text trait and  get the port setup with the
													// text traits
	UTextTraits::SetPortTextTraits ( textTID );

													// Setup the title justification which is always left justified
	SInt16	titleJust = teFlushLeft;

													// Calculate the title rect
	CalcTitleRect ( titleRect );

													// Setup the text color which by default is black
	RGBColor	textColor;
	::GetForeColor ( &textColor );
	::TextMode ( srcOr );

													// Get the current item's title
	Str255 currentItemTitle;
	GetCurrentItemTitle ( currentItemTitle );

	if (IsPushed()) {						// Adjust color based on button state
		textColor = Color_White;

	} else if (mControlPane->GetBitDepth() < 4) {
		textColor = Color_Black;

	} else if (!IsEnabled() || !IsActive()) {
		UTextDrawing::DimTextColor(textColor);
	}

												// Now get the actual title drawn with all the
												// appropriate settings
	::RGBForeColor ( &textColor );
	UTextDrawing::DrawTruncatedWithJust( currentItemTitle,
										 titleRect,
										 titleJust,
										 true);
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupLabel												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupLabel()
{
													// Get some loal variables setup including the
													// rect for the title
	ResIDT	textTID = GetTextTraitsID ();
	Rect	labelRect;

													// Get the text traits setup for the port
	 UTextTraits::SetPortTextTraits ( textTID );

													// Calculate the title rect
	CalcLabelRect ( labelRect );
													// Now get the actual title drawn with all the
													// appropriate settings
	Str255	popupLabel;
	GetDescriptor ( popupLabel );
													// Setup the labels style
	::TextFace ( GetLabelStyle () );

													// Save off the text color as setup by the
													// TextTrait
	RGBColor	textColor;
	::GetForeColor ( &textColor );
	::TextMode ( srcOr );

	if ( (mControlPane->GetBitDepth() >= 4)  &&		// Dim color of inactive
		 (!IsEnabled() || !IsActive()) ) {			//   or disabled text

			UTextDrawing::DimTextColor(textColor);
			::RGBForeColor(&textColor);
	}
												// Now get the actual title drawn with all the
												// appropriate settings
	UTextDrawing::DrawTruncatedWithJust (	popupLabel,
											labelRect,
											GetLabelJust (),
											true );
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupArrow												  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupArrow()
{
													// Get the local popup frame rect
	Rect	popupFrame;
	CalcLocalPopupFrameRect ( popupFrame );

													// Get the height for the popup
	SInt16	popupHeight = UGraphicUtils::RectHeight ( popupFrame );
	SInt16	popupWidth = UGraphicUtils::RectWidth ( popupFrame );

													// Setup some variables used in the drawing loop
	SInt16		bevelWidth = 3;
													// Figure out if we are drawing mini arrows or
													// not
	bool		miniArrows = popupHeight < 18;

	SInt16		popupButtonWidth = gsPopup_ArrowButtonWidth;
	SInt16		rowCount = 3;						// Zero-based count

	if (miniArrows) {
		popupButtonWidth -= 3;
		rowCount = 2;
	}

	RGBColor	indicatorColor = Color_Black;
	SInt16		leftEdge;
	SInt16		rightEdge;
	SInt16  	counter;

	if (mControlPane->GetBitDepth() < 4) {					// BLACK & WHITE
												// We only draw the indicator when we are not
												// hilited as the entire button including the
												// indicator is inverted for the hilited state
												// We draw the indicator in black when normal,
												// white when hilited and with a gray pattern
												// when the button is dimmed
		indicatorColor = IsPushed () && IsEnabled () ? Color_White : Color_Black;
		if (!IsActive() || !IsEnabled ()) {
			StColorPenState::SetGrayPattern();
		}

	} else {									// COLOR

												// We draw the indicator in black when normal
												// gray 7 when dimmed and white when hilited
		if ( !IsEnabled () || !IsActive ()) {
			 indicatorColor = UGAColorRamp::GetColor(7);
		}
		if ( IsPushed ()) {
			indicatorColor = UGAColorRamp::GetColor(7);
			if (IsEnabled()) {
				indicatorColor = Color_White;
			}
		}
	}
												// Set the fore color for drawing
	::RGBForeColor ( &indicatorColor );

												// If the popup is not using a pulldown menu we
												// will need to draw the double style of arrows.
												// In addition if the height of the popup is less
												// than 18 pixels we are going to be drawing the
												// mini version of the icons so we need to setup
												// the values appropriately
	if ( !IsPulldownMenu ()) {
		SInt16		arrowWidth;
		SInt16		topStart;
		SInt16		offset;
												// Mini arrow drawing values
		if ( miniArrows ) {
			arrowWidth = gsPopup_MultiArrowWidth - 2;
			topStart   = (SInt16) ((popupHeight - (((rowCount + 1) * 2) + 2)) / 2);
			offset     = (SInt16) (((popupButtonWidth - ((bevelWidth * 2) +
												gsPopup_MultiArrowWidth)) / 2) - 1);
			leftEdge   = (SInt16) ((popupButtonWidth - arrowWidth) - 2);
			rightEdge  = (SInt16) (leftEdge - (gsPopup_MultiArrowWidth - 3));

												// If we are only drawing the arrow portion of
												// the popup we need to set it up a little
												// differently
			if ( mArrowOnly ) {
				leftEdge  = (SInt16) (popupWidth - ((popupWidth - arrowWidth) / 2));
				rightEdge = (SInt16) (leftEdge - (arrowWidth - 1));
			}

		} else {								// Normal arrow drawing values

			arrowWidth = gsPopup_MultiArrowWidth;
			topStart   = (SInt16) ((popupHeight - (((rowCount + 1) * 2) + 2)) / 2);
			offset     = (SInt16) (((popupButtonWidth - ((bevelWidth * 2) +
												gsPopup_MultiArrowWidth)) / 2) + 1);
			leftEdge   = (SInt16) (popupButtonWidth - arrowWidth);
			rightEdge  = (SInt16) (leftEdge - (gsPopup_MultiArrowWidth - 1));
		}

												// TOP ARROW
												// Arrow drawing loop draws 4 rows to make the
												// arrow, the top arrow points upwards
		for ( counter = 0; counter <= rowCount; counter++ ) {
			::MoveTo ( (SInt16) (popupFrame.right - (leftEdge - (rowCount - counter))),
					   (SInt16) (popupFrame.top + topStart + counter) );
			::MacLineTo ( (SInt16) (popupFrame.right - (rightEdge + (rowCount - counter))),
						  (SInt16) (popupFrame.top + topStart + counter) );
		}

												// BOTTOM ARROW
												// Arrow drawing loop draws 4 rows to make the
												// arrow, the bottom arrow points downwards
		SInt16		botStart = (SInt16) (topStart + (miniArrows ? 5 : 6));
		for ( counter = 0; counter <= rowCount; counter++ ) {
			::MoveTo ( (SInt16) (popupFrame.right - (leftEdge - counter)),
					   (SInt16) (popupFrame.top + botStart + counter) );
			::MacLineTo ( (SInt16) (popupFrame.right - (rightEdge + counter)),
						  (SInt16) (popupFrame.top + botStart + counter) );
		}

	} else {
												// For the pulldown menu there is only one arrow
												// drawn that faces down, figure out the left
												// and right edges based on whether we are
												// drawing only the arrow portion or the entire
												// popup
		SInt16		start = (SInt16) ((( UGraphicUtils::RectHeight ( popupFrame ) -
															gsPopup_ArrowHeight) / 2) + 1);

		if ( mArrowOnly ) {
			leftEdge = (SInt16) (popupWidth - ((popupWidth - gsPopup_ArrowWidth) / 2));
			rightEdge = (SInt16) (leftEdge - (gsPopup_ArrowWidth - 1));
		} else {
			leftEdge = gsPopup_ArrowButtonWidth - 6;
			rightEdge = (SInt16) (leftEdge - (gsPopup_ArrowWidth - 1));
		}
												// Arrow drawing loop draws 5 rows to make the
												// arrow
		SInt16  counter;
		for ( counter = 0; counter <= 4; counter++ ) {
			::MoveTo ( (SInt16) (popupFrame.right - (leftEdge - counter)),
					   (SInt16) (popupFrame.top + start + counter) );
			::MacLineTo ( (SInt16) (popupFrame.right - (rightEdge + counter)),
						  (SInt16) (popupFrame.top + start + counter) );
		}
	}
}


#pragma mark -
#pragma mark === BEST RECT

// ---------------------------------------------------------------------------
//	¥ SupportsCalcBestRect											  [public]
// ---------------------------------------------------------------------------

Boolean
LGAPopupButtonImp::SupportsCalcBestRect() const
{
	return true;
}


// ---------------------------------------------------------------------------
//	¥ CalcBestControlRect											  [public]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::CalcBestControlRect(
	SInt16	&outWidth,
	SInt16	&outHeight,
	SInt16	&outBaselineOffset) const
{

	if ( IsArrowOnly ()) {
		Rect	localFrame;
		CalcLocalPopupFrameRect ( localFrame );
		outWidth = UGraphicUtils::RectWidth ( localFrame );
		outHeight = UGraphicUtils::RectHeight ( localFrame );
		outBaselineOffset = (SInt16) (UGraphicUtils::RectHeight ( localFrame ) * 0.66);

	} else {

		StTextState			theTextState;

														// Start by getting the width of the menu
														// associated with the popup
		SInt16	popupWidth = (SInt16) (CalcMaxPopupItemWidth () + gsPopup_TitleInset +
						gsPopup_TitleTrailingInset + gsPopup_ArrowButtonWidth);

														// Get the port setup to the text traits
														// values
		UTextTraits::SetPortTextTraits ( GetTextTraitsID ());

														// Get the text for the label so that we can
														// figure out the width which we only do if
														// there is a label otherwise it is simply 0
		SInt16	labelWidth = 0;
		if ( HasLabel () ) {
			Str255	label;
			GetDescriptor ( label );
			::TextFace ( mLabelStyle );
			labelWidth = (SInt16) (::StringWidth ( label ) + gsPopup_LabelOffset);
		}

														// Adjust the popup width with the label width
														// and the width of the button at the end of
														// the popup
		popupWidth += labelWidth;
														// Get the font info
		FontInfo	fi;
		::GetFontInfo ( &fi );
														// Setup some string data
		SInt16	topOffset    = gsPopup_TopOffset;
		SInt16	bottomOffset = gsPopup_BottomOffset;

		if (fi.ascent <= 10) {
			topOffset	 -= 1;
			bottomOffset -= 1;
		}

		outHeight	= (SInt16) (fi.ascent + fi.descent + topOffset + bottomOffset);
		outWidth	= popupWidth;

		outBaselineOffset = fi.ascent;
		if (fi.ascent == 12) {
			outBaselineOffset += 1;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupNormalBW											   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupNormalBW()
{
	StColorPenState	saveColorPenState;

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Before we draw anything we need to erase the
													// control in case we we had previously been
													// hilited
	::RGBBackColor ( &Color_White );
	::MacInsetRect ( &localFrame, 1, 1 );
	::EraseRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

													// Draw the frame for the control
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

}


// ---------------------------------------------------------------------------
//	¥ DrawPopupHilitedBW										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupHilitedBW	()
{
	StColorPenState	saveColorPenState;

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Draw the frame for the control
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

													// Get the frame for the control
	::MacInsetRect ( &localFrame, 1, 1 );
	::PaintRoundRect ( &localFrame, 4, 4 );
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupDimmedBW											   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupDimmedBW()
{
	StColorPenState	saveColorPenState;
	StColorPenState::Normalize();

	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

	::MacInsetRect ( &localFrame, 1, 1 );			// Erase interior
	::EraseRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

	StColorPenState::SetGrayPattern();				// Dotted line border
	::FrameRoundRect ( &localFrame, 8, 8 );
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupNormalColor										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupNormalColor()
{
	StColorPenState	savePenState;

	RGBColor	tempColor;

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Get the height for the popup
	SInt16		popupHeight = UGraphicUtils::RectHeight ( localFrame );
	Boolean		miniArrows = popupHeight < 18;
	SInt16		popupButtonWidth = (SInt16) (miniArrows ? gsPopup_ArrowButtonWidth - 3 :
													gsPopup_ArrowButtonWidth);

													// Draw the frame for the control outside of
													// everything else
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

													// First make sure the face of the control is
													// drawn
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
	::RGBForeColor ( &tempColor );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

	if ( !IsArrowOnly ()) {
													// EDGES ON BODY OF POPUP
													// LIGHT EDGES
													// Start by rendering the bevelled edges of the
													// sides facing the light source
		::RGBForeColor ( &Color_White );
		::MoveTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.top + 2) );
		::MoveTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.top + 1) );
		::MacLineTo ( (SInt16) (localFrame.right - popupButtonWidth), (SInt16) (localFrame.top + 1) );

													// SHADOW EDGES
													// Render the shadow bevels
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide (
								localFrame, 2, 2, 1, (SInt16) (popupButtonWidth - 1) );
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - (popupButtonWidth)),
								(SInt16) (localFrame.top + 1),
								tempColor );
	}

													// EDGES ON ARROW BUTTON
													// LIGHT EDGES
													// Start by rendering the bevelled edges of the
													// sides facing the light source
													// Setup the appropriate width for the popup
													// arrow portion of the rendering
	SInt16	arrowButtonWidth = popupButtonWidth;
	if ( IsArrowOnly ()) {
		arrowButtonWidth = UGraphicUtils::RectWidth ( localFrame );
	}

	::RGBForeColor ( &Color_White );
	UGraphicUtils::TopLeftSide (
						localFrame,
						2,
						(SInt16) (UGraphicUtils::RectWidth ( localFrame ) - (arrowButtonWidth - 2)),
						3,
						3 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.top + 1),
							tempColor );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 2),
							(SInt16) (localFrame.top + 2),
							tempColor );

													// SHADOW EDGES
													// Render the shadow bevels
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 	(SInt16) (localFrame.left + (UGraphicUtils::RectWidth ( localFrame ) - (arrowButtonWidth - 2))),
				(SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( 	(SInt16) (localFrame.right - 3),
					(SInt16) (localFrame.bottom - 2) );
	::MoveTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.top +  3) );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame,
							3,
							(SInt16) (UGraphicUtils::RectWidth (localFrame) - (arrowButtonWidth - 3)),
							2,
							2 );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.bottom - 3),
							tempColor );

	if ( !IsArrowOnly ()) {
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
		UGraphicUtils::PaintColorPixel (
								(SInt16) (localFrame.right - (popupButtonWidth - 1)),
								(SInt16) (localFrame.bottom - 2),
								tempColor );
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPopupHilitedColor										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupHilitedColor	()
{

	StColorPenState	savePenState;

	RGBColor	tempColor;

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Get the height for the popup
	SInt16		popupHeight = UGraphicUtils::RectHeight ( localFrame );
	Boolean	miniArrows = popupHeight < 18;
	SInt16		popupButtonWidth = (SInt16) (miniArrows ? gsPopup_ArrowButtonWidth - 3 :
													gsPopup_ArrowButtonWidth);

													// Draw the frame for the control outside of
													// everything else
	::RGBForeColor ( &Color_Black );
	::FrameRoundRect ( &localFrame, 8, 8 );

													// First make sure the face of the control is
													// drawn
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
	::RGBForeColor ( &tempColor );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

	if ( !IsArrowOnly ()) {
													// EDGES ON BODY OF POPUP
													// LIGHT EDGES
													// Start by rendering the bevelled edges of the
													// sides facing the light source
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
		::RGBForeColor ( &tempColor );
		::MoveTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.top + 2) );
		::MoveTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.top + 1) );
		::MacLineTo ( (SInt16) (localFrame.right - (popupButtonWidth - 1)),
					  (SInt16) (localFrame.top + 1) );

													// SHADOW EDGES
													// Render the shadow bevels
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 2, 2, 1, (SInt16) (popupButtonWidth - 1));
	}

													// EDGES ON ARROW BUTTON
													// LIGHT EDGES
													// Start by rendering the bevelled edges of the
													// sides facing the light source
													// Setup the appropriate width for the popup
													// arrow portion of the rendering
	SInt16	arrowButtonWidth = popupButtonWidth;
	if ( IsArrowOnly ()) {
		arrowButtonWidth = UGraphicUtils::RectWidth ( localFrame );
	}

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide (
						localFrame,
						1,
						(SInt16) (UGraphicUtils::RectWidth ( localFrame ) - (arrowButtonWidth - 1)),
						1,
						2 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide (
						localFrame,
						2,
						(SInt16) (UGraphicUtils::RectWidth ( localFrame ) - (arrowButtonWidth - 2)),
						3,
						3 );

													// SHADOW EDGES
													// Render the shadow bevels
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 	(SInt16) (localFrame.left + (UGraphicUtils::RectWidth ( localFrame ) - (arrowButtonWidth - 2))),
				(SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( 	(SInt16) (localFrame.right - 3),
					(SInt16) (localFrame.bottom - 2) );
	::MoveTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.top +  3) );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide (
							localFrame,
							3,
							(SInt16) (UGraphicUtils::RectWidth (localFrame) - (popupButtonWidth - 3)),
							2,
							2 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 3),
							(SInt16) (localFrame.bottom - 3),
							tempColor );

}


// ---------------------------------------------------------------------------
//	¥ DrawPopupDimmedColor										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DrawPopupDimmedColor	()
{

	StColorPenState	savePenState;

	RGBColor	tempColor;

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Get the height for the popup
	SInt16		popupHeight = UGraphicUtils::RectHeight ( localFrame );
	Boolean	miniArrows = popupHeight < 18;
	SInt16		popupButtonWidth = (SInt16) (miniArrows ? gsPopup_ArrowButtonWidth - 3 :
													gsPopup_ArrowButtonWidth);

													// Draw the frame for the control outside of
													// everything else
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	::RGBForeColor ( &tempColor );
	::FrameRoundRect ( &localFrame, 8, 8 );

													// First make sure the face of the control is
													// drawn
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
	::RGBForeColor ( &tempColor );
	::PaintRoundRect ( &localFrame, 4, 4 );
	::MacInsetRect ( &localFrame, -1, -1 );

													// Separator between arrow and body
	if ( !IsArrowOnly ()) {
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		::RGBForeColor ( &tempColor );
		::MoveTo ( 	(SInt16) (localFrame.right - popupButtonWidth),
						(SInt16) (localFrame.top + 2) );
		::MacLineTo ( (SInt16) (localFrame.right - popupButtonWidth),
						(SInt16) (localFrame.bottom - 2) );
	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ PointInHotSpot											   [protected]
// ---------------------------------------------------------------------------
//

Boolean
LGAPopupButtonImp::PointInHotSpot (
		Point 	inPoint,
		SInt16		/*inHotSpot*/ ) const
{

	Rect	popupRect;
	CalcLocalPopupFrameRect ( popupRect );

													// We only return true if the mouse is in the
													// popup portion of the control not in the label
	return ::MacPtInRect ( inPoint, &popupRect );

}


// ---------------------------------------------------------------------------
//	¥ TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------
//

Boolean
LGAPopupButtonImp::TrackHotSpot (
		SInt16		inHotSpot,
		Point 	inPoint,
		SInt16		/*inModifiers*/ )
{

													// We only want the popup menu to appear if the
													// mouse went down in the our hot spot which is
													// the popup portion of the control not the
													// label area
	MenuHandle	menuH = GetMacMenuH();
	if ( PointInHotSpot ( inPoint, inHotSpot ) && menuH != nil ) {
													// Get things started off on the right foot
		Boolean		currInside = true;
		Boolean		prevInside = false;
		HotSpotAction ( inHotSpot, currInside, prevInside );

													// We skip the normal tracking that is done in
													// the control as the call to PopupMenuSelect
													// will take control of the tracking once the
													// menu is up
													// Now we need to handle the display of the
													// actual popup menu we start by setting up some
													// values that we will need
		SInt16	menuID = 0;
		SInt16 menuItem = (SInt16) mControlPane->GetValue ();
		SInt16	currItem = (SInt16) (IsPulldownMenu () ? 1 : mControlPane->GetValue ());
		Point popLoc;
		GetPopupMenuPosition ( popLoc );

													// Call our utility function which handles the
													// display of the menu menu is disposed of inside
													// this function
		HandlePopupMenuSelect ( popLoc, currItem, menuID, menuItem );

													// Setup the current item field with the newly
													// selected item this will trigger a broadcast
													// of the new value
		if ( menuItem > 0 ) {
			SetupCurrentMenuItem ( menuH, menuItem );
			mControlPane->SetValue ( menuItem );
		}

													// Make sure that we get the HotSpotAction
													// called one last time
		HotSpotAction ( inHotSpot, false, true );

		return menuItem > 0;
	}

	return false;

}



// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------
//
//	We have overridden this method from our superclass as we are not interested in the
//	control mode we always treat the popup as a button regardless of what the user has
//	set the control mode too

void
LGAPopupButtonImp::HotSpotAction	(
		SInt16		/* inHotSpot */,
		Boolean	inCurrInside,
		Boolean	inPrevInside )
{
													// If the mouse moved in or out of the hot spot
													// handle the hiliting of the control, NOTE: for
													// the popup we ignore the control mode as it
													// does not make any sense to have a popup that
													// behaves like either a radio button or a switch
	if ( inCurrInside != inPrevInside ) {
		SetPushedState ( inCurrInside );	// Toggle the hilite state of the control
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::DoneTracking(
	SInt16		/* inHotSpot */,
	Boolean		/* inGoodTrack */)
{
													// Make sure that by default we turn the
													// hiliting off
	SetPushedState ( false );
}


#pragma mark -
#pragma mark === POPUP MENU HANDLING

// ---------------------------------------------------------------------------
//	¥ HandlePopupMenuSelect										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::HandlePopupMenuSelect	(
		Point		inPopupLoc,
		SInt16		inCurrentItem,
		SInt16		&outMenuID,
		SInt16		&outMenuItem )
{

	MenuHandle	menuH = GetMacMenuH ();
	ThrowIfNil_ ( menuH );
	if ( menuH ) {
														// Save off the current system font family
														// and size
		SInt16 saveFont = ::LMGetSysFontFam ();
		SInt16 saveSize = ::LMGetSysFontSize ();

														// Enclose this all in a try catch block so
														// that we can at least reset the system font
														// if something goes wrong
		try {
														// Handle the actual insertion into the
														// hierarchical menubar
			::MacInsertMenu ( menuH, hierMenu );

														// Reconfigure the system font so that the
														// menu will be drawn in our desired font and
														// size
			mControlPane->FocusDraw ();
			{
				ResIDT	textTID = GetTextTraitsID ();
				TextTraitsH traitsH = UTextTraits::LoadTextTraits ( textTID );
				if ( traitsH )  {
					::LMSetSysFontFam ( (**traitsH).fontNumber );
					::LMSetSysFontSize ( (**traitsH).size );
					::LMSetLastSPExtra ( -1L );
				}
			}
														// Before we display the menu we need to make
														// sure that we have the current item marked
														// in the menu. NOTE: we do NOT use the
														// current item that has been passed in here
														// as that always has a value of one in the
														// case of a pulldown menu
			SetupCurrentMenuItem ( menuH, (SInt16) mControlPane->GetValue () );

														// Then we call PopupMenuSelect and wait for
														// it to return
			SInt32 result = ::PopUpMenuSelect ( 	menuH,
															inPopupLoc.v,
															inPopupLoc.h,
															inCurrentItem );

														// Then we extract the values from the
														// returned result these are then passed back
														// out to the caller
			outMenuID = HiWord ( result );
			outMenuItem = LoWord ( result );

		} catch (...) { }

														// Restore the system font
		::LMSetSysFontFam ( saveFont );
		::LMSetSysFontSize ( saveSize );
		::LMSetLastSPExtra ( -1L );

														// Finally get the menu removed
		::MacDeleteMenu ( ::GetMenuID(menuH) );
	}
}


#pragma mark -
#pragma mark === MISCELLANEOUS


// ---------------------------------------------------------------------------
//	¥ AdjustPopupWidth											   [protected]
// ---------------------------------------------------------------------------
//

bool
LGAPopupButtonImp::AdjustPopupWidth	()
{
													// We are going to look in the menu info record
													// to see what the maximum width of the menu has
													// been setup as, we then use that to figure out
													// the width for the popup portion of the popup.
													// To handle this correctly we need to do the
													// skanky low memory global tweaking we do when
													// displaying the menu to make sure that menu
													// size is calculated for the correct text trait
													// settings, otherwise the size is based on
													// Chicago 12 which means we could end up
													// resizing the popup to the wrong size
	bool	adjusted = false;
	MenuHandle	menuH = GetMacMenuH();
	if ( menuH ) {
													// Get the rect that represents the frame of the
													// control portion of the popup
		Rect popupFrame;
		CalcLocalPopupFrameRect ( popupFrame );

													// Save off the current system font family and
													// size
		SInt16 saveFont = ::LMGetSysFontFam ();
		SInt16 saveSize = ::LMGetSysFontSize ();

													// Enclose this all in a try catch block so that
													// we can at least reset the system font if
													// something goes wrong
		try {
													// Reconfigure the system font so that the menu
													// will be drawn in our desired font and size
			mControlPane->FocusDraw ();
			{
				ResIDT	textTID = GetTextTraitsID ();
				TextTraitsH traitsH = UTextTraits::LoadTextTraits ( textTID );
				if ( traitsH )  {
					::LMSetSysFontFam ( (**traitsH).fontNumber );
					::LMSetSysFontSize ( (**traitsH).size );
					::LMSetLastSPExtra ( -1L );
				}
			}
													// Get the menu size recalculated
			::CalcMenuSize ( menuH );

		} catch (...) { }

													// Set the width of the popup to be the menu width
													// plus the label width and an additional amount
													// that is based on the width of the button
													// portion of the popup.  If we don't add this
													// additional amount the longest item in the
													// menu will always end up beig truncated which
													// is not what we want
		SInt16	newWidth = (SInt16) (::GetMenuWidth(menuH) + mLabelWidth + ((gsPopup_ArrowButtonWidth / 2) + 2));


		SDimension16 frameSize;						// Get the current frame size
		mControlPane->GetFrameSize ( frameSize );

		if ( newWidth != frameSize.width ) {		// Resize if width is different

			mControlPane->ResizeFrameTo ( newWidth, frameSize.height, true );
			mControlPane->FocusDraw ();				// ResizeFromTo may change focus
			adjusted = true;
		}

													// Restore the system font
		::LMSetSysFontFam ( saveFont );
		::LMSetSysFontSize ( saveSize );
		::LMSetLastSPExtra ( -1L );
	}

	return adjusted;
}


// ---------------------------------------------------------------------------
//	¥ CalcMaxPopupItemWidth										   [protected]
// ---------------------------------------------------------------------------
//

SInt16
LGAPopupButtonImp::CalcMaxPopupItemWidth () const
{

	StTextState 		theTextState;

	SInt16	width = 0;
													// We are going to iterate over all the items in
													// the menu item in order to figure out the max
													// width from the longest menu item
	MenuHandle	menuH = GetMacMenuH();
	if ( menuH ) {
													// Get the port setup with the text traits
		UTextTraits::SetPortTextTraits ( GetTextTraitsID () );

		SInt16	itemCount = (SInt16) ::CountMenuItems ( menuH );

		for ( SInt16 index = 1; index <= itemCount; index++ ) {
			Str255	itemText;
			::GetMenuItemText ( menuH, index, itemText );

			SInt16 currWidth = ::StringWidth ( itemText );
			if ( currWidth > width ) {
				width = currWidth;
			}
		}
	}
													// Return the width of the menu if there is one
	return width;

}


// ---------------------------------------------------------------------------
//	¥ CalcPopupLabelWidth										   [protected]
// ---------------------------------------------------------------------------

SInt16
LGAPopupButtonImp::CalcPopupLabelWidth ()
{

	if ( HasLabel ()) {
		StTextState			theTextState;

													// Get the port setup too the text traits values
		UTextTraits::SetPortTextTraits ( GetTextTraitsID ());

													// Get the text for the label so that we can
													// figure out the width which we only do if
													// there is a label otherwise it is simply 0
		SInt16	labelWidth = 0;
		Str255	label;
		GetDescriptor ( label );
		::TextFace ( mLabelStyle );
		labelWidth = (SInt16) (::StringWidth ( label ) + gsPopup_LabelOffset);

		return labelWidth;
	}

	return 0;

}


// ---------------------------------------------------------------------------
//	¥ ResizeForArrowOnly										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::ResizeForArrowOnly ()
{
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalPopupFrameRect ( localFrame );

													// Get the height for the popup
	SInt16		popupHeight = UGraphicUtils::RectHeight ( localFrame );
	Boolean		miniArrows = popupHeight < 18;
	SInt16		popupButtonWidth = (SInt16) (miniArrows ? gsPopup_ArrowButtonWidth - 3 :
													gsPopup_ArrowButtonWidth);

													// We can handle two different variants each
													// designed to be able to match up with any
													// corresponding popup that is a similar size
													// the first is the min case in order for this
													// version to be drawn a starting height under
													// 18 pixels must be specified for the popup.
													// For the larger version anything 18 pixels and
													// over will be drawn with the larger arrows
	SDimension16	newSize;
	SDimension16	frameSize;
	mControlPane->GetFrameSize ( frameSize );
	newSize = frameSize;
	if ( miniArrows ) {
													// If the popup is set to be arrow only then we
													// will automatically make it the minimum
													// recommended size, if it isn't already
		if ( mArrowOnly && frameSize.width < popupButtonWidth ) {
			newSize.width = (SInt16) (popupButtonWidth - 3);
		}
		if ( mArrowOnly && frameSize.height < gsPopup_ArrowButtonHeight ) {
			newSize.height = gsPopup_ArrowButtonHeight - 2;
		}

	} else {
													// If the popup is set to be arrow only then we
													// will automatically make it the minimum
													// recommended size, if it isn't already
		if ( mArrowOnly && frameSize.width < popupButtonWidth ) {
			newSize.width = popupButtonWidth;
		}
		if ( mArrowOnly && frameSize.height < gsPopup_ArrowButtonHeight ) {
			newSize.height = gsPopup_ArrowButtonHeight;
		}
	}

													// Now adjust the size if needed
	if ( newSize.width != frameSize.width || newSize.height != frameSize.height ) {
		mControlPane->ResizeFrameTo ( newSize.width, newSize.height, true );
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcPopupMinMaxValues										   [protected]
// ---------------------------------------------------------------------------

void
LGAPopupButtonImp::CalcPopupMinMaxValues(
	MenuHandle	inMenuH,
	SInt32&		outMinValue,
	SInt32&		outMaxValue)
{
								// Setup the min and max values for the popup
	if (inMenuH != nil) {
		outMinValue = 1;
		outMaxValue = ::CountMenuItems (inMenuH);
	}
}


PP_End_Namespace_PowerPlant
