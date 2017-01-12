// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabsControl.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LTabsControl
#define _H_LTabsControl
#pragma once

#include <LControlView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LTabsControl : public LControlView {
public:
	enum { class_ID		= FOUR_CHAR_CODE('tabs'),
		   imp_class_ID	= FOUR_CHAR_CODE('itab') };

						LTabsControl(
								LStream*			inStream,
								ClassIDT			inImpID = imp_class_ID);

						LTabsControl(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								MessageT			inMessage,
								SInt16				inControlKind,
								ResIDT				inTabxID,
								SInt16				inInitialValue,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LTabsControl();

	void				EnableTab(
								SInt16				inTabIndex,
								Boolean				inEnabled);

	Boolean				IsTabEnabled( SInt16 inTabIndex ) const;

	virtual void		GetEmbeddedTextColor(
								SInt16				inDepth,
								bool				inHasColor,
								bool				inIsActive,
								RGBColor&			outColor) const;

private:
						LTabsControl();
						LTabsControl( const LTabsControl& );
	LTabsControl&		operator = ( const LTabsControl& );

	void				InitTabsControl( SInt16	inInitialValue );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
