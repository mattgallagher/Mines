// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGroupBox.h					PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LGroupBox
#define _H_LGroupBox
#pragma once

#include <LCaption.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGroupBox : public LCaption {
public:
	enum { class_ID = FOUR_CHAR_CODE('gbox') };

						LGroupBox();

						LGroupBox( const LGroupBox& inOriginal );

						LGroupBox(
								const SPaneInfo&	inPaneInfo,
								Str255				inString,
								ResIDT				inTextTraitsID);

						LGroupBox( LStream* inStream );

	virtual				~LGroupBox();

protected:
	RGBColor			mFrameColor;

	virtual void		DrawSelf();

	virtual void		DrawText( const Rect& inRect );

	virtual void		DrawBorder( const Rect& inRect );

	virtual void		CalcTextBoxFrame( Rect& outRect );
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
