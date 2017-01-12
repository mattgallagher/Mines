// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMCheckBoxGroupBoxImp.h	PowerPlant 2.2.2	©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMCheckBoxGroupBoxImp
#define _H_LAMCheckBoxGroupBoxImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMCheckBoxGroupBoxImp : public LAMControlImp {
public:
						LAMCheckBoxGroupBoxImp( LStream* inStream = nil );

	virtual				~LAMCheckBoxGroupBoxImp();

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);
								
	virtual void		FinishCreateSelf();
								
	virtual void		DrawSelf();
	
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;
	
protected:
	
	virtual void		AdjustControlBounds( Rect& ioBounds );
	
protected:
	bool			mIsPrimary;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
