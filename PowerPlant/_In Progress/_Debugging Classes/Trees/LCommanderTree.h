// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCommanderTree.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Greg Bolsinga, John C. Daub
//
//	See LCommanderTree.cp for more information.

#ifndef _H_LCommanderTree
#define _H_LCommanderTree
#pragma once

#include <LTree.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LStream;
class	LCommander;
class	LStr255;

class LCommanderTree :	public	LTree {
public:
	enum	{ class_ID = FOUR_CHAR_CODE('ltre') };

								LCommanderTree();
								LCommanderTree(
										const LCommanderTree&	inOriginal);
								LCommanderTree(
										const SPaneInfo&		inPaneInfo,
										const SViewInfo&		inViewInfo,
										UInt32					inThreshold,
										ResIDT					inTxtrID,
										const RGBColor&			inCurTargetColor = Color_Red,
										Style					inCurChainStyle = bold,
										Style					inLatentSubStyle = italic );

								LCommanderTree(
										LStream*				inStream);
	virtual						~LCommanderTree();

protected:
	virtual	void				FinishCreateSelf();

	virtual	void				WalkCommanderChain(
										const LCommander&		inCommander,
										const SInt32&			inLevel,
										bool					inGenerateExtraInfo);

	virtual	void				MakeCommanderText(
										const LCommander&		inCommander,
										LStr255&				outText,
										SInt32					inLevel);

	virtual	void				BuildTree(
										bool					inGenerateExtraInfo);

			RGBColor			mCurTargetColor;
			Style				mCurChainStyle;
			Style				mLatentSubStyle;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif /* _H_LCommanderTree */
