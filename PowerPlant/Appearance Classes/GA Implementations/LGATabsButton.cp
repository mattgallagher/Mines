// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGATabsButton.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGATabsButton.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LControl.h>
#include <LStream.h>
#include <UTextTraits.h>
#include <UDrawingUtils.h>
#include <UDrawingState.h>
#include <LString.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


enum {
	tabButton_SmallTabBottomOffset 	= 6,
	tabButton_LargeTabBottomOffset 	= 9,
	tabButton_tabButtonSlope	 	= 11,
	tabButton_IconOffset			= 4,
	tabButton_IconTopOffset			= 3
};


// ---------------------------------------------------------------------------
//	¥ LGATabsButton							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGATabsButton::LGATabsButton()
{
	mPushed			= false;
	mWantsLargeTab	= false;
}


// ---------------------------------------------------------------------------
//	¥ LGATabsButton							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGATabsButton::LGATabsButton(
	const LGATabsButton&	inOriginal)

	: LControl(inOriginal),
	  LGATitleMixin(inOriginal),
	  LGAIconSuiteMixin(inOriginal)
{
	mPushed			= inOriginal.mPushed;
	mWantsLargeTab	= inOriginal.mWantsLargeTab;
}


// ---------------------------------------------------------------------------
//	¥ LGATabsButton							Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LGATabsButton::LGATabsButton(
	const SPaneInfo&		inPaneInfo,
	const SControlInfo&		inControlInfo,
	ConstStringPtr			inTitle,
	Boolean					inHiliteTitle,
	ResIDT					inTextTraitsID,
	ResIDT					inIconSuiteID,
	Boolean					inWantsLargeTab)

	: LControl(inPaneInfo,
				inControlInfo.valueMessage,
				inControlInfo.value,
				inControlInfo.minValue,
				inControlInfo.maxValue)
{
	mTitle			= inTitle;
	mHiliteTitle	= inHiliteTitle;
	mTextTraitsID	= inTextTraitsID;
	mWantsLargeTab	= inWantsLargeTab;
	mPushed			= false;
													// If we have a valid icon suite ID then we need
													// to get it loaded and cached this is all
													// handled simply by setting the resource ID
	if ( (inIconSuiteID != 0)  &&  (inIconSuiteID != resID_Undefined) ) {
		SetIconResourceID(inIconSuiteID);
													// At the same time we will also get the icon
													// size figured out
		CalcIconSuiteSize();
	} else {
		mIconSuiteID = 0;
	}
}


// ---------------------------------------------------------------------------
//	¥ ~LGATabsButton						Destructor				  [public]
// ---------------------------------------------------------------------------

LGATabsButton::~LGATabsButton()
{
}

#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGATabsButton::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mTitle, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ IsLeftMostButton												  [public]
// ---------------------------------------------------------------------------

bool
LGATabsButton::IsLeftMostButton()
{
	SPoint32 	buttonLoc;
	GetFrameLocation(buttonLoc);

	return (buttonLoc.h == 6);
}


// ---------------------------------------------------------------------------
//	¥ CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::CalcTitleRect(
	Rect	&outRect)
{
	StTextState			theTextState;
													// Get some loal variables setup including the
													// rect for the title
	ResIDT	textTID = GetTextTraitsID ();

													// Get the port setup with the text traits
	UTextTraits::SetPortTextTraits ( textTID );

													// Figure out the height of the text for the
													// selected font
	FontInfo fInfo;
	::GetFontInfo ( &fInfo );
	SInt16		textHeight = (SInt16) (fInfo.ascent + fInfo.descent);

													// Figure out the size of the title rect
													// starting from the local frame we have to
													// subtract the width of the side slopes for the
													// button and then setup the correct location
													// for the title based on AGA for tab size
	CalcLocalFrameRect ( outRect );
	outRect.bottom -= WantsLargeTab () ? tabButton_LargeTabBottomOffset -
								fInfo.descent : tabButton_SmallTabBottomOffset - fInfo.descent;
	outRect.left += tabButton_tabButtonSlope;

													// If we have an icon then we need to move the
													// title over to the right
	if ( HasIconSuite ()) {
		outRect.left += 16 + tabButton_IconOffset;
	}

	outRect.right -= tabButton_tabButtonSlope;
	outRect.top = (SInt16) (outRect.bottom - (textHeight + 1));

}


// ---------------------------------------------------------------------------
//	¥ CalcLocalIconRect												  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::CalcLocalIconRect ( Rect &outRect )
{
													// We always start by getting the graphic
													// location recalculated
	Point	iconLocation;
	CalcIconLocation ( iconLocation );
													// If we have an icon suite handle we figure out
													// the size of the rectangle it would occupy
	if ( GetIconSuiteH () != nil ) {
													// Setup the rectangle based on the location and
													// size of the icon
		outRect.top    = iconLocation.v;
		outRect.left   = iconLocation.h;
		outRect.bottom = (SInt16) (iconLocation.v + mSizeSelector);
		outRect.right  = (SInt16) (outRect.left + 16);
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcTabButtonRegion											  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGATabsButton::CalcTabButtonRegion () const
{
													//	Return the approriate region based on the
													// size of the button
	if ( WantsLargeTab () ) {
		return CalcLargeTabButtonRegion ();
	} else {
		return CalcSmallTabButtonRegion ();
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcSmallTabButtonRegion										  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGATabsButton::CalcSmallTabButtonRegion () const
{

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

													// Build the left slope of the tab
	StRegion		leftSlopeRgn;
	{
		StRegionBuilder	builder(leftSlopeRgn);
													// ROW 3 & 4
		::MoveTo (	  localFrame.left, (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
													// ROW 5 & 6
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 6) );
													// ROW 7 & 8
		::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 7) );
		::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
													// ROW 9 & 10
		::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 9) );
		::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 10) );
													// ROW 11 & 12
		::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 11) );
		::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 12) );
													// ROW 13 & 14
		::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 13) );
		::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 14) );
													// ROW 15 & 16
		::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 15) );
		::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 16) );
													// ROW 17
		::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 17) );
													// ROW 18
		::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 18) );
		::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 18) );
													// ROW 19
		::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 19) );
		::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 19) );
													// BACK TO ROW 1
		::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	}

													// Build the right slope of the tab
	StRegion		rightSlopeRgn;
	{
		StRegionBuilder	builder(rightSlopeRgn);
													// ROW 3 & 4
		::MoveTo (    (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 4) );
													// ROW 5 & 6
		::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 5) );
		::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 6) );
													// ROW 7 & 8
		::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 7) );
		::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 8) );
													// ROW 9 & 10
		::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 9) );
		::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 10) );
													// ROW 11 & 12
		::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 11) );
		::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 12) );
													// ROW 13 & 14
		::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 13) );
		::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 14) );
													// ROW 15 & 16
		::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 15) );
		::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 16) );
													// ROW 17
		::MacLineTo ( (SInt16) (localFrame.right - 8), (SInt16) (localFrame.bottom - 17) );
													// ROW 18
		::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 18) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 18) );
													// ROW 19
		::MacLineTo ( (SInt16) (localFrame.right - 11), (SInt16) (localFrame.bottom - 19) );
		::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 19) );
													// BACK TO ROW 1
		::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
	}
													// Build the button region
	::MacInsetRect ( &localFrame, 11, 0 );
	localFrame.bottom -= 3;

	StRegion		buttonRgn ( localFrame );
													// Now we need to union the regions together to
													// make the complete button region

	buttonRgn += leftSlopeRgn;
	buttonRgn += rightSlopeRgn;
													// Return the tab size which affects the tabs
													// height
	return buttonRgn.Release();

}


// ---------------------------------------------------------------------------
//	¥ CalcLargeTabButtonRegion										  [public]
// ---------------------------------------------------------------------------

RgnHandle
LGATabsButton::CalcLargeTabButtonRegion () const
{

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

													// Build the left slope of the tab button
	StRegion		leftSlopeRgn;
	{
		StRegionBuilder	builder(leftSlopeRgn);
													// ROW 3 & 4
		::MoveTo (    localFrame.left, (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
													// ROW 5 - 7
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
		::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 7) );
													// ROW 8 - 10
		::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
		::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 10) );
													// ROW 11 - 13
		::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 11) );
		::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 13) );
													// ROW 14 - 16
		::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 14) );
		::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 16) );
													// ROW 17 - 19
		::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 17) );
		::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 19) );
													// ROW 20 & 21
		::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 20) );
		::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 21) );
													// ROW 22
		::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 22) );
													// ROW 23
		::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 23) );
		::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 23) );
													// ROW 24
		::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 24) );
		::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 24) );
													// BACK TO ROW 1
		::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	}

													// Build the right slope of the tab button
	StRegion		rightSlopeRgn;
	{
		StRegionBuilder	builder(rightSlopeRgn);
													// ROW 3 & 4
		::MoveTo (    (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 4) );
													// ROW 5 - 7
		::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 5) );
		::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 7) );
													// ROW 8 - 10
		::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 8) );
		::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 10) );
													// ROW 11 - 13
		::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 11) );
		::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 13) );
													// ROW 14 - 16
		::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 14) );
		::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 16) );
													// ROW 17 - 19
		::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 17) );
		::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 19) );
													// ROW 20 & 21
		::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 20) );
		::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 21) );
													// ROW 22
		::MacLineTo ( (SInt16) (localFrame.right - 8), (SInt16) (localFrame.bottom - 22) );
													// ROW 23
		::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 23) );
		::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 23) );
													// ROW 24
		::MacLineTo ( (SInt16) (localFrame.right - 11), (SInt16) (localFrame.bottom - 24) );
		::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 24) );
													// BACK TO ROW 1
		::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 3) );
		::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
	}
													// Setup the face of the button
	::MacInsetRect ( &localFrame, 11, 0 );
	localFrame.bottom -= 3;

	StRegion		buttonRgn ( localFrame );
													// Now we need to union the regions together to
													// make the complete button region
	buttonRgn += leftSlopeRgn;
	buttonRgn += rightSlopeRgn;
													// Return the tab size which affects the tabs
													// height
	return buttonRgn.Release();

}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ SetValue														  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::SetValue(
	SInt32	inValue)
{
	LControl::SetValue(inValue);
	Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::SetPushedState(
	Boolean	inPushedState)
{
	if (mPushed != inPushedState) {		// Pushed state has changed
		mPushed = inPushedState;

			// Set icon transform as follows:
			//		Disabled	Tab is disabled
			//		None		Tab is enabled but not pushed
			//		Selected	Tab is enabled and pushed

		if (HasIconSuite()) {
			mTransform = kTransformDisabled;
			if (IsEnabled()) {
				mTransform = kTransformNone;
				if (inPushedState) {
					mTransform = kTransformSelected;
				}
			}
		}

		Draw(nil);						// Redraw tab to reflect new state
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::EnableSelf()
{
		// Tab is being enabled. If tab is also active, then we need
		// to redraw the tab in its normal state.

	if (IsActive()) {
		mTransform = kTransformNone;		// Normal icon
		Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------
//

void
LGATabsButton::DisableSelf ()
{
		// Tab is being disabled. If tab is also active, then we need
		// to redraw the tab in its dimmed state.

	if (IsActive()) {
		mTransform = kTransformDisabled;	// Dimmed icon
		Draw(nil);
	}
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::ActivateSelf()
{
		// Tab is being activated. If tab is also enabled, then we need
		// to redraw the tab in its normal state.

	if (IsEnabled()) {
		mTransform = kTransformNone;		// Normal icon
		Refresh ();
	}
}



// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::DeactivateSelf ()
{
		// Tab is being deactivated. If tab is also enabled, then we need
		// to redraw the tab in its dimmed state.

	if (IsEnabled()) {
		mTransform = kTransformDisabled;	// Dimmed icon
		Refresh ();
	}
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawSelf()
{
	StColorPenState::Normalize ();

	if (GetBitDepth() < 4) {					// BLACK & WHITE
												// Get the control drawn in its various states
		if (IsPushed()) {
			DrawTabButtonHilitedBW();

		} else if (!IsEnabled() || !IsActive()) {
			DrawTabButtonDimmedBW ();

		} else if (IsSelected()) {
			DrawTabButtonOnBW();

		} else {
			DrawTabButtonNormalBW ();
		}

	} else {								// COLOR
											// Get the control drawn in its various states
		if (IsPushed()) {
			DrawTabButtonHilitedColor();

		} else if (!IsEnabled() || !IsActive()) {
			DrawTabButtonDimmedColor();

		} else if (IsSelected()) {
			DrawTabButtonOnColor();

		} else {
			DrawTabButtonNormalColor();
		}
	}
													// If we have an icon then get it drawn now
	if (HasIconSuite()) {
		DrawIconSuite();
	}
													// Call our title drawing routine
	if (HasTitle()) {
		DrawTabButtonTitle();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonTitle											  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonTitle()
{
	StTextState		theTextState;

													// Get some loal variables setup including the
													// rect for the title
	ResIDT	textTID = GetTextTraitsID ();
	Rect		titleRect;
													// Figure out what the justification is from the
													// text trait and  get the port setup with the
													// text traits
	UTextTraits::SetPortTextTraits ( textTID );

													// Calculate the title rect
	CalcTitleRect ( titleRect );

													// Get the text color from the fore color
	RGBColor	textColor;
	::GetForeColor ( &textColor );
													// Setup a device loop so that we can handle
													// drawing at the correct bit depth

	if (GetBitDepth() < 4) {						// BLACK & WHITE
		textColor = IsPushed () ? Color_White : Color_Black;

											// If the control is dimmed then we use the
											// grayishTextOr transfer mode to draw the text
		if ( !IsEnabled () ) {
			::TextMode ( grayishTextOr );
		}

	} else {								// COLOR
											// If control is selected we always draw the
											// text in the title hilite color
		if ( IsPushed ()) {
			textColor = Color_White;
		}
											// If the control is dimmed we lighten the text
											// color before drawing
		if ( !IsActive () || !IsEnabled () ) {
			UTextDrawing::DimTextColor ( textColor );
		}
	}
													// Get the text color setup
	::RGBForeColor ( &textColor );
													// Now get the actual title drawn with all the
													// appropriate settings
	Str255 controlTitle;
	GetDescriptor ( controlTitle );
	SInt16	just = teCenter;

	if ( HasIconSuite ()) {
		just = teFlushLeft;
	}
													// Now we can finally get the title for the tab
													// drawn
	UTextDrawing::DrawTruncatedWithJust(controlTitle, titleRect, just);
}


// ---------------------------------------------------------------------------
//	¥ DrawIconSuite													  [public]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawIconSuite()
{
	StColorPenState	theColorPenState;
	theColorPenState.Normalize ();
													// Get the icon rectangle
	Rect iconRect;
	CalcLocalIconRect ( iconRect );
													//  Now we plot the icon by calling the toolbox
													// passing it our cached handle to the icon
													// suite
	if ( GetIconSuiteH ()) {
		::PlotIconSuite ( &iconRect, kAlignNone, mTransform, GetIconSuiteH());
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonNormalBW										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonNormalBW()
{

	StColorPenState	theColorPenState;

													// We start by filling the button region with
													// white
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	::RGBForeColor ( &Color_White );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// PANEL TOP EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the bottom edge
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonOnBW											   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonOnBW()
{
	StColorPenState	theColorPenState;

													// We start by filling the button region with
													// white
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	::RGBForeColor ( &Color_White );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// PANEL TOP EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the bottom edge
	::RGBForeColor ( &Color_White );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonHilitedBW									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonHilitedBW()
{
	StColorPenState	theColorPenState;

													// We start by filling the button region with
													// white
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	::RGBForeColor ( &Color_Black );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// PANEL TOP EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the bottom edge
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonDimmedBW										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonDimmedBW()
{
	StColorPenState	theColorPenState;

													// We start by filling the button region with
													// white
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	::RGBForeColor ( &Color_White );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// Render the line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// PANEL TOP EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the bottom edge
	RGBColor	tempColor = (IsSelected () ? Color_White : Color_Black);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonNormalColor									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonNormalColor	()
{

	StColorPenState	theColorPenState;

	RGBColor	tempColor;
													// We start by filling the button region
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );

													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// PANEL TOP EDGE
													// Render the black line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 1) );

													// Render the white line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray1);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 2) );

													// PANEL BOTTOM EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 2) );

													// Render the white line along the top edge
	::RGBForeColor ( &Color_White );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 1) );
}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonOnColor										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonOnColor	()
{

	StColorPenState	theColorPenState;

	RGBColor	tempColor;
													// We start by filling the button region
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray1);
	::RGBForeColor ( &tempColor );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// PANEL TOP EDGE
													// Render the black line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 1) );

													// Render the white line along the top edge
	::RGBForeColor ( &Color_White );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 2) );

													// PANEL BOTTOM EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the first line along the bottom edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray1);
	::RGBForeColor ( &tempColor );
	::MoveTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 3) );

													// Render the second line along the bottom edge
	::MoveTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 2) );

													// Render the third line along the bottom edge
	::MoveTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 1) );

}


// ---------------------------------------------------------------------------
//	¥ DrawTabButtonHilitedColor									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonHilitedColor()
{

	StColorPenState	theColorPenState;

	RGBColor	tempColor;
													// We start by filling the button region
	RgnHandle buttonRgn = CalcTabButtonRegion();
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray9);
	::RGBForeColor ( &tempColor );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// PANEL TOP EDGE
													// Render the black line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray11);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 1) );

													// Render the white line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray10);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.top + 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.top + 2) );

													// PANEL BOTTOM EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the black line along the top edge
	::RGBForeColor ( &Color_Black );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 2) );

													// Render the white line along the top edge
	::RGBForeColor ( &Color_White );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 1) );

}

// ---------------------------------------------------------------------------
//	¥ DrawTabButtonDimmedColor									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawTabButtonDimmedColor	()
{

	StColorPenState	theColorPenState;

	RGBColor	tempColor;
													// We start by filling the button region
	RgnHandle buttonRgn = CalcTabButtonRegion ();
	tempColor = IsSelected () ? UGAColorRamp::GetColor (colorRamp_Gray1) :
						UGAColorRamp::GetColor (colorRamp_Gray2);
	::RGBForeColor ( &tempColor );
	::MacPaintRgn ( buttonRgn );
	::DisposeRgn ( buttonRgn );
													// Get the appropriate size drawn for the slope
	if ( WantsLargeTab ()) {
		DrawLargeTabLeftSlope ();
		DrawLargeTabRightSlope ();

	} else {
		DrawSmallTabLeftSlope ();
		DrawSmallTabRightSlope ();
	}
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, 0 );

													// PANEL TOP EDGE
													// Render the dark gray line along the top edge
	tempColor = UGAColorRamp::GetColor (colorRamp_Gray10);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, localFrame.top );
	::MacLineTo ( (SInt16) (localFrame.right - 1), localFrame.top );

													// PANEL BOTTOM EDGE
													// Adjust the rect back to full width
	::MacInsetRect ( &localFrame, -tabButton_tabButtonSlope, 0 );

													// Render the lines along the bottom edge, the
													// first one is drawn in gray10 when the button
													// is not selected and the same as the face when
													// selected the others are drawn in the face
													// color to remove any shading that might have
													// been there
	tempColor = (IsSelected () ? UGAColorRamp::GetColor (colorRamp_Gray1) :
										UGAColorRamp::GetColor (colorRamp_Gray10));
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );

	tempColor = UGAColorRamp::GetColor (colorRamp_Gray1);
	::RGBForeColor ( &tempColor );
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 2) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 2) );

	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 1) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 1) );

}


// ---------------------------------------------------------------------------
//	¥ DrawSmallTabLeftSlope										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawSmallTabLeftSlope()
{
	StColorPenState	thePenState;
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	RGBColor	tempColor = Color_Black;

	if ( (GetBitDepth() >= 4)  &&
		 (!IsActive() || !IsEnabled()) ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor ( &tempColor );

												// ROW 3 & 4
	::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 & 6
	::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
	::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 6) );
												// ROW 7 & 8
	::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 7) );
	::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
												// ROW 9 & 10
	::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 9) );
	::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 & 12
	::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 12) );
												// ROW 13 & 14
	::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 13) );
	::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 14) );
												// ROW 15 & 16
	::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 15) );
	::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 16) );
												// ROW 17
	::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 17) );
												// ROW 18
	::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 18) );
	::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 18) );
												// ROW 19
	::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 19) );
	::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 19) );

												// COLOR DETAILS
	if (GetBitDepth() >= 4) {
												// SHADOW EDGE
		if ( IsActive () && IsEnabled ()) {
			if ( IsSelected () || IsPushed ()) {
												// Setup the correct color for the state we are
												// drawing
				if ( IsSelected ()) {
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
					::RGBForeColor ( &tempColor );

				} else if ( IsPushed ()) {
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					::RGBForeColor ( &tempColor );
				}

												// If the button is selected we need to draw a
												// few additional pixels to meet up with the
												// panels edges
				if ( IsSelected ()) {
												// ROW 2 & 3
					::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 2) );
					::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
				}

												// Inset the rect horizontally
				::MacInsetRect ( &localFrame, 1, 0 );

												// ROW 3 & 4
				::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
				::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 & 6
				::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
				::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 6) );
												// ROW 7 & 8
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 7) );
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
												// ROW 9 & 10
				::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 9) );
				::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 & 12
				::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 11) );
				::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 12) );
												// ROW 13 & 14
				::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 13) );
				::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 14) );
												// ROW 15 & 16
				::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 15) );
				::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 16) );
												// ROW 17
				::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 17) );
				::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 17) );
												// ROW 18
				::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 18) );
				::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 18) );

												// Inset the rect horizontally
				::MacInsetRect ( &localFrame, -1, 0 );
			}

												// LIGHT EDGE
												// Setup the correct color for the state we are
												// drawing
			if ( IsSelected ()) {
				::RGBForeColor ( &Color_White );

			} else if ( IsPushed ()) {
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				::RGBForeColor ( &tempColor );

			} else {
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				::RGBForeColor ( &tempColor );
			}
												// If the button is selected we need to draw a
												// few additional pixels to meet up with the
												// panels edges
			if ( IsSelected ()) {
												// ROW 1 - 3
				::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 1) );
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 3) );
			}
												// Inset the rect horizontally
			::MacInsetRect ( &localFrame, 2, 0 );

												// ROW 3 & 4
			::MoveTo (    localFrame.left, (SInt16) (localFrame.bottom - 3) );
			::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 & 6
			::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 6) );
												// ROW 7 & 8
			::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 7) );
			::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
												// ROW 9 & 10
			::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 9) );
			::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 & 12
			::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 11) );
			::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 12) );
												// ROW 13 & 14
			::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 13) );
			::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 14) );
												// ROW 15 & 16
			::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 15) );
			::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 16) );
			::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 16) );
												// ROW 17
			::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 17) );
			::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 17) );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawSmallTabRightSlope									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawSmallTabRightSlope()
{

	StColorPenState	thePenState;
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	RGBColor	tempColor = Color_Black;

	if ( (GetBitDepth() >= 4)  &&
		 (!IsActive() || !IsEnabled()) ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor ( &tempColor );

												// ROW 3 & 4
	::MoveTo (    (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 4) );
												// ROW 5 & 6
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 5) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 6) );
												// ROW 7 & 8
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 7) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 8) );
												// ROW 9 & 10
	::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 9) );
	::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 & 12
	::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 12) );
												// ROW 13 & 14
	::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 13) );
	::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 14) );
												// ROW 15 & 16
	::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 15) );
	::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 16) );
												// ROW 17
	::MacLineTo ( (SInt16) (localFrame.right - 8), (SInt16) (localFrame.bottom - 17) );
												// ROW 18
	::MacLineTo ( (SInt16) (localFrame.right -  9), (SInt16) (localFrame.bottom - 18) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 18) );
												// ROW 19
	::MacLineTo ( (SInt16) (localFrame.right - 11), (SInt16) (localFrame.bottom - 19) );
	::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 19) );

												// COLOR DETAILS
	if ( GetBitDepth() >= 4 ) {
												// SHADOWS
		if ( IsActive () && IsEnabled ()) {
			if ( IsSelected ()) {
												// ROW 1
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 1),
										(SInt16) (localFrame.bottom - 1),
										tempColor );
												// ROW 2
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 1),
										(SInt16) (localFrame.bottom - 2),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 2),
										tempColor );
												// ROW 3
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 1),
										(SInt16) (localFrame.bottom - 3),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 3),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 3),
										tempColor );
												// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
												// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
												// ROW 6
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
												// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
												// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
												// ROW 9
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
												// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
												// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 11),
										tempColor);
													// ROW 12
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
													// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
													// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
													// ROW 15
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
													// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
													// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 17),
										Color_White );
													// ROW 18
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 18),
										tempColor );

			} else if ( IsPushed ()) {		// PUSHED STATE
													// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
													// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
													// ROW 6
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
													// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
													// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
													// ROW 9
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
													// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
													// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
													// ROW 12
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
													// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
													// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
													// ROW 15
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
													// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
													// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
													// ROW 18
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 18),
										tempColor );

			} else {								// NORMAL STATE
													// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
													// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
													// ROW 6
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
													// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
													// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
													// ROW 9
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
													// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
													// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
													// ROW 12
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
													// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
													// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
													// ROW 15
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
													// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
													// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
													// ROW 18
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 18),
										tempColor );
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawLargeTabLeftSlope										   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawLargeTabLeftSlope()
{

	StColorPenState	thePenState;
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	RGBColor	tempColor = Color_Black;

	if ( (GetBitDepth() >= 4)  &&
		 (!IsActive() || !IsEnabled()) ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor ( &tempColor );

												// ROW 3 & 4
	::MoveTo (    localFrame.left, (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 - 7
	::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
	::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 7) );
												// ROW 8 - 10
	::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
	::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 - 13
	::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 13) );
												// ROW 14 - 16
	::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 14) );
	::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 16) );
												// ROW 17 - 19
	::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 17) );
	::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 19) );
												// ROW 20 & 21
	::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 20) );
	::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 21) );
												// ROW 22
	::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 22) );
												// ROW 23
	::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 23) );
	::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 23) );
												// ROW 24
	::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 24) );
	::MacLineTo ( (SInt16) (localFrame.left + 11), (SInt16) (localFrame.bottom - 24) );

												// COLOR DETAILS
	if ( GetBitDepth() >= 4 ) {
												// SHADOW EDGE
		if ( IsActive () && IsEnabled ()) {
			if ( IsSelected () || IsPushed ()) {
												// Setup the correct color for the state we are
												// drawing
				if ( IsSelected ()) {
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
					::RGBForeColor ( &tempColor );

				} else if ( IsPushed ()) {
					tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
					::RGBForeColor ( &tempColor );
				}
												// If the button is selected we need to draw a
												// few additional pixels to meet up with the
												// panels edges
				if ( IsSelected ()) {
												// ROW 2 & 3
					::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 2) );
					::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 3) );
				}
												// Inset the rect horizontally
				::MacInsetRect ( &localFrame, 1, 0 );

												// ROW 3 & 4
				::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
				::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 - 7
				::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
				::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 7) );
												// ROW 8 - 10
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 - 13
				::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 11) );
				::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 13) );
												// ROW 14 - 16
				::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 14) );
				::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 16) );
												// ROW 17 - 19
				::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 17) );
				::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 19) );
												// ROW 20 & 21
				::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 20) );
				::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 21) );
				::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 21) );
												// ROW 22
				::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 22) );
				::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 22) );
												// ROW 23
				::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 23) );
				::MacLineTo ( (SInt16) (localFrame.left + 10), (SInt16) (localFrame.bottom - 23) );

												// Inset the rect horizontally
				::MacInsetRect ( &localFrame, -1, 0 );
			}
												// LIGHT EDGE
												// Setup the correct color for the state we are
												// drawing
			if ( IsSelected ()) {
				::RGBForeColor ( &Color_White );

			} else if ( IsPushed ()) {
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				::RGBForeColor ( &tempColor );

			} else {
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				::RGBForeColor ( &tempColor );
			}
												// If the button is selected we need to draw a
												// few additional pixels to meet up with the
												// panels edges
			if ( IsSelected ()) {
												// ROW 1 - 3
				::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 1) );
				::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 3) );
			}
												// Inset the rect horizontally
			::MacInsetRect ( &localFrame, 2, 0 );

												// ROW 3 & 4
			::MoveTo ( localFrame.left, (SInt16) (localFrame.bottom - 3) );
			::MacLineTo ( localFrame.left, (SInt16) (localFrame.bottom - 4) );
												// ROW 5 - 7
			::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 5) );
			::MacLineTo ( (SInt16) (localFrame.left + 1), (SInt16) (localFrame.bottom - 7) );
												// ROW 8 - 10
			::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 8) );
			::MacLineTo ( (SInt16) (localFrame.left + 2), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 - 13
			::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 11) );
			::MacLineTo ( (SInt16) (localFrame.left + 3), (SInt16) (localFrame.bottom - 13) );
												// ROW 14 - 16
			::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 14) );
			::MacLineTo ( (SInt16) (localFrame.left + 4), (SInt16) (localFrame.bottom - 16) );
												// ROW 17 - 19
			::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 17) );
			::MacLineTo ( (SInt16) (localFrame.left + 5), (SInt16) (localFrame.bottom - 19) );
												// ROW 20
			::MacLineTo ( (SInt16) (localFrame.left + 6), (SInt16) (localFrame.bottom - 20) );
												// ROW 21
			::MacLineTo ( (SInt16) (localFrame.left + 7), (SInt16) (localFrame.bottom - 21) );
												// ROW 22 & 23
			::MacLineTo ( (SInt16) (localFrame.left + 8), (SInt16) (localFrame.bottom - 22) );
			::MacLineTo ( (SInt16) (localFrame.left + 9), (SInt16) (localFrame.bottom - 22) );
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawLargeTabRightSlope									   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DrawLargeTabRightSlope	()
{

	StColorPenState	thePenState;
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	RGBColor	tempColor = Color_Black;

	if ( (GetBitDepth() >= 4)  &&
		 (!IsActive() || !IsEnabled()) ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor ( &tempColor );

												// ROW 3 & 4
	::MoveTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 3) );
	::MacLineTo ( (SInt16) (localFrame.right - 1), (SInt16) (localFrame.bottom - 4) );
												// ROW 5 - 7
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 5) );
	::MacLineTo ( (SInt16) (localFrame.right - 2), (SInt16) (localFrame.bottom - 7) );
												// ROW 8 - 10
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 8) );
	::MacLineTo ( (SInt16) (localFrame.right - 3), (SInt16) (localFrame.bottom - 10) );
												// ROW 11 - 13
	::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 11) );
	::MacLineTo ( (SInt16) (localFrame.right - 4), (SInt16) (localFrame.bottom - 13) );
												// ROW 14 - 16
	::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 14) );
	::MacLineTo ( (SInt16) (localFrame.right - 5), (SInt16) (localFrame.bottom - 16) );
												// ROW 17 - 19
	::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 17) );
	::MacLineTo ( (SInt16) (localFrame.right - 6), (SInt16) (localFrame.bottom - 19) );
												// ROW 20 & 21
	::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 20) );
	::MacLineTo ( (SInt16) (localFrame.right - 7), (SInt16) (localFrame.bottom - 21) );
												// ROW 22
	::MacLineTo ( (SInt16) (localFrame.right - 8), (SInt16) (localFrame.bottom - 22) );
												// ROW 23
	::MacLineTo ( (SInt16) (localFrame.right - 9), (SInt16) (localFrame.bottom - 23) );
	::MacLineTo ( (SInt16) (localFrame.right - 10), (SInt16) (localFrame.bottom - 23) );
												// ROW 24
	::MacLineTo ( (SInt16) (localFrame.right - 11), (SInt16) (localFrame.bottom - 24) );
	::MacLineTo ( (SInt16) (localFrame.right - 12), (SInt16) (localFrame.bottom - 24) );

												// COLOR DETAILS
	if ( GetBitDepth() >= 4 ) {
												// SHADOWs
		if ( IsActive () && IsEnabled ()) {
			if ( IsSelected ()) {
												// ROW 1
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 1),
										(SInt16) (localFrame.bottom - 1),
										Color_White );
												// ROW 2
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 1),
										(SInt16) (localFrame.bottom - 2),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 2),
										tempColor );
												// ROW 3
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 3),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray1);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 3),
										tempColor);
												// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
												// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
												// ROW 6
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
												// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
												// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
												// ROW 9
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
												// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
												// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
												// ROW 12
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
												// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
												// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
												// ROW 15
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
												// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
												// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
												// ROW 18
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 18),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 18),
										tempColor );
												// ROW 19
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 19),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 19),
										tempColor );
												// ROW 20
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 20),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 20),
										tempColor );
												// ROW 21
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 21),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 21),
										tempColor );
												// ROW 22
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 22),
										Color_White );
												// ROW 23
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 23),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 12),
										(SInt16) (localFrame.bottom - 23),
										tempColor );

			} else if ( IsPushed ()) {	// PUSHED STATE
												// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
												// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
												// ROW 6
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
												// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
												// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
												// ROW 9
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
												// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
												// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
												// ROW 12
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 12),
										tempColor);
												// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
												// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
												// ROW 15
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
												// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
												// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
												// ROW 18
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 18),
										tempColor );
												// ROW 19
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 19),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 19),
										tempColor);
												// ROW 20
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 20),
										tempColor );
												// ROW 21
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 21),
										tempColor);
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 21),
										tempColor );
												// ROW 22
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
												// ROW 23
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 23),
										tempColor );

			} else {							// NORMAL STATE
												// ROW 4
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 2),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 4),
										tempColor );
												// ROW 5
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 5),
										tempColor );
												// ROW 6
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 6),
										tempColor );
												// ROW 7
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 3),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 7),
										tempColor );
												// ROW 8
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 8),
										tempColor );
												// ROW 9
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 9),
										tempColor );
												// ROW 10
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 4),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 10),
										tempColor );
												// ROW 11
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 11),
										tempColor );
												// ROW 12
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 12),
										tempColor );
												// ROW 13
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 5),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 13),
										tempColor );
												// ROW 14
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 14),
										tempColor );
												// ROW 15
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 15),
										tempColor );
												// ROW 16
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 6),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 16),
										tempColor );
												// ROW 17
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 17),
										tempColor );
												// ROW 18
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 18),
										tempColor );
												// ROW 19
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 7),
										(SInt16) (localFrame.bottom - 19),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 19),
										tempColor );
												// ROW 20
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 20),
										tempColor );
												// ROW 21
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 8),
										(SInt16) (localFrame.bottom - 21),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray4);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 21),
										tempColor );
												// ROW 22
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 9),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray2);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 10),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 22),
										tempColor );
												// ROW 23
				tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
				UGraphicUtils::PaintColorPixel (
										(SInt16) (localFrame.right - 11),
										(SInt16) (localFrame.bottom - 23),
										tempColor );
			}
		}
	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ PointIsInFrame											   [protected]
// ---------------------------------------------------------------------------

Boolean
LGATabsButton::PointIsInFrame(
	SInt32	inHorizPort,
	SInt32	inVertPort) const
{
													// If the option to handle the click only in the
													// region occupied by the icon is on then handle
													// that first otherwise we just go to the
													// standard version of this method which handles
													// a click in the pane's frame
	RgnHandle		tabButtonRgn = CalcTabButtonRegion ();
	Point			portPt = { (SInt16) inVertPort, (SInt16) inHorizPort };
	PortToLocalPoint ( portPt );
	Boolean		inTabButton = ::PtInRgn ( portPt, tabButtonRgn );
	::DisposeRgn ( tabButtonRgn );

	return inTabButton;
}


// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
													// If the mouse moved in or out of the hot spot
													// handle the hiliting of the control which is
													// slightly different for each of the modes
													// supported by the control
	if (inCurrInside != inPrevInside) {
													// Change the hilite state
		SetPushedState(inCurrInside && !IsSelected());
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::DoneTracking(
	SInt16 		/* inHotSpot */,
	Boolean 	/* inGoodTrack */)
{
													// Make sure that by default we turn the
													// hiliting off, all we are going to do is turn
													// the actual flag off as we don't want the
													// button to be redrawn in its normal state
													// before it gets drawn in the On state, by
													// doing this it will just go from the hilited
													// to the On state.  We can do this because we
													// know we are going to be redrawn when the
													// value gets set.
	bool wasPushed = mPushed;
	mPushed = false;
													// Reset the transform to the normal state
	if ( HasIconSuite ()) {
		mTransform = kTransformNone;
	}
													// Do the right thing about getting the value
													// set after the tab has been clicked in its
													// hotspot, NOTE: this method assumes the entire
													// control is the hotspot
	if ( wasPushed && !IsSelected ()) {
		SetValue ( Button_On );
	}
}


#pragma mark -
#pragma mark === MISCELLANEOUS

// ---------------------------------------------------------------------------
//	¥ CalcIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::CalcIconLocation(
	Point&	outIconLocation)
{
											// Get the local inset frame rect
	Rect		localFrame;
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, tabButton_tabButtonSlope, tabButton_IconTopOffset );
	outIconLocation.h = localFrame.left;
	outIconLocation.v = localFrame.top;

}


// ---------------------------------------------------------------------------
//	¥ CalcIconSuiteSize											   [protected]
// ---------------------------------------------------------------------------

void
LGATabsButton::CalcIconSuiteSize()
{
								// Figure out the icon size selector which is
								// either a small or a mini icon depending on
								// the size of the tab
	if (HasIconSuite()) {
		mSizeSelector = sizeSelector_MiniIconSize;
		if (WantsLargeTab()) {
			mSizeSelector = sizeSelector_SmallIconSize;
		}
	}
}


PP_End_Namespace_PowerPlant
