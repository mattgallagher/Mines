// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCmdBevelButton.cp			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	A BevelButton which sends a command to the current target when clicked

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LCmdBevelButton.h>
#include <LCommander.h>
#include <LStream.h>
#include <PP_Messages.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LCmdBevelButton						Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LCmdBevelButton::LCmdBevelButton(
	LStream*	inStream,
	ClassIDT	inImpID)

	: LBevelButton(inStream, inImpID)
{
	*inStream >> mCommand;

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ LCmdBevelButton						Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Constructor for a CmdBevelButton with no menu
//	It can be a Push, Toggle, or Sticky Button.
//
//	See ConstrolDefinitions.h for enumerated constants for
//		bevel proc
//		behavior
//		content type
//		title placement
//		title alignment
//		graphic alignment

LCmdBevelButton::LCmdBevelButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inBevelProc,
	SInt16				inBehavior,
	SInt16				inContentType,
	SInt16				inContentResID,
	ResIDT				inTextTraits,
	ConstStringPtr		inTitle,
	SInt16				inInitialValue,
	SInt16				inTitlePlacement,
	SInt16				inTitleAlignment,
	SInt16				inTitleOffset,
	SInt16				inGraphicAlignment,
	Point				inGraphicOffset,
	CommandT			inCommand,
	ClassIDT			inImpID)

	: LBevelButton(inPaneInfo, inValueMessage, inBevelProc, inBehavior,
						inContentType, inContentResID, inTextTraits,
						inTitle, inInitialValue, inTitlePlacement,
						inTitleAlignment, inTitleOffset, inGraphicAlignment,
						inGraphicOffset, inImpID)
{
	mCommand = inCommand;

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ LCmdBevelButton						Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Constructor for a CmdBevelButton with a Menu
//
//	See ConstrolDefinitions.h for enumerated constants for
//		bevel proc
//		menu placement
//		content type
//		title placement
//		title alignment
//		graphic alignment

LCmdBevelButton::LCmdBevelButton(
	const SPaneInfo&	inPaneInfo,
	MessageT			inValueMessage,
	SInt16				inBevelProc,
	ResIDT				inMenuID,
	SInt16				inMenuPlacement,
	SInt16				inContentType,
	SInt16				inContentResID,
	ResIDT				inTextTraits,
	ConstStringPtr		inTitle,
	SInt16				inInitialValue,
	SInt16				inTitlePlacement,
	SInt16				inTitleAlignment,
	SInt16				inTitleOffset,
	SInt16				inGraphicAlignment,
	Point				inGraphicOffset,
	Boolean				inCenterPopupGlyph,
	CommandT			inCommand,
	ClassIDT			inImpID)

	: LBevelButton(inPaneInfo, inValueMessage, inBevelProc, inMenuID,
						inMenuPlacement, inContentType, inContentResID,
						inTextTraits, inTitle, inInitialValue,
						inTitlePlacement, inTitleAlignment, inTitleOffset,
						inGraphicAlignment, inGraphicOffset,
						inCenterPopupGlyph, inImpID)
{
	mCommand = inCommand;

	StartRepeating();
}


// ---------------------------------------------------------------------------
//	¥ ~LCmdBevelButton						Destructor				  [public]
// ---------------------------------------------------------------------------

LCmdBevelButton::~LCmdBevelButton()
{
}


// ---------------------------------------------------------------------------
//	¥ SpendTime														  [public]
// ---------------------------------------------------------------------------

void
LCmdBevelButton::SpendTime(
	const EventRecord&	/* inMacEvent */)
{
		// Enable or disable the button based on the state of
		// its associated command

		// Check if command status could have changed and if
		// we have a useful command

	if ( LCommander::GetUpdateCommandStatus() &&
		 (mCommand != cmd_Nothing) ) {

		 	// Query target about the status of our command

		LCommander*	target = LCommander::GetTarget();
		if (target != nil) {

			Boolean		cmdEnabled = false;
			Boolean		usesMark   = false;
			UInt16		mark;
			Str255		cmdName;

			target->ProcessCommandStatus(mCommand, cmdEnabled, usesMark,
											mark, cmdName);

			if (cmdEnabled) {		// Match visual state of Button
				Enable();			//   to the state of its command
			} else {
				Disable();
			}


			if (usesMark) {			// For Buttons associated with a
									//   markable menu item, Button is
									//   on when item is marked.
				SInt32	value = Button_Off;
				if (mark != noMark) {
					value  = Button_On;
				}
				SetValue(value);
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ HotSpotResult												   [protected]
// ---------------------------------------------------------------------------
//	Respond to a click in a BevelButton

void
LCmdBevelButton::HotSpotResult(
	SInt16	inHotSpot)
{
									// Let superclass do its thing
	LBevelButton::HotSpotResult(inHotSpot);

	if (mCommand != cmd_Nothing) {	// We have a useful command

		LCommander* target = LCommander::GetTarget();

		if (target != nil) {		// There is a target to handle our command

				// We pass a parameter with our command. For regular
				// buttons, we pass our value, which is either
				// Button_On or Button_Off. For buttons with a popup
				// menu, we pass the selected menu item index

			SInt32	value = mValue;

			if (mBehavior == kControlBehaviorPopup) {
				value = GetCurrentMenuItem();
			}

			target->ProcessCommand(mCommand, &value);
		}
	}
}


PP_End_Namespace_PowerPlant
