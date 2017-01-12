// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMPopupGroupBoxImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMPopupGroupBoxImp
#define _H_LAMPopupGroupBoxImp
#pragma once

#include <LAMPopupButtonImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#ifndef PP_Uses_Carbon_Events
	#define	PP_Uses_Carbon_Events		0
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMPopupGroupBoxImp : public LAMPopupButtonImp {
public:
						LAMPopupGroupBoxImp( LStream* inStream = nil );

	virtual				~LAMPopupGroupBoxImp();

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual void		FinishCreateSelf();
	
	virtual void		DrawSelf();

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
#if not PP_Uses_Carbon_Events
	virtual void		PostSetValue();
#endif
	
	virtual OSStatus	GetStructureOffsets( Rect& outOffsets ) const;

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;

protected:
	virtual void		AdjustControlBounds( Rect& ioBounds );

#if not PP_Uses_Carbon_Events
	virtual MenuHandle	GetMacMenuH() const;
#endif
	
protected:
	bool			mIsPrimary;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
