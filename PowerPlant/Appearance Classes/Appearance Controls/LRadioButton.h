// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRadioButton.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LRadioButton
#define _H_LRadioButton
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LRadioButton : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('rdbt'),
		   imp_class_ID	= FOUR_CHAR_CODE('irad') };

						LRadioButton(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LRadioButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage = 0,
								SInt32				inValue = 0,
								ConstStringPtr		inTitle = Str_Empty,
								ResIDT				inTextTraitsID = 0,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LRadioButton();

	virtual void		SetValue( SInt32 inValue );

protected:
	virtual void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
