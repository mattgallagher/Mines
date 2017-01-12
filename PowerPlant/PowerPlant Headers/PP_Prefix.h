// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_Prefix.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	To insure that PowerPlant and compiler options are set properly,
//	the first #include for every file should be <PP_Prefix.h>, or some
//	header that indirectly #include's this file.
//
//	Via nested #include's, every PP class file eventually #include's
//	<PP_Prefix.h>. So #include'ing any PP header as your first #include
//	file or using one of the PP Precompiled header files is OK.

#ifndef _H_PP_Prefix
#define _H_PP_Prefix
#pragma once

	// Header files required for almost all PP files

#include <PP_Macros.h>
#include <PP_Types.h>
#include <PP_Constants.h>
#include <UException.h>

#endif
