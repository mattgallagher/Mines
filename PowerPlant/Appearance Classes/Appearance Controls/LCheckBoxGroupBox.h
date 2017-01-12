// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCheckBoxGroupBox.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LCheckBoxGroupBox
#define _H_LCheckBoxGroupBox
#pragma once

#include <LControlView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCheckBoxGroupBox : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('cbgb'),
		   imp_class_ID	= FOUR_CHAR_CODE('icbg') };

						LCheckBoxGroupBox(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LCheckBoxGroupBox(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								MessageT			inValueMessage,
								SInt32				inValue,
								Boolean				inPrimary = true,
								ResIDT				inTextTraitsID = 0,
								ConstStringPtr		inTitle = Str_Empty,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LCheckBoxGroupBox();

	virtual void		SetValue( SInt32 inValue );

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

protected:
	virtual void		FinishCreateSelf();

	virtual void		HotSpotResult( SInt16 inHotSpot );

	virtual void		AdjustSubPanes( bool inEnable );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
