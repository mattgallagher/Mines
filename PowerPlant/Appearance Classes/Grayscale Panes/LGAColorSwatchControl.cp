// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAColorSwatchControl.cp	PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair
//
//	Control for displaying a color that the user can alter by calling up
//	the System Color Picker.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LGAColorSwatchControl.h>
#include <UGraphicUtils.h>
#include <UGAColorRamp.h>

#include <LStream.h>
#include <UDrawingUtils.h>
#include <UDesktop.h>
#include <UQDOperators.h>
#include <PP_Messages.h>

#include <ColorPicker.h>

PP_Begin_Namespace_PowerPlant

#pragma mark === INITIALIZATION

// ---------------------------------------------------------------------------
//	¥ LGAColorSwatchControl					Default Constructor		  [public]
// ---------------------------------------------------------------------------

LGAColorSwatchControl::LGAColorSwatchControl()
{
	mSwatchColor = Color_White;
	mPushed		 = false;
}


// ---------------------------------------------------------------------------
//	¥ LGAColorSwatchControl					Copy Constructor		  [public]
// ---------------------------------------------------------------------------

LGAColorSwatchControl::LGAColorSwatchControl(
	const LGAColorSwatchControl&	inOriginal)

	: LControl(inOriginal),
	  mPromptString(inOriginal.mPromptString)
{
	mSwatchColor = inOriginal.mSwatchColor;
	mPushed		 = inOriginal.mPushed;
}


// ---------------------------------------------------------------------------
//	¥ LGAColorSwatchControl					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LGAColorSwatchControl::LGAColorSwatchControl(
	LStream*	inStream)

	: LControl(inStream)
{
	*inStream >> mSwatchColor.red;
	*inStream >> mSwatchColor.green;
	*inStream >> mSwatchColor.blue;

	inStream->ReadPString(mPromptString);

	mPushed = false;
}


// ---------------------------------------------------------------------------
//	¥ LGAColorSwatchControl					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LGAColorSwatchControl::LGAColorSwatchControl(
	const SPaneInfo&	inPaneInfo,
	const SControlInfo&	inControlInfo,
	const RGBColor&		inSwatchColor,
	Str255				inPromptString)

	: LControl(inPaneInfo,
				inControlInfo.valueMessage,
				inControlInfo.value,
				inControlInfo.minValue,
				inControlInfo.maxValue),

	  mPromptString(inPromptString)

{
	mSwatchColor  = inSwatchColor;
	mPushed		  = false;
}


// ---------------------------------------------------------------------------
//	¥ ~LGAColorSwatchControl				Destructor				  [public]
// ---------------------------------------------------------------------------

LGAColorSwatchControl::~LGAColorSwatchControl ()
{
}


#pragma mark -
#pragma mark === ACCESSORS

// ---------------------------------------------------------------------------
//	¥ GetDescriptor													  [public]
// ---------------------------------------------------------------------------

StringPtr
LGAColorSwatchControl::GetDescriptor(
	Str255	outDescriptor) const
{
	return LString::CopyPStr(mPromptString, outDescriptor);
}


// ---------------------------------------------------------------------------
//	¥ SetSwatchColor												  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::SetSwatchColor(
	const RGBColor&		inSwatchColor)
{
												// We only adjust the color if we have to
	if (mSwatchColor != inSwatchColor) {
												// Update to the new color
		mSwatchColor = inSwatchColor;
												// The color changed so we have to redraw the swatch
		Draw(nil);
												// Finally we broadcast the new color to any
												// listeners
		BroadcastValueMessage();
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDescriptor													  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::SetDescriptor (
	ConstStringPtr	 inDescriptor )
{
	mPromptString = inDescriptor;
}


// ---------------------------------------------------------------------------
//	¥ SetPushedState												  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::SetPushedState(
	Boolean	 inPushedState )
{
	if (mPushed != inPushedState) {
		mPushed = inPushedState;
		Draw(nil);
	}
}


#pragma mark -
#pragma mark === ENABLING & DISABLING

// ---------------------------------------------------------------------------
//	¥ EnableSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::EnableSelf()
{
	Draw(nil);
}


// ---------------------------------------------------------------------------
//	¥ DisableSelf													  [public]
// ---------------------------------------------------------------------------
//

void
LGAColorSwatchControl::DisableSelf()
{
	Draw(nil);
}


#pragma mark -
#pragma mark === ACTIVATION

// ---------------------------------------------------------------------------
//	¥ ActivateSelf													  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::ActivateSelf()
{
	Refresh();
}


// ---------------------------------------------------------------------------
//	¥ DeactivateSelf												  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::DeactivateSelf()
{
	Refresh();
}


#pragma mark -
#pragma mark === DRAWING

// ---------------------------------------------------------------------------
//	¥ DrawSelf														  [public]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::DrawSelf ()
{
	::PenNormal();
	ApplyForeAndBackColors ();

	Rect frame;
	CalcLocalFrameRect ( frame );

	::EraseRect ( &frame );						// Start out by erasing the whole control

	RGBColor	tempColor;
	SInt16		depth;
	bool		hasColor;
	GetDeviceInfo(depth, hasColor);
	
	bool		enabledAndActive = IsEnabled() && IsActive();

	Rect		localFrame = frame;

	::MacInsetRect ( &localFrame, 1, 1 );

											// First we are going to draw a little shadow
											// under the color swatch
	::MacOffsetRect ( &localFrame, 1, 1 );

	tempColor = Color_Black;
	if (depth >= 4) {
		tempColor = enabledAndActive ? UGAColorRamp::GetColor(6)
									 : UGAColorRamp::GetColor(4);
	}

	::RGBForeColor ( &tempColor );

	if ( depth < 4 ) {
		StColorPenState::SetGrayPattern();
	}
	::PaintRect ( &localFrame );
	::MacOffsetRect ( &localFrame, -1, -1 );
	::PaintRect ( &localFrame );
	::MacOffsetRect ( &localFrame, -1, -1 );

											// Then we paint the swatch color, either hilited
											// if it is being pushed, normal or dimmed
	if ( depth >= 4 ) {				// COLOR

		::PenNormal ();
											// Setup the swatch color
		if ( IsEnabled () && IsPushed ()) {
			::MacOffsetRect ( &localFrame, 2, 2 );

			if ( mSwatchColor == Color_Black) {
				tempColor = Color_White;

			} else {
				tempColor = UGraphicUtils::Darken ( mSwatchColor );
			}

		} else {
			tempColor = mSwatchColor;
			if (!enabledAndActive) {
				UTextDrawing::DimTextColor(tempColor);
			}
		}

		::RGBForeColor ( &tempColor );
		::PaintRect ( &localFrame );

											// Now we frame the color swatch
		tempColor = UGAColorRamp::GetColor(7);
		if (enabledAndActive) {
			tempColor = Color_Black;
		}

		::RGBForeColor ( &tempColor );
		::MacFrameRect ( &localFrame );

	} else {								// BLACK & WHITE

		::RGBForeColor ( &Color_Black );
		if (enabledAndActive) {
			StColorPenState::SetGrayPattern();
		}
		::MacFrameRect ( &localFrame );

										// Paint the swatch color
		::MacInsetRect ( &localFrame, 2, 2 );
		::RGBForeColor ( &mSwatchColor );
		::PaintRect ( &localFrame );
	}
}


#pragma mark -
#pragma mark === MOUSE TRACKING

// ---------------------------------------------------------------------------
//	¥ HotSpotAction												   [protected]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::HotSpotAction(
	SInt16		/* inHotSpot */,
	Boolean		inCurrInside,
	Boolean		inPrevInside)
{
												// If the mouse moved in or out of the hot spot
												// handle the hiliting of the swatch
	if ( inCurrInside != inPrevInside ) {
												// We act like a button so we simply toggle the
												// state
		SetPushedState ( inCurrInside );
	}
}


// ---------------------------------------------------------------------------
//	¥ DoneTracking												   [protected]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::DoneTracking(
	SInt16		/*inHotSpot*/,
	Boolean		inGoodTrack)
{
												// Make sure the hilite state gets turned off
	SetPushedState ( false );

												// Display the color picker and allow the user to
												// select a new color if the color changes then we
												// set the swatch to the new color and get it
												// redrawn, we also have to make sure we deactivate
												// the desktop so that any windows behind the
												// picker are disabled.
	if ( inGoodTrack ) {
		RGBColor	outColor;
		Str255		prompt;
		GetDescriptor ( prompt );
		UDesktop::Deactivate ();

		if ( ::GetColor ( Point_00, prompt, &mSwatchColor, &outColor )) {
			SetSwatchColor ( outColor );
		}

		UDesktop::Activate ();
	}
}


#pragma mark -
#pragma mark === BROADCASTING

// ---------------------------------------------------------------------------
//	¥ BroadcastValueMessage										   [protected]
// ---------------------------------------------------------------------------

void
LGAColorSwatchControl::BroadcastValueMessage ()
{
	if (mValueMessage != msg_Nothing)  {
		BroadcastMessage(mValueMessage, (void*) &mSwatchColor);
	}
}


PP_End_Namespace_PowerPlant
