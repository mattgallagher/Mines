// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIconControl.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LIconControl
#define _H_LIconControl
#pragma once

#include <LControlPane.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LIconControl : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('ictl'),
		   imp_class_ID	= FOUR_CHAR_CODE('iicl') };

						LIconControl(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LIconControl(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt16				inControlKind,
								ResIDT				inResID,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LIconControl();

	void				SetIconAlignment( IconAlignmentType inAlignment );

	IconAlignmentType	GetIconAlignment() const;

	void				SetIconTransform( IconTransformType inTransform );

	IconTransformType	GetIconTransform() const;

protected:
	virtual void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
