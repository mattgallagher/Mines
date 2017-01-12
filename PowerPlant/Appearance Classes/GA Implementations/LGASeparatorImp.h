// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGASeparatorImp.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGASeparatorImp
#define _H_LGASeparatorImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGASeparatorImp : public LGAControlImp {
public:
						LGASeparatorImp(
								LControlPane*	inControlPane );

						LGASeparatorImp(
								LStream*		inStream );

	virtual				~LGASeparatorImp ();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	bool				IsHorizontal()	{ return mHorizontal; }

	virtual	void		DrawSelf();

protected:
	bool		mHorizontal;	// Horizontal or vertical?
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
