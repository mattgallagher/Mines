// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LIconPane.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Pane which draws a single icon from an icon family

#ifndef _H_LIconPane
#define _H_LIconPane
#pragma once

#include <LPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LIconPane : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('icnp') };

						LIconPane();

						LIconPane(
								const SPaneInfo&	inPaneInfo,
								ResIDT				inIconID);

						LIconPane( LStream* inStream );

	virtual				~LIconPane();

	void				SetIconID( ResIDT inIconID );
	
	ResIDT				GetIconID( void ) const	
								{ return mIconID; }

	virtual void		DrawSelf();

protected:
	ResIDT				mIconID;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
