// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UVolume.h			   		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub
//
//	UVolume contains some methods used by UProcess for volume searching.
//	Not intended to be glamorous, but could be extended in the future. Meantime,
//	check out MoreFiles from Jim Luther/Apple DTS. Very cool.

#ifndef _H_UVolume
#define _H_UVolume
#pragma once

#include <PP_Debug.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

namespace UVolume {
	bool		IsApplication(
					const FSSpec&		inFileSpec);

	SInt16		GetIndVolume(
					SInt16				inIndex);

	bool		IsSearchable(
					SInt16				inIndex);

	SInt16		GetNumVolumes();

	bool		VolHasDesktopDB(
					SInt16				inVRefNum);

	bool		FindAppOnVolume(
					OSType				inSignature,
					SInt16 				inVRefNum,
					FSSpec&				outFSSpec);

	bool		FindApp(
					OSType				inSignature,
					FSSpec&				outFSSpec);

#if PP_MoreFiles_Support

	bool		FindAppOnVolume(
					ConstStr255Param	inName,
					SInt16				inVRefNum,
					FSSpec&				outFSSpec,
					bool				inPartial = true);

	bool		FindApp(
					ConstStr255Param	inName,
					FSSpec&				outFSSpec,
					bool				inPartial = true);
#endif
}

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UVolume
