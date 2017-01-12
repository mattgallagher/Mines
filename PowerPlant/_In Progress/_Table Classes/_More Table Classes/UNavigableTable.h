// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UNavigableTable.h			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//  Original Author:  Andy Dent
//
//	Mixin parent class for LTextTableView, CDragDropOutlineTable etc.
//	makes table an LCommander which can be scrolled by typing

#ifndef _H_UNavigableTable
#define _H_UNavigableTable
#pragma once

#include <LCommander.h>

PP_Begin_Namespace_PowerPlant
	class	LFocusBox;
	class	LTableView;
PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LNavTableTypingTimeout;

class	LNavigableTable : public LCommander {
public:

						LNavigableTable	(LTableView* mTable);
	virtual				~LNavigableTable();

	virtual void		BeTarget();
	virtual void		DontBeTarget();

	virtual void		DoNavigationKey(
								const EventRecord	&inKeyEvent);
	virtual void		DoTypeSelection(
								const EventRecord	&inKeyEvent);
	virtual void		DoStringSelection(
								const char* inChars, unsigned int numChars );

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void				*ioParam = nil);

	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean				&outEnabled,
								Boolean				&outUsesMark,
								UInt16				&outMark,
								Str255				outName);

	virtual Boolean		HandleKeyPress(
								const EventRecord&	inKeyEvent);

protected:
	LTableView*		mTable;
	LFocusBox*		mFocusBox;
	LNavTableTypingTimeout*	mTimer;	// owned
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
