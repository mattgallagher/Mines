// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSlider.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSlider
#define _H_LSlider
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LSlider : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('slid'),
		   imp_class_ID	= FOUR_CHAR_CODE('isld') };

						LSlider(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LSlider(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue,
								bool				inLiveFeedback = false,
								bool				inNonDirectional = false,
								bool				inReverseDirection = false,
								bool				inHasTickMarks = false,
								SInt16				inNumTickMarks = 0,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LSlider();

	virtual void		DoTrackAction(
								SInt16			inHotSpot,
								SInt32			inValue);

private:
	void				InitSlider();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
