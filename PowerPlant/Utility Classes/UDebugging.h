// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UDebugging.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Debugging macros and functions
//
//		These debugging macros let you control what happens when you
//		throw an exception or raise a signal. By defining certain
//		compiler symbols and global variables, you can display an alert,
//		break into the low-level debugger, or break into the source-level
//		debugger before an exception is thrown or when a signal is raised.
//
//
//	Throw options
//
//		The macros for throwing exceptions defined in UException.h all
//		eventually invoke the Throw_ macro. For example,
//			ThrowIfOSErr_(err) is defined as if ((err) != 0) Throw_(err)
//
//		This header defines the Throw_ macro. The definition used depends
//		on the setting the the Debug_Throw compiler symbol. If you
//		don't #define Debug_Throw, Throw_ is defined to just call the
//		Throw() function. This creates no extra runtime overhead; the
//		preprocessor does all the work.
//
//		However, if you #define Debug_Throw, the Throw_ macro is defined to
//		check the static variable sDebugThrow to decide what to do before
//		eventually calling Throw(). The value of gDebugThrow may be:
//
//			debugAction_Nothing				do nothing
//			debugAction_Alert				display an Alert box
//			debugAction_Debugger			break into debugger
//
//		The Alert box displays the Exception code, as well as the file
//		name and line number where the Throw_ was made.
//
//		Breaking into the debugger will either stop execution in the
//		Metrowerks source debugger or MacsBug (or a third-party low-level
//		debugger). The MW source debugger has a preference which controls
//		whether it intercepts the break or lets it fall to the low-level
//		debugger.
//
//		WARNING: Breaking into the debugger will probably crash your machine
//		if you aren't running the source debugger and don't have a low-level
//		debugger installed.
//
//
//	Signal options
//
//		This header also defines macros for raising Signals. The
//		SignalStringLiteral_(str) macro takes a string literal
//		argument (text enclosed in double quotes). Do NOT include a
//		leading \p for a Pascal string. The macro will add a \p
//		based on compiler settings.
//
//		The SignalString_(str) macro takes a string variable as an
//		argument. The string variable can be a Pascal string (unsigned char*
//		with length byte) or a C string (char * with a null terminator).
//		This macro does work with string literals, but you should use
//		the SignalStringLiteral_() macro for those instead.
//
//		The SignalIf_(test) and SignalIfNot_(test) macros each take a
//		boolean condition as an argument, and raise a signal depending
//		on whether the condition is true or false.
//
//		If you don't #define Debug_Signal, the Signal macros do nothing.
//		There is no runtime overhead; the preprocessor substitutes
//		white space for the macros.
//
//		If you #define Debug_Signal, then the macros are defined to check
//		the static variable sDebugSignal to decide what to do. The options
//		are the same as those describe above for gDebugThrow.
//
//
//	Usage Notes
//
//		By default, sDebugThrow and sDebugSignal are set to
//		debugAction_Nothing. These are runtime variables, so you can set
//		their values at any point in the program. Usually, you will set
//		their values at the beginning of your main program, but you can set
//		them in other places if you want to use different options in different
//		sections of code.
//
//		Another technique is to change the values of sDebugThrow and/or
//		sDebugSignal from the source-level debugger when you are stopped
//		at a breakpoint.

#ifndef _H_UDebugging
#define _H_UDebugging
#pragma once

#include <PP_Types.h>
#include <LException.h>
#include <TextUtils.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


// ---------------------------------------------------------------------------
//	Function name identifier

#if !PP_Supports_Function_Identifier

	extern const char	__func__[];			// Compiler doesn't support __func__
											//   so we define it ourselves
#endif


PP_Begin_Namespace_PowerPlant


typedef long	ExceptionCode;

// ---------------------------------------------------------------------------
// ¥ Debugging Actions

typedef enum {
	debugAction_Nothing				= 0,
	debugAction_Alert				= 1,
	debugAction_Debugger			= 2,

	debugAction_LowLevelDebugger	= 2,	// Obsolete. Use debugAction_Debugger and set
	debugAction_SourceDebugger		= 2		// preference in source debugger to pick option
} EDebugAction;

template <class T> class StValueChanger;	// Forward declaration


// ---------------------------------------------------------------------------
//	Macro for throwing an exception

#ifndef Throw_Err
	#if PP_Obsolete_ThrowExceptionCode		// Old way. Throw an ExceptionCode
											//   which is just a long
		#define Throw_Err(err)		throw (PP_PowerPlant::ExceptionCode)(err)

	#else									// New way. Call function.

		#define Throw_Err(err)		PP_PowerPlant::LException::Throw(err)

	#endif
#endif


// ---------------------------------------------------------------------------

namespace	UDebugging {

	extern EDebugAction		gDebugThrow;
	extern EDebugAction		gDebugSignal;

	void			DebugThrow(
							ExceptionCode		inErr,
							const char*			inFunction,
							ConstStringPtr		inFile,
							long				inLine);

	void			DebugThrow(
							ExceptionCode		inErr,
							const char*			inFunction,
							const char*			inFile,
							long				inLine);

	void			AlertThrowAt(
							ConstStringPtr		inError,
							const char*			inFunction,
							ConstStringPtr		inFile,
							long				inLine);

	void			AlertThrowAt(
							ConstStringPtr		inError,
							const char*			inFunction,
							const char*			inFile,
							long				inLine);

	void			AlertSignalAt(
							ConstStringPtr		inTestStr,
							const char*			inFunction,
							ConstStringPtr		inFile,
							long				inLine);

	void			AlertSignalAt(
							ConstStringPtr		inTestStr,
							const char*			inFunction,
							const char*			inFile,
							long				inLine);

	void			AlertSignalAt(
							const char*			inTestStr,
							const char*			inFunction,
							ConstStringPtr		inFile,
							long				inLine);

	void			AlertSignalAt(
							const char*			inTestStr,
							const char*			inFunction,
							const char*			inFile,
							long				inLine);

	void			AlertSignalOSStatusAt(
							OSStatus			inStatus,
							const char*			inFunction,
							ConstStringPtr		inFile,
							long				inLine);

	void			AlertSignalOSStatusAt(
							OSStatus			inStatus,
							const char*			inFunction,
							const char*			inFile,
							long				inLine);

	inline void		SetDebugThrow( EDebugAction	 inAction )
						{
							gDebugThrow = inAction;
						}

	inline void		SetDebugSignal( EDebugAction inAction )
						{
							gDebugSignal = inAction;
						}

	inline EDebugAction	GetDebugThrow()		{ return gDebugThrow; }
	inline EDebugAction	GetDebugSignal()	{ return gDebugSignal; }

	inline void		DebugString( ConstStringPtr inPStr )
						{
							::DebugStr(inPStr);
						}

	inline void		DebugString( const char* inCStr )
						{
							#if TARGET_OS_MAC && CGLUESUPPORTED
								::debugstr(inCStr);
							#else
								#pragma unused(inCStr)
								::Debugger();	// Too bad. Ignore the string
							#endif
						}
						
	void			DebugNumber( SInt32 inNumber );
}


// ---------------------------------------------------------------------------
// ¥ Obsolete Macros for breaking into the debugger

		// Just call Debugger() or DebugStr() directly. These macros
		// are an artifact from the days when debugging worked
		// differently on 68K and PPC. Now, the MW source debugger
		// has a preference option that controls whether it intercepts
		// Debugger/DebugStr calls.

#define	BreakToLowLevelDebugger_()		::Debugger()
#define	BreakStrToLowLevelDebugger_(s)	::DebugStr(s)

#if TARGET_CPU_68K
	#define	BreakToSourceDebugger_()		::SysBreak()
	#define	BreakStrToSourceDebugger_(s)	::SysBreakStr(s)

#else
	#define	BreakToSourceDebugger_()		::Debugger()
	#define	BreakStrToSourceDebugger_(s)	::DebugStr(s)
#endif


// ---------------------------------------------------------------------------
// ¥ Throw Debugging

#ifdef Debug_Throw

									// Set the Throw debugging option
	#define SetDebugThrow_(inAction)										\
				PP_PowerPlant::UDebugging::SetDebugThrow(inAction)

									// Throw an exception with debugging info
	#define Throw_(err)														\
		PP_PowerPlant::UDebugging::DebugThrow((err), __func__, StringLiteral_(__FILE__), __LINE__)

									// Temporarily disable Throw debugging
	#define StDisableDebugThrow_()											\
			PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
				__okToThrow(PP_PowerPlant::UDebugging::gDebugThrow,			\
							PP_PowerPlant::debugAction_Nothing)

#else

	#define SetDebugThrow_(inAction)

	#define Throw_(err)		Throw_Err(err)

	#define StDisableDebugThrow_()

#endif // Debug_Throw


// ---------------------------------------------------------------------------
// ¥ Signal Debugging

#ifdef Debug_Signal

									// Set the Signal debugging option
	#define SetDebugSignal_(inAction)										\
				PP_PowerPlant::UDebugging::SetDebugSignal(inAction)

									// Signal with a string message, which
									//   may be either a Pascal or a C string.
									//   Although this macro works, use
									//   SignalStringLiteral_() for string
									//   literals. Use this macro for string
									//   variables.
	#define SignalString_(str)												\
		do {																\
			if (PP_PowerPlant::UDebugging::GetDebugSignal() ==				\
					PP_PowerPlant::debugAction_Alert) {						\
				PP_PowerPlant::UDebugging::AlertSignalAt(					\
								str, __func__,								\
								StringLiteral_(__FILE__), __LINE__);		\
			} else if (PP_PowerPlant::UDebugging::GetDebugSignal() ==		\
					PP_PowerPlant::debugAction_Debugger) {					\
				PP_PowerPlant::UDebugging::DebugString(str);				\
			}																\
		} while (false)

									// Signal with a string literal message,
									//   which must be quoted string with
									//   no \p at the beginning
	#define SignalStringLiteral_(str)	SignalString_(StringLiteral_(str))

									// Signal if the test condition is true
	#define SignalIf_(test)								\
	    do {											\
	        if (test) SignalStringLiteral_(#test);		\
	    } while (false)

									// Signal if the test condition is false
	#define SignalIfNot_(test)			SignalIf_(!(test))
	
									// Signal if the status in not noErr
	#define SignalIfOSStatus_(status)										\
		do {																\
			OSStatus	__theStatus = status;								\
			if (__theStatus != noErr) {										\
				if (PP_PowerPlant::UDebugging::GetDebugSignal() ==			\
						PP_PowerPlant::debugAction_Alert) {					\
					PP_PowerPlant::UDebugging::AlertSignalOSStatusAt(		\
									__theStatus, __func__,					\
									StringLiteral_(__FILE__), __LINE__);	\
				} else if (PP_PowerPlant::UDebugging::GetDebugSignal() ==	\
						PP_PowerPlant::debugAction_Debugger) {				\
					PP_PowerPlant::UDebugging::DebugNumber(status);			\
				}															\
			}																\
		} while (false)
		
									// Signal if the err is not noErr
	#define SignalIfOSErr_(err)			SignalIfOSStatus_((OSStatus)(err))

									// Temporarily disable Signals
	#define StDisableDebugSignal_()											\
			PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
				__okToSignal(PP_PowerPlant::UDebugging::gDebugSignal,		\
							PP_PowerPlant::debugAction_Nothing)

#else

	#define SetDebugSignal_(inAction)

	#define SignalString_(str)
	#define SignalStringLiteral_(str)
	#define SignalIf_(test)
	#define SignalIfNot_(test)
	#define	SignalIfOSStatus_(status)
	#define	SignalIfOSErr_(err)
	
	#define StDisableDebugSignal_()

#endif // Debug_Signal

									// Signal with a Pascal string message,
									//   which may be a literal or a variable
									//   Obsolete - Use SignalString_()
#define SignalPStr_(pstr)		SignalString_(pstr)

									// Signal with a C string message, which
									//   must a literal C string
									//   Obsolete - Use SignalStringLiteral_()
#define SignalCStr_(cstr)		SignalStringLiteral_(cstr)

#define Assert_(test)			SignalIfNot_(test)


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
