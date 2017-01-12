//##############################################################################
// Mines - CMinesApp.h				by Matt Gallagher �1999 All rights reserved
//##############################################################################

#pragma once

#include <LDocApplication.h>
#include "COptionsWindow.h"

class CMinesApp : public LDocApplication {
public:
							CMinesApp();
	virtual 				~CMinesApp();
	
	// Names for the "Game" menu
	LStr255					mShowText, mHideText;

protected:
	// Overridden internal methods
	void					FindCommandStatus( CommandT inCommand,
								Boolean &outEnabled, Boolean &outUsesMark,
								Char16 &outMark, Str255 outName );
	Boolean					ObeyCommand( CommandT inCommand, void *ioParam );
	virtual void			OpenDocument( FSSpec *inMacFSSpec );
	virtual LModelObject *	MakeNewDocument();
	virtual void			ChooseDocument();
	void					StartUp();

private:
	// Reference to Options Window
	COptionsWindow			*mOptionsWindow;
};
