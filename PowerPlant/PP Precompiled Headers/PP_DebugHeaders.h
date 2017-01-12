// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_DebugHeaders.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

	// Use PowerPlant-specific Precompiled header

#if __MACH__
	#include <PP_MSLDebugHeadersMach-O++>

#elif __POWERPC__
	#include <PP_DebugHeadersPPC++>

#elif __CFM68K__
	#include <PP_DebugHeadersCFM68K++>

#else
	#include <PP_DebugHeaders68K++>
#endif

#include <PP_PCH_Options.h>		// Check for compatibility with pch options
