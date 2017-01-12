// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LScrollBar.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LScrollBar
#define _H_LScrollBar
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LScrollBar : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('sbar'),
		   imp_class_ID	= FOUR_CHAR_CODE('isba') };

						LScrollBar(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LScrollBar(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								SInt32				inMinValue,
								SInt32				inMaxValue,
								bool				inLiveScrolling = false,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LScrollBar();

	virtual void		DoTrackAction(
								SInt16				inHotSpot,
								SInt32				inValue);

	void				SetScrollViewSize( SInt32 inViewSize );

	struct SScrollMessage {
		LScrollBar*	scrollBar;
		SInt16		hotSpot;
		SInt32		value;
	};

protected:
	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();

	virtual void		DoneTracking(
								SInt16				inHotSpot,
								Boolean				inGoodTrack);

private:
	void				InitScrollBar();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
