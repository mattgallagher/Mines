// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LWindowHeader.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LWindowHeader
#define _H_LWindowHeader
#pragma once

#include <LControlView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LWindowHeader : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('winh'),
		   imp_class_ID	= FOUR_CHAR_CODE('iwnh') };

						LWindowHeader(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LWindowHeader(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								bool				inListView = false,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LWindowHeader();

	virtual void		GetEmbeddedTextColor(
								SInt16				inDepth,
								bool				inHasColor,
								bool				inIsActive,
								RGBColor&			outColor) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
