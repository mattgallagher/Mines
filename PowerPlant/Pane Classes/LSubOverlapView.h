// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSubOverlapView.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	Container View for Panes which might overlap.

#ifndef _H_LSubOverlapView
#define _H_LSubOverlapView
#pragma once

#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LSubOverlapView : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('solv') };

						LSubOverlapView();

						LSubOverlapView( const LSubOverlapView& inOriginal );

						LSubOverlapView(
							const SPaneInfo&	inPaneInfo,
							const SViewInfo&	inViewInfo);

						LSubOverlapView( LStream* inStream );

	virtual				~LSubOverlapView();

	virtual Boolean		FocusDraw( LPane* inSubPane = nil );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
