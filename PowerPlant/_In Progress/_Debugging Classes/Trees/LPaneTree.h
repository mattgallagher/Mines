// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPaneTree.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub. Thanks to Greg Bolsinga.
//
//	See LPaneTree.h for more information

#ifndef _H_LPaneTree
#define _H_LPaneTree
#pragma once

#include <LTree.h>
#include <LWindow.h>
#include <LString.h>
#include <UTextTraits.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LPaneTree	:	public LTree {
public:
			enum { class_ID = FOUR_CHAR_CODE('ptre') };

							LPaneTree();
							LPaneTree(
									LStream*			inStream);
							LPaneTree(
									const LPaneTree&	inOriginal);
							LPaneTree(
									const SPaneInfo&	inPaneInfo,
									const SViewInfo&	inViewInfo,
									UInt32				inThreshold,
									ResIDT				inTxtrID,
									const RGBColor		inMousedPaneColor);
	virtual					~LPaneTree();

	static	LWindow*		GetTargetWindow(
									SInt16				inIndex);

	virtual	void			MakePaneText(
									const LPane&		inPane,
									LStr255&			outText,
									SInt32				inLevel);

protected:
	virtual	void			WalkPaneList(
									const LPane&		inPane,
									SInt32				inLevel,
									bool				inGenerateExtraInfo);

	virtual	void			BuildTree(
									bool				inGenerateExtraInfo);

			RGBColor		mCurMousedColor;
			LPane*			mCurMousedPane;

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LPaneTree
