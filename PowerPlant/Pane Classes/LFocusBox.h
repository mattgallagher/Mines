// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LFocusBox.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A FocusBox outlines a Pane to indicate that Pane is the current focus
//	for keystrokes.

#ifndef _H_LFocusBox
#define _H_LFocusBox
#pragma once

#include <LPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LFocusBox : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('fbox') };

					LFocusBox();
					
					LFocusBox( const LFocusBox& inOriginal );
					
					LFocusBox( LStream* inStream );

	virtual void	AttachPane(
							LPane*		inPane,
							Boolean		inSameBindings = true);

	virtual void	Refresh();
	
	virtual void	DontRefresh( Boolean inOKIfHidden = false );

protected:
	virtual void	DrawSelf();
	
	virtual void	ShowSelf();
	
	virtual void	HideSelf();

	virtual RgnHandle	GetBoxRegion( const Rect& inFrame ) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
