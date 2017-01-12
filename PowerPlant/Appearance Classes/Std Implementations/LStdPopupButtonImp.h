// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStdPopupButtonImp.h		PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStdPopupButtonImp
#define _H_LStdPopupButtonImp
#pragma once

#include <LStdControlImp.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStdPopupButtonImp : public LStdControlImp {
public:
						LStdPopupButtonImp( LStream* inStream = nil );

	virtual				~LStdPopupButtonImp();

	virtual void		Init(	LControlPane*	inControlPane,
								LStream*		inStream);

	virtual void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inMENUResType = 0);

	virtual SInt16		FindHotSpot( Point inPoint ) const;

	virtual Boolean		TrackHotSpot(
								SInt16			inHotSpot,
								Point			inPoint,
								SInt16			inModifiers);

	virtual void		DrawSelf();

	virtual void		SetValue( SInt32 inValue );

protected:
	MenuHandle			GetMacMenuH() const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
