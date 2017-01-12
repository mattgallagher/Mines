// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNavigableTable.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	Mixin parent class for LTextTableView, CDragDropOutlineTable etc.
//	makes table an LCommander which can be scrolled by typing

#include <UNavigableTable.h>

#include <LTableView.h>
#include <LFocusBox.h>
#include <UTableHelpers.h>
#include <LPeriodical.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <UKeyFilters.h>
#include <LString.h>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//		LNavTableTypingTimeout
// ---------------------------------------------------------------------------
class LNavTableTypingTimeout : public LPeriodical {
public:
	LNavTableTypingTimeout(LNavigableTable* inTable, unsigned long inWaitTicks);
	virtual ~LNavTableTypingTimeout() {};

	virtual	void	SpendTime(const EventRecord&);
	void AddChar(char);
	void Enable();
	void Disable();

private:
	LNavigableTable*	 mTable;
	unsigned long	mWaitTicks, mTriggerAt;
	LStr255	mString;
};


LNavTableTypingTimeout::LNavTableTypingTimeout(LNavigableTable* inTable, unsigned long inWaitTicks) :
	mTable(inTable),
	mWaitTicks(inWaitTicks),
	mTriggerAt(0)	// not until told to!
{
}


void
LNavTableTypingTimeout::SpendTime(const EventRecord& /* inMacEvent */)
{
	unsigned long ticksNow = ::TickCount();
	if (mTriggerAt && ticksNow >= mTriggerAt) {
		mTable->DoStringSelection((const char*)&mString[1], mString.Length());
		Disable();
	}
}


void
LNavTableTypingTimeout::AddChar(char inChar)
{
	mString += inChar;
	Enable();
}


void
LNavTableTypingTimeout::Enable()
{
	mTriggerAt = ::TickCount() + mWaitTicks;
	StartRepeating();
}


void
LNavTableTypingTimeout::Disable()
{
	mTriggerAt = 0;
	StopRepeating();
	mString = "";
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ Constructor/Dectructor
// ---------------------------------------------------------------------------
//	Note that you can't create this mixin without passing in the table

LNavigableTable::LNavigableTable(LTableView* inTable) :
	mTable(inTable),
	mFocusBox(0),
	mTimer(0)
{
}


LNavigableTable::~LNavigableTable()
{
	delete mTimer;
}


// ---------------------------------------------------------------------------
//	¥ BeTarget
// ---------------------------------------------------------------------------
//	ListBox is becoming the Target

void
LNavigableTable::BeTarget()
{
	if ((mFocusBox != nil) && mTable->IsVisible()) {
		mFocusBox->Show();
	}
}


// ---------------------------------------------------------------------------
//	¥ DontBeTarget
// ---------------------------------------------------------------------------
//	ListBox is no longer the Target

void
LNavigableTable::DontBeTarget()
{
	if (mFocusBox != nil) {
		mFocusBox->Hide();
	}
}



// ---------------------------------------------------------------------------
//	¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Respond to Command message

Boolean
LNavigableTable::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

		case msg_TabSelect:
			if (!mTable->IsEnabled()) {
				cmdHandled = false;
			}
			break;

		case cmd_SelectAll:
			mTable->SelectAllCells();
			break;

		default:
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}

	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
LNavigableTable::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_SelectAll: {		// Check if cells exist and if List
									//   can have more than one selection
// NOT YET IMPLEMENTED
			break;
		}

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ HandleKeyPress
// ---------------------------------------------------------------------------
//	ListBox supports keyboard navigation and type selection

Boolean
LNavigableTable::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	Boolean	keyHandled = true;
	UInt16	theKey = (UInt16) (inKeyEvent.message & charCodeMask);

	mTable->FocusDraw();

	if (UKeyFilters::IsNavigationKey(theKey)) {
		DoNavigationKey(inKeyEvent);

	}  else if ( (UKeyFilters::IsPrintingChar(theKey)) &&
                 (!(inKeyEvent.modifiers & cmdKey)) ) {
					 if (!mTimer)
					 	mTimer = new LNavTableTypingTimeout(this, 30);	// half second wait = 30 ticks
					mTimer->AddChar((char) theKey);
		// used to, just one key -- DoTypeSelection(inKeyEvent);

	} else {
		keyHandled = LCommander::HandleKeyPress(inKeyEvent);
	}

	return keyHandled;
}


// ---------------------------------------------------------------------------
//	¥ DoNavigationKey
// ---------------------------------------------------------------------------
//	Implements keyboard navigation by supporting selection change using
//	the arrow keys, page up, page down, home, and end

void
LNavigableTable::DoNavigationKey(
	const EventRecord	&inKeyEvent)
{
// NOT YET IMPLEMENTED - arrow keys
// see LOutlineKeySelector::UpArrow etc. for ideas

	char	theKey = (char) (inKeyEvent.message & charCodeMask);
//	Boolean	cmdKeyDown = (inKeyEvent.modifiers & cmdKey) != 0;
//	Boolean	extendSelection = ((inKeyEvent.modifiers & shiftKey) != 0); // AND SELECTION ALLOWS IT

	switch (theKey) {
/*
		case char_LeftArrow:
			if (::LGetSelect(true, &theSelection, mMacListH)) {
				if (cmdKeyDown && extendSelection) {
									// Select all cells left of the first
									//   selected cell
					while (--theSelection.h >= 0) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select leftmost cell in the same row as
									//   the first selected cell
					theSelection.h = 0;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell left of the first selected cell
					if (theSelection.h > 0) {
						theSelection.h -= 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell to the left of the first
									//   selected cell
					if(theSelection.h > 0) {
						theSelection.h -= 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_RightArrow:
			if (GetLastSelectedCell(theSelection)) {
				SInt16	numColumns = (**mMacListH).dataBounds.right - 1;

				if (cmdKeyDown && extendSelection) {
									// Select all cells right of the last
									//   selected cell
					while (++theSelection.h <= numColumns) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select rightmost cell in the same row as
									//   the last selected cell
					theSelection.h = numColumns;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the cell
									//   to the right of the last selected cell
					if(theSelection.h < numColumns) {
						theSelection.h += 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell to the right of the last
									//   selected cell

					if(theSelection.h < numColumns) {
						theSelection.h += 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_UpArrow:
			if (::LGetSelect(true, &theSelection, mMacListH)) {
				if (cmdKeyDown && extendSelection) {
									// Select all cells above the first
									//   selected cell
					while (--theSelection.v >= 0) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select cell at top of column of the
									//   first selected cell
					theSelection.v = 0;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell above the first selected cell
					if (theSelection.v > 0) {
						theSelection.v -= 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell one above the first
									//   selected cell
					if(theSelection.v > 0) {
						theSelection.v -= 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;

		case char_DownArrow:
			if (GetLastSelectedCell(theSelection)) {
				SInt16	numRows = (**mMacListH).dataBounds.bottom - 1;

				if (cmdKeyDown && extendSelection) {
									// Select all cells below the last
									//   selected cell
					while (++theSelection.v <= numRows) {
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else if (cmdKeyDown) {
									// Select cell at bottom of column of
									//   the last selected cell
					theSelection.v = numRows;
					SelectOneCell(theSelection);

				} else if (extendSelection) {
									// Extend selection by selecting the
									//   cell below the last selected cell
					if(theSelection.v < numRows) {
						theSelection.v += 1;
						::LSetSelect(true, theSelection, mMacListH);
					}

				} else {			// Select cell one below the last
									//   selected cell

					if(theSelection.v < numRows) {
						theSelection.v += 1;
					}
					SelectOneCell(theSelection);
				}
				MakeCellVisible(theSelection);
			}
			break;
*/
		case char_Home: {
			STableCell	firstCell(1, 1);
			mTable->ScrollCellIntoFrame(firstCell);
			break;
		}

		case char_End: {
			LTableStorage*	storage = mTable->GetTableStorage();
			if (storage != nil) {
				TableIndexT  numRows, numCols;
				storage->GetStorageSize(numRows, numCols);
				STableCell	lastCell(numRows, 1);
				mTable->ScrollCellIntoFrame(lastCell);
			}
		}
			break;


//Page Up and Down pinched from LKeyScrollAttachment::ExecuteSelf
		case char_PageUp: {		// Scroll up by height of Frame,
								//   but not past top of Image
			SPoint32		frameLoc;
			SPoint32		imageLoc;
			mTable->GetFrameLocation(frameLoc);
			mTable->GetImageLocation(imageLoc);

			SInt32	upMax = frameLoc.v - imageLoc.v;
			if (upMax > 0) {
				SPoint32		scrollUnit;
				SDimension16	frameSize;
				mTable->GetScrollUnit(scrollUnit);
				mTable->GetFrameSize(frameSize);

				SInt32	up = (frameSize.height - 1) / scrollUnit.v;
				if (up <= 0) {
					up = 1;
				}
				up *= scrollUnit.v;
				if (up > upMax) {
					up = upMax;
				}
				mTable->ScrollImageBy(0, -up, true);
			}
			break;
		}

		case char_PageDown: {	// Scroll down by height of Frame,
								//   but not past bottom of Image
			SPoint32		frameLoc;
			SPoint32		imageLoc;
			SDimension16	frameSize;
			SDimension32	imageSize;
			mTable->GetFrameLocation(frameLoc);
			mTable->GetImageLocation(imageLoc);
			mTable->GetFrameSize(frameSize);
			mTable->GetImageSize(imageSize);

			SInt32	downMax = imageSize.height - frameSize.height -
								(frameLoc.v - imageLoc.v);
			if (downMax > 0) {
				SPoint32		scrollUnit;
				mTable->GetScrollUnit(scrollUnit);

				SInt32	down = (frameSize.height - 1) / scrollUnit.v;
				if (down <= 0) {
					down = 1;
				}
				down *= scrollUnit.v;
				if (down > downMax) {
					down = downMax;
				}
				mTable->ScrollImageBy(0, down, true);
			}
			break;
		}

	}

}


// ---------------------------------------------------------------------------
//	¥ DoTypeSelection
// ---------------------------------------------------------------------------
//	Change selection to the item beginning with the input characters

void
LNavigableTable::DoTypeSelection(
	const EventRecord&	 inKeyEvent )
{
// NOT USED any more - we have a timer fired by HandleKeyPress
// HandleKeyPress guarantees this is is a printing char by the time it calls us
	char	theKey = (char) (inKeyEvent.message & charCodeMask);
	DoStringSelection(&theKey, 1);
}


// ---------------------------------------------------------------------------
//	¥ DoStringSelection
// ---------------------------------------------------------------------------
//	Change selection to the item beginning with the input characters

void
LNavigableTable::DoStringSelection(const char* inChars, unsigned int numChars )
{
	STableCell matchingCell;
	bool foundIt = mTable->FindCellData(matchingCell, inChars, numChars);	// just works on one char for now
	if (!foundIt) {
		TableIndexT numRows, numCols;
		mTable->GetTableSize(numRows, numCols);
		matchingCell.col = 1;
		matchingCell.row = numRows;	// goto last cell as we assume data beyond last data
		// and it makes far more sense to show the user that than to leave them wondering what
		// on earth happened
	}
	mTable->ScrollCellIntoFrame(matchingCell);
}


PP_End_Namespace_PowerPlant
