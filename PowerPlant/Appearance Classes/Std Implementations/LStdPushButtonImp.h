// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdPushButtonImp.h			PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStdPushButtonImp
#define _H_LStdPushButtonImp
#pragma once

#include <LStdControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStdPushButtonImp : public LStdControlImp {
public:
						LStdPushButtonImp( LStream* inStream = nil );

	virtual				~LStdPushButtonImp();

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;

	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;
	
	virtual void		DrawSelf();
	
protected:
	bool			mIsDefault;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
