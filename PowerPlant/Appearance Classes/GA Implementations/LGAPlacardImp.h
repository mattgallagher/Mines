// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAPlacardImp.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAPlacardImp
#define _H_LGAPlacardImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAPlacardImp : public LGAControlImp {
public:
						LGAPlacardImp( LControlPane* inControlPane );

						LGAPlacardImp( LStream* inStream );

	virtual				~LGAPlacardImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	virtual bool		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;

	virtual	void		DrawSelf();

};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
