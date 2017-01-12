// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageButton.cp				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPageButton.h>

#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LStream.h>
#include <LView.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UQDOperators.h>
#include <LString.h>
#include <UTextTraits.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Constants

const	RGBColor	pageButton_DefaultBackColor	= UGAColorRamp::GetColor(colorRamp_Gray2);
const	RGBColor	pageButton_DefaultFaceColor	= UGAColorRamp::GetColor(colorRamp_Gray9);
const	RGBColor	pageButton_TitleHiliteColor = { 0, 65535, 0 };

const	SInt16		pageButton_BottomOffset 	= 12;
const	SInt16		pageButton_TextBottomOffset = 7;


// ---------------------------------------------------------------------------
//	¥ LPageButton							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPageButton::LPageButton()
{
	mPushed			 = false;
	mBackColor		 = pageButton_DefaultBackColor;
	mFaceColor		 = pageButton_DefaultFaceColor;
	mPushedTextColor = pageButton_TitleHiliteColor;
}


// ---------------------------------------------------------------------------
//	¥ LPageButton							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LPageButton::LPageButton(
	const LPageButton&	inOriginal)

	: LControl(inOriginal),
	  LGATitleMixin(inOriginal)
{
	mPushed			 = inOriginal.mPushed;
	mBackColor		 = inOriginal.mBackColor;
	mFaceColor		 = inOriginal.mFaceColor;
	mPushedTextColor = inOriginal.mPushedTextColor;
}


// ---------------------------------------------------------------------------
//	¥ LPageButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPageButton::LPageButton(
	const SPaneInfo& 		inPaneInfo,
	const SControlInfo& 	inControlInfo,
	ConstStringPtr			inTitle,
	Boolean					inHiliteTitle,
	ResIDT					inTextTraitsID,
	const RGBColor&			inBackColor,
	const RGBColor&			inFaceColor,
	const RGBColor&			inPushedTextColor)

	: LControl (inPaneInfo, inControlInfo.valueMessage,
						    inControlInfo.value,
						    inControlInfo.minValue,
						    inControlInfo.maxValue)
{
	mTitle				= inTitle;
	mHiliteTitle		= inHiliteTitle;
	mTextTraitsID		= inTextTraitsID;
	mPushed				= false;
	mBackColor			= inBackColor;
	mFaceColor			= inFaceColor;
	mPushedTextColor	= inPushedTextColor;
}


// ---------------------------------------------------------------------------
//	¥ ~LPageButton							Destructor				  [public]
// ---------------------------------------------------------------------------

LPageButton::~LPageButton ()
{
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LPageButton::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mTitle, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ IsLeftMostButton												  [public]
// ---------------------------------------------------------------------------

Boolean
LPageButton::IsLeftMostButton()
{
	SPoint32 	buttonLoc;
	GetFrameLocation(buttonLoc);

	return (buttonLoc.h == 0);
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LPageButton::CalcTitleRect	( Rect	&outRect )
{
												// Get some loal variables setup including the rect
												// for the title
	ResIDT	textTID = GetTextTraitsID ();

												// Get the port setup with the text traits
	UTextTraits::SetPortTextTraits ( textTID );

												// Figure out the height of the text for the
												// selected font
	FontInfo fInfo;
	::GetFontInfo ( &fInfo );
	SInt16		textHeight = (SInt16) (fInfo.ascent + fInfo.descent);

												// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );
	outRect.bottom -= pageButton_BottomOffset;
	::MacInsetRect ( &outRect, 4, 2 );

												// Adjust the title rect to match the height of
												// the font
	outRect.top += (( UGraphicUtils::RectHeight ( outRect ) - textHeight) / 2);
	outRect.bottom = (SInt16) (outRect.top + textHeight);
	::MacInsetRect ( &outRect, 6, 0 );

												// If the button is hilited then we need to make
												// sure that the title rect is offset appropriately,
												// we move it down to a certain offset from the
												// bottom of the button
	if ( IsPushed ()) {
		SInt16	botAdjust;
		Rect localFrame;
		CalcLocalFrameRect ( localFrame );

		botAdjust = (SInt16) ((localFrame.bottom - outRect.bottom) - pageButton_TextBottomOffset);
		::MacOffsetRect ( &outRect, 0, botAdjust );
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcButtonRegion												  [public]
// ---------------------------------------------------------------------------

RgnHandle
LPageButton::CalcButtonRegion	()
{

	StRegion		buttonRgn;

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	if ( IsPushed ()) {
													// Build a region that represents the hilited
													// shape of the button
		StRegionBuilder	builder ( buttonRgn );
		::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 1) );
		::MacLineTo ( localFrame.right, (SInt16) (localFrame.top + 1) );
		::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - 12) );
		::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 12) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.top + 1) );

	} else {

		localFrame.bottom -= 12;
		buttonRgn = localFrame;
	}

	return buttonRgn.Release();

}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LPageButton::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------
//

void
LPageButton::SetValue(
	SInt32	inValue)
{
												// Call our superclass to handle the setting of
												// the value
	LControl::SetValue ( inValue );

												// If the button is being turned on or off we need
												// to make sure that its pushed state is set
												// appropriately
	SetPushedState ( inValue == Button_Off ? false : true );

}


// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LPageButton::SetPushedState(
	Boolean	inPushedState)
{
												// Only change the hilite state if we have to
	if ( mPushed != inPushedState ) {
		mPushed = inPushedState;
												// Because the state changed we need to make sure
												// that we get things redrawn so that the change
												// takes effect
		Draw ( nil );
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------
//

void
LPageButton::EnableSelf ()
{
	Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------
//

void
LPageButton::DisableSelf ()
{
	Draw(nil);
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LPageButton::ActivateSelf()
{
	Refresh ();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LPageButton::DeactivateSelf	()
{
	Refresh ();
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LPageButton::DrawSelf()
{
	StColorPenState::Normalize ();
	
	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);
	
										// BLACK & WHITE
	if ( bitDepth < 4 ) {
										// Get the control drawn in its various states
		if ( IsEnabled () && IsActive ()) {

			if ( IsPushed ()) {
				DrawPageButtonPushedBW ();
			} else {
				DrawPageButtonNormalBW ();
			}

		} else {
			DrawPageButtonDimmedBW ();
		}

	} else {							// COLOR
										// Get the control drawn in its various states
		if ( IsEnabled () && IsActive ()) {
			if ( IsPushed ()) {
				DrawPageButtonPushedColor ();
			} else {
				DrawPageButtonNormalColor ();
			}
		} else {
			DrawPageButtonDimmedColor ();
		}
	}
												// Call our title drawing routine even though it
												// is non-functional in here
	DrawPageButtonTitle ();

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonTitle										   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonTitle()
{
	StTextState		theTextState;
												// Get some loal variables setup including the rect
												// for the title
	ResIDT		textTID = GetTextTraitsID ();
	Rect		titleRect;

												// Figure out what the justification is from the
												// text trait and get the port setup with the text
												// traits
	SInt16	titleJust = UTextTraits::SetPortTextTraits ( textTID );

												// Get a reference to the foreground color which is
												// used to draw the title text, this color would
												// have been setup by the call to SetPortTextTraits
	RGBColor	textColor;
	::GetForeColor ( &textColor );

												// Calculate the title rect
	CalcTitleRect ( titleRect );
	
	SInt16	bitDepth;
	bool	hasColor;
	
	GetDeviceInfo(bitDepth, hasColor);

	if ( bitDepth < 4 ) {				// BLACK & WHITE

		textColor = Color_Black;

										// If the control is dimmed then we use the
										// grayishTextOr transfer mode to draw the text
		if ( !IsEnabled ()) {
			::TextMode ( grayishTextOr );
		}

	} else {							// COLOR
										// If control is selected we always draw the text
										// in the title hilite color
		if ( IsPushed ()) {
			textColor = mPushedTextColor;
		}
										// If the control is dimmed or inactive we lighten
										// the text color before drawing,
		if ( !IsEnabled () || !IsActive ()) {
			textColor = UGraphicUtils::Lighten ( textColor );
		}
	}
												// Get the text color setup
	::RGBForeColor ( &textColor );
												// Now get the actual title drawn with all the
												// appropriate settings
	Str255 controlTitle;
	GetDescriptor ( controlTitle );
	UTextDrawing::DrawTruncatedWithJust ( controlTitle, titleRect, titleJust );

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonNormalBW									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonNormalBW	()
{
	StColorPenState::Normalize ();
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
	::RGBBackColor ( &Color_White );
	::EraseRect ( &localFrame );
												// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// Render the line along the bottom edge
	::MoveTo ( 	localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)) );
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					localFrame.top );

												// Now draw the shadow along the bottom of the
												// button
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MoveTo ( 	localFrame.left,
				(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)) );

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonPushedBW									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonPushedBW	()
{
	StColorPenState::Normalize ();


	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
												// We start by rendering the back color so that
												// everything  starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
	Rect backRect = localFrame;
	::RGBBackColor ( &Color_White );
	::EraseRect ( &backRect );
												// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// We inset the vertical component on the rect by
												// one pixel
	::MacInsetRect ( &localFrame, 0, 1 );

												// Render the dark edge along the bottom of the
												// button
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// DROP SHADOW
												// Now draw the shadow along the bottom of the
												// button
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::MoveTo ( 		localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));
	::MacLineTo ( 	(SInt16) (localFrame.left + 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

												// Now we need to render the shadows under the
												// button
	::MoveTo ( (SInt16) (localFrame.left + 12), (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 2) );
	::PenSize ( 2, 1 );
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::PenSize ( 1, 1 );
	::MoveTo ( (SInt16) (localFrame.left + 13), (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonDimmedBW									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonDimmedBW()
{
	StColorPenState::Normalize();

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	if ( !IsPushed ()) {					// NORMAL STATE
												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
		::RGBBackColor ( &Color_White );
		::EraseRect ( &localFrame );

												// Render the line along the top edge
		::RGBForeColor ( &Color_Black );
		::MoveTo ( localFrame.left, localFrame.top );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// Render the line along the bottom edge
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)) );
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						localFrame.top );
												// Now draw the shadow along the bottom of the
												// button
		::MoveTo (		localFrame.left,
						(SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo (	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

	} else {									// PUSHED STATE

												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
		Rect backRect = localFrame;
		::RGBBackColor ( &Color_White );
		::EraseRect ( &backRect );
												// Render the line along the top edge
		::RGBForeColor ( &Color_Black );
		::MoveTo ( localFrame.left, localFrame.top );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );
												// We inset the vertical component on the rect by
												// one pixel
		::MacInsetRect ( &localFrame, 0, 1 );
												// Render the dark edge along the bottom of the button
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// DROP SHADOW
													// Now draw the shadow along the bottom of the
													// button
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));
		::MacLineTo ( 	(SInt16) (localFrame.left + 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

													// Now we need to render the shadows under the
													// button
		::MoveTo ( (SInt16) (localFrame.left + 12), (SInt16) (localFrame.bottom - 2) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 2) );
		::PenSize ( 2, 1 );
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::PenSize ( 1, 1 );
		::MoveTo ( (SInt16) (localFrame.left + 13), (SInt16) (localFrame.bottom - 1) );
		::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 1) );
		::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

	}
}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonNormalColor									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonNormalColor	()
{
	StColorPenState::Normalize ();

	RGBColor	tempColor;
												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

												// BUTTON FACE
												// First make sure the face of the control is drawn
	::RGBForeColor ( &mFaceColor);
	localFrame.bottom -= pageButton_BottomOffset;
	::PaintRect ( &localFrame );
	localFrame.bottom += pageButton_BottomOffset;

												// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// LEFT TOP EDGES
												// Render the left top edge - first shade
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( localFrame, 1, 0, pageButton_BottomOffset + 2, 0 );

											   // BOTTOM RIGHT EDGES
											   // Render the bottom right edge - first shade
	tempColor = UGraphicUtils::Darken ( mFaceColor );
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame, 2, 1, pageButton_BottomOffset + 1, 0 );

												// Render the line along the bottom edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( 		localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)) );

												// BACKGROUND
												// Fill the background in the area below the button
												// before we drawing the shadows

	if (mBackColor == Color_White) {			// Interpret white to mean transparent
		mSuperView->ApplyForeAndBackColors();

	} else {
		Pattern	whitePat;
		::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
		::RGBBackColor(&mBackColor);
	}

	localFrame.top += (SInt16) (localFrame.bottom - pageButton_BottomOffset);
	::EraseRect ( &localFrame );
	localFrame.top -= (SInt16) (localFrame.bottom - pageButton_BottomOffset);

												// DROP SHADOW
												// Now draw the shadow along the bottom of the
												// button
	UGAColorRamp::GetColor(colorRamp_Gray8, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );

	UGAColorRamp::GetColor(colorRamp_Gray6, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 		localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)) );

	UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 		localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)) );
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)) );

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonPushedColor									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonPushedColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;

	Rect localFrame;
	CalcLocalFrameRect ( localFrame );
												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
	if (mBackColor == Color_White) {
		mSuperView->ApplyForeAndBackColors();

	} else {
		Pattern	whitePat;
		::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
		::RGBBackColor(&mBackColor);
	}

	Rect backRect = localFrame;
	backRect.top = (SInt16) (backRect.bottom - pageButton_BottomOffset);
	::EraseRect ( &backRect );
												// TOP EDGE - BLACK LINE
												// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );
												// FILL BUTTON FACE
												// Build a region that represents the hilited shape
												// of the button
	RgnHandle hiliteRgn = CalcButtonRegion ();

												// Fill the buttons content to start the rendering
	::RGBForeColor ( &mFaceColor);
	::MacPaintRgn ( hiliteRgn );
	::DisposeRgn ( hiliteRgn );
												// LEFT TOP EDGES
												// Render the left top edge - first shade
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( 	localFrame,
													1,
													0,
													pageButton_BottomOffset + 2,
													0 );

												// BOTTOM RIGHT EDGES
	::MacInsetRect ( &localFrame, 0, 1 );
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
	::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );

												// LEFT EDGE SLOPE
												// Light light shade
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
	::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 4) );

												// Dark shadow shade - bottom, right slope, and
												// right edges
	tempColor = UGraphicUtils::Darken ( mFaceColor );
	::RGBForeColor ( &tempColor );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 4) );
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.top + 1) );

												// CORNER PIXELS BOTTOM LEFT
												// Now we need to set a couple of specific pixels
	tempColor = UGraphicUtils::Lighten ( mFaceColor );
	UGraphicUtils::PaintColorPixel ( (SInt16) (localFrame.left + 9),
									 (SInt16) (localFrame.bottom - 4),
									 tempColor );

												// DROP SHADOW
	UGAColorRamp::GetColor(colorRamp_Gray8, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
	::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

	UGAColorRamp::GetColor(colorRamp_Gray6, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 	localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));
	::MacLineTo ( 	(SInt16) (localFrame.left + 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

	UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( 	localFrame.left,
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));
	::MacLineTo ( 	(SInt16) (localFrame.left + 2),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));

												// Dark gray
	UGAColorRamp::GetColor(colorRamp_Gray8, tempColor);
	::RGBForeColor ( &tempColor );
	::MoveTo ( (SInt16) (localFrame.left + 12), (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 2) );
	::PenSize ( 2, 1 );
	::MacLineTo ( 	(SInt16) (localFrame.right - 1),
					(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

												// Medium gray
	UGAColorRamp::GetColor(colorRamp_Gray6, tempColor);
	::RGBForeColor ( &tempColor );
	::PenSize ( 1, 1 );
	::MoveTo ( (SInt16) (localFrame.left + 13), (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right), (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

												// Light gray
	UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
	::RGBForeColor ( &tempColor );
	::PenSize ( 1, 1 );
	::MoveTo ( (SInt16) (localFrame.left + 14), localFrame.bottom );
	::MacLineTo ( (SInt16) (localFrame.right - 9), localFrame.bottom );
	::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));

}


// ---------------------------------------------------------------------------
//	¥ DrawPageButtonDimmedColor									   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DrawPageButtonDimmedColor()
{
	StColorPenState::Normalize();

	RGBColor	tempColor;

												// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	if ( !IsPushed ()) {					// NORMAL STATE
												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
		if (mBackColor == Color_White) {
			mSuperView->ApplyForeAndBackColors();

		} else {
			Pattern	whitePat;
			::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
			::RGBBackColor(&mBackColor);
		}

		::EraseRect ( &localFrame );

												// First make sure the face of the control is drawn
		tempColor = UGraphicUtils::Lighten ( mFaceColor );
		::RGBForeColor ( &tempColor );
		localFrame.bottom -= pageButton_BottomOffset;
		::PaintRect ( &localFrame );
		localFrame.bottom += pageButton_BottomOffset;

												// Render the line along the top edge
		UGAColorRamp::GetColor(colorRamp_Gray7, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( localFrame.left, localFrame.top );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// Render the line along the bottom edge
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset + 1)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.top + 1) );
												// Now draw the shadow along the bottom of the
												// button
		UGAColorRamp::GetColor(colorRamp_Gray5, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - pageButton_BottomOffset) );

		UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

		UGAColorRamp::GetColor(colorRamp_Gray3, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

	} else {									// PUSHED STATE
												// We start by rendering the back color so that
												// everything starts out correct this needs to be
												// done to handle the redraw after the button has
												// been hilited
		Rect backRect = localFrame;
		backRect.top = (SInt16) (backRect.bottom - pageButton_BottomOffset);

		if (mBackColor == Color_White) {
			mSuperView->ApplyForeAndBackColors();

		} else {
			Pattern	whitePat;
			::BackPat( UQDGlobals::GetWhitePat(&whitePat) );
			::RGBBackColor(&mBackColor);
		}

		::EraseRect ( &backRect );

												// TOP EDGE - GRAY LINE
												// Render the line along the top edge
		UGAColorRamp::GetColor(colorRamp_Gray7, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( localFrame.left, localFrame.top );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// FILL BUTTON FACE
												// Build a region that represents the hilited shape
												// of the button
		RgnHandle hiliteRgn = CalcButtonRegion ();

												// Fill the buttons content to start the rendering
		tempColor = UGraphicUtils::Lighten ( mFaceColor );
		::RGBForeColor ( &tempColor );
		::MacPaintRgn ( hiliteRgn );
		::DisposeRgn ( hiliteRgn );

												// BOTTOM RIGHT EDGES
		::MacInsetRect ( &localFrame, 0, 1 );

												// GRAY EDGE FRAMING
		UGAColorRamp::GetColor(colorRamp_Gray7, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - pageButton_BottomOffset) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

												// DROP SHADOW
		UGAColorRamp::GetColor(colorRamp_Gray5, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

		UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));
		::MacLineTo ( 	(SInt16) (localFrame.left + 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

		UGAColorRamp::GetColor(colorRamp_Gray3, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( 		localFrame.left,
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));
		::MacLineTo ( 	(SInt16) (localFrame.left + 2),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));

												// Dark gray
		UGAColorRamp::GetColor(colorRamp_Gray5, tempColor);
		::RGBForeColor ( &tempColor );
		::MoveTo ( (SInt16) (localFrame.left + 12), (SInt16) (localFrame.bottom - 2) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 2) );
		::PenSize ( 2, 1 );
		::MacLineTo ( 	(SInt16) (localFrame.right - 1),
						(SInt16) (localFrame.bottom - (pageButton_BottomOffset - 1)));

												// Medium gray
		UGAColorRamp::GetColor(colorRamp_Gray4, tempColor);
		::RGBForeColor ( &tempColor );
		::PenSize ( 1, 1 );
		::MoveTo ( (SInt16) (localFrame.left + 13), (SInt16) (localFrame.bottom - 1) );
		::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 1) );
		::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 2)));

												// Light gray
		UGAColorRamp::GetColor(colorRamp_Gray3, tempColor);
		::RGBForeColor ( &tempColor );
		::PenSize ( 1, 1 );
		::MoveTo ( (SInt16) (localFrame.left + 14), localFrame.bottom );
		::MacLineTo ( (SInt16) (localFrame.right - 9), localFrame.bottom );
		::MacLineTo ( localFrame.right, (SInt16) (localFrame.bottom - (pageButton_BottomOffset - 3)));

	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::HotSpotAction	(
	SInt16		/* inHotSpot */,
	Boolean	inCurrInside,
	Boolean	inPrevInside )
{
												// If the mouse moved in or out of the hot spot
												// handle the hiliting of the control
	if ( inCurrInside != inPrevInside ) {

												// Things are a little trickier as we behave a
												// radio button so we only need to toggle the state
												// if the control is not already selected, we also
												// want to avoid doing any unnecessary drawing
		if ( inCurrInside ) {

			if ( !IsPushed ()) {
				SetPushedState ( true );
			}

		} else if ( !IsSelected ()) {
												// If the button was not selected when we clicked
												// on it then we need to make sure we turn the
												// hilite state off  so that it reverts back to its
												// normal state
			SetPushedState ( false );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::DoneTracking(
	SInt16	/* inHotSpot */,
	Boolean	/* inGoodClick */)
{
}	// Do Nothing


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------

void
LPageButton::HotSpotResult(
	SInt16	/* inHotSpot */)
{
												// If the button is not currently selected then we
												// need to ensure that the button is 'on', once it
												// is on we never change it back as it behaves like
												// a radio button and they only get turned off by
												// someone else when another button in a group is
												// selected
	if (!IsSelected()) {
		SetValue(Button_On);
												// We are turning the button on so we also need to
												// broadcast the control clicked message in case
												// someone wants to do something special when a
												// click occurs in a page button
		BroadcastMessage(msg_ControlClicked, (void*) this);
	}
}


PP_End_Namespace_PowerPlant
