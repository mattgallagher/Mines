//##############################################################################
// Mines - CMinesApp.h				by Matt Gallagher �1999 All rights reserved
//##############################################################################

#pragma once

#include <LOffscreenView.h>

#include "CMinesConstants.h"
#include "CMinesWindow.h"

class CMinesView : public LOffscreenView {
public:
	enum { class_ID = 'Mosv' };
						CMinesView();
						CMinesView(const SPaneInfo& inPaneInfo,
									   const SViewInfo& inViewInfo);
						CMinesView(LStream* inStream);
	virtual				~CMinesView();
	
	// Public Methods
	void				LinkToField( CMinesWindow* mineField );
	void				DrawSquare( short picID, Rect &picRect );

protected:
	// Overridden inherited method
	virtual void  	DrawSelf();
	void		   	ClickSelf(const SMouseDownEvent &mouseEvent);
	virtual void  	Click(SMouseDownEvent &inMouseDown);

private:
	void				LoadPictures();

	CMinesWindow		*mMineField;
	LArray				mPictures;
};
