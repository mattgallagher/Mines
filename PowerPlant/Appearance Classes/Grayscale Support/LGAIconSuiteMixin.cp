// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAIconSuiteMixin.cp		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAIconSuiteMixin.h>
#include <UGraphicUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAIconSuiteMixin						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGAIconSuiteMixin::LGAIconSuiteMixin()
{
	mIconSuiteH			= nil;
	mIconSuiteID		= 0;
	mTransform			= kTransformNone;
	mLastTransform		= kTransformNone;
	mSizeSelector		= sizeSelector_LargeIconSize;
	mIconPlacement		= iconPosition_Center;
	mIconLocation		= Point_00;
	mEdgeOffset			= 4;
	mHiliteIcon			= false;
	mOffsetIconOnHilite	= true;
	mDisposeIconSuiteH	= false;

										//  Get the icon location calculated
	CalcIconLocation ( mIconLocation );
}


// ---------------------------------------------------------------------------
//	¥ LGAIconSuiteMixin						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGAIconSuiteMixin::LGAIconSuiteMixin(
	const LGAIconSuiteMixin&	inOriginal)
{
	mIconSuiteH			= inOriginal.mIconSuiteH;
	mIconSuiteID		= inOriginal.mIconSuiteID;
	mTransform			= inOriginal.mTransform;
	mLastTransform		= inOriginal.mLastTransform;
	mSizeSelector		= inOriginal.mSizeSelector;
	mIconPlacement		= inOriginal.mIconPlacement;
	mIconLocation		= inOriginal.mIconLocation;
	mEdgeOffset			= inOriginal.mEdgeOffset;
	mHiliteIcon			= inOriginal.mHiliteIcon;
	mOffsetIconOnHilite	= inOriginal.mOffsetIconOnHilite;
	mDisposeIconSuiteH	= inOriginal.mDisposeIconSuiteH;

}


// ---------------------------------------------------------------------------
//	¥ ~LGAIconSuiteMixin					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAIconSuiteMixin::~LGAIconSuiteMixin()
{
	if ( DisposesIconSuiteH () && mIconSuiteH != nil ) {
		::DisposeIconSuite ( mIconSuiteH, true );
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LoadIconSuiteHandle											  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::LoadIconSuiteHandle()
{

										// Load and cache the icon suite
	Handle				theIconSuite;
	IconSelectorValue	selector = svAllAvailableData;

										// Now handle the actual loading of the icon suite
	::GetIconSuite ( &theIconSuite, mIconSuiteID, selector );
	ThrowIfNil_ ( theIconSuite );
	if ( theIconSuite ) {
		SetIconSuiteH ( theIconSuite, true );
	}

}


// ---------------------------------------------------------------------------
//	¥ SetIconSuiteH													  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetIconSuiteH(
	Handle		inIconSuiteH,
	bool		inDisposesHandle)
{
										// Before we set the handle we need to make sure that we
										// get rid of any cached handle that we might have, we
										// only do this if we had been setup to dispose the
										// handle before, otherwise its the users responsibility
	if ( DisposesIconSuiteH () && mIconSuiteH ) {
		::DisposeIconSuite ( mIconSuiteH, true );
		mIconSuiteH = nil;
	}

										// Now set our cache to the new handle
	mIconSuiteH = inIconSuiteH;

										//  We also setup the cache disposal flag
	mDisposeIconSuiteH = inDisposesHandle;
}


// ---------------------------------------------------------------------------
//	¥ SetIconResourceID												  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetIconResourceID(
	ResIDT	inResID)
{
										// Only make the change if it is different, if the ID did
										// in fact change then we also need to make sure that we
										// get the icon suite cached
	if ( mIconSuiteID != inResID ) {
		mIconSuiteID = inResID;
										// If the icon is being set to zero or undefined we need
										// to clear up a number of fields
		if ( inResID == 0 || inResID == resID_Undefined ) {
										// Get rid of the cached icon suite if needed
			if ( DisposesIconSuiteH () && mIconSuiteH != nil )
				::DisposeIconSuite ( mIconSuiteH, true );

			mIconSuiteH = nil;
			mDisposeIconSuiteH = false;
		} else {
										// Load and cache the icon suite
			LoadIconSuiteHandle ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ SetIconTransform												  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetIconTransform(
	SInt16		inTransform)
{
	if ( mTransform != inTransform ) {
		mTransform = inTransform;
		mLastTransform = inTransform;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetIconSize													  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetIconSize(
	ESizeSelector	inSizeSelector)
{
										// Only make the change if it is different
	if ( mSizeSelector != inSizeSelector ) {
		mSizeSelector = inSizeSelector;

										// We also need to get the icons location recalculated
		CalcIconLocation ( mIconLocation );
	}
}


// ---------------------------------------------------------------------------
//	¥ SetIconPosition												  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetIconPosition(
	EIconPosition	inPosition)
{
										// Only make the change if it is different
	if ( mIconPlacement != inPosition ) {
		mIconPlacement = inPosition;

										// We also need to get the icons location recalculated
		CalcIconLocation ( mIconLocation );
	}
}


// ---------------------------------------------------------------------------
//	¥ SetEdgeOffset													  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::SetEdgeOffset(
		SInt16		inOffset)
{
										// Only make the change if it is different
	if ( mEdgeOffset != inOffset ) {
		mEdgeOffset = inOffset;

										// We also need to get the icons location recalculated
										// as this change may alter where it is drawn
		CalcIconLocation ( mIconLocation );
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalIconRect												  [public]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::CalcLocalIconRect(
	Rect&	/* outRect */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGAIconSuiteMixin::CalcIconLocation(
	Point&	/* outIconLocation */)
{
}


PP_End_Namespace_PowerPlant
