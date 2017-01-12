// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_DebugMacros.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	This file contains various macros for use throughout your code. They
//	perform validations, simplify common tasks, etc.

#ifndef _H_PP_DebugMacros
#define _H_PP_DebugMacros
#pragma once

#include <PP_Debug.h>
#include <LView.h>

#include <UOnyx.h>
#include <UDebugNew.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
//	A variation on SignalString_ (used internally by other Debugging
//	Classes macros) to allow the passing of the __FILE__ and __LINE__
//	constants. The given string can be either a Pascal or C string.

#ifdef Debug_Signal

	#define SignalStringInfo_(str, file, line)								\
		do {																\
			if (PP_PowerPlant::UDebugging::GetDebugSignal() ==				\
					PP_PowerPlant::debugAction_Alert) {						\
				PP_PowerPlant::UDebugging::AlertSignalAt(str, "", file, line);	\
			} else if (PP_PowerPlant::UDebugging::GetDebugSignal() ==		\
					PP_PowerPlant::debugAction_Debugger) {					\
				PP_PowerPlant::UDebugging::DebugString(str);				\
			}																\
		} while (false)

#else

	#define SignalStringInfo_(str, file, line)		{(void) file; (void) line;}

#endif

// ---------------------------------------------------------------------------
//	Variations on the PowerPlant Throw_ macros (used internally by other
//	Debugging Classes macros) to allow the passing of the __FILE__ and __LINE__
//	constants for more accurate debugging information.

#define ThrowWithInfoIfNil_(ptr, file, line)											\
	do {																				\
		if ((ptr) == nil) ThrowWithInfo_(PP_PowerPlant::err_NilPointer, file, line);	\
	} while (false)


#ifdef Debug_Throw

							// Throw an exception with specified debugging info
	#define ThrowWithInfo_(err, file, line)							\
		PP_PowerPlant::UDebugging::DebugThrow((err), "", file, line)

#else

		// The "file" and "line" arguments are used like this to prevent
		// the compiler from generating an "unused argument" warning. No
		// code should be generated so this shouldn't cause any problems.

	#define ThrowWithInfo_(err, file, line)		Throw_Err(err); {(void) file; (void) line;}

#endif



// ---------------------------------------------------------------------------
// Validation macros. QC and DebugNew are used if those supports are enabled.

#if PP_Debug

		//	ValidatePtr_ should be called before dereferencing any pointer.
		//	It may be used with Memory Manager Ptr pointers only.
	#define ValidatePtr_(p)													\
		do {																\
			ThrowIf_(p == nil);												\
			QCVerifyPtr_(p);												\
		} while (false)

		//	ValidateHandle_ should be called before dereferencing any handle.
		//	It may be used with Memory Manager Handles only.
	#define ValidateHandle_(h)												\
		do {																\
			ThrowIf_((h == nil) || ((*h) == nil));							\
			QCVerifyHandle_(h);												\
		} while (false)

#else

	#define ValidatePtr_(p)	 				ThrowIf_(p == nil)
	#define ValidateHandle_(h)				ThrowIf_((h == nil) || ((*h) == nil))

#endif

#if PP_Debug

		//	ValidateObject_ should be called before dereferencing any
		//	C++ object. ValidateObject_ must NOT be used for stack-based
		//	objects. Should function with C++ objects allocated via new
		//	or new[] (provided the version of DebugNew used can validate
		//	objects allocated via new[]).
	#define ValidateObject_(obj)											\
		do {																\
			ThrowIf_(obj == nil);											\
			DebugNewValidatePtr_(const_cast<void*>							\
					(dynamic_cast<const void*>(obj)));						\
		} while (false)

		//	ValidateSimpleObject_ is a variant of ValidateObject_ for
		//	objects which do not have virtual members.
	#define ValidateSimpleObject_(obj)										\
		do {																\
			ThrowIf_(obj == nil);											\
			DebugNewValidatePtr_(static_cast<void*>(obj));					\
		} while (false)

		//	ValidateThis_ is a shortcut for ValidateObject_(this).
		//	It should be placed at the top of object methods where practical.
	#define ValidateThis_()					ValidateObject_(this)

#else

	#define ValidateObject_(obj)			ThrowIf_(obj == nil)
	#define ValidateSimpleObject_(obj)		ThrowIf_(obj == nil)
	#define ValidateThis_()					((void)0)

#endif

	// A synonym for ValidateObject_
#define ValidateObj_(obj)				ValidateObject_(obj)


// ---------------------------------------------------------------------------
// Useful assertions

#if PP_Debug

		// Performs an assertion to ensure the given Handle is locked
	#define AssertHandleLocked_(h)															\
				do {																		\
					ThrowIf_((h == nil) || ((*h) == nil));									\
					if ((::HGetState((Handle)(h)) & 0x80) == 0) {							\
						SignalStringLiteral_("Handle is unlocked, but should be locked");	\
					}																		\
				} while (false)

		// Performs an assertion to ensure the given Handle is unlocked
	#define AssertHandleUnlocked_(h)														\
				do {																		\
					ThrowIf_((h == nil) || ((*h) == nil));									\
					if ((::HGetState((Handle)(h)) & 0x80) != 0) {							\
						SignalStringLiteral_("Handle is locked, but should be unlocked");	\
					}																		\
				} while (false)

		// Performs an assertion to ensure the given Handle is a Resource Handle
	#define AssertHandleResource_(h)												\
				do {																\
					SInt16 resAttrs = ::GetResAttrs(h);								\
					if (::ResError() != noErr) {									\
						SignalStringLiteral_("Handle is not a Resource Handle");	\
					}																\
				} while (false)

		// Performs an assertion to ensure the given Handle is a Memory Handle
	#define AssertHandleMemory_(h)													\
				do {																\
					SInt16 resAttrs = ::GetResAttrs(h);								\
					if (::ResError() == noErr) {									\
						SignalStringLiteral_("Handle is not a Memory Handle");		\
					}																\
				} while (false)


		// Perform an assertion on the given test. Similar to Assert_
		// but does not preprocess away if PP_Debug is false; instead
		// no test is performed. (The strange syntax of the reported
		// string is to simulate the same report given by Assert_).
	#define Verify_(test)														\
				do {															\
					if (!(test)) SignalStringLiteral_("!("#test")");			\
				} while (false)

#else

	#define AssertHandleLocked_(h)
	#define AssertHandleUnlocked_(h)
	#define AssertHandleResource_(h)
	#define AssertHandleMemory_(h)

	#define Verify_(test)						((void)(test))

#endif


// ---------------------------------------------------------------------------
//	C++ object/pointer disposal macros

#if (PP_Debug) && (PP_DebugNew_Support) && (DEBUG_NEW >=1)


		// Deletes the given object. Before deletion validates the pointer,
		// performs a few assertions. After deleting, sets the pointer
		// variable to nil. (the F1 and F3 values are the ZAP_UNINITIALIZED
		// and ZAP_RELEASED values from DebugNew. Unfortunately they are
		// defined in DebugNew.cp so those symbols cannot directly be
		// used here.
	#define DisposeOf_(obj)													\
		do {																\
			if ((obj)) {													\
				ValidateObject_((obj));										\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF1F1F1F1UL));	\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF3F3F3F3UL));	\
				delete (obj);												\
				(obj) = nil;												\
			}																\
		} while (false)

		// A variant of DisposeOf_ for use on "simple" objects (objects
		// with no virtual methods).
	#define DisposeOfSimple_(obj)											\
		do {																\
			if ((obj)) {													\
				ValidateSimpleObject_((obj));								\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF1F1F1F1UL));	\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF3F3F3F3UL));	\
				delete (obj);												\
				(obj) = nil;												\
			}																\
		} while (false)


		// Same as DisposeOf_, but uses delete[]
	#define DisposeOfArray_(obj)											\
		do {																\
			if ((obj)) {													\
				ValidateObject_((obj));										\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF1F1F1F1UL));	\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF3F3F3F3UL));	\
				delete[] (obj);												\
				(obj) = nil;												\
			}																\
		} while (false)

		// Same as DisposeOfSimple_, but uses delete[]
	#define DisposeOfSimpleArray_(obj)										\
		do {																\
			if ((obj)) {													\
				ValidateSimpleObject_((obj));								\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF1F1F1F1UL));	\
				Assert_((reinterpret_cast<UInt32>(obj) != 0xF3F3F3F3UL));	\
				delete[] (obj);												\
				(obj) = nil;												\
			}																\
		} while (false)

#else

		// In non-Debug mode, just delete the pointer and set the
		// variable to nil
	#define DisposeOf_(obj)													\
		do {																\
			delete (obj);													\
			(obj) = nil;													\
		} while (false)

	#define DisposeOfSimple_(obj)		DisposeOf_(obj)

	#define DisposeOfArray_(obj)											\
		do {																\
			delete[] (obj);													\
			(obj) = nil;													\
		} while (false)

	#define DisposeOfSimpleArray_(obj)	DisposeOfArray_(obj)

#endif

	// Synonyms for DisposeOf_ macros
#define Forget_(obj)				DisposeOf_(obj)
#define ForgetSimple_(obj)			DisposeOfSimple_(obj)
#define ForgetArray_(obj)			DisposeOfArray_(obj)
#define ForgetSimpleArray_(obj)		DisposeOfSimpleArray_(obj)


// ---------------------------------------------------------------------------
//	Suppression macros
// ---------------------------------------------------------------------------
//	PowerPlant's core exception mechanisms (UDebugging, UException) are
//	very useful tools for development and debugging -- the extra information
//	generated by UDebugging during a throw is quite helpful at pinpointing
//	the location (file, line, error) of a particular exception.
//
//	The one shortcoming of the mechanism is the means of reporting the
//	exception is globally controlled (by the Debug_Throw/Debug_Signal
//	macros at compile time, or the gDebugThrow/gDebugSignal globals at
//	runtime). There are certainly times that one might wish to have an
//	exception thrown or signal raised but not be notified of the action
//	(i.e. you don't want to see the alert or break into a debugger).
//
//	As a workaround for this, one can use the StValueChanger template class
//	to temporarily change the debugAction. Typically you would use it like so:
//
//		try {
//		#ifdef Debug_Throw
//			PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>
//					suppressThrow(PP_PowerPlant::UDebugging::gDebugThrow,
//									PP_PowerPlant::debugAction_Nothing);
//		#endif
//			DoSomethingThatCouldThrowAnException();
//		} catch (...) {
//			// Handle the exception
//		}
//
//	Explanation: StValueChanger changes the value of the specified variable
//	to the given value for the scope of the StValueChanger object, saving the
//	variable's original value in its constructor and restoring the original
//	value in its destructor. The expression is wrapped by a check for
//	Debug_Throw as this action is only desired in the debug build (as the
//	debugAction should be suppressed in the release build anyways) -- might
//	as well remove the overhead of the object that does nothing useful.
//
//	Since all of the above a wee bit cumbersome to type, enter these macros. :-)
//
//	These macros are designed to temporarily modify the throw/signal debugAction.
//	"Disable" macros will completely disable the debugAction, "Change" macros
//	will change the action to whatever you desire (debugActions are listed
//	in UDebugging.h). Using these macros, the above code would now be:
//
//		try {
//			StDisableDebugThrow_();
//			DoSomethingThatCouldThrowAnException();
//		} catch (...) {
//			// Handle the exception
//		}

#ifdef Debug_Throw

	template <class T> class StValueChanger;	// Forward declaration

	#ifndef StDisableDebugThrow_				// May already be #defined in UDebugging.h
		#define StDisableDebugThrow_()											\
				PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
					__disableThrow(PP_PowerPlant::UDebugging::gDebugThrow,		\
									PP_PowerPlant::debugAction_Nothing)
	#endif // StDisableDebugThrow_

		#define StChangeDebugThrow_(debugAction)								\
				PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
					__changeThrow(PP_PowerPlant::UDebugging::gDebugThrow,		\
									debugAction)
#else

	#define	StDisableDebugThrow_()
	#define StChangeDebugThrow_(debugAction)

#endif


#ifdef Debug_Signal

	template <class T> class StValueChanger;	// Forward declaration

	#ifndef StDisableDebugSignal_				// May already be #defined in UDebugging.h
		#define StDisableDebugSignal_()											\
				PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
					__disableSignal(PP_PowerPlant::UDebugging::gDebugSignal,	\
									 PP_PowerPlant::debugAction_Nothing)
	#endif // StDisableDebugSignal_

		#define StChangeDebugSignal_(debugAction)								\
				PP_PowerPlant::StValueChanger<PP_PowerPlant::EDebugAction>		\
					__changeSignal(PP_PowerPlant::UDebugging::gDebugSignal,		\
									debugAction)
#else

	#define	StDisableDebugSignal_()
	#define StChangeDebugSignal_(debugAction)

#endif


// ---------------------------------------------------------------------------
// 	Other Handy Macros
// ---------------------------------------------------------------------------
//	These are some handy macros that can simply some common actions
//	that you take in PowerPlant: finding a Pane by its ID, and performing
//	dynamic_casts. These macros are designed to address the more common
//	situations in which these behaviors are used -- be aware of how the
//	macros expand and the checks they perform (especially how failures are
//	handled).
//
//	These macros are available if PP_Debug is on or off and their fundamental
//	behaviors do not change between debug and release vesions. However their
//	behaviors will change slightly due to Debug_Throw and Debug_Signal.


// ---------------------------------------------------------------------------
// A replacement for the commonly used FindPaneByID. Used like this:
//		LFooPane* theFooPane = FindPaneByID_(theWindow, kFooPaneID, LFooPane);

	#define FindPaneByID_( ContainerView, PaneID, PaneClassType )						\
				PP_PowerPlant::_TFindPaneByID<PaneClassType>(ContainerView,	PaneID,		\
															__FILE__, __LINE__ )

		// Instead of throwing on failures, this variant on FindPaneByID_
		// raises signals and returns a nil pointer.

	#define FindPaneByIDNoThrow_( ContainerView, PaneID, PaneClassType )					\
				PP_PowerPlant::_TFindPaneByIDNoThrow<PaneClassType>(ContainerView, PaneID,	\
																	__FILE__, __LINE__ )

class LPane;	// Forward declarations

		// This is the template function used by FindPaneByID_ to do
		// it's magic. You shouldn't call this function directly -- use
		// the macro instead.

	template<class PaneClassType>
	inline PaneClassType*
	_TFindPaneByID(
		LView* 			inView,
		PaneIDT			inPaneID,
		const char*		file,
		int				line)
	{
			// When PowerPlant has a more descriptive exception
			// class/mechanism, the exception thrown will probably
			// change to something more descriptive of the exact
			// error (instead of a generic ThrowIfNil_)

			// Ensure we have a container view
		Assert_(inView);

			// See if the pane first exists
		LPane*	paneAsLPane = inView->FindPaneByID(inPaneID);
		ThrowWithInfoIfNil_(paneAsLPane, file, line);

			// Now try to convert the pane to the requested type
		PaneClassType* result = dynamic_cast<PaneClassType*>(paneAsLPane);
		ThrowWithInfoIfNil_(result, file, line);
		
		ValidateObject_(result);

		return result;
	}

		// This is the template function used by FindPaneByIDNoThrow_ to do
		// it's magic. You shouldn't call this function directly -- use
		// the macro instead.

	template<class PaneClassType>
	inline PaneClassType*
	_TFindPaneByIDNoThrow(
		LView* 			inView,
		PaneIDT			inPaneID,
		const char*		file,
		int				line)
	{
			// Ensure we have a container view
		Assert_(inView);

		PaneClassType* result = nil;

			// See if the pane first exists
		LPane*	paneAsLPane = inView->FindPaneByID(inPaneID);
		if (paneAsLPane == nil) {
			SignalStringInfo_("FindPaneByIDNoThrow_: FindPaneByID failed", file, line);
			return result;
		}

			// Now try to convert the pane to the requested type
		result = dynamic_cast<PaneClassType*>(paneAsLPane);
		if (result == nil) {
			SignalStringInfo_("FindPaneByIDNoThrow_: dynamic_cast failed", file, line);
		}
		
		try {
			ValidateObject_(result);
		} catch (...) {
			SignalStringInfo_("FindPaneByIDNoThrow_: Validate threw... that's odd", file, line);
		}

		return result;
	}


// ---------------------------------------------------------------------------
// A replacement for dynamic_cast. This only does dynamic_cast of pointers
// not of references. Use:	LBar* theBar = DebugCast_(theFooPtr, LFoo, LBar);

	#define	DebugCast_(ptr, BaseType, ResultType)					\
				PP_PowerPlant::_TDebugCast<ResultType, BaseType>	\
								((BaseType*)ptr, __FILE__, __LINE__)

			// A variant of DebugCast_ that does not throw on failure but
			// instead signals and returns a nil pointer.
	#define	DebugCastNoThrow_(ptr, BaseType, ResultType)					\
				PP_PowerPlant::_TDebugCastNoThrow<ResultType, BaseType>		\
								((BaseType*)ptr, __FILE__, __LINE__)


		// This is the template function used by DebugCast_ to do
		// it's magic. You shouldn't need to call this direct -- use
		// the macro instead.
	template <class ResultType, class BaseType>
	inline ResultType*
	_TDebugCast(
		BaseType*		inPtr,
		const char*		file,
		int				line)
	{
			// Perform the cast
		ResultType*	result = dynamic_cast<ResultType*>(inPtr);
		ThrowWithInfoIfNil_(result, file, line);

		ValidateObject_(result);
		
		return result;
	}


		// This is the template function used by DebugCastNoThrow_ to do
		// it's magic. You shouldn't need to call this direct -- use
		// the macro instead.
	template <class ResultType, class BaseType>
	inline ResultType*
	_TDebugCastNoThrow(
		BaseType*		inPtr,
		const char*		file,
		int				line)
	{
			// Perform the cast
		ResultType* result = dynamic_cast<ResultType*>(inPtr);
		if (result == nil) {
			SignalStringInfo_("DebugCastNoThrow_: dynamic_cast failed", file, line);
		}

		try {
			ValidateObject_(result);
		} catch (...) {
			SignalStringInfo_("DebugCastNoThrow_: Validate threw... that's odd", file, line);
		}

		return result;
	}

// ---------------------------------------------------------------------------


PP_End_Namespace_PowerPlant

 #if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif	// _H_PP_DebugMacros
