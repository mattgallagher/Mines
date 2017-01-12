// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LOutlineKeySelector.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LOutlineKeySelector
#define _H_LOutlineKeySelector
#pragma once

#include <LAttachment.h>
#include <UTables.h>

#include <Events.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LOutlineTable;
struct SCommandStatus;


// ===========================================================================
//		¥ LOutlineKeySelector
// ===========================================================================
//	An OutlineKeySelector is designed to be attached to an OutlineTable.
//	When the table (which must also derived from LCommander) is in the
//	target chain, this attachment will handle the following key/command
//	behaviors:
//
//	Keystrokes
//	----------
//		Up arrow:		Select the next cell above the current selection.
//		Down arrow:		Select the next cell below the current selection.
//						(The shift key may be added to the up or down
//						arrows to signify that the selection is extended.)
//
//		Left arrow:		Collapse all selected cells.
//		Right arrow:	Expand all selected cells. If the option key is
//						down, a deep expand is performed.
//
//	Menu commands
//	-------------
//		Select All:		Selects all cells in the outline.
//

class LOutlineKeySelector : public LAttachment {

public:
	enum { class_ID = FOUR_CHAR_CODE('okey') };

							LOutlineKeySelector(
									LOutlineTable*		inOutlineTable,
									MessageT			inMessage = msg_AnyMessage);
							LOutlineKeySelector(
									LStream*			inStream);
	virtual					~LOutlineKeySelector();

	// event dispatching

protected:
	virtual void			ExecuteSelf(
									MessageT			inMessage,
									void*				ioParam);

	virtual void			FindCommandStatus(
									SCommandStatus*		inCommandStatus);
	virtual void			HandleKeyEvent(
									const EventRecord*	inEvent);

	// selection behaviors

	virtual void			SelectAll();

	virtual void			UpArrow(
									Boolean				inExtendSelection);
	virtual void			DownArrow(
									Boolean				inExtendSelection);

	virtual void			ScrollRowIntoFrame(
									TableIndexT			inRow);

	// collapse/expand behaviors

	virtual void			CollapseSelection();
	virtual void			ExpandSelection();
	virtual void			DeepExpandSelection();


	LOutlineTable*			mOutlineTable;

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
