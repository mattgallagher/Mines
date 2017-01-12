// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LThemeTextBox.h				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LThemeTextBox
#define _H_LThemeTextBox
#pragma once

#include <LPane.h>
#include <LCFString.h>
#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LThemeTextBox : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('ttxb') };
	
						LThemeTextBox();
						
						LThemeTextBox(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inString,
								ThemeFontID			inThemeFontID);
								
						LThemeTextBox( LStream* inStream );
								
	virtual				~LThemeTextBox();
	
protected:
	virtual void		ActivateSelf();
	
	virtual void		DeactivateSelf();
	
	virtual void		EnableSelf();
	
	virtual void		DisableSelf();

	virtual void		DrawSelf();
	
protected:
	LCFString			mString;
	ThemeFontID			mThemeFontID;
	ResIDT				mTextTraitsID;
	bool				mDimWhenInactive;
	bool				mDimWhenDisabled;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif
