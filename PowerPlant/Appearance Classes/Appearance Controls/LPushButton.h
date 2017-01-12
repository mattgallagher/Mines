// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPushButton.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPushButton
#define _H_LPushButton
#pragma once

#include <LControlPane.h>
#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPushButton : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('push'),
		   imp_class_ID	= FOUR_CHAR_CODE('ipus') };

						LPushButton(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LPushButton(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								ConstStringPtr		inTitle,
								SInt16				inKind = kControlPushButtonProc,
								ResIDT				inTextTraitsID = 0,
								SInt16				inCicnID = 0,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LPushButton();

	void				SetDefaultButton( Boolean inDefault );

	Boolean				IsDefaultButton() const;

protected:
	virtual void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
