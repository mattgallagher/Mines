// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBroadcasterEditField.cp	PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LBroadcasterEditField.h>

#include <LStream.h>
#include <UTETextAction.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Resources.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LBroadcasterEditField					Default Constructor		  [public]
// ---------------------------------------------------------------------------

LBroadcasterEditField::LBroadcasterEditField()
{
	mKeyFilter = &UKeyFilters::PrintingCharField;
	mValueMessage = mPaneID;
}


// ---------------------------------------------------------------------------
//	¥ LBroadcasterEditField					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LBroadcasterEditField::LBroadcasterEditField(
	LStream* 	inStream)

	: LEditField(inStream)
{
	mValueMessage = mPaneID;
}


// ---------------------------------------------------------------------------
//	¥ LBroadcasterEditField					Parameterized Constructor [public]
// ---------------------------------------------------------------------------

LBroadcasterEditField::LBroadcasterEditField(
	const SPaneInfo&	inPaneInfo,
	ConstStringPtr		inString,
	ResIDT				inTextTraitsID,
	SInt16				inMaxChars,
	UInt8				inAttributes,
	TEKeyFilterFunc		inKeyFilter,
	LCommander*			inSuper)

: LEditField(inPaneInfo, inString, inTextTraitsID,
			inMaxChars, inAttributes,
			&UKeyFilters::PrintingCharField, inSuper)

{
	mKeyFilter = inKeyFilter;
	mValueMessage = mPaneID;
}


// ---------------------------------------------------------------------------
//	¥ ~LBroadcasterEditField				Destructor				  [public]
// ---------------------------------------------------------------------------

LBroadcasterEditField::~LBroadcasterEditField()
{
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus												  [public]
// ---------------------------------------------------------------------------
//	Make sure the Edit menu items have their standard text.

void
LBroadcasterEditField::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Cut:
			::GetIndString(outName, STRx_DefaultEditStrings, str_CutDefault);
			LEditField::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
			break;

		case cmd_Copy:
			::GetIndString(outName, STRx_DefaultEditStrings, str_CopyDefault);
			LEditField::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
			break;

		case cmd_Paste:
			::GetIndString(outName, STRx_DefaultEditStrings, str_PasteDefault);
			LEditField::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
			break;

		case cmd_Clear:
			::GetIndString(outName, STRx_DefaultEditStrings, str_ClearDefault);
			LEditField::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
			break;

		default:
			LEditField::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
	}
}


// ---------------------------------------------------------------------------
//	¥ BroadcastValueMessage											  [public]
// ---------------------------------------------------------------------------
//	Broadcast a message with the new value of the edit field.

void
LBroadcasterEditField::BroadcastValueMessage()
{
	if (mValueMessage != cmd_Nothing) {
		SInt32 value = GetValue();
		BroadcastMessage(mValueMessage, (void*) &value);
	}
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget												   [protected]
// ---------------------------------------------------------------------------
//	Overriden to broadcast the value message.

void
LBroadcasterEditField::DontBeTarget()
{
	BroadcastValueMessage();
	LEditField::DontBeTarget();
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress											   [protected]
// ---------------------------------------------------------------------------
//	If the enter key is pressed, the value message is sent.

Boolean
LBroadcasterEditField::HandleKeyPress(
	const EventRecord& inKeyEvent)
{
	UInt16		theChar = (UInt16) (inKeyEvent.message & charCodeMask);
	LCommander	*theTarget = GetTarget();

	// Intercept Enter and Return key for immediate value change.

	EKeyStatus theKeyStatus = keyStatus_Input;
	if (mKeyFilter != nil) {
		theKeyStatus = (*mKeyFilter)(mTextEditH, (UInt16) inKeyEvent.message,
										theChar, inKeyEvent.modifiers);
	}

	if ((theKeyStatus != keyStatus_Input)
	  && (UKeyFilters::IsActionKey((UInt16) inKeyEvent.message))) {
		switch (theChar) {
			case char_Return:
			case char_Enter:
				BroadcastValueMessage();
				SelectAll();
		}
	}

	// If the Target is the same, proceed with normal processing.
	// If the Target changed, then our broadcast did something to
	// the command chain (maybe even deleted this object) and we
	// should just return immediately.

	Boolean	keyHandled = true;
	if (theTarget == GetTarget()) {
		keyHandled = LEditField::HandleKeyPress(inKeyEvent);
	}

	return keyHandled;
}


PP_End_Namespace_PowerPlant
