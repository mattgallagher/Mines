// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PP_PCH_Options.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//
//	This file is #include'd by the standard PowerPlant prefix files to check
//	that the compiler options in the PowerPlant PreCompiled Header (PCH)
//	files are compatible with those of the current project.

// ---------------------------------------------------------------------------
//	PP PCH files have "Enable bool support" (C/C++ Language Project Settings)
//	turned ON. With this setting, "true" and "false" are keywords and are
//	not #define'd to 1 and 0 (in Types.h).
//
//	If the project has "Enable bool support" turned OFF, the symbols "true"
//	and "false" are undefined (they aren't keywords, but the PCH file has
//	not defined them because the PCH assumed they were keywords).
//
//	Since "bool" is now part of the C++ standard, you should turn ON this
//	option. If for some reason you don't wish to do this, you should make
//	your own precompiled header file with the "bool" option off.

#if !__option(bool)
	#ifdef __cplusplus
	#warning Settings mismatch: PCH has bool ON, Project has bool OFF
	#endif

		// Define "true" and "false" so code still compiles.

	#ifndef true
		#define true			1
	#endif
	#ifndef false
		#define false			0
	#endif
#endif
