// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UStandardFile.h				PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UStandardFile
#define _H_UStandardFile
#pragma once

#include <PP_Prefix.h>
#include <PP_Resources.h>

#include <StandardFile.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	UStandardFile {
public:
	static Boolean		GetDirectory(
								StandardFileReply&	outReply,
								ResIDT				inDLOGid = DLOG_GetDirectory);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
