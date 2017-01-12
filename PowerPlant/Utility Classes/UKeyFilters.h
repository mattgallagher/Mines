// ===========================================================================
//	UKeyFilters.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UKeyFilters
#define _H_UKeyFilters
#pragma once

#include <PP_Prefix.h>

#include <Events.h>
#include <TextEdit.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

enum	EKeyStatus {
	keyStatus_Input,			// Input character
	keyStatus_TEDelete,			// TextEdit Delete key
	keyStatus_TECursor,			// TextEdit Cursor movement key
	keyStatus_ExtraEdit,		// Edit key not supported by TE
	keyStatus_Ignore,			// Do nothing with the keystroke
	keyStatus_Reject,			// Invalid keystroke
	keyStatus_PassUp			// Pass keystroke to next handler
};

enum EKeyFilter {
	keyFilter_None				= 0,
	keyFilter_Integer			= 1,
	keyFilter_AlphaNumeric		= 2,
	keyFilter_PrintingChar		= 3,
	keyFilter_NegativeInteger	= 4,
	keyFilter_PrintingCharAndCR	= 5
};


typedef EKeyStatus	(*KeyFilterFunc)(const EventRecord&);	// Old Filter

	// TEKeyFilterFunc - Filter function gets passed a TEHandle
	// as well as information about the keystroke. Information in
	// the TEHandle, such as the selection and the text already
	// in the field, let's the Filter perform context-sensitive
	// filtering. And the character code is an i/o parameter so
	// that the Filter can change it (for example, automatically
	// converting from lower to upper case).

typedef EKeyStatus	(*TEKeyFilterFunc)(TEHandle			inMacTEH,
									   UInt16			inKeyCode,
									   UInt16			&ioCharCode,
									   EventModifiers	inModifiers);

// ---------------------------------------------------------------------------

class UKeyFilters {

public:
	static EKeyStatus	IntegerField(const EventRecord	&inKeyEvent);
	static EKeyStatus	AlphaNumericField(const EventRecord	&inKeyEvent);
	static EKeyStatus	PrintingCharField(const EventRecord	&inKeyEvent);

	static TEKeyFilterFunc	SelectTEKeyFilter(
								SInt8		inSelector);

	static EKeyStatus	IntegerField(
								TEHandle		inMacTEH,
								UInt16			inKeyCode,
								UInt16			&ioCharCode,
								EventModifiers	inModifiers);

	static EKeyStatus	AlphaNumericField(
								TEHandle		inMacTEH,
								UInt16			inKeyCode,
								UInt16			&ioCharCode,
								EventModifiers	inModifiers);

	static EKeyStatus	PrintingCharField(
								TEHandle		inMacTEH,
								UInt16			inKeyCode,
								UInt16			&ioCharCode,
								EventModifiers	inModifiers);

	static EKeyStatus	NegativeIntegerField(
								TEHandle		inMacTEH,
								UInt16			inKeyCode,
								UInt16			&ioCharCode,
								EventModifiers	inModifiers);

	static EKeyStatus	PrintingCharAndCRField(
								TEHandle		inMacTEH,
								UInt16			inKeyCode,
								UInt16			&ioCharCode,
								EventModifiers	inModifiers);

	static bool			IsTEDeleteKey(UInt16 inKey);
	static bool			IsTECursorKey(UInt16 inKey);
	static bool			IsExtraEditKey(UInt16 inKey);

	static bool			IsNavigationKey(UInt16 inKey);
	static bool			IsActionKey(UInt16 inKey);

	static bool			IsNonprintingChar(UInt16 inChar);
	static bool			IsPrintingChar(UInt16 inChar);
	static bool			IsNumberChar(UInt16 inChar);
	static bool			IsLetterChar(UInt16 inChar);

	static bool			IsEscapeKey(const EventRecord &inKeyEvent);
	static bool			IsCmdPeriod(const EventRecord &inKeyEvent);

	static bool			IsHelpKey(const EventRecord &inKeyEvent);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
