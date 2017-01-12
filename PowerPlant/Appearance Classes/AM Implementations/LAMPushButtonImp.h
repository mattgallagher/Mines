// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMPushButtonImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMPushButtonImp
#define _H_LAMPushButtonImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMPushButtonImp : public LAMControlImp {
public:
						LAMPushButtonImp( LStream* inStream = nil );

	virtual				~LAMPushButtonImp();

	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
