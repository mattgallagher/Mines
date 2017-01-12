// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGAWindowHeaderImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGAWindowHeaderImp
#define _H_LGAWindowHeaderImp
#pragma once

#include <LGAControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGAWindowHeaderImp : public LGAControlImp {
public:
						LGAWindowHeaderImp(
								LControlPane*	inControlPane);

						LGAWindowHeaderImp(
								LStream*		inStream);

	virtual				~LGAWindowHeaderImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);

	bool				HasBottomDivider() const
							{
								return mHasBottomDivider;
							}

	void				SetHasBottomDivider(
								bool			inHasBottomDivider,
								bool			inRedraw = true);

	virtual bool		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;

	virtual	void		DrawSelf();

protected:
	bool			mHasBottomDivider;		// Is there a black line drawn
											// along the bottom of the header

	virtual	void		DrawHeaderFace();
	virtual	void		DrawHeaderBorder();
	virtual	void		DrawHeaderShading();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
