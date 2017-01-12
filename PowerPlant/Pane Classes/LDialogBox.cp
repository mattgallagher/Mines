// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LDialogBox.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A DialogBox is a Window that has an optional Default and/or Cancel
//	Button. Return and Enter are keyboard equivalents for the Default
//	Button, shich draws in a special manner depending on the Theme.
//	Escape and Command-Period are keyboard equivalents for the Cancel Button.
//
//	The default button must be a LControl that handles the data tag
//	kControlPushButtonDefaultTag. Usually, this should be a LPushButton.
//	For compatibility with older projects, LStdButton also works.
//
//	The DialogBox is a Listener to these buttons, which must derived from
//	LControl. However, the DialogBox does not attach any special meaning
//	to the buttons. The user must give meaning to these buttons by assigning
//	them an appropriate message (the one broadcasted when they are clicked).
//
//	A DialogBox handles negative message numbers by relaying them to its
//	SuperCommander as commands to ObeyCommand(). This allows any
//	Commander to create a DialogBox and then to receive as commands
//	the messages sent by the DialogBox's Broadcasters (which are usually
//	Controls such as buttons).

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LDialogBox.h>
#include <LControl.h>
#include <LStream.h>

#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UEnvironment.h>
#include <UKeyFilters.h>

#include <ControlDefinitions.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LDialogBox							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LDialogBox::LDialogBox()
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LDialogBox							Constructor				  [public[
// ---------------------------------------------------------------------------
//	Construct DialogBox from the data in a struct

LDialogBox::LDialogBox(
	const SWindowInfo&	inWindowInfo)

	: LWindow(inWindowInfo)
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LDialogBox							Constructor				  [pulbic]
// ---------------------------------------------------------------------------
//	Construct a DialogBox from a WIND Resource with the specified attributes
//	and SuperCommander
//
//	Side Effect: Created window becomes the current port

LDialogBox::LDialogBox(
	ResIDT			inWINDid,
	UInt32			inAttributes,
	LCommander*		inSuper)

	: LWindow(inWINDid, inAttributes, inSuper)
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LDialogBox							Constructor				  [public]
// ---------------------------------------------------------------------------

LDialogBox::LDialogBox(
	LCommander*		inSuperCommander,
	const Rect&		inGlobalBounds,
	ConstStringPtr	inTitle,
	SInt16			inProcID,
	UInt32			inAttributes,
	WindowPtr		inBehind,
	PaneIDT			inDefaultButtonID,
	PaneIDT			inCancelButtonID)

	: LWindow(inSuperCommander, inGlobalBounds, inTitle, inProcID,
					inAttributes, inBehind)
{
	mDefaultButtonID = inDefaultButtonID;
	mCancelButtonID	 = inCancelButtonID;
}


// ---------------------------------------------------------------------------
//	¥ LDialogBox							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LDialogBox::LDialogBox(
	LStream*	inStream)

	: LWindow(inStream)
{
	*inStream >> mDefaultButtonID;
	*inStream >> mCancelButtonID;
}


// ---------------------------------------------------------------------------
//	¥ ~LDialogBox							Destructor				  [public]
// ---------------------------------------------------------------------------

LDialogBox::~LDialogBox()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LDialogBox::FinishCreateSelf()
{
	LWindow::FinishCreateSelf();
	
	SetDefaultTag(mDefaultButtonID, true);
	SetCancelTag(mCancelButtonID, true);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDefaultButton												  [public]
// ---------------------------------------------------------------------------
//	Specify the PaneID of the default button of a DialogBox. The default
//	button must be an LControl that handles the data tag
//	kControlPushButtonDefaultTag.
//
//	Use PaneIDT_Undefined to specify no default button
//
//	Keyboard equivalents (Return and Enter) simulate a click inside the
//	default button.

void
LDialogBox::SetDefaultButton(
	PaneIDT	inButtonID)
{
	if (inButtonID != mDefaultButtonID) {
	
			// There can only be one default button. Turn off default
			// state for existing default button, then turn it on
			// for the new default button.
	
		SetDefaultTag(mDefaultButtonID, false);
		SetDefaultTag(inButtonID, true);
		
		mDefaultButtonID = inButtonID;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCancelButton												  [public]
// ---------------------------------------------------------------------------
//	Specify the PaneID of the cancel button of a DialogBox. The cancel
//	button must be derived from LControl.
//
//	Use PaneIDT_Undefined to specify no cancel button
//
//	Keyboard equivalents (Escape and Command-Period) simulate a click inside
//	the cancel button.

void
LDialogBox::SetCancelButton(
	PaneIDT	inButtonID)
{
	if (inButtonID != mCancelButtonID) {
	
			// There can only be one cancel button. Turn off cancel
			// state for existing cancel button, then turn it on
			// for the new cancel button.
	
		SetCancelTag(mCancelButtonID, false);
		SetCancelTag(inButtonID, true);
		
		mCancelButtonID = inButtonID;
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultTag												   [protected]
// ---------------------------------------------------------------------------
//	Set whether or not a Button is the default one
//
//	PaneID must be for a LControl object that handles the
//	kControlPushButtonDefaultTag data tag.
//
//	Used internally. Clients should call SetDefaultButton().

void
LDialogBox::SetDefaultTag(
	PaneIDT		inButtonID,
	Boolean		inBeDefault)
{
	if (inButtonID != PaneIDT_Undefined) {
	
		LControl*	theButton = dynamic_cast<LControl*>
											(FindPaneByID(inButtonID));
		if (theButton != nil) {
			theButton->SetDataTag(0, kControlPushButtonDefaultTag,
										sizeof(Boolean), (Ptr) &inBeDefault);
										
			if (inBeDefault) {
				theButton->AddListener(this);
			}
		}
		
		SignalIf_( theButton == nil );
	}
}


// ---------------------------------------------------------------------------
//	¥ SetCancelTag												   [protected]
// ---------------------------------------------------------------------------
//	Set whether or not a Button is the cancel one
//
//	PaneID must be for a LControl object that handles the
//	kControlPushButtonDefaultTag data tag
//
//	Used internally. Clients should call SetCancelButton().

void
LDialogBox::SetCancelTag(
	PaneIDT		inButtonID,
	Boolean		inBeCancel)
{
	if (inButtonID != PaneIDT_Undefined) {
	
		LControl*	theButton = dynamic_cast<LControl*>
											(FindPaneByID(inButtonID));
		if (theButton != nil) {
		
				// The kControlPushButtonCancelTag is supported
				// in Appearance 1.1 or later
		
			if (UEnvironment::HasFeature(env_HasAppearance11)) {
				theButton->SetDataTag(0, kControlPushButtonCancelTag,
										sizeof(Boolean), (Ptr) &inBeCancel);
			}
										
			if (inBeCancel) {
				theButton->AddListener(this);
			}
		}
		
		SignalIf_( theButton == nil );
	}
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ HandleKeyPress												  [public]
// ---------------------------------------------------------------------------
//	DialogBox handles keyboard equivalents for hitting the Default and
//	Cancel Buttons.
//
//		Default Button: Enter, Return
//		Cancel Button:  Escape, Command-Period

Boolean
LDialogBox::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
	Boolean		keyHandled	= true;
	LControl*	keyButton	= nil;
	UInt8		theChar		= (UInt8) (inKeyEvent.message & charCodeMask);

	if ( (theChar == char_Enter) || (theChar == char_Return) ) {
	
		if (mDefaultButtonID != PaneIDT_Undefined) {
			keyButton = dynamic_cast<LControl*>(FindPaneByID(mDefaultButtonID));
		}

	} else if ( UKeyFilters::IsEscapeKey(inKeyEvent) ||
				UKeyFilters::IsCmdPeriod(inKeyEvent) ) {
				
		if (mCancelButtonID != PaneIDT_Undefined) {
			keyButton = dynamic_cast<LControl*>(FindPaneByID(mCancelButtonID));
		}
	}

	if (keyButton != nil) {
		keyButton->SimulateHotSpotClick(kControlButtonPart);

	} else {
		keyHandled = LWindow::HandleKeyPress(inKeyEvent);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ ListenToMessage												  [public]
// ---------------------------------------------------------------------------
//	Respond to messages from Broadcasters
//
//	DialogBox responds to the cmd_Close message by deleting itself.
//
//	Also, negative message numbers are relayed to ProcessCommand().
//	This allows subclasses of DialogBox to handle messages as commands.
//	Or, if you don't want to subclass DialogBox, the SuperCommander
//	of the DialogBox will receive the messages as commands.

void
LDialogBox::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	if (inMessage == cmd_Close) {
		DoClose();

	} else if (inMessage < 0) {
									// Relay message to supercommander
		if (GetSuperCommander() != nil) {
			SDialogResponse	theResponse;
			theResponse.dialogBox = this;
			theResponse.messageParam = ioParam;
			ProcessCommand(inMessage, &theResponse);
		}
	}
}


PP_End_Namespace_PowerPlant
