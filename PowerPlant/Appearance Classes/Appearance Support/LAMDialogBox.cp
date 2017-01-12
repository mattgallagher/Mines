// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMDialogBox.cp				PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LAMDialogBox.h>
#include <LControlPane.h>
#include <LStream.h>

#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UKeyFilters.h>

#include <UDrawingState.h>
#include <UEnvironment.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LAMDialogBox							Default Constructor		  [public]
// ---------------------------------------------------------------------------

LAMDialogBox::LAMDialogBox()
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LAMDialogBox							Constructor				  [public[
// ---------------------------------------------------------------------------
//	Construct DialogBox from the data in a struct

LAMDialogBox::LAMDialogBox(
	const SWindowInfo&	inWindowInfo)

	: LWindow(inWindowInfo)
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LAMDialogBox							Constructor				  [pulbic]
// ---------------------------------------------------------------------------
//	Construct a DialogBox from a WIND Resource with the specified attributes
//	and SuperCommander
//
//	Side Effect: Created window becomes the current port

LAMDialogBox::LAMDialogBox(
	ResIDT			inWINDid,
	UInt32			inAttributes,
	LCommander*		inSuper)

	: LWindow(inWINDid, inAttributes, inSuper)
{
	mDefaultButtonID = PaneIDT_Undefined;
	mCancelButtonID  = PaneIDT_Undefined;
}


// ---------------------------------------------------------------------------
//	¥ LAMDialogBox							Constructor				  [public]
// ---------------------------------------------------------------------------

LAMDialogBox::LAMDialogBox(
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
//	¥ LAMDialogBox							Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LAMDialogBox::LAMDialogBox(
	LStream*	inStream)

	: LWindow(inStream)
{
	*inStream >> mDefaultButtonID;
	*inStream >> mCancelButtonID;
}


// ---------------------------------------------------------------------------
//	¥ ~LAMDialogBox							Destructor				  [public]
// ---------------------------------------------------------------------------

LAMDialogBox::~LAMDialogBox()
{
}


// ---------------------------------------------------------------------------
//	¥ FinishCreateSelf											   [protected]
// ---------------------------------------------------------------------------

void
LAMDialogBox::FinishCreateSelf()
{
	SetDefaultTag(mDefaultButtonID, true);
	SetCancelButton(mCancelButtonID);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SetDefaultButton												  [public]
// ---------------------------------------------------------------------------
//	Specify the PaneID of the default button of a DialogBox. The default
//	button must be an LControlPane that handles the data tag
//	kControlPushButtonDefaultTag. Usually, it will be a LPushButton.
//
//	Use PaneIDT_Undefined to specify no default button
//
//	Keyboard equivalents (Return and Enter) simulate a click inside the
//	default button.

void
LAMDialogBox::SetDefaultButton(
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
LAMDialogBox::SetCancelButton(
	PaneIDT	inButtonID)
{
	mCancelButtonID = inButtonID;

	if (inButtonID != PaneIDT_Undefined) {
		LControl*	theButton = dynamic_cast<LControl*>(FindPaneByID(inButtonID));
		if (theButton != nil) {
			theButton->AddListener(this);
		}
		
		SignalIf_( theButton == nil );
	}
}


// ---------------------------------------------------------------------------
//	¥ SetDefaultTag												   [protected]
// ---------------------------------------------------------------------------
//	Set whether or not a Button is the default one
//
//	PaneID must be for a LControl object that handles the
//	kControlPushButtonDefaultTag data tag

void
LAMDialogBox::SetDefaultTag(
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
LAMDialogBox::HandleKeyPress(
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
LAMDialogBox::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam)
{
	if (inMessage == cmd_Close) {
		DoClose();

	} else if (inMessage < 0) {
									// Relay message to supercommander
		if (GetSuperCommander() != nil) {
			SAMDialogResponse	theResponse;
			theResponse.dialogBox = this;
			theResponse.messageParam = ioParam;
			ProcessCommand(inMessage, &theResponse);
		}
	}
}


PP_End_Namespace_PowerPlant
