// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGASeparatorImp.cp			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGASeparatorImp.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LStream.h>
#include <UDrawingState.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LGASeparatorImp						Constructor				  [public]
// ---------------------------------------------------------------------------

LGASeparatorImp::LGASeparatorImp(
	LControlPane*	inControlPane)

	: LGAControlImp(inControlPane)
{
	mHorizontal = true;
}


// ---------------------------------------------------------------------------
//	¥ LGASeparatorImp						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGASeparatorImp::LGASeparatorImp(
	LStream*	inStream)

	: LGAControlImp(inStream)
{
	mHorizontal = true;
}


// ---------------------------------------------------------------------------
//	¥ ~LGASeparatorImp						Destructor				  [public]
// ---------------------------------------------------------------------------

LGASeparatorImp::~LGASeparatorImp ()
{
}


// ---------------------------------------------------------------------------
//	¥ Init															  [public]
// ---------------------------------------------------------------------------

void
LGASeparatorImp::Init(
	LControlPane*	inControlPane,
	LStream*		inStream)
{
	SInt16	controlKind;
	Str255	title;
	ResIDT	textTraitID;

	*inStream >> controlKind;
	*inStream >> textTraitID;
	inStream->ReadPString(title);

	mControlPane = inControlPane;
}


// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGASeparatorImp::DrawSelf()
{
	Rect	frameRect;
	CalcLocalFrameRect ( frameRect );

	mHorizontal = (frameRect.right - frameRect.left) >	// Determine orientation
				  (frameRect.bottom - frameRect.top);

	RGBColor	tempColor;
	SInt8		index;
								// Calculate width and height of the pane
	SInt16		width  = UGraphicUtils::RectWidth ( frameRect );
	SInt16		height = UGraphicUtils::RectHeight ( frameRect );


	StColorPenState::Normalize ();

	if ( mControlPane->GetBitDepth() < 4 ) {	// BLACK & WHITE
		StColorPenState::SetGrayPattern();
		if ( IsHorizontal ()) {
			::MoveTo ( frameRect.left, (SInt16) (frameRect.top + (height / 2)));
			::Line ( (SInt16) (width - 1), 0 );
		} else {
			::MoveTo ( (SInt16) (frameRect.left + (width / 2)), frameRect.top );
			::Line ( 0, (SInt16) (height - 1) );
		}

	} else {									// COLOR

		if ( IsHorizontal ()) {					// Horizontal
													// Dark line
			index = colorRamp_Gray4;
			if (IsEnabled () && IsActive ()) {
				index = colorRamp_Gray7;
			}
			UGAColorRamp::GetColor(index, tempColor);

			::RGBForeColor ( &tempColor );
			::MoveTo ( frameRect.left, (SInt16) (frameRect.top + (height / 2)));
			::Line ( (SInt16) (width - 2), 0 );

													// Light line
			index = colorRamp_Gray2;
			if (IsEnabled () && IsActive ()) {
				index = colorRamp_White;
			}
			UGAColorRamp::GetColor(index, tempColor);

			::RGBForeColor ( &tempColor );
			::MoveTo ( (SInt16) (frameRect.left + 1), (SInt16) (frameRect.top + (height / 2) + 1) );
			::Line ( (SInt16) (width - 2), 0 );

		} else  {								// Vertical
													// Dark line

			index = colorRamp_Gray4;
			if (IsEnabled () && IsActive ()) {
				index = colorRamp_Gray7;
			}
			UGAColorRamp::GetColor(index, tempColor);

			::RGBForeColor ( &tempColor );
			::MoveTo ( (SInt16) (frameRect.left + (width / 2)), frameRect.top );
			::Line ( 0, (SInt16) (height - 2) );

													// Light line
			index = colorRamp_Gray2;
			if (IsEnabled () && IsActive ()) {
				index = colorRamp_White;
			}
			UGAColorRamp::GetColor(index, tempColor);

			::RGBForeColor ( &tempColor );
			::MoveTo ( (SInt16) (frameRect.left + (width / 2) + 1), (SInt16) (frameRect.top + 1) );
			::Line ( 0, (SInt16) (height - 2) );
		}
	}
}


PP_End_Namespace_PowerPlant
