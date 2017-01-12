// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPlacardImp.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAPlacardImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>
#include <LView.h>
#include <LStream.h>
#include <UDrawingState.h>

#include <MacWindows.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGAPlacardImp							Constructor				  [public]
// ---------------------------------------------------------------------------

LGAPlacardImp::LGAPlacardImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
}


// ---------------------------------------------------------------------------
//	¥ LGAPlacardImp							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAPlacardImp::LGAPlacardImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ ~LGAPlacardImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGAPlacardImp::~LGAPlacardImp()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGAPlacardImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitsID;

	*inStream >> controlKind;
	*inStream >> textTraitsID;
	inStream->ReadPString(title);

	Init(inControlPane, controlKind, title, textTraitsID);
}


// ---------------------------------------------------------------------------
//	¥ Init									Parameterized Initializer [public]
// ---------------------------------------------------------------------------

void
LGAPlacardImp::Init(
	LControlPane*	inControlPane,
	SInt16			inControlKind,
	ConstStringPtr	inTitle,
	ResIDT			inTextTraitsID,
	SInt32			inRefCon)
{
	LGAControlImp::Init(inControlPane, inControlKind, inTitle,
							inTextTraitsID, inRefCon);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ GetForeAndBackColors											  [public]
// ---------------------------------------------------------------------------

bool
LGAPlacardImp::GetForeAndBackColors(
	RGBColor*	outForeColor,
	RGBColor*	outBackColor) const
{
	if (outForeColor != nil) {
		*outForeColor = Color_Black;
	}

	if (outBackColor != nil) {
		*outBackColor = IsEnabled() && IsActive() ?
								UGAColorRamp::GetColor(colorRamp_Gray2) :
								UGAColorRamp::GetColor(colorRamp_Gray1);
	}

	return true;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------
//

void
LGAPlacardImp::DrawSelf()
{
	StColorPenState::Normalize();

	Rect	frameRect;
	CalcLocalFrameRect ( frameRect );

	bool	enabledAndActive = IsActive() && IsEnabled();

										// BORDER
	RGBColor	tempColor = Color_Black;
	if ( (mControlPane->GetBitDepth() >= 4)  &&  !enabledAndActive ) {

		tempColor = UGAColorRamp::GetColor(colorRamp_Gray10);
	}

	::RGBForeColor ( &tempColor );
	::MacFrameRect ( &frameRect );

										// FACE
										// BLACK & WHITE
	if ( mControlPane->GetBitDepth() < 4 ) {
												// The face is painted in white
		::RGBForeColor ( &Color_White );
		::MacInsetRect ( &frameRect, 1, 1 );
		::PaintRect ( &frameRect );
		::MacInsetRect ( &frameRect, -1, -1 );

	} else {							// COLOR
										// The face is painted with the face color if enabled
										// and active otherwise it is painted with the
										// standard AGA color gray1
		tempColor = enabledAndActive ? UGAColorRamp::GetColor(colorRamp_Gray2)
									 : UGAColorRamp::GetColor(colorRamp_Gray1);
		::RGBForeColor ( &tempColor );
		::MacInsetRect ( &frameRect, 1, 1 );
		::PaintRect ( &frameRect );

										// The shadows are only drawn if we are enabled and
										// active
		if (enabledAndActive) {
										// Frame the light portion of the shading which is in
										// white
			::RGBForeColor ( &Color_White );
			UGraphicUtils::TopLeftSide ( frameRect, 0, 0, 1, 0 );

										// Frame the dark portion of the shading which is
										// done with a gray5 shade
			tempColor = UGAColorRamp::GetColor(colorRamp_Gray5);
			::RGBForeColor ( &tempColor );
			UGraphicUtils::BottomRightSide ( frameRect, 1, 1, 0, 0 );
		}
	}
}


PP_End_Namespace_PowerPlant
