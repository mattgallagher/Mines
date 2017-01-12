// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Debug.h					PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	This file is the "prefix" file for the Debugging Classes (akin to what
//	PP_Prefix.h/PP_Macros.h is for all of PowerPlant). It establishes various
//	settings for support from the classes.
//
//	Each of these people helped in some way to make the Debugging classes
//	possible, and I give them much thanx: Greg Dow, Eric Scouten, Jesse
//	Jones, Greg Bolsinga, Joshua Golub, Mike Lockwood, dEVoN Hubbard,
//	Steve Jasik, and Matt Henderson.
//	As well, thanx to my beta testers: Tom Van Lenten, Tim Paustian,
//	Steve Gilardi, Joe Chan, Rick Aurbach, meeroh Jurisic, Carl Constantine,
//	Mitch Jones, Steven Frank - Hsoi

#ifndef _H_PP_Debug
#define _H_PP_Debug

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
// ¥	Macros controlling the features of the debugging classes. You will
//		probably want to "override" these yourself in some place like a
//		customized precompiled header file and/or prefix file.
// ---------------------------------------------------------------------------

//	NB: The Debug_Throw and Debug_Signal macros (see UDebugging.h) are not
//	manipulated here. You must establish these accordingly yourself.

//	Enable PowerPlant's Debugging capabilities. Set to a non-zero value
//	to gain the benefits of the Debugging Classes. A value of zero should
//	disable Debugging supports (and is what you should use for your
//	final/release builds).

#ifndef PP_Debug
	#define PP_Debug							1		// ON by default.
#endif


// ---------------------------------------------------------------------------
//	3rd party (non-PowerPlant) supports

//	MoreFiles, from Jim Luther (Apple DTS). <http://members.aol.com/jumplong/>

#ifndef PP_MoreFiles_Support
	#define	PP_MoreFiles_Support				0		// OFF by default
#endif

//	Spotlight, from Onyx Technology. <http://www.onyx-tech.com/>

#ifndef PP_Spotlight_Support
	#define PP_Spotlight_Support				0		// OFF by default
#endif

//	QC, from Onyx Technology. <http://www.onyx-tech.com/>

#ifndef PP_QC_Support
	#define PP_QC_Support						0		// OFF by default
#endif

//	Metrowerks' DebugNew. Don't forget to set DEBUG_NEW to a
//	appropriate value as well (see DebugNew.h and the DebugNew
//	documentation for more information).

#ifndef PP_DebugNew_Support
	#define PP_DebugNew_Support					0		// OFF by default
#endif


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_PP_Debug
