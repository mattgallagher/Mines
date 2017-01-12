// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTextGroupBox.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTextGroupBox
#define _H_LTextGroupBox
#pragma once

#include <LControlView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTextGroupBox : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('tgbx'),
		   imp_class_ID	= FOUR_CHAR_CODE('itgb') };

						LTextGroupBox(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LTextGroupBox(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								Boolean				inPrimary = true,
								ResIDT				inTextTraitsID = 0,
								ConstStringPtr		inTitle = Str_Empty,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LTextGroupBox();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
