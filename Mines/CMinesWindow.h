//##############################################################################
// Mines - CMinesApp.h				by Matt Gallagher �1999 All rights reserved
//##############################################################################

#pragma once

#include <LWindow.h>

class CMinesWindow : public LWindow, public LListener, public LPeriodical {
public:
	enum { class_ID = 'Mwin' };
					CMinesWindow(LStream* inStream);
	virtual			~CMinesWindow();

	// Overridden LListener Methods
	virtual void	ListenToMessage( MessageT inMessage, void *ioParam );
	
	// Overridden LCommander Methods
	virtual Boolean	ObeyCommand( CommandT inCommand, void *ioParam );
	virtual void	FindCommandStatus( CommandT inCommand,
						Boolean &outEnabled, Boolean &outUsesMark,
						UInt16 &outMark, Str255 outName );
	
	// Overridden LWindow Methods
	void			FinishCreate();
	virtual void	CalcStandardBoundsForScreen(
								 const Rect			&inScreenBounds,
								 Rect				&outStdBounds) const;
	void			SetMaxWindowBounds( short maxWidth, short maxHeight );
	void			ActivateSelf();
	
	// Other public methods
	Boolean			GetSquare( short square, short &picID, Rect &picRect );
	void			TrackClick( short h, short v, EventModifiers inModifiers );
	void			SpendTime( const EventRecord &inMacEvent );
	void			GetFieldSize( SDimension32 &outFieldSize );

private:
	// Internal methods
	void			NewGame();
	void			EndGame( short cause );
	void			AddMines(short numMines);
	void			ClearZeroSquare(short square);
	void			ShowDialog( short inResDialog );
	
	short			mFieldHeight, mFieldWidth;
	short			mNumCoveredSquares;
	short			mNumMines;
	long			mFinishTime, mGameLength;
	short			*mMineField;
	Boolean		mInGame, mAutoStart, mUntimed;
};
