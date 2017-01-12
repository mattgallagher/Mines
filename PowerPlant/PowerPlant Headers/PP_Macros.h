// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Macros.h					PowerPlant 2.2.5	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Macro definitions for controlling conditional compilation options
//
//	The basic format of most of the options is:
//
//		#ifndef Option_Name
//			#define Option_Name		Default_Value
//		#endif
//
//		#if Option_Name
//			// Define symbols for the option being ON
//		#else
//			// Define symbols for the option being OFF
//		#endif
//
//	If you wish to set the option yourself, you should place the appropriate
//	#define of the Option_Name in a header file that gets #include'd before
//	this one, normally your project prefix file or precompiled header.

#ifndef _H_PP_Macros
#define _H_PP_Macros
#pragma once

#include <PP_Requirements.h>


// ---------------------------------------------------------------------------
//	PowerPlant version number

#define		__PowerPlant__	0x02258000	// Version 2.2.5


// ---------------------------------------------------------------------------
//	Target OS

#ifndef PP_Target_Carbon				// Default is Classic
	#define PP_Target_Classic		1
	#define PP_Target_Carbon		0
#endif

#ifndef PP_Target_Classic
	#define PP_Target_Classic		(!PP_Target_Carbon)
#endif

#ifndef __dest_os						// For MSL

	#ifndef __MACH__
		#define __MACH__	0
	#endif

	#if __MACH__
		#include <ansi_prefix.mach.h>
	#else
		#define __dest_os  __mac_os
	#endif
#endif


// ---------------------------------------------------------------------------
//	PowerPlant Namespace

#ifndef PP_Uses_PowerPlant_Namespace
	#define PP_Uses_PowerPlant_Namespace	0		// Default to OFF
#endif

#if PP_Uses_PowerPlant_Namespace
	#define PP_Begin_Namespace_PowerPlant	namespace PowerPlant {
	#define PP_End_Namespace_PowerPlant		}
	#define PP_Using_Namespace_PowerPlant	using namespace PowerPlant;
	#define PP_PowerPlant					PowerPlant
#else
	#define PP_Begin_Namespace_PowerPlant
	#define PP_End_Namespace_PowerPlant
	#define PP_Using_Namespace_PowerPlant
	#define PP_PowerPlant
#endif


// ---------------------------------------------------------------------------
//	std Namespace

	// Macro for controlling use of "std" namespace for the C++
	// standard libraries. Within CodeWarrior, this setting should
	// be ON if _MSL_USING_NAMESPACE is #define'd.
	//
	// Set to OFF only if you have explicitly disabled namespace support
	// in the MSL or are using another implementation of the C++
	// standard libraries that does not support namespaces.

#ifndef PP_Uses_Std_Namespace
	#define	PP_Uses_Std_Namespace			1		// Default to ON
#endif

#if PP_Uses_Std_Namespace
	#define PP_Using_Namespace_Std			using namespace std;
	#define PP_STD							_STD
	#define PP_CSTD							_CSTD
#else
	#define PP_Using_Namespace_Std
	#define PP_STD
	#define PP_CSTD
#endif


// ---------------------------------------------------------------------------
//	Compiler Support for String Literals

	// Mac OS uses Pascal-style strings (length byte followed by characters)
	// for most Toolbox calls. Prefixing string literals with \p (which
	// the compiler replaces with a length byte) is a non-standard extension
	// to C supported by Mac OS compilers.
	//
	// If you use literal strings in your code and want to build with
	// multiple compilers that may or may not support Pascal strings, you
	// must be able to handle both C and Pascal strings. One way to do
	// this is to only use string literals as arguments to functions,
	// and to overload such functions so that there is a version that
	// accepts C strings (char *) and one that accepts Pascal strings
	// (unsigned char *).

#ifndef PP_Supports_Pascal_Strings
	#define PP_Supports_Pascal_Strings		1		// Default is true
#endif

#if PP_Supports_Pascal_Strings
	#define StringLiteral_(str)		("\p" str)		// Pascal string
#else
	#define StringLiteral_(str)		(str)			// C string
#endif


// ---------------------------------------------------------------------------
//	Compiler Support for Function Name Identifier

	// The C9X specification for the C Language defines __func__ as a
	// predefined identifier that names the current function. This feature
	// is supported by Metrowerks C/C++ compiler verison 2.3 and later
	// (CW Pro 5.3 and later).
	//
	// PowerPlant uses __func__ to display the function name in debugging
	// alerts.
	//
	// If you use a compiler that doesn't support this feature, you need
	// set this flag to 0. GCC has an extension which implements this
	// feature as __FUNCTION__. So for GCC, you can redefine the symbol:
	//
	//		#define __func__	__FUNCTION__	// For GCC
	//
	// This redefinition will also work for the Metrowerks compiler, which
	// also support __FUNCTION__.
	
#ifndef PP_Supports_Function_Identifier
	#define PP_Supports_Function_Identifier		1	// Default is true
#endif


// ---------------------------------------------------------------------------
//	Option for separate Standard and Aqua MenuBars

	// Mac OS X automatically adds an Application menu which contains
	// items such as "Preferences" and "Quit". On earlier systems,
	// programs put these items in their File and/or Edit menus.
	//
	// If you want the same program to run on multiple systems, you
	// may wish to use separate MBAR and MENU resources to avoid
	// duplicate items. In such cases, #define PP_Uses_Aqua_MenuBar
	// to 1 (true) in your prefix file. Then use MBAR_Standard (128)
	// for Mac OS 9 and earlioer menus, and MBAR_Aqua (129) for
	// Mac OS X menus.

#ifndef PP_Uses_Aqua_MenuBar
	#define PP_Uses_Aqua_MenuBar		0			// Default is false
#endif


// ---------------------------------------------------------------------------
//	Option for using Core Graphics contexts in windows

	// On Mac OS X, you can create a CGContextRef for a Window port.
	// This lets you draw using Core Graphics (Quartz). If you define
	// this flag to 1 (true), the LWindow override of GetCGContext()
	// will return a CGContextRef on Mac OS X, and the LWindow destructor
	// will release the CGContextRef.
	//
	// Since this is a Mac OS X feature, it applies for Carbon targets
	// only. For CFM runtimes, the routine to release a CGContextRef
	// must be loaded from a framework at runtime. This requires extra
	// code and files to be included in the project, so you won't want
	// to on the option unless you use Core Graphics. For MachO runtimes,
	// the Core Graphics functions are regular system calls, so we turn
	// on the option by default.

#ifndef PP_Uses_Window_CGContext
	#define	PP_Uses_Window_CGContext	TARGET_RT_MAC_MACHO
#endif


// ---------------------------------------------------------------------------
//	Option for using Carbon Events

	// CarbonLib 1.1 and later, as well as Mac OS X, support Carbon Events.
	// Presently, this flag controls whether LWindow uses Carbon Events
	// for certain actions. This support is still "in progress", so use
	// with care.

#ifndef PP_Uses_Carbon_Events
	#define	PP_Uses_Carbon_Events		0
#endif


// ---------------------------------------------------------------------------
//	Option for using Contextual Menus

	// This flag controls whether modern support for contextual menus is
	// included. It controls whether code is added to Carbon Event
	// handlers and LPane. This feature may only be used when Carbon
	// Event support is enabled.

#ifndef PP_Uses_ContextMenus
	#define PP_Uses_ContextMenus		0
#endif

#if !PP_Uses_Carbon_Events
	#undef PP_Uses_ContextMenus
	#define PP_Uses_ContextMenus		0
#endif


// ---------------------------------------------------------------------------
//	Option for implementing Periodicals using event loop Timers

	// CarbonLib 1.1 and later, as well as Mac OS X, support event loop
	// timers. Timers fire at specified intervals, even during mouse down
	// tracking. They work particularly well with Carbon Events.
	//
	// With this option on, LPeriodical will use a Timer if you call
	// StartIdling() or StartRepeating() with an interval parameter
	// (a double value representing the seconds between Timer firings).
	//
	// NOTE: You should not turn on this option unless your program uses
	// Carbon Events for window drawing or requires Mac OS X. With
	// CarbonLib on Mac OS 8 or 9, window contents won't redraw properly
	// during tracking of pull down menus which cover Panes that draw
	// from a Timer. The menu manager's cache of the bits behind the
	// pull down menu are invalidated by the drawing and not restored.

#ifndef PP_Uses_Periodical_Timers
	#define PP_Uses_Periodical_Timers	0
#endif


// ---------------------------------------------------------------------------
//	Option for implementation of UCursor

	//	1) Standard - Uses MacSetCursor(), which is available on all Systems.
	//					Must use this option for 68K targets.
	//
	//	2) Theme - Uses SetThemeCursor, which is available with
	//				Appearance Manager 1.1 or later
	//
	//	3) Conditional - Checks at runtime. Uses Theme version if AM 1.1
	//						is available, otherise Standard

#if !defined(PP_Option_Cursor_Standard) &&	\
	!defined(PP_Option_Cursor_Theme) &&		\
	!defined(PP_Option_Cursor_Conditional)		// Undefined. Use defaults.
	
	#if TARGET_API_MAC_OSX
		#define	PP_Option_Cursor_Standard		0
		#define PP_Option_Cursor_Theme			1	// OS X has Themes
		#define PP_Option_Cursor_Conditional	0
		
	#else
		#define	PP_Option_Cursor_Standard		1	// Works on all Systems
		#define PP_Option_Cursor_Theme			0
		#define PP_Option_Cursor_Conditional	0
	#endif
#endif
	

// ---------------------------------------------------------------------------
//	Option for implementation of standard dialogs

	// Navigation Services provides standard dialogs for confirming
	// document saves and for choosing and specifying files. However,
	// Nav Services is not present in all Systems. It is available as
	// a separate SDK for System 8.1 and ships with System 8.5 and later.
	// For compatibility, PP offers three implementations of the dialogs.
	//
	//		1) Classic Only - Uses Alert and Standard File, which is
	//							available in System 7 or later
	//
	//		2) Nav Services Only - You are responsible for checking for
	//								Nav Services before using the dialogs.
	//								You may want to check at launch and
	//								refuse to run or disable features.
	//
	//		3) Conditional - Checks at runtime. Uses Nav Services if
	//							available, otherwise Classic
	//
	//	Note: For Carbon targets, you must use Nav Services Only.

#define	PP_StdDialogs_ClassicOnly			1		// Always use Classic
#define	PP_StdDialogs_NavServicesOnly		2		// Always use Nav Services
#define PP_StdDialogs_Conditional			3		// Check at runtime

#ifndef	PP_StdDialogs_Option

	#if PP_Target_Carbon			// Under Carbon, NavServices is
									//   the only option
		#define PP_StdDialogs_Option	PP_StdDialogs_NavServicesOnly

	#else							// Default to Classic otherwise
		#define PP_StdDialogs_Option	PP_StdDialogs_ClassicOnly
	#endif

#endif

#if PP_Target_Carbon && (PP_StdDialogs_Option != PP_StdDialogs_NavServicesOnly)
	#error Must use NavServices for Carbon targets
#endif


// ---------------------------------------------------------------------------
//	Option for including PP implementation of RepositionWindow()

		// Although CarbonLib implements most of the important functions of
		// Window Manager 2.0, it does not implement RepositionWindow().
		// Window Mgr 2.0 has support for floating windows and it was
		// introduced with Mac OS 8.5.
		//
		// This presents a problem for systems using CarbonLib 1.0.x on
		// Mac OS 8.1. PP's floating window code isn't Carbonized (it
		// uses too many non-Carbon routines). So, to support floating
		// windows on this configuration, PP has its own implementation
		// of RepositionWindow() in UWindows, which is called by
		// UWMgr20-Desktop.cp when the Toolbox implementation isn't present.
		//
		// UWindows::RepositionWindow() brings in a lot of code, all of
		// which is unnecessary on Mac OS 8.5 or later. So, this flag
		// let's you turn on or off this code.
		//
		// Define the flag to 1 if all the following conditions are true:
		//		- Target is Carbon 1.0.x
		//		- Program needs to support Mac OS 8.1
		//		- Program uses UWMgr20Desktop.cp instead of UDesktop.cp
		//			in order to support floating windows
		
#ifndef PP_Implement_RepositionWindow			// Default to ON for Carbon
												//   and OFF for Classic
	#define PP_Implement_RepositionWindow	PP_Target_Carbon
#endif


// ---------------------------------------------------------------------------
//	Option for defining PP integer types

		// PP_Types.h has typedef's for signed and unsigned types
		// of char, short, and long. Apple's MacTypes.h also has
		// typedef's for those items but with slightly different names.
		//
		// You should use the Apple typedef's. The PP typedef's are
		// deprecated.

#ifndef PP_Uses_Old_Integer_Types
	#define	PP_Uses_Old_Integer_Types		0		// Default to OFF
#endif


// ---------------------------------------------------------------------------
//	Option for throwing exceptions

		// PP now throws LException objects. To switch back to the
		// old behavior of throwing ExceptionCodes (long integers),
		// set the symbol to 1 (true).

#ifndef PP_Obsolete_ThrowExceptionCode
	#define PP_Obsolete_ThrowExceptionCode	0
#endif


// ---------------------------------------------------------------------------
//	Option for warning if using obsolete classes

		// Whether or not to issue a compile-time warning if a project
		// uses a class that will be obsolete.

#ifndef PP_Warn_Obsolete_Classes
	#define PP_Warn_Obsolete_Classes		1		// Default is ON
#endif


// ---------------------------------------------------------------------------
//	Option for explaining name changes

		// Symbol names can change due to bug fixes, enhancements, or
		// changes in Apple's Universal Interfaces. You will need to change
		// existing code that uses the old names.
		//
		// With this option on, PP will #define the old name to a message
		// that explains what to do. The explanatory message will be
		// invalid code, so the message will appear as a compilation error.
		//
		// Once you have updated your code, you may want to turn off this
		// option to remove unnecessary #define's.

#ifndef PP_Explain_Name_Changes
	#define PP_Explain_Name_Changes			1		// Default is ON
#endif


// ---------------------------------------------------------------------------
//	Import option for CFM68K

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
	#define PP_Uses_Pragma_Import	1
#else
	#define PP_Uses_Pragma_Import	0
#endif


// ---------------------------------------------------------------------------
//	Option for warning if duplicate class IDs are registered

		// To create objects from data (usually PPob resources), you need
		// to register a four-character class ID with URegistrar.
		// URegistrar::RegisterClass() overwrites the existing entry if
		// you register that same class ID twice. This allows you to
		// dynamically change registrations at runtime.
		//
		// However, duplicate class IDs might a mistake, where you
		// inadvertantly used the same ID for different classes.
		// This option lets you specify whether to Signal at runtime
		// for duplicate class IDs.

#ifndef PP_Warn_Duplicate_Class_ID
	#define	PP_Warn_Duplicate_Class_ID	0	// Default is OFF
#endif

		
// ---------------------------------------------------------------------------
//	Preprocessor symbols for Apple Universal Headers options

#undef SystemSevenOrLater				// PowerPlant requires System 7
#define SystemSevenOrLater	1

#undef OLDROUTINENAMES					// PP uses only new names
#define OLDROUTINENAMES		0

#undef OLDROUTINELOCATIONS				// PP uses only new header locations
#define OLDROUTINELOCATIONS	0


#endif
