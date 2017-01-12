// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPicture.cp					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Displays a 'PICT' resource
//
//	LPicture stores the ID of a PICT resource as a member variable, and
//	always calls GetPicture() to get a Handle to the picture. It purposely
//	does not release the PICT resource when deleted, since other LPicture
//	views may be using the same picture. For example, you might use the
//	same picture as a background or graphic element in multiple windows.
//
//	If you are concerned about the memory used by the PICT resource, mark
//	it as purgeable in your resource file.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPicture.h>
#include <LStream.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPicture								Default Constructor		  [public]
// ---------------------------------------------------------------------------

LPicture::LPicture()
{
	mPICTid = resID_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LPicture								Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LPicture::LPicture(
	const LPicture&		inOriginal)

	: LView(inOriginal)
{
	mPICTid = inOriginal.mPICTid;
	InitPicture();
}


// ---------------------------------------------------------------------------
//	¥ LPicture								Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LPicture::LPicture(
	const SPaneInfo&	inPaneInfo,
	const SViewInfo&	inViewInfo,
	ResIDT				inPICTid)

	: LView(inPaneInfo, inViewInfo)
{
	mPICTid = inPICTid;
	InitPicture();
}


// ---------------------------------------------------------------------------
//	¥ LPicture								Stream Constructor	 	 [public]
// ---------------------------------------------------------------------------

LPicture::LPicture(
	LStream*	inStream)

	: LView(inStream)
{
	*inStream >> mPICTid;
	InitPicture();
}


// ---------------------------------------------------------------------------
//	¥ LPicture								Constructor				  [public]
// ---------------------------------------------------------------------------

LPicture::LPicture(
	ResIDT	inPictureID)			// 'PICT" Resource ID
{
	mPICTid = inPictureID;
	InitPicture();
}


// ---------------------------------------------------------------------------
//	~LPicture								Destructor				  [public]
// ---------------------------------------------------------------------------

LPicture::~LPicture()
{
}


// ---------------------------------------------------------------------------
//	¥ InitPicture													 [private]
// ---------------------------------------------------------------------------
//	Private Initializer. Assumes mPICTid is set.

void
LPicture::InitPicture()
{
	mRefreshAllWhenResized = false;

		// If the PICT Resource exists (can be loaded), set the Image
		// size to the size of the picture.

	PicHandle	macPictureH = ::GetPicture(mPICTid);
	if (macPictureH != nil) {
		Rect	picFrame;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
		QDGetPictureBounds(macPictureH, &picFrame);
#else
		picFrame = (**macPictureH).picFrame;
#endif
		ResizeImageTo(picFrame.right - picFrame.left,
					  picFrame.bottom - picFrame.top, false);
	}
}


// ---------------------------------------------------------------------------
//	¥ GetPictureID													  [public]
// ---------------------------------------------------------------------------
//	Return the PICT Resource ID associated with a Picture
//	Defined inline in header

#pragma mark LPicture::GetPictureID


// ---------------------------------------------------------------------------
//	¥ SetPictureID													  [public]
// ---------------------------------------------------------------------------
//	Set the PICT Resource ID associated with a Picture
//
//	This changes size of the Image to match the bounds of the PICT.

void
LPicture::SetPictureID(
	ResIDT	inPictureID)
{
	mPICTid = inPictureID;
	InitPicture();
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf													   [protected]
// ---------------------------------------------------------------------------
//	Draw a Picture

void
LPicture::DrawSelf()
{
		// If Picture resource exists, draw it. Otherwise, fill the
		// Frame with a light gray pattern and a one-pixel border.

	PicHandle	macPictureH = ::GetPicture(mPICTid);
	if (macPictureH != nil) {

		SDimension32	imageSize;
		GetImageSize(imageSize);

		Rect	pictureBounds;
		pictureBounds.left   = 0;
		pictureBounds.top    = 0;
		pictureBounds.right  = (SInt16) imageSize.width;
		pictureBounds.bottom = (SInt16) imageSize.height;

		::DrawPicture(macPictureH, &pictureBounds);

	} else {
		Rect	frame;
		CalcLocalFrameRect(frame);
		::PenNormal();

		Pattern		ltGrayPat;
		::MacFillRect(&frame, UQDGlobals::GetLightGrayPat(&ltGrayPat));

		::MacFrameRect(&frame);
	}
}


PP_End_Namespace_PowerPlant
