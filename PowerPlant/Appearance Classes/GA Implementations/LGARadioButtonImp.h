// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LGARadioButtonImp.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: Robin Mair

#ifndef _H_LGARadioButtonImp
#define _H_LGARadioButtonImp
#pragma once

#include <LGATitleMixin.h>
#include <LGAControlImp.h>
#include <LControlImp.h>
#include <LControlPane.h>
#include <LStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class LGARadioButtonImp :	public LGAControlImp,
							public LGATitleMixin {
public:
						LGARadioButtonImp(
								LControlPane*	inControlPane);

						LGARadioButtonImp(
								LStream*		inStream);

	virtual				~LGARadioButtonImp ();

	virtual	void		Init(	LControlPane*	inControlPane,
								LStream*		inStream );

	virtual	void		Init(	LControlPane*	inControlPane,
								SInt16			inControlKind,
								ConstStringPtr	inTitle = Str_Empty,
								ResIDT			inTextTraitsID = 0,
								SInt32			inRefCon = 0);


							// Getters

	bool				IsSelected () const
							{
								return mControlPane->GetValue() == Button_On;
							}

	virtual StringPtr	GetDescriptor (
								Str255	outDescriptor ) const;

	virtual	void		CalcTitleRect (
								Rect	&outRect );

	virtual	void		CalcLocalControlRect	(
								Rect	&outRect );

	bool				IsMixedState () const
							{
								return mControlPane->GetValue() == Button_Mixed;
							}

	virtual	ResIDT		GetTextTraitsID () const
							{
								return LGATitleMixin::GetTextTraitsID();
							}

							// Setters

	virtual void		SetDescriptor (
								ConstStringPtr	inDescriptor );

	virtual void		SetTextTraitsID (
								ResIDT	inTextTraitsID );

	virtual	void		PostSetValue ();


							// Drawing

	virtual	void		DrawSelf	();

	virtual	void		DrawRadioTitle	();

	virtual	void		DrawRadioNormal ();
	virtual	void		DrawRadioHilited ();
	virtual	void		DrawRadioDimmed ();

protected:

	virtual	void		DrawRadioChoice(
								SInt16		inChoice,
								const Rect&	inFrame);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
