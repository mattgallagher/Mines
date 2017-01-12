// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDisclosureTriangle.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LDisclosureTriangle
#define _H_LDisclosureTriangle
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LDisclosureTriangle : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('dtri'),
		   imp_class_ID	= FOUR_CHAR_CODE('idtr') };

						LDisclosureTriangle(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LDisclosureTriangle(
								const SPaneInfo&	inPaneInfo,
								MessageT			inValueMessage,
								SInt32				inValue = 0,
								bool				inFaceLeft = false,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LDisclosureTriangle();

protected:
	virtual void		HotSpotResult( SInt16 inHotSpot );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
