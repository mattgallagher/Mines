// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMultiPanelView.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LMultiPanelView
#define _H_LMultiPanelView
#pragma once

#include <LView.h>
#include <LCommander.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

struct SPanelAndID {
	LView*		panel;
	ResIDT		PPobID;
};

// ---------------------------------------------------------------------------

class LMultiPanelView : public LView,
						public LCommander,
						public LListener {
public:
	enum { class_ID = FOUR_CHAR_CODE('mpvw') };

						LMultiPanelView();
						
						LMultiPanelView( LStream* inStream );
						
						LMultiPanelView(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								LCommander*			inSuperCommander);
								
	virtual				~LMultiPanelView();

	virtual void		AddPanel(
								ResIDT			inPPobID,
								LView*			inPanel,
								UInt16			inAtIndex);

	virtual LView*		RemovePanel( UInt16 inIndex );

	virtual void		SwitchToPanel(
								UInt16			inIndex,
								bool			inDeleteCurrent = false);

	virtual LView*		CreatePanel( UInt16 inIndex );

	virtual void		CreateAllPanels();

	virtual void		SetPanel(
								ResIDT			inPPobID,
								LView*			inPanel,
								UInt16			inIndex,
								bool			inDeleteOld);

	UInt16				GetPanelCount() const
							{
								return (UInt16) mPanels.GetCount();
							}

	LView*				GetPanel( UInt16 inIndex ) const;

	ResIDT				GetPanelResID( UInt16 inIndex ) const;

	LView*				GetCurrentPanel() const
							{
								return GetPanel(mCurrentIndex);
							}

	UInt16				GetCurrentIndex() const
							{
								return mCurrentIndex;
							}

	void				SetSwitchMessage( MessageT inMessage )
							{
								mSwitchMessage = inMessage;
							}

	MessageT			GetSwitchMessage() const
							{
								return mSwitchMessage;
							}

	virtual void		ListenToMessage(
								MessageT		inMessage,
								void*			ioParam);

protected:
	virtual void		FinishCreateSelf();

	virtual void		ShowPanel( UInt16 inIndex );

	virtual void		DrawSelf();

protected:
	TArray<SPanelAndID>	mPanels;
	MessageT			mSwitchMessage;
	UInt16				mCurrentIndex;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
