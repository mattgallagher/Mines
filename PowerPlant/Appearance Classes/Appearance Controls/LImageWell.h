// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LImageWell.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LImageWell
#define _H_LImageWell
#pragma once

#include <LControlView.h>
#include <Appearance.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LImageWell : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('iwel'),
		   imp_class_ID	= FOUR_CHAR_CODE('iiwl') };

						LImageWell(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LImageWell(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								MessageT			inValueMessage,
								SInt16				inContentType,
								SInt16				inContentResID,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LImageWell();

	void				SetContentInfo( ControlButtonContentInfo& inInfo );

	void				GetContentInfo( ControlButtonContentInfo& outInfo ) const;

	void				SetTransform( IconTransformType inTransform );

	IconTransformType	GetTransform() const;

	virtual void		HotSpotResult( SInt16 inHotSpot );

private:
						LImageWell();
						LImageWell( const LImageWell& );
		LImageWell&		operator = ( const LImageWell& );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
