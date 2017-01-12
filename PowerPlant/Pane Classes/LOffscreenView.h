// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOffscreenView.h			PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOffscreenView
#define _H_LOffscreenView
#pragma once

#include <LView.h>
#include <QDOffscreen.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LOffscreenView : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('offv') };

							LOffscreenView();

							LOffscreenView(
									const SPaneInfo&	inPaneInfo,
									const SViewInfo&	inViewInfo);

							LOffscreenView(LStream*		inStream);

	virtual					~LOffscreenView();

	virtual Boolean			EstablishPort();

	virtual void			Draw(	RgnHandle			inSuperDrawRgnH);

	virtual void			SubImageChanged(
									LView*				inSubView);

	virtual void			Click(	SMouseDownEvent&	inMouseDown);

	virtual void			AdjustMouse(
									Point				inPortPt,
									const EventRecord	&inMacEvent,
									RgnHandle			outMouseRgn);

	virtual Boolean			IsHitBy(
									SInt32				inHorizPort,
									SInt32				inVertPort);

protected:
	GWorldPtr				mOffscreenWorld;
	bool					mDrawingSelf;
	bool					mMousingSelf;

	virtual void			DrawOffscreen(
									RgnHandle			inSuperDrawRgnH,
									GWorldFlags			inFlags = 0);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
