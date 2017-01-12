// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_MacHeadersMach-O.c		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A variation of the standard MacHeaders.c designed for use with
//	PowerPlant programs.

/*
 *	Metrowerks-specific definitions
 *
 *	These definitions are commonly used but not in Apple's headers. We define
 *	them in our precompiled header so we can use the Apple headers without
 *  modification.
 */

#ifndef __NOEXTENSIONS__
	#define __NOEXTENSIONS__
#endif
#ifndef __CF_USE_FRAMEWORK_INCLUDES__
	#define __CF_USE_FRAMEWORK_INCLUDES__
#endif

#include <Carbon/Carbon.h>


#if 0 /* see if we can get by without these for Mach-O */

	#ifndef PtoCstr
		#define PtoCstr		p2cstr
	#endif

	#ifndef CtoPstr
		#define CtoPstr		c2pstr
	#endif

	#ifndef PtoCString
		#define PtoCString	p2cstr
	#endif

	#ifndef CtoPString
		#define CtoPString	c2pstr
	#endif

	#ifndef topLeft
		#define topLeft(r)	(((Point *) &(r))[0])
	#endif

	#ifndef botRight
		#define botRight(r)	(((Point *) &(r))[1])
	#endif

	#ifndef TRUE
		#define TRUE		true
	#endif
	#ifndef FALSE
		#define FALSE		false
	#endif

#endif /* see if we can get by without these for Mach-O */

