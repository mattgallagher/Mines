// Copyright й2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAImageWellImp.cp			PowerPlant 2.2.2	й1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAImageWellImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LView.h>
#include <LStream.h>
#include <UDrawingState.h>
#include <PP_Messages.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	е LGAImageWellImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGAImageWellImp::LGAImageWellImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	е LGAImageWellImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAImageWellImp::LGAImageWellImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mHasAutoTracking = false;
	mContentType	 = kControlContentTextOnly;
	mResourceID		 = 0;
}


// ---------------------------------------------------------------------------
//	е ~LGAImageWellImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAImageWellImp::~LGAImageWellImp ()
{
}


// ---------------------------------------------------------------------------
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::Init(
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
//	е Init															  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::Init(
	LControlPane*	inControlPane,
	SInt16			/* inControlKind */,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;

												// Extract the information from the control values
												// that we care about
	mHasAutoTracking = false; 			// ее Not supported yet! (inControlKind == kControlImageWellAutoTrackProc);
	mContentType = (SInt16) mControlPane->GetMinValue ();
	ResIDT	resourceID = (ResIDT) mControlPane->GetValue ();

												// Setting the resource ID will result in the right
												// thing being done based on the content type
												// specified
	if ( HasIconSuiteRes () || HasCIconRes () || HasPictRes ()) {
		if ( resourceID != 0 && resourceID != resID_Undefined ) {
			SetResourceID ( resourceID );
		} else {
			mResourceID = resourceID;
		}

	} else {
		mResourceID = 0;
	}
												// If the content is an icon suite then we need to
												// get all the appropriate data setup for that type
	if ( HasIconSuiteRes ()) {
												// Figure out what size the icon suite will be
												// drawn at, either large, small or mini
		CalcIconSuiteSize ();
												// Make sure that the transform for the icon is
												// correctly setup
		AdjustTransform();
	}

												// Setup the initial value now
	mControlPane->SetValue ( Button_Off );
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е CalcLocalGraphicRect											  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::CalcLocalGraphicRect (
		Rect &outRect )
{
												// We always start by getting the icon location
												// recalculated
	Point	graphicLocation;
	CalcGraphicLocation ( graphicLocation );

	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
		case kControlContentIconSuiteHandle: {
												// If we have an icon suite handle we figure out
												// the size of the rectangle it would occupy,
												// otherwise we simply return an empty rectangle.
			if ( GetIconSuiteH () != nil ) {
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
												// the size of the rectangle it would occupy,
												// otherwise we simply return an empty rectangle
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
												// size of the picture
			outRect.top = graphicLocation.v;
			outRect.left = graphicLocation.h;
			outRect.bottom = (SInt16) (graphicLocation.v + pictSize.height);
			outRect.right = (SInt16) (outRect.left + pictSize.width);
		}
		break;

		default:
			outRect = Rect_0000;
			break;
	}
}


// ---------------------------------------------------------------------------
//	е SetResourceID																		[public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::SetResourceID (
		ResIDT	inResID )
{
												// Setup the new resource ID
	mResourceID = inResID;
												// Now make sure that we load the appropriate type
												// of resource to match the new ID
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
//	е AdjustTransform												  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::AdjustTransform()
{
									// Transforms only apply to icon suites
	if (HasIconSuiteRes() || HasIconSuiteHandle()) {

									// Adjust the transform as per the new
									//   state of the well
		if (IsEnabled() && IsActive()) {
			mTransform = kTransformNone;
		} else {
			mTransform = kTransformDisabled;
		}
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LGAImageWellImp::GetForeAndBackColors	(
		RGBColor		*outForeColor,
		RGBColor		*outBackColor ) const
{
	if (outForeColor != nil) {
		*outForeColor = Color_Black;
	}

	if (outBackColor != nil) {
		*outBackColor = Color_White;
	}

	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::EnableSelf()
{
 	if (mControlPane->GetActiveState() == triState_On) {
		AdjustTransform();
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	е DisableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DisableSelf ()
{
	if (mControlPane->GetActiveState() == triState_On) {
		AdjustTransform();
		mControlPane->Draw(nil);
	}
}


// ---------------------------------------------------------------------------
//	е ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::ActivateSelf ()
{
	if (mControlPane->GetEnabledState() == triState_On) {
		AdjustTransform();
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	е DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DeactivateSelf ()
{
	if (mControlPane->GetEnabledState() == triState_On) {
		AdjustTransform();
		Refresh ();
	}
}


// ---------------------------------------------------------------------------
//	е ShowSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::ShowSelf ()
{
	AdjustTransform();
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е SetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::SetDataTag(
	SInt16			inPartCode,
	FourCharCode	inTag,
	Size			inDataSize,
	void*			inDataPtr)
{
	switch (inTag) {
	
		case kControlImageWellContentTag: {
			ControlButtonContentInfo controlInfo = *(ControlButtonContentInfo *)inDataPtr;

												// Get the new values assigned where appropriate
												// NOTE: currently we are not handling the icon
												// ref case
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
					AdjustTransform();
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

		case kControlImageWellTransformTag: {
												// We only apply a transform change if we have a
												// content type of icon suite
			if ( HasIconSuiteRes () || HasIconSuiteHandle ()) {
				SInt16	value = *(SInt16 *)inDataPtr;
				mTransform = value;
				mControlPane->Draw ( nil );
			}
			break;
		}
		
		default:
			LGAControlImp::SetDataTag(inPartCode, inTag, inDataSize, inDataPtr);
			break;
	}
}


// ---------------------------------------------------------------------------
//	е GetDataTag													  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::GetDataTag(
		SInt16			inPartCode,
		FourCharCode	inTag,
		Size			inBufferSize,
		void*			inBuffer,
		Size*			outDataSize) const
{

	switch ( inTag ) {
	
		case kControlImageWellContentTag: {
			ControlButtonContentInfo controlInfo;
			controlInfo.contentType = mContentType;
			
			switch ( mContentType ) {
			
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
					controlInfo.u.picture = HasPictHandle () ? GetPictureHandle () : nil;
					break;
			}

			*(ControlButtonContentInfo *)inBuffer = controlInfo;
	
			if (outDataSize != nil) {
				*outDataSize = sizeof(controlInfo);
			}
			break;
		}

		case kControlImageWellTransformTag: {
												// We only return a transform if we have a content
												// type of icon suite res or icon suite handle
			if ( HasIconSuiteRes () || HasIconSuiteHandle ()) {
				*(SInt16 *)inBuffer =  mTransform;
				if (outDataSize != nil) {
					*outDataSize = sizeof(mTransform);
				}
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

// ---------------------------------------------------------------------------
//	е DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DrawSelf()
{
	StColorPenState::Normalize();

	Rect	frame;
	CalcLocalFrameRect(frame);

	RGBColor	penColor = Color_Black;

	if (mControlPane->GetBitDepth() >= 4) {

		if (IsActive() and IsEnabled()) {	// Bevels around frame

											// Gray on left and top
			UGAColorRamp::GetColor(colorRamp_Gray7, penColor);
			::RGBForeColor(&penColor);
			::MoveTo(frame.left, (SInt16) (frame.bottom - 2));
			::LineTo(frame.left, frame.top);
			::LineTo((SInt16) (frame.right - 2), frame.top);

											// White on bottom and right
			::RGBForeColor(&Color_White);
			::MoveTo((SInt16) (frame.left + 1), (SInt16) (frame.bottom - 1));
			::LineTo((SInt16) (frame.right - 1), (SInt16) (frame.bottom - 1));
			::LineTo((SInt16) (frame.right - 1), (SInt16) (frame.top + 1));

			penColor = Color_Black;			// Color for Frame

		} else {
											// Inactive Frame is gray
			UGAColorRamp::GetColor(colorRamp_Gray10, penColor);
		}
	}

	::MacInsetRect(&frame, 1, 1);			// Draw Frame
	::RGBForeColor(&penColor);
	::MacFrameRect(&frame);

											// Erase interior to white
	::MacInsetRect(&frame, 1, 1);
	::RGBBackColor(&Color_White);
	::EraseRect(&frame);

	DrawGraphic();
}



// ---------------------------------------------------------------------------
//	е DrawGraphic												   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DrawGraphic()
{
	switch ( mContentType ) {
		case kControlContentIconSuiteRes:
		case kControlContentIconSuiteHandle:
			DrawIconSuiteGraphic ();
		break;

		case kControlContentCIconRes:
		case kControlContentCIconHandle:
			DrawCIconGraphic ();
		break;

		case kControlContentPictRes:
		case kControlContentPictHandle:
			DrawPictGraphic ();
		break;
	}
}


// ---------------------------------------------------------------------------
//	е DrawIconSuiteGraphic										   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DrawIconSuiteGraphic()
{

	StColorPenState	theColorPenState;
	theColorPenState.Normalize ();
												// Get the icon rectangle
	Rect iconRect;
	CalcLocalGraphicRect ( iconRect );
												//  Now we plot the icon by calling the toolbox
												// passing it our cached handle to the icon suite
	if ( GetIconSuiteH () != nil ) {
		::PlotIconSuite ( &iconRect, kAlignNone, mTransform, GetIconSuiteH () );
	}
}


// ---------------------------------------------------------------------------
//	е DrawCIconGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DrawCIconGraphic()
{
	Rect iconRect;
	CalcLocalGraphicRect(iconRect);

	SInt16	transform = kTransformNone;
	if (!IsActive() || !IsEnabled()) {
		transform = kTransformDisabled;
	}

	UGraphicUtils::DrawCIconGraphic(mCIconH, iconRect, transform,
									mControlPane->GetBitDepth());
}


// ---------------------------------------------------------------------------
//	е DrawPictGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::DrawPictGraphic()
{
	Rect pictureRect;
	CalcLocalGraphicRect(pictureRect);

	PicHandle	thePictH = GetPictureHandle();

	SInt16	transform = kTransformNone;
	if ( !IsActive() || !IsEnabled() ) {
		transform = kTransformDisabled;
	}

	UGraphicUtils::DrawPictGraphic(thePictH, pictureRect, transform,
									mControlPane->GetBitDepth());
}


// ---------------------------------------------------------------------------
//	е RefreshGraphic											   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::RefreshGraphic()
{
	Rect	graphicRect;
	CalcLocalGraphicRect(graphicRect);

	mControlPane->RefreshRect(graphicRect);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		/* inCurrInside */,
	Boolean		/* inPrevInside */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	е CalcGraphicLocation										   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::CalcGraphicLocation(
	Point&	outLocation)
{
	switch (mContentType) {

		case kControlContentIconSuiteRes:
		case kControlContentCIconRes:
		case kControlContentIconSuiteHandle:
		case kControlContentCIconHandle:
			CalcIconLocation(outLocation);
			break;

		case kControlContentPictRes:
		case kControlContentPictHandle:
			CalcPictureLocation(outLocation);
			break;
	}
}


// ---------------------------------------------------------------------------
//	е CalcIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::CalcIconLocation(
	Point&	outIconLocation)
{
	Rect		localFrame;
	SInt16		width, height;
												// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, 2, 2 );
	outIconLocation.h = localFrame.left;
	outIconLocation.v = localFrame.top;

												// Setup the width and height values
	width = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SDimension16	iconSize = GetCIconSize ();
	SInt16	iconHeight = (SInt16) (HasIconSuiteContent() ? mSizeSelector : iconSize.height);
	SInt16	iconWidth = (SInt16) (HasIconSuiteContent() ? (mSizeSelector == 12 ?
										16 : mSizeSelector) : iconSize.width);

												// Now we calculate the location of the icon based
												// on the size the idea being to always have the
												// graphic centered on the well
	outIconLocation.h += (width - iconWidth) / 2 ;
	outIconLocation.v += (height - iconHeight) / 2;

}


// ---------------------------------------------------------------------------
//	е CalcIconSuiteSize											   [protected]
// ---------------------------------------------------------------------------

void
LGAImageWellImp::CalcIconSuiteSize()
{
												// If the content is an icon suite then we need to
												// get all the appropriate data setup for that type
	if ( HasIconSuiteContent ()) {
												// Here we are going to attempt to figure out what
												// size icon we are going to be drawing
		Rect	localFrame;
		CalcLocalFrameRect ( localFrame );
		SInt16				width, height;
		SInt16				bevelHSlop = 2;
		SInt16				bevelVSlop = 2;

												// Now we can figure out the width and height that
												// will be used to figure out the icon size
		width  = (SInt16) (UGraphicUtils::RectWidth ( localFrame ) - bevelHSlop);
		height = (SInt16) (UGraphicUtils::RectHeight ( localFrame ) - bevelVSlop);

												// Now that we figure out the size of the icon
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
LGAImageWellImp::CalcPictureLocation(
	Point&	outPictureLocation)
{
	Rect		localFrame;
	SInt16		width, height;
												// Get the local inset frame rect
	CalcLocalFrameRect ( localFrame );
	::MacInsetRect ( &localFrame, 2, 2 );
	outPictureLocation.h = localFrame.left;
	outPictureLocation.v = localFrame.top;

												// Get the size of the picture
	SDimension16	pictSize = GetPictureSize ();

												// Setup the width and height values
	width = UGraphicUtils::RectWidth ( localFrame );
	height = UGraphicUtils::RectHeight ( localFrame );
	SInt16	pictHeight = pictSize.height;
	SInt16	pictWidth = pictSize.width;

												// Now set the location
	outPictureLocation.h += (width - pictWidth) / 2;
	outPictureLocation.v += (height - pictHeight) / 2;

}


PP_End_Namespace_PowerPlant
