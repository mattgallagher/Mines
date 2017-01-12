// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGABevelButtonImp.cp		PowerPlant 2.2.2	й1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGABevelButtonImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LBevelButton.h>
#include <LView.h>
#include <LControlPane.h>
#include <LStream.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UTextTraits.h>
#include <UTBAccessors.h>

#include <LowMem.h>
#include <ToolUtils.h>

PP_Begin_Namespace_PowerPlant


const	UInt16	bevelButton_SmallMark	=	char_Bullet;	// Mark used for small font popups

enum {
	bevelButton_GraphicSlop 			= 	3,
	bevelButton_LargePopupGlyphSize 	= 	9,
	bevelButton_LargePopupGlyphRows 	= 	5,
	bevelButton_SmallPopupGlyphSize 	= 	5,
	bevelButton_SmallPopupGlyphRows 	= 	3
};


// ---------------------------------------------------------------------------
//	е LGABevelButtonImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGABevelButtonImp::LGABevelButtonImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	е LGABevelButtonImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGABevelButtonImp::LGABevelButtonImp(
	LStream*	inStream)

	: LGAControlImp (inStream)
{
	mContentType		= kControlContentTextOnly;
	mOffsetContents		= true;
	mMultiValueMenu		= false;
	mBehavior			= kControlBehaviorPushbutton;
	mResourceID			= resID_Undefined;
	mUseWindowFont		= false;
	mArrowRight			= false;
	mBevelWidth			= 2;
	mTitlePlacement		= kControlBevelButtonPlaceSysDirection;
	mTitleAlignment		= kControlBevelButtonAlignTextSysDirection;
	mTitleOffset		= 0;
	mGraphicAlignment	= kControlBevelButtonAlignSysDirection;
	mGraphicOffset		= Point_00;
	mCenterPopupGlyph	= false;

											// Override default settings
	mHiliteIcon			= true;				// BevelButtons hilite by default
	mHiliteCIcon		= true;
}


// ---------------------------------------------------------------------------
//	е ~LGABevelButtonImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGABevelButtonImp::~LGABevelButtonImp()
{
}


// ---------------------------------------------------------------------------
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16		controlKind;
	Str255		title;
	ResIDT		textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitID);
}


// ---------------------------------------------------------------------------
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::Init (
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			/*inRefCon*/ )
{
													// Start by setup the control pane reference
	mControlPane = inControlPane;

													// Setup the title and text trait ID
	mTextTraitsID = inTextTraitsID;
	mTitle = inTitle;

													// Do some initialization

													// Assigment will strip off the high 2 bytes and
													// copy the low 2 bytes
	UInt16	theBehavior = (UInt16) mControlPane->GetMinValue ();

													// For resource-based content types, the Resource
													// ID is in the max value.

	SInt32	resourceID = mControlPane->GetMaxValue ();

													// The behavior is actually just the first 8-bits
													// of theBehavior variable, but the Toolbox mask
													// constants are 16-bit values

													// Use Toolbox contants to mask off bits
	mOffsetContents = (theBehavior & kControlBehaviorOffsetContents) != 0;
	mMultiValueMenu = (theBehavior & kControlBehaviorMultiValueMenu) != 0;

													// Click behavior is in bits 8 and 9
													//
													//	Possible values:
													//		kControlBehaviorPushbutton	0x0000 (0)
													//		kControlBehaviorToggles		0x0100 (256)
													//		kControlBehaviorSticky		0x0200 (512)
													//
													// NOTE: The mask operation below allows the
													// mathematical possibility that the value will
													// be 0x0300, which corresponds to a behavior
													// that is both toggle and sticky. This is
													// self-contradictory, so it's a bad value. You
													// could be extra careful and check for that
													// value here.
	UInt16	clickBehavior = (UInt16) (theBehavior & 0x00000300);

													// Make sure we don't end up with a bad value,
													// if we do then we set it to
													// the toggle value and hope for the best
	if ( clickBehavior == 0x00000300 ) {
		clickBehavior = kControlBehaviorToggles;
	}
													// Coerce the result to our enumeration
	mBehavior = (SInt16) clickBehavior;

													// Content type is in the low byte of the min
													// value. Assigment from 4-byte value to 1-byte
													// value will copy just the low byte, which is
													// what we want.
													// Possible value:
													//		kControlContentTextOnly				= 0,
													//		kControlContentIconSuiteRes		= 1,
													//		kControlContentCIconRes				= 2,
													//		kControlContentPictRes				= 3,
													//		kControlContentIconSuiteHandle 	= 129,
													//		kControlContentCIconHandle			= 130,
													//		kControlContentPictHandle			= 131,
													//		kControlContentIconRef				= 132
	UInt8	contentType = (UInt8) mControlPane->GetMinValue ();
	mContentType = contentType;

													// Read PPob data for LStdControl

													// In PP, we normally ignore the "use window
													// font" variation code setting. We use the font
													// specified by the Text Traits ID. But for
													// completeness we extract it anyway
	mUseWindowFont = (inControlKind & kControlUsesOwningWindowsFontVariant) != 0;

													// Popup Arrow Direction (also location of menu
													// on button)
													//		= true, Arrow faces right and is on right
													//       of button which will result in the menu
													//			poping up to the right
													//		= false, Arrow faces down and is on bottom
													//			 of the button
	mArrowRight = (inControlKind & kControlBevelButtonMenuOnRight) != 0;

													// Bevel size:
													//
													//		kControlBevelButtonSmallBevelVariant	= 0
													//		kControlBevelButtonNormalBevelVariant	= 1
													//		kControlBevelButtonLargeBevelVariant	= 2
													//
													// NOTE: The mask operation below allows the
													// possibility that the value will be 3, which
													// is undefined.
	UInt8	bevelSize = (UInt8) (inControlKind & 0x0003);

													// If for some reason the bevel size is larger
													// than that allowed we will default it to the
													// normal bevel size
	if ( bevelSize > 2 ) {
		bevelSize = 1;
	}
													// Setup the bevel width by adding one to the
													// extracted value we store this as the actual
													// width we use for drawing purposes
													// NOTE: this width does not include the border
													// around the button
	mBevelWidth = (SInt16) (bevelSize + 1);

													// Setting the resource ID will result in the
													// right thing being done based on the content
													// type specified, either loading and caching an
													// icon or caching the picture frame, NOTE: we
													// only do this for the resource based content
													// for handle based content we set the ID to
													// zero and we make sure we start out with the
													// caches empty
	if ( HasIconSuiteRes() || HasCIconRes() || HasPictRes()) {
		if ( resourceID != 0 && resourceID != resID_Undefined ) {
			SetResourceID ( (SInt16) resourceID );

		} else {
			mResourceID = (SInt16) resourceID;
		}

	} else {
		mResourceID = 0;
		mIconSuiteH = nil;
		mCIconH		= nil;
		mPictureH	= nil;
	}
													// If the content is an icon suite then we need
													// to get all the appropriate data setup for
													// that type
	if ( HasIconSuiteContent ()) {
													// Figure out what size the icon suite will be
													// drawn at, either large, small or mini
		CalcIconSuiteSize ();
	}

													// Make sure that the transform for the icon is
													// correctly setup, NOTE: this applies to any of
													// the content types as we use the transform for
													// both the cicn and the picture types
	AdjustTransform ( false, false );

													// Start out with this set to false it will get
													// set when we get called through the SetDataTag call
	mCenterPopupGlyph = false;
}


void
LGABevelButtonImp::FinishCreateSelf()
{
	mPushed = mControlPane->GetValue () == Button_On;
}

#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	е GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGABevelButtonImp::GetDescriptor (
		Str255	outDescriptor ) const
{
	return LString::CopyPStr ( mTitle, outDescriptor );
}


// ---------------------------------------------------------------------------
//	е CalcTitleRect													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcTitleRect(
	Rect&	outRect)
{
	StTextState		theTextState;

	UTextTraits::SetPortTextTraits( GetTextTraitsID() );

													// Figure out the height of the text for the
													// selected font
	FontInfo fi;
	GetFontInfo ( &fi );
	SInt16		textHeight   = (SInt16) (fi.ascent + fi.descent);
	SInt16		textBaseline = fi.ascent;
	SInt16		titleWidth   = CalcTitleWidth ();

													// Make sure that the bevel width is adjusted to
													// accomodate the border frame
	SInt16 bevelWidth = (SInt16) (GetBevelWidth () + 1);

													// Get the local inset frame rectangle
	CalcLocalFrameRect ( outRect );
	::MacInsetRect ( &outRect, bevelWidth, bevelWidth );

													// Save off the button width less bevels
	SInt16		buttonWidth = (SInt16) (UGraphicUtils::RectWidth ( outRect ) - (bevelWidth * 2));

													// TEXT ONLY
													// If we only have a title an no graphic then
													// things become easier
	if ( HasTextOnly ()) {
													// Adjust the title rect to match the height of
													// the font
		outRect.top += (( UGraphicUtils::RectHeight ( outRect ) -
									textBaseline) / 2) - 2;
		outRect.bottom = (SInt16) (outRect.top + textHeight);
													// If there is a menu present then we need to
													// make sure that we leave room for the popup
													// glyph to the tight of the title
		if ( HasPopup ()) {
			outRect.right -= WantsLargePopupIndicator () ?
										bevelButton_LargePopupGlyphSize + 2:
											bevelButton_SmallPopupGlyphSize + 2;
		}

													// Apply the title offset based on the alignment
													// of the title
		switch ( mTitleAlignment ) {
			case kControlBevelButtonAlignTextFlushLeft:
				outRect.left += mTitleOffset;
			break;

			case kControlBevelButtonAlignTextFlushRight:
				outRect.right -= mTitleOffset;
			break;

			case kControlBevelButtonAlignTextSysDirection:
													//	еее NOTE: this will have to be fixed to check
													// the direction of the current script and then
													// do either the left or right flush as above,
													// for now it just defaults to left flush
				outRect.left += mTitleOffset;
			break;
		}

	} else {										// GRAPHICS AND TITLE


													// Figure out the width and height of the
													// graphic based on content type

		SInt16		graphicHeight = 32;				// Init to CIcon Content sizes
		SInt16		graphicWidth  = 32;

		if (HasIconSuiteContent()) {
			graphicHeight = mSizeSelector;
			graphicWidth  = mSizeSelector;
			if (graphicWidth == 12) {				// Mini icons are 12 x 16
				graphicWidth = 16;
			}

		} else if (HasPictContent()) {
			SDimension16 pictSize = GetPictureSize ();
			graphicHeight = pictSize.height;
			graphicWidth  = pictSize.width;

		}

													// Calculate the mid point based on the font
													// ascent
		SInt16	titleTop = (SInt16) (((outRect.bottom - outRect.top) - textHeight) / 2);

													// Get the local graphic rect which we want
													// to use for positioning the title in some
													// of the cases
		Rect	graphicRect;
		CalcLocalGraphicRect ( graphicRect );

													// Now we modify the title rect based on the
													// icon placement
		switch ( mTitlePlacement ) {
			case kControlBevelButtonPlaceSysDirection:
			break;

			case kControlBevelButtonPlaceNormally: {
													// Basically all we do is setup the title
													// rect so that it is centered vertically and
													// the full width of the button less the
													// bevels
				outRect.top += (( UGraphicUtils::RectHeight ( outRect ) -
											textBaseline) / 2) - 2;
				outRect.bottom = (SInt16) (outRect.top + textHeight);

				switch ( mTitleAlignment ) {
					case kControlBevelButtonAlignTextFlushRight:
						outRect.right -= mTitleOffset;
					break;

					case kControlBevelButtonAlignTextFlushLeft:
						outRect.left += mTitleOffset;
					break;
				}
			}
			break;

			case kControlBevelButtonPlaceToRightOfGraphic: {
				outRect.top += titleTop;
				outRect.bottom = (SInt16) (outRect.top + textHeight);
				outRect.left += graphicWidth + bevelButton_GraphicSlop;
													// If there is a menu present then we need to
													// make sure that we leave room for the popup
													// glyph to the tight of the title
				if ( HasPopup ()) {
					outRect.right -= WantsLargePopupIndicator () ?
												bevelButton_LargePopupGlyphSize + 2 :
													bevelButton_SmallPopupGlyphSize + 2;
				}
													// Depending on the alignment of the title we
													// will need to add or remove the title offset
													// in the appropriate place, NOTE: we will
													// need to add support for the default case
													// which will need to look at the script
													// direction in order to figure out where it
													// needs to be added
				if ( mTitleAlignment == kControlBevelButtonAlignTextFlushLeft ) {
					outRect.left += mTitleOffset;

				}else if ( mTitleAlignment == kControlBevelButtonAlignTextFlushRight ) {
					outRect.right -= mTitleOffset;
				}
			}
			break;

			case kControlBevelButtonPlaceToLeftOfGraphic: {
				outRect.top += titleTop;
				outRect.bottom = (SInt16) (outRect.top + textHeight);

													// We only handle the adjustment of the rect
													// if the alignment is not centered, for some
													// reason Apple chose to implement the title
													// centered as being centered on the entire
													// width of the control regardless of whether
													// a graphic is present or not
				if ( mTitleAlignment != kControlBevelButtonAlignTextCenter ) {
					outRect.right -= graphicWidth + bevelButton_GraphicSlop;

													// Depending on the alignment of the title we
													// will need to add or remove the title offset
													// in the appropriate place.
													// NOTE: we will need to add support for the
													// default case which will need to look at
													// the script direction in order to figure
													// out where it needs to be added
					if ( mTitleAlignment == kControlBevelButtonAlignTextFlushLeft ) {
						outRect.left += mTitleOffset;

					} else if ( mTitleAlignment == kControlBevelButtonAlignTextFlushRight ) {
						outRect.right -= mTitleOffset;
					}
				}
													// Before we proceed any further we are going
													// to check if the title rect is wide enough
													// to handle the title text, if it isn't and
													// there is enough room available we will
													// make it wider despite the fact that it
													// might overlap the graphic.  This was done
													// to match the way the Apple version of this control works
				SInt16	titleRectWidth = UGraphicUtils::RectWidth ( outRect );
				if ( titleRectWidth < titleWidth && buttonWidth > titleWidth ) {
					outRect.right = (SInt16) (outRect.left + titleWidth);
				}
			}
			break;

			case kControlBevelButtonPlaceBelowGraphic: {
				outRect.left += mTitleOffset;
				outRect.top = (SInt16) (graphicRect.bottom + bevelButton_GraphicSlop);
				outRect.bottom = (SInt16) (outRect.top + textHeight);
			}
			break;

			case kControlBevelButtonPlaceAboveGraphic: {
				outRect.left += mTitleOffset;
				outRect.bottom = (SInt16) (graphicRect.top - (bevelButton_GraphicSlop + 1));
				outRect.top = (SInt16) (outRect.bottom - textHeight);
			}
			break;
		}
	}

													// If the button is hilited and the offset flag
													// is set then we need to offset the rect by one
													// pixel to the bottom and right
	if ( IsPushed () && mOffsetContents ) {
		if ( HasTextOnly ()) {
			::MacOffsetRect ( &outRect, 1, 1 );

		} else if ( mTransform == kTransformSelected ) {
			::MacOffsetRect ( &outRect, 1, 1 );
		}
	}
}


// ---------------------------------------------------------------------------
//	е CalcLocalGraphicRect											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcLocalGraphicRect (
		Rect &outRect )
{
													// We always start by getting the graphic
													// location recalculated
	Point	graphicLocation;
	CalcGraphicLocation ( graphicLocation );

	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
		case kControlContentIconSuiteHandle: {
													// If we have an icon suite handle we figure out
													// the size of the rectangle it would occupy,
			if ( mIconSuiteH != nil ) {
													// Setup the rectangle based on the location and
													// size of the icon
				outRect.top = graphicLocation.v;
				outRect.left = graphicLocation.h;
				outRect.bottom = (SInt16) (graphicLocation.v + mSizeSelector);
				outRect.right = (SInt16) (outRect.left + (mSizeSelector == 12 ? 16 : mSizeSelector));
			}
		}
		break;

		case kControlContentCIconRes:
		case kControlContentCIconHandle: {
													// If we have an icon suite handle we figure out
													// the size of the rectangle it would occupy
			if ( mCIconH != nil ) {
													// Setup the rectangle based on the location and
													// size of the icon
				SDimension16	iconSize = GetCIconSize ();
				outRect.top = graphicLocation.v;
				outRect.left = graphicLocation.h;
				outRect.bottom = (SInt16) (graphicLocation.v + iconSize.height);
				outRect.right = (SInt16) (outRect.left + iconSize.width);
			}
		}
		break;

		case kControlContentPictRes:
		case kControlContentPictHandle: {
			SDimension16	pictSize = GetPictureSize ();

													// Setup the rectangle based on the location and
													// size of the icon
			outRect.top = graphicLocation.v;
			outRect.left = graphicLocation.h;
			outRect.bottom = (SInt16) (graphicLocation.v + pictSize.height);
			outRect.right = (SInt16) (outRect.left + pictSize.width);
		}
		break;

		default:
													// By default we simply return and empty rect
			outRect = Rect_0000;
	}

													// If we are hilited and the user has requested
													// that the icon be offset then handle it now
	if ( IsPushed () && mOffsetContents && mTransform == kTransformSelected ) {
		::MacOffsetRect ( &outRect, 1, 1 );
	}
}


// ---------------------------------------------------------------------------
//	е WantsLargePopupIndicator										  [public]
// ---------------------------------------------------------------------------

Boolean
LGABevelButtonImp::WantsLargePopupIndicator() const
{
													// If we are a text only popup then the arrow is
													// always large
	if ( HasTitle ()) {
		return true;

	} else {
		Rect localFrame;
		CalcLocalFrameRect ( localFrame );
		SInt16	width = (SInt16) (UGraphicUtils::RectWidth ( localFrame ) - ((mBevelWidth + 1) * 2));

													// The arrow size is determined by the amount of
													// available width around the icon, if it is a
													// large icon we go to the large arrow when the
													// width is 6 or greater, with the smaller icons
													// if it is 22 or greater.  This will probably
													// be different if there is no icon and also if
													// there is a different type of graphic
		if ( HasIconSuiteContent ()) {
			if ( mSizeSelector == 32 ) {
				return (width - 32) >= 6;
			} else if ( mSizeSelector < 32 ) {
				return (width - 16) >= 22;
			}
		} else if ( HasCIconContent ()) {
			return (width - 32) >= 6;
		} else if ( HasPictContent ()) {
			SDimension16	pictSize = GetPictureSize ();
			return (width - pictSize.width) >= 6;
		}
	}

	return false;

}


// ---------------------------------------------------------------------------
//	е GetMacMenuH													  [public]
// ---------------------------------------------------------------------------

MenuHandle
LGABevelButtonImp::GetMacMenuH() const
{
	MenuHandle	menuH = nil;

	LBevelButton	*button = dynamic_cast<LBevelButton*>(mControlPane);
	if (button) {
		menuH = button->GetMacMenuH();
	}

	return menuH;
}


// ---------------------------------------------------------------------------
//	е GetMenuValue													  [public]
// ---------------------------------------------------------------------------

SInt16
LGABevelButtonImp::GetMenuValue() const
{
	SInt16	menuValue = 0;

	LBevelButton	*button = dynamic_cast<LBevelButton*>(mControlPane);
	if (button) {
		menuValue = button->GetCurrentMenuItem();
	}

	return menuValue;
}


// ---------------------------------------------------------------------------
//	е SetMenuValue													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetMenuValue(
	SInt16		inItem)
{
	LBevelButton	*button = dynamic_cast<LBevelButton*>(mControlPane);
	if (button) {
		button->SetCurrentMenuItem(inItem);
	}
}


// ---------------------------------------------------------------------------
//	е GetCurrentItemTitle											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::GetCurrentItemTitle (
		Str255 outItemTitle )
{
													// Load the popup menu and then find the title
	MenuHandle	popupMenu = GetMacMenuH ();
	if ( popupMenu ) {
		::GetMenuItemText ( popupMenu, GetMenuValue(), outItemTitle );
	}
}


// ---------------------------------------------------------------------------
//	е GetMenuFontSize												  [public]
// ---------------------------------------------------------------------------

SInt16
LGABevelButtonImp::GetMenuFontSize () const
{
													// Get the font size for the popup so we can
													// figure out the correct mark to apply
	StTextState			theTextState;

	ResIDT	textTID = GetTextTraitsID ();
	TextTraitsH traitsH = UTextTraits::LoadTextTraits ( textTID );
	if (traitsH)  {
		return (**traitsH).size;
	}
													// Because we should never really get here we
													// simply return zero
	return 0;
}


// ---------------------------------------------------------------------------
//	е GetPopupMenuPosition											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::GetPopupMenuPosition(
	Point&	outPopupLoc )
{
													// Get the local popup button frame, this will
													// be correctly setup depending on whether the
													// button is arrow only or not
	Rect	popupRect;
	CalcLocalFrameRect ( popupRect );

	if ( !HasPopupToRight ()) {
		outPopupLoc.v = popupRect.bottom;
		outPopupLoc.h = popupRect.left;

	} else {
													// Setup the vertical value
		outPopupLoc.v = popupRect.top;

													// Setup the horizontal component which is
													// always at the right edge
		outPopupLoc.h = popupRect.right;
	}

													// Because PopMenuSelect needs the location for
													// the popup menu global coordinates we need to
													// convert the location
	mControlPane->LocalToPortPoint ( outPopupLoc );
	mControlPane->PortToGlobalPoint ( outPopupLoc );

}


// ---------------------------------------------------------------------------
//	е SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetDescriptor(
	ConstStringPtr	 inDescriptor)
{
	mTitle = inDescriptor;
	Refresh();
}


// ---------------------------------------------------------------------------
//	е SetTextTraitsID												  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetTextTraitsID(
	ResIDT	 inTextTraitsID)
{
	if (mTextTraitsID != inTextTraitsID) {
		mTextTraitsID = inTextTraitsID;
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	е SetResourceID													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetResourceID(
	ResIDT	inResID)
{
													// Setup the new resource ID
	mResourceID = inResID;
													// Now make sure that we load the appropriate
													// type of resource to match the new ID
	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
			SetIconResourceID ( mResourceID );
			break;

		case kControlContentCIconRes:
			SetCIconResourceID ( mResourceID );
			break;

		case kControlContentPictRes:
			SetPictureResourceID ( mResourceID );
			break;
	}
}


// ---------------------------------------------------------------------------
//	е SetBevelWidth													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetBevelWidth(
	SInt16		inBevelWidth,
	Boolean		inRedraw)
{
	if (mBevelWidth != inBevelWidth) {
		mBevelWidth = inBevelWidth;

		if (inRedraw) {
			mControlPane->Draw(nil);
		}
	}
}


// ---------------------------------------------------------------------------
//	е SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetPushedState (
		Boolean	inPushedState )
{
													// We only change the state if we have to
	if ( mPushed != inPushedState ) {
		mPushed = inPushedState;
													// Get the transform adjusted if needed
		AdjustTransform ( inPushedState );

													// Get the hiliting adjusted if needed
		AdjustTitleHiliting ( inPushedState );

													// Because the state has changed we need to make
													// sure  that things get redrawn so that the
													// change can take effect
		mControlPane->Draw ( nil );
	}
}


// ---------------------------------------------------------------------------
//	е PostSetValue													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::PostSetValue()
{
													// And make sure we get the hilite state changed
	SetPushedState ( mControlPane->GetValue () == Button_On );

													// Get the transform adjusted if needed
	AdjustTransform ();

													// Make sure the title hiliting is adjusted
	AdjustTitleHiliting ();

}


// ---------------------------------------------------------------------------
//	е SetupCurrentMenuItem											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetupCurrentMenuItem(
	MenuHandle		inMenuH,
	SInt16			inCurrentItem)
{
													// We need to be able to handle two different
													// approaches to the marking of the menu items,
													// 1) for the multivalue case we allow the user
													// to mark and unmark all the items in the menu,
													// 2) we handle the standard behavior of only
													// one item marked at a time in the menu
	if ( inMenuH ) {
		SInt16	mark;
		if ( HasMultiValueMenu ()) {
													// We start out by checking if the item is
													// marked or not, if it is not mark then the
													// mark is set to our mark otherwise it is left
													// at 0 which will result in the mark being
													// removed
			::GetItemMark ( inMenuH, inCurrentItem, &mark );
			if ( mark == 0 ) {
				mark = (SInt16) (GetMenuFontSize () < 12 ? bevelButton_SmallMark : checkMark);
			} else {
				mark = 0;
			}

		} else {
			SInt16		oldItem = GetMenuValue();
			if ( oldItem != inCurrentItem  ) {
													// Remove the old mark
				::SetItemMark ( inMenuH, oldItem, 0 );
			}
													// Always make sure the item is marked
			mark = (SInt16) (GetMenuFontSize () < 12 ? bevelButton_SmallMark : checkMark);
		}
													// Now we can finally get the mark set, this
													// will either mark it or not based on the value
													// for the mark
		::SetItemMark ( 	inMenuH,
								inCurrentItem,
								mark );

		SetMenuValue(inCurrentItem);
	}
}


// ---------------------------------------------------------------------------
//	е AdjustTitleHiliting											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::AdjustTitleHiliting(
	Boolean		inMouseInButton,
	Boolean 	inRedrawTitle)
{
													// We only bother doing this if we have a title
	if ( HasTitle ()) {
													// Adjust the transform as per the new state of
													// the button
		Boolean		oldHiliteTitle = mHiliteTitle;
		if ( IsEnabled () && IsActive () ) {
			if ( IsPushed () && !IsSelected ()) {
				mHiliteTitle = true;
			} else if ( IsSelected ()) {
				mHiliteTitle = inMouseInButton;
			} else {
				mHiliteTitle = false;
			}
													// If the hilite state has changed then we need
													// to make sure we get the title redrawn if
													// requested
			if ( oldHiliteTitle != mHiliteTitle && inRedrawTitle &&
						mControlPane->IsVisible () && mControlPane->FocusDraw () ) {

				Rect	frame;
				CalcLocalFrameRect(frame);
				StColorDrawLoop		drawLoop(frame);
				SInt16				depth;

				while (drawLoop.NextDepth(depth)) {
					WipeTitle ();
					DrawButtonTitle (depth);
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	е AdjustTransform												  [public]
// ---------------------------------------------------------------------------
//
//	This method is called from a number of different locations within this class and
// is the one place where all tweaks to the transform are handled.  Before this
//	method we were having to do adjusting allover the place now it is handled in a
// more centralized manner

void
LGABevelButtonImp::AdjustTransform(
	Boolean		inMouseInButton,
	Boolean 	inRedrawIcon)
{
													//ее NOTE: We are using the transform field of
													// the icon suite mixin for all of the content
													// types we just use it differently for the PICT
													// and CIcon cases

													// Adjust the transform as per the new state of the button
	SInt16		oldTransform = mTransform;
	if ( IsEnabled () && IsActive ()) {
		if ( IsPushed () && !IsSelected ()) {
			mTransform = kTransformSelected;
		} else if ( IsSelected ()) {
			mTransform = inMouseInButton ? kTransformSelected : kTransformNone;
		} else {
			mTransform = kTransformNone;
		}
	} else {
		mTransform = kTransformDisabled;
	}
													// If the state is being changed just so that
													// the transform will be updated then we only
													// need to get the graphic itself redrawn
	if ( oldTransform != mTransform && inRedrawIcon &&
								IsVisible () && mControlPane->FocusDraw () ) {
													// Before we do anything we need to wipe area
													// occupied by the graphic so that the old
													// position is erased

		Rect	frame;
		CalcLocalFrameRect(frame);
		StColorDrawLoop		drawLoop(frame);
		SInt16				depth;

		while (drawLoop.NextDepth(depth)) {
			WipeGraphic ();

			if ( mIconSuiteH != nil ) {
				DrawIconSuiteGraphic ();
			}

			if ( HasCIconContent () && mCIconH != nil ) {
				DrawCIconGraphic (depth);
			}

			if ( HasPictContent ()) {
				DrawPictGraphic (depth);
			}
		}
	}
}


#pragma mark -
#pragma mark === COLORS

// ---------------------------------------------------------------------------
//	е GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LGABevelButtonImp::GetForeAndBackColors(
	RGBColor	*outForeColor,
	RGBColor	*outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = Color_Black;
	}

	if (outBackColor != nil) {
		if ( IsEnabled () && IsActive()) {
			*outBackColor = IsPushed () ? UGAColorRamp::GetColor(colorRamp_Gray7) :
									UGAColorRamp::GetColor(colorRamp_Gray3);
		} else {
			*outBackColor = IsPushed() ? UGAColorRamp::GetColor(colorRamp_Gray4) :
									UGAColorRamp::GetColor(colorRamp_Gray2);
		}
	}

	return true;
}

#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	е EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::EnableSelf()
{
 	if (mControlPane->GetEnabledState() == triState_On) {
													// Get the transform adjusted if needed
		AdjustTransform(false, false);

													// Get the title hiliting adjusted to reflect
													// the new state
		AdjustTitleHiliting(false, false);

													// Redraw the button so the changes show up
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	е DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DisableSelf ()
{
 	if (!IsEnabled()) {
													// Get the transform adjusted if needed
		AdjustTransform(false, false);

													// Get the title hiliting adjusted to reflect
													// the new state
		AdjustTitleHiliting(false, false);

													// Get everything redrawn so the changes take
													// effect
		mControlPane->Draw(nil);
	}
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	е ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::ActivateSelf()
{
 	if (IsActive()) {
													// Get the transform adjusted if needed
		AdjustTransform(false, false);

													// Get the title hiliting adjusted to reflect
													// the new state
		AdjustTitleHiliting(false, false);

													// Redraw the buttons so the changes show up
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//	е DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DeactivateSelf()
{
 	if (!IsActive()) {
													// Get the transform adjusted if needed
		AdjustTransform(false, false);

													// Get the title hiliting adjusted to reflect
													// the new state
		AdjustTitleHiliting (false, false);

													// Redraw the buttons so the changes show up
		Refresh();
	}
}


#pragma mark -
#pragma mark === VISIBILITY

// ---------------------------------------------------------------------------
//	е ShowSelf
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::ShowSelf()
{
													// Get the transform adjusted if needed
	AdjustTransform(false, false);

													// Get the title hiliting adjusted to reflect
													// the new state
	AdjustTitleHiliting (false, false);
}


#pragma mark -
#pragma mark === DATA HANDLING

// ---------------------------------------------------------------------------
//	е SetDataTag
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::SetDataTag (
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {
	
//		case kControlBevelButtonMenuValueTag:
//		break;

//		case kControlBevelButtonMenuHandleTag:
//		break;

		case kControlBevelButtonContentTag: {
			ControlButtonContentInfo controlInfo = *(ControlButtonContentInfo *)inDataPtr;

													// Get the new values assigned where appropriate
													// NOTE: currently we are not handling the icon
													// ref case and for picts we are not caching the
													// PicHandle except when the handle based option
													// is being used
			mContentType = controlInfo.contentType;
			
			switch ( mContentType ) {
			
				case kControlContentCIconRes:
				case kControlContentIconSuiteRes:
				case kControlContentPictRes:
					SetResourceID ( controlInfo.u.resID );
				break;

				case kControlContentCIconHandle: {
													// We need to make sure that the resource ID is
													// zeroed out so that everything is setup for
													// handle based stuff
					SetCIconResourceID ( 0 );
					mResourceID = 0;

													// Now we can set the handle up in the cache
					SetCIconH ( controlInfo.u.cIconHandle );
				}
				break;

				case kControlContentIconSuiteHandle: {
													// We need to make sure that the resource ID is
													// zeroed out so that everything is setup for
													// handle based stuff
					SetIconResourceID ( 0 );
					mResourceID = 0;

													// Now we can set the handle up in the cache
					SetIconSuiteH ( controlInfo.u.iconSuite );

													// Figure out what size the icon suite will be
													// drawn at, either large, small or mini
					CalcIconSuiteSize ();

													// Make sure that the transform for the icon is
													// correctly setup
					AdjustTransform ( false, false );
				}
				break;

				case kControlContentPictHandle: {
													// To be safe when the picture handle is passed
													// into us we make sure that the resource ID is
													// set to zero so that we don't try to load a
													// picture from resources should something go
													// wrong with the handle we are handed
					SetPictureResourceID ( 0 );
					mResourceID = 0;

													// Now place the handle in the cache
					SetPictureHandle( controlInfo.u.picture );
				}
				break;
			}
													// Get the control refreshed so that the change
													// shows up
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonTransformTag: {
													// We only apply a transform change if we have a
													// content type of icon suite
			if ( HasIconSuiteContent ()) {
				SInt16	value = *(SInt16 *)inDataPtr;
				mTransform = value;
				mControlPane->Draw ( nil );
			}
		}
		break;

		case kControlBevelButtonTextPlaceTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mTitlePlacement = value;
			CalcIconSuiteSize();					// Title placement can affect icon size
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonTextAlignTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mTitleAlignment = value;
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonTextOffsetTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mTitleOffset = value;
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonGraphicAlignTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mGraphicAlignment = value;
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonGraphicOffsetTag: {
			Point	value = *(Point *)inDataPtr;
			mGraphicOffset = value;
			mControlPane->Draw ( nil );
		}
		break;

		case kControlBevelButtonCenterPopupGlyphTag: {
			Boolean	value = *(Boolean *)inDataPtr;
			mCenterPopupGlyph = value;
			mControlPane->Draw ( nil );
		}
		break;
		
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	е GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize ) const
{
	Size	dataSize = 0;
													// Handle getting the data for the various tags
													// supported  by the bevel button
	switch (inTag) {
	
//		case kControlBevelButtonMenuValueTag:
//		break;

//		case kControlBevelButtonMenuHandleTag:
//		break;

		case kControlBevelButtonContentTag: {
			ControlButtonContentInfo controlInfo;
			controlInfo.contentType = mContentType;
			
			switch (mContentType) {
			
				case kControlContentCIconRes:
					controlInfo.u.resID = mResourceID;
					break;

				case kControlContentIconSuiteRes:
					controlInfo.u.resID = mResourceID;
					break;

				case kControlContentPictRes:
					controlInfo.u.resID = mResourceID;
					break;

				case kControlContentCIconHandle:
					controlInfo.u.cIconHandle = HasCIconHandle () ? mCIconH : nil;
					break;

				case kControlContentIconSuiteHandle:
					controlInfo.u.iconSuite = HasIconSuiteHandle () ? mIconSuiteH : nil;
					break;

				case kControlContentPictHandle:
					controlInfo.u.picture = HasPictHandle () ? GetPictureHandle() : nil;
					break;
			}

			dataSize = sizeof ( controlInfo );
			*(ControlButtonContentInfo *)inBuffer = controlInfo;
		}
		break;

		case kControlBevelButtonTransformTag: {
													// We only return a transform if we have a
													// content type of icon suite res or icon suite
													// handle
			if ( HasIconSuiteContent ()) {
				dataSize = sizeof ( mTransform );
				*(SInt16 *)inBuffer =  mTransform;
			} else {
				dataSize = 0;
			}
		}
		break;

		case kControlBevelButtonTextPlaceTag: {
			dataSize = sizeof ( mTitlePlacement );
			*(SInt16 *)inBuffer =  mTitlePlacement;
		}
		break;

		case kControlBevelButtonTextAlignTag: {
			dataSize = sizeof ( mTitleAlignment );
			*(SInt16 *)inBuffer =  mTitleAlignment;
		}
		break;

		case kControlBevelButtonTextOffsetTag: {
			dataSize = sizeof ( mTitleOffset );
			*(SInt16 *)inBuffer =  mTitleOffset;
		}
		break;

		case kControlBevelButtonGraphicAlignTag: {
			dataSize = sizeof ( mGraphicAlignment );
			*(SInt16 *)inBuffer =  mGraphicAlignment;
		}
		break;

		case kControlBevelButtonGraphicOffsetTag: {
			dataSize = sizeof ( mGraphicOffset );
			*(Point *)inBuffer =  mGraphicOffset;
		}
		break;

		case kControlBevelButtonCenterPopupGlyphTag: {
			dataSize = sizeof ( mCenterPopupGlyph );
			*(Boolean *)inBuffer =  mCenterPopupGlyph;
		}
		break;
		
		default:
			LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			if (*outDataSize != nil) {
				dataSize = *outDataSize;
			}
			break;
	}
														// If we are being asked to return the data
														// size then handle that now
	if ( outDataSize != nil ) {
		*outDataSize = dataSize;
	}
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	е DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawSelf()
{
	StColorPenState::Normalize ();

													// Get the control drawn in its various states
	if ( IsEnabled () && IsActive ()) {
		if ( IsPushed ()) {
			DrawBevelButtonPushed ();
		} else {
			DrawBevelButtonNormal ();
		}
	} else {
		DrawBevelButtonDimmed ();
	}
													// If we have a icon suite graphic type then
													// draw it now
	if ( !HasTextOnly ()) {
		DrawGraphic (mControlPane->GetBitDepth());
	}
													// If we have a title then make sure it gets
													// drawn now
	if ( HasTitle ()) {
		DrawButtonTitle (mControlPane->GetBitDepth());
	}
													// The last thing we need to get drawn is the
													// popup indicator, which is only drawn if we
													// are a popup
	if ( HasPopup ()) {
		DrawPopupGlyph ();
	}
}


// ---------------------------------------------------------------------------
//	е DrawGraphic													  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawGraphic(
	SInt16	inDepth)
{
	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
		case kControlContentIconSuiteHandle:
			DrawIconSuiteGraphic();
		break;

		case kControlContentCIconRes:
		case kControlContentCIconHandle:
			DrawCIconGraphic(inDepth);
		break;

		case kControlContentPictRes:
		case kControlContentPictHandle:
			DrawPictGraphic(inDepth);
		break;
	}
}


// ---------------------------------------------------------------------------
//	е DrawControlNormal												  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonNormal()
{
	if ( mControlPane->GetBitDepth() < 4 ) {	// BLACK & WHITE
		DrawBevelButtonNormalBW ();

	} else {									// COLOR
		DrawBevelButtonNormalColor ();
	}
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonPushed											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonPushed()
{
	if ( mControlPane->GetBitDepth() < 4 ) {
		DrawBevelButtonPushedBW ();

	} else {									// COLOR
		DrawBevelButtonPushedColor ();
	}
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonDimmed											  [public]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonDimmed()
{
	if ( mControlPane->GetBitDepth() < 4 ) {	// BLACK & WHITE
		DrawBevelButtonDimmedBW ();

	} else {									// COLOR
		DrawBevelButtonDimmedColor ();
	}
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonNormalBW									   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonNormalBW()
{
	StColorPenState::Normalize();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	::EraseRect ( &localFrame );
	::MacFrameRect ( &localFrame );
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonPushedBW									   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonPushedBW ()
{
	StColorPenState::Normalize ();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	::PaintRect ( &localFrame );
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonDimmedBW									   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonDimmedBW()
{
	StColorPenState::Normalize();

	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

	StColorPenState::SetGrayPattern();
	::MacFrameRect ( &localFrame );			// Draw border with dotted line

	::MacInsetRect(&localFrame, 1, 1);

	if (IsPushed()) {
		::PaintRect(&localFrame);			// Gray interior when pushed

	} else {
		::EraseRect(&localFrame);			// White interior
	}
}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonNormalColor								   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonNormalColor ()
{
	StColorPenState::Normalize ();

	RGBColor	tempColor;
													// Get the bevel width
	SInt16		bevelWidth = GetBevelWidth ();

													// Setup a color array that will handle the
													// number of bevels
	RGBColor colorArray[3];
													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

													// FRAME BUTTON
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray9);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

	tempColor = UGAColorRamp::GetColor(colorRamp_GrayA1);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

													// TOP RIGHT
	tempColor = (bevelWidth == 2 ? UGAColorRamp::GetColor(colorRamp_Gray9) :
															UGAColorRamp::GetColor(colorRamp_Gray10));
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 1),
							localFrame.top,
							tempColor );

													// BOTTOM LEFT
	UGraphicUtils::PaintColorPixel (
							localFrame.left,
							(SInt16) (localFrame.bottom - 1),
							tempColor );

													// FACE COLOR
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray3);
	::RGBForeColor ( &tempColor );
	::PaintRect ( &localFrame );

													// LIGHT BEVELS
													// Setup Colors
	switch ( bevelWidth ) {
		case 1:
			colorArray[0] = Color_White;
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray3);
			colorArray[1] = Color_White;
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray3);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray1);
			colorArray[2] = Color_White;
		break;

	}

													// Draw top and left edges
	UGraphicUtils::BevelledEdge (
								localFrame,
								rectEdges_TopLeft,
								colorArray,
								bevelWidth );

													// SHADOW BEVELS
													// Setup Colors
	switch ( bevelWidth ) {
		case 1:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray7);
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray8);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray6);
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray10);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray8);
			colorArray[2] = UGAColorRamp::GetColor(colorRamp_Gray6);
		break;

	}

													// Draw bottom and right edges
	UGraphicUtils::BevelledEdge (
								localFrame,
								rectEdges_BottomRight,
								colorArray,
								bevelWidth );

													// CORNER PIXELS
	switch ( bevelWidth ) {
		case 1:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray3);
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray5);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray3);
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray5);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray4);
			colorArray[2] = UGAColorRamp::GetColor(colorRamp_Gray3);
		break;
	}

													// Paint corner pixels
													// TOP RIGHT
	UGraphicUtils::PaintCornerPixels (
								localFrame,
								rectCorners_TopRight,
								colorArray,
								bevelWidth );

													// BOTTOM LEFT
	UGraphicUtils::PaintCornerPixels (
								localFrame,
								rectCorners_BottomLeft,
								colorArray,
								bevelWidth );

}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonPushedColor								   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonPushedColor ()
{

	StColorPenState::Normalize ();

	RGBColor	tempColor;
													// Get the bevel width
	SInt16		bevelWidth = GetBevelWidth ();
													// Setup a color array that will handle the
													// number of bevels
	RGBColor colorArray[3];

	Rect localFrame;
	CalcLocalFrameRect ( localFrame );

													// FRAME BUTTON
	tempColor = UGAColorRamp::GetColor(colorRamp_GrayA2);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 1, 1 );

	tempColor = UGAColorRamp::GetColor(colorRamp_Gray11);
	::RGBForeColor ( &tempColor );
	UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

													// TOP RIGHT
	tempColor = (bevelWidth == 2 ? UGAColorRamp::GetColor(colorRamp_GrayA2) :
															UGAColorRamp::GetColor(colorRamp_Gray12));
	UGraphicUtils::PaintColorPixel (
							(SInt16) (localFrame.right - 1),
							localFrame.top,
							tempColor );

													// BOTTOM LEFT
	UGraphicUtils::PaintColorPixel ( localFrame.left,
							(SInt16) (localFrame.bottom - 1),
							tempColor );

													// FACE COLOR
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
	::RGBForeColor ( &tempColor );
	::PaintRect ( &localFrame );

													// LIGHT BEVELS
													// Setup Colors
	switch ( bevelWidth ) {
		case 1:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray10);
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray11);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray9);
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_GrayA1);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray11);
			colorArray[2] = UGAColorRamp::GetColor(colorRamp_Gray9);
		break;
	}

													// Draw top left edges
	UGraphicUtils::BevelledEdge (
								localFrame,
								rectEdges_TopLeft,
								colorArray,
								bevelWidth );

													// SHADOW BEVELS
													// Setup Colors
	switch ( bevelWidth ) {
		case 1:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray5);
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray4);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray6);
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray4);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray5);
			colorArray[2] = UGAColorRamp::GetColor(colorRamp_Gray6);
		break;
	}

													// Draw bottom right edges
	UGraphicUtils::BevelledEdge (
								localFrame,
								rectEdges_BottomRight,
								colorArray,
								bevelWidth );

													//е CORNER PIXELS
													// TOP RIGHT
	switch ( bevelWidth ){
		case 1:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray8);
		break;

		case 2:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray8);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray7);
		break;

		case 3:
			colorArray[0] = UGAColorRamp::GetColor(colorRamp_Gray11);
			colorArray[1] = UGAColorRamp::GetColor(colorRamp_Gray8);
			colorArray[2] = UGAColorRamp::GetColor(colorRamp_Gray7);
		break;
	}

													// CORNER PIXELS
													// TOP RIGHT
	UGraphicUtils::PaintCornerPixels (
								localFrame,
								rectCorners_TopRight,
								colorArray,
								bevelWidth );

													// BOTTOM LEFT
	UGraphicUtils::PaintCornerPixels (
								localFrame,
								rectCorners_BottomLeft,
								colorArray,
								bevelWidth );

}


// ---------------------------------------------------------------------------
//	е DrawBevelButtonDimmedColor								   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawBevelButtonDimmedColor ()
{

	StColorPenState::Normalize ();

	RGBColor	tempColor;

	SInt16		bevelWidth = GetBevelWidth ();

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );

													// DISABLED NORMAL STATE
	if ( !IsPushed ()) {
													// FRAME BUTTON
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 0, 0 );


		tempColor = (bevelWidth == 3 ? UGAColorRamp::GetColor(colorRamp_Gray7) :
													UGAColorRamp::GetColor(colorRamp_Gray6));
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );


													// TOP RIGHT
		if ( bevelWidth == 3 ) {
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 1),
									localFrame.top,
									tempColor );

													// BOTTOM LEFT
			UGraphicUtils::PaintColorPixel (
									localFrame.left,
									(SInt16) (localFrame.bottom - 1),
									tempColor );
		}

	} else {										// DISABLED HILITED STATE
													// FRAME BUTTON
		tempColor = UGAColorRamp::GetColor(colorRamp_Gray8);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::TopLeftSide ( localFrame, 0, 0, 0, 0 );

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray6);
		::RGBForeColor ( &tempColor );
		UGraphicUtils::BottomRightSide ( localFrame, 1, 1, 0, 0 );

		if ( bevelWidth != 2 ) {
													// TOP RIGHT
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray7);
			UGraphicUtils::PaintColorPixel (
									(SInt16) (localFrame.right - 1),
									localFrame.top,
									tempColor );

													// BOTTOM LEFT
			UGraphicUtils::PaintColorPixel (
									localFrame.left,
									(SInt16) (localFrame.bottom - 1),
									tempColor );
		}
	}
													// Paint the face of the button
	::MacInsetRect ( &localFrame, 1, 1 );
	tempColor = ( !IsPushed () ? UGAColorRamp::GetColor(colorRamp_Gray2) :
									UGAColorRamp::GetColor(colorRamp_Gray4));
	::RGBForeColor ( &tempColor );
	::PaintRect ( &localFrame );

}


// ---------------------------------------------------------------------------
//	е DrawPopupGlyph											   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawPopupGlyph ()
{

													// Get the frame for the control
	Rect	localFrame;
	CalcLocalFrameRect ( localFrame );
	SInt16	height = UGraphicUtils::RectHeight ( localFrame );

	RGBColor			indicatorColor = Color_Black;

	if (mControlPane->GetBitDepth() < 4) {					// BLACK & WHITE
												// We only draw the indicator when we are not
												// hilited as the entire button including the
												// indicator is inverted for the hilited state
												// We draw the indicator in black when normal,
												// white when hilited and with a gray pattern
												// when the button is dimmed
		indicatorColor = (IsPushed () && IsEnabled ()) ? Color_White : Color_Black;
		if ( !IsEnabled ()) {
			StColorPenState::SetGrayPattern();
		}

	} else {									// COLOR
												// We draw the indicator in black when normal
												// gray 8 when dimmed and white when hilited
		indicatorColor = Color_Black;
		if ( !IsEnabled () || !IsActive ()) {
			 indicatorColor = UGAColorRamp::GetColor(8);
		}

		if ( IsPushed ()) {
			indicatorColor = UGAColorRamp::GetColor(8);
			if (IsEnabled()) {
				indicatorColor = Color_White;
			}
		}
	}

												// Set the foreground color for our drawing
	::RGBForeColor ( &indicatorColor );

												// Setup a bunch of variables that are going to
												// be used to draw the glyph that take into
												// account the size and the orientation
	SInt16	startVPosition, startHPosition, endHPosition, endVPosition;
	SInt16	size, rows;
	size = (WantsLargePopupIndicator () ? bevelButton_LargePopupGlyphSize :
											bevelButton_SmallPopupGlyphSize);
	rows = (WantsLargePopupIndicator () ? bevelButton_LargePopupGlyphRows :
											bevelButton_SmallPopupGlyphRows);

	if ( !HasPopupToRight ()) {		// DOWN

		if ( WantsPopupGlyphCentered ()) {
			startVPosition = (SInt16) (localFrame.top + (((height - rows) / 2) + 1));
		} else {
			startVPosition = (SInt16) (localFrame.bottom - (GetBevelWidth () + 2 + rows));
		}
		startHPosition = (SInt16) (localFrame.right - (GetBevelWidth () + 2 + size));
		endHPosition = (SInt16) (startHPosition + size);

	} else {									// RIGHT

		if ( WantsPopupGlyphCentered ()) {
			startVPosition = (SInt16) (localFrame.top + (((height - size) / 2) + 1));
		} else {
			startVPosition = (SInt16) (localFrame.bottom - (GetBevelWidth () + 2 + size));
		}
		startHPosition = (SInt16) (localFrame.right - (GetBevelWidth () + 2 + rows));
		endVPosition = (SInt16) (startVPosition + size);
	}
												// The glyph is drawn in the bottom right
												// corner of the button
	SInt16 index;
	for ( index = 0; index < rows; index++ ) {
		::MoveTo ( 	(SInt16) (startHPosition + index),
						(SInt16) (startVPosition + index));
		if ( !HasPopupToRight ()) {
			::Line ( (SInt16) (size - (1 + (2 * index))), 0 );
		} else {
			::Line ( 0, (SInt16) (size - (1 + (2 * index))));
		}
	}
}


// ---------------------------------------------------------------------------
//	е DrawButtonTitle											   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawButtonTitle(
	SInt16	inDepth)
{

	StColorPenState	theColorPenState;
	StTextState			theTextState;
	StColorPenState::Normalize ();
													// Save off the clipping as we are going to mess
													// with it here
	StClipRgnState		theClipState;
													// Get the clipping modified so that the buttons
													// bevels are clipped out, so the title cannot
													// draw over the bevels
	ClipToBevelButtonFace ();
													// Get some loal variables setup including the
													// rect for the title
	ResIDT	textTID = GetTextTraitsID ();
	Rect		titleRect;
													// Setup the port text style from the text traits
	UTextTraits::SetPortTextTraits ( textTID );

													// Calculate the title rect
	CalcTitleRect ( titleRect );

													// Save the primary text color
	RGBColor	textColor;
	::GetForeColor ( &textColor );

													// Get the title
	Str255 controlTitle;
	GetDescriptor ( controlTitle );

	if ( inDepth < 4 ) {
												// If the control is dimmed then we use the
												// grayishTextOr transfer mode to draw the text
		if ( !IsEnabled ()) {
			textColor = Color_Black;
			::RGBForeColor ( &textColor );
			::TextMode ( grayishTextOr );

		} else if ( IsEnabled () && IsPushed () ) {
												// When we are hilited we simply draw the title in white
			textColor = Color_White;
			::RGBForeColor ( &textColor );
		}
												// Now get the actual title drawn with all the appropriate settings
		UTextDrawing::DrawTruncatedWithJust (
										controlTitle,
										titleRect,
										mTitleAlignment,
										true );

	} else {
												// If control is selected we always draw the text
												// in the title hilite color, if requested
		if ( IsPushed () && GetHiliteTitle ()) {
			textColor = Color_White;
			::RGBForeColor ( &textColor );
		}
												// If the control is dimmed then we have to do
												// our own version of the grayishTextOr as it
												// does not appear to work correctly across
												// multiple devices
		if ( !IsEnabled () && !IsPushed () || !IsActive ()) {
			UTextDrawing::DimTextColor ( textColor );
			::TextMode ( srcOr );
			::RGBForeColor ( &textColor );
		}

												// Now get the actual title drawn with all the
												// appropriate settings
		UTextDrawing::DrawTruncatedWithJust (
										controlTitle,
										titleRect,
										mTitleAlignment,
										true );
	}
}


// ---------------------------------------------------------------------------
//	е DrawIconSuiteGraphic										   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawIconSuiteGraphic ()
{

	StColorPenState	theColorPenState;
	theColorPenState.Normalize ();
													// Get the icon rectangle
	Rect iconRect;
	CalcLocalGraphicRect ( iconRect );

	SInt16	iconAlign;
	switch ( mGraphicAlignment ) {
		case kControlBevelButtonAlignSysDirection:
			iconAlign = kAlignAbsoluteCenter;
		break;

		case kControlBevelButtonAlignCenter:
			iconAlign = kAlignAbsoluteCenter;
		break;

		case kControlBevelButtonAlignLeft:
			iconAlign = kAlignVerticalCenter;
		break;

		case kControlBevelButtonAlignRight:
			iconAlign = kAlignVerticalCenter;
		break;

		case kControlBevelButtonAlignTop:
			iconAlign = kAlignCenterTop;
		break;

		case kControlBevelButtonAlignBottom:
			iconAlign = kAlignCenterBottom;
		break;

		default:
			iconAlign = kAlignNone;
		break;
	}

													// Save off the current clipping as we are going
													// to mess with it here
	StClipRgnState		theClipState;

													// Get the clipping modified so that the buttons
													// bevels are clipped out, so the graphic cannot
													// draw over the bevels
	ClipToBevelButtonFace ();

													//  Now we plot the icon by calling the toolbox
													// passing it our cached handle to the icon suite
	if ( GetIconSuiteH ()) {
		::PlotIconSuite ( &iconRect, iconAlign, mTransform, GetIconSuiteH () );
	}
}


// ---------------------------------------------------------------------------
//	е DrawCIconGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawCIconGraphic(
	SInt16	inDepth)
{
	Rect iconRect;
	CalcLocalGraphicRect(iconRect);

	StClipRgnState		saveClip;
	ClipToBevelButtonFace();				// Don't draw over bevels

	UGraphicUtils::DrawCIconGraphic(mCIconH, iconRect, mTransform, inDepth);
}


// ---------------------------------------------------------------------------
//	е DrawPictGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DrawPictGraphic(
	SInt16	inDepth)
{
	Rect pictureRect;
	CalcLocalGraphicRect(pictureRect);

	StClipRgnState	saveClip;
	ClipToBevelButtonFace ();				// Don't draw over bevels

	PicHandle	thePictH = GetPictureHandle();

	UGraphicUtils::DrawPictGraphic(thePictH, pictureRect, mTransform, inDepth);
}


// ---------------------------------------------------------------------------
//	е WipeGraphic												   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::WipeGraphic ()
{
													// What we are going to do in this function is
													// to simply erase the background where the
													// graphic is located to the current background
													// color, this effectively erases the graphic
													// with a  minimal amount of flicker, when we do
													// this we also adjust the clipping to the face
													// of the button as we don't want to be erasing
													// anywhere in the buttons bevel edges
	if ( IsVisible () && mControlPane->FocusDraw ()) {
		StColorPenState	theColorPenState;
		theColorPenState.Normalize ();

		Rect graphicRect;
		CalcLocalGraphicRect ( graphicRect );
		::MacInsetRect ( &graphicRect, -1, -1 );

													// Get the colors applied
		ApplyForeAndBackColors ();
													// Save off the current clipping as we are going
													// to mess with it here
		StClipRgnState		theClipState;
													// Get the clipping modified so that the buttons
													// bevels are clipped out, so that we don't erase
													// any of the bevels
		ClipToBevelButtonFace ();
													// Now we can handle the erasing
		::EraseRect ( &graphicRect );
	}
}


// ---------------------------------------------------------------------------
//	е WipeTitle													   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::WipeTitle ()
{
													// What we are going to do in this function is
													// to simply erase the background where the title
													// is located to the current background color,
													// this effectively erases the title with a
													// minimal amount of flicker, when we do this we
													// also adjust the clipping to the face of the
													// button as we don't want to be erasing anywhere
													// in the buttons bevel edges
	if ( IsVisible () && mControlPane->FocusDraw ()) {
		StColorPenState	theColorPenState;
		theColorPenState.Normalize ();

		Rect titleRect;
		CalcTitleRect ( titleRect );
		::MacInsetRect ( &titleRect, -1, -1 );
													// Get the colors applied
		ApplyForeAndBackColors ();
													// Save off the current clipping as we are going
													// to mess with it here
		StClipRgnState		theClipState;
													// Get the clipping modified so that the buttons
													// bevels are clipped out, so that we don't
													// erase any of the bevels
		ClipToBevelButtonFace ();
													// Now we can handle the erasing
		::EraseRect ( &titleRect );
	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	е TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------
//

Boolean
LGABevelButtonImp::TrackHotSpot (
		SInt16		inHotSpot,
		Point 	inPoint,
		SInt16		inModifiers )
{

													// If we have a popup then we need to handle
													// things a little differently than we do just
													// for a button, if there is no popup we simply
													// let our superclass handle things
	if ( HasPopup ()) {
		MenuHandle	menuH = GetMacMenuH ();

													// We only want the popup menu to appear if the
													// mouse went down in the our hot spot which is
													// the popup portion of the control
		if ( PointInHotSpot ( inPoint, inHotSpot ) && menuH != nil ) {
													// Get things started off on the right foot
			Boolean		currInside = true;
			Boolean		prevInside = false;
			HotSpotAction ( inHotSpot, currInside, prevInside );

													// We skip the normal tracking that is done in
													// the control as the call to PopupMenuSelect
													// will take control of the tracking once the
													// menu is up.
													// Now we need to handle the display of the
													// actual popup menu we start by setting up some
													// values that we will need
			SInt16	menuID = 0;
			SInt16 menuItem = GetMenuValue();
			SInt16	currItem = 1;
			Point popLoc;
			GetPopupMenuPosition ( popLoc );

													// Call our utility function which handles the
													// display of the menu, menu is disposed of
													// inside this function
			HandlePopupMenuSelect ( popLoc, currItem, menuID, menuItem );

													// Setup the current item field with the newly
													// selected item
			if ( menuItem > 0 ) {
				SetupCurrentMenuItem (  menuH, menuItem );

													// NOTE: this was a call to SetValue but I
													// changed it to a PokeValue because it was
													// resulting in a double broadcast of the value
													// message  which occurs because the default
													// behavior of a pushbutton is to just broadcast
													// its value message when the button is pushed a
													// popup bevel button always has the pushbutton
													// or toggle behavior, this gets around the
													// problem
				mControlPane->PokeValue ( menuItem );
			}

													// Make sure that we get the HotSpotAction
													// called one last time so that the pushed state
													// is cleared
			HotSpotAction ( inHotSpot, false, true );

													// Return true if an item was selected
			return menuItem > 0;
		}

	} else {
		return LGAControlImp::TrackHotSpot ( inHotSpot, inPoint, inModifiers );
	}

	return false;

}



// ---------------------------------------------------------------------------
//	е HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::HotSpotAction	(
		SInt16		/* inHotSpot */,
		Boolean	inCurrInside,
		Boolean	inPrevInside )
{

													// If the mouse moved in or out of the hot spot
													// handle the hiliting of the control which is
													// slightly different for each of the modes
													// supported by the control
	if ( inCurrInside != inPrevInside ) {
		switch ( GetControlBehavior ()) {
			case kControlBehaviorPushbutton: {
													// For a straight button all we have to worry
													// about is switching the hiliting of the button
				SetPushedState ( inCurrInside );
			}
			break;

			case kControlBehaviorToggles: {
													// If the button is a toggle and it is currently
													// selected then we only need to mess with the
													// transform of the icon or the hiliting of the
													// title or both depending on the settings. If
													// we are not currently selected then we just
													// go ahead and  call SetPushedState
				if ( IsSelected ()) {
					AdjustTransform ( inCurrInside );
					AdjustTitleHiliting ( inCurrInside );
				} else {
					SetPushedState ( inCurrInside );
				}
			}
			break;

			case kControlBehaviorSticky: {
													// Things are a little trickier if we are a
													// radio button as we only need to toggle the
													// state if the control is not already selected,
													// we also want to avoid doing any unnecessary
													// drawing

				if ( inCurrInside ) {
					if ( !IsPushed ())
						SetPushedState ( true );
				} else if ( !IsSelected ()) {
					SetPushedState ( false );
				}
													// Get the transform adjusted if needed
				AdjustTransform ( inCurrInside );

													// Make sure we get the transform for the icon
													// adjusted to reflect the current state of the
													// button
				AdjustTitleHiliting ( inCurrInside );
			}
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	е DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::DoneTracking	(
		SInt16		inHotSpot,
		Boolean	inGoodTrack )
{
													// Make sure that by default we turn the
													// hiliting off if we are in button or radio
													// mode, switch mode is handled in the
													// HotSpotAction method
	if ( mBehavior == kControlBehaviorPushbutton ) {
		HotSpotAction ( inHotSpot, false, true );

	} else if ( mBehavior == kControlBehaviorToggles ) {
		if ( inGoodTrack )
			mControlPane->SetValue(1 - mControlPane->GetValue());

	} else if ( mBehavior == kControlBehaviorSticky ) {
													// Get the transform adjusted if needed
		AdjustTransform ();

													// Make sure we get the transform for the icon
													// adjusted to reflect the current state of the
													// button
		AdjustTitleHiliting ();
	}
}


// ---------------------------------------------------------------------------
//	е HandlePopupMenuSelect										   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::HandlePopupMenuSelect	(
		Point			inPopupLoc,
		SInt16			inCurrentItem,
		SInt16			&outMenuID,
		SInt16			&outMenuItem )
{
													// Load the menu from the resource
	MenuHandle	menuH = GetMacMenuH ();
	if ( menuH )
	{
													// Save off the current system font family and
													// size
		SInt16 saveFont = ::LMGetSysFontFam ();
		SInt16 saveSize = ::LMGetSysFontSize ();

													// Enclose this all in a try catch block so that
													// we can at least reset the system font if
													// something goes wrong
		try  {
													// Handle the actual insertion into the hierarchical menubar
			::MacInsertMenu ( menuH, hierMenu );

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
													// Before we display the menu we need to make
													// sure that we have the current item marked in
													// the menu. NOTE: we do NOT use the current
													// item that has been passed in here as that
													// always has a value of one as our menus are
													// always pulldown menus.  We only do this if
													// the menu is not a multi value menu
			if ( !HasMultiValueMenu ()) {
				SetupCurrentMenuItem ( 	menuH,
												GetMenuValue() );
			}
													// Then we call PopupMenuSelect and wait for it
													// to return
			SInt32 result = ::PopUpMenuSelect ( 	menuH,
															inPopupLoc.v,
															inPopupLoc.h,
															inCurrentItem );

													// Then we extract the values from the returned
													// result these are then passed back out to the
													// caller
			outMenuID = HiWord ( result );
			outMenuItem = LoWord ( result );

		} catch (...)  { }
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
//	е CalcGraphicLocation
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcGraphicLocation (
		Point	&outLocation )
{

	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
		case kControlContentCIconRes:
		case kControlContentIconSuiteHandle:
		case kControlContentCIconHandle:
			CalcIconLocation ( outLocation );
		break;

		case kControlContentPictRes:
		case kControlContentPictHandle:
			CalcPictureLocation ( outLocation );
		break;
	}
}


// ---------------------------------------------------------------------------
//	е CalcIconLocation
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcIconLocation (
		Point	&outIconLocation )
{

	Rect		localFrame;
	SInt16		width, height;
	SInt16		bevelWidth = (SInt16) (GetBevelWidth () + 1); 	// We add one for the border that way
																//		the icon will always be setup
																//		so that it is not drawn over
																//		an edge

													// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, bevelWidth, bevelWidth );
	outIconLocation.h = localFrame.left;
	outIconLocation.v = localFrame.top;

													// Setup the width and height values
	width  = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SDimension16	iconSize = GetCIconSize ();
	SInt16	iconHeight = (SInt16) (HasIconSuiteContent () ? mSizeSelector : iconSize.height);
	SInt16	iconWidth  = (SInt16) (HasIconSuiteContent () ? (mSizeSelector == 12 ?
										16 : mSizeSelector) : iconSize.width);

													// Now we calculate the location of the icon
													// based on the graphic alignment
	switch ( mGraphicAlignment ) {
		case kControlBevelButtonAlignSysDirection: {
			outIconLocation.h += mGraphicOffset.h;
			outIconLocation.v += ( height / 2 ) - ( iconHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignCenter: {
			SInt16	locH = (SInt16) ((width - iconWidth) / 2);
			SInt16	locV = (SInt16) ((height - iconHeight) / 2);
			if ( HasTitle ()) {
				if ( mTitlePlacement == kControlBevelButtonPlaceAboveGraphic ) {
					locV = (SInt16) ((((height - (bevelButton_GraphicSlop +
										CalcTitleHeight () + iconHeight)) / 2) +
											(bevelButton_GraphicSlop + CalcTitleHeight ())) - 1);

				} else if ( mTitlePlacement == kControlBevelButtonPlaceBelowGraphic ) {
					locV = (SInt16) (( height - (bevelButton_GraphicSlop +
										CalcTitleHeight () + iconHeight)) / 2);
				}
			}
													// Now set the location
			outIconLocation.h += locH;
			outIconLocation.v += locV;
		}
		break;

		case kControlBevelButtonAlignLeft: {
			outIconLocation.h += mGraphicOffset.h;
			outIconLocation.v += ( height / 2 ) - ( iconHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignRight: {
			outIconLocation.h += width - ( iconWidth + mGraphicOffset.h );
			outIconLocation.v += ( height / 2 ) - ( iconHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignTop: {
			outIconLocation.h += ( width / 2 ) - ( iconWidth / 2 );
			outIconLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottom: {
			outIconLocation.h += ( width / 2 ) - ( iconWidth / 2 );
			outIconLocation.v += height - ( iconHeight + mGraphicOffset.v );
		}
		break;

		case kControlBevelButtonAlignTopLeft: {
			outIconLocation.h += mGraphicOffset.h;
			outIconLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottomLeft: {
			outIconLocation.h += mGraphicOffset.h;
			outIconLocation.v += height - ( iconHeight + mGraphicOffset.v );
		}
		break;

		case kControlBevelButtonAlignTopRight: {
			outIconLocation.h += width - ( iconWidth + mGraphicOffset.h );
			outIconLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottomRight: {
			outIconLocation.h += width - ( iconWidth + mGraphicOffset.h );
			outIconLocation.v += height - ( iconHeight + mGraphicOffset.v );
		}
		break;
	}
}


// ---------------------------------------------------------------------------
//	е CalcIconSuiteSize											   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcIconSuiteSize()
{
													// If the content is an icon suite then we need
													// to get all the appropriate data setup for
													// that type
	if (HasIconSuiteContent()) {
													// Here we are going to attempt to figure out
													// what size icon we are going to be drawing
		Rect	localFrame;
		CalcLocalFrameRect ( localFrame );

		SInt16		bevelHSlop = (SInt16) (((mBevelWidth + 1) * 2) + mGraphicOffset.h);
		SInt16		bevelVSlop = (SInt16) (((mBevelWidth + 1) * 2) + mGraphicOffset.v);

													// If we have a title then depending on the
													// placement of the title we need to potentially
													// adjust the slop to take into the title offset
													// and the
		if ( HasTitle ()) {
			SInt16	titleWidth = CalcTitleWidth ();
			SInt16	titleHeight = CalcTitleHeight ();

			switch (mTitlePlacement) {

				case kControlBevelButtonPlaceToRightOfGraphic:
				case kControlBevelButtonPlaceToLeftOfGraphic:
				case kControlBevelButtonPlaceSysDirection:
				case kControlBevelButtonPlaceNormally:
					bevelHSlop += mTitleOffset + titleWidth;
					break;

				case kControlBevelButtonPlaceBelowGraphic:
				case kControlBevelButtonPlaceAboveGraphic:
					bevelVSlop += mTitleOffset + titleHeight;
					break;
			}
		}

													// Now we can figure out the width and height
													// that will be used to figure out the icon size
		SInt16	width  = (SInt16) (UGraphicUtils::RectWidth ( localFrame ) - bevelHSlop);
		SInt16	height = (SInt16) (UGraphicUtils::RectHeight ( localFrame ) - bevelVSlop);

													// Now that we have filtered out all of the
													// dependencies we can figure out what size the
													// icon should be, anything over 32 in width and
													// height is a large icon, below 16 in width or
													// height is a mini icon, and the rest is a
													// small icon
		if ( width >= 32 && height >= 32 ) {
			mSizeSelector = sizeSelector_LargeIconSize;

		} else {
			if ( width < 16 || height < 16 ) {
				mSizeSelector = sizeSelector_MiniIconSize;
			} else {
				mSizeSelector = sizeSelector_SmallIconSize;
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	е CalcPictureLocation										   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::CalcPictureLocation (
		Point	&outPictureLocation )
{

	Rect		localFrame;
	SInt16		width, height;
	SInt16		bevelWidth = (SInt16) (GetBevelWidth () + 1); 	// We add one for the border that way
																//		the icon will always be setup
																//		so that it is not drawn over
																//		an edge

													// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, bevelWidth, bevelWidth );
	outPictureLocation.h = localFrame.left;
	outPictureLocation.v = localFrame.top;

													// Get the size of the picture
	SDimension16	pictSize = GetPictureSize ();

													// Setup the width and height values
	width = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SInt16	pictHeight = pictSize.height;
	SInt16	pictWidth = pictSize.width;

													// Now we calculate the location of the icon
													// based on the graphic alignment
	switch ( mGraphicAlignment ) {
		case kControlBevelButtonAlignSysDirection: {
			outPictureLocation.h += mGraphicOffset.h;
			outPictureLocation.v += ( height / 2 ) - ( pictHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignCenter: {
			SInt16	locH = (SInt16) (( width / 2 ) - ( pictWidth / 2 ));
			SInt16	locV = (SInt16) (( height / 2 ) - ( pictHeight / 2 ));
			if ( HasTitle ()) {
				if ( mTitlePlacement == kControlBevelButtonPlaceAboveGraphic ) {
					locV = (SInt16) ((((height - (bevelButton_GraphicSlop +
										CalcTitleHeight () + pictHeight)) / 2) +
											(bevelButton_GraphicSlop + CalcTitleHeight ())) - 1);

				} else if ( mTitlePlacement == kControlBevelButtonPlaceBelowGraphic ) {
					locV = (SInt16) (( height / 2 ) - ((bevelButton_GraphicSlop +
										CalcTitleHeight () + pictHeight) / 2));
				}
			}
													// Now set the location
			outPictureLocation.h += locH;
			outPictureLocation.v += locV;
		}
		break;

		case kControlBevelButtonAlignLeft: {
			outPictureLocation.h += mGraphicOffset.h;
			outPictureLocation.v += ( height / 2 ) - ( pictHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignRight: {
			outPictureLocation.h += width - ( pictWidth + mGraphicOffset.h );
			outPictureLocation.v += ( height / 2 ) - ( pictHeight / 2 );
		}
		break;

		case kControlBevelButtonAlignTop: {
			outPictureLocation.h += ( width / 2 ) - ( pictWidth / 2 );
			outPictureLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottom: {
			outPictureLocation.h += ( width / 2 ) - ( pictWidth / 2 );
			outPictureLocation.v += height - ( pictHeight + mGraphicOffset.v );
		}
		break;

		case kControlBevelButtonAlignTopLeft: {
			outPictureLocation.h += mGraphicOffset.h;
			outPictureLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottomLeft: {
			outPictureLocation.h += mGraphicOffset.h;
			outPictureLocation.v += height - ( pictHeight + mGraphicOffset.v );
		}
		break;

		case kControlBevelButtonAlignTopRight: {
			outPictureLocation.h += width - ( pictWidth + mGraphicOffset.h );
			outPictureLocation.v += mGraphicOffset.v;
		}
		break;

		case kControlBevelButtonAlignBottomRight: {
			outPictureLocation.h += width - ( pictWidth + mGraphicOffset.h );
			outPictureLocation.v += height - ( pictHeight + mGraphicOffset.v );
		}
		break;
	}
}


// ---------------------------------------------------------------------------
//	е CalcTitleWidth											   [protected]
// ---------------------------------------------------------------------------

SInt16
LGABevelButtonImp::CalcTitleWidth()
{
													// If we have a title then we setup the text
													// style and get the width otherwise we just
													// return zero
	if ( HasTitle ()) {
		StTextState			theTextState;
		StColorPenState	thePenState;

													// Get the text traits ID for the control
		ResIDT	textTID = GetTextTraitsID ();

													// Get the port setup with the text traits
		UTextTraits::SetPortTextTraits ( textTID );

													// Figure out the width of the title
		Str255 tempStr;
		GetDescriptor ( tempStr );
		return ::StringWidth ( tempStr );
	}

	return 0;

}


// ---------------------------------------------------------------------------
//	е CalcTitleHeight											   [protected]
// ---------------------------------------------------------------------------

SInt16
LGABevelButtonImp::CalcTitleHeight ()
{
													// If we have a title then we setup the text
													// style and get the height from the font info
	if ( HasTitle ()) {
		StTextState			theTextState;
		StColorPenState	thePenState;

													// Get the text traits ID for the control
		ResIDT	textTID = GetTextTraitsID ();

													// Get the port setup with the text traits
		UTextTraits::SetPortTextTraits ( textTID );

													// Figure out the height of the text for the
													// selected font
		FontInfo fi;
		::GetFontInfo ( &fi );

													// The height of the title is the sum of the
													// ascent and descent
		return (SInt16) (fi.ascent + fi.descent);
	}

	return 0;

}


// ---------------------------------------------------------------------------
//	е ClipToBevelButtonFace										   [protected]
// ---------------------------------------------------------------------------

void
LGABevelButtonImp::ClipToBevelButtonFace()
{
													// What we are going to do here is find the
													// intersection between the current clipping
													// region and the face of the bevel button
	Rect 	localFrame;
	CalcLocalFrameRect ( localFrame );

													// The face is the local frame less the bevels
													// and the frame around the button
	::MacInsetRect ( &localFrame, (SInt16) (GetBevelWidth () + 1), (SInt16) (GetBevelWidth () + 1) );

													// Get the current clipping region and find the
													// intersection between it and the face rect
	StRegion		clipRgn;
	::GetClip ( clipRgn );
	clipRgn &= localFrame;
													// Now set the clipping to that intersection
	::SetClip ( clipRgn );
}


PP_End_Namespace_PowerPlant
