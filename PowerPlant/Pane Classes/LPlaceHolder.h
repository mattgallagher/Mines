// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPlaceHolder.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPlaceHolder
#define _H_LPlaceHolder
#pragma once

#include <LView.h>
#include <Icons.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPlaceHolder : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('plac') };

						LPlaceHolder();

						LPlaceHolder(
								const LPlaceHolder&		inOriginal);

						LPlaceHolder(
								const SPaneInfo&		inPaneInfo,
								const SViewInfo&		inViewInfo,
								SInt16					inOccupantAlignment);

						LPlaceHolder(
								LStream*				inStream);

	virtual				~LPlaceHolder();

	virtual void		InstallOccupant(
								LPane*					inOccupant,
								SInt16					inAlignment = -1);

	virtual LPane*		RemoveOccupant();

	virtual void		CountPanels(
								UInt32&					outHorizPanels,
								UInt32&					outVertPanels);

	virtual Boolean		ScrollToPanel(
								const PanelSpec&		inPanel);

protected:
	LPane*		mOccupant;
	LView*		mOccupantSuperView;
	Handle		mOccupantPlaceH;
	SInt16		mOccupantAlignment;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
