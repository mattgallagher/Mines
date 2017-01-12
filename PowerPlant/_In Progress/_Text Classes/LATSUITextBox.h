// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LATSUITextBox.h				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LATSUITextBox
#define _H_LATSUITextBox
#pragma once

#include <LPane.h>
#include <LString.h>
#include <UATSUI.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LATSUITextBox : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('utbx') };
	
						LATSUITextBox();
						
						LATSUITextBox(
								const SPaneInfo&		inPaneInfo,
								ConstStringPtr			inString);
								
						LATSUITextBox( LStream* inStream );
								
	virtual				~LATSUITextBox();
	
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
#if PP_Uses_CFDescriptor
	virtual void		SetCFDescriptor( CFStringRef inString );
#endif
	
protected:
	virtual void		FinishCreateSelf();

	virtual void		DrawSelf();
	
	virtual void		PrepareTextLayout();
	
protected:
	LATSUITextLayout	mTextLayout;
	LATSUIStyle			mStyle;
	
	UniChar*			mTextPtr;
	UniCharCount		mTextLength;
	
	ItemCount				mLineCount;
	UniCharArrayOffset*		mLineBreaks;
	ATSUTextMeasurement*	mLineHeights;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif
