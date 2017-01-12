// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Types.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_PP_Types
#define _H_PP_Types
#pragma once

#include <PP_Macros.h>
#include <MacTypes.h>				// Toolbox data types

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Enumeration for tri-state hierarchical properties

enum	ETriState {
	triState_Off,			// Setting is OFF
	triState_Latent,		// Setting is ON, but Super's setting is OFF
	triState_On				// Setting is ON, and Super's setting is ON
};


// ---------------------------------------------------------------------------
//	Types for PowerPlant Identifiers

typedef		SInt32			CommandT;
typedef		SInt32			MessageT;

typedef		SInt16			ResIDT;
typedef		SInt32			PaneIDT;
typedef		FourCharCode	ClassIDT;
typedef		FourCharCode	DataIDT;
typedef		FourCharCode	ObjectIDT;


// ---------------------------------------------------------------------------
//	Integer types

	//	These types are obsolete. Use the equivalent Toolbox types instead.


#if PP_Uses_Old_Integer_Types						// Toolbox Type

	typedef		signed char				Int8;		// SInt8
	typedef		signed short			Int16;		// SInt16
	typedef		signed long				Int32;		// SInt32

	typedef		unsigned char			Uint8;		// UInt8
	typedef		unsigned short			Uint16;		// UInt16
	typedef		unsigned long			Uint32;		// UInt32

	typedef		UInt16					Char16;		// UInt16
	typedef		unsigned char			Uchar;		// UInt8

#endif


PP_End_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	Macros for accessing the top-left and bottom-right corners of a Rect

	//	The original Toolbox specifies these accessors, but Apple no longer
	//	includes them in the Universal Headers. We define them because a lot
	//	of existing code uses them.

#ifndef topLeft
	#define topLeft(r)	(((Point *) &(r))[0])
#endif

#ifndef botRight
	#define botRight(r)	(((Point *) &(r))[1])
#endif


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
