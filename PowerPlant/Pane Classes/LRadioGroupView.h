// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LRadioGroupView.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LRadioGroupView
#define _H_LRadioGroupView
#pragma once

#include <LView.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LControl;

// ---------------------------------------------------------------------------

class	LRadioGroupView : public LView,
						  public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('rgpv') };

						LRadioGroupView();

						LRadioGroupView(
								const SPaneInfo	&inPaneInfo,
								const SViewInfo &inViewInfo);

						LRadioGroupView(
								LStream		*inStream);

	virtual				~LRadioGroupView();

	virtual void		AddRadio(
								LControl	*inRadio);

	virtual PaneIDT		GetCurrentRadioID();

	virtual void		SetCurrentRadioID(
								PaneIDT		inRadioID);

	virtual void		ListenToMessage(
								MessageT	inMessage,
								void		*ioParam);

protected:
	TArray<LControl*>	mRadioButtons;
	LControl			*mCurrentRadio;

	virtual void		FinishCreateSelf();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
