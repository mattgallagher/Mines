// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextButton.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A button that behaves like a radio button, but displays text
//	(like the sort… buttons in the Finder).

#ifndef _H_LTextButton
#define _H_LTextButton
#pragma once

#include <LControl.h>
#include <LString.h>
#include <UTextTraits.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LTextButton : public LControl {
public:
	enum { class_ID = FOUR_CHAR_CODE('txbt') };

							LTextButton();

							LTextButton( LStream* inStream );

	virtual					~LTextButton();

	virtual StringPtr		GetDescriptor( Str255 outDescriptor ) const;

	virtual void			SetDescriptor( ConstStringPtr inDescriptor );

	virtual void			SetValue( SInt32 inValue );

protected:
	LStr255				mText;					// Displayed text
	ResIDT				mTextTraitsID;			// Format of text
	SInt16				mSelectedStyle;			// Format when selected

	virtual void			DrawSelf();

	virtual void			HotSpotAction(
									SInt16			inHotSpot,
									Boolean			inCurrInside,
									Boolean			inPrevInside);

	virtual void			DoneTracking(
									SInt16			inHotSpot,
									Boolean			inGoodTrack);

	virtual void			HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
