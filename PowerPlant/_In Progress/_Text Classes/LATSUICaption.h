// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LATSUICaption.h				PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LATSUICaption
#define _H_LATSUICaption
#pragma once

#include <LPane.h>
#include <LString.h>
#include <UATSUI.h>

#include <CFString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LATSUICaption : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('ucap') };
	
						LATSUICaption();
						
						LATSUICaption(
								const SPaneInfo&		inPaneInfo,
								ConstStringPtr			inString);
								
						LATSUICaption( LStream* inStream );
								
	virtual				~LATSUICaption();
	
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
	
	ATSUTextMeasurement		mBeforeLocation;
	ATSUTextMeasurement		mBaselineLocation;	// Distance from top of frame
												//   to text baseline
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif


#endif
