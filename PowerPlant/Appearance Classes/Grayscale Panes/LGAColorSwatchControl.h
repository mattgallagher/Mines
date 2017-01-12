// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAColorSwatchControl.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair
//
//	Control for displaying a color that the user can alter by calling up
//	the System Color Picker.

#ifndef _H_LGAColorSwatchControl
#define _H_LGAColorSwatchControl
#pragma once

#include <LControl.h>
#include <LString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAColorSwatchControl : public LControl {
public:
	enum	{ class_ID = FOUR_CHAR_CODE('cswt') };

							LGAColorSwatchControl();

							LGAColorSwatchControl(
									const LGAColorSwatchControl& inOriginal);

							LGAColorSwatchControl(
									LStream*			inStream);

							LGAColorSwatchControl(
									const SPaneInfo&	inPaneInfo,
									const SControlInfo&	inControlInfo,
									const RGBColor&		inSwatchColor,
									Str255				inPromptString);

	virtual					~LGAColorSwatchControl();

	void					GetSwatchColor(
									RGBColor	&outColor) const
								{
									outColor = mSwatchColor;
								}

	virtual StringPtr		GetDescriptor(
									Str255				outDescriptor) const;

	bool					IsPushed() const		{ return mPushed; }

	virtual	void			SetSwatchColor(
									const RGBColor&		inSwatchColor);

	virtual void			SetDescriptor(
									ConstStringPtr		inDescriptor);

	virtual	void			SetPushedState(
									Boolean				inPushedState);

	virtual	void			EnableSelf();
	virtual	void			DisableSelf();

	virtual void			ActivateSelf();
	virtual void			DeactivateSelf();

	virtual	void			DrawSelf();

protected:
	RGBColor	mSwatchColor;		//	The color being displayed by the swatch
	LStr255		mPromptString;		//	Prompt string for color picker dialog
	bool		mPushed;			//	Used when we are tracking the control


	virtual	void			HotSpotAction(
										SInt16			inHotSpot,
										Boolean			inCurrInside,
										Boolean			inPrevInside);

	virtual	void			DoneTracking(
										SInt16			inHotSpot,
										Boolean			inGoodTrack);

	virtual	void			BroadcastValueMessage();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
