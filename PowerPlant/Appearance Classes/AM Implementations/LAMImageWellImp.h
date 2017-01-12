// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMImageWellImp.h			PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMImageWellImp
#define _H_LAMImageWellImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMImageWellImp : public LAMControlImp {
public:
						LAMImageWellImp( LStream* inStream = nil );

	virtual				~LAMImageWellImp();
	
	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
