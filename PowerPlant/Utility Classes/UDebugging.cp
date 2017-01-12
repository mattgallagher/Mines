// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugging.cp				PowerPlant 2.2.5	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Utility functions for debugging

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <UDebugging.h>
#include <PP_Constants.h>
#include <Dialogs.h>
#include <Processes.h>


// ---------------------------------------------------------------------------
//	Automatic release notes via compiler warning messages
//
//		To turn off the warnings, put
//			#define PP_Suppress_Notes_225	1
//		in your prefix file or edit the !PP_Notes_225.txt file

#if !PP_Suppress_Notes_225
	#include "!PP_Notes_225.txt"
#endif


// ---------------------------------------------------------------------------
//	Function name identifier

#if !PP_Supports_Function_Identifier

	const char	__func__[] = { 0 };		// Compiler doesn't support __func__
										//   so we define it as null string
#endif


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	Global variables for specifying the action to take for
//	a Throw and a Signal

EDebugAction	UDebugging::gDebugThrow  = debugAction_Nothing;
EDebugAction	UDebugging::gDebugSignal = debugAction_Nothing;


// ---------------------------------------------------------------------------
//	Alert resources and item numbers of buttons

const	ResIDT	ALRT_ThrowAt	= 251;
const	ResIDT	ALRT_SignalAt	= 252;

const	SInt16	button_Continue	= 1;
const	SInt16	button_Abort	= 4;
const	SInt16	button_Debugger	= 5;
const	SInt16	button_Quiet	= 6;


// ---------------------------------------------------------------------------
//	Local function prototypes

namespace UDebugging {

	unsigned char*	LoadPStrFromCStr(
							Str255			outPStr,
							const char*		inCStr);
}


// ---------------------------------------------------------------------------
//	¥ DebugThrow
// ---------------------------------------------------------------------------

void
UDebugging::DebugThrow(
	ExceptionCode		inErr,
	const char*			inFunction,
	ConstStringPtr		inFile,
	long				inLine)
{
	unsigned char	errStr[16];
	::NumToString(inErr, errStr);

	if (gDebugThrow == debugAction_Alert) {
		AlertThrowAt(errStr, inFunction, inFile, inLine);

	} else if (gDebugThrow == debugAction_Debugger) {
		::DebugStr(errStr);
	}

	Throw_Err(inErr);
}


// ---------------------------------------------------------------------------
//	¥ DebugThrow
// ---------------------------------------------------------------------------

void
UDebugging::DebugThrow(
	ExceptionCode		inErr,
	const char*			inFunction,
	const char*			inFile,
	long				inLine)
{
	unsigned char	errStr[16];
	::NumToString(inErr, errStr);

	if (gDebugThrow == debugAction_Alert) {
		AlertThrowAt(errStr, inFunction, inFile, inLine);

	} else if (gDebugThrow == debugAction_Debugger) {
		::DebugStr(errStr);
	}

	Throw_Err(inErr);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ LoadPStrFromCStr
// ---------------------------------------------------------------------------
//	Copy contents of a C string into a Pascal string
//
//	Returns a pointer to the Pascal string

unsigned char*
UDebugging::LoadPStrFromCStr(
	Str255			outPStr,
	const char*		inCStr)
{
		// Find length of C string by searching for the terminating
		// null character. However, don't bother to look past 255
		// characters, since that's all that a Pascal string can hold.

	unsigned char	strLength = 0;

	while ( (strLength < 255)  &&  (inCStr[strLength] != '\0') ) {
		strLength += 1;
	}

	outPStr[0] = strLength;
	::BlockMoveData(inCStr, outPStr + 1, strLength);

	return outPStr;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AlertThrowAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Throw occurs

void
UDebugging::AlertThrowAt(
	ConstStringPtr		inError,
	const char*			inFunction,
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	Str255	funcStr;
	LoadPStrFromCStr(funcStr, inFunction);

	Str15	lineStr;
	::NumToString(inLine, lineStr);

	::ParamText(inError, funcStr, inFile, lineStr);

	SInt16	button = ::StopAlert(ALRT_ThrowAt, nil);

	switch (button) {

//		case button_Continue:		// Do nothing
//			break;

		case button_Abort:
			::ExitToShell();
			break;

		case button_Debugger:
			::Debugger();
			break;

		case button_Quiet:
			gDebugThrow  = debugAction_Nothing;
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ AlertThrowAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Throw occurs

void
UDebugging::AlertThrowAt(
	ConstStringPtr		inError,
	const char*			inFunction,
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	fileStr;

	AlertThrowAt(inError,
				 inFunction,
				 LoadPStrFromCStr(fileStr, inFile),
				 inLine);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	ConstStringPtr		inTestStr,		// Pascal string
	const char*			inFunction,
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	Str255	funcStr;
	LoadPStrFromCStr(funcStr, inFunction);

	Str15	lineStr;
	::NumToString(inLine, lineStr);

	::ParamText(inTestStr, funcStr, inFile, lineStr);

	SInt16	button = ::StopAlert(ALRT_SignalAt, nil);

	switch (button) {

//		case button_Continue:		// Do nothing
//			break;

		case button_Abort:
			::ExitToShell();
			break;

		case button_Debugger:
			::Debugger();
			break;

		case button_Quiet:
			gDebugSignal  = debugAction_Nothing;
			break;
	}
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	ConstStringPtr		inTestStr,		// Pascal string
	const char*			inFunction,
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	fileStr;

	AlertSignalAt(inTestStr,
				  inFunction,
				  LoadPStrFromCStr(fileStr, inFile),
				  inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	const char*			inTestStr,		// C string
	const char*			inFunction,
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	Str255	testStr;

	AlertSignalAt(LoadPStrFromCStr(testStr, inTestStr),
				  inFunction,
				  inFile,
				  inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	const char*			inTestStr,		// C string
	const char*			inFunction,
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	testStr;
	Str255	fileStr;

	AlertSignalAt(LoadPStrFromCStr(testStr, inTestStr),
				  inFunction,
				  LoadPStrFromCStr(fileStr, inFile),
				  inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalOSStatusAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalOSStatusAt(
	OSStatus			inStatus,
	const char*			inFunction,
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	Str63	statusStr = "\pStatus Code = ";

	Str15	numStr;
	::NumToString(inStatus, numStr);
	
	::BlockMoveData(numStr + 1, statusStr + statusStr[0] + 1, numStr[0]);
	
	statusStr[0] += numStr[0];

	AlertSignalAt(statusStr, inFunction, inFile, inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalOSStatusAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalOSStatusAt(
	OSStatus			inStatus,
	const char*			inFunction,
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	fileStr;

	AlertSignalOSStatusAt(inStatus,
				  inFunction,
				  LoadPStrFromCStr(fileStr, inFile),
				  inLine);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	DebugNumber
// ---------------------------------------------------------------------------
//	Convert a number into a string to pass to a Debugger

void
UDebugging::DebugNumber(
	SInt32		inNumber)
{
	Str15	numStr;
	::NumToString(inNumber, numStr);
	::DebugStr(numStr);
}


PP_End_Namespace_PowerPlant
