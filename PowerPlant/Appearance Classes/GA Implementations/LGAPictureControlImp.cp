// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPictureControlImp.cp		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	This is an implementation of the Appearance Manager's "Picture CDEF"
//	control.
//
//	Similar to LPicture, we just store the ResIDT of a 'PICT' resource and
//	always call ::GetPicture() when we need to draw. This class purposefully
//	does not release the PICT resource as this could cause problems (e.g.
//	the same resource used in multiple locations).
//
//	To contend with the memory claimed by the 'PICT' resource, you should
//	mark your resource as purgeable.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPictureControlImp.h>
#include <LStream.h>
#include <UDrawingUtils.h>
#include <UDrawingState.h>
#include <UGAColorRamp.h>
#include <UGraphicUtils.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAPictureControlImp					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPictureControlImp::LGAPictureControlImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mPICTid = resID_Undefined;
	mTrack  = true;
}


// ---------------------------------------------------------------------------
//	¥ LGAPictureControlImp					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LGAPictureControlImp::LGAPictureControlImp(
	LControlPane*	inControlPane,
	ResIDT			inPICTid)

	: LGAControlImp(inControlPane)
{
	mPICTid = inPICTid;
	mTrack  = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAPictureControlImp					Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPictureControlImp::~LGAPictureControlImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init									Stream Initializer		  [public]
// ---------------------------------------------------------------------------

void
LGAPictureControlImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString ( title );

	Init( inControlPane, controlKind );
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initilizer  [public]
// ---------------------------------------------------------------------------

void
LGAPictureControlImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	/* inTitle */,
	ResIDT			/* inTextTraitsID */,
	SInt32			/* inRefCon */)
{
	mControlPane = inControlPane;
	Assert_(mControlPane != nil);

	if ( inControlKind == kControlPictureProc ) {
		mTrack = true;
	} else if ( inControlKind == kControlPictureNoTrackProc ) {
		mTrack = false;
	} else {
		SignalStringLiteral_("Unknown controlKind");
		mTrack = false;
	}

	// the Appearance Manager uses a control's value to specify
	// the ResIDT of the PICT for the control. We'lll just store
	// a local copy for convenience.

	mPICTid = (ResIDT) mControlPane->GetValue();

	mControlPane->SetRefreshAllWhenResized(Refresh_No);
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------
//	Do the cosmetic renderings

void
LGAPictureControlImp::DrawSelf()
{
	Rect	frame;
	CalcLocalFrameRect(frame);

	PicHandle thePictH = ::GetPicture(mPICTid);

	if (thePictH != nil) {

		SInt16	transform = kTransformNone;
		if (IsActive() && IsEnabled()) {
			if (mTrack && IsPushed()) {
				transform = kTransformSelected;
			}
		} else {
			transform = kTransformDisabled;
		}

		UGraphicUtils::DrawPictGraphic(thePictH, frame, transform,
										mControlPane->GetBitDepth());

	} else {					// No Picture. Draw a gray rectangle.
		::PenNormal();

		Pattern		ltGrayPat;
		::MacFillRect(&frame, UQDGlobals::GetLightGrayPat(&ltGrayPat));

		::MacFrameRect(&frame);
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking													  [public]
// ---------------------------------------------------------------------------

void
LGAPictureControlImp::DoneTracking(
	SInt16		/* inHotSpot */,
	Boolean		/* inGoodTrack */)
{
	SetPushedState(false);
}


PP_End_Namespace_PowerPlant
