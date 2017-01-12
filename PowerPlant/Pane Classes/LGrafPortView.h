// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGrafPortView.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LGrafPortView
#define _H_LGrafPortView
#pragma once

#include <LView.h>
#include <LCommander.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LGrafPortView : public LView,
						public LCommander {
public:
	enum { class_ID = FOUR_CHAR_CODE('gpvw') };

	static LGrafPortView*	CreateGrafPortView(
									ResIDT			inGrafPortView,
									LCommander*		inSuper);

						LGrafPortView();

						LGrafPortView(
								GrafPtr				inGrafPtr);

						LGrafPortView(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								LCommander*			inSuperCommander = nil);

						LGrafPortView(
								LStream*			inStream);

	virtual				~LGrafPortView();

	virtual GrafPtr		GetMacPort() const;

	virtual WindowPtr	GetMacWindow() const;

	virtual Boolean		EstablishPort();

	virtual void		SetForeAndBackColors(
								const RGBColor*		inForeColor,
								const RGBColor*		inBackColor);

	virtual void		GetForeAndBackColors(
								RGBColor*			outForeColor,
								RGBColor*			outBackColor) const;

	virtual void		ApplyForeAndBackColors() const;

	virtual void		InvalPortRect(
								const Rect*			inRect);

	virtual void		InvalPortRgn(
								RgnHandle			inRgnH);

	virtual void		ValidPortRect(
								const Rect*			inRect);

	virtual void		ValidPortRgn(
								RgnHandle			inRgnH);

	virtual void		GlobalToPortPoint(
								Point&				ioPoint) const;

	virtual void		PortToGlobalPoint(
								Point&				ioPoint) const;

	virtual void		UpdatePort();

	virtual void		Draw(	RgnHandle			inSuperDrawRgnH);

	virtual void		Show();

	virtual void		Activate();

	virtual void		Deactivate();

	virtual void		ClickInContent(
								const EventRecord&	inMacEvent);

	virtual bool		DoKeyPress(
								const EventRecord&	inMacEvent);

	virtual void		DoIdle(	const EventRecord&	inMacEvent);

	virtual void		DispatchCommand(
								CommandT			inCommand,
								void*				ioParam);

protected:
	GrafPtr			mGrafPtr;
	RGBColor		mForeColor;
	RGBColor		mBackColor;

	virtual void		DrawSelf();

private:
	void				InitGrafPortView();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
