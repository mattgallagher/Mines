// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPictureMixin.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPictureMixin.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAPictureMixin						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPictureMixin::LGAPictureMixin()
{
	mPictureH			= nil;
	mPictureResID		= 0;
	mDisposePictureH	= false;
}


// ---------------------------------------------------------------------------
// 	¥ LGAPictureMixin						Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPictureMixin::LGAPictureMixin(
	const LGAPictureMixin&	inOriginal)
{
	mPictureH			= inOriginal.mPictureH;
	mPictureResID		= inOriginal.mPictureResID;
	mDisposePictureH	= inOriginal.mDisposePictureH;
}


// ---------------------------------------------------------------------------
// 	¥ ~LGAPictureMixin						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPictureMixin::~LGAPictureMixin()
{
	DisposePictureHandle();
}

#pragma mark -


// ---------------------------------------------------------------------------
//	¥ GetPictureHandle												  [public]
// ---------------------------------------------------------------------------


PicHandle
LGAPictureMixin::GetPictureHandle() const
{
	PicHandle	pictH = mPictureH;

	if ( (pictH == nil) &&
		 (mPictureResID != 0) &&
		 (mPictureResID != resID_Undefined) ) {

		pictH = ::GetPicture(mPictureResID);
	}

	return pictH;
}


// ---------------------------------------------------------------------------
//	¥ GetPictureSize												  [public]
// ---------------------------------------------------------------------------

SDimension16
LGAPictureMixin::GetPictureSize() const
{
	SDimension16	pictureSize = { 0, 0 };

	PicHandle	pictH = GetPictureHandle();

	if (pictH != nil) {

		Rect	pictFrame;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
		QDGetPictureBounds(pictH, &pictFrame);
#else
		pictFrame = (**pictH).picFrame;
#endif

		pictureSize.width  = (SInt16) (pictFrame.right - pictFrame.left);
		pictureSize.height = (SInt16) (pictFrame.bottom - pictFrame.top);
	}

	return pictureSize;
}


// ---------------------------------------------------------------------------
//	¥ GetPictureFrame												  [public]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::GetPictureFrame(
	Rect&	outFrame)
{
	outFrame = Rect_0000;				// If there is no Picture

	PicHandle	pictH = GetPictureHandle();

	if (pictH != nil) {
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
		QDGetPictureBounds(pictH, &outFrame);
#else
		outFrame = (**pictH).picFrame;
#endif
	}
}


// ---------------------------------------------------------------------------
//	¥ SetPictureHandle												  [public]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::SetPictureHandle(
	PicHandle	inPictureH,
	bool		inDisposesHandle)
{
	if (mPictureH != inPictureH) {			// Setting to a different Picture
		DisposePictureHandle();				// Dispose current Picture
		mPictureH = inPictureH;
	}

	mDisposePictureH = inDisposesHandle;

	mPictureResID	 = 0;					// Don't use resource ID
}


// ---------------------------------------------------------------------------
//	¥ SetPictureResourceID											  [public]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::SetPictureResourceID(
	ResIDT	inResID)
{
	DisposePictureHandle();				// Dispose current Picture Handle

	mPictureResID = inResID;
}


// ---------------------------------------------------------------------------
//	¥ CalcLocalPictureRect											  [public]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::CalcLocalPictureRect(
	Rect&	/* outRect */)
{
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DisposePictureHandle											  [public]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::DisposePictureHandle()
{
										// Dispose Picture if we own it and
										//   it exists
	if (DisposesPictureH() && (mPictureH != nil)) {
		::DisposeHandle((Handle) mPictureH);
	}

	mPictureH = nil;
}


// ---------------------------------------------------------------------------
//	¥ CalcPictureLocation										   [protected]
// ---------------------------------------------------------------------------

void
LGAPictureMixin::CalcPictureLocation(
	Point&	/* outPictureLocation */)
{
}


PP_End_Namespace_PowerPlant
