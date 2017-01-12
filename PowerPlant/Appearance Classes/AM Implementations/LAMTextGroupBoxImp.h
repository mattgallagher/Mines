// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LAMTextGroupBoxImp.h			PowerPlant 2.2.2©2000-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LAMTextGroupBoxImp
#define _H_LAMTextGroupBoxImp
#pragma once

#include <LAMControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LAMTextGroupBoxImp : public LAMControlImp {
public:
						LAMTextGroupBoxImp( LStream* inStream = nil );

	virtual				~LAMTextGroupBoxImp();

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);
								
	virtual void		DrawSelf();
	
	virtual void		SetDescriptor( ConstStringPtr inDescriptor );
	
protected:
	bool			mIsPrimary;
	bool			mTitleIsEmpty;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
