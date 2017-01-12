//##############################################################################
// Mines - CMinesWindow.h			by Matt Gallagher �1999 All rights reserved
//##############################################################################

#include <LStaticText.h>
#include <LBevelButton.h>

#include "CMinesApp.h"
#include "CMinesWindow.h"
#include "CMinesView.h"
#include "COptionsWindow.h"
#include "CMinesConstants.h"

//#############################
// The constructor
//#############################
CMinesWindow::CMinesWindow(LStream* inStream) :
	LWindow(inStream)
{
	// Initialise some variables
	mFieldHeight = 12;
	mFieldWidth = 20;
	mNumCoveredSquares = mFieldHeight * mFieldWidth;
	mNumMines = 20;
	mGameLength = 200;
	mInGame = false;
	mAutoStart = false;
	mUntimed = false;
	
	// Place a decorative minefield in the window to start with
	mMineField = (short *)::NewPtr( sizeof(short)*20*12 );
	ThrowIfMemFail_(mMineField);
	
	//Clear the minefield
	short i=0;
	while( i < mFieldWidth*mFieldHeight )
	{
		mMineField[i] = kZeroMines;
		i++;
	}
}

//#############################
// The destructor
//#############################
CMinesWindow::~CMinesWindow()
{
	// Dispose of all allocated memory
	if( mMineField != nil )
	{
		::DisposePtr( (Ptr)mMineField );
	}
}

//#############################
// The destructor
//#############################
void CMinesWindow::FinishCreate()
{
	LWindow::FinishCreate();

	// Become a listener to all the window's controls
	UReanimator::LinkListenerToControls( this, this, rPPob_MinesWindow );
	
	// Link the minefield to the MinesView in the window
	CMinesView *minesView;
	minesView = dynamic_cast<CMinesView *> (FindPaneByID(kMineFieldPane));
	minesView->LinkToField( this );
}

//#############################
// Listen to messages
//#############################
void CMinesWindow::ListenToMessage( MessageT inMessage, void *ioParam )
{
#pragma unused( ioParam )
	switch( inMessage )
	{
		case msg_NewGame:
		{
			NewGame();
		}
		break;

		case msg_EndGame:
		{
			EndGame( kUserStopped );
		}
		break;
	}
}

//#############################
// Obey Commands
//#############################
Boolean CMinesWindow::ObeyCommand( CommandT inCommand, void *ioParam )
{
	Boolean cmdHandled=true;

	switch( inCommand )
	{
		case cmd_NewGame:
		{
			NewGame();
		}
		break;
		
		case cmd_NewCustomGame:
		{
			NewGameOptionsMsg	*inMessage = (NewGameOptionsMsg*)ioParam;
			
			mFieldWidth = inMessage->fieldSize;
			mFieldHeight = inMessage->fieldSize * 12 / 20;
			mNumMines = inMessage->numMines;
			mGameLength = inMessage->gameLength;
			mAutoStart = inMessage->autoStart;
			mUntimed = inMessage->untimed;
			
			NewGame();
			DoSetZoom(true);
			
			if( inMessage->fieldData != nil )
			{
				::BlockMove( inMessage->fieldData, mMineField,
					mFieldWidth*mFieldHeight*sizeof( short ) );
			}
			
			NewGameOptionsMsg	outMessage;

			// Fill in the message to send
			outMessage.fieldSize = mFieldWidth;
			outMessage.numMines = mNumMines;
			outMessage.gameLength = mGameLength;
			outMessage.autoStart = mAutoStart;
			outMessage.untimed = mUntimed;
			
			// Send the message
			mSuperCommander->ProcessCommand( cmd_NewFrontWindow, &outMessage );
		}
		break;
		
		case cmd_EndGame:
		{
			EndGame( kUserStopped );
		}
		break;
		
		default:
		{
			// Call inherited.
			cmdHandled = LWindow::ObeyCommand( inCommand, ioParam );
		}
		break;
	}
	
	return cmdHandled;
}

//#############################
// Update Menus
//#############################
void CMinesWindow::FindCommandStatus( CommandT inCommand, Boolean &outEnabled,
	Boolean &outUsesMark, UInt16 &outMark, Str255 outName )
{
	switch( inCommand )
	{
		case cmd_NewGame:
		{
			// You can always start a new game
			outEnabled = true;
		}
		break;
		
		case cmd_EndGame:
		{
			// Only if a game is in progress
			outEnabled = mInGame;
		}
		break;
				
		default:
		{
			// Call inherited.
			LWindow::FindCommandStatus( inCommand, outEnabled,
				outUsesMark, outMark, outName );
		}
		break;
	}
}

//#############################
// Return enough information to draw a square
//#############################
void CMinesWindow::GetFieldSize( SDimension32 &outFieldSize )
{
	outFieldSize.height = mFieldHeight * 32;
	outFieldSize.width = mFieldWidth * 32;
}

//#############################
// Return enough information to draw a square
//#############################
Boolean CMinesWindow::GetSquare( short square, short &picID, Rect &picRect )
{
	// Return false if square is outside range
	if( square >= mFieldHeight * mFieldWidth )
	{
		return false;
	}
	
	// Set the appropriate rectangle to draw this square into
	picRect.left	= (square % mFieldWidth) * 32;
	picRect.top 	= (square / mFieldWidth) * 32;
	picRect.right	= picRect.left + 32;
	picRect.bottom	= picRect.top + 32;
	
	// Return the correct picture ID for the square
	if( mMineField[square] > 0x00FF )
	{
		picID = (mMineField[square] & 0xFF00) >> 8;
	}
	else
	{
		picID = mMineField[square];
	}

	return true;
}

//#############################
// Start a new game
//#############################
void CMinesWindow::NewGame()
{
	// Delete any previous minefield
	if( mMineField != nil )
	{
		::DisposePtr( (Ptr)mMineField );
	}
	
	// Stop Idling (only needed in wierd options-changed cases
	StopIdling();
	
	// Create the new minefield
	mMineField = (short *)::NewPtr( sizeof(short)*mFieldHeight*mFieldWidth );
	ThrowIfMemFail_(mMineField);
	
	// Initialise variables
	mNumCoveredSquares = mFieldHeight * mFieldWidth;
	mInGame = true;
	
	// Initialise the minefield
	short i=0;
	while( i < mFieldWidth*mFieldHeight )
	{
		mMineField[i] = (kUntouchedSquare << 8) | kZeroMines;
		i++;
	}
	
	// Lay the mines
	AddMines(mNumMines);
	
	if( mAutoStart )
	{
		// Randomly select a square
		short randNum, numSquares;
		long numTries;
		numSquares = mFieldWidth*mFieldHeight;
		randNum = (Random()+32768.0)*(numSquares/65536.0);
		numTries = 1;
		while( (mMineField[randNum] & 0x00FF)!=kZeroMines && numTries<1000000 )
		{
			// Choose a different square if the one chosen is not empty
			randNum = (Random()+32768.0)*(numSquares/65536.0);
			numTries++;
		}
		
		if( numTries != 1000000 )
		{
			// Clear the square and display
			ClearZeroSquare( randNum );
		}
	}
	
	// Enable the time display and the EndGame button
	LStaticText		*timeRemaining, *timeDisplay;
	LBevelButton	*endGame;
	timeRemaining	= dynamic_cast<LStaticText *>
					(FindPaneByID(kTimeRemaining));
	timeDisplay		= dynamic_cast<LStaticText *>
					(FindPaneByID(kTimeDisplay));
	endGame			= dynamic_cast<LBevelButton *>
					(FindPaneByID(kEndGameButton));
	timeRemaining->Enable();
	endGame->Enable();

	if( mUntimed == false )
	{
		// Start the timer
		timeDisplay->Enable();
		mFinishTime = ::TickCount() + mGameLength*60;
		StartIdling();
		
		// Force the timer to update immediately
		EventRecord	dummyVariable;
		SpendTime( dummyVariable );
	}
	else
	{
		// Assign the string for display
		LStr255		 timeString( "Untimed Game" );

		// Get a handle to the time display and assign the string
		timeDisplay->SetText( timeString );
		timeDisplay->Disable();
	}
	
	// Force an update of the mines view
	CMinesView	*minesView;
	minesView	= dynamic_cast<CMinesView *>(FindPaneByID(kMineFieldPane));
	
	// Remember to tell the view that the field size may have changed
	minesView->LinkToField( this );
	minesView->Draw( nil );
		
}

//#############################
// End the current game
//#############################
void CMinesWindow::EndGame( short cause )
{
#pragma unused( cause )
	mInGame = false;
	
	// Stop the timer
	StopIdling();
	
	// Disable the time display and the EndGame button
	LStaticText		*timeRemaining, *timeDisplay;
	LBevelButton	*endGame;
	timeRemaining	= dynamic_cast<LStaticText *>
					(FindPaneByID(kTimeRemaining));
	timeDisplay		= dynamic_cast<LStaticText *>
					(FindPaneByID(kTimeDisplay));
	endGame			= dynamic_cast<LBevelButton *>
					(FindPaneByID(kEndGameButton));
	timeRemaining->Disable();
	timeDisplay->Disable();
	endGame->Disable();
	
	if( cause != kUserStopped )
	{
		// Show a dialog to notify the user why the game is over
		ShowDialog( rPPobEndGameBaseIndex + cause );
	}
}

//#############################
// Add mines to the mine field
//#############################
void CMinesWindow::AddMines(short numMines)
{
	short	randNum, numSquares;
	
	numSquares = mFieldHeight*mFieldWidth;
	while( numMines > 0 )
	{
		// Randomly select a square
		randNum = (Random()+32768.0)*(numSquares/65536.0);
		while( (mMineField[randNum] & 0x00FF) == kRevealedBomb )
		{
			// Choose a different square if the one chosen is already mined
			randNum = (Random()+32768.0)*(numSquares/65536.0);
		}
		
		// Put a mine in this square
		mMineField[randNum] = (mMineField[randNum]&0xFF00) | kRevealedBomb;
		
		// Add one to the proximity count of each surrounding square
		if( (randNum%mFieldWidth)!=0 )
		{
			if( (randNum/mFieldWidth)!=0 )
				if( (mMineField[randNum-mFieldWidth-1]&0x00FF) < kEightMines )
					mMineField[randNum-mFieldWidth-1]++;
			if( (mMineField[randNum-1]&0x00FF) < kEightMines )
				mMineField[randNum-1]++;
			if( (randNum/mFieldWidth)!= mFieldHeight-1 )
				if( (mMineField[randNum+mFieldWidth-1]&0x00FF) < kEightMines )
					mMineField[randNum+mFieldWidth-1]++;
		}
		if( (randNum/mFieldWidth)!= 0 )
			if( (mMineField[randNum-mFieldWidth]&0x00FF) < kEightMines )
				mMineField[randNum-mFieldWidth]++;
		if( (randNum/mFieldWidth)!= mFieldHeight-1 )
			if( (mMineField[randNum+mFieldWidth]&0x00FF) < kEightMines )
				mMineField[randNum+mFieldWidth]++;
		if( (randNum%mFieldWidth)!=mFieldWidth-1 )
		{
			if( (randNum/mFieldWidth)!=0 )
				if( (mMineField[randNum-mFieldWidth+1]&0x00FF) < kEightMines )
					mMineField[randNum-mFieldWidth+1]++;
			if( (mMineField[randNum+1]&0x00FF) < kEightMines )
				mMineField[randNum+1]++;
			if( (randNum/mFieldWidth)!= mFieldHeight-1 )
				if( (mMineField[randNum+mFieldWidth+1]&0x00FF) < kEightMines )
					mMineField[randNum+mFieldWidth+1]++;
		}
		
		numMines--;
	}
}

//#############################
// Track and handle a mouse click in the minefield
//#############################
void CMinesWindow::TrackClick( short h, short v, EventModifiers inModifiers )
{
	// Determine which square was clicked in
	short	theSquare = (h / 32) + ((v / 32) * mFieldWidth);
	
	if( !mInGame )
	{
		// If not in a game, tell the user to start a new game and return
		ShowDialog( rPPobStartNewGameDialog );
		return;
	}
	short covering = (mMineField[theSquare] & 0xFF00) >> 8;
	if( (theSquare >= mFieldHeight*mFieldWidth) || (theSquare < 0) ||
	   !(covering == kUntouchedSquare ||
		(covering == kProtectedSquare && (inModifiers & shiftKey))) )
	{
		// If the calculated square is not a valid square, return
		// of if the square pressed was neither Untouched, nor Protected and
		// the shift key is down
		return;
	}

	// Get a reference to the minesView so we can talk to it later
	CMinesView		*minesView;
	minesView		= dynamic_cast<CMinesView *>
					(FindPaneByID(kMineFieldPane));
	
	// Calculate the rectangle for this square
	Rect frameRect;
	frameRect.left		= (h/32) * 32;
	frameRect.top 		= (v/32) * 32;
	frameRect.right		= frameRect.left+32;
	frameRect.bottom	= frameRect.top+32;
	
	// Track while the mouse is down
	Boolean	inSquare=true;
	Boolean wasInSquare=true;
	minesView->DrawSquare( kPressedSquare, frameRect );
	while( ::StillDown() )
	{
		wasInSquare = inSquare;
		Point where;
		::GetMouse( &where );
		inSquare = ::PtInRect( where, &frameRect );
		if( inSquare != wasInSquare )
		{
			if( inSquare )
			{
				// Moved into the square, draw it pressed
				minesView->DrawSquare( kPressedSquare, frameRect );
			}
			else
			{
				// Moved out of the square, draw it untouched
				minesView->DrawSquare( kUntouchedSquare, frameRect );
			}
		}
	}
	
	// Only do the rest if the mouse was released in the square
	if( inSquare )
	{
		// If the shift key was held (we are protecting/unprotecting a square)
		if( inModifiers & shiftKey )
		{
			if( (mMineField[theSquare] & 0xFF00) == (kUntouchedSquare << 8) )
			{
				// Mark this square protected
				mMineField[theSquare] = (mMineField[theSquare] & 0x00FF)
					| ( kProtectedSquare << 8 );
				minesView->DrawSquare( kProtectedSquare, frameRect );
			}
			else
			{
				// Unmark this square
				mMineField[theSquare] = (mMineField[theSquare] & 0x00FF)
					| ( kUntouchedSquare << 8 );
				minesView->DrawSquare( kUntouchedSquare, frameRect );
			}
		}
		else
		{
			// Uncover the square
			mMineField[ theSquare ] = mMineField[ theSquare ] & 0x00FF;
			minesView->DrawSquare( mMineField[ theSquare ], frameRect );
			mNumCoveredSquares--;
			
			if( mMineField[ theSquare ] == kZeroMines )
			{
				// Add one to NumCoveredSquare, because ClearZeroSquare
				// will subtract for this square again
				mNumCoveredSquares++;

				// If this square is a zero, clear the surroundings
				ClearZeroSquare(theSquare);
				
				// Force an update of the mines view
				CMinesView		*minesView;
				minesView		= dynamic_cast<CMinesView *>
								(FindPaneByID(kMineFieldPane));
				minesView->Draw( nil );
			}
			else if( mMineField[theSquare]==kRevealedBomb )
			{
				// End the game if a mine was revealed
				EndGame( kClickedMine );
			}
		}
		
		// If coveredSquares = numMines then the game is won
		if( mNumCoveredSquares == mNumMines )
		{
			EndGame( kWonGame );
		}
	}
}

//#############################
// Track and handle a mouse click in the minefield
//#############################
void CMinesWindow::ClearZeroSquare( short square )
{
	// Uncover this square
	mMineField[ square ] = mMineField[ square ] & 0x00FF;
	mNumCoveredSquares--;
	if( mMineField[ square ] != kZeroMines ) return;

	// Add one to the proximity count of each surrounding square
	if( (square%mFieldWidth)!=0 )
	{
		if( (square/mFieldWidth)!=0 )
			if(((mMineField[square-mFieldWidth-1]&0xFF00)>>8)==kUntouchedSquare)
				ClearZeroSquare(square-mFieldWidth-1);
		if( ((mMineField[square-1]&0xFF00)>>8)==kUntouchedSquare)
			ClearZeroSquare(square-1);
		if( (square/mFieldWidth)!= mFieldHeight-1 )
			if( ((mMineField[square+mFieldWidth-1]&0xFF00)>>8)==kUntouchedSquare)
				ClearZeroSquare(square+mFieldWidth-1);
	}
	if( (square/mFieldWidth)!= 0 )
		if( ((mMineField[square-mFieldWidth]&0xFF00)>>8)==kUntouchedSquare)
			ClearZeroSquare(square-mFieldWidth);
	if( (square/mFieldWidth)!= mFieldHeight-1 )
		if( ((mMineField[square+mFieldWidth]&0xFF00)>>8)==kUntouchedSquare)
			ClearZeroSquare(square+mFieldWidth);
	if( (square%mFieldWidth)!=mFieldWidth-1 )
	{
		if( (square/mFieldWidth)!=0 )
			if( ((mMineField[square-mFieldWidth+1]&0xFF00)>>8)==kUntouchedSquare)
				ClearZeroSquare(square-mFieldWidth+1);
		if( ((mMineField[square+1]&0xFF00)>>8)==kUntouchedSquare)
			ClearZeroSquare(square+1);
		if( (square/mFieldWidth)!= mFieldHeight-1 )
			if( ((mMineField[square+mFieldWidth+1]&0xFF00)>>8)==kUntouchedSquare)
				ClearZeroSquare(square+mFieldWidth+1);
	}
}

//#############################
// Adjust the time remaining
//#############################
void CMinesWindow::SpendTime( const EventRecord &mouseEvent )
{
#pragma unused( mouseEvent )
	// Get the current time
	long time = ::TickCount();
	
	// Enforce that time remaining cannot be negative
	if( mFinishTime < time )
	{
		time = mFinishTime;
	}
	
	// Assign the string for display
	LStr255		 timeString( (mFinishTime - time)/60 );
	timeString = timeString + "\p seconds";

	// Get a handle to the time display and assign the string
	LStaticText	  *timeDisplay;
	timeDisplay	= dynamic_cast<LStaticText *> (FindPaneByID(kTimeDisplay));
	timeDisplay->SetText( timeString );

	// End the game if time has expired
	if( time == mFinishTime )
	{
		EndGame( kTimeExpired );
		
		// No actual events have happened, an "Updates events"
		// must be passed to force an update of the menus
		CMinesApp *theApp = static_cast<CMinesApp *>(GetTopCommander());
		theApp->UpdateMenus();
	}
}

//#############################
// Display an end game dialog
//#############################
void CMinesWindow::ShowDialog( short inResDialog )
{
	// Create a dialog handler (Note: while "this" is given as the LCommander,
	// we don't actually send any messages that aren't handled by theHandler)
	StDialogHandler theHandler( inResDialog, this );
	
	// Get the dialog window
	LWindow		*theDialog;
	theDialog =	theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	// Show the dialog
	theDialog->Show();
	
	// Loop until the dialog is dismissed
	while( true )
	{
		// Handle dialog messages
		MessageT theMessage = theHandler.DoDialog();
		
		if( theMessage == msg_OK )
		{
			// End when "OK" selected. Everything will be shoved off the stack.
			break;
		}
	}
}

//#############################
// Change the maximum size of the window
//#############################
void CMinesWindow::SetMaxWindowBounds( short maxWidth, short maxHeight )
{
	mMinMaxSize.right = maxWidth;
	mMinMaxSize.bottom = maxHeight;
}

//#############################
// Ensure that the standard bounds are not bigger than the enforced maximum
//#############################
void CMinesWindow::CalcStandardBoundsForScreen(
	const Rect&	inScreenBounds,
	Rect&		outStdBounds) const
{
	Point	windowExtreme;
	
	LWindow::CalcStandardBoundsForScreen( inScreenBounds, outStdBounds );
	
	// Save the maximum allowable bottom right
	windowExtreme.h = outStdBounds.right;
	windowExtreme.v = outStdBounds.bottom;
	
	// Restrict the "standard" size to the maximum size
	if( outStdBounds.right - outStdBounds.left > mMinMaxSize.right )
		outStdBounds.right = outStdBounds.left + mMinMaxSize.right;
	if( outStdBounds.bottom - outStdBounds.top > mMinMaxSize.bottom )
		outStdBounds.bottom = outStdBounds.top + mMinMaxSize.bottom;
	
	// Try to position the new rectangle in the old one's spot
	::OffsetRect( &outStdBounds, mUserBounds.left - outStdBounds.left,
					mUserBounds.top - outStdBounds.top );

	// Shift the rect back by the amount it exceeds the maximum
	if( outStdBounds.right > windowExtreme.h )
		::OffsetRect( &outStdBounds, windowExtreme.h - outStdBounds.right, 0 );
	if( outStdBounds.bottom > windowExtreme.v )
		::OffsetRect( &outStdBounds, 0, windowExtreme.v - outStdBounds.bottom );
}

//#############################
// Notify the floating palette on enabling that this is the front window
//#############################
void CMinesWindow::ActivateSelf()
{
	LWindow::ActivateSelf();
	
	NewGameOptionsMsg	outMessage;
	
	// Fill in the message to send
	outMessage.fieldSize = mFieldWidth;
	outMessage.numMines = mNumMines;
	outMessage.gameLength = mGameLength;
	outMessage.autoStart = mAutoStart;
	outMessage.untimed = mUntimed;
	
	// Send the message
	mSuperCommander->ProcessCommand( cmd_NewFrontWindow, &outMessage );
}
