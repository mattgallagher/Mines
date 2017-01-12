// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSeparatorLine.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSeparatorLine
#define _H_LSeparatorLine
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LSeparatorLine : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('sepl'),
		   imp_class_ID	= FOUR_CHAR_CODE('isep') };

						LSeparatorLine(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LSeparatorLine(
								const SPaneInfo&	inPaneInfo,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LSeparatorLine();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
