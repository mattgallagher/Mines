// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGACIconMixin.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGACIconMixin.h>
#include <UGraphicUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGACIconMixin							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGACIconMixin::LGACIconMixin()
{
	mCIconH					= nil;
	mCIconResID				= 0;
	mCIconLocation			= Point_00;
	mHiliteCIcon			= false;
	mOffsetCIconOnHilite	= true;
	mDisposeCIconH			= false;

	CalcCIconLocation(mCIconLocation);
}


// ---------------------------------------------------------------------------
//	¥ LGACIconMixin							Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGACIconMixin::LGACIconMixin(
	const LGACIconMixin&	inOriginal)
{
	mCIconH					= inOriginal.mCIconH;
	mCIconResID				= inOriginal.mCIconResID;
	mCIconLocation			= inOriginal.mCIconLocation;
	mHiliteCIcon			= inOriginal.mHiliteCIcon;
	mOffsetCIconOnHilite	= inOriginal.mOffsetCIconOnHilite;
	mDisposeCIconH			= inOriginal.mDisposeCIconH;
}


// ---------------------------------------------------------------------------
//	¥ ~LGACIconMixin						Destructor				  [public]
// ---------------------------------------------------------------------------

LGACIconMixin::~LGACIconMixin()
{
	if ( DisposesCIconH() && (mCIconH != nil) ) {
		::DisposeCIcon(mCIconH);
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LoadCIconHandle												  [public]
// ---------------------------------------------------------------------------

void
LGACIconMixin::LoadCIconHandle()
{
											// Load and cache the icon handle
	CIconHandle		theIconH;
	theIconH = ::GetCIcon(mCIconResID);
	ThrowIfNil_ ( theIconH );

	SetCIconH(theIconH, true);
}


// ---------------------------------------------------------------------------
// 	¥ GetCIconSize													  [public]
// ---------------------------------------------------------------------------

SDimension16
LGACIconMixin::GetCIconSize()
{
	SDimension16	iconSize;
	iconSize.width = iconSize.height = 0;

											// Figure out what size the icon is and then return
											// that using the PP dimension struct
	if (mCIconH != nil) {
											// We figure the icon's size out from the handle where
											// we use the pixmap to determine the width and height
		iconSize.width  = (SInt16) ((**mCIconH).iconPMap.bounds.right  - (**mCIconH).iconPMap.bounds.left);
		iconSize.height = (SInt16) ((**mCIconH).iconPMap.bounds.bottom - (**mCIconH).iconPMap.bounds.top);
	}
											// If we don't have a handle then simply return an
											// empty dimension
	return iconSize;
}


// ---------------------------------------------------------------------------
//	¥ SetCIconH														  [public]
// ---------------------------------------------------------------------------

void
LGACIconMixin::SetCIconH(
	CIconHandle		inCIconH,
	bool			inDisposesHandle)
{

											// Before we set the handle we need to make sure that
											//	we get rid of any cached handle that we might have,
											// we only do this if we had been setup to dispose the
											// handle before, otherwise its the users
											// responsibility
	if ( DisposesCIconH () && mCIconH ) {
		::DisposeCIcon ( mCIconH );
		mCIconH = nil;
	}
											// Now set our cache to the new handle
	mCIconH = inCIconH;
											// We also setup the cache disposal flag
	mDisposeCIconH = inDisposesHandle;

}


// ---------------------------------------------------------------------------
//	¥ SetCIconResourceID											  [public]
// ---------------------------------------------------------------------------

void
LGACIconMixin::SetCIconResourceID(
	ResIDT	inResID)
{
											// Only make the change if it is different, if the ID
											// did in fact change then we also need to make sure
											// that we get the icon suite cached
	if ( mCIconResID != inResID ) {
		mCIconResID = inResID;
											// If the resource ID is being set to zero or undefined
											// we need to make sure that we get some of the
											// associated fields cleared out
		if ( inResID == 0 || inResID == resID_Undefined ) {
											// Get rid of the cached icon if needed
			if ( DisposesCIconH () && mCIconH != nil ) {
				::DisposeCIcon( mCIconH );
			}

			mCIconH = nil;
			mDisposeCIconH = false;
		} else {
											// Load and cache the icon handle
			LoadCIconHandle ();
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalCIconRect											  [public]
// ---------------------------------------------------------------------------

void
LGACIconMixin::CalcLocalCIconRect(
	Rect&	/* outRect */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CalcCIconLocation											   [protected]
// ---------------------------------------------------------------------------

void
LGACIconMixin::CalcCIconLocation(
	Point&	/* outCIconLocation */)
{
}


PP_End_Namespace_PowerPlant
