// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAIconControlImp.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAIconControlImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LControl.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAIconControlImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAIconControlImp::LGAIconControlImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAIconControlImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAIconControlImp::LGAIconControlImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mIconAlignment	= 0;
	mIconType		= FOUR_CHAR_CODE('iclx');
	mWantsTracking	= true;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAIconControlImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAIconControlImp::~LGAIconControlImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init ( inControlPane, controlKind, title, textTraitID );
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
												// Setup our fields
	mControlPane = inControlPane;
	mIconAlignment = 0;
												// Setup the icon type and whether we are tracking
												// or not
	mIconType = (SInt32) (inControlKind == kControlIconProc || inControlKind ==
							kControlIconNoTrackProc ? FOUR_CHAR_CODE('cicn') : FOUR_CHAR_CODE('iclx'));
	mWantsTracking = (inControlKind == kControlIconProc) ||
									(inControlKind == kControlIconSuiteProc);

												// Get the resource ID setup
	ResIDT	resourceID = (ResIDT) mControlPane->GetValue ();
	if ( resourceID != 0 && resourceID != resID_Undefined )
		SetResourceID ( resourceID );

												// If we have an icon suite we need to start out by
												// figuring out what size we are going to draw it
	if ( HasIconSuite ()) {
		CalcIconSuiteSize ();
	}
												// Make sure we have the transform correctly setup
												// based on the state of the icon
	SInt16 transform = !mControlPane->GetEnabledState () ? kTransformDisabled : kTransformNone;
	SetIconTransform ( transform );

}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ CalcLocalIconRect												  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::CalcLocalIconRect(
	Rect&	outRect)
{
												// We always start by getting the graphic location
												// recalculated
	Point	iconLocation;
	CalcIconLocation ( iconLocation );

	outRect.top    = iconLocation.v;
	outRect.left   = iconLocation.h;

	if ( HasIconSuite ()) {
		outRect.bottom = (SInt16) (iconLocation.v + mSizeSelector);
		outRect.right  = (SInt16) (outRect.left + (mSizeSelector == 12 ? 16 : mSizeSelector));

	} else {
		SDimension16	iconSize = GetCIconSize();
		outRect.bottom = (SInt16) (iconLocation.v + iconSize.height);
		outRect.right  = (SInt16) (outRect.left + iconSize.width);
	}
}


// ---------------------------------------------------------------------------
//	¥ SetResourceID													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::SetResourceID(
	ResIDT	inResID)
{
												// Setup the new resource ID
	mResourceID = inResID;
												// Call the appropriate method in the mixin classes
	if ( HasIconSuite ()) {
		SetIconResourceID ( inResID );
	} else {
		SetCIconResourceID ( inResID );
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::SetPushedState(
	Boolean		inPushedState)
{
												// We only change the state if we have to
	if ( mPushed != inPushedState ) {
		mPushed = inPushedState;

												// Things have changed so we need to make sure that
												// the transform is in synch with the change
		if ( inPushedState ) {
			if ( IsEnabled ())
				mTransform += kTransformSelected;

		} else {
			if ( IsEnabled ()) {
				if ( mTransform == kTransformSelected ) {
					mTransform = kTransformNone;
				} else {
					mTransform -= kTransformSelected;
				}
			}
		}
												// Because the state has changed we need to make
												// sure that things get redrawn so that the change
												// can take effect
		mControlPane->Draw ( nil );
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::EnableSelf()
{
	if (mControlPane->GetActiveState() == triState_On) {
													// If we are an icon suite then we set the
													// transform back to the last one, otherwise if
													// we are a CIcon we simply set it to none
		mTransform = kTransformNone;
		if ( HasIconSuite ()) {
			mTransform = mLastTransform;
		}

		mControlPane->Draw ( nil );
	}
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DisableSelf()
{
												// Always just set the transform to be disabled
	if (mControlPane->GetActiveState() == triState_On) {
												// Set the transform to be disabled and then redraw
												// everything, NOTE: we are going to do this by
												// simply adding the disabled transform to the
												// current transform
		mLastTransform = mTransform;
		if ( mTransform != kTransformOpen && mTransform != kTransformDisabled ) {
			mTransform += kTransformDisabled;
		}

		mControlPane->Draw ( nil );
	}
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::ActivateSelf()
{
	if (mControlPane->GetEnabledState() == triState_On) {
													// If we are an icon suite then we set the
													// transform back to the last one, otherwise if
													// we are a CIcon we simply set it too none
		mTransform = kTransformNone;
		if ( HasIconSuite ()) {
			mTransform = mLastTransform;
		}
															// Get everything redrawn
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DeactivateSelf()
{
	if (mControlPane->GetEnabledState() == triState_On) {
													// Adjust the transform and then refresh
													// NOTE: we are going to do this by simply
													// adding the disabled transform to the current
													// transform
		mLastTransform = mTransform;
		if ( mTransform != kTransformOpen && mTransform != kTransformDisabled ) {
			mTransform += kTransformDisabled;
		}
													// Get everything redrawn
		Refresh();
	}
}


#pragma mark -
#pragma mark === DATA HANDLING

// ---------------------------------------------------------------------------
//	¥ SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr )
{
	switch (inTag) {
	
		case kControlIconAlignmentTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mIconAlignment = value;
			mControlPane->Draw ( nil );
			break;
		}

		case kControlIconTransformTag: {
			SInt16	value = *(SInt16 *)inDataPtr;
			mTransform = value;
			mControlPane->Draw ( nil );
			break;
		}
		
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::GetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inBufferSize,
	void*			inBuffer,
	Size*			outDataSize) const
{
	switch (inTag) {
	
		case kControlIconAlignmentTag: {
			*(SInt16 *)inBuffer =  mIconAlignment;
			if (outDataSize != nil) {
				*outDataSize = sizeof(mIconAlignment);
			}
			break;
		}

		case kControlIconTransformTag: {
			*(SInt16 *)inBuffer =  mTransform;
			if (outDataSize != nil) {
				*outDataSize = sizeof(mTransform);
			}
			break;
		}
		
		default:
			LGAControlImp::GetDataTag(inPartCode, inTag, inBufferSize,
										inBuffer, outDataSize);
			break;
	}
}

#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DrawSelf()
{
	if (HasIconSuite()) {
		DrawIconSuiteGraphic();

	} else {
		DrawCIconGraphic();
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawIconSuiteGraphic										   [protected]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DrawIconSuiteGraphic()
{

	StColorPenState	theColorPenState;
	theColorPenState.Normalize();
												// Get the icon rectangle
	Rect iconRect;
	CalcLocalIconRect ( iconRect );

	SInt16	iconAlign;
	switch ( mIconAlignment ) {
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
												//  Now we plot the icon by calling the toolbox
												// passing it our cached handle to the icon suite
	if ( mIconSuiteH != nil ) {
		::PlotIconSuite ( &iconRect, iconAlign, mTransform, mIconSuiteH );
	}
}


// ---------------------------------------------------------------------------
//	¥ DrawCIconGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DrawCIconGraphic()
{
	Rect iconRect;
	CalcLocalIconRect(iconRect);

	UGraphicUtils::DrawCIconGraphic(mCIconH, iconRect, mTransform,
									mControlPane->GetBitDepth());
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ TrackHotSpot												   [protected]
// ---------------------------------------------------------------------------
//

Boolean
LGAIconControlImp::TrackHotSpot(
	SInt16		inHotSpot,
	Point		inPoint,
	SInt16		inModifiers)
{

												// We only want to handle the tracking if it has
												// been selected
	if ( WantsTracking ()) {
		return LGAControlImp::TrackHotSpot ( inHotSpot, inPoint, inModifiers );
	}
												// User didn't want tracking so we just bail out
	return false;

}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::DoneTracking(
	SInt16		/* inHotSpot */,
	Boolean		/* inGoodTrack */)
{
	SetPushedState(false);				// Turn off hiliting
}


#pragma mark -
#pragma mark === MISCELLANEOUS

// ---------------------------------------------------------------------------
//	¥ CalcIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::CalcIconLocation(
	Point&	outIconLocation)
{
	Rect		localFrame;
	SInt16		width, height;
												// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	outIconLocation.h = localFrame.left;
	outIconLocation.v = localFrame.top;

												// Setup the width and height values
	width  = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SDimension16	iconSize = GetCIconSize ();

	SInt16	iconHeight = iconSize.height;
	SInt16	iconWidth  = iconSize.width;

	if (HasIconSuite()) {
		iconHeight = mSizeSelector;
		iconWidth  = mSizeSelector;
		if (iconWidth == 12) {
			iconWidth = 16;
		}
	}

												// Now we calculate the location of the icon based
												// on the graphic alignment
	switch ( mIconAlignment ) {
		case kControlBevelButtonAlignSysDirection: {
			outIconLocation.v += (height - iconHeight) / 2;
		}
		break;

		case kControlBevelButtonAlignCenter: {
			outIconLocation.h += (width - iconWidth) / 2 ;
			outIconLocation.v += (height - iconHeight) / 2;
		}
		break;

		case kControlBevelButtonAlignLeft: {
			outIconLocation.v += (height - iconHeight) / 2;
		}
		break;

		case kControlBevelButtonAlignRight: {
			outIconLocation.h += width - iconWidth;
			outIconLocation.v += (height - iconHeight) / 2;
		}
		break;

		case kControlBevelButtonAlignTop: {
			outIconLocation.h += (width - iconWidth) / 2;
		}
		break;

		case kControlBevelButtonAlignBottom: {
			outIconLocation.h += ( width - iconWidth) / 2;
			outIconLocation.v += height - iconHeight;
		}
		break;

		case kControlBevelButtonAlignBottomLeft: {
			outIconLocation.v += height - iconHeight;
		}
		break;

		case kControlBevelButtonAlignTopRight: {
			outIconLocation.h += width - iconWidth;
		}
		break;

		case kControlBevelButtonAlignBottomRight: {
			outIconLocation.h += width - iconWidth;
			outIconLocation.v += height - iconHeight;
		}
		break;
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcIconSuiteSize											   [protected]
// ---------------------------------------------------------------------------

void
LGAIconControlImp::CalcIconSuiteSize()
{
												// If the content is an icon suite then we need to
												// figure out the correct size
	if (HasIconSuite()) {
												// Here we are going to attempt to figure out what
												// size icon we are going to be drawing
		Rect	localFrame;
		CalcLocalFrameRect ( localFrame );
		SInt16				width, height;
												// Now we can figure out the width and height that
												// will be used to figure out the icon size
		width = UGraphicUtils::RectWidth ( localFrame );
		height = UGraphicUtils::RectHeight ( localFrame );

												// Now figure out the appropriate size based on the
												// size of the control
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


PP_End_Namespace_PowerPlant
