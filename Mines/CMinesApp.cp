//##############################################################################
// Mines - CMinesApp.cp				by Matt Gallagher �1999 All rights reserved
//##############################################################################

// PowerPlant Classes included for registration
#include <UControlRegistry.h>
#include <LActiveScroller.h>
#include <LBevelButton.h>
#include <LAMBevelButtonImp.h>
#include <LGABevelButtonImp.h>

// Macintosh header file for the call to RegisterAppearanceClient
#include <Appearance.h>

// Application headers
#include "CMinesApp.h"
#include "CMinesConstants.h"
#include "CMinesView.h"
#include "CMinesWindow.h"

//#############################
// Program main()
//#############################
int main(void)
{
	// Initialize the heap. Parameter is number of master handle
	// blocks to allocate.
	InitializeHeap( 4 );
	
	// Initialize the MacOS toolbox.
	UQDGlobals::InitializeToolbox();
	SInt32 seed = (SInt32)CFAbsoluteTimeGetCurrent();
	UQDGlobals::SetRandomSeed(seed);

	// Install a GrowZone function to catch  low memory situations.
	// Parameter is the size of the memory reserve in bytes.
	new LGrowZone( 20000 );

	// Create the application object and run it.
	CMinesApp	theApp;
	theApp.Run();
}

//#############################
// CMinesApp - Class Registration and initialising
//#############################
CMinesApp::CMinesApp()
{
	// Setup the throw and signal actions.
	SetDebugThrow_( debugAction_Alert );
	SetDebugSignal_( debugAction_Alert );

	// Register appearance manager classes
	if (UEnvironment::HasFeature (env_HasAppearance)) {
		::RegisterAppearanceClient();
		UControlRegistry::RegisterAMClasses();
		
		// Note: Registering GABevelButton instead
		URegistrar::UnregisterClass(LBevelButton::imp_class_ID);
		RegisterClassID_(LGABevelButtonImp, LBevelButton::imp_class_ID);
	}
	else
	{
		UControlRegistry::RegisterGAClasses();
	}
	
	// Register other required PowerPlant core classes.
	RegisterClass_(LDialogBox);
	RegisterClass_(LActiveScroller);
	RegisterClass_(LStdButton);
	RegisterClass_(LPicture);
	RegisterClass_(LView);
	RegisterClass_(LPlaceHolder);
	RegisterClass_(LWindow);

	// Register custom classes.
	RegisterClass_(CMinesView);
	RegisterClass_(CMinesWindow);
	RegisterClass_(COptionsWindow);
	
	mOptionsWindow	= static_cast<COptionsWindow *>
					  (LWindow::CreateWindow( rPPob_OptionsWindow, this ));
	ThrowIfNil_( mOptionsWindow );
	
	
	// Get the text for the options window menu item
	mShowText.Assign( textListR, 1 );
	mHideText.Assign( textListR, 2 );
	
	MakeNewDocument();
}

//#############################
// CMinesApp - Destructor
//#############################
CMinesApp::~CMinesApp()
{
}

//#############################
// CMinesApp - Startup without document
//#############################
void CMinesApp::StartUp()
{
	// Create the single application window.
	ObeyCommand( cmd_New, nil );
}

//#############################
// CMinesApp - Create the game window
//#############################
LModelObject* CMinesApp::MakeNewDocument()
{
	// Create the window.
	CMinesWindow	*minesWindow;
	minesWindow		= static_cast<CMinesWindow *>(LWindow::CreateWindow(
						rPPob_MinesWindow, mOptionsWindow ));
	ThrowIfNil_( minesWindow );
	
	// Show the window.
	minesWindow->Show();
	
	return minesWindow;
}

//#############################
// CMinesApp - ObeyCommands
//#############################
Boolean CMinesApp::ObeyCommand( CommandT inCommand, void *ioParam )
{
	Boolean cmdHandled=true;

	switch( inCommand )
	{
		case cmd_Options:
		{
			if( mOptionsWindow->IsVisible() )
			{
				mOptionsWindow->Hide();
			}
			else
			{
				mOptionsWindow->Show();
			}
		}
		break;

		default:
		{
			// Call inherited.
			cmdHandled = LDocApplication::ObeyCommand( inCommand, ioParam );
		}
		break;
	}
	
	return cmdHandled;
}

//#############################
// Update Menus
//#############################
void CMinesApp::FindCommandStatus( CommandT inCommand, Boolean &outEnabled,
	Boolean &outUsesMark, UInt16 &outMark, Str255 outName )
{
	switch( inCommand )
	{
		case cmd_New:
		{
			// You can always start a new game
			outEnabled = true;
		}
		break;

		case cmd_Options:
		{
			// Show can only view the options window if a window is visible
			LWindow *topWindow = UDesktop::FetchTopRegular();
			if( topWindow != nil &&
				topWindow->GetPaneID()==rPPob_MinesWindow )
			{
				outEnabled = true;
				if( mOptionsWindow->IsVisible() )
				{
					// Make the text read "Hide Game Options"
					LString::CopyPStr(mHideText, outName, sizeof(Str255));
				}
				else
				{
					// Make the menu item text read "Show Game Options"
					LString::CopyPStr(mShowText, outName, sizeof(Str255));
				}
			}
			else
			{
				// Of course, if there are no windows open, disable the menu
				mOptionsWindow->Hide();
				outEnabled = false;
			}
		}
		break;
		
		default:
		{
			// Call inherited.
			LDocApplication::FindCommandStatus( inCommand, outEnabled,
				outUsesMark, outMark, outName );
		}
		break;
	}
}

//#############################
// Open a new window document
//#############################
void CMinesApp::OpenDocument( FSSpec *inMacFSSpec )
{
#pragma unused (inMacFSSpec)
	// Create a new document
	MakeNewDocument();
	
	// Open the file and get the mines data out of it
	
	// Send the data in a command to the target (which is the new window)
}

//#############################
// Choose a new document using the standard get file
//#############################
void
CMinesApp::ChooseDocument()
{
	// Deactivate the desktop.
	::UDesktop::Deactivate();

	// Browse for a document.
    NavReplyRecord reply;
    NavTypeList  typeList = {0};
    NavTypeListPtr typeListPtr;
        
    typeList.componentSignature = kNavGenericSignature;
    typeList.reserved = 1;
    typeList.osTypeCount = 1;
    typeList.osType[0]='Myns';
        
    typeListPtr = &typeList;
	::NavGetFile( nil, &reply, nil, nil, nil, nil, &typeListPtr, nil);

	// Activate the desktop.
	::UDesktop::Activate();
	
	// Send an apple event to open the file.	
    if( reply.validRecord )
    {
        AEDesc resultDesc;
        OSErr the_sErr = noErr;
			
        //grab information about file for opening:	
        if ((the_sErr = AEGetNthDesc( &(reply.selection),1, typeFSS, NULL, &resultDesc )) ==noErr)
        {
				FSSpec fspec;
            the_sErr = AEGetDescData(&resultDesc, (void *)&fspec,sizeof(FSSpec));
            if(the_sErr == noErr)
                AEDisposeDesc( &resultDesc );
				SendAEOpenDoc( fspec );
        }    
	}
}
