// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOverlappingView.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOverlappingView
#define _H_LOverlappingView
#pragma once

#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LOverlappingView : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('ovlv') };

						LOverlappingView();

						LOverlappingView( const LOverlappingView& inOriginal );

						LOverlappingView(
							const SPaneInfo&	inPaneInfo,
							const SViewInfo&	inViewInfo);

						LOverlappingView( LStream* inStream );

	virtual				~LOverlappingView();

	virtual Boolean		FocusDraw(LPane *inSubPane = nil);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
