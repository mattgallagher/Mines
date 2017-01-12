// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPictureControl.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPictureControl
#define _H_LPictureControl
#pragma once

#include <LControlView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPictureControl : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('picd'),
		   imp_class_ID	= FOUR_CHAR_CODE('ipcd') };

						LPictureControl(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LPictureControl(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								MessageT			inMessage,
								bool				inTracking,
								ResIDT				inPICTResID,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LPictureControl();

	virtual void		HotSpotResult( SInt16 inHotSpot );

private:
						LPictureControl();
						LPictureControl( const LPictureControl& );
	LPictureControl&	operator = ( const LPictureControl& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
