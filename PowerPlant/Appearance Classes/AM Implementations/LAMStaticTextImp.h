// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMStaticTextImp.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMStaticTextImp
#define _H_LAMStaticTextImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMStaticTextImp : public LAMControlImp {
public:
						LAMStaticTextImp( LStream* inStream = nil );
	
	virtual				~LAMStaticTextImp();
	
	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;
								
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
	#if PP_Uses_CFDescriptor
	
	virtual CFStringRef		CopyCFDescriptor() const;
	
	virtual void			SetCFDescriptor( CFStringRef inStringRef );
	
	#endif
	
	virtual bool		GetValue( SInt32& outValue ) const;
															
	virtual void		SetValue( SInt32 inValue );
	
	virtual void		DrawSelf();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
