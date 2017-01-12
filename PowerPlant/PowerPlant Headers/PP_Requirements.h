// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Requirements.h			PowerPlant 2.2.2	©2001-2005 Metrowerks Inc.
// ===========================================================================
//
//	PowerPlant supports several different versions of Mac system software.
//	In order to provide a consistent API and feature set, PP has custom
//	implementations of some system software features. After performing a
//	runtime check, the code uses the system implementation if present, and
//	the custom implementation otherwise.
//
//	If your program has system requirements beyond the minimum supported
//	by PowerPlant, you should set the appropriate preprocessor symbols
//	described in this header. Doing so will eliminate unnecessary runtime
//	checks and unused code.
//
//	For version numbers, we use the binary coded decimal represention that
//	the Toolbox uses for most Gestalt values. That is, version JJ.K.L is
//	specified as the hex number 0xJJKL. For example, version 7.5.1 is 0x0751.


#ifndef _H_PP_Requirements
#define _H_PP_Requirements
#pragma once

#include <ConditionalMacros.h>

// ---------------------------------------------------------------------------
//	System Software Requirements

#ifndef PP_Minimum_System_Version

	#if TARGET_API_MAC_OSX				// OS X = 10.0
		#define PP_Minimum_System_Version	0x1000

	#elif TARGET_API_MAC_CARBON			// Carbon runs on Mac OS 8.1 or later
		#define PP_Minimum_System_Version	0x0810

	#elif TARGET_CPU_PPC				// PowerPC began with Mac OS 7.1.2
		#define PP_Minimum_System_Version	0x0712
		
	#else								// PP supports System 7.0 or later
		#define PP_Minimum_System_Version	0x0700
	#endif

#endif


// ---------------------------------------------------------------------------
//	Carbon Requirements

#ifndef PP_Minimum_Carbon_Version

	#if TARGET_API_MAC_OSX				// OS X has Carbon 1.3
		#define PP_Minimum_Carbon_Version	0x0130
		
	#elif TARGET_API_MAC_CARBON			// PP supports Carbon 1.0
		#define PP_Minimum_Carbon_Version	0x0100

	#else								// Not a Carbon Target
		#define PP_Minimum_Carbon_Version	0x0000
	#endif
		
#endif


// ---------------------------------------------------------------------------
//	Appearance Manager Requirements

#ifndef PP_Minimum_Appearance_Version

	#if TARGET_API_MAC_OSX				// OS X has Appearance 2.0
		#define	PP_Minimum_Appearance_Version	0x0200
		
	#elif PP_Minimum_System_Version >= 0x0900
										// OS 9.0 has Appearance 1.1.1
		#define	PP_Minimum_Appearance_Version	0x0111
		
	#else 								// No Appearance Manager at all
		#define	PP_Minimum_Appearance_Version	0x0000
	#endif
	
#endif


#endif
