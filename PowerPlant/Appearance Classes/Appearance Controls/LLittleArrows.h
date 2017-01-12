// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LLittleArrows.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LLittleArrows
#define _H_LLittleArrows
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LLittleArrows : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('larr'),
		   imp_class_ID	= FOUR_CHAR_CODE('ilar') };

						LLittleArrows(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LLittleArrows(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LLittleArrows();

	virtual void		DoTrackAction(
								SInt16			inHotSpot,
								SInt32			inValue);

private:
	void				InitLittleArrows();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
