// ===========================================================================
//	UKeyFilters.cp				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UKeyFilters.h>
#include <PP_KeyCodes.h>

#include <Script.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ IntegerField
// ---------------------------------------------------------------------------
//	Key Filter for Integer characters
//
//		> Identify delete and cursor keys
//		> Accept numbers (0 to 9)
//		> Reject all other printing characters
//		> PassUp all other characters

EKeyStatus
UKeyFilters::IntegerField(
	const EventRecord&	inKeyEvent)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;
	UInt16		theKey		 = (UInt16) inKeyEvent.message;
	UInt16		theChar		 = (UInt16) (theKey & charCodeMask);

	if (IsTEDeleteKey(theKey)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(theKey)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(theKey)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(theChar)) {

		if (IsNumberChar(theChar)) {
			theKeyStatus = keyStatus_Input;

		} else {
			theKeyStatus = keyStatus_Reject;
		}
	}

	return theKeyStatus;
}


// ---------------------------------------------------------------------------
//	¥ AlphaNumericField
// ---------------------------------------------------------------------------
//	Key Filter for AlphaNumeric characters
//
//		> Identify delete and cursor keys
//		> Accept numbers (0 to 9) and letters (a to z, and A to Z)
//		> Reject all other printing characters
//		> PassUp all other characters

EKeyStatus
UKeyFilters::AlphaNumericField(
	const EventRecord&	inKeyEvent)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;
	UInt16		theKey		 = (UInt16) inKeyEvent.message;
	UInt16		theChar		 = (UInt16) (theKey & charCodeMask);

	if (IsTEDeleteKey(theKey)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(theKey)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(theKey)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(theChar)) {

		if (IsNumberChar(theChar)  ||  IsLetterChar(theChar)) {
			theKeyStatus = keyStatus_Input;

		} else {
			theKeyStatus = keyStatus_Reject;
		}
	}

	return theKeyStatus;
}


// ---------------------------------------------------------------------------
//	¥ PrintingCharField
// ---------------------------------------------------------------------------
//	Key Filter for Printing characters
//
//		> Identify delete and cursor keys
//		> Accept printing character
//		> PassUp all other characters

EKeyStatus
UKeyFilters::PrintingCharField(
	const EventRecord&	inKeyEvent)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;
	UInt16		theKey		 = (UInt16) inKeyEvent.message;
	UInt16		theChar		 = (UInt16) (theKey & charCodeMask);

	if (IsTEDeleteKey(theKey)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(theKey)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(theKey)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(theChar)) {
		theKeyStatus = keyStatus_Input;
	}

	return theKeyStatus;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ SelectTEKeyFilter
// ---------------------------------------------------------------------------
//	Return a key filter function pointer based on a selector

TEKeyFilterFunc
UKeyFilters::SelectTEKeyFilter(
	SInt8	inSelector)
{
	TEKeyFilterFunc	theFilter = nil;

	switch (inSelector) {

		case keyFilter_Integer:
			theFilter = &UKeyFilters::IntegerField;
			break;

		case keyFilter_AlphaNumeric:
			theFilter = &UKeyFilters::AlphaNumericField;
			break;

		case keyFilter_PrintingChar:
			theFilter = &UKeyFilters::PrintingCharField;
			break;

		case keyFilter_NegativeInteger:
			theFilter = &UKeyFilters::NegativeIntegerField;
			break;

		case keyFilter_PrintingCharAndCR:
			theFilter = &UKeyFilters::PrintingCharAndCRField;
			break;
	}

	return theFilter;
}


// ---------------------------------------------------------------------------
//	¥ IntegerField
// ---------------------------------------------------------------------------
//	Key Filter for Integer characters
//
//		> Identify delete and cursor keys
//		> Accept numbers (0 to 9)
//		> Reject all other printing characters
//		> PassUp all other characters

EKeyStatus
UKeyFilters::IntegerField(
	TEHandle		/* inMacTEH */,
	UInt16			inKeyCode,
	UInt16&			ioCharCode,
	EventModifiers	/* inModifiers */)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;

	if (IsTEDeleteKey(inKeyCode)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(inKeyCode)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(inKeyCode)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(ioCharCode)) {

		if (IsNumberChar(ioCharCode)) {
			theKeyStatus = keyStatus_Input;

		} else {
			theKeyStatus = keyStatus_Reject;
		}
	}

	return theKeyStatus;
}


// ---------------------------------------------------------------------------
//	¥ AlphaNumericField
// ---------------------------------------------------------------------------
//	Key Filter for AlphaNumeric characters
//
//		> Identify delete and cursor keys
//		> Accept numbers (0 to 9) and letters (a to z, and A to Z)
//		> Reject all other printing characters
//		> PassUp all other characters

EKeyStatus
UKeyFilters::AlphaNumericField(
	TEHandle		/* inMacTEH */,
	UInt16			inKeyCode,
	UInt16&			ioCharCode,
	EventModifiers	/* inModifiers */)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;

	if (IsTEDeleteKey(inKeyCode)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(inKeyCode)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(inKeyCode)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(ioCharCode)) {

		if (IsNumberChar(ioCharCode)  ||  IsLetterChar(ioCharCode)) {
			theKeyStatus = keyStatus_Input;

		} else {
			theKeyStatus = keyStatus_Reject;
		}
	}

	return theKeyStatus;
}


// ---------------------------------------------------------------------------
//	¥ PrintingCharField
// ---------------------------------------------------------------------------
//	Key Filter for Printing characters
//
//		> Identify delete and cursor keys
//		> Accept printing character
//		> PassUp all other characters

EKeyStatus
UKeyFilters::PrintingCharField(
	TEHandle		/* inMacTEH */,
	UInt16			inKeyCode,
	UInt16&			ioCharCode,
	EventModifiers	/* inModifiers */)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;

	if (IsTEDeleteKey(inKeyCode)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(inKeyCode)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(inKeyCode)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (IsPrintingChar(ioCharCode)) {
		theKeyStatus = keyStatus_Input;
	}

	return theKeyStatus;
}


// ---------------------------------------------------------------------------
//	¥ NegativeIntegerField
// ---------------------------------------------------------------------------
//	Key Filter for Negative Integer characters
//
//		> Accept minus sign (-) as first character
//		> Otherwise, same as IntegerField

EKeyStatus
UKeyFilters::NegativeIntegerField(
	TEHandle		inMacTEH,
	UInt16			inKeyCode,
	UInt16&			ioCharCode,
	EventModifiers	inModifiers)
{
	EKeyStatus	status = keyStatus_Input;

	if ( ((**inMacTEH).selStart != 0)  ||		// Check for first character
		 (ioCharCode != char_MinusSign) ) {		//   being a minus sign

		status = IntegerField(inMacTEH, inKeyCode, ioCharCode, inModifiers);
	}

	return status;
}


// ---------------------------------------------------------------------------
//	¥ PrintingCharAndCRField
// ---------------------------------------------------------------------------
//	Key Filter for Printing characters
//
//		> Identify delete and cursor keys
//		> Accept printing character
//		> Accept carriage return
//		> PassUp all other characters

EKeyStatus
UKeyFilters::PrintingCharAndCRField(
	TEHandle		/* inMacTEH */,
	UInt16			inKeyCode,
	UInt16&			ioCharCode,
	EventModifiers	/* inModifiers */)
{
	EKeyStatus	theKeyStatus = keyStatus_PassUp;

	if (IsTEDeleteKey(inKeyCode)) {
		theKeyStatus = keyStatus_TEDelete;

	} else if (IsTECursorKey(inKeyCode)) {
		theKeyStatus = keyStatus_TECursor;

	} else if (IsExtraEditKey(inKeyCode)) {
		theKeyStatus = keyStatus_ExtraEdit;

	} else if (ioCharCode == char_Return) {
		theKeyStatus = keyStatus_Input;

	} else if (IsPrintingChar(ioCharCode)) {
		theKeyStatus = keyStatus_Input;
	}

	return theKeyStatus;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ IsTEDeleteKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsTEDeleteKey(
	UInt16	inKey)
{
	return ((inKey & charCodeMask) == char_Backspace);
}


// ---------------------------------------------------------------------------
//	¥ IsTECursorKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsTECursorKey(
	UInt16	inKey)
{
	Boolean	isCursor = false;

	switch (inKey & charCodeMask) {

		case char_LeftArrow:
		case char_RightArrow:
		case char_UpArrow:
		case char_DownArrow:
			isCursor = true;
			break;
	}

	return isCursor;
}


// ---------------------------------------------------------------------------
//	¥ IsExtraEditKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsExtraEditKey(
	UInt16	inKey)
{
	Boolean	isExtraEdit = false;

	switch (inKey & charCodeMask) {

		case char_Home:
		case char_End:
		case char_PageUp:
		case char_PageDown:
		case char_FwdDelete:
			isExtraEdit = true;
			break;

		case char_Clear:
			isExtraEdit = (inKey & keyCodeMask) == vkey_Clear;
			break;
	}

	return isExtraEdit;
}


// ---------------------------------------------------------------------------
//	¥ IsNavigationKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsNavigationKey(
	UInt16	inKey)
{
	Boolean	isNavigation = false;

	switch (inKey & charCodeMask) {

		case char_LeftArrow:
		case char_RightArrow:
		case char_UpArrow:
		case char_DownArrow:
		case char_Home:
		case char_End:
		case char_PageUp:
		case char_PageDown:
			isNavigation = true;
			break;
	}

	return isNavigation;
}


// ---------------------------------------------------------------------------
//	¥ IsActionKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsActionKey(
	UInt16	inKey)
{
	Boolean	isAction = false;

	switch (inKey & charCodeMask) {

		case char_Enter:
		case char_Tab:
		case char_Return:
			isAction = true;
			break;

		case char_Escape:
			isAction = (inKey & keyCodeMask) == vkey_Escape;
			break;
	}

	return isAction;
}


// ---------------------------------------------------------------------------
//	¥ IsNonprintingChar
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsNonprintingChar(
	UInt16	inChar)
{
	return (inChar < char_FirstPrinting) ||
		   (inChar == char_FwdDelete);
}


// ---------------------------------------------------------------------------
//	¥ IsPrintingChar
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsPrintingChar(
	UInt16	inChar)
{
	return (inChar >= char_FirstPrinting) &&
		   (inChar != char_FwdDelete);
}


// ---------------------------------------------------------------------------
//	¥ IsNumberChar
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsNumberChar(
	UInt16	inChar)
{
	return (inChar >= '0') && (inChar <= '9');
}


// ---------------------------------------------------------------------------
//	¥ IsLetterChar
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsLetterChar(
	UInt16	inChar)
{
	return ((inChar >= 'a') && (inChar <= 'z'))  ||	// lower case
		   ((inChar >= 'A') && (inChar <= 'Z'));	// upper case
}


// ---------------------------------------------------------------------------
//	¥ IsEscapeKey
// ---------------------------------------------------------------------------

bool
UKeyFilters::IsEscapeKey(
	const EventRecord&	inKeyEvent)
{
	return ( ((inKeyEvent.message & charCodeMask) == char_Escape) &&
			 ((inKeyEvent.message & keyCodeMask)  == vkey_Escape) );
}


// ---------------------------------------------------------------------------
//	¥ IsCmdPeriod
// ---------------------------------------------------------------------------
//	Return whether a keystroke event is a command-period
//
//	This routine supports international keyboards by using a
//	combination of the techniques described in TechNote Text-23
//	and IM:Text page C-24.

bool
UKeyFilters::IsCmdPeriod(
	const EventRecord&	inKeyEvent)
{
	return ( (inKeyEvent.modifiers & cmdKey)  &&
			 ::IsCmdChar(&inKeyEvent, char_Period) );
}


// ---------------------------------------------------------------------------
//	¥ IsHelpKey
// ---------------------------------------------------------------------------
//	Return whether a keystroke event is a command-? or help key
//
//	This routine supports international keyboards by using a
//	combination of the techniques described in TechNote Text-23
//	and IM:Text page C-24.

bool
UKeyFilters::IsHelpKey(
	const EventRecord&	inKeyEvent)
{
	return ( ((inKeyEvent.message & charCodeMask) == char_Help) ||
			 ((inKeyEvent.modifiers & cmdKey)  && ::IsCmdChar(&inKeyEvent, '?')) );
}


PP_End_Namespace_PowerPlant
