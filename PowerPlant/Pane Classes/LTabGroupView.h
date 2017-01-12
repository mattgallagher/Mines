// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LTabGroupView.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LTabGroupView
#define _H_LTabGroupView
#pragma once

#include <LView.h>
#include <LTabGroup.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LTabGroupView : public LView,
					  public LTabGroup {
public:
	enum { class_ID = FOUR_CHAR_CODE('tbgv') };

						LTabGroupView();

						LTabGroupView( const LTabGroupView& inOriginal );

						LTabGroupView( LStream* inStream );

						LTabGroupView(
								const SPaneInfo&		inPaneInfo,
								const SViewInfo&		inViewInfo,
								LCommander*				inSuper);

	virtual				~LTabGroupView();
	
	virtual void		Hide();
	
protected:
	virtual void		SuperHide();

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
